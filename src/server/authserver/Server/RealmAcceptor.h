
#ifndef __REALMACCEPTOR_H__
#define __REALMACCEPTOR_H__

#include "SocketMgr.h"
#include "AuthSocket.h"

class RealmAcceptor : public SocketMgr<AuthSocket>
{
public:
    static RealmAcceptor& Instance()
    {
        static RealmAcceptor instance;
        return instance;
    }

    bool StartNetwork(std::string const& bindIp, uint16 port, int threadCount = 1) override
    {
        if (!SocketMgr<AuthSocket>::StartNetwork(bindIp, port, threadCount))
            return false;

        _acceptor->AsyncAcceptWithCallback<&RealmAcceptor::OnSocketAccept>();
        return true;
    }

protected:
    NetworkThread<AuthSocket>* CreateThreads() const override
    {
        return new NetworkThread<AuthSocket>[1];
    }

    static void OnSocketAccept(tcp::socket&& sock, uint32 threadIndex)
    {
        Instance().OnSocketOpen(std::forward<tcp::socket>(sock), threadIndex);
    }
};

#define sRealmAcceptor RealmAcceptor::Instance()

#endif
