

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Log.h"
#include "Opcodes.h"
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "World.h"
#include "Player.h"
#include "Util.h"
#include "Warden.h"
#include "AccountMgr.h"
#include "fmt/format.h"

#include <fstream>
#include <sstream>

Warden::Warden()
    : _session(nullptr)
    , _module(nullptr)
    , _clientResponseTimer(0)
    , _previousTimestamp(0)
    , _dataSent(false)
    , _initialized(false)
    , _checkTimer(10000)
    , _sendLuaChecks(false)
    , _normalCheckCounter(0)
{
    memset(_inputKey, 0, sizeof(_inputKey));
    memset(_outputKey, 0, sizeof(_outputKey));
    memset(_seed, 0, sizeof(_seed));

    _globalTablesAccessor = GenerateRandomIdentifier(6, S_WARDEN_CHARSET);
    _sendAddonMessageFunc = GenerateRandomIdentifier(4, S_WARDEN_CHARSET);
    _messageTargetName    = GenerateRandomIdentifier(WARDEN_MESSAGE_TARGET_SIZE, S_WARDEN_CHARSET);

    /// *** THIS IS IMPORTANT ***
    /// if ({} == nill)then {}=SendAddonMessage end
    /// This function has to be at the beginning and sent each cycle, whether you send all of the checks or selected ones
    /// Client reloads can clear our functions, hence the above
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_SEND_ADDON, _sendAddonMessageFunc));

    //! Now check global tables, if key is not nil then send value to the realm via addon channel
    //! Function will get triggered likewise, assuming our random message func is called AfGh then AfGh("AfGh",data,"WHISPER",ourTargetName)
    //! and realm will receive a prefixed message, in this case it will be: AfGh/t[OurData]
    //! make sure to send before any other checks that rely on it to trigger or extract data from
    //! some checks might trigger this one on their own to send data once triggered
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_GLOBAL_TABLES_CHECKER, _globalTablesAccessor, _sendAddonMessageFunc, _messageTargetName));

    //! Check if lua state is tainted
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_CHECK_LUA_STATE, _globalTablesAccessor));

    //! Check for PQR frame within the client
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_CHECK_PQR_FRAME, _globalTablesAccessor));

    //! Set up a trap within CastSpellByName function that sets GlobalTable value whenever that function is triggered
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_CHECK_CASTSPELLBYNAME, _globalTablesAccessor));

    //! Same as CastSpellByName
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_CHECK_CASTSPELLBYID, _globalTablesAccessor));

    // Same as CastSpellByName
    _preparedLuaScripts.emplace_back(fmt::format(S_WARDEN_CHECK_AUCTIONHOUSE_QUERY, _globalTablesAccessor));

    //! Force sending data to server if any
    _preparedLuaScripts.emplace_back(fmt::format("if chk{0}~=nil then chk{0}() end", _globalTablesAccessor));
}

Warden::~Warden()
{
    delete[] _module->CompressedData;
    delete _module;
    _module      = nullptr;
    _initialized = false;
}

void Warden::SendModuleToClient()
{
    sLog->outDebug(LOG_FILTER_WARDEN, "Send module to client");

    // Create packet structure
    WardenModuleTransfer packet;

    uint32 sizeLeft = _module->CompressedSize;
    uint32 pos      = 0;
    uint16 burstSize;
    while (sizeLeft > 0)
    {
        burstSize       = sizeLeft < 500 ? sizeLeft : 500;
        packet.Command  = WARDEN_SMSG_MODULE_CACHE;
        packet.DataSize = burstSize;
        memcpy(packet.Data, &_module->CompressedData[pos], burstSize);
        sizeLeft -= burstSize;
        pos += burstSize;

        EncryptData((uint8*)&packet, burstSize + 3);
        WorldPacket pkt1(SMSG_WARDEN_DATA, burstSize + 3);
        pkt1.append((uint8*)&packet, burstSize + 3);
        _session->SendPacket(&pkt1);
    }
}

void Warden::RequestModule()
{
    sLog->outDebug(LOG_FILTER_WARDEN, "Request module");

    // Create packet structure
    WardenModuleUse request;
    request.Command = WARDEN_SMSG_MODULE_USE;

    memcpy(request.ModuleId, _module->Id, 16);
    memcpy(request.ModuleKey, _module->Key, 16);
    request.Size = _module->CompressedSize;

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&request, sizeof(WardenModuleUse));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenModuleUse));
    pkt.append((uint8*)&request, sizeof(WardenModuleUse));
    _session->SendPacket(&pkt);
}

void Warden::Update()
{
    if (_initialized)
    {
        uint32 currentTimestamp = World::GetGameTimeMS();
        uint32 diff             = getMSTimeDiff(_previousTimestamp, currentTimestamp);
        _previousTimestamp      = currentTimestamp;

        if (_dataSent)
        {
            uint32 maxClientResponseDelay = sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_RESPONSE_DELAY);
            if (maxClientResponseDelay > 0)
            {
                if (_clientResponseTimer > maxClientResponseDelay * IN_MILLISECONDS)
                    _session->KickPlayer();
                else
                    _clientResponseTimer += diff;
            }
        }
        else
        {
            if (diff >= _checkTimer)
                RequestData();
            else
                _checkTimer -= diff;
        }
    }
}

void Warden::DecryptData(uint8* buffer, uint32 length)
{
    _inputCrypto.UpdateData(buffer, length);
}

void Warden::EncryptData(uint8* buffer, uint32 length)
{
    _outputCrypto.UpdateData(buffer, length);
}

bool Warden::IsValidCheckSum(uint32 checksum, const uint8* data, const uint16 length)
{
    uint32 newChecksum = BuildChecksum(data, length);

    if (checksum != newChecksum)
    {
        sLog->outDebug(LOG_FILTER_WARDEN, "CHECKSUM IS NOT VALID");
        return false;
    }
    else
    {
        sLog->outDebug(LOG_FILTER_WARDEN, "CHECKSUM IS VALID");
        return true;
    }
}

struct keyData
{
    union {
        struct
        {
            uint8 bytes[20];
        } bytes;

        struct
        {
            uint32 ints[5];
        } ints;
    };
};

uint32 Warden::BuildChecksum(const uint8* data, uint32 length)
{
    keyData hash;
    SHA1(data, length, hash.bytes.bytes);
    uint32 checkSum = 0;
    for (uint8 i = 0; i < 5; ++i)
        checkSum = checkSum ^ hash.ints.ints[i];

    return checkSum;
}

std::string Warden::Penalty(WardenCheck* check /*= NULL*/, int16 checkFailed /*= 0*/)
{
    WardenActions action;

    if (check)
        action = check->Action;
    else
        action = WardenActions(sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_FAIL_ACTION));

    //! ToDo: workaround for lua warden sometimes getting to Warden::HandleWardenDataOpcode
    //! if check 0 then return undefined and do nothing, if -1 (its penalty for wrong hash)
    //! else client failed some kind of check
    if (!checkFailed)
        return "Undefined";

    std::string banReason    = "Anticheat violation";
    std::string longDuration = "1209600s";

    bool longBan = false; // 14d = 1209600s
    if (checkFailed)
    {
        switch (checkFailed)
        {
            case 47: banReason += " (FrameXML Signature Check)"; break;
            case 51: banReason += " (Lua DoString)"; break;
            case 59: banReason += " (Lua Protection Patch)"; break;
            case 72: banReason += " (Movement State related)"; break;
            case 118: banReason += " (Wall Climb)"; break;
            case 121: banReason += " (No Fall Damage Patch)"; break;
            case 193: banReason += " (Follow Unit Check)"; break;
            case 209:
                banReason += " (WoWEmuHacker Injection)";
                longBan = true;
                break;
            case 237: banReason += " (AddChatMessage)"; break;
            case 246: banReason += " (Language Patch)"; break;
            case 260: banReason += " (Jump Momentum)"; break;
            case 288: banReason += " (Language Patch)"; break;
            case 308: banReason += " (SendChatMessage)"; break;
            case 312: banReason += " (Jump Physics)"; break;
            case 314: banReason += " (GetCharacterInfo)"; break;
            case 329: banReason += " (Wall Climb)"; break;
            case 343: banReason += " (Login Password Pointer)"; break;
            case 349: banReason += " (Language Patch)"; break;
            case 712: banReason += " (WS2_32.Send)"; break;
            case 779:
                banReason += " (rEdoX Packet Editor)";
                longBan = true;
                break;
            case 780: banReason += " (Lua Protection Remover)"; break;
            case 781: banReason += " (Walk on Water Patch)"; break;
            case 782:
                banReason += " (Collision M2 Special)";
                longBan = true;
                break;
            case 783:
                banReason += " (Collision M2 Regular)";
                longBan = true;
                break;
            case 784:
                banReason += " (Collision WMD)";
                longBan = true;
                break;
            case 785:
                banReason += " (Hitchhikers Hack)";
                longBan = true;
                break;
            case 786:
                banReason += " (WPE PRO)";
                longBan = true;
                break;
            case 787:
                banReason += " (ICanHazSpeed)";
                longBan = true;
                break;
            case 788:
                banReason += " (Maelstrom)";
                longBan = true;
                break;
            case 789:
                banReason += " (Maelstrom)";
                longBan = true;
                break;
            case 790:
                banReason += " (HideToolz)";
                longBan = true;
                break;
            case 791:
                banReason += " (Cheat Engine)";
                longBan = true;
                break;
        }
    }

    // Force to ban cheaters with 3th party software injection
    if (longBan)
        action = WARDEN_ACTION_BAN;

    switch (action)
    {
        case WARDEN_ACTION_LOG:
            sLog->outCheat("[Warden] Player: %s (Account: %u) check failed: %u", _session->GetPlayerName().c_str(), _session->GetAccountId(), checkFailed);
            return "None";
            break;
        case WARDEN_ACTION_KICK:
            sLog->outCheat("[Warden] Player: %s (Account: %u) has been kicked due to check failed: %u", _session->GetPlayerName().c_str(), _session->GetAccountId(), checkFailed);
            _session->KickPlayer();
            return "Kick";
            break;
        case WARDEN_ACTION_BAN:
        {
            std::stringstream duration;
            duration << sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_BAN_DURATION) << "s";
            std::string accountName;
            AccountMgr::GetName(_session->GetAccountId(), accountName);

            std::string durationStr = duration.str();
            sLog->outCheat("[Warden] Player: %s (Account: %u) has been banned for %s due to check failed: %u", _session->GetPlayerName().c_str(), _session->GetAccountId(), (longBan ? longDuration.c_str() : durationStr.c_str()), checkFailed);
            sWorld->BanAccount(BAN_ACCOUNT, accountName, (longBan ? longDuration.c_str() : duration.str().c_str()), banReason, "Server");

            return "Ban";
        }
        default:
            break;
    }
    return "Undefined";
}

void WorldSession::HandleWardenDataOpcode(WorldPacket& recvData)
{
    if (!_warden || recvData.empty())
    {
        recvData.rfinish();
        return;
    }

    _warden->DecryptData(recvData.contents(), recvData.size());
    uint8 opcode;
    recvData >> opcode;
    recvData.hexlike();

    sLog->outDebug(LOG_FILTER_WARDEN, "warden opcode %02X of size %u.", opcode, uint32(recvData.size() - 1));

    switch (opcode)
    {
        case WARDEN_CMSG_MODULE_MISSING:
            _warden->SendModuleToClient();
            break;
        case WARDEN_CMSG_MODULE_OK:
            _warden->RequestHash();
            break;
        case WARDEN_CMSG_CHEAT_CHECKS_RESULT:
            _warden->HandleData(recvData);
            break;
        case WARDEN_CMSG_MEM_CHECKS_RESULT:
            sLog->outDebug(LOG_FILTER_WARDEN, "NYI WARDEN_CMSG_MEM_CHECKS_RESULT received!");
            break;
        case WARDEN_CMSG_HASH_RESULT:
            _warden->HandleHashResult(recvData);
            _warden->InitializeModule();
            break;
        case WARDEN_CMSG_MODULE_FAILED:
            sLog->outDebug(LOG_FILTER_WARDEN, "NYI WARDEN_CMSG_MODULE_FAILED received!");
            break;
        default:
            sLog->outDebug(LOG_FILTER_WARDEN, "Got unknown warden opcode %02X of size %u.", opcode, uint32(recvData.size() - 1));
            break;
    }
}

void Warden::LoadAndSendLuaCheck(std::string const& filename)
{
    if (!sWorld->getIntConfig(CONFIG_ENABLE_WARDEN_LUA))
        return;

    std::string path = fmt::format(sWorld->GetLuaScriptsPath() + "{}.log", filename);
    std::stringstream strstream;
    std::ifstream fstream;
    fstream.open(strstream.str(), std::ios_base::in);
    if (!fstream.is_open())
        return;

    strstream << fstream.rdbuf();
    std::string fileContent = strstream.str();

    ByteBuffer buff;
    buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);

    buff << uint8(fileContent.size());
    buff.append(fileContent.c_str(), fileContent.size());

    uint8 xorByte = _inputKey[0];

    // Add TIMING_CHECK
    buff << uint8(0x00);
    buff << uint8(LUA_STR_CHECK ^ xorByte);

    uint8 index = 1;
    buff << uint8(index++);
    buff << uint8(xorByte);
    buff.hexlike();

    // Encrypt with warden RC4 key
    EncryptData(buff.contents(), buff.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff.size());
    pkt.append(buff);
    _session->SendPacket(&pkt);

    OnLuaCodeSent();
}

void WardenReport::TrimCallstack()
{
    if (Message.length() > static_cast<size_t>(WARDEN_CALLSTACK_MAXIMUM_SIZE))
        Message.resize(static_cast<size_t>(WARDEN_CALLSTACK_MAXIMUM_SIZE));
}

namespace WardenHelpers {

bool ParseWardenMessage(std::string& message)
{
    if (message.empty())
        return false;

    for (auto const& keyword : WardenHelpers::CLIENT_DEBUGSTACK_SAFE_KEY_WORDS_STORE)
    {
        if (message.find(keyword) != std::string::npos)
            return true;
    }

    return false;
}
}
