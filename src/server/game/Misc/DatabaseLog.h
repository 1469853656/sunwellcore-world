/*
 * Copyright (C)
 * Copyright (C)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DBLOG_H
#define DBLOG_H

#include "Common.h"
#include "Player.h"
#include "DatabaseEnv.h"
#include "fmt/format.h"
#include "fmt/printf.h"

namespace DBLog {
// STRING
template<typename S, typename... Args>
void OutString(const S& fmt_string, Args... args)
{
    sLog->outString(fmt::format(fmt_string, std::forward<Args>(args)...).c_str());
    // TODO: add table
}

// MISC
template<typename S, typename... Args>
void OutMisc(const S& fmt_string, Args... args)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_MISC_LOGS);
    stmt->setString(0, fmt::format(fmt_string, std::forward<Args>(args)...));
    LogDatabase.AsyncQuery(stmt);
}

// CHEAT
template<typename S, typename... Args>
void OutCheat(const S& fmt_string, Args... args)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_CHEAT_LOGS);
    stmt->setString(0, fmt::format(fmt_string, std::forward<Args>(args)...));
    LogDatabase.AsyncQuery(stmt);
}

// STORE
template<typename S, typename... Args>
void OutStore(const S& fmt_string, Args... args)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_STORE_LOGS);
    stmt->setString(0, fmt::format(fmt_string, std::forward<Args>(args)...));
    LogDatabase.AsyncQuery(stmt);
}

// REPORT
template<typename S, typename... Args>
void OutReport(uint32 const reportType, const S& fmt_string, Args... args)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_REPORT_LOGS);
    stmt->setUInt32(0, reportType);
    stmt->setString(1, fmt::format(fmt_string, std::forward<Args>(args)...));
    LogDatabase.AsyncQuery(stmt);
}

// WHISPER
template<typename S, typename... Args>
void OutWhisper(const S& fmt_string, Args... args)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_WHISPER_LOGS);
    auto reportString       = fmt::format(fmt_string, std::forward<Args>(args)...);
    Trinity::Containers::EraseIf(reportString, [](char const c) { return c == '}' || c == '{'; });
    stmt->setString(0, reportString);
    LogDatabase.AsyncQuery(stmt);
}

// MUTE
inline void OutMute(uint32 const accountID, std::string_view const characterName, std::string_view const muteReason, std::string_view const mutedBy, uint32 const minutesDuration)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_REP_MUTE_LOGS);
    stmt->setUInt32(0, accountID);
    stmt->setString(1, characterName.data());
    std::string muteReasonChr = muteReason.data();
    Trinity::Containers::EraseIf(muteReasonChr, [](char const c) { return c == '}' || c == '{'; });
    stmt->setString(2, muteReasonChr);
    stmt->setString(3, mutedBy.data());
    stmt->setUInt32(4, minutesDuration);
    LogDatabase.Execute(stmt);
}

// GAMEMASTER
inline void OutGameMaster(Player const* gameMaster, std::string const& logMessage)
{
    auto stmt = LogDatabase.GetPreparedStatement(LOG_INS_GM_LOGS);
    stmt->setUInt32(0, gameMaster->GetSession()->GetAccountId());
    stmt->setString(1, gameMaster->GetName());
    stmt->setString(2, gameMaster->GetSession()->GetRemoteAddress());
    stmt->setString(3, logMessage);
    LogDatabase.AsyncQuery(stmt);
}
}

#endif // DBLOG_H
