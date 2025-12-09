
#ifndef _MMAP_MANAGER_H
#define _MMAP_MANAGER_H

#include "Define.h"
#include "DetourAlloc.h"
#include "DetourNavMeshQuery.h"
#include "IMMAPManager.h"

#include <unordered_map>

//  move map related classes
namespace MMAP
{
    typedef std::unordered_map<uint32, dtTileRef> MMapTileSet;
    typedef std::unordered_map<uint32, dtNavMeshQuery*> NavMeshQuerySet;

    // dummy struct to hold map's mmap data
    struct MMapData
    {
        MMapData(dtNavMesh* mesh) : navMesh(mesh) {}
        ~MMapData()
        {
            for (NavMeshQuerySet::iterator i = navMeshQueries.begin(); i != navMeshQueries.end(); ++i)
                dtFreeNavMeshQuery(i->second);

            if (navMesh)
                dtFreeNavMesh(navMesh);
        }

        dtNavMesh* navMesh;

        // we have to use single dtNavMeshQuery for every instance, since those are not thread safe
        NavMeshQuerySet navMeshQueries;     // instanceId to query
        MMapTileSet mmapLoadedTiles;        // maps [map grid coords] to [dtTile]
    };


    typedef std::unordered_map<uint32, MMapData*> MMapDataSet;

    // singleton class
    // holds all all access to mmap loading unloading and meshes
    class MMapManager : public IMMapManager
    {
        public:
            MMapManager();
            ~MMapManager();

            bool loadMap(uint32 mapId, int32 x, int32 y);
            bool unloadMap(uint32 mapId, int32 x, int32 y);
            bool unloadMapInstance(uint32 mapId, uint32 instanceId);
            dtNavMesh const* GetNavMesh(uint32 mapId);

            std::shared_mutex& GetMMapLock(uint32 mapId);
            std::shared_mutex& GetManagerLock() { return MMapManagerLock; }
        private:
            uint32 getLoadedTilesCount() const { return loadedTiles; }
            uint32 getLoadedMapsCount() const { return loadedMMaps.size(); }
            bool loadMapData(uint32 mapId);
            uint32 packTileID(int32 x, int32 y);

            MMapDataSet loadedMMaps;
            uint32 loadedTiles;
            std::string m_dataDir;
            std::unordered_map<uint32, std::shared_mutex> m_mapLocks;

            std::shared_mutex MMapManagerLock;
    };
}

#endif
