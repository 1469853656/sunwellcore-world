#ifndef __WORLDSOCKETMGR_H
#define __WORLDSOCKETMGR_H
#include "SocketMgr.h"

class WorldSocket;

class WorldSocketMgr : public SocketMgr<WorldSocket>
{
public:
    bool StartNetwork(std::string const& bindIp, uint16 port, int threadCount = 1) override;
    void StopNetwork() override;
    void OnSocketOpen(tcp::socket&& sock, uint32 thread) override;

    static WorldSocketMgr& Instance();
    int getSendBufferSize() const {return m_SockOutUBuff;};
protected:
    WorldSocketMgr();
    NetworkThread<WorldSocket>* CreateThreads() const override;

    int m_SockOutKBuff;
    int m_SockOutUBuff;
    bool m_UseNoDelay;
};
#define sWorldSocketMgr WorldSocketMgr::Instance()

#endif
