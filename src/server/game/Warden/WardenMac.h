
#ifndef _WARDEN_MAC_H
#define _WARDEN_MAC_H

#include "Cryptography/ARC4.h"
#include <map>
#include "ByteBuffer.h"
#include "Warden.h"

class WorldSession;
class Warden;

class WardenMac : public Warden
{
public:
    WardenMac();
    ~WardenMac();

    void Init(WorldSession* session, ::SessionKey const& k) override;
    ClientWardenModule* GetModuleForClient() override;
    void InitializeModule() override;
    void RequestHash() override;
    void HandleHashResult(ByteBuffer& buff) override;
    void RequestData() override;
    void HandleData(ByteBuffer& buff) override;
};

#endif
