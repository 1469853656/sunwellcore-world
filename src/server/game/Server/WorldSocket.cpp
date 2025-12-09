#include "WorldSocket.h"
#include "Common.h"
#include "Player.h"
#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "ByteBuffer.h"
#include "Opcodes.h"
#include "DatabaseEnv.h"
#include "WorldSession.h"
#include "WorldSocketMgr.h"
#include "Log.h"
#include "PacketLog.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Cryptography/CryptoHash.h"
#include "Cryptography/CryptoRandom.h"
#include "Util.h"
#include <sstream>
#include <iomanip>

#include <memory>
#include "Profiler.h"

#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct ServerPktHeader
{
    /**
     * size is the length of the payload _plus_ the length of the opcode
     */
    ServerPktHeader(uint32 size, uint16 cmd)
        : size(size)
    {
        uint8 headerIndex = 0;
        if (isLargePacket())
        {
            header[headerIndex++] = 0x80 | (0xFF & (size >> 16));
        }
        header[headerIndex++] = 0xFF & (size >> 8);
        header[headerIndex++] = 0xFF & size;

        header[headerIndex++] = 0xFF & cmd;
        header[headerIndex++] = 0xFF & (cmd >> 8);
    }

    uint8 getHeaderLength()
    {
        // cmd = 2 bytes, size= 2||3bytes
        return 2 + (isLargePacket() ? 3 : 2);
    }

    bool isLargePacket() const
    {
        return size > 0x7FFF;
    }

    const uint32 size;
    uint8 header[5];
};

struct ClientPktHeader
{
    uint16 size;
    uint32 cmd;

    bool IsValidSize() const { return size >= 4 && size < 10240; }
    bool IsValidOpcode() const { return cmd < NUM_MSG_TYPES; }
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

WorldSocket::WorldSocket(tcp::socket&& sock)
    : Socket(std::move(sock))
    , m_OverSpeedPings(0)
    , m_Session(nullptr)
    , m_isPacketLoggingEnabled(false)
    , m_OutBufferSize(65536)
{
    Trinity::Crypto::GetRandomBytes(m_Seed);

    _headerBuffer.Resize(sizeof(ClientPktHeader));
    packetLog = std::make_unique<PacketLog>();
}

WorldSocket::~WorldSocket(void) = default;

void WorldSocket::SendPacket(WorldPacket const& packet)
{
    if (!IsOpen())
        return;

    if (m_isPacketLoggingEnabled && packetLog->CanLogPacket())
        packetLog->LogPacket(packet, SERVER_TO_CLIENT);

    m_bufferQueue.Enqueue(new WorldPacketNode(packet));
}

void WorldSocket::Start()
{
    WorldPacket packet(SMSG_AUTH_CHALLENGE, 40);
    packet << uint32(1); // 1...31
    packet.append(m_Seed);
    packet.append(Trinity::Crypto::GetRandomBytes<32>());

    SendPacket(packet);
    AsyncRead();
}

void WorldSocket::ReadHandler()
{
    if (!IsOpen())
        return;

    MessageBuffer& packet = GetReadBuffer();
    while (packet.GetActiveSize() > 0)
    {
        if (_headerBuffer.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), _headerBuffer.GetRemainingSpace());
            _headerBuffer.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (_headerBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }

            // We just received nice new header
            if (!ReadHeaderHandler())
            {
                CloseSocket();
                return;
            }
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetRemainingSpace() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(packet.GetActiveSize(), _packetBuffer.GetRemainingSpace());
            _packetBuffer.Write(packet.GetReadPointer(), readDataSize);
            packet.ReadCompleted(readDataSize);

            if (_packetBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole data this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }
        }

        // just received fresh new payload
        bool result = ProcessIncoming();
        _headerBuffer.Reset();
        if (!result)
        {
            CloseSocket();
            return;
        }
    }

    AsyncRead();
}

bool WorldSocket::ReadHeaderHandler()
{
    ASSERT(_headerBuffer.GetActiveSize() == sizeof(ClientPktHeader));

    if (m_Crypt.IsInitialized())
        m_Crypt.DecryptRecv(_headerBuffer.GetReadPointer(), sizeof(ClientPktHeader));

    ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
    EndianConvertReverse(header->size);
    EndianConvert(header->cmd);

    if (!header->IsValidSize() || !header->IsValidOpcode())
    {
        sLog->outError("WorldSocket::ReadHeaderHandler(): client %s sent malformed packet (size: %hu, cmd: %u)",
            GetRemoteIpAddress().to_string().c_str(), header->size, header->cmd);
        return false;
    }

    header->size -= sizeof(header->cmd);
    _packetBuffer.Resize(header->size);
    return true;
}

bool WorldSocket::Update()
{
    // move packets from packet queue to raw data write queue
    WorldPacketNode* queued = nullptr;
    MessageBuffer buffer(m_OutBufferSize);
    while (m_bufferQueue.Dequeue(queued))
    {
        ServerPktHeader header(queued->size() + 2, queued->GetOpcode());

        if (m_Crypt.IsInitialized())
            m_Crypt.EncryptSend((uint8*)header.header, header.getHeaderLength());

        if (buffer.GetRemainingSpace() < queued->size() + header.getHeaderLength())
        {
            QueuePacket(std::move(buffer));
            buffer.Resize(m_OutBufferSize);
        }

        if (buffer.GetRemainingSpace() >= queued->size() + header.getHeaderLength())
        {
            buffer.Write(header.header, header.getHeaderLength());
            if (!queued->empty())
                buffer.Write(queued->contents(), queued->size());
        }
        else    // single packet larger than 4096 bytes
        {
            MessageBuffer packetBuffer(queued->size() + header.getHeaderLength());
            packetBuffer.Write(header.header, header.getHeaderLength());
            if (!queued->empty())
                packetBuffer.Write(queued->contents(), queued->size());

            QueuePacket(std::move(packetBuffer));
        }
        delete queued;
    }

    if (buffer.GetActiveSize() > 0)
        QueuePacket(std::move(buffer));

    if (!Socket<WorldSocket>::Update())
        return false;

    _queryProcessor.ProcessReadyCallbacks();
    return true;
}

bool WorldSocket::ProcessIncoming()
{
    ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
    uint32 opcode = (header->cmd);

    WorldPacket packet(opcode, std::move(_packetBuffer));

    if (!IsOpen())
        return false;

    // Dump received packet.
    if (m_isPacketLoggingEnabled && packetLog->CanLogPacket())
        packetLog->LogPacket(packet, CLIENT_TO_SERVER);

    try
    {
        switch (opcode)
        {
            case CMSG_PING:
                if (packet.size() != 8)
                {
                    sLog->outError("Client send malformed CMSG_PING (Ip: %s)", GetRemoteIpAddress().to_string().c_str());
                    return false;
                }
                return HandlePing(packet);
            case CMSG_AUTH_SESSION:
                if (m_Session)
                {
                    sLog->outError("WorldSocket::ProcessIncoming: Player send CMSG_AUTH_SESSION again");
                    return false;
                }

                m_authPacket = std::move(packet);
                try
                {
                    if (!HandleAuthHello()) DelayedCloseSocket();
                }
                catch (ByteBufferPositionException const&)
                {
                    sLog->outError("Client send malformed CMSG_AUTH_SESSION (Ip: %s)", GetRemoteIpAddress().to_string().c_str());
                }

                break;
            case CMSG_KEEP_ALIVE:
                break;
            default:
            {
                std::lock_guard sessionGuard(m_SessionLock);

                if (m_Session != nullptr)
                {
                    // Our Idle timer will reset on any non PING opcodes.
                    // Catches people idling on the login screen and any lingering ingame connections.
                    m_Session->ResetTimeOutTime();

                    // OK, give the packet to WorldSession
                    m_Session->QueuePacket(new WorldPacket(std::move(packet)));
                    break;
                }
                else
                {
                    sLog->outError("WorldSocket::ProcessIncoming: Client not authed opcode = %u", uint32(opcode));
                    return false;
                }
            }
        }
    }
    catch (ByteBufferException&)
    {
        sLog->outError("WorldSocket::ProcessIncoming ByteBufferException occured while parsing an instant handled packet (opcode: %u) from client %s, accountid=%i. Disconnected client.", opcode, GetRemoteIpAddress().to_string().c_str(), m_Session ? m_Session->GetAccountId() : -1);
        if (sLog->IsOutDebug())
        {
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Dumping error causing packet:");
            packet.hexlike();
        }

        return false;
    }

    return true;
}

bool WorldSocket::HandleAuthHello()
{
    if (sWorld->IsClosed())
    {
        WorldPacket packet;
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_REJECT);
        SendPacket(packet);

        sLog->outError("WorldSocket::HandleAuthHello: World closed, denying client (%s).", GetRemoteIpAddress().to_string().c_str());
        return false;
    }
    std::string account;
    m_authPacket.read_skip(8); // Client build number and some other uint32
    m_authPacket >> account;
    m_authPacket.read_skip(4); // Another unknown uint32
    if (m_authPacket.size() - m_authPacket.rpos() < 44)
    {
        sLog->outError("CMSG_AUTH_SESSION packet is too short to be legit for (%s).", GetRemoteIpAddress().to_string().c_str());
        return false;
    }

    auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
    stmt->setInt32(0, int32(realmID));
    stmt->setString(1, account);

    _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([this]<typename T0>(T0&& PH1) { HandleAuthAccount(std::forward<T0>(PH1)); }));

    sLog->outDebug(LOG_FILTER_PLAYER_AUTH, "WorldSocket::HandleAuthHello for account (%s) ", account.c_str());
    return true;
}

bool WorldSocket::HandleAuthAccount(PreparedQueryResult result)
{
    // Stop if the account is not found
    if (!result)
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_UNKNOWN_ACCOUNT);
        SendPacket(packet);

        sLog->outError("WorldSocket::HandleAuthAccount: Sent Auth Response (unknown account).");
        return false;
    }

    std::string const& address = GetRemoteIpAddress().to_string();
    if (sWorld->IsClosed())
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_REJECT);
        SendPacket(packet);

        sLog->outError("WorldSocket::HandleAuthAccount: World closed, denying client (%s).", address.c_str());
        return false;
    }

    // Read remaining content of the packet
    uint32 battlegroupID = 0;
    int32 realmId = -1;
    uint32 build = 0;
    std::array<uint8, 4> localChallenge = {};
    uint32 regionID = 0;
    uint64 dosResponse = 0;
    Trinity::Crypto::SHA1::Digest digest = {};

    m_authPacket.read(localChallenge);
    m_authPacket >> regionID;
    m_authPacket >> battlegroupID;
    m_authPacket >> realmId; // realmId from auth_database.realmlist table
    m_authPacket >> dosResponse;
    m_authPacket.read(digest);

    Field* fields = result->Fetch();
    uint32 accountId = fields[0].GetUInt32();
    std::string account = fields[13].GetString();

    if (realmId != realmID)
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(REALM_LIST_REALM_NOT_FOUND);
        SendPacket(packet);

       sLog->outError("WorldSocket::HandleAuthAccount: [Account: %s, AccountId: %d] Missmatch realmID %u vs %u.", account.c_str(), accountId, realmId, realmID);
        return false;
    }

    // Must be done before WorldSession is created
    std::string os = fields[9].GetString();
    if (sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED) && os != "Win" && os != "OSX")
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_REJECT);
        SendPacket(packet);

        sLog->outError("WorldSocket::HandleAuthAccount: Client %s accid %u attempted to log in using invalid client OS (%s).", account.c_str(), accountId, os.c_str());
        return false;
    }

    ::SessionKey sessionKey = fields[1].GetBinary<SESSION_KEY_LENGTH>();

    uint8 t[4] = { 0x00, 0x00, 0x00, 0x00 };

    Trinity::Crypto::SHA1 sha;
    sha.UpdateData(account);
    sha.UpdateData(t);
    sha.UpdateData(localChallenge);
    sha.UpdateData(m_Seed);
    sha.UpdateData(sessionKey);
    sha.Finalize();

    if (sha.GetDigest() != digest)
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_FAILED);
        SendPacket(packet);

        sLog->outError("WorldSocket::HandleAuthAccount: Authentication failed for account: %u  address: %s", accountId, address.c_str());
        return false;
    }

    ///- Re-check ip locking (same check as in authserver).
    if (sWorld->getBoolConfig(CONFIG_STRICT_IPLOCK) || fields[3].GetUInt8() == 1) // ip in database is set by logon server, should never be different from currently used
    {
        std::string lastIP = fields[2].GetString();
        if (lastIP != address)
        {
            WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
            packet << uint8(AUTH_FAILED);
            SendPacket(packet);

            sLog->outBasic("WorldSocket::HandleAuthAccount: Sent Auth Response (Account IP differs for account %u).", accountId);
            return false;
        }
    }

    int32 time_diff = int32(fields[14].GetInt64());
    if (sWorld->getBoolConfig(CONFIG_STRICT_IPLOCK) && time_diff > sWorld->getIntConfig(CONFIG_BAN_CHECK_TIME_DIFFERENCE))
    {
        WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_FAILED);
        SendPacket(packet);

        sLog->outBasic("WorldSocket::HandleAuthAccount: Sent Auth Response (too long time between logon and realm connections for account %u).", accountId);
        return false;
    }

    uint8 security = fields[10].GetUInt8();
    bool skipQueue = security > 0;
    WorldPacket packet;

    uint8 expansion                 = fields[5].GetUInt8();
    int64 mutetime                  = fields[6].GetInt64();
    LocaleConstant locale           = LocaleConstant(fields[7].GetUInt8());
    uint32 recruiter                = fields[8].GetUInt32();

    uint32 world_expansion = sWorld->getIntConfig(CONFIG_EXPANSION);
    if (expansion > world_expansion)
        expansion = world_expansion;
    if (locale >= TOTAL_LOCALES)
        locale = LOCALE_enUS;


    //! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
    if (mutetime < 0)
    {
        mutetime = time(nullptr) - mutetime;
        auto stmt     = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
        stmt->setInt64(0, mutetime);
        stmt->setUInt32(1, accountId);
        LoginDatabase.Execute(stmt);
    }

    bool isRecruiter = false;
    bool hasBoostedCharacter = false;

    bool const isBanned = fields[11].GetUInt64() != 0;
    if (!sWorld->getBoolConfig(CONFIG_STRICT_IPLOCK) && isBanned) // if account banned
    {
        packet.Initialize(SMSG_AUTH_RESPONSE, 1);
        packet << uint8(AUTH_BANNED);
        SendPacket(packet);

        sLog->outError("WorldSocket::HandleAuthSession: Sent Auth Response (Account %u banned).", accountId);
        return false;
    }

    // Check locked state for server
    AccountTypes allowedAccountType = sWorld->GetPlayerSecurityLimit();
    if (AccountTypes(security) < allowedAccountType)
    {
        WorldPacket Packet(SMSG_AUTH_RESPONSE, 1);
        Packet << uint8(AUTH_UNAVAILABLE);

        SendPacket(packet);
        return false;
    }
    sLog->outDebug(LOG_FILTER_PLAYER_AUTH, "Client (%s %u) authenticated successfuly from %s", account.c_str(), accountId, GetRemoteIpAddress().to_string().c_str());

    if (!sWorld->getBoolConfig(CONFIG_STRICT_IPLOCK))
    {
        // Update the last_ip in the database
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_IP_BY_ID);
        stmt->setString(0, GetRemoteIpAddress().to_string());
        stmt->setUInt32(1, accountId);
        LoginDatabase.Execute(stmt);
    }

    // NOTE ATM the socket is single-threaded, have this in mind ...
    m_Session = new WorldSession(accountId, shared_from_this(), AccountTypes(security), expansion, mutetime, locale, recruiter, isRecruiter, skipQueue, hasBoostedCharacter);
    if (!m_Session)
    {
        sLog->outError("[BadAlloc WorldSession] Client %s accid %u  (%s).", account.c_str(), accountId, os.c_str());
        return false;
    }

    m_Crypt.Init(sessionKey);

    {
        PROFILE_SCOPE("ReadAddonsInfo");
        m_Session->ReadAddonsInfo(m_authPacket);
    }

    // Initialize Warden system only if it is enabled by config
    if (sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED))
    {
        PROFILE_SCOPE("InitWarden");
        m_Session->InitWarden(sessionKey, os);
    }

    sWorld->QueueNewSession(m_Session);

    return true;
}

void WorldSocket::StartLoggingPackets(std::string account)
{
    packetLog->Initialize(account);
    m_isPacketLoggingEnabled = true;
}

void WorldSocket::OnClose()
{
    {
        std::lock_guard sessionGuard(m_SessionLock);
        m_Session = nullptr;
    }
}

bool WorldSocket::HandlePing(WorldPacket& recvPacket)
{
    uint32 ping;
    uint32 latency;

    // Get the ping packet content
    recvPacket >> ping;
    recvPacket >> latency;

    if (m_LastPingTime == std::chrono::steady_clock::time_point())
        m_LastPingTime = std::chrono::steady_clock::now(); // for 1st ping
    else
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration diff_time = now - m_LastPingTime;
        m_LastPingTime = now;

        if (diff_time < std::chrono::seconds(27))
        {
            ++m_OverSpeedPings;

            uint32 max_count = sWorld->getIntConfig(CONFIG_MAX_OVERSPEED_PINGS);

            if (max_count && m_OverSpeedPings > max_count)
            {
                std::lock_guard guard(m_SessionLock);

                if (m_Session && AccountMgr::IsPlayerAccount(m_Session->GetSecurity()))
                {
                    Player* _player = m_Session->GetPlayer();
                    sLog->outError("WorldSocket::HandlePing: Player (account: %u, GUID: %u, name: %s) kicked for over-speed pings (address: %s)",
                        m_Session->GetAccountId(),
                        _player ? _player->GetGUIDLow() : 0,
                        _player ? _player->GetName().c_str() : "<none>",
                        GetRemoteIpAddress().to_string().c_str());

                    return false;
                }
            }
        }
        else
            m_OverSpeedPings = 0;
    }

    // critical section
    {
        std::lock_guard guard(m_SessionLock);

        if (m_Session)
        {
            m_Session->SetLatency(latency);
            m_Session->ResetClientTimeDelay();
        }
        else
        {
            sLog->outError("WorldSocket::HandlePing: peer sent CMSG_PING, "
                           "but is not authenticated or got recently kicked, "
                           " address = %s",
                GetRemoteIpAddress().to_string().c_str());
            return false;
        }
    }

    WorldPacket packet(SMSG_PONG, 4);
    packet << ping;
    SendPacket(packet);
    return true;
}
