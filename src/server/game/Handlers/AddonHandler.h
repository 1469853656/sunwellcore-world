
#ifndef __ADDONHANDLER_H
#define __ADDONHANDLER_H

#include "Common.h"
#include "Config.h"
#include "Singleton.h"
#include "WorldPacket.h"

class AddonHandler : public Sunwell::Singleton<AddonHandler>
{
    /* Construction */
    friend class Sunwell::Singleton<AddonHandler>;
    AddonHandler();

public:
    ~AddonHandler();
    //build addon packet
    bool BuildAddonPacket(WorldPacket* Source, WorldPacket* Target);
};
#define sAddOnHandler AddonHandler::instance()
#endif
