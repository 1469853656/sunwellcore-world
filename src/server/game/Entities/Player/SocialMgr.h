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

#ifndef __TRINITY_SOCIALMGR_H
#define __TRINITY_SOCIALMGR_H

#include "Common.h"

class SocialMgr;
class PlayerSocial;
class Player;
class WorldPacket;

enum FriendStatus
{
    FRIEND_STATUS_OFFLINE   = 0x00,
    FRIEND_STATUS_ONLINE    = 0x01,
    FRIEND_STATUS_AFK       = 0x02,
    FRIEND_STATUS_DND       = 0x04,
    FRIEND_STATUS_RAF       = 0x08
};

enum SocialFlag
{
    SOCIAL_FLAG_FRIEND      = 0x01,
    SOCIAL_FLAG_IGNORED     = 0x02,
    SOCIAL_FLAG_MUTED       = 0x04,                          // guessed
    SOCIAL_FLAG_UNK         = 0x08                           // Unknown - does not appear to be RaF
};

struct FriendInfo
{
    FriendStatus Status = FRIEND_STATUS_OFFLINE;
    uint8 Flags = 0;
    uint32 Area = 0;
    uint8 Level = 0;
    uint8 Class = 0;
    std::string Note;

    FriendInfo() = default;
    FriendInfo(uint8 const flags, std::string note) : Flags(flags), Note(std::move(note)) { }
};

using PlayerSocialMap = std::unordered_map<uint32, FriendInfo>;
using SocialMap = std::unordered_map<uint32, PlayerSocial>;

/// Results of friend related commands
enum FriendsResult
{
    FRIEND_DB_ERROR         = 0x00,
    FRIEND_LIST_FULL        = 0x01,
    FRIEND_ONLINE           = 0x02,
    FRIEND_OFFLINE          = 0x03,
    FRIEND_NOT_FOUND        = 0x04,
    FRIEND_REMOVED          = 0x05,
    FRIEND_ADDED_ONLINE     = 0x06,
    FRIEND_ADDED_OFFLINE    = 0x07,
    FRIEND_ALREADY          = 0x08,
    FRIEND_SELF             = 0x09,
    FRIEND_ENEMY            = 0x0A,
    FRIEND_IGNORE_FULL      = 0x0B,
    FRIEND_IGNORE_SELF      = 0x0C,
    FRIEND_IGNORE_NOT_FOUND = 0x0D,
    FRIEND_IGNORE_ALREADY   = 0x0E,
    FRIEND_IGNORE_ADDED     = 0x0F,
    FRIEND_IGNORE_REMOVED   = 0x10,
    FRIEND_IGNORE_AMBIGUOUS = 0x11,                         // That name is ambiguous, type more of the player's server name
    FRIEND_MUTE_FULL        = 0x12,
    FRIEND_MUTE_SELF        = 0x13,
    FRIEND_MUTE_NOT_FOUND   = 0x14,
    FRIEND_MUTE_ALREADY     = 0x15,
    FRIEND_MUTE_ADDED       = 0x16,
    FRIEND_MUTE_REMOVED     = 0x17,
    FRIEND_MUTE_AMBIGUOUS   = 0x18,                         // That name is ambiguous, type more of the player's server name
    FRIEND_UNK7             = 0x19,                         // no message at client
    FRIEND_UNKNOWN          = 0x1A                          // Unknown friend response from server
};

namespace SocialConstants
{
    constexpr uint32 FriendLimit = 50;
    constexpr uint32 IgnoreLimit = 50;
}

class PlayerSocial
{
    friend class SocialMgr;

    public:
        // adding/removing
        bool AddToSocialList(uint32 friendCharacterID, bool ignore);
        void RemoveFromSocialList(uint32 friendCharacterID, bool ignore);
        void SetFriendNote(uint32 friendCharacterID, std::string note);

        // Packet send's
        void SendSocialList(Player* player);

        // Misc
        bool HasFriend(uint32 friendCharacterID) const;
        bool HasIgnore(uint32 ignoreCharacterID) const;
        uint32 GetPlayerGUID() const { return m_playerGUID; }
        void SetPlayerGUID(uint32 const characterID) { m_playerGUID = characterID; }
        auto GetNumberOfSocialsWithFlag(SocialFlag const flag) const
        {
            return static_cast<uint32>(std::count_if(std::begin(m_playerSocialMap), std::end(m_playerSocialMap), [flag](auto const& pair) { return pair.second.Flags & flag; }));
        }

    private:
        PlayerSocialMap m_playerSocialMap;
        uint32 m_playerGUID = 0;
};

class SocialMgr : public Sunwell::Singleton<SocialMgr>
{
    friend class Singleton<SocialMgr>;

    public:
        // Misc
        void RemovePlayerSocial(uint32 const characterID) { m_socialMap.erase(characterID); }
        static void GetFriendInfo(Player* player, uint32 friendCharacterID, FriendInfo& friendInfo);
        static bool CanBeSeen(Player const* playerSource, Player const* playerFriend);

        // Packet management
        static void MakeFriendStatusPacket(FriendsResult result, uint32 friendCharacterID, WorldPacket* data);
        void SendFriendStatus(Player* player, FriendsResult result, uint32 friendCharacterID, bool broadcast);
        void BroadcastToFriendListers(Player* player, WorldPacket const* packet);

        // Loading
        PlayerSocial* LoadFromDB(PreparedQueryResult result, uint32 guid);

    private:
        SocialMap m_socialMap;
};

#define sSocialMgr SocialMgr::instance()
#endif

