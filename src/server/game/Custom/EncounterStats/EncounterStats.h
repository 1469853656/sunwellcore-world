#ifndef ENCOUNTER_STATS_H
#define ENCOUNTER_STATS_H

#include "Containers.h"
#include "Singleton.h"
#include "Timer.h"

enum class PlayerStatType : uint8
{
    DamageDone,
    EffectiveDamageDone,
    HealingDone,
    EffectiveHealingDone,
    DamageTaken,
    HealingTaken
};

struct EncounterStats
{
    struct PlayerStats
    {
        uint32 DamageDone = 0;
        uint32 EffectiveDamageDone = 0;
        uint32 HealingDone = 0;
        uint32 EffectiveHealingDone = 0;
        uint32 DamageTaken = 0;
        uint32 HealingTaken = 0;

        float AverageItemLevel = 0.0f;
        uint8 PlayerClass = 0;
        uint32 PlayerTalentTree = 0;
        uint8 PlayerRace = 0;
        uint8 PlayerGender = 0;
    };

    struct EncounterRecordData
    {
        double ElapsedTime = 0.0;
        bool IsGuild = false;
        std::string GuildOrGroupLeaderName;
    };

    void ResetEncounterStats();
    void SaveToDB(uint32 groupLeaderCharacterID, uint32 guildID, double elapsedTime, Difficulty difficulty, uint32 customDifficulty, uint32 mapID, uint32 instanceID, uint32 creatureID, uint32 mapType, std::string const& mapName, std::string const& additionalData) const;
    static std::optional<EncounterRecordData> GetEncounterRecord(std::string const& encounterName, uint32 mapID, uint32 difficulty, uint32 customDifficulty, std::string const& additionalData);

    std::chrono::steady_clock::time_point StartTime = {};
    std::chrono::steady_clock::time_point EndTime = {};
    std::unordered_map<uint32, PlayerStats> PlayerStats;
    uint64 EncounterGUID = 0; // Do we need this?
    std::string EncounterName;
};

#endif
