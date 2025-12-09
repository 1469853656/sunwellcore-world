

#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "DatabaseEnv.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Log.h"
#include "Map.h"
#include "MythicMgr.hpp"
#include "Player.h"
#include "Pet.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Spell.h"
#include "PoolMgr.h"
#include "Chat.h"
#include "DatabaseLog.h"

#include "fmt/format.h"

#include <numeric>

#include "GuildMgr.h"

Creature* InstanceScript::GetCreature(uint32 type) const
{
    return instance->GetCreature(GetGuidData(type));
}

GameObject* InstanceScript::GetGameObject(uint32 type) const
{
    return instance->GetGameObject(GetGuidData(type));
}

uint64 InstanceScript::GetGuidData(uint32 type) const
{
    GuidMap::const_iterator i = _objectGuids.find(type);
    if (i != _objectGuids.end())
        return i->second;
    return 0;
}

void InstanceScript::LoadObjectData(ObjectData const* creatureData, ObjectData const* gameObjectData)
{
    if (creatureData)
        LoadObjectData(creatureData, _creatureInfo);

    if (gameObjectData)
        LoadObjectData(gameObjectData, _gameObjectInfo);
}

void InstanceScript::LoadObjectData(ObjectData const* data, ObjectInfoMap& objectInfo)
{
    while (data->entry)
    {
        ASSERT(objectInfo.find(data->entry) == objectInfo.end());
        objectInfo[data->entry] = data->type;
        ++data;
    }
}

void InstanceScript::LoadTrashMapInfoData()
{
    if (!instance)
        return;

    auto instanceId = instance->GetId();
    if (auto result = WorldDatabase.PQuery("SELECT BossDataId, TrashGUID FROM instance_trash_map_data WHERE InstanceId='%u'", instanceId))
    {
        do
        {
            Field* fields     = result->Fetch();
            uint32 BossDataId = fields[0].GetUInt32();
            uint32 TrashGUID  = fields[1].GetUInt32();
            _trashMapData.insert(std::make_pair(BossDataId, TrashGUID));
        } while (result->NextRow());
    }
}

void InstanceScript::AddObject(Creature* obj, bool add)
{
    ObjectInfoMap::const_iterator j = _creatureInfo.find(obj->GetEntry());
    if (j != _creatureInfo.end())
        AddObject(obj, j->second, add);
}

void InstanceScript::AddObject(GameObject* obj, bool add)
{
    ObjectInfoMap::const_iterator j = _gameObjectInfo.find(obj->GetEntry());
    if (j != _gameObjectInfo.end())
        AddObject(obj, j->second, add);
}

void InstanceScript::AddObject(WorldObject* obj, uint32 type, bool add)
{
    if (add)
        _objectGuids[type] = obj->GetGUID();
    else
    {
        GuidMap::iterator i = _objectGuids.find(type);
        if (i != _objectGuids.end() && i->second == obj->GetGUID())
            _objectGuids.erase(i);
    }
}

void InstanceScript::OnCreatureCreate(Creature* creature)
{
    if (IsMythic() && creature->GetDBTableGUIDLow() && sPoolMgr->IsPartOfAPool<Creature>(creature->GetDBTableGUIDLow()))
    {
        //! Do not despawn in current loop!!!!!
        creature->DespawnOrUnsummon(3s);
        return;
    }

    _creatureGuids.insert(creature->GetGUID());
    if (IsMythic() && !IsMythicTimerRunning())
        creature->SetMythicLocked(true);

    if (!_trashMapData.empty())
    {
        for (auto itr : _trashMapData)
        {
            if (creature->GetDBTableGUIDLow() == itr.second)
                if (GetBossState(itr.first) == DONE)
                {
                    creature->AddObjectToRemoveList();
                    return;
                }
        }
    }

    AddObject(creature, true);
    AddMinion(creature, true);
}

void InstanceScript::OnCreatureRemove(Creature* creature)
{
    _creatureGuids.erase(creature->GetGUID());

    AddObject(creature, false);
    AddMinion(creature, false);
}

void InstanceScript::OnGameObjectCreate(GameObject* go)
{
    AddObject(go, true);
    AddDoor(go, true);
}

void InstanceScript::OnGameObjectRemove(GameObject* go)
{
    AddObject(go, false);
    AddDoor(go, false);
}

void InstanceScript::SaveToDB()
{
    std::string data = GetSaveData();
    //if (data.empty()) // pussywizard: encounterMask can be updated and theres no reason to not save
    //    return;

    // pussywizard:
    InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(instance->GetInstanceId());
    if (save)
        save->SetInstanceData(data);

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_INSTANCE_SAVE_DATA);
    stmt->setString(0, data);
    stmt->setUInt32(1, to_underlying(_customDifficultyID));
    stmt->setUInt32(2, instance->GetInstanceId());
    CharacterDatabase.Execute(stmt);
}

void InstanceScript::HandleGameObject(uint64 GUID, bool open, GameObject* go)
{
    if (!go)
        go = instance->GetGameObject(GUID);
    if (go)
        go->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

bool InstanceScript::IsEncounterInProgress() const
{
    for (const auto& boss : bosses)
        if (boss.state == IN_PROGRESS)
            return true;

    return false;
}

void InstanceScript::LoadMinionData(const MinionData* data)
{
    while (data->entry)
    {
        if (data->bossId < bosses.size())
            minions.insert(std::make_pair(data->entry, MinionInfo(&bosses[data->bossId])));

        ++data;
    }
}

void InstanceScript::LoadDoorData(const DoorData* data)
{
    while (data->entry)
    {
        if (data->bossId < bosses.size())
            doors.insert(std::make_pair(data->entry, DoorInfo(&bosses[data->bossId], data->type, BoundaryType(data->boundary))));

        ++data;
    }; //sLog->outDebug(LOG_FILTER_TSCR, "InstanceScript::LoadDoorData: " UI64FMTD " doors loaded.", uint64(doors.size()));
}

void InstanceScript::UpdateMinionState(Creature* minion, EncounterState state)
{
    switch (state)
    {
        case NOT_STARTED:
            if (!minion->IsAlive())
                minion->Respawn();
            else if (minion->IsInCombat())
                minion->AI()->EnterEvadeMode();
            break;
        case IN_PROGRESS:
            if (!minion->IsAlive())
                minion->Respawn();
            else if (!minion->GetVictim())
                minion->AI()->DoZoneInCombat(nullptr, 100.0f);
            break;
        default:
            break;
    }
}

void InstanceScript::UpdateDoorState(GameObject* door)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    // xinef: doors can be assigned to few bosses, if any of them demands doors closed - they should be closed (added & operator for assigment)
    bool open = true;
    for (; range.first != range.second && open; ++range.first)
    {
        DoorInfo const& info = range.first->second;
        switch (info.type)
        {
            case DOOR_TYPE_ROOM:
                open &= (info.bossInfo->state != IN_PROGRESS) ? true : false;
                break;
            case DOOR_TYPE_PASSAGE:
                open &= (info.bossInfo->state == DONE) ? true : false;
                break;
            case DOOR_TYPE_SPAWN_HOLE:
                open &= (info.bossInfo->state == IN_PROGRESS) ? true : false;
                break;
            default:
                break;
        }
    }

    door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

void InstanceScript::AddDoor(GameObject* door, bool add)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    for (; range.first != range.second; ++range.first)
    {
        DoorInfo const& data = range.first->second;

        if (add)
        {
            data.bossInfo->door[data.type].insert(door);
            switch (data.boundary)
            {
                default:
                case BOUNDARY_NONE:
                    break;
                case BOUNDARY_N:
                case BOUNDARY_S:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX();
                    break;
                case BOUNDARY_E:
                case BOUNDARY_W:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionY();
                    break;
                case BOUNDARY_NW:
                case BOUNDARY_SE:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() + door->GetPositionY();
                    break;
                case BOUNDARY_NE:
                case BOUNDARY_SW:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() - door->GetPositionY();
                    break;
            }
        }
        else
            data.bossInfo->door[data.type].erase(door);
    }

    if (add)
        UpdateDoorState(door);
}

void InstanceScript::AddMinion(Creature* minion, bool add)
{
    MinionInfoMap::iterator itr = minions.find(minion->GetEntry());
    if (itr == minions.end())
        return;

    if (add)
        itr->second.bossInfo->minion.insert(minion);
    else
        itr->second.bossInfo->minion.erase(minion);
}

bool InstanceScript::SetBossState(uint32 id, EncounterState state)
{
    if (id < bosses.size())
    {
        BossInfo* bossInfo = &bosses[id];
        if (bossInfo->state == TO_BE_DECIDED) // loading
        {
            bossInfo->state = state;
            //sLog->outError("Inialize boss %u state as %u.", id, (uint32)state);
            return false;
        }
        else
        {
            if (bossInfo->state == state)
                return false;

            if (state == DONE)
                for (auto minion : bossInfo->minion)
                    if (minion->isWorldBoss() && minion->IsAlive())
                        return false;

            bossInfo->state = state;
            SaveToDB();
        }

        for (auto& type : bossInfo->door)
            for (auto i : type)
                UpdateDoorState(i);

        for (auto i : bossInfo->minion)
            UpdateMinionState(i, state);

        return true;
    }
    return false;
}

std::string InstanceScript::LoadBossState(const char* data)
{
    if (!data)
        return nullptr;
    std::istringstream loadStream(data);
    uint32 buff;
    uint32 bossId = 0;
    for (auto i = bosses.begin(); i != bosses.end(); ++i, ++bossId)
    {
        loadStream >> buff;
        if (buff < TO_BE_DECIDED)
            SetBossState(bossId, (EncounterState)buff);
    }
    return loadStream.str();
}

std::string InstanceScript::GetBossSaveData()
{
    std::ostringstream saveStream;
    for (auto& boss : bosses)
        saveStream << (uint32)boss.state << ' ';
    return saveStream.str();
}

void InstanceScript::SetHeaders(std::string const& dataHeaders)
{
    for (char header : dataHeaders)
        if (isalpha(header))
            headers.push_back(header);
}

bool InstanceScript::ReadSaveDataHeaders(std::istringstream& data)
{
    for (char header : headers)
    {
        char buff;
        data >> buff;

        if (header != buff)
            return false;
    }

    return true;
}

void InstanceScript::ReadSaveDataBossStates(std::istringstream& data)
{
    uint32 bossId = 0;
    for (auto i = bosses.begin(); i != bosses.end(); ++i, ++bossId)
    {
        uint32 buff;
        data >> buff;
        if (buff == IN_PROGRESS || buff == FAIL || buff == SPECIAL)
            buff = NOT_STARTED;

        if (buff < TO_BE_DECIDED)
            SetBossState(bossId, EncounterState(buff));
    }
}

void InstanceScript::Create()
{
    for (size_t i = 0; i < bosses.size(); ++i)
        SetBossState(i, NOT_STARTED);
}

void InstanceScript::Load(char const* data)
{
    if (!data)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(data);

    std::istringstream loadStream(data);

    if (ReadSaveDataHeaders(loadStream))
    {
        ReadSaveDataBossStates(loadStream);
        ReadSaveDataMore(loadStream);
    }
    else
        OUT_LOAD_INST_DATA_FAIL;

    OUT_LOAD_INST_DATA_COMPLETE;
}

std::string InstanceScript::GetSaveData()
{
    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;

    WriteSaveDataHeaders(saveStream);
    WriteSaveDataBossStates(saveStream);
    WriteSaveDataMore(saveStream);

    OUT_SAVE_INST_DATA_COMPLETE;

    return saveStream.str();
}

void InstanceScript::WriteSaveDataHeaders(std::ostringstream& data)
{
    for (char header : headers)
        data << header << ' ';
}

void InstanceScript::WriteSaveDataBossStates(std::ostringstream& data)
{
    for (BossInfo const& bossInfo : bosses)
        data << uint32(bossInfo.state) << ' ';
}

void InstanceScript::StartEncounterStatsCounting(uint64 sourceGUID, std::string const& name)
{
    if (!sCustomMgr->GetBoolConfig(CONFIG_ENCOUNTER_STATS_ENABLED))
        return;

    if (!sourceGUID)
        return;

    _currentEncounterStats.ResetEncounterStats();
    _currentEncounterStats.EncounterGUID = sourceGUID;
    _currentEncounterStats.EncounterName = name;
    _currentEncounterStats.StartTime = std::chrono::steady_clock::now();
}

std::string GetDifficultyString(InstanceScript const* instance, std::string_view additionalData)
{
    constexpr uint32 NAXXRAMAS = 533;
    constexpr uint32 OBSIDIAN_SANCTUM = 615;
    constexpr uint32 EYE_OF_ETERNITY = 616;

    InstanceMap const* map = instance->instance->ToInstanceMap();
    uint32 const mapID = map->GetId();
    std::string postfix;
    switch (mapID) 
    {
        case NAXXRAMAS:
        case OBSIDIAN_SANCTUM:
        case EYE_OF_ETERNITY:
            postfix = map->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL && instance->GetCustomDifficulty() == CustomDifficulty::Ascended10 ? "Ascended" : "Normal";
            if (mapID == OBSIDIAN_SANCTUM)
                postfix += fmt::format(" {} Drakes", additionalData.front());

            break;
        default:
            postfix = map->IsHeroic() ? "Heroic" : "Normal";
            break;
    }

    return fmt::format("{} {}", map->GetMaxPlayers(), postfix);
}

std::string FormatElapsedTime(double elapsedTime)
{
    return fmt::format("{} minutes and {:.1f} seconds", static_cast<int>(elapsedTime / 60), std::fmod(elapsedTime, 60.0f));
}

void InstanceScript::FinishEncounterStatsCounting(std::string const& additionalData)
{
    if (!sCustomMgr->GetBoolConfig(CONFIG_ENCOUNTER_STATS_ENABLED))
        return;

    if (!_currentEncounterStats.EncounterGUID)
        return;

    _currentEncounterStats.EndTime = std::chrono::steady_clock::now();
    auto const elapsedTime = std::chrono::duration<double, std::ratio<1>>(_currentEncounterStats.EndTime - _currentEncounterStats.StartTime).count();
    Difficulty const encounterDifficulty = instance->GetDifficulty();

    uint32 groupLeaderCharacterID = 0;
    std::string groupLeaderName;
    std::map<uint32 /*guildID*/, uint32 /*playerCount*/> playersByGuildCount;

    for (auto& playerStatsPair : _currentEncounterStats.PlayerStats)
    {
        uint64 const playerGUID = MAKE_NEW_GUID(playerStatsPair.first, 0, HIGHGUID_PLAYER);
        Player const* player = instance->GetPlayer(playerGUID);
        if (!player)
            continue;

        if (groupLeaderName.empty())
        {
            if (Group const* group = player->GetGroup())
            {
                if (group->GetLeaderName())
                {
                    groupLeaderCharacterID = GUID_LOPART(group->GetLeaderGUID());
                    groupLeaderName = group->GetLeaderName();
                }
            }
            else
            {
                if (!sCustomMgr->CheckRealmPhase(RealmPhase::REALM_PHASE_PTR))
                {
                    DBLog::OutCheat("Player {} killed boss without group, cheater?", player->GetName());
                    return;
                }
            }
        }

        if (uint32 const playerGuildID = player->GetGuildId())
            playersByGuildCount[playerGuildID]++;

        auto& playerStats = playerStatsPair.second;
        playerStats.AverageItemLevel = std::roundf(player->GetAverageItemLevel() * 100.0f) / 100.0f;
        playerStats.PlayerClass = player->getClass();
        playerStats.PlayerRace = player->getRace();
        playerStats.PlayerTalentTree = player->GetMostPointsTalentTree();
        playerStats.PlayerGender = player->getGender();
    }

    std::string biggestGuildName;
    uint32 guildID = 0;
    uint32 biggestGuildPlayerCount = 0;
    if (!playersByGuildCount.empty())
    {
        auto const biggestGuild = std::ranges::max_element(playersByGuildCount, [](const auto& a, const auto& b) { return a.second < b.second; });
        if (biggestGuild != playersByGuildCount.end())
        {
            guildID = biggestGuild->first;
            biggestGuildPlayerCount = biggestGuild->second;
        }
    }

    // 10 man requires at least 8 players to count as a guild run
    // 25 man requires at least 20 players to count as a guild run
    uint32 const requiredPlayersForGuildRun = instance->Is25ManRaid() ? 20 : 8;
    if (requiredPlayersForGuildRun > biggestGuildPlayerCount)
        guildID = 0;
    else
        biggestGuildName = sGuildMgr->GetGuildNameById(guildID);

    if (sCustomMgr->GetBoolConfig(CONFIG_ANNOUNCE_ENCOUNTER_STATS_RECORD))
    {
        std::string difficultyString = GetDifficultyString(this, additionalData);
        std::string guildOrLeaderName = !biggestGuildName.empty() ? fmt::format("<{}>", biggestGuildName) : fmt::format("{}'s group", groupLeaderName);
        std::string elapsedTimeString = FormatElapsedTime(elapsedTime);
        std::string announceText = fmt::format("|cffff9933{} ({}) has been slain by |cFFFFD700{}|cffff9933 in {}", _currentEncounterStats.EncounterName, difficultyString, guildOrLeaderName, elapsedTimeString);

        auto oldRecord = EncounterStats::GetEncounterRecord(_currentEncounterStats.EncounterName, instance->GetId(), encounterDifficulty, to_underlying(GetCustomDifficulty()), additionalData);
        if (oldRecord && oldRecord->ElapsedTime > elapsedTime) 
        {
            std::string previousRecordText = fmt::format(" (Previous record: {} by {})", FormatElapsedTime(oldRecord->ElapsedTime), oldRecord->IsGuild ? fmt::format("<{}>", oldRecord->GuildOrGroupLeaderName) : fmt::format("{}'s group", oldRecord->GuildOrGroupLeaderName));
            announceText += previousRecordText;
        }

        sWorld->SendText(announceText.c_str());
    }

    uint32 mapType = MAP_RAID;
    if (MapEntry const* mapEntry = instance->GetEntry())
        mapType = mapEntry->map_type;

    _currentEncounterStats.SaveToDB(groupLeaderCharacterID, guildID, elapsedTime, encounterDifficulty, to_underlying(GetCustomDifficulty()), instance->GetId(), instance->GetInstanceId(), GUID_ENPART(_currentEncounterStats.EncounterGUID), mapType, instance->GetMapName(), additionalData);
}

void InstanceScript::UpdateEncounterStatsForPlayer(Player const* player, PlayerStatType statType, uint32 value)
{
    if (!_currentEncounterStats.EncounterGUID)
        return;

    switch (statType)
    {
        case PlayerStatType::DamageDone:
            _currentEncounterStats.PlayerStats[player->GetGUIDLow()].DamageDone += value;
            break;
        case PlayerStatType::EffectiveDamageDone:
            _currentEncounterStats.PlayerStats[player->GetGUIDLow()].EffectiveDamageDone += value;
            break;
        case PlayerStatType::HealingDone:
            _currentEncounterStats.PlayerStats[player->GetGUIDLow()].HealingDone += value;
            break;
        case PlayerStatType::EffectiveHealingDone:
            _currentEncounterStats.PlayerStats[player->GetGUIDLow()].EffectiveHealingDone += value;
            break;
        case PlayerStatType::DamageTaken:
            _currentEncounterStats.PlayerStats[player->GetGUIDLow()].DamageTaken += value;
            break;
        case PlayerStatType::HealingTaken:
            _currentEncounterStats.PlayerStats[player->GetGUIDLow()].HealingTaken += value;
            break;
    }
}

void InstanceScript::DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
{
    if (!uiGuid)
        return;

    GameObject* go = instance->GetGameObject(uiGuid);

    if (go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (go->getLootState() == GO_READY)
                go->UseDoorOrButton(uiWithRestoreTime, bUseAlternativeState);
            else if (go->getLootState() == GO_ACTIVATED)
                go->ResetDoorOrButton();
        }
        else
            sLog->outError("SD2: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.", go->GetEntry(), go->GetGoType());
    }
}

void InstanceScript::DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn)
{
    if (GameObject* go = instance->GetGameObject(uiGuid))
    {
        //not expect any of these should ever be handled
        if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE || go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON || go->GetGoType() == GAMEOBJECT_TYPE_TRAP)
            return;

        if (go->isSpawned())
            return;

        go->SetRespawnTime(uiTimeToDespawn);
    }
}

void InstanceScript::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        for (const auto& ref : players)
            if (Player* player = ref.GetSource())
                player->SendUpdateWorldState(uiStateId, uiStateData);
    }
    else
        ; //sLog->outDebug(LOG_FILTER_TSCR, "TSCR: DoUpdateWorldState attempt send data but no players in map.");
}

// Send Notify to all players in instance
void InstanceScript::DoSendNotifyToInstance(char const* format, ...)
{
    InstanceMap::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        va_list ap;
        va_start(ap, format);
        char buff[1024];
        vsnprintf(buff, 1024, format, ap);
        va_end(ap);
        for (const auto& i : players)
            if (Player* player = i.GetSource())
                player->GetSession()->SendNotification("%s", buff);
    }
}

// Update Achievement Criteria for all players in instance
void InstanceScript::DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1 /*= 0*/, uint32 miscValue2 /*= 0*/, Unit* unit /*= NULL*/)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();

    for (const auto& i : PlayerList)
        if (Player* player = i.GetSource())
            player->UpdateAchievementCriteria(type, miscValue1, miscValue2, unit);
}

// Start timed achievement for all players in instance
void InstanceScript::DoStartTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();

    for (const auto& i : PlayerList)
        if (Player* player = i.GetSource())
            player->StartTimedAchievement(type, entry);
}

// Stop timed achievement for all players in instance
void InstanceScript::DoStopTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();

    for (const auto& i : PlayerList)
        if (Player* player = i.GetSource())
            player->RemoveTimedAchievement(type, entry);
}

// Remove Auras due to Spell on all players in instance
void InstanceScript::DoRemoveAurasDueToSpellOnPlayers(uint32 spell)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    for (const auto& ref : PlayerList)
    {
        if (Player* player = ref.GetSource())
        {
            player->RemoveAurasDueToSpell(spell);
            if (Pet* pet = player->GetPet())
                pet->RemoveAurasDueToSpell(spell);
        }
    }
}

void InstanceScript::DoRemoveAurasDueToSpellOnPlayers(std::initializer_list<uint32> spells)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    for (const auto& ref : PlayerList)
    {
        if (Player* player = ref.GetSource())
        {
            for (uint32 spell : spells)
            {
                player->RemoveAurasDueToSpell(spell);
                if (Pet* pet = player->GetPet())
                    pet->RemoveAurasDueToSpell(spell);
            }
        }
    }
}

// Cast spell on all players in instance
void InstanceScript::DoCastSpellOnPlayers(uint32 spell)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();

    for (const auto& i : PlayerList)
        if (Player* player = i.GetSource())
            player->CastSpell(player, spell, true);
}

bool InstanceScript::IsBoostEnabledForInstance() const
{
    switch (instance->GetId())
    {
        case 580: // Sunwell Plateau
            return sCustomMgr->GetBoolConfig(CONFIG_SUNWELL_PLATEAU_TIMEWALKING_ENABLED)/* && _customDifficultyID == CustomDifficulty::Timewalking*/;
        case 533: // Naxxramas
            if (instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                return sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED);

            if (instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
                return _customDifficultyID == CustomDifficulty::Ascended10 && sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_10_BOOST_ENABLED);

            break;
        case 615: // Obsidian Sanctum
            if (instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                return sCustomMgr->GetBoolConfig(CONFIG_OBSIDIAN_SANCTUM_25_BOOST_ENABLED);

            if (instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
                return _customDifficultyID == CustomDifficulty::Ascended10 && sCustomMgr->GetBoolConfig(CONFIG_OBSIDIAN_SANCTUM_10_BOOST_ENABLED);

            break;
        case 616: // Eye of Eternity
            if (instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                return sCustomMgr->GetBoolConfig(CONFIG_EYE_OF_ETERNITY_25_BOOST_ENABLED);

            if (instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
                return _customDifficultyID == CustomDifficulty::Ascended10 && sCustomMgr->GetBoolConfig(CONFIG_EYE_OF_ETERNITY_10_BOOST_ENABLED);

            break;
        default:
            break;
    }

    return false;
}

Optional<Difficulty> InstanceScript::IsBoostEnabled() const
{
    Difficulty difficulty = instance->GetDifficulty();
    switch (instance->GetId())
    {
        // Trial of the Crusader
        case 649:
            if (difficulty == RAID_DIFFICULTY_25MAN_HEROIC && sWorld->getBoolConfig(CONFIG_ENABLE_TOGC25_BOOST))
                return difficulty;
            break;
        // Icecrown Citadel
        case 631:
            if ((difficulty == RAID_DIFFICULTY_10MAN_HEROIC || difficulty == RAID_DIFFICULTY_25MAN_HEROIC) && sWorld->getBoolConfig(CONFIG_ENABLE_ICC_BOOST))
                return difficulty;
            break;
    }

    return Optional<Difficulty>();
}

void InstanceScript::IncreaseMeleeDmgPct(Creature* creature, float val)
{
    if (CreatureTemplate const* creaTemplate = sObjectMgr->GetCreatureTemplate(creature->GetEntry()))
    {
        if (CreatureTemplate const* properCreaTemplate = sObjectMgr->GetCreatureTemplate(creaTemplate->DifficultyEntry[creature->GetMap()->GetSpawnMode() - 1]))
        {
            creature->SetFloatValue(UNIT_FIELD_MINDAMAGE, (properCreaTemplate->mindmg * properCreaTemplate->dmg_multiplier) * (1.0f + val));
            creature->SetFloatValue(UNIT_FIELD_MAXDAMAGE, (properCreaTemplate->maxdmg * properCreaTemplate->dmg_multiplier) * (1.0f + val));
        }
    }
}

bool InstanceScript::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= NULL*/, uint32 /*miscvalue1*/ /*= 0*/)
{
    sLog->outError("Achievement system call InstanceScript::CheckAchievementCriteriaMeet but instance script for map %u not have implementation for achievement criteria %u",
        instance->GetId(), criteria_id);
    return false;
}

void InstanceScript::SetCompletedEncountersMask(uint32 newMask, bool save)
{
    if (completedEncounters == newMask)
        return;
    completedEncounters = newMask;
    // pussywizard:
    if (save)
    {
        InstanceSave* iSave = sInstanceSaveMgr->GetInstanceSave(instance->GetInstanceId());
        if (iSave)
            iSave->SetCompletedEncounterMask(completedEncounters);

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_INSTANCE_SAVE_ENCOUNTERMASK);
        stmt->setUInt32(0, completedEncounters);
        stmt->setUInt32(1, instance->GetInstanceId());
        CharacterDatabase.Execute(stmt);
    }
}

void InstanceScript::SendEncounterUnit(uint32 type, Unit* unit /*= NULL*/, uint8 param1 /*= 0*/, uint8 param2 /*= 0*/)
{
    // size of this packet is at most 15 (usually less)
    WorldPacket data(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, 15);
    data << uint32(type);

    switch (type)
    {
        case ENCOUNTER_FRAME_ENGAGE:
        case ENCOUNTER_FRAME_DISENGAGE:
        case ENCOUNTER_FRAME_UPDATE_PRIORITY:
            data.append(unit->GetPackGUID());
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_ADD_TIMER:
        case ENCOUNTER_FRAME_ENABLE_OBJECTIVE:
        case ENCOUNTER_FRAME_DISABLE_OBJECTIVE:
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_UPDATE_OBJECTIVE:
            data << uint8(param1);
            data << uint8(param2);
            break;
        case ENCOUNTER_FRAME_REFRESH_FRAMES:
        default:
            break;
    }

    instance->SendToPlayers(&data);
}

void InstanceScript::DoCastSpellOnRandomPlayer(uint32 spellId)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    std::vector<uint64> _playerGUIDs;
    for (auto const& it : PlayerList)
    {
        if (Player* player = it.GetSource())
            if (player->IsAlive() && !player->IsGameMaster() && !player->IsDuringRemoveFromWorld() &&
                !player->isBeingLoaded() && !player->IsBeingTeleported())
                _playerGUIDs.push_back(player->GetGUID());
    }

    if (_playerGUIDs.empty())
        return;

    uint64 const _playerGUID = Trinity::Containers::SelectRandomContainerElement(_playerGUIDs);
    Player* target           = instance->GetPlayer(_playerGUID);
    if (target)
        target->CastSpell(target, spellId, true);
}

void InstanceScript::Update(uint32 diff)
{
    _scheduler.Update(diff);
    _mythicScheduler.Update(diff);
}

void InstanceScript::HandlePlayerResurrect(Player* player)
{
    //! Reapply avoidance reduction auras on player
    if (IsMythic() && IsMythicTimerRunning() && !player->HasAura(Mythic::SPELL_MYTHIC_BROKEN_BONES_BASE))
    {
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
        player->CastCustomSpell(Mythic::SPELL_MYTHIC_BROKEN_BONES_BASE, val, player, TRIGGERED_FULL_MASK);
    }
}

/** Mythic stuff **/
void InstanceScript::EnableMythic()
{
    if (!_mythicKeyLevel)
        _mythicKeyLevel = 1;

    _isMythicTimerRunning = false;
    _isMythic             = true;
    _fireDragonScheduled  = false;
    _ghoulsScheduled      = false;
    _holyBombsScheduled   = false;
    _mythicScheduler.CancelAll();
    sMythicMgr.GetMythicAffixesForInstance(_affixes);
    _mythicEventId = sMythicMgr.GetCurrentMythicEventId();
}

void InstanceScript::SetMythicKeyLevel(uint32 keyLevel, Player* player)
{
    if (!player || player->isBeingLoaded() || player->IsBeingTeleported() ||
        player->IsDuringRemoveFromWorld() || (player->GetSession() && player->GetSession()->isLogingOut()))
        return;

    _mythicKeyLevel = keyLevel;

    for (uint64 guid : _creatureGuids)
    {
        Creature* creature = instance->GetCreature(guid);
        if (creature && creature->m_mythicScaling && creature->m_mythicScaling->m_canBeRespawned)
        {
            if (!creature->IsAlive())
                creature->Respawn();

            creature->m_mythicScaling->ScaleToKeyLevel();
        }
    }

    auto const& list = instance->GetPlayers();
    for (auto const& ref : list)
    {
        Player* player = ref.GetSource();
        if (!player || player->IsGameMaster() || player->isBeingLoaded() || player->IsDuringRemoveFromWorld()
            || player->IsBeingTeleported())
            continue;

        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
        player->CastCustomSpell(Mythic::SPELL_MYTHIC_BROKEN_BONES_BASE, val, player, TRIGGERED_FULL_MASK);
    }

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MYTHIC_KEY_USED);
    if (stmt)
    {
        stmt->setUInt32(0, player->GetGUID());
        stmt->setUInt32(1, time(nullptr));
        CharacterDatabase.Execute(stmt);
    }
}

void InstanceScript::ToggleMythicTimers(bool state)
{
    _isMythicTimerRunning = state;
    if (state)
    {
        for (uint64 guid : _creatureGuids)
        {
            Creature* creature = instance->GetCreature(guid);
            if (creature)
                creature->SetMythicLocked(false);
        }

        StartMythicEvent();
    }
}

void InstanceScript::UpdateMythicTimers(uint32 diff)
{
    if (!IsMythicTimerRunning())
        return;

    _currentMythicTime += diff;
}

void InstanceScript::OnPlayerEnterMythic(Player* player)
{
    if (!_mythicStoneGUID)
    {
        GameObject* go = instance->SummonGameObject(Mythic::GO_MYTHIC_STONE_ENTRY, sMythicMgr.GetMythicStoneSpawnpositionFor(instance->GetId()), 0.f, 0.f, 0.f, 0.f, 0);
        ASSERT(go);
        _mythicStoneGUID = go->GetGUID();
    }

    if (sMythicMgr.GetCurrentMythicEventId() == Mythic::MythicEventType::MYTHIC_EVENT_ALCHEMIST)
        SendMythicAlchemistWorldStates(player);

    if (IsMythicTimerRunning() && !player->HasAura(62354))
    {
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
        player->CastCustomSpell(62354, val, player, TRIGGERED_FULL_MASK);
    }
}

void InstanceScript::HandleMythicAffix(uint32 flag, Player* player)
{
    if (_affixes.empty())
        return;

    auto const it = std::find_if(_affixes.begin(), _affixes.end(), [this, &flag](Mythic::MythicAffixData& elem) {
        return elem.AffixFlags == flag;
    });

    if (it == _affixes.end())
        return;

    switch (it->AffixId)
    {
        //! Triggered by MYTHIC_PROC_FLAG_INITIALIZE_MYTHIC
        case Mythic::AFFIX_FIRE_DRAGON_WHILE_IN_COMBAT: {
            SetupFireDragonAffix();
            break;
        }
        case Mythic::AFFIX_PERIODIC_SPAWN_GHOULS: {
            SetupGhoulsAffix();
            break;
        }
        case Mythic::AFFIX_HOLY_BOMBS: {
            SetupHolyBombs();
            break;
        }
        default:
            break;
    }
}

constexpr uint32 NPC_MYTHIC_DROGON_ENTRY{ 91700 };
void InstanceScript::SetupFireDragonAffix()
{
    if (!_fireDragonScheduled)
    {
        _fireDragonScheduled = true;
        _mythicScheduler.Schedule(5s, [&](TaskContext func) {
            auto const& pList = instance->GetPlayers();
            if (!IsEncounterInProgress())
            {
                std::vector<Player*> _combatPlayers;
                for (auto const& ref : pList)
                {
                    Player* player = ref.GetSource();
                    if (!player || !player->IsInCombat() || player->IsDuringRemoveFromWorld() || player->isBeingLoaded()
                        || player->IsBeingTeleported())
                        continue;

                    _combatPlayers.push_back(player);
                }

                if (_combatPlayers.empty())
                    func.Repeat(45s);
                else
                {
                    Player* player = Trinity::Containers::SelectRandomContainerElement(_combatPlayers);
                    if (player)
                    {
                        Position pos = player->GetPosition();
                        pos.m_positionZ += 7.5f;
                        player->SummonCreature(NPC_MYTHIC_DROGON_ENTRY, pos);
                        func.Repeat(2min);
                    }
                    else
                        func.Repeat(45s);
                }
            }
            else
                func.Repeat(150s);
        });
    }
}

constexpr uint32 NPC_DRUDGE_GHOUL_MYTHIC{ 91702 };
void InstanceScript::SetupGhoulsAffix()
{
    if (!_ghoulsScheduled)
    {
        _ghoulsScheduled = true;
        _mythicScheduler.Schedule(5s, [&](TaskContext func)
        {
            auto const& pList = instance->GetPlayers();
            if (!IsEncounterInProgress())
            {
                std::vector<Player*> _combatPlayers;
                for (auto const& ref : pList)
                {
                    Player* player = ref.GetSource();
                    if (!player || !player->IsInCombat() || player->IsDuringRemoveFromWorld() || player->isBeingLoaded()
                        || player->IsBeingTeleported())
                        continue;

                    _combatPlayers.push_back(player);
                }

                if (_combatPlayers.empty())
                    func.Repeat(45s);
                else
                {
                    Player* player = Trinity::Containers::SelectRandomContainerElement(_combatPlayers);
                    if (player)
                    {
                        float angle = 0.f;
                        float step  = M_PI / 3;
                        for (uint32 i = 0; i < 3; ++i, angle += step)
                        {
                            Position pos = player->GetPosition();
                            player->MovePositionToFirstCollision(pos, 2.5f, angle);
                            Creature* drudge = player->SummonCreature(NPC_DRUDGE_GHOUL_MYTHIC, pos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                            if (drudge)
                            {
                                drudge->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                                drudge->CastSpell(drudge, 69639, true);
                            }
                        }

                        func.Repeat(2min);
                    }
                    else
                        func.Repeat(45s);
                }
            }
            else
                func.Repeat(150s);
        });
    }
}

constexpr uint32 NPC_HOLY_BOMB_MYTHIC{ 91703 };
void InstanceScript::SetupHolyBombs()
{
    if (!_holyBombsScheduled)
    {
        _holyBombsScheduled = true;
        _mythicScheduler.Schedule(5s, [&](TaskContext func) {
            auto const& pList = instance->GetPlayers();
            if (!IsEncounterInProgress())
            {
                std::vector<Player*> _combatPlayers;
                for (auto const& ref : pList)
                {
                    Player* player = ref.GetSource();
                    if (!player || !player->IsInCombat() || player->IsDuringRemoveFromWorld() || player->isBeingLoaded()
                        || player->IsBeingTeleported())
                        continue;

                    _combatPlayers.push_back(player);
                }

                if (_combatPlayers.empty())
                    func.Repeat(45s);
                else
                {
                    Player* player = Trinity::Containers::SelectRandomContainerElement(_combatPlayers);
                    if (player)
                    {
                        Position pos = player->GetPosition();
                        pos.m_positionZ += 2.0f;
                        Creature* bomb = player->SummonCreature(NPC_HOLY_BOMB_MYTHIC, pos);
                        if (bomb)
                        {
                            bomb->SetPassive();
                            bomb->SetCanFly(true);
                            bomb->SetDisableGravity(true);
                            bomb->SetSelectable(true);
                        }

                        func.Repeat(2min);
                    }
                    else
                        func.Repeat(45s);
                }
            }
            else
                func.Repeat(150s);
        });
    }
}

void InstanceScript::HandleMythicPlayerDeath(Player* victim)
{
    if (!IsMythicTimerRunning())
        return;

    if (!victim)
        return;

    std::string deathString = fmt::format(MYTHIC_WHISPER_PLAYER_DIED, victim->GetName(), (Mythic::MYTHIC_PLAYER_DEATH_TIME_PENALTY / 1000));
    victim->MonsterTextEmote(deathString.c_str(), nullptr, true);

    _currentMythicTime += Mythic::MYTHIC_PLAYER_DEATH_TIME_PENALTY;
    // handle api
}

void InstanceScript::RewardMythicWithTime(uint32 time)
{
    int32 curTime = static_cast<int32>(_currentMythicTime);
    int32 newTime = std::max(0, curTime - static_cast<int32>(time));

    _currentMythicTime = newTime;
}

void InstanceScript::HandleMythicEventEnd(uint32 eventId)
{
    RewardMythicWithTime(10 * MINUTE * IN_MILLISECONDS);
    Player* screamer{ nullptr };
    auto const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        for (const auto& ref : players)
            if (Player* player = ref.GetSource())
                screamer = player;
    }

    if (screamer)
        screamer->MonsterTextEmote("Congratulations! Your total mythic timer has been reduced by 10 minutes!", nullptr, true);
}

void InstanceScript::FinishMythicInstance()
{
    m_mythicDone = true;
    ToggleMythicTimers(false);

    bool _newRecord          = false;
    uint32 _bestInstanceTime = sMythicMgr.GetBestMythicTimeFor(instance->GetId(), _mythicKeyLevel);
    uint32 timeType          = 4; // None
    Mythic::MythicRecordsStore _names;
    _names.reserve(5);

    auto RewardPlayers = [this, &_names](uint32 type, bool isNewRecord)
    {
        auto const& players = instance->GetPlayers();
        for (auto const& ref : players)
        {
            Player* player = ref.GetSource();
            if (player && !player->IsGameMaster() && !player->IsDuringRemoveFromWorld())
            {
                _names.push_back(player->GetName());
                player->AddMythicReward(_mythicKeyLevel, type, _currentMythicTime, isNewRecord, instance->GetId());
            }
        }
    };

    bool _populateSilverBronze = false;
    if (!_bestInstanceTime) // noone finished the instance yet
    {
        _newRecord            = true;
        timeType              = 1; // new best
        _populateSilverBronze = true;
    }
    else
    {
        if (_bestInstanceTime > _currentMythicTime)
        {
            timeType   = 1;
            _newRecord = true;
        }
        else
        {
            uint32 goldBounds   = static_cast<uint32>(_bestInstanceTime * 1.2f);
            uint32 silverBounds = static_cast<uint32>(_bestInstanceTime * 1.5f);
            uint32 bronzeBounds = static_cast<uint32>(_bestInstanceTime * 2.0f);

            uint32 _silverBracketRecord = sMythicMgr.GetSilverBracketTimeFor(instance->GetId(), _mythicKeyLevel);
            uint32 _bronzeBracketRecord = sMythicMgr.GetBronzeBracketTimeFor(instance->GetId(), _mythicKeyLevel);

            //! At this point we're sure record tables are populated and we're not new gold record
            if (goldBounds > _currentMythicTime)
            {
                timeType = 1;
            }
            else if (silverBounds > _currentMythicTime)
            {
                timeType = 2;
                if (_silverBracketRecord > _currentMythicTime)
                    _newRecord = true;
            }
            else if (bronzeBounds > _currentMythicTime)
            {
                timeType = 3;
                if (_bronzeBracketRecord > _currentMythicTime)
                    _newRecord = true;
            }
        }
    }

    RewardPlayers(timeType, _newRecord);

    if (_newRecord)
        sMythicMgr.SaveRecords(instance->GetId(), _mythicKeyLevel, timeType, _currentMythicTime, _names, _populateSilverBronze);
}

void InstanceScript::StartMythicEvent()
{
    switch (sMythicMgr.GetCurrentMythicEventId())
    {
        case Mythic::MythicEventType::MYTHIC_EVENT_CRYSTALS: {
            StartMythicCrystalsEvent();
            break;
        }
        case Mythic::MythicEventType::MYTHIC_EVENT_LOST_NPCS: {
            StartMythicLostNpcsEvent();
            break;
        }
        case Mythic::MythicEventType::MYTHIC_EVENT_ALCHEMIST: {
            StartMythicAlchemistEvent();
            break;
        }
    }
}

void InstanceScript::StartMythicCrystalsEvent()
{
    // 2 crystals for Blood Furnance, 3 for rest
    uint32 size = instance->GetId() == 542 ? 2 : 3;
    Trinity::Containers::RandomResize(_mythicEventInfo.crystalsEvent.CorrectOrder, size);
    Trinity::Containers::RandomShuffle(_mythicEventInfo.crystalsEvent.CorrectOrder);
    if (Creature* npc = instance->SummonCreature(Mythic::MYTHIC_CRYSTALS_NPC_ID, sMythicMgr.GetEventCrystalPosition(instance->GetId(), 4).value()))
        npc->AI()->DoAction(1);
}

void InstanceScript::StartMythicLostNpcsEvent()
{
    Trinity::Containers::RandomShuffle(_mythicEventInfo.lostNpcsEvent.LostNpcs);
    Trinity::Containers::RandomResize(_mythicEventInfo.lostNpcsEvent.LostNpcs, Mythic::MYTHIC_LOST_NPCS_TO_FOUND);
    std::sort(_mythicEventInfo.lostNpcsEvent.LostNpcs.begin(), _mythicEventInfo.lostNpcsEvent.LostNpcs.end());
    instance->SummonCreature(sMythicMgr.GetEventLostNpcEntry(instance->GetId(), 0), sMythicMgr.GetEventLostNpcPosition(instance->GetId(), 6).value());
}

void InstanceScript::StartMythicAlchemistEvent()
{
    for (uint32 i = 0; i < Mythic::MYTHIC_ALCHEMIST_BOOKS_COUNT; ++i)
    {
        for (uint32 j = 0; j < 3; ++j)
        {
            _mythicEventInfo.alchemistEvent.IngredientsCount[i][j] = urand(1, 10);
        }
    }

    std::vector<uint32> numbers(10);
    std::iota(numbers.begin(), numbers.end(), 1); // 1 - 10
    for (uint32 i = 0; i < Mythic::MYTHIC_ALCHEMIST_INGREDIENTS_COUNT; ++i)
    {
        std::vector<uint32> freeNumbers = numbers;
        for (uint32 j = 0; j < 3; ++j)
        {
            // First answer is correct
            uint32 num = 0;
            if (j == 0)
                num = _mythicEventInfo.alchemistEvent.IngredientsCount[i / 3][i % 3];
            else
                num = Trinity::Containers::SelectRandomContainerElement(freeNumbers);

            _mythicEventInfo.alchemistEvent.PossibleAnswers[i][j] = num;
            freeNumbers.erase(std::remove(freeNumbers.begin(), freeNumbers.end(), num), freeNumbers.end());
        }
    }

    auto pos = sMythicMgr.GetEventAlchemistPosition(instance->GetId(), 3);
    if (pos)
    {
        instance->SummonCreature(Mythic::MYTHIC_ALCHEMIST_ENTRY, pos.value());
    }

    for (uint32 i = 0; i < Mythic::MYTHIC_ALCHEMIST_BOOKS_COUNT; ++i)
    {
        auto pos = sMythicMgr.GetEventAlchemistPosition(instance->GetId(), i);
        if (!pos)
            continue;
        instance->SummonGameObject(Mythic::MYTHIC_ALCHEMIST_BOOK_ENTRY + i, pos.value(), 0.f, 0.f, 0.f, 0.f, 0.f);
    }

    _mythicEventInfo.alchemistEvent.initialized = true;

    auto& playerList = instance->GetPlayers();

    for (auto itr = playerList.begin(); itr != playerList.end(); ++itr)
    {
        Player* player = itr->GetSource();

        if (!player)
            continue;

        SendMythicAlchemistWorldStates(player);
    }
}

void InstanceScript::SummonMythicEventCrystal(Unit* summoner)
{
    Mythic::MythicEventInfo::CrystalsEvent& eventInfo = _mythicEventInfo.crystalsEvent;
    if (!summoner || eventInfo.CrystalsSpawned >= Mythic::MYTHIC_CRYSTALS_EVENT_COUNT)
        return;

    // If respawn timer is less than 2 minutes creature was most likely killed by despawn at evade, don't summon crystal in this case
    if (summoner->IsCreature() && summoner->ToCreature()->GetRespawnDelay() < 2 * MINUTE)
        return;

    uint32 gobEntry = eventInfo.GetCorrectGobjectIdAtPosition(eventInfo.CrystalsSpawned++);
    Position pos    = summoner->GetPosition();
    pos.RelocateOffset({ 0.f, 0.f, 5.f });
    if (GameObject* crystal = summoner->SummonGameObject(gobEntry, pos, 0.f, 0.f, 0.f, 0.f, 0.f))
        crystal->AI()->DoAction(1);
}

void InstanceScript::SendMythicAlchemistWorldStates(Player* player)
{
    if (!_mythicEventInfo.alchemistEvent.initialized)
        return;

    uint32 count = 0;
    for (uint32 i = Mythic::MythicEventsWorldStates::MYTHIC_ALCHEMIST_WS_FIRST; i <= Mythic::MythicEventsWorldStates::MYTHIC_ALCHEMIST_WS_LAST; ++i)
    {
        uint32 indexI = count / 3;
        uint32 indexJ = count % 3;
        player->SendUpdateWorldState(i, _mythicEventInfo.alchemistEvent.IngredientsCount[indexI][indexJ]);
        ++count;
    }
}

void InstanceScript::ApplyMassResurrectCooldown()
{
    _massResurrectCooldown = std::chrono::system_clock::now();
}

bool InstanceScript::CanUseMassResurrect(Milliseconds& time) const
{
    if (instance->GetId() == 631 || instance->IsBattlegroundOrArena() || IsEncounterInProgress())
        return false;

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto _timeSince                           = std::chrono::duration_cast<std::chrono::milliseconds>(now - _massResurrectCooldown);
    Milliseconds cooldown                     = instance->IsRaid() ? Milliseconds(25 * MINUTE * IN_MILLISECONDS) : Milliseconds(5 * MINUTE * IN_MILLISECONDS);

    uint32 _left = 0;
    if (_timeSince < cooldown)
        time = cooldown - _timeSince;
    return _timeSince >= cooldown;
}

TeamId InstanceScript::GetTeamIdInInstance() const
{
    TeamId _team                    = TEAM_NEUTRAL;
    Map::PlayerList const& _players = instance->GetPlayers();
    for (const auto& ref : _players)
    {
        Player* player = ref.GetSource();
        if (player && !player->IsGameMaster())
        {
            _team = player->GetTeam();
            break;
        }
    }

    return _team;
}

TeamId InstanceScript::GetDominatingInstanceTeamId() const
{
    uint32 alliancePlayers = 0;
    uint32 hordePlayers    = 0;
    for (auto const& ref : instance->GetPlayers())
    {
        Player const* player = ref.GetSource();
        player->GetTeam(CF_DENY) == TEAM_ALLIANCE ? ++alliancePlayers : ++hordePlayers;
    }

    if (alliancePlayers == 0 && hordePlayers == 0)
        return TEAM_NEUTRAL;

    if (alliancePlayers == hordePlayers)
        return RAND(TEAM_ALLIANCE, TEAM_HORDE);

    return alliancePlayers > hordePlayers ? TEAM_ALLIANCE : TEAM_HORDE;
}

