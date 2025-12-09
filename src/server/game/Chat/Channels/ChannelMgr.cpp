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

#include "ChannelMgr.h"
#include "Player.h"

#include <fmt/format.h>

void ChannelMgr::LoadChannels()
{
    uint32 const oldMSTime   = getMSTime();
    uint32 count             = 0;
    uint32 hordeCount        = 0;
    uint32 allianceCount     = 0;
    uint32 crossfactionCount = 0;
    //          0       1     2      3          4
    QueryResult const result = CharacterDatabase.PQuery("SELECT channelId, name, team, announce, password FROM channels ORDER BY channelId ASC");
    if (!result)
    {
        sLog->outString(">> Loaded 0 channels");
        sLog->outString();
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        if (!fields)
            break;

        uint32 channelDBId      = fields[0].GetUInt32();
        std::string channelName = fields[1].GetString();
        std::wstring channelWName;
        if (!Utf8toWStr(channelName, channelWName))
            continue;

        auto channelTeamID = static_cast<TeamId>(fields[2].GetUInt32());
        auto newChannel    = std::make_unique<Channel>(channelName, 0, channelDBId, channelTeamID, fields[3].GetUInt8());
        newChannel->SetPassword(fields[4].GetString());
        //                                                                         0          1
        if (QueryResult const banResult = CharacterDatabase.PQuery("SELECT playerGUID, banTime FROM channels_bans WHERE channelId = %u", channelDBId))
        {
            do
            {
                Field* banFields = banResult->Fetch();
                if (!banFields)
                    break;

                newChannel->AddBan(banFields[0].GetUInt32(), banFields[1].GetUInt32());
            } while (banResult->NextRow());
        }

        _channels.insert({ channelWName, std::move(newChannel) });
        switch (channelTeamID)
        {
            case TEAM_ALLIANCE:
                ++allianceCount;
                break;
            case TEAM_HORDE:
                ++hordeCount;
                break;
            case TEAM_NEUTRAL:
                ++crossfactionCount;
                break;
        }

        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u channels (%u horde, %u alliance, %u crossfaction) in %ums", count, hordeCount, allianceCount, crossfactionCount, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

ChannelMgr* ChannelMgr::instance()
{
    static ChannelMgr instance;
    return &instance;
}

Channel* ChannelMgr::GetJoinChannel(std::string const& name, uint32 const channelID, TeamId const teamID)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return nullptr;

    wstrToLower(wname);
    for (ChannelInfo const& cInfo : _channels)
        if (cInfo.Name == wname && cInfo.ChannelPtr)
            if (cInfo.ChannelPtr->GetTeamID() == TEAM_NEUTRAL || cInfo.ChannelPtr->GetTeamID() == teamID)
                return cInfo.ChannelPtr.get();

    std::string chNameLower = name;
    std::transform(chNameLower.begin(), chNameLower.end(), chNameLower.begin(), ::tolower);

    auto newChannel     = std::make_unique<Channel>(chNameLower, channelID, 0, teamID);
    Channel* channelPtr = newChannel.get();
    _channels.insert({ wname, std::move(newChannel) });
    return channelPtr;
}

Channel* ChannelMgr::GetChannel(std::string const& name, Player const* player, bool const pkt) const
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return nullptr;

    wstrToLower(wname);
    for (ChannelInfo const& cInfo : _channels)
        if (cInfo.Name == wname && cInfo.ChannelPtr)
            if (cInfo.ChannelPtr->GetTeamID() == TEAM_NEUTRAL || cInfo.ChannelPtr->GetTeamID() == player->GetTeam(CF_DENY))
                return cInfo.ChannelPtr.get();

    if (pkt)
    {
        WorldPacket data(SMSG_CHANNEL_NOTIFY, 1 + name.size());
        data << static_cast<uint8>(CHAT_NOT_MEMBER_NOTICE) << name;
        player->GetSession()->SendPacket(&data);
    }

    return nullptr;
}

void ChannelMgr::LoadChannelRights()
{
    uint32 const oldMSTime = getMSTime();
    _channelRights.clear();
    //                0        1         2           3            4            5
    QueryResult const result = CharacterDatabase.Query("SELECT ChannelID, flags, speakdelay, joinmessage, delaymessage, moderators FROM channels_rights");
    if (!result)
    {
        sLog->outString();
        sLog->outString(">>  Loaded 0 Channel Rights!");
        return;
    }

    size_t const count = result->GetRowCount();
    _channelRights.reserve(count);
    do
    {
        Field const* fields = result->Fetch();
        std::vector<uint32> moderators;
        if (const char* moderatorList = fields[5].GetCString())
        {
            Tokenizer tokens(moderatorList, ' ');
            for (char const* token : tokens)
                if (auto const moderatorAccountID = char_cast<uint32>(token))
                    moderators.emplace_back(moderatorAccountID.value());
        }

        ChannelRights channelRights;
        channelRights.ChannelID    = fields[0].GetUInt32();
        channelRights.Flags        = fields[1].GetUInt32();
        channelRights.SpeakDelay   = fields[2].GetUInt32();
        channelRights.JoinMessage  = fields[3].GetString();
        channelRights.SpeakMessage = fields[4].GetString();
        channelRights.Moderators   = std::move(moderators);

        _channelRights.emplace_back(std::move(channelRights));
    } while (result->NextRow());

    sLog->outString("%s", fmt::format(">> Loaded {} Channel Rights in {} ms", count, GetMSTimeDiffToNow(oldMSTime)).c_str());
    sLog->outString();
}

ChannelRights const* ChannelMgr::GetChannelRightsFor(uint32 const channelID) const
{
    auto const it = std::find_if(std::begin(_channelRights), std::end(_channelRights), [channelID](ChannelRights const& rights) { return rights.ChannelID == channelID; });
    if (it == std::end(_channelRights))
        return nullptr;

    return &*it;
}
