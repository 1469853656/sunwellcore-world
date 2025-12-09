

#ifndef TRINITY_INSTANCE_DATA_H
#define TRINITY_INSTANCE_DATA_H

#include "ZoneScript.h"
#include "World.h"
#include "ObjectMgr.h"
#include "EncounterStats.h"
//#include "GameObject.h"
//#include "Map.h"

#define OUT_SAVE_INST_DATA ;          //sLog->outDebug(LOG_FILTER_TSCR, "TSCR: Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE ; //sLog->outDebug(LOG_FILTER_TSCR, "TSCR: Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a) ;       //sLog->outDebug(LOG_FILTER_TSCR, "TSCR: Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE ; //sLog->outDebug(LOG_FILTER_TSCR, "TSCR: Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL sLog->outError("TSCR: Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

class Map;
class Unit;
class Player;
class GameObject;
class Creature;
struct MythicMapData;

typedef std::set<GameObject*> DoorSet;
typedef std::set<Creature*> MinionSet;

enum AscendedLootMode : uint16
{
    LOOT_MODE_BOTH_10       = 1,
    LOOT_MODE_NORMAL_10     = 16,
    LOOT_MODE_ASCENDED_10   = 32
};

enum EncounterFrameType
{
    ENCOUNTER_FRAME_ENGAGE            = 0,
    ENCOUNTER_FRAME_DISENGAGE         = 1,
    ENCOUNTER_FRAME_UPDATE_PRIORITY   = 2,
    ENCOUNTER_FRAME_ADD_TIMER         = 3,
    ENCOUNTER_FRAME_ENABLE_OBJECTIVE  = 4,
    ENCOUNTER_FRAME_UPDATE_OBJECTIVE  = 5,
    ENCOUNTER_FRAME_DISABLE_OBJECTIVE = 6,
    ENCOUNTER_FRAME_REFRESH_FRAMES    = 7, // Xinef: can be used to refresh frames after unit was destroyed from client and send back (phase changes)
};

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4,
    TO_BE_DECIDED = 5,
};

enum DoorType
{
    DOOR_TYPE_ROOM       = 0, // Door can open if encounter is not in progress
    DOOR_TYPE_PASSAGE    = 1, // Door can open if encounter is done
    DOOR_TYPE_SPAWN_HOLE = 2, // Door can open if encounter is in progress, typically used for spawning places
    MAX_DOOR_TYPES,
};

enum BoundaryType
{
    BOUNDARY_NONE = 0,
    BOUNDARY_N,
    BOUNDARY_S,
    BOUNDARY_E,
    BOUNDARY_W,
    BOUNDARY_NE,
    BOUNDARY_NW,
    BOUNDARY_SE,
    BOUNDARY_SW,
    BOUNDARY_MAX_X = BOUNDARY_N,
    BOUNDARY_MIN_X = BOUNDARY_S,
    BOUNDARY_MAX_Y = BOUNDARY_W,
    BOUNDARY_MIN_Y = BOUNDARY_E,
};

typedef std::map<BoundaryType, float> BossBoundaryMap;
typedef std::map<uint32 /*type*/, uint64 /*guid*/> GuidMap;
typedef std::map<uint32 /*entry*/, uint32 /*type*/> ObjectInfoMap;
typedef std::set<uint64> CreatureGuidMap;

struct DoorData
{
    uint32 entry, bossId;
    DoorType type;
    uint32 boundary;
};

struct MinionData
{
    uint32 entry, bossId;
};

struct ObjectData
{
    uint32 entry;
    uint32 type;
};

struct BossInfo
{
    BossInfo()
        : state(TO_BE_DECIDED)
    {
    }
    EncounterState state;
    DoorSet door[MAX_DOOR_TYPES];
    MinionSet minion;
    BossBoundaryMap boundary;
};

struct DoorInfo
{
    explicit DoorInfo(BossInfo* _bossInfo, DoorType _type, BoundaryType _boundary)
        : bossInfo(_bossInfo)
        , type(_type)
        , boundary(_boundary)
    {
    }
    BossInfo* bossInfo;
    DoorType type;
    BoundaryType boundary;
};

struct MinionInfo
{
    explicit MinionInfo(BossInfo* _bossInfo)
        : bossInfo(_bossInfo)
    {
    }
    BossInfo* bossInfo;
};

typedef std::multimap<uint32 /*entry*/, DoorInfo> DoorInfoMap;
typedef std::pair<DoorInfoMap::const_iterator, DoorInfoMap::const_iterator> DoorInfoMapBounds;

typedef std::map<uint32 /*entry*/, MinionInfo> MinionInfoMap;

class InstanceScript : public ZoneScript
{
public:
    explicit InstanceScript(Map* map)
        : instance(map)
        , completedEncounters(0)
        , m_mythicDone(false)
        , _isMythic(false)
        , _isMythicTimerRunning(false)
        , _mythicKeyLevel(1)
        , _currentMythicTime(0)
        , _mythicStoneGUID(0)
    {
        _affixes.reserve(5);
        _massResurrectCooldown = std::chrono::system_clock::time_point{};
        _scheduler.CancelAll();
    }

    virtual ~InstanceScript() {}

    Map* instance;

    //On creation, NOT load.
    virtual void Initialize() {}

    // On instance load, exactly ONE of these methods will ALWAYS be called:
    // if we're starting without any saved instance data
    virtual void Create();
    // if we're loading existing instance save data
    virtual void Load(char const* data);

    //When save is needed, this function generates the data
    virtual std::string GetSaveData();

    Creature* GetCreature(uint32 type) const;
    GameObject* GetGameObject(uint32 type) const;
    uint64 GetGuidData(uint32 type) const;
    void LoadObjectData(ObjectData const* creatureData, ObjectData const* gameObjectData);

    void SaveToDB();

    virtual void Update(uint32 diff);

    // Called when a creature/gameobject is added to map or removed from map.
    // Insert/Remove objectguid to dynamic guid store
    virtual void OnCreatureCreate(Creature* creature) override;
    virtual void OnCreatureRemove(Creature* creature) override;

    virtual void OnGameObjectCreate(GameObject* go) override;
    virtual void OnGameObjectRemove(GameObject* go) override;

    //Used by the map's CanEnter function.
    //This is to prevent players from entering during boss encounters.
    virtual bool IsEncounterInProgress() const;

    //Called when a player successfully enters the instance.
    virtual void OnPlayerEnter(Player* /*player*/) { }
    virtual void OnPlayerEnterMythic(Player* /*player*/);
    virtual bool BuildMythicResurrectGossip(Player* /*player*/) { return true; }
    virtual void OnPlayerUseMythicShortcut(uint32 /*shortCutId*/, Player* /*player*/) { }

    virtual void OnPlayerAreaUpdate(Player* /*player*/, uint32 /*oldArea*/, uint32 /*newArea*/) {}

    virtual bool IsResurrectionPrevented() { return false; }
    virtual void HandlePlayerResurrect(Player* /*player*/);

    //Handle open / close objects
    //use HandleGameObject(0, boolen, GO); in OnObjectCreate in instance scripts
    //use HandleGameObject(GUID, boolen, NULL); in any other script
    void HandleGameObject(uint64 guid, bool open, GameObject* go = nullptr);

    //change active state of doors or buttons
    void DoUseDoorOrButton(uint64 guid, uint32 withRestoreTime = 0, bool useAlternativeState = false);

    //Respawns a GO having negative spawntimesecs in gameobject-table
    void DoRespawnGameObject(uint64 guid, uint32 timeToDespawn = MINUTE);

    //sends world state update to all players in instance
    void DoUpdateWorldState(uint32 worldstateId, uint32 worldstateValue);

    // Send Notify to all players in instance
    void DoSendNotifyToInstance(char const* format, ...);

    // Update Achievement Criteria for all players in instance
    void DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1 = 0, uint32 miscValue2 = 0, Unit* unit = nullptr);

    // Start/Stop Timed Achievement Criteria for all players in instance
    void DoStartTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry);
    void DoStopTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry);

    // Remove Auras due to Spell on all players in instance
    void DoRemoveAurasDueToSpellOnPlayers(uint32 spell);
    void DoRemoveAurasDueToSpellOnPlayers(std::initializer_list<uint32> spells);

    // Cast spell on all players in instance
    void DoCastSpellOnPlayers(uint32 spell);
    void DoCastSpellOnRandomPlayer(uint32 spell);

    virtual bool SetBossState(uint32 id, EncounterState state);
    EncounterState GetBossState(uint32 id) const { return id < bosses.size() ? bosses[id].state : TO_BE_DECIDED; }
    BossBoundaryMap const* GetBossBoundary(uint32 id) const { return id < bosses.size() ? &bosses[id].boundary : nullptr; }
    BossInfo const* GetBossInfo(uint32 id) const { return &bosses[id]; }

    bool IsBoostEnabledForInstance() const;
    Optional<Difficulty> IsBoostEnabled() const;
    void IncreaseMeleeDmgPct(Creature* creature, float pct);

    // Achievement criteria additional requirements check
    // NOTE: not use this if same can be checked existed requirement types from AchievementCriteriaRequirementType
    virtual bool CheckAchievementCriteriaMeet(uint32 /*criteria_id*/, Player const* /*source*/, Unit const* /*target*/  = nullptr, uint32 /*miscvalue1*/ = 0);

    // Checks boss requirements (one boss required to kill other)
    virtual bool CheckRequiredBosses(uint32 /*bossId*/, Player const* /*player*/ = nullptr) const { return true; }

    void SetCompletedEncountersMask(uint32 newMask, bool save);

    // Returns completed encounters mask for packets
    uint32 GetCompletedEncounterMask() const { return completedEncounters; }

    void SendEncounterUnit(uint32 type, Unit* unit = nullptr, uint8 param1 = 0, uint8 param2 = 0);

    virtual void FillInitialWorldStates(WorldPacket& /*data*/) {}
    virtual bool CanReportChest(uint32 chestId) const { return true; }

    /** Mythic system **/
    void EnableMythic();
    void SetMythicKeyLevel(uint32 keyLevel, Player* /*player*/);
    inline bool IsMythicDone() const { return m_mythicDone; }
    inline bool IsMythic() const { return _isMythic; }
    inline bool IsMythicTimerRunning() const { return _isMythicTimerRunning; }
    inline uint32 const GetMythicTimeElapsed() { return _currentMythicTime; }
    void ToggleMythicTimers(bool /*enable*/);
    void UpdateMythicTimers(uint32 /*diff*/);
    void HandleMythicPlayerDeath(Player* /*player*/);
    void RewardMythicWithTime(uint32 /*time*/);
    void HandleMythicEventEnd(uint32 /*eventId*/);
    void FinishMythicInstance();
    void HandleMythicAffix(uint32 flag, Player* /*player*/);
    void StartMythicEvent();
    void StartMythicCrystalsEvent();
    void StartMythicLostNpcsEvent();
    void StartMythicAlchemistEvent();
    void SummonMythicEventCrystal(Unit* summoner);
    void SendMythicAlchemistWorldStates(Player* player);
    Mythic::MythicEventInfo& GetMythicEventInfo() { return _mythicEventInfo; }
    TaskScheduler& GetMythicScheduler() { return _mythicScheduler; }

    uint32 const GetMythicKeyLevel() const { return _mythicKeyLevel; }
    std::chrono::system_clock::time_point _massResurrectCooldown;
    void ApplyMassResurrectCooldown();
    bool CanUseMassResurrect(Milliseconds& /*time*/) const;
    TeamId GetTeamIdInInstance() const;

    TeamId GetDominatingInstanceTeamId() const;

    void StartEncounterStatsCounting(uint64 sourceGUID, std::string const& name);
    void FinishEncounterStatsCounting(std::string const& additionalData = "");
    void UpdateEncounterStatsForPlayer(Player const* player, PlayerStatType statType, uint32 value);

    void SetCustomDifficulty(CustomDifficulty difficulty) { _customDifficultyID = difficulty; }
    CustomDifficulty GetCustomDifficulty() const { return _customDifficultyID; }

protected:
    void LoadTrashMapInfoData();
    void AddObject(Creature* obj, bool add);
    void AddObject(GameObject* obj, bool add);
    void AddObject(WorldObject* obj, uint32 type, bool add);
    void SetBossNumber(uint32 number) { bosses.resize(number); }
    void LoadDoorData(DoorData const* data);
    void LoadMinionData(MinionData const* data);

    void AddDoor(GameObject* door, bool add);
    void AddMinion(Creature* minion, bool add);

    void UpdateDoorState(GameObject* door);
    void UpdateMinionState(Creature* minion, EncounterState state);

    std::string LoadBossState(char const* data);
    std::string GetBossSaveData();

    // Instance Load and Save
    void SetHeaders(std::string const& dataHeaders);
    bool ReadSaveDataHeaders(std::istringstream& data);
    void ReadSaveDataBossStates(std::istringstream& data);
    virtual void ReadSaveDataMore(std::istringstream& /*data*/) {}
    void WriteSaveDataHeaders(std::ostringstream& data);
    void WriteSaveDataBossStates(std::ostringstream& data);
    virtual void WriteSaveDataMore(std::ostringstream& /*data*/) {}

private:
    std::vector<char> headers;
    static void LoadObjectData(ObjectData const* creatureData, ObjectInfoMap& objectInfo);
    ObjectInfoMap _creatureInfo;
    ObjectInfoMap _gameObjectInfo;
    std::vector<BossInfo> bosses;
    DoorInfoMap doors;
    MinionInfoMap minions;
    uint32 completedEncounters; // completed encounter mask, bit indexes are DungeonEncounter.dbc boss numbers, used for packets
    GuidMap _objectGuids;
    CreatureGuidMap _creatureGuids;
    CustomDifficulty _customDifficultyID = CustomDifficulty::Disabled;
    // Trash mapping system
    std::multimap<uint32, uint32> _trashMapData; // <bossDataId, trashGUID>

    TaskScheduler _scheduler;

    EncounterStats _currentEncounterStats;

    /** Mythic **/
    bool m_mythicDone;
    bool _isMythic;
    bool _isMythicTimerRunning;
    uint32 _mythicKeyLevel;
    uint32 _currentMythicTime;
    uint64 _mythicStoneGUID;
    std::vector<Mythic::MythicAffixData> _affixes;
    TaskScheduler _mythicScheduler;

    /** Mythic Events **/
    Mythic::MythicEventInfo _mythicEventInfo;

    /** Instance affix stuff **/
    bool _fireDragonScheduled;
    void SetupFireDragonAffix();
    bool _ghoulsScheduled;
    void SetupGhoulsAffix();
    bool _holyBombsScheduled;
    void SetupHolyBombs();
    uint32 _mythicEventId;
};

inline uint16 GetLootModeFor10Man(InstanceScript const* instance)
{
    uint16 lootMode = LOOT_MODE_BOTH_10;
    if (instance->GetCustomDifficulty() == CustomDifficulty::Ascended10)
        lootMode |= LOOT_MODE_ASCENDED_10;
    else
        lootMode |= LOOT_MODE_NORMAL_10;

    return lootMode;
}

template<class AI, class T>
AI* GetInstanceAI(T* obj, char const* scriptName)
{
    if (InstanceMap* instance = obj->GetMap()->ToInstanceMap())
        if (instance->GetInstanceScript())
            if (instance->GetScriptId() == sObjectMgr->GetScriptId(scriptName))
                return new AI(obj);

    return nullptr;
};

template<class AI, class T>
AI* GetInstanceAI(T* obj)
{
    if (InstanceMap* instance = obj->GetMap()->ToInstanceMap())
        if (instance->GetInstanceScript())
            return new AI(obj);

    return nullptr;
};

#endif
