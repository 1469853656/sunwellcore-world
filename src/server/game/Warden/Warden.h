

#ifndef _WARDEN_BASE_H
#define _WARDEN_BASE_H

#include <map>
#include "Cryptography/ARC4.h"
#include "ByteBuffer.h"
#include "WardenCheckMgr.h"

enum WardenOpcodes
{
    // Client->Server
    WARDEN_CMSG_MODULE_MISSING      = 0,
    WARDEN_CMSG_MODULE_OK           = 1,
    WARDEN_CMSG_CHEAT_CHECKS_RESULT = 2,
    WARDEN_CMSG_MEM_CHECKS_RESULT   = 3, // only sent if MEM_CHECK bytes doesn't match
    WARDEN_CMSG_HASH_RESULT         = 4,
    WARDEN_CMSG_MODULE_FAILED       = 5, // this is sent when client failed to load uploaded module due to cache fail

    // Server->Client
    WARDEN_SMSG_MODULE_USE           = 0,
    WARDEN_SMSG_MODULE_CACHE         = 1,
    WARDEN_SMSG_CHEAT_CHECKS_REQUEST = 2,
    WARDEN_SMSG_MODULE_INITIALIZE    = 3,
    WARDEN_SMSG_MEM_CHECKS_REQUEST   = 4, // byte len; while(!EOF) { byte unk(1); byte index(++); string module(can be 0); int offset; byte len; byte[] bytes_to_compare[len]; }
    WARDEN_SMSG_HASH_REQUEST         = 5
};

enum WardenCheckType
{
    //! This allows to send strings as well
    MEM_CHECK     = 0xF3, // 243: byte moduleNameIndex + uint Offset + byte Len (check to ensure memory isn't modified)
    PAGE_CHECK_A  = 0xB2, // 178: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans all pages for specified hash)
    PAGE_CHECK_B  = 0xBF, // 191: uint Seed + byte[20] SHA1 + uint Addr + byte Len (scans only pages starts with MZ+PE headers for specified hash)
    MPQ_CHECK     = 0x98, // 152: byte fileNameIndex (check to ensure MPQ file isn't modified)
    LUA_STR_CHECK = 0x8B, // 139: byte luaNameIndex (check to ensure -> GLOBAL <- LUA string isn't used)
    DRIVER_CHECK  = 0x71, // 113: uint Seed + byte[20] SHA1 + byte driverNameIndex (check to ensure driver isn't loaded)
    //! Im pretty sure that wasnt used on 3.3.5a
    TIMING_CHECK = 0x57, //  87: empty (check to ensure GetTickCount() isn't detoured)
    PROC_CHECK   = 0x7E, // 126: uint Seed + byte[20] SHA1 + byte moluleNameIndex + byte procNameIndex + uint Offset + byte Len (check to ensure proc isn't detoured)
    MODULE_CHECK = 0xD9, // 217: uint Seed + byte[20] SHA1 (check to ensure module isn't injected)
};

#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct WardenModuleUse
{
    uint8 Command;
    uint8 ModuleId[16];
    uint8 ModuleKey[16];
    uint32 Size;
};

struct WardenModuleTransfer
{
    uint8 Command;
    uint16 DataSize;
    uint8 Data[500];
};

struct WardenHashRequest
{
    uint8 Command;
    uint8 Seed[16];
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

struct ClientWardenModule
{
    uint8 Id[16];
    uint8 Key[16];
    uint32 CompressedSize;
    uint8* CompressedData;
};

class WorldSession;

static std::string const S_WARDEN_GET_GLOBAL_NEWS = R"(local n = _G['sunnews'] n=n.."{}" _G['sunnews'] = n)";
static std::string const S_WARDEN_CHARSET         = R"(abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ)";
constexpr uint32 WARDEN_MESSAGE_TARGET_SIZE{ 4 };
constexpr size_t WARDEN_CALLSTACK_MAXIMUM_SIZE{ 500 };
constexpr uint32 WARDEN_CYCLES_REQUIRED_FOR_LUA_TURN{ 4 };

/** Message prefix legend:
- isecCLIN = insecure client
- PKUFN = PQR EventFrame found
- inby = Cast spell by name triggered
- indi = cast spell by id triggered
- inha = auction query got triggered
**/
static std::string const S_WARDEN_SEND_ADDON                = R"(if ({0} == nil)then {0}=SendAddonMessage end)";
static std::string const S_WARDEN_GLOBAL_TABLES_CHECKER     = R"(if (chk{0}==nil)then chk{0}=function() local a=_G['{0}'] if (a ~= nil)then local b=''..a {1}('{1}',b,'WHISPER','{2}') _G['{0}']=nil end end end)";
static std::string const S_WARDEN_CHECK_LUA_STATE           = R"(if (issecure()==false)then _G['{}']=':isecCLIN' end)";
static std::string const S_WARDEN_CHECK_PQR_FRAME           = R"(if (UIParent) then if(PQR_EventFrame~=nil)then _G["{}"]=":PKUFN" end end)";
static std::string const S_WARDEN_CHECK_CASTSPELLBYNAME     = R"(if(rcbn==nil and CastSpellByName~=nil)then rcbn=CastSpellByName CastSpellByName=function(a, b) rcbn(a, b) local z=("inby:"..debugstack(2)) _G["{0}"]=z CastSpellByName=rcbn rcbn=nil if chk{0}~=nil then chk{0}() end end end)";
static std::string const S_WARDEN_CHECK_CASTSPELLBYID       = R"(if(rcbid==nil and CastSpellByID~=nil)then rcbid=CastSpellByID CastSpellByID=function(a, b) rcbid(a, b) local z=("indi:"..debugstack(2)) _G["{0}"]=z CastSpellByID=rcbid rcbid=nil if chk{0}~=nil then chk{0}() end end end)";
static std::string const S_WARDEN_CHECK_AUCTIONHOUSE_QUERY  = R"(if(rqai==nil and QueryAuctionItems~=nil) then rqai=QueryAuctionItems QueryAuctionItems=function(a,b,c,d,e,f,g,h,j) rqai(a,b,c,d,e,f,g,h,j) local a=("inha:"..debugstack(2)) _G["{0}"]=a QueryAuctionItems=rqai rqai=nil end end)";

struct WardenReport
{
    WardenReport(std::string message, uint64 guid, std::chrono::system_clock::time_point time)
        : Message(message)
        , Guid(guid)
        , ReportTime(time)
    {
    }

    void TrimCallstack();

    std::string Message = "";
    uint64 Guid         = 0;
    std::chrono::system_clock::time_point ReportTime{};
};

struct WardenFailureReport
{
    WardenFailureReport(std::string message, std::string const& name, uint32 guidLow, uint32 accountId, time_t failureTime)
        : FailureMessage(message)
        , CharacterName(name)
        , GuidLow(guidLow)
        , AccountId(accountId)
        , FailureTime(failureTime)
    {
    }


    std::string FailureMessage = "";
    std::string CharacterName  = "";
    uint32 GuidLow             = 0;
    uint32 AccountId           = 0;
    time_t FailureTime         = 0;
};

class Warden
{
    friend class WardenWin;
    friend class WardenMac;

public:
    Warden();
    virtual ~Warden();

    virtual void Init(WorldSession* session, ::SessionKey const& key)                          = 0;
    virtual ClientWardenModule* GetModuleForClient()                                           = 0;
    virtual void InitializeModule()                                                            = 0;
    virtual void RequestHash()                                                                 = 0;
    virtual void HandleHashResult(ByteBuffer& buff)                                            = 0;
    virtual void RequestData()                                                                 = 0;
    virtual void HandleData(ByteBuffer& buff)                                                  = 0;
    virtual void SendCharacterScreenScript()                                                   = 0;
    virtual void SendLuaChecks()                                                               = 0;
    virtual bool CanSendLuaCode() const                                                        = 0;
    virtual bool IsAddonMessageResult(std::string const& /*target*/) const                     = 0;
    virtual void HandleAddonMessageResult(Player* /*sender*/, std::string&& /*message*/) = 0;
    virtual void OnLuaCodeSent()                                                               = 0;

    void SendModuleToClient();
    void RequestModule();
    void Update();
    void DecryptData(uint8* buffer, uint32 length);
    void EncryptData(uint8* buffer, uint32 length);

    static bool IsValidCheckSum(uint32 checksum, const uint8* data, const uint16 length);
    static uint32 BuildChecksum(const uint8* data, uint32 length);

    inline bool IsInitialized() const { return _initialized; }

    void LoadAndSendLuaCheck(std::string const& /*filename*/);

    // If no check is passed, the default action from config is executed
    std::string Penalty(WardenCheck* check = nullptr, int16 checkFailed = 0);

private:
    WorldSession* _session;
    ClientWardenModule* _module;

    Trinity::Crypto::ARC4 _inputCrypto;
    Trinity::Crypto::ARC4 _outputCrypto;

    uint8 _inputKey[16];
    uint8 _outputKey[16];
    uint8 _seed[16];
    uint32 _clientResponseTimer; // Timer for client response delay
    uint32 _previousTimestamp;

    bool _dataSent;
    bool _initialized;

    uint32 _checkTimer;
    bool _sendLuaChecks;
    uint32 _normalCheckCounter;

public:
    //! Global tables accessor to extract data from the client, different for each client
    std::string _globalTablesAccessor;
    //! Function prefix for our replaced SendAddonMessage function
    std::string _sendAddonMessageFunc;
    //! Message target for our warden messages, tries to be unique for each character
    std::string _messageTargetName;

    StringVector _preparedLuaScripts;
};

namespace WardenHelpers {

static const std::vector<std::string> CLIENT_DEBUGSTACK_SAFE_KEY_WORDS_STORE
{
    "Secure",
    "OnEnterPressed",
    "Enchantrix",
    "Spotter",
    "Auction",
    "Auc-Advanced"
};

bool ParseWardenMessage(std::string& message);

} // end of namespace WardenHelpers

#endif
