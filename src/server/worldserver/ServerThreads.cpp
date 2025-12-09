#include "Common.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "WorldSocketMgr.h"
#include "Database/DatabaseEnv.h"
#include "ScriptMgr.h"
#include "BattlegroundMgr.h"
#include "MapManager.h"
#include "Timer.h"
#include "ServerThreads.h"
#include "OutdoorPvPMgr.h"
#include "AvgDiffTracker.h"
#include "AsyncAuctionListing.h"
#include "ChatProcessor.hpp"

#ifdef _WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif
#include "Config.h"
#include "Profiler.h"

#include "easy/profiler.h"

/// Heartbeat for the World
void Sunwell::WorldThread()
{
    uint32 realCurrTime = 0;
    uint32 realPrevTime = getMSTime();

#ifdef WITH_PROFILER
    if (sConfigMgr->GetIntDefault("Profiler.Enabled", 1))
    {
        uint16_t port = sConfigMgr->GetIntDefault("Profiler.Port", profiler::DEFAULT_PORT);
        profiler::startListen(port);
    }
#endif

    sScriptMgr->OnStartup();

    ///- While we have not World::m_stopEvent, update the world
    while (!World::IsStopped())
    {
        PROFILE_SCOPE("WorldRunnable");

        ++World::m_worldLoopCounter;
        realCurrTime = getMSTime();

        uint32 diff = getMSTimeDiff(realPrevTime, realCurrTime);

        sWorld->Update(diff);
        realPrevTime = realCurrTime;

        uint32 executionTimeDiff = getMSTimeDiff(realCurrTime, getMSTime());
        devDiffTracker.Update(executionTimeDiff);
        avgDiffTracker.Update(executionTimeDiff > WORLD_SLEEP_CONST ? executionTimeDiff : WORLD_SLEEP_CONST);

        if (executionTimeDiff < WORLD_SLEEP_CONST)
            std::this_thread::sleep_for(std::chrono::milliseconds(WORLD_SLEEP_CONST - executionTimeDiff));

#ifdef _WIN32
        if (m_ServiceStatus == 0)
            World::StopNow(SHUTDOWN_EXIT_CODE);

        while (m_ServiceStatus == 2)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
    }

    GetChatProcessor().Shutdown();

    sLog->SetLogDB(false);

    sScriptMgr->OnShutdown();

    sWorld->KickAll();         // save and kick all players
    sWorld->OnShutdown();

    // unload battleground templates before different singletons destroyed
    sBattlegroundMgr->DeleteAllBattlegrounds();

    sMapMgr->UnloadAll();         // unload all grids (including locked in memory)
    sObjectAccessor->UnloadAll(); // unload 'i_player2corpse' storage and remove from world
    sScriptMgr->Unload();
    sOutdoorPvPMgr->Die();
}

void Sunwell::AuctionListingThread()
{
    sLog->outString("Starting up Auction House Listing thread...");
    while (!World::IsStopped())
    {
        if (AsyncAuctionListingMgr::IsAuctionListingAllowed())
        {
            PROFILE_SCOPE("AsyncAuctionListing");

            uint32 diff = AsyncAuctionListingMgr::GetDiff();
            AsyncAuctionListingMgr::ResetDiff();

            if (AsyncAuctionListingMgr::GetTempList().size() || AsyncAuctionListingMgr::GetList().size())
            {
                std::lock_guard lg(AsyncAuctionListingMgr::GetLock());

                {
                    std::lock_guard lg(AsyncAuctionListingMgr::GetTempLock());
                    for (std::list<AuctionListItemsDelayEvent>::iterator itr = AsyncAuctionListingMgr::GetTempList().begin(); itr != AsyncAuctionListingMgr::GetTempList().end(); ++itr)
                        AsyncAuctionListingMgr::GetList().push_back((*itr));
                    AsyncAuctionListingMgr::GetTempList().clear();
                }

                for (std::list<AuctionListItemsDelayEvent>::iterator itr = AsyncAuctionListingMgr::GetList().begin(); itr != AsyncAuctionListingMgr::GetList().end(); ++itr)
                {
                    if ((*itr)._msTimer <= diff)
                        (*itr)._msTimer = 0;
                    else
                        (*itr)._msTimer -= diff;
                }

                for (std::list<AuctionListItemsDelayEvent>::iterator itr = AsyncAuctionListingMgr::GetList().begin(); itr != AsyncAuctionListingMgr::GetList().end(); ++itr)
                    if ((*itr)._msTimer == 0)
                    {
                        if ((*itr).Execute())
                            AsyncAuctionListingMgr::GetList().erase(itr);
                        break;
                    }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    sLog->outString("Auction House Listing thread exiting without problems.");
}

void Sunwell::FreezeDetectorThread(uint32 _delayTime)
{
    uint32 _loops=0;
    uint32 _lastChange=0;
    if (!_delayTime)
        return;

    sLog->outString("Starting up anti-freeze thread (%u seconds max stuck time)...", _delayTime/1000);
    while (!World::IsStopped())
    {
        uint32 curtime = getMSTime();
        if (_loops != World::m_worldLoopCounter)
        {
            _lastChange = curtime;
            _loops = World::m_worldLoopCounter;
        }
        else if (getMSTimeDiff(_lastChange, curtime) > _delayTime)
        {
            sLog->outString("World Thread hangs, kicking out server!");
            ASSERT(false);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    sLog->outString("Anti-freeze thread exiting without problems.");
}

