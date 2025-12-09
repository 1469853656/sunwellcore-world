#include "EncounterStats.h"

#include "GuildMgr.h"

void EncounterStats::ResetEncounterStats()
{
    StartTime = {};
    PlayerStats.clear();
    EncounterGUID = 0;
    EncounterName.clear();
}

void EncounterStats::SaveToDB(uint32 groupLeaderCharacterID, uint32 guildID, double elapsedTime, Difficulty difficulty, uint32 customDifficulty, uint32 mapID, uint32 instanceID, uint32 creatureID, uint32 mapType, std::string const& mapName, std::string const& additionalData) const
{
    auto const trans = LogDatabase.BeginTransaction();

    auto const endTimestamp = sWorld->GetGameTime();
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_ENCOUNTER_STATS);
    std::string const encounterKey = fmt::format("{}:{}", instanceID, endTimestamp);
    stmt->setString(0, encounterKey);
    stmt->setInt64(1, endTimestamp);
    stmt->setUInt32(2, groupLeaderCharacterID);
    stmt->setUInt32(3, guildID);
    stmt->setUInt32(4, creatureID);
    stmt->setUInt32(5, mapType);
    stmt->setString(6, EncounterName);

    std::string convertedName = EncounterName;
    std::ranges::transform(convertedName, convertedName.begin(), [](char c)
    {
        if (c == ' ')
            return '_';

        if (!isalpha(c))
            return '\0';

        return static_cast<char>(std::tolower(c));
    });

    stmt->setString(7, convertedName);

    stmt->setDouble(8, elapsedTime);
    stmt->setUInt8(9, static_cast<uint8>(difficulty));
    stmt->setUInt32(10, customDifficulty);
    stmt->setUInt32(11, mapID);
    stmt->setString(12, mapName);
    stmt->setString(13, additionalData);

    trans->Append(stmt);

    for (auto const& [characterID, playerStats] : PlayerStats)
    {
        stmt = LogDatabase.GetPreparedStatement(LOG_INS_ENCOUNTER_PLAYER_STATS);
        stmt->setString(0, encounterKey);
        stmt->setInt64(1, endTimestamp);
        stmt->setUInt32(2, groupLeaderCharacterID);
        stmt->setUInt32(3, guildID);
        stmt->setUInt32(4, characterID);
        stmt->setDouble(5, playerStats.DamageDone / elapsedTime);
        stmt->setUInt32(6, playerStats.DamageDone);
        stmt->setDouble(7, playerStats.EffectiveDamageDone / elapsedTime);
        stmt->setUInt32(8, playerStats.EffectiveDamageDone);
        stmt->setUInt32(9, playerStats.DamageTaken);
        stmt->setDouble(10, playerStats.HealingDone / elapsedTime);
        stmt->setUInt32(11, playerStats.HealingDone);
        stmt->setDouble(12, playerStats.EffectiveHealingDone / elapsedTime);
        stmt->setUInt32(13, playerStats.EffectiveHealingDone);
        stmt->setUInt32(14, playerStats.HealingTaken);
        stmt->setFloat(15, playerStats.AverageItemLevel);
        stmt->setUInt8(16, playerStats.PlayerClass);
        stmt->setUInt32(17, playerStats.PlayerTalentTree);
        stmt->setUInt8(18, playerStats.PlayerRace);
        stmt->setUInt8(19, playerStats.PlayerGender);

        trans->Append(stmt);
    }

    LogDatabase.CommitTransaction(trans);
}

std::optional<EncounterStats::EncounterRecordData> EncounterStats::GetEncounterRecord(std::string const& encounterName, uint32 mapID, uint32 difficulty, uint32 customDifficulty, std::string const& additionalData)
{
    auto const stmt = LogDatabase.GetPreparedStatement(LOG_SEL_ENCOUNTER_RECORD);
    stmt->setString(0, encounterName);
    stmt->setUInt32(1, mapID);
    stmt->setUInt8(2, static_cast<uint8>(difficulty));
    stmt->setUInt32(3, customDifficulty);
    stmt->setString(4, additionalData);
    if (auto const result = LogDatabase.Query(stmt))
    {
        Field const* fields = result->Fetch();
        double const elapsedTime = fields[0].GetDouble();
        uint32 const groupLeaderCharacterID = fields[1].GetUInt32();
        uint32 const guildID = fields[2].GetUInt32();
        EncounterRecordData encounterRecordData;
        if (guildID == 0)
        {
            if (groupLeaderCharacterID == 0)
                return {};

            encounterRecordData.GuildOrGroupLeaderName = sWorld->GetGlobalPlayerName(groupLeaderCharacterID);
            encounterRecordData.IsGuild = false;
        }
        else
        {
            encounterRecordData.GuildOrGroupLeaderName = sGuildMgr->GetGuildNameById(guildID);
            encounterRecordData.IsGuild = true;
        }

        if (encounterRecordData.GuildOrGroupLeaderName.empty())
            return {};

        encounterRecordData.ElapsedTime = elapsedTime;
        return encounterRecordData;
    }

    return {};
}
