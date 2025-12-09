
#ifndef _AUTHSOCKET_H
#define _AUTHSOCKET_H

#include "Common.h"
#include "BigNumber.h"
#include "Socket.h"
#include "ByteBuffer.h"

using boost::asio::ip::tcp;

struct Realm;

enum eStatus
{
    STATUS_CHALLENGE,
    STATUS_LOGON_PROOF,
    STATUS_AUTHED,
    STATUS_CLOSED
};

enum eAuthCmd
{
    AUTH_LOGON_CHALLENGE                         = 0x00,
    AUTH_LOGON_PROOF                             = 0x01,
    REALM_LIST                                   = 0x10,
};

class AuthSocket;
typedef struct AuthHandler
{
    eStatus status;
    size_t packetSize;
    bool (AuthSocket::*handler)(void);
} AuthHandler;

class AuthSocket: public Socket<AuthSocket>
{
public:
    static std::unordered_map<uint8, AuthHandler> InitHandlers();
    AuthSocket(tcp::socket&& socket);

    void Start() override;
    bool Update() override;
    void SendPacket(ByteBuffer& packet);

protected:
    void ReadHandler() override;
private:
    bool _HandleLogonChallenge();
    bool _HandleLogonProof();
    bool _HandleRealmList();
    void _SetVSFields(const std::string& rI);

    BigNumber N, s, g, v;
    BigNumber b, B;
    BigNumber K;

    eStatus _status;

    std::string _login;

    // Since GetLocaleByName() is _NOT_ bijective, we have to store the locale as a string. Otherwise we can't differ
    // between enUS and enGB, which is important for the patch system
    std::string _localizationName;
    std::string _os;
    std::string _localIp;
    uint16 _build;
    uint8 _expversion;
    AccountTypes _accountSecurityLevel;
};

#endif
