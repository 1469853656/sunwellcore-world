
#ifndef __RAACCEPTOR_H__
#define __RAACCEPTOR_H__

#include "SocketMgr.h"
#include "RASocket.h"

class RAAcceptor : public SocketMgr<RASocket>
{
public:
    static RAAcceptor& Instance()
    {
        static RAAcceptor instance;
        return instance;
    }

    bool StartNetwork(std::string const& bindIp, uint16 port, int threadCount = 1) override
    {
        if (!SocketMgr<RASocket>::StartNetwork(bindIp, port, threadCount))
            return false;

        _acceptor->AsyncAcceptWithCallback<&RAAcceptor::OnSocketAccept>();
        return true;
    }

protected:
    NetworkThread<RASocket>* CreateThreads() const override
    {
        return new NetworkThread<RASocket>[1];
    }

    static void OnSocketAccept(tcp::socket&& sock, uint32 threadIndex)
    {
        Instance().OnSocketOpen(std::forward<tcp::socket>(sock), threadIndex);
    }
};

#define sRAAcceptor RAAcceptor::Instance()

#endif
