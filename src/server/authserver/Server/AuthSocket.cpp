
#include <algorithm>
#include <openssl/md5.h>

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "ByteBuffer.h"
#include "Configuration/Config.h"
#include "Log.h"
#include "RealmList.h"
#include "AuthSocket.h"
#include "AuthCodes.h"
#include "SHA1.h"
#include "openssl/crypto.h"

#define ChunkSize 2048


// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some paltform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

typedef struct AUTH_LOGON_CHALLENGE_C
{
    uint8   cmd;
    uint8   error;
    uint16  size;
    uint32  gamename;
    uint8   version1;
    uint8   version2;
    uint8   version3;
    uint16  build;
    uint32  platform;
    uint32  os;
    uint32  country;
    uint32  timezone_bias;
    uint32  ip;
    uint8   I_len;
    uint8   I[1];
} sAuthLogonChallenge_C;

typedef struct AUTH_LOGON_PROOF_C
{
    uint8   cmd;
    uint8   A[32];
    uint8   M1[20];
    uint8   crc_hash[20];
    uint8   number_of_keys;
    uint8   securityFlags;                                  // 0x00-0x04
} sAuthLogonProof_C;

typedef struct AUTH_LOGON_PROOF_S
{
    uint8   cmd;
    uint8   error;
    uint8   M2[20];
    uint32  unk1;
    uint32  unk2;
    uint16  unk3;
} sAuthLogonProof_S;

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some paltform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

#define MINIMAL_REQUEST_SIZE 4
#define REALMLIST_REQUEST_SIZE 5
std::unordered_map<uint8, AuthHandler> AuthSocket::InitHandlers()
{
    std::unordered_map<uint8, AuthHandler> handlers;

    handlers[AUTH_LOGON_CHALLENGE]     = { STATUS_CHALLENGE,   MINIMAL_REQUEST_SIZE, &AuthSocket::_HandleLogonChallenge };
    handlers[AUTH_LOGON_PROOF]         = { STATUS_LOGON_PROOF, sizeof(AUTH_LOGON_PROOF_C), &AuthSocket::_HandleLogonProof };
    handlers[REALM_LIST]               = { STATUS_AUTHED,      REALMLIST_REQUEST_SIZE, &AuthSocket::_HandleRealmList };

    return handlers;
}
std::unordered_map<uint8, AuthHandler> const Handlers = AuthSocket::InitHandlers();

#define AUTH_TOTAL_COMMANDS 8

// Constructor - set the N and g values for SRP6
AuthSocket::AuthSocket(tcp::socket&& socket) :
    Socket(std::move(socket)), _status(STATUS_CHALLENGE), _build(0),
    _expversion(0), _accountSecurityLevel(SEC_PLAYER)
{
    N.SetHexStr("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
    g.SetDword(7);
}

void AuthSocket::Start()
{
    std::string ip_address = GetRemoteIpAddress().to_string();
    sLog->outBasic("Accepted connection from %s", ip_address.c_str());
    AsyncRead();
}

bool AuthSocket::Update()
{
    if (!Socket<AuthSocket>::Update())
        return false;

    return true;
}

void AuthSocket::ReadHandler()
{
    MessageBuffer& packet = GetReadBuffer();
    while (packet.GetActiveSize())
    {
        uint8 cmd = packet.GetReadPointer()[0];
        auto itr = Handlers.find(cmd);
        if (itr == Handlers.end())
        {
            packet.Reset();
            break;
        }

        if (_status != itr->second.status)
        {
            CloseSocket();
            return;
        }

        uint16 size = uint16(itr->second.packetSize);
        if (packet.GetActiveSize() < size)
            break;

        if (cmd == AUTH_LOGON_CHALLENGE)
        {
            sAuthLogonChallenge_C* challenge = reinterpret_cast<sAuthLogonChallenge_C*>(packet.GetReadPointer());
            size += challenge->size;
        }

        if (packet.GetActiveSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            CloseSocket();
            return;
        }

        packet.ReadCompleted(size);
    }

    AsyncRead();
}

// Make the SRP6 calculation from hash in dB
void AuthSocket::_SetVSFields(const std::string& rI)
{
    s.SetRand(32 * 8);

    BigNumber I;
    I.SetHexStr(rI.c_str());

    // In case of leading zeros in the rI hash, restore them
    uint8 mDigest[SHA_DIGEST_LENGTH];
    memset(mDigest, 0, SHA_DIGEST_LENGTH);
    if (I.GetNumBytes() <= SHA_DIGEST_LENGTH)
        memcpy(mDigest, I.AsByteArray().get(), I.GetNumBytes());

    std::reverse(mDigest, mDigest + SHA_DIGEST_LENGTH);

    SHA1Hash sha;
    sha.UpdateData(s.AsByteArray().get(), s.GetNumBytes());
    sha.UpdateData(mDigest, SHA_DIGEST_LENGTH);
    sha.Finalize();
    BigNumber x;
    x.SetBinary(sha.GetDigest(), sha.GetLength());
    v = g.ModExp(x, N);

    // No SQL injection (username escaped)
    char *v_hex, *s_hex;
    v_hex = v.AsHexStr();
    s_hex = s.AsHexStr();

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_VS);
    stmt->setString(0, v_hex);
    stmt->setString(1, s_hex);
    stmt->setString(2, _login);
    LoginDatabase.Execute(stmt);

    OPENSSL_free(v_hex);
    OPENSSL_free(s_hex);
}

// Logon Challenge command handler
bool AuthSocket::_HandleLogonChallenge()
{
    _status = STATUS_CLOSED;
    sAuthLogonChallenge_C *ch = (sAuthLogonChallenge_C*)(GetReadBuffer().GetReadPointer());

    _login = std::string((const char*)ch->I, ch->I_len);

    // BigEndian code, nop in little endian case
    // size already converted
    EndianConvert(ch->gamename);
    EndianConvert(ch->build);
    EndianConvert(ch->platform);
    EndianConvert(ch->os);
    EndianConvert(ch->country);
    EndianConvert(ch->timezone_bias);
    EndianConvert(ch->ip);

    std::stringstream tmpLocalIp;
    tmpLocalIp << (uint32)(ch->ip & 0xff) << "." << (uint32)((ch->ip >> 8) & 0xff) << "." << (uint32)((ch->ip >> 16) & 0xff) << "." << (uint32)((ch->ip >> 24) & 0xff);

    _localIp = tmpLocalIp.str();

    ByteBuffer pkt;

    _build = ch->build;
    _expversion = uint8(AuthHelper::IsPostBCAcceptedClientBuild(_build) ? POST_BC_EXP_FLAG : (AuthHelper::IsPreBCAcceptedClientBuild(_build) ? PRE_BC_EXP_FLAG : NO_VALID_EXP_FLAG));
    _os = (const char*)(&ch->os);

    if (_os.size() > 4)
        return false;

    // Restore string order as its byte order is reversed
    std::reverse(_os.begin(), _os.end());

    pkt << uint8(AUTH_LOGON_CHALLENGE);
    pkt << uint8(0x00);

    // Get the account details from the account table
    // No SQL injection (prepared statement)
    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_LOGONCHALLENGE);
    stmt->setString(0, _login);

    PreparedQueryResult res2 = LoginDatabase.Query(stmt);
    if (!res2)
    {
        pkt << uint8(WOW_FAIL_UNKNOWN_ACCOUNT);
        SendPacket(pkt);
        return true;
    }

    Field* fields = res2->Fetch();
    // Get the password from the account table, upper it, and make the SRP6 calculation
    std::string rI = fields[0].GetString();

    // Don't calculate (v, s) if there are already some in the database
    std::string databaseV = fields[5].GetString();
    std::string databaseS = fields[6].GetString();

    sLog->outDebug(LOG_FILTER_NETWORKIO, "database authentication values: v='%s' s='%s'", databaseV.c_str(), databaseS.c_str());

    // multiply with 2 since bytes are stored as hexstring
    if (databaseV.size() != 32 * 2 || databaseS.size() != 32 * 2)
        _SetVSFields(rI);
    else
    {
        s.SetHexStr(databaseS.c_str());
        v.SetHexStr(databaseV.c_str());
    }

    b.SetRand(19 * 8);
    BigNumber gmod = g.ModExp(b, N);
    B = ((v * 3) + gmod) % N;

    ASSERT(gmod.GetNumBytes() <= 32);

    BigNumber unk3;
    unk3.SetRand(16 * 8);

    // Fill the response packet with the result
    if (AuthHelper::IsAcceptedClientBuild(_build))
        pkt << uint8(WOW_SUCCESS);
    else
        pkt << uint8(WOW_FAIL_VERSION_INVALID);

    // B may be calculated < 32B so we force minimal length to 32B
    pkt.append(B.AsByteArray(32).get(), 32);      // 32 bytes
    pkt << uint8(1);
    pkt.append(g.AsByteArray().get(), 1);
    pkt << uint8(32);
    pkt.append(N.AsByteArray(32).get(), 32);
    pkt.append(s.AsByteArray().get(), s.GetNumBytes());   // 32 bytes
    pkt.append(unk3.AsByteArray(16).get(), 16);
    pkt << uint8(0);            // security flags (0x0...0x04)

    uint8 secLevel = fields[4].GetUInt8();
    _accountSecurityLevel = secLevel <= SEC_ADMINISTRATOR ? AccountTypes(secLevel) : SEC_ADMINISTRATOR;

    _localizationName.resize(4);

    ///- All good, await client's proof
    _status = STATUS_LOGON_PROOF;

    SendPacket(pkt);
    return true;
}

// Logon Proof command handler
bool AuthSocket::_HandleLogonProof()
{
    // Read the packet
    sAuthLogonProof_C *lp = (sAuthLogonProof_C*)(GetReadBuffer().GetReadPointer());
    _status = STATUS_CLOSED;

    // If the client has no valid version
    if (_expversion == NO_VALID_EXP_FLAG)
    {
        // Check if we have the appropriate patch on the disk
        sLog->outDebug(LOG_FILTER_NETWORKIO, "Client with invalid version, patching is not implemented");
        return false;
    }

    // Continue the SRP6 calculation based on data received from the client
    BigNumber A;

    A.SetBinary(lp->A, 32);

    // SRP safeguard: abort if A == 0
    if (A.isZero())
    {
        return false;
    }

    if ((A % N).isZero())
    {
        return false;
    }

    SHA1Hash sha;
    sha.UpdateBigNumbers(&A, &B, NULL);
    sha.Finalize();
    BigNumber u;
    u.SetBinary(sha.GetDigest(), 20);
    BigNumber S = (A * (v.ModExp(u, N))).ModExp(b, N);

    uint8 t[32];
    uint8 t1[16];
    uint8 vK[40];
    memcpy(t, S.AsByteArray(32).get(), 32);

    for (int i = 0; i < 16; ++i)
        t1[i] = t[i * 2];

    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();

    for (int i = 0; i < 20; ++i)
        vK[i * 2] = sha.GetDigest()[i];

    for (int i = 0; i < 16; ++i)
        t1[i] = t[i * 2 + 1];

    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();

    for (int i = 0; i < 20; ++i)
        vK[i * 2 + 1] = sha.GetDigest()[i];

    K.SetBinary(vK, 40);

    uint8 hash[20];

    sha.Initialize();
    sha.UpdateBigNumbers(&N, NULL);
    sha.Finalize();
    memcpy(hash, sha.GetDigest(), 20);
    sha.Initialize();
    sha.UpdateBigNumbers(&g, NULL);
    sha.Finalize();

    for (int i = 0; i < 20; ++i)
        hash[i] ^= sha.GetDigest()[i];

    BigNumber t3;
    t3.SetBinary(hash, 20);

    sha.Initialize();
    sha.UpdateData(_login);
    sha.Finalize();
    uint8 t4[SHA_DIGEST_LENGTH];
    memcpy(t4, sha.GetDigest(), SHA_DIGEST_LENGTH);

    sha.Initialize();
    sha.UpdateBigNumbers(&t3, NULL);
    sha.UpdateData(t4, SHA_DIGEST_LENGTH);
    sha.UpdateBigNumbers(&s, &A, &B, &K, NULL);
    sha.Finalize();
    BigNumber M;
    M.SetBinary(sha.GetDigest(), 20);

    // Check if SRP6 results match (password is correct), else send an error
    if (!memcmp(M.AsByteArray().get(), lp->M1, 20))
    {

        // Update the sessionkey, last_ip, last login time and reset number of failed logins in the account table for this account
        // No SQL injection (escaped user name) and IP address as received by socket
        const char *K_hex = K.AsHexStr();

        PreparedStatement *stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LOGONPROOF);
        stmt->setString(0, K_hex);
        stmt->setString(1, GetRemoteIpAddress().to_string().c_str());
        stmt->setString(2, _localIp.c_str());
        stmt->setUInt32(3, GetLocaleByName(_localizationName));
        stmt->setString(4, _os);
        stmt->setString(5, _login);
        LoginDatabase.DirectExecute(stmt);

        OPENSSL_free((void*)K_hex);

        // Finish SRP6 and send the final result to the client
        sha.Initialize();
        sha.UpdateBigNumbers(&A, &M, &K, NULL);
        sha.Finalize();

        ByteBuffer packet;
        sAuthLogonProof_S proof;
        memcpy(proof.M2, sha.GetDigest(), 20);
        proof.cmd = AUTH_LOGON_PROOF;
        proof.error = 0;
        proof.unk1 = 0x00800000;    // Accountflags. 0x01 = GM, 0x08 = Trial, 0x00800000 = Pro pass (arena tournament)
        proof.unk2 = 0x00;          // SurveyId
        proof.unk3 = 0x00;
        packet.resize(sizeof(proof));
        std::memcpy(packet.contents(), &proof, sizeof(proof));

        ///- Set _status to authed!
        _status = STATUS_AUTHED;
        SendPacket(packet);
    }
    else
    {
        ByteBuffer packet;
        packet << uint8(AUTH_LOGON_PROOF);
        packet << uint8(WOW_FAIL_UNKNOWN_ACCOUNT);
        packet << uint16(0);
        SendPacket(packet);
    }

    return true;
}

// Realm List command handler
bool AuthSocket::_HandleRealmList()
{

    // Get the user id (else close the connection)
    // No SQL injection (prepared statement)
    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
    stmt->setString(0, _login);
    PreparedQueryResult result = LoginDatabase.Query(stmt);
    if (!result)
    {
        return false;
    }

    Field* fields = result->Fetch();
    uint32 id = fields[0].GetUInt32();

    // Update realm list if need
    sRealmList->UpdateIfNeed();


    // Circle through realms in the RealmList and construct the return packet (including # of user characters in each realm)
    ByteBuffer pkt;

    size_t RealmListSize = 0;
    for (RealmList::RealmMap::const_iterator i = sRealmList->begin(); i != sRealmList->end(); ++i)
    {
        const Realm& realm = i->second;
        // don't work with realms which not compatible with the client
        bool okBuild = ((_expversion & POST_BC_EXP_FLAG) && realm.gamebuild == _build) || ((_expversion & PRE_BC_EXP_FLAG) && !AuthHelper::IsPreBCAcceptedClientBuild(realm.gamebuild));

        // No SQL injection. id of realm is controlled by the database.
        uint32 flag                     = realm.flag;
        RealmBuildInfo const* buildInfo = AuthHelper::GetBuildInfo(realm.gamebuild);
        if (!okBuild)
        {
            if (!buildInfo)
                continue;

            flag |= REALM_FLAG_OFFLINE | REALM_FLAG_SPECIFYBUILD; // tell the client what build the realm is for
        }

        if (!buildInfo)
            flag &= ~REALM_FLAG_SPECIFYBUILD;

        std::string name = i->first;
        if (_expversion & PRE_BC_EXP_FLAG && flag & REALM_FLAG_SPECIFYBUILD)
        {
            std::ostringstream ss;
            ss << name << " (" << buildInfo->MajorVersion << '.' << buildInfo->MinorVersion << '.' << buildInfo->BugfixVersion << ')';
            name = ss.str();
        }

        // We don't need the port number from which client connects with but the realm's port

        uint8 lock = (realm.allowedSecurityLevel > _accountSecurityLevel) ? 1 : 0;

        uint8 AmountOfCharacters = 0;
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_NUM_CHARS_ON_REALM);
        stmt->setUInt32(0, realm.m_ID);
        stmt->setUInt32(1, id);
        result = LoginDatabase.Query(stmt);
        if (result)
            AmountOfCharacters = (*result)[0].GetUInt8();

        pkt << realm.icon;                                  // realm type
        if (_expversion & POST_BC_EXP_FLAG)                 // only 2.x and 3.x clients
            pkt << lock;                                    // if 1, then realm locked
        pkt << uint8(flag);                                 // RealmFlags
        pkt << name;
        pkt << realm.Address;
        pkt << realm.populationLevel;
        pkt << AmountOfCharacters;
        pkt << realm.timezone;                              // realm category
        if (_expversion & POST_BC_EXP_FLAG)                 // 2.x and 3.x clients
            pkt << uint8(0x2C);                             // unk, may be realm number/id?
        else
            pkt << uint8(0x0);                              // 1.12.1 and 1.12.2 clients

        if (_expversion & POST_BC_EXP_FLAG && flag & REALM_FLAG_SPECIFYBUILD)
        {
            pkt << uint8(buildInfo->MajorVersion);
            pkt << uint8(buildInfo->MinorVersion);
            pkt << uint8(buildInfo->BugfixVersion);
            pkt << uint16(buildInfo->Build);
        }

        ++RealmListSize;
    }

    if (_expversion & POST_BC_EXP_FLAG) // 2.x and 3.x clients
    {
        pkt << uint8(0x10);
        pkt << uint8(0x00);
    }
    else // 1.12.1 and 1.12.2 clients
    {
        pkt << uint8(0x00);
        pkt << uint8(0x02);
    }

    // make a ByteBuffer which stores the RealmList's size
    ByteBuffer RealmListSizeBuffer;
    RealmListSizeBuffer << uint32(0);
    if (_expversion & POST_BC_EXP_FLAG) // only 2.x and 3.x clients
        RealmListSizeBuffer << uint16(RealmListSize);
    else
        RealmListSizeBuffer << uint32(RealmListSize);

    ByteBuffer hdr;
    hdr << uint8(REALM_LIST);
    hdr << uint16(pkt.size() + RealmListSizeBuffer.size());
    hdr.append(RealmListSizeBuffer);                        // append RealmList's size buffer
    hdr.append(pkt);                                        // append realms in the realmlist


    SendPacket(hdr);
    return true;
}

void AuthSocket::SendPacket(ByteBuffer& packet)
{
    if (!IsOpen())
        return;

    if (!packet.empty())
    {
        MessageBuffer buffer(packet.size());
        buffer.Write(packet.contents(), packet.size());
        QueuePacket(std::move(buffer));
    }
}

