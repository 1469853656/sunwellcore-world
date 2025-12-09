
#include "Threading/BackgroundWorker.hpp"
#include "Cryptography/HMAC.h"
#include "Cryptography/WardenKeyGeneration.h"
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Log.h"
#include "Opcodes.h"
#include "ByteBuffer.h"
#include <openssl/md5.h>
#include "Database/DatabaseEnv.h"
#include "World.h"
#include "Player.h"
#include "Util.h"
#include "WardenWin.h"
#include "WardenModuleWin.h"
#include "WardenCheckMgr.h"
#include "StringValidation.h"
#include "AccountMgr.h"

#include <cstring>
#include <chrono>

WardenWin::WardenWin()
    : Warden()
    , _serverTicks(0)
{
}

WardenWin::~WardenWin()
{
    // Xinef: ZOMG! CRASH DEBUG INFO
    uint32 otherSize = _otherChecksTodo.size();
    uint32 memSize   = _memChecksTodo.size();
    uint32 curSize   = _currentChecks.size();
    bool otherClear  = _otherChecksTodo.empty();
    bool memClear    = _memChecksTodo.empty();
    bool curClear    = _currentChecks.empty();

    sLog->outDebug(LOG_FILTER_POOLSYS, "IM DESTRUCTING MYSELF QQ, OTHERSIZE: %u, OTHEREM: %u, MEMSIZE: %u, MEMEM: %u, CURSIZE: %u, CUREM: %u!\n", otherSize, otherClear, memSize, memClear, curSize, curClear);
    _otherChecksTodo.clear();
    _memChecksTodo.clear();
    _currentChecks.clear();
    sLog->outDebug(LOG_FILTER_POOLSYS, "IM DESTRUCTING MYSELF QQ, OTHERSIZE: %u, OTHEREM: %u, MEMSIZE: %u, MEMEM: %u, CURSIZE: %u, CUREM: %u!\n", otherSize, otherClear, memSize, memClear, curSize, curClear);
}

void WardenWin::Init(WorldSession* session, ::SessionKey const& K)
{
    _session = session;
    // Generate Warden Key
    SessionKeyGenerator<Trinity::Crypto::SHA1> WK(K);
    WK.Generate(_inputKey, 16);
    WK.Generate(_outputKey, 16);

    memcpy(_seed, Module.Seed, 16);

    _inputCrypto.Init(_inputKey);
    _outputCrypto.Init(_outputKey);
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "Server side warden for client %u initializing...", session->GetAccountId());
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "C->S Key: %s", ByteArrayToHexStr(_inputKey, 16).c_str());
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "S->C Key: %s", ByteArrayToHexStr(_outputKey, 16).c_str());
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "  Seed: %s", ByteArrayToHexStr(_seed, 16).c_str());
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "Loading Module...");

    _module = GetModuleForClient();

    ; //sLog->outDebug(LOG_FILTER_WARDEN, "Module Key: %s", ByteArrayToHexStr(_module->Key, 16).c_str());
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "Module ID: %s", ByteArrayToHexStr(_module->Id, 16).c_str());
    RequestModule();
}

ClientWardenModule* WardenWin::GetModuleForClient()
{
    ClientWardenModule* mod = new ClientWardenModule;

    uint32 length = sizeof(Module.Module);

    // data assign
    mod->CompressedSize = length;
    mod->CompressedData = new uint8[length];
    memcpy(mod->CompressedData, Module.Module, length);
    memcpy(mod->Key, Module.ModuleKey, 16);

    // md5 hash
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, mod->CompressedData, length);
    MD5_Final((uint8*)&mod->Id, &ctx);

    return mod;
}

void WardenWin::InitializeModule()
{
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "Initialize module");

    // Create packet structure
    WardenInitModuleRequest Request;
    Request.Command1        = WARDEN_SMSG_MODULE_INITIALIZE;
    Request.Size1           = 20;
    Request.Unk1            = 1;
    Request.Unk2            = 0;
    Request.Type            = 1;
    Request.String_library1 = 0;
    Request.Function1[0]    = 0x00024F80; // 0x00400000 + 0x00024F80 SFileOpenFile
    Request.Function1[1]    = 0x000218C0; // 0x00400000 + 0x000218C0 SFileGetFileSize
    Request.Function1[2]    = 0x00022530; // 0x00400000 + 0x00022530 SFileReadFile
    Request.Function1[3]    = 0x00022910; // 0x00400000 + 0x00022910 SFileCloseFile
    Request.CheckSumm1      = BuildChecksum(&Request.Unk1, 20);

    Request.Command2        = WARDEN_SMSG_MODULE_INITIALIZE;
    Request.Size2           = 8;
    Request.Unk3            = 4;
    Request.Unk4            = 0;
    Request.String_library2 = 0;
    Request.Function2       = 0x00419210; // 0x00819D40;                         // 0x00400000 + 0x00419D40 FrameScript::Execute

    Request.Function2_set = 1;
    Request.CheckSumm2    = BuildChecksum(&Request.Unk3, 8);

    Request.Command3        = WARDEN_SMSG_MODULE_INITIALIZE;
    Request.Size3           = 8;
    Request.Unk5            = 1;
    Request.Unk6            = 1;
    Request.String_library3 = 0;
    Request.Function3       = 0x0046AE20; // 0x00400000 + 0x0046AE20 PerformanceCounter
    Request.Function3_set   = 1;
    Request.CheckSumm3      = BuildChecksum(&Request.Unk5, 8);

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&Request, sizeof(WardenInitModuleRequest));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenInitModuleRequest));
    pkt.append((uint8*)&Request, sizeof(WardenInitModuleRequest));
    _session->SendPacket(&pkt);
}

void WardenWin::RequestHash()
{
    ; //sLog->outDebug(LOG_FILTER_WARDEN, "Request hash");

    // Create packet structure
    WardenHashRequest Request;
    Request.Command = WARDEN_SMSG_HASH_REQUEST;
    memcpy(Request.Seed, _seed, 16);

    // Encrypt with warden RC4 key.
    EncryptData((uint8*)&Request, sizeof(WardenHashRequest));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenHashRequest));
    pkt.append((uint8*)&Request, sizeof(WardenHashRequest));
    _session->SendPacket(&pkt);
}

void WardenWin::HandleHashResult(ByteBuffer& buff)
{
    buff.rpos(buff.wpos());

    // Verify key
    if (memcmp(buff.contents() + 1, Module.ClientKeySeedHash, 20) != 0)
    {
        sLog->outDebug(LOG_FILTER_WARDEN, "Request hash reply: failed");
        Penalty(nullptr, -1);
        return;
    }

    sLog->outDebug(LOG_FILTER_WARDEN, "Request hash reply: succeed");

    // Change keys here
    memcpy(_inputKey, Module.ClientKeySeed, 16);
    memcpy(_outputKey, Module.ServerKeySeed, 16);

    _inputCrypto.Init(_inputKey);
    _outputCrypto.Init(_outputKey);

    _initialized = true;

    _previousTimestamp = World::GetGameTimeMS();
}

void WardenWin::RequestData()
{
    sLog->outDebug(LOG_FILTER_WARDEN, "Warden::RequestData");

    // If all checks were done, fill the todo list again
    if (_memChecksTodo.empty())
        _memChecksTodo.assign(sWardenCheckMgr->MemChecksIdPool.begin(), sWardenCheckMgr->MemChecksIdPool.end());

    if (_otherChecksTodo.empty())
        _otherChecksTodo.assign(sWardenCheckMgr->OtherChecksIdPool.begin(), sWardenCheckMgr->OtherChecksIdPool.end());

    _serverTicks = World::GetGameTimeMS();

    if (CanSendLuaCode())
    {
        _dataSent   = false;
        _checkTimer = 20 * IN_MILLISECONDS;
        SendLuaChecks();
        return;
    }

    uint16 id;
    uint8 type;
    WardenCheck* wd;
    _currentChecks.clear();

    // Build check request
    for (uint32 i = 0; i < sWorld->getIntConfig(CONFIG_WARDEN_NUM_MEM_CHECKS); ++i)
    {
        // If todo list is done break loop (will be filled on next Update() run)
        if (_memChecksTodo.empty())
            break;

        // Get check id from the end and remove it from todo
        id = _memChecksTodo.back();
        _memChecksTodo.pop_back();

        // Add the id to the list sent in this cycle
        if (id != 786 /*WPE PRO*/ && id != 209 /*WoWEmuHacker*/)
            _currentChecks.push_back(id);
    }
    _currentChecks.push_back(786);
    _currentChecks.push_back(209);

    ByteBuffer buff;
    buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);

    std::shared_lock lock(sWardenCheckMgr->_checkStoreLock);

    for (uint32 i = 0; i < sWorld->getIntConfig(CONFIG_WARDEN_NUM_OTHER_CHECKS); ++i)
    {
        // If todo list is done break loop (will be filled on next Update() run)
        if (_otherChecksTodo.empty())
            break;

        // Get check id from the end and remove it from todo
        id = _otherChecksTodo.back();
        _otherChecksTodo.pop_back();

        // Add the id to the list sent in this cycle
        _currentChecks.push_back(id);

        wd = sWardenCheckMgr->GetWardenDataById(id);
        ASSERT(wd);

        switch (wd->Type)
        {
            case MPQ_CHECK:
            case DRIVER_CHECK:
            case LUA_STR_CHECK:
                buff << uint8(wd->Str.size());
                buff.append(wd->Str.c_str(), wd->Str.size());
                break;
            default:
                break;
        }
    }

    uint8 xorByte = _inputKey[0];

    // Add TIMING_CHECK
    buff << uint8(0x00);
    buff << uint8(TIMING_CHECK ^ xorByte);

    uint8 index = 1;

    for (std::list<uint16>::iterator itr = _currentChecks.begin(); itr != _currentChecks.end(); ++itr)
    {
        wd = sWardenCheckMgr->GetWardenDataById(*itr);

        if (!wd)
            continue;

        type = wd->Type;
        buff << uint8(type ^ xorByte);
        switch (type)
        {
            case MEM_CHECK:
            {
                buff << uint8(0x00);
                buff << uint32(wd->Address);
                buff << uint8(wd->Length);
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            {
                buff.append(wd->Data.AsByteArray(0, false).get(), wd->Data.GetNumBytes());
                buff << uint32(wd->Address);
                buff << uint8(wd->Length);
                break;
            }
            case MPQ_CHECK:
            case LUA_STR_CHECK:
            {
                buff << uint8(index++);
                break;
            }
            case DRIVER_CHECK:
            {
                buff.append(wd->Data.AsByteArray(0, false).get(), wd->Data.GetNumBytes());
                buff << uint8(index++);
                break;
            }
            case MODULE_CHECK:
            {
                uint32 seed = rand32();
                buff << uint32(seed);
                Trinity::Crypto::HMAC_SHA1 hmac((uint8*)&seed, 4);
                hmac.UpdateData(wd->Str);
                hmac.Finalize();
                buff.append(hmac.GetDigest());
                break;
            }
            /*case PROC_CHECK:
            {
                buff.append(wd->i.AsByteArray(0, false).get(), wd->i.GetNumBytes());
                buff << uint8(index++);
                buff << uint8(index++);
                buff << uint32(wd->Address);
                buff << uint8(wd->Length);
                break;
            }*/
            default:
                break; // Should never happen
        }
    }
    buff << uint8(xorByte);
    buff.hexlike();

    // Encrypt with warden RC4 key
    EncryptData(buff.contents(), buff.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff.size());
    pkt.append(buff);
    _session->SendPacket(&pkt);

    _dataSent = true;

    if (sWorld->getIntConfig(CONFIG_ENABLE_WARDEN_LUA) && ++_normalCheckCounter > WARDEN_CYCLES_REQUIRED_FOR_LUA_TURN)
    {
        _sendLuaChecks      = true;
        _normalCheckCounter = 0;
    }
}

std::string PacketContentsToString(const uint8* packetContents, uint16 len)
{
    std::ostringstream stream;

    for (uint32 i = 0; i < len; i++)
    {
        if (static_cast<int>(packetContents[i]) < 16)
            stream << std::uppercase << std::hex << "0" << static_cast<int>(packetContents[i]) << "";
        else
            stream << std::uppercase << std::hex << static_cast<int>(packetContents[i]) << "";
    }

    std::string data_str = stream.str();
    return data_str;
}

void WardenWin::HandleData(ByteBuffer& buff)
{
    sLog->outDebug(LOG_FILTER_WARDEN, "Handle data");

    //! Data sent by lua module, discard everything, it returns garbage data
    if (!_dataSent)
        return;

    _dataSent            = false;
    _clientResponseTimer = 0;

    uint16 Length;
    buff >> Length;
    uint32 Checksum;
    buff >> Checksum;

    if (!IsValidCheckSum(Checksum, buff.contents() + buff.rpos(), Length))
    {
        buff.rpos(buff.wpos());
        sLog->outDebug(LOG_FILTER_WARDEN, "CHECKSUM FAIL");
        _session->KickPlayer(true);
        return;
    }

    // TIMING_CHECK
    {
        uint8 result;
        buff >> result;
        // TODO: test it.
        if (result == 0x00)
        {
            sLog->outDebug(LOG_FILTER_WARDEN, "TIMING CHECK FAIL result 0x00");
            _session->KickPlayer(true);
            return;
        }

        uint32 newClientTicks;
        buff >> newClientTicks;

        uint32 ticksNow = World::GetGameTimeMS();
        uint32 ourTicks = newClientTicks + (ticksNow - _serverTicks);

        if (sLog->IsOutDebug())
        {
            sLog->outDebug(LOG_FILTER_WARDEN, "ServerTicks %u", ticksNow);      // Now
            sLog->outDebug(LOG_FILTER_WARDEN, "RequestTicks %u", _serverTicks); // At request
            sLog->outDebug(LOG_FILTER_WARDEN, "Ticks %u", newClientTicks);      // At response
            sLog->outDebug(LOG_FILTER_WARDEN, "Ticks diff %u", ourTicks - newClientTicks);
        }
    }

    WardenCheckResult* rs;
    WardenCheck* rd;
    uint8 type;
    uint16 checkFailed = 0;

    std::shared_lock lock(sWardenCheckMgr->_checkStoreLock);

    for (std::list<uint16>::iterator itr = _currentChecks.begin(); itr != _currentChecks.end(); ++itr)
    {
        rd = sWardenCheckMgr->GetWardenDataById(*itr);
        rs = sWardenCheckMgr->GetWardenResultById(*itr);

        type = rd->Type;
        switch (type)
        {
            case MEM_CHECK:
            {
                uint8 Mem_Result;
                buff >> Mem_Result;

                if (Mem_Result != 0)
                {
                    sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MEM_CHECK not 0x00, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    checkFailed = *itr;
                    continue;
                }

                if (memcmp(buff.contents() + buff.rpos(), rs->Result.AsByteArray(0, false).get(), rd->Length) != 0)
                {
                    sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MEM_CHECK fail CheckId %u account Id %u", *itr, _session->GetAccountId());
                    checkFailed = *itr;
                    buff.rpos(buff.rpos() + rd->Length);
                    continue;
                }

                buff.rpos(buff.rpos() + rd->Length);
                sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MEM_CHECK passed CheckId %u account Id %u", *itr, _session->GetAccountId());
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            case DRIVER_CHECK:
            case MODULE_CHECK:
            {
                const uint8 byte = 0xE9;
                if (memcmp(buff.contents() + buff.rpos(), &byte, sizeof(uint8)) != 0)
                {
                    if (sLog->IsOutDebug())
                    {
                        if (type == PAGE_CHECK_A || type == PAGE_CHECK_B)
                            sLog->outDebug(LOG_FILTER_WARDEN, "RESULT PAGE_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                        if (type == MODULE_CHECK)
                            sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MODULE_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                        if (type == DRIVER_CHECK)
                            sLog->outDebug(LOG_FILTER_WARDEN, "RESULT DRIVER_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    }
                    checkFailed = *itr;
                    buff.rpos(buff.rpos() + 1);
                    continue;
                }

                buff.rpos(buff.rpos() + 1);
                if (sLog->IsOutDebug())
                {
                    if (type == PAGE_CHECK_A || type == PAGE_CHECK_B)
                        sLog->outDebug(LOG_FILTER_WARDEN, "RESULT PAGE_CHECK passed CheckId %u account Id %u", *itr, _session->GetAccountId());
                    else if (type == MODULE_CHECK)
                        sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MODULE_CHECK passed CheckId %u account Id %u", *itr, _session->GetAccountId());
                    else if (type == DRIVER_CHECK)
                        sLog->outDebug(LOG_FILTER_WARDEN, "RESULT DRIVER_CHECK passed CheckId %u account Id %u", *itr, _session->GetAccountId());
                }
                break;
            }
            case LUA_STR_CHECK:
            {
                uint8 Lua_Result;
                buff >> Lua_Result;

                if (Lua_Result != 0)
                {
                    sLog->outDebug(LOG_FILTER_WARDEN, "RESULT LUA_STR_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    checkFailed = *itr;
                    continue;
                }

                uint8 luaStrLen;
                buff >> luaStrLen;

                if (luaStrLen != 0)
                {
                    char* str = new char[luaStrLen + 1];
                    memcpy(str, buff.contents() + buff.rpos(), luaStrLen);
                    str[luaStrLen] = '\0'; // null terminator
                    sLog->outDebug(LOG_FILTER_WARDEN, "Lua string: %s", str);
                    delete[] str;
                }
                buff.rpos(buff.rpos() + luaStrLen); // Skip string
                sLog->outDebug(LOG_FILTER_WARDEN, "RESULT LUA_STR_CHECK passed, CheckId %u account Id %u", *itr, _session->GetAccountId());
                break;
            }
            case MPQ_CHECK:
            {
                uint8 Mpq_Result;
                buff >> Mpq_Result;

                if (Mpq_Result != 0)
                {
                    sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MPQ_CHECK not 0x00 account id %u", _session->GetAccountId());
                    checkFailed = *itr;
                    continue;
                }

                if (memcmp(buff.contents() + buff.rpos(), rs->Result.AsByteArray(0, false).get(), 20) != 0) // SHA1
                {
                    sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MPQ_CHECK fail, CheckId %u account Id %u", *itr, _session->GetAccountId());
                    checkFailed = *itr;
                    buff.rpos(buff.rpos() + 20); // 20 bytes SHA1
                    continue;
                }

                buff.rpos(buff.rpos() + 20); // 20 bytes SHA1
                sLog->outDebug(LOG_FILTER_WARDEN, "RESULT MPQ_CHECK passed, CheckId %u account Id %u", *itr, _session->GetAccountId());
                break;
            }
            default: // Should never happen
                break;
        }
    }

    if (checkFailed > 0)
    {
        WardenCheck* check = sWardenCheckMgr->GetWardenDataById(checkFailed);
        Penalty(check, checkFailed);
    }

    // Set hold off timer, minimum timer should at least be 10 second
    uint32 holdOff = sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF);
    _checkTimer    = (holdOff < 10 ? 10 : holdOff) * IN_MILLISECONDS;
}

void WardenWin::SendLuaChecks()
{
    if (!sWorld->getIntConfig(CONFIG_ENABLE_WARDEN_LUA))
        return;

    for (auto const& ref : _preparedLuaScripts)
    {
        ByteBuffer buff;
        buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);

        buff << uint8(ref.size());
        buff.append(ref.c_str(), ref.size());

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
    }

    OnLuaCodeSent();
}

void WardenWin::SendCharacterScreenScript()
{
    if (!sWorld->getIntConfig(CONFIG_ENABLE_WARDEN_LUA) || !sWorld->getIntConfig(CONFIG_WARDEN_LUA_ENABLE_CHARACTER_SCREEN_MSG))
        return;

    //! This function is called when player requests char enum
    //! but that doesnt mean warden was initialized at this point, in most cases it will be
    if (!IsInitialized())
        return;

    for (auto const& ref : sWorld->GetCharacterScreenScript())
    {
        ByteBuffer buff;
        buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);

        buff << uint8(ref.size());
        buff.append(ref.c_str(), ref.size());

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
    }

    OnLuaCodeSent();
}

bool WardenWin::CanSendLuaCode() const
{
    bool const playerMeetsRequirements = _session->GetPlayer() && !_session->GetPlayer()->isBeingLoaded() && _session->GetPlayer()->getLevel() >= sWorld->getIntConfig(CONFIG_WARDEN_LUA_MINIMUM_PLAYER_LEVEL);
    return _initialized && sWorld->getIntConfig(CONFIG_ENABLE_WARDEN_LUA) && _sendLuaChecks && playerMeetsRequirements;
}

bool WardenWin::IsAddonMessageResult(std::string const& target) const
{
    if (!ValidateInputString(target, WARDEN_MESSAGE_TARGET_SIZE))
        return false;

    if (target != _messageTargetName)
        return false;

    return true;
}

void WardenWin::HandleAddonMessageResult(Player* sender, std::string&& message)
{
    sBackgroundWorker->Run([senderData = WardenReport(std::move(message), sender->GetGUIDLow(), std::chrono::system_clock::now())]() mutable
    {
        bool const result = WardenHelpers::ParseWardenMessage(senderData.Message);
        if (!result)
        {
            senderData.TrimCallstack();
            sWorld->AddWardenReport(std::move(senderData));
        }
    }, Threading::DetachedTask);
}

void WardenWin::OnLuaCodeSent()
{
    _dataSent      = false;
    _checkTimer    = 20 * IN_MILLISECONDS;
    _sendLuaChecks = false;
    _normalCheckCounter = 0;
}
