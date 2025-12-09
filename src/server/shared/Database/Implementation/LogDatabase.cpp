#include "LogDatabase.h"
#include "MySQLPreparedStatement.h"

void LogDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_LOGDATABASE_STATEMENTS);

    // Log tables
    PrepareStatement(LOG_INS_MISC_LOGS, "INSERT INTO log_misc (log_message, date) VALUES (?, now())", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_CHEAT_LOGS, "INSERT INTO log_cheat (log_message, date) VALUES (?, now())", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_STORE_LOGS, "INSERT INTO log_store (log_message, date) VALUES (?, now())", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_REPORT_LOGS, "INSERT INTO log_report (report_type, log_message, date) VALUES (?, ?, now())", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_WHISPER_LOGS, "INSERT INTO log_whisper (log_message, date) VALUES (?, now())", CONNECTION_ASYNC);
    PrepareStatement(LOG_REP_MUTE_LOGS, "REPLACE INTO log_mute (AccountID, CharacterName, MuteReason, MutedBy, DurationMinutes) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOG_SEL_MUTE_LOGS, "SELECT CharacterName, MuteReason, MutedBy, DurationMinutes, UNIX_TIMESTAMP(MuteDate) FROM log_mute WHERE AccountID = ? ORDER BY MuteDate DESC", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_GM_LOGS, "INSERT INTO log_gamemaster (AccountID, GMName, IP, log_message, date) VALUES (?, ?, ?, ?, now())", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_ENCOUNTER_STATS, "INSERT INTO log_encounter_stats "
                                              "(EncounterKey, FinishedAtTimestamp, GroupLeaderCharacterID, GuildID, CreatureID, MapType, EncounterName, Slug, ElapsedTime, EncounterDifficulty, CustomDifficulty, MapID, MapName, AdditionalData) "
                                              " VALUES (?, FROM_UNIXTIME(?, '%Y-%m-%d %H:%i:%s'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOG_INS_ENCOUNTER_PLAYER_STATS, "INSERT INTO log_encounter_player_stats "
                    "(EncounterKey, FinishedAtTimestamp, GroupLeaderCharacterID, GuildID, CharacterID, DamageDonePerSecond, DamageDone, EffectiveDamageDonePerSecond, EffectiveDamageDone, DamageTaken, HealDonePerSecond, HealDone, EffectiveHealDonePerSecond, EffectiveHealDone, HealTaken,"
                    "AverageItemLevel, Class, TalentTree, Race, Gender) "
                                              " VALUES (?, FROM_UNIXTIME(?, '%Y-%m-%d %H:%i:%s'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(LOG_SEL_ENCOUNTER_RECORD, "SELECT ElapsedTime, GroupLeaderCharacterID, GuildID FROM log_encounter_stats WHERE EncounterName LIKE ? AND MapID = ? AND EncounterDifficulty = ? AND CustomDifficulty = ? AND AdditionalData LIKE ? ORDER BY ElapsedTime DESC LIMIT 1", CONNECTION_SYNCH);
}
