

#ifndef _IMMAPMANAGER_H
#define _IMMAPMANAGER_H

#include <string>
#include "DetourNavMesh.h"
#include "Define.h"
#include <shared_mutex>

// Interface for IMMapManger
namespace MMAP {
    class IMMapManager
    {
        public:
            IMMapManager() {}
            virtual ~IMMapManager(void) {}

            virtual bool loadMap(uint32 mapId, int32 x, int32 y) {return false;};
            virtual bool unloadMap(uint32 mapId, int32 x, int32 y) {return false;};
            virtual bool unloadMapInstance(uint32 mapId, uint32 instanceId) {return false;};
            virtual dtNavMesh const* GetNavMesh(uint32 mapId) =0;
            virtual std::shared_mutex& GetMMapLock(uint32 mapId) =0;
            virtual std::shared_mutex& GetManagerLock() =0;
    };
}

#endif
