#ifndef _SOLOQUEUE_H
#define _SOLOQUEUE_H

#include "ArenaTeam.h"
#include "Singleton.h"

class Battleground;
class BattlegroundQueue;
class ChatHandler;

struct PvPDifficultyEntry;

enum Specializations
{
    SPEC_DPS            = 0,
    SPEC_HEAL           = 1,
    SPEC_TANK           = 2,

    SPEC_DPS_MELEE      = 3,
    SPEC_DPS_RANGED     = 4,

    MAX_SPECES
};

struct SoloQueue
{
    SoloQueue() : arenaTeamId(0), mmrChangeTimer(0), mmrTolerance(0), talentsSpecialization(0), playerClass(0), joinTime(0), ownerGUID(0) { }

    uint32 arenaTeamId;
    uint64 ownerGUID;
    uint32 mmrChangeTimer;
    uint32 mmrTolerance;
    uint32 talentsSpecialization;
    uint8 playerClass;
    uint32 joinTime;

    void UpdateMMRTolerance(uint32 diff);

    void SetLastDPSList(std::set<uint64> const& list);
    std::set<uint64> const& GetLastDPSList() const;

private:
    std::set<uint64> lastDPSList;
};

struct OptimizedSoloQueue
{
    OptimizedSoloQueue() : averageMMRRating(0), averageRating(0), leaderArenaTeamID(0), playerGUID(0) { }

    std::vector<SoloQueue> acceptedList;
    uint32 averageMMRRating;
    uint32 averageRating;
    uint64 playerGUID;
    uint32 leaderArenaTeamID;

    void SetLastDPSList(std::set<uint64> const& list);
    std::set<uint64> const& GetLastDPSList() const;

private:
    std::set<uint64> lastDPSList;

};

class SoloQueueMgr : public Sunwell::Singleton<SoloQueueMgr>
{
    friend struct Sunwell::Singleton<SoloQueueMgr>;

    public:
        SoloQueueMgr();
        ~SoloQueueMgr() { }

        typedef std::unordered_map<uint32, SoloQueue> SoloQueuePreparationQueue;

        bool AddToPreparationQueue(Player* player, Battleground* bg, BattlegroundQueue* bgQueue, PvPDifficultyEntry const* bracketEntry, uint32 joinTime = 0);
        void RemoveFromPreparationQueue(Player* player, bool castDeserter = false);
        bool IsInPreparationQueue(Player* player);
        SoloQueue const* GetSoloQueue(Player* player) const;

        void Update(uint32 diff);

        bool SelectRandomSoloQueues(SoloQueue& soloQueue, std::vector<std::pair<uint32, SoloQueue>> const& acceptedList, std::vector<SoloQueue>& newList, std::set<uint64>& currentDPSClassMask);

        static uint32 CalculateAverageMMR(std::vector<SoloQueue> const& list);
        static uint32 CalculateAverageRating(std::vector<SoloQueue> const& list);
        static uint32 CalculateAverageJoinTime(std::vector<SoloQueue> const& list);

        void PrintSoloQueueStats(ChatHandler* handler);

        static uint32 GetTalentsSpecialization(Player* player);

    private:
        SoloQueuePreparationQueue _preparationQueue;
        uint32 _updateQueuesTimer;
};

#define sSoloQueueMgr SoloQueueMgr::instance()

#endif
