
#include "MMapFactory.h"
#include "MMapManager.h"
#include <set>

namespace MMAP
{
    // ######################## MMapFactory ########################
    // our global singleton copy
    MMapManager *g_MMapManager = nullptr;

    IMMapManager* MMapFactory::createOrGetMMapManager()
    {
        if (g_MMapManager == nullptr)
            g_MMapManager = new MMapManager();

        return g_MMapManager;
    }

    void MMapFactory::clear()
    {
        if (g_MMapManager)
        {
            delete g_MMapManager;
            g_MMapManager = nullptr;
        }
    }
}
