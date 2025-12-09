#pragma once

#include "TaskScheduler.h"

class ChatHandler;

struct HellforgeStatValues
{
    uint32 StatValue;
    float StatVariance;
};
using HellforgeStats = std::unordered_map<uint32, HellforgeStatValues>;

constexpr uint32 MIN_ITEMS_GATHERED_REQUIRED{ 20 };
using GatheringTimeStamp = std::chrono::time_point<std::chrono::steady_clock>;
using GatheringClock     = std::chrono::steady_clock;
struct GatheringPlayerEntry
{
private:
    uint32 gatheredItemsSoFar = 0;

public:
    GatheringTimeStamp firstGatheredItemTime;
    GatheringTimeStamp lastGatheredItemTime;
    void IncrementGatheredItems() { ++gatheredItemsSoFar; }
    uint32 GetTotalGatheredItems() const { return gatheredItemsSoFar; }
    void SetGatheredItemsSoFar(uint32 val) { gatheredItemsSoFar = val; }
};
using PlayersGatheringStore = std::unordered_map<uint64, GatheringPlayerEntry>;

class WorldCache
{
public:
    static WorldCache& GetInstance();

    /*** DUEL RELATED ***/
    //! get auras from config that ought to be checked and removed when duel starts
    void LoadDuelResetAuras();
    std::vector<uint32>& GetAurasToResetAfterDuel();

    /*** HELLFORGE ***/
    //! loads various stats of bosses for custom hellforge instance (hellforge_boss_stats table in world database)
    void LoadHellforgeBossValues();
    void GetStatValue(uint32 statId, HellforgeStatValues& stat);
    HellforgeStats GetStatValues(std::vector<uint32> _statIds);

    /** Currently gathering listing **/
    void AddOrUpdateFishingList(uint64 /*guid*/);
    void ListCurrentFishers(ChatHandler* /*handler*/);

    void AddOrUpdateHerbalismList(uint64 /*guid*/);
    void ListCurrentHerbalists(ChatHandler* /*handler*/);

    void AddOrUpdateMiningList(uint64 /*guid*/);
    void ListCurrentMiners(ChatHandler* /*handler*/);

    /** Generic **/
    void OnWorldUpdate(uint32 diff);

    bool IsWintergraspBattleActive() const;
    void SetWintergraspBattleState(bool /*active*/);
    void LoadAuctionHouseLoggedItems();
    bool ShouldBeLoggedInAuctionHouseLog(uint32 entry);
    void LoadLotteryRewards();
    uint32 GetRandomLotteryReward() const;

    void LoadBoostItems();
    std::vector<uint32> GetBoostItemsForClassAndSpec(uint32 clas, uint32 spec) const { return _boostItems[clas][spec]; }

private:
    WorldCache();

    //! Misc
    std::vector<uint32> _duelResetSpellIDs;
    std::vector<uint32> _auctionHouseLoggedItems;
    HellforgeStats _hellforgeStatValues;
    std::vector<uint32> _lotteryRewards;

    //! Gatherers list stuff
    PlayersGatheringStore _playersCurrentlyFishing;
    PlayersGatheringStore _playersCurrentlyHerbing;
    PlayersGatheringStore _playersCurrentlyMining;

    void AddOrUpdateGatheringList(uint64 /*guid*/, PlayersGatheringStore& /*store*/);
    void ListCurrentGatherers(ChatHandler* /*handler*/, PlayersGatheringStore& /*store*/, std::string /*gathererName*/, std::string /*itemName*/, std::string /*verb*/);
    void UpdateGatherers();
    void UpdateGatherersList(PlayersGatheringStore& /*store*/);

    TaskScheduler scheduler;
    bool _isGathererListLocked;
    bool _isWintergraspWarActive;

    std::vector<uint32> _boostItems[12][4];
protected:
    WorldCache(WorldCache const&) = delete;
    WorldCache(WorldCache&&)      = delete;
    WorldCache& operator=(WorldCache const&) = delete;
    WorldCache& operator=(WorldCache&&) = delete;
};

#define sWorldCache WorldCache::GetInstance()
