#pragma once
#include "Define.h"
#include "DetourNavMesh.h"
namespace MMAP
{
    constexpr uint32 MMAP_MAGIC{ 0x4d4d4150 }; // 'MMAP'
    constexpr uint32 MMAP_VERSION{ 3 };

    struct MmapTileHeader
    {
        uint32 mmapMagic;
        uint32 dtVersion;
        uint32 mmapVersion;
        uint32 size;
        bool usesLiquids : 1;

        MmapTileHeader() : mmapMagic(MMAP_MAGIC), dtVersion(DT_NAVMESH_VERSION),
            mmapVersion(MMAP_VERSION), size(0), usesLiquids(true) {}
    };
}
