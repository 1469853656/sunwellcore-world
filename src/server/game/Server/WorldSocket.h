#ifndef _WORLDSOCKET_H
#define _WORLDSOCKET_H

#include "AuthCrypt.h"
#include "AsyncCallbackProcessor.h"
#include "Socket.h"
#include "PacketLog.h"
#include "PreparedStatement.h"
#include "QueryHolder.h"
#include "WorldPacket.h"
#include "MPSCQueue.hpp"
#include "Cryptography/CryptoHash.h"
#include "Cryptography/HMAC.h"
#include "ByteBuffer.h"
#include <memory>

#include "World.h"

class WorldSession;
class AuthQueryHolder;

class WorldPacketNode : public WorldPacket
{
public:
    WorldPacketNode(WorldPacket p) : WorldPacket(p)
    {
        link.store(nullptr, std::memory_order_relaxed);
    }
    std::atomic<WorldPacketNode*> link;
};

using PacketQueue = MPSCQueueIntrusive<WorldPacketNode, &WorldPacketNode::link>;

class WorldSocket : public Socket<WorldSocket>
{
public:
    WorldSocket(tcp::socket&& socket);
    ~WorldSocket();

    void SendPacket(WorldPacket const& pct);
    void Start() override;
    bool Update() override;

protected:
    void OnClose() override;
    void ReadHandler() override;

public:

    bool HandleAuthHello();
    bool HandleAuthAccount(PreparedQueryResult result);
    bool ReadHeaderHandler();
    inline bool IsPacketLoggingEnabled() const { return m_isPacketLoggingEnabled; }
    void StartLoggingPackets(std::string account);

    void SetSendBufferSize(size_t size) {m_OutBufferSize = size;}

    WorldSession* GetSession() const
    {
        return m_Session;
    }

private:
    bool ProcessIncoming();

    bool HandlePing(WorldPacket& recvPacket);

private:
    std::chrono::steady_clock::time_point m_LastPingTime;
    /// Keep track of over-speed pings, to prevent ping flood.
    uint32 m_OverSpeedPings;

    std::mutex m_SessionLock;
    WorldSession* m_Session;

    bool m_isPacketLoggingEnabled;
    std::unique_ptr<PacketLog> packetLog;

    MessageBuffer _headerBuffer;
    MessageBuffer _packetBuffer;
    size_t m_OutBufferSize;
    PacketQueue m_bufferQueue;

    AuthCrypt m_Crypt;
    std::array<uint8, 4> m_Seed;
    WorldPacket m_authPacket;

    QueryCallbackProcessor _queryProcessor;
};

#endif /* _WORLDSOCKET_H */

