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

#ifndef __TRINITY_CHANNELMGR_H
#define __TRINITY_CHANNELMGR_H

#include "Common.h"
#include "Channel.h"

#include <string>

namespace ChannelConstants {
constexpr uint32 MaxPasswordLength = 31;
}

struct ChannelRights;
class Channel;

struct ChannelInfo
{
    std::wstring Name;
    std::unique_ptr<Channel> ChannelPtr;

    bool operator==(ChannelInfo const& rhs) const
    {
        if (Name == rhs.Name && ChannelPtr && ChannelPtr->GetTeamID() == rhs.ChannelPtr->GetTeamID())
            return true;

        return false;
    }
};

template<>
struct std::hash<ChannelInfo>
{
    size_t operator()(ChannelInfo const& cInfo) const noexcept
    {
        size_t hash = 0;
        HashCombine(hash, cInfo.Name);
        HashCombine(hash, cInfo.ChannelPtr->GetTeamID());
        return hash;
    }
};

using ChannelSet       = std::unordered_set<ChannelInfo>;
using ChannelRightsVec = std::vector<ChannelRights>;

struct ChannelMgr
{
    static ChannelMgr* instance();

    Channel* GetJoinChannel(std::string const& name, uint32 channelID, TeamId teamID);
    Channel* GetChannel(std::string const& name, Player const* player, bool pkt = true) const;

    void LoadChannels();
    void LoadChannelRights();

    ChannelRights const* GetChannelRightsFor(uint32 channelID) const;

private:
    ChannelSet _channels;
    ChannelRightsVec _channelRights;
};

static ChannelMgr* sChannelMgr = ChannelMgr::instance();

#endif
