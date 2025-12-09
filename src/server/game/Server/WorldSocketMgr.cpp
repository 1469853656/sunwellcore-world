#include "WorldSocketMgr.h"
#include "WorldSocket.h"
#include "ScriptMgr.h"
#include "NetworkThread.h"

//Configuration subdirectory withing include is needed because windows is case insensitive and includes pcre2 config.h
#include "Configuration/Config.h"

class WorldSocketThread : public NetworkThread<WorldSocket>
{
public:
    void SocketAdded(std::shared_ptr<WorldSocket> sock) override
    {
        sock->SetSendBufferSize(sWorldSocketMgr.getSendBufferSize());
        sScriptMgr->OnSocketOpen(sock.get());
    }

    void SocketRemoved(std::shared_ptr<WorldSocket> sock) override
    {
        sScriptMgr->OnSocketClose(sock.get());
    }
};

WorldSocketMgr::WorldSocketMgr()
    : m_SockOutKBuff(-1)
    , m_SockOutUBuff(65536)
    , m_UseNoDelay(true)
{
}

WorldSocketMgr& WorldSocketMgr::Instance()
{
    static WorldSocketMgr instance;
    return instance;
}

static void SocketAcceptCallback(tcp::socket&& sock, uint32 thread)
{
    sWorldSocketMgr.OnSocketOpen(std::forward<tcp::socket>(sock), thread);
}

bool WorldSocketMgr::StartNetwork(std::string const& bindIp, uint16 port, int threadCount)
{
    m_UseNoDelay = sConfigMgr->GetBoolDefault("Network.TcpNodelay", true);

    // -1 means use default
    m_SockOutKBuff = sConfigMgr->GetIntDefault("Network.OutKBuff", -1);
    m_SockOutUBuff = sConfigMgr->GetIntDefault("Network.OutUBuff", 65536);

    if (m_SockOutUBuff <= 0)
    {
        sLog->outError("Network.OutUBuff is wrong in your config file");
        return false;
    }

    if (!SocketMgr<WorldSocket>::StartNetwork(bindIp, port, threadCount))
        return false;

    _acceptor->AsyncAcceptWithCallback<&SocketAcceptCallback>();
    sScriptMgr->OnNetworkStart();

    return true;
}

void WorldSocketMgr::StopNetwork()
{
    SocketMgr<WorldSocket>::StopNetwork();
    sScriptMgr->OnNetworkStop();
}

void WorldSocketMgr::OnSocketOpen(tcp::socket&& sock, uint32 thread)
{
    if (m_SockOutKBuff >= 0)
    {
        boost::system::error_code err;
        sock.set_option(boost::asio::socket_base::send_buffer_size(m_SockOutKBuff), err);
        if (err && err != boost::system::errc::not_supported)
        {
            sLog->outError("WorldSocketMgr::OnSocketOpen sock.set_option(boost::asio::socket_base::send_buffer_size) err = %s", err.message().c_str());
            return;
        }
    }

    if (m_UseNoDelay)
    {
        boost::system::error_code err;
        sock.set_option(tcp::no_delay(true), err);
        if (err)
        {
            sLog->outError("WorldSocketMgr::OnSocketOpen sock.set_option(boost::asio::ip::tcp::no_delay) err = %s", err.message().c_str());
            return;
        }
    }

    SocketMgr<WorldSocket>::OnSocketOpen(std::forward<tcp::socket>(sock), thread);
}

NetworkThread<WorldSocket>* WorldSocketMgr::CreateThreads() const
{
    return new WorldSocketThread[GetNetworkThreadCount()];
}
