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

#include "SocialMgr.h"
#include "DatabaseEnv.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "Util.h"
#include "AccountMgr.h"

bool PlayerSocial::AddToSocialList(uint32 const friendCharacterID, bool const ignore)
{
    // check client limits
    if (ignore)
    {
        if (GetNumberOfSocialsWithFlag(SOCIAL_FLAG_IGNORED) >= SocialConstants::IgnoreLimit)
            return false;
    }
    else
    {
        if (GetNumberOfSocialsWithFlag(SOCIAL_FLAG_FRIEND) >= SocialConstants::FriendLimit)
            return false;
    }

    uint8 flag = SOCIAL_FLAG_FRIEND;
    if (ignore)
        flag = SOCIAL_FLAG_IGNORED;

    if (auto const itr = m_playerSocialMap.find(friendCharacterID); itr != m_playerSocialMap.end())
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_CHARACTER_SOCIAL_FLAGS);
        stmt->setUInt8(0, flag);
        stmt->setUInt32(1, GetPlayerGUID());
        stmt->setUInt32(2, friendCharacterID);

        CharacterDatabase.Execute(stmt);
        itr->second.Flags |= flag;
    }
    else
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_SOCIAL);
        stmt->setUInt32(0, GetPlayerGUID());
        stmt->setUInt32(1, friendCharacterID);
        stmt->setUInt8(2, flag);

        CharacterDatabase.Execute(stmt);

        FriendInfo fi;
        fi.Flags |= flag;
        m_playerSocialMap[friendCharacterID] = fi;
    }

    return true;
}

void PlayerSocial::RemoveFromSocialList(uint32 const friendCharacterID, bool const ignore)
{
    auto const itr = m_playerSocialMap.find(friendCharacterID);
    if (itr == m_playerSocialMap.end())                     // not exist
        return;

    uint8 flag = SOCIAL_FLAG_FRIEND;
    if (ignore)
        flag = SOCIAL_FLAG_IGNORED;

    itr->second.Flags &= ~flag;
    if (itr->second.Flags == 0)
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_SOCIAL);

        stmt->setUInt32(0, GetPlayerGUID());
        stmt->setUInt32(1, friendCharacterID);

        CharacterDatabase.Execute(stmt);

        m_playerSocialMap.erase(itr);
    }
    else
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_REM_CHARACTER_SOCIAL_FLAGS);

        stmt->setUInt8(0, flag);
        stmt->setUInt32(1, GetPlayerGUID());
        stmt->setUInt32(2, friendCharacterID);

        CharacterDatabase.Execute(stmt);
    }
}

void PlayerSocial::SetFriendNote(uint32 const friendCharacterID, std::string note)
{
    auto const itr = m_playerSocialMap.find(friendCharacterID);
    if (itr == m_playerSocialMap.end()) // not exist
        return;

    Utf8Truncate(note, 48);                                  // DB and client size limitation

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_SOCIAL_NOTE);

    stmt->setString(0, note);
    stmt->setUInt32(1, GetPlayerGUID());
    stmt->setUInt32(2, friendCharacterID);

    CharacterDatabase.Execute(stmt);

    itr->second.Note = std::move(note);
}

void PlayerSocial::SendSocialList(Player* player)
{
    if (!player)
        return;

    size_t const socialMapSize = m_playerSocialMap.size();

    WorldPacket data(SMSG_CONTACT_LIST, (4+4+ socialMapSize *25));     // just can guess size
    data << static_cast<uint32>(7);                                      // 0x1 = Friendlist update. 0x2 = Ignorelist update. 0x4 = Mutelist update.
    data << static_cast<uint32>(socialMapSize);                                   // friends count

    for (auto& [characterID, friendInfo] : m_playerSocialMap)
    {
        sSocialMgr->GetFriendInfo(player, characterID, friendInfo);

        data << static_cast<uint64>(characterID);                         // player guid
        data << static_cast<uint32>(friendInfo.Flags);                  // player flag (0x1 = Friend, 0x2 = Ignored, 0x4 = Muted)
        data << friendInfo.Note;                           // string note
        if (friendInfo.Flags & SOCIAL_FLAG_FRIEND)         // if IsFriend()
        {
            data << static_cast<uint8>(friendInfo.Status);              // online/offline/etc?
            if (friendInfo.Status)                         // if online
            {
                data << friendInfo.Area;           // player area
                data << static_cast<uint32>(friendInfo.Level);          // player level
                data << static_cast<uint32>(friendInfo.Class);          // player class
            }
        }
    }

    player->GetSession()->SendPacket(&data);
}

bool PlayerSocial::HasFriend(uint32 const friendCharacterID) const
{
    if (auto const itr = m_playerSocialMap.find(friendCharacterID); itr != m_playerSocialMap.end())
        return itr->second.Flags & SOCIAL_FLAG_FRIEND;

    return false;
}

bool PlayerSocial::HasIgnore(uint32 const ignoreCharacterID) const
{
    if (auto const itr = m_playerSocialMap.find(ignoreCharacterID); itr != m_playerSocialMap.end())
        return itr->second.Flags & SOCIAL_FLAG_IGNORED;

    return false;
}

void SocialMgr::GetFriendInfo(Player* player, uint32 const friendCharacterID, FriendInfo &friendInfo)
{
    if (!player)
        return;

    friendInfo.Status = FRIEND_STATUS_OFFLINE;
    friendInfo.Area = 0;
    friendInfo.Level = 0;
    friendInfo.Class = 0;

    Player* playerFriend = ObjectAccessor::FindPlayer(friendCharacterID);
    if (!playerFriend)
        return;

    AccountTypes const security = player->GetSession()->GetSecurity();
    bool const notGM = AccountMgr::IsPlayerAccount(security);
    if (notGM && playerFriend->GetSession()->GetSecurity() >= SEC_MODERATOR)
        return;

    auto const itr = player->GetSocial()->m_playerSocialMap.find(friendCharacterID);
    if (itr != player->GetSocial()->m_playerSocialMap.end())
        friendInfo.Note = itr->second.Note;

    // PLAYER see his team only and PLAYER can't see MODERATOR, GAME MASTER, ADMINISTRATOR characters
    // MODERATOR, GAME MASTER, ADMINISTRATOR can see all
    if (!notGM || CanBeSeen(player, playerFriend))
    {
        friendInfo.Status = FRIEND_STATUS_ONLINE;
        if (playerFriend->isAFK())
            friendInfo.Status = FRIEND_STATUS_AFK;
        if (playerFriend->isDND())
            friendInfo.Status = FRIEND_STATUS_DND;

        friendInfo.Area = playerFriend->GetZoneId();
        friendInfo.Level = playerFriend->getLevel();
        friendInfo.Class = playerFriend->getClass();
    }
}

bool SocialMgr::CanBeSeen(Player const* playerSource, Player const* playerFriend)
{
    if (!playerSource->IsVisibleGloballyFor(playerFriend))
        return false;

    if (!sWorld->IsCrossfactionEnabled(Crossfaction::FriendList) && playerFriend->GetTeam(CF_DENY) != playerSource->GetTeam(CF_DENY))
        return false;

    return true;
}

void SocialMgr::MakeFriendStatusPacket(FriendsResult const result, uint32 const friendCharacterID, WorldPacket* data)
{
    data->Initialize(SMSG_FRIEND_STATUS, 9);
    *data << static_cast<uint8>(result);
    *data << MAKE_NEW_GUID(friendCharacterID, 0, HIGHGUID_PLAYER);
}

void SocialMgr::SendFriendStatus(Player* player, FriendsResult const result, uint32 const friendCharacterID, bool const broadcast)
{
    FriendInfo fi;

    WorldPacket data;
    MakeFriendStatusPacket(result, friendCharacterID, &data);
    GetFriendInfo(player, friendCharacterID, fi);
    switch (result)
    {
        case FRIEND_ADDED_OFFLINE:
        case FRIEND_ADDED_ONLINE:
            data << fi.Note;
            break;
        default:
            break;
    }

    switch (result)
    {
        case FRIEND_ADDED_ONLINE:
        case FRIEND_ONLINE:
            data << static_cast<uint8>(fi.Status);
            data << fi.Area;
            data << static_cast<uint32>(fi.Level);
            data << static_cast<uint32>(fi.Class);
            break;
        default:
            break;
    }

    if (broadcast)
        BroadcastToFriendListers(player, &data);
    else
        player->GetSession()->SendPacket(&data);
}

void SocialMgr::BroadcastToFriendListers(Player* player, WorldPacket const* packet)
{
    if (!player)
        return;

    TeamId const teamId = player->GetTeam();
    AccountTypes const security = player->GetSession()->GetSecurity();
    uint32 const characterID = player->GetGUIDLow();

    for (const auto& [friendCharacterID, socialData] : m_socialMap)
    {
        if (auto const itr2 = socialData.m_playerSocialMap.find(characterID); itr2 != socialData.m_playerSocialMap.end() && (itr2->second.Flags & SOCIAL_FLAG_FRIEND))
        {
            Player const* playerFriend = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(friendCharacterID, 0, HIGHGUID_PLAYER));
            if (!playerFriend || !playerFriend->IsInWorld() || !playerFriend->GetSession() || playerFriend->GetSession()->isLogingOut())
                continue;

            // PLAYER see his team only and PLAYER can't see MODERATOR, GAME MASTER, ADMINISTRATOR characters
            // MODERATOR, GAME MASTER, ADMINISTRATOR can see all
            if (AccountMgr::IsPlayerAccount(security))
            {
                if (!AccountMgr::IsPlayerAccount(playerFriend->GetSession()->GetSecurity()))
                    continue;

                if (!CanBeSeen(player, playerFriend))
                    continue;
            }

            playerFriend->GetSession()->SendPacket(packet);
        }
    }
}

PlayerSocial* SocialMgr::LoadFromDB(PreparedQueryResult result, uint32 const guid)
{
    PlayerSocial* social = &m_socialMap[guid];
    social->SetPlayerGUID(guid);

    if (!result)
        return social;

    uint32 friendGuid = 0;

    do
    {
        Field const* fields = result->Fetch();
        friendGuid = fields[0].GetUInt32();
        uint8 const flags = fields[1].GetUInt8();
        social->m_playerSocialMap[friendGuid] = FriendInfo(flags, fields[2].GetString());

        // client's friends list and ignore list limit
        if (social->m_playerSocialMap.size() >= SocialConstants::FriendLimit + SocialConstants::IgnoreLimit)
            break;
    }

    while (result->NextRow());

    return social;
}
