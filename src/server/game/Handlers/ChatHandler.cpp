

#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "Warden.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"

#include "CellImpl.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "utf8.h"

#include <cctype>
#include <algorithm>
#include "ChatProcessor.hpp"
#include "CustomMgr.h"

void WorldSession::HandleMessagechatOpcode(WorldPacket& recvData)
{
    uint32 type;
    uint32 lang;

    recvData >> type;
    recvData >> lang;

    if (type >= MAX_CHAT_MSG_TYPE)
    {
        sLog->outError("CHAT: Wrong message type received: %u", type);
        recvData.rfinish();
        return;
    }

    Player* sender = GetPlayer();

    //sLog->outDebug("CHAT: packet received. type %u, lang %u", type, lang);

    //! Riztazz: but not for addon messages, it might break addons and our warden lua sender
    if (sCustomMgr->CheckRealmPhase(RealmPhase::REALM_PHASE_RELEASED) && AccountMgr::IsPlayerAccount(GetSecurity()) && lang != LANG_ADDON)
    {
        switch (type)
        {
            //case CHAT_MSG_ADDON:
            case CHAT_MSG_SYSTEM:
            case CHAT_MSG_PARTY:
            case CHAT_MSG_RAID:
            case CHAT_MSG_GUILD:
            case CHAT_MSG_OFFICER:
            case CHAT_MSG_AFK:
            case CHAT_MSG_DND:
            case CHAT_MSG_RAID_LEADER:
            case CHAT_MSG_RAID_WARNING:
            case CHAT_MSG_BATTLEGROUND:
            case CHAT_MSG_BATTLEGROUND_LEADER:
            case CHAT_MSG_PARTY_LEADER:
            // temporary for transfers!
            case CHAT_MSG_SAY:
                break;
            default:
                if (sender->getLevel() <= 79 && sender->GetTotalPlayedTime() < 4 * HOUR)
                {
                    SendNotification("Speaking is allowed after playing for at least 4 hours. You may use party and guild chat.");
                    recvData.rfinish();
                    return;
                }
        }
    }

    // Angrathar: Newbie gamemasters doesn't have permissions to speak on public channels.
    if (sWorld->getBoolConfig(CONFIG_SPECIAL_ANGRATHAR) && AccountMgr::IsModeratorAccount(GetSecurity()))
    {
        if (type == CHAT_MSG_CHANNEL)
        {
            SendNotification("Your GM rank doesn't allow you to write on public channels.");
            recvData.rfinish();
            return;
        }
    }

    // pussywizard:
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_YELL:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_TEXT_EMOTE:
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND: {
            if (sender->IsSpectator())
            {
                recvData.rfinish();
                return;
            }
        }
    }

    // prevent talking at unknown language (cheating)
    LanguageDesc const* langDesc = GetLanguageDescByID(lang);
    if (!langDesc)
    {
        SendNotification(LANG_UNKNOWN_LANGUAGE);
        recvData.rfinish();
        return;
    }

    if (langDesc->skill_id != 0 && !sender->HasSkill(langDesc->skill_id))
    {
        // also check SPELL_AURA_COMPREHEND_LANGUAGE (client offers option to speak in that language)
        Unit::AuraEffectList const& langAuras = sender->GetAuraEffectsByType(SPELL_AURA_COMPREHEND_LANGUAGE);
        bool foundAura                        = false;
        for (Unit::AuraEffectList::const_iterator i = langAuras.begin(); i != langAuras.end(); ++i)
        {
            if ((*i)->GetMiscValue() == int32(lang))
            {
                foundAura = true;
                break;
            }
        }

        if (!foundAura)
        {
            SendNotification(LANG_NOT_LEARNED_LANGUAGE);
            recvData.rfinish();
            return;
        }
    }

    if (lang == LANG_ADDON)
    {
        // LANG_ADDON is only valid for the following message types
        switch (type)
        {
            case CHAT_MSG_PARTY:
            case CHAT_MSG_RAID:
            case CHAT_MSG_GUILD:
            case CHAT_MSG_BATTLEGROUND:
            case CHAT_MSG_WHISPER:
                // check if addon messages are disabled
                if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
                {
                    recvData.rfinish();
                    return;
                }
                break;
            default:
                sLog->outError("Player %s (GUID: %u) sent a chatmessage with an invalid language/message type combination",
                    GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow());

                recvData.rfinish();
                return;
        }
    }
    // LANG_ADDON should not be changed nor be affected by flood control
    else
    {
        auto GetMessageLimitForType = [](uint32 type, Player* player) -> uint32 {
            switch (type)
            {
                case CHAT_MSG_RAID:
                case CHAT_MSG_RAID_LEADER:
                case CHAT_MSG_RAID_WARNING:
                    return 35;
                case CHAT_MSG_GUILD:
                case CHAT_MSG_OFFICER:
                    return 150;
                case CHAT_MSG_WHISPER:
                    if (player->getLevel() >= 80)
                        return 15;
                default:
                    return 0;
            }

            return 0;
        };

        uint32 specialMessageLimit = 0;
        // send in universal language if player in .gmon mode (ignore spell effects)
        if (sender->IsGameMaster())
            lang = LANG_UNIVERSAL;
        else if (sender->GetArenaType() == ARENA_TYPE_SOLO_QUEUE)
            lang = LANG_UNIVERSAL;
        else
        {
            switch (type)
            {
                case CHAT_MSG_PARTY:
                case CHAT_MSG_PARTY_LEADER:
                case CHAT_MSG_RAID:
                case CHAT_MSG_RAID_LEADER:
                case CHAT_MSG_RAID_WARNING:
                    if (sWorld->IsCrossfactionEnabled(Crossfaction::Group))
                        lang = LANG_UNIVERSAL;

                    specialMessageLimit = 35;
                    break;
                default:
                    break;
            }


            switch (type)
            {
                case CHAT_MSG_PARTY:
                case CHAT_MSG_PARTY_LEADER: 
                {
                    if (sWorld->IsCrossfactionEnabled(Crossfaction::DungeonFinder))
                        //if (sender->getLevel() >= sCustomMgr->GetIntConfig(CONFIG_CROSSFACTION_RDF_MINLVL) && sender->getLevel() <= sCustomMgr->GetIntConfig(CONFIG_CROSSFACTION_RDF_MAXLVL))
                            lang = LANG_UNIVERSAL;
                    break;
                }
                default:
                    break;
            }

            // but overwrite it by SPELL_AURA_MOD_LANGUAGE auras (only single case used)
            Unit::AuraEffectList const& ModLangAuras = sender->GetAuraEffectsByType(SPELL_AURA_MOD_LANGUAGE);
            if (!ModLangAuras.empty())
                lang = ModLangAuras.front()->GetMiscValue();
        }

        if (type != CHAT_MSG_AFK && type != CHAT_MSG_DND)
            sender->UpdateSpeakTime(GetMessageLimitForType(type, sender));
    }

    // pussywizard: optimization
    /*if (sender->HasAura(1852) && type != CHAT_MSG_WHISPER)
    {
        SendNotification(GetTrinityString(LANG_GM_SILENCE), sender->GetName().c_str());
        recvData.rfinish();
        return;
    }*/

    bool const isProperTypeForCrossfaction = [type]()
    {
        switch (type)
        {
            case CHAT_MSG_SAY:
            case CHAT_MSG_EMOTE:
            case CHAT_MSG_YELL:
            case CHAT_MSG_PARTY:
            case CHAT_MSG_PARTY_LEADER:
            case CHAT_MSG_GUILD:
            case CHAT_MSG_OFFICER:
            case CHAT_MSG_RAID:
            case CHAT_MSG_RAID_LEADER:
            case CHAT_MSG_RAID_WARNING:
            case CHAT_MSG_BATTLEGROUND:
            case CHAT_MSG_BATTLEGROUND_LEADER:
            case CHAT_MSG_WHISPER:
            case CHAT_MSG_CHANNEL:
            case CHAT_MSG_AFK:
            case CHAT_MSG_DND:
                return true;
            default:
                return false;
        }
    }();

    if (isProperTypeForCrossfaction && lang != static_cast<uint32>(LANG_ADDON) && sWorld->IsCrossfactionEnabled(Crossfaction::Chat))
        lang = LANG_UNIVERSAL;

    bool const isProperTypeForGuildCrossfaction = [type]() 
    {
        switch (type)
        {
            case CHAT_MSG_GUILD:
            case CHAT_MSG_OFFICER:
                return true;
            default:
                return false;
        }
    }();

    if (isProperTypeForGuildCrossfaction && lang != static_cast<uint32>(LANG_ADDON) && sWorld->IsCrossfactionEnabled(Crossfaction::Guild))
        lang = LANG_UNIVERSAL;

    std::string to, channel, msg;
    bool ignoreChecks = false;
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER:
            if (lang == static_cast<uint32>(LANG_ADDON))
                msg = recvData.ReadCString(false);
            else
                recvData >> msg;
            break;
        case CHAT_MSG_WHISPER:
            recvData >> to;
            if (lang == static_cast<uint32>(LANG_ADDON))
                msg = recvData.ReadCString(false);
            else
                recvData >> msg;
            break;
        case CHAT_MSG_CHANNEL:
            recvData >> channel;
            if (lang == static_cast<uint32>(LANG_ADDON))
                msg = recvData.ReadCString(false);
            else
                recvData >> msg;
            break;
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
            if (lang == static_cast<uint32>(LANG_ADDON))
                msg = recvData.ReadCString(false);
            else
                recvData >> msg;
            ignoreChecks = true;
            break;
    }

    bool const isWardenLuaEnabled = sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED) && sWorld->getIntConfig(CONFIG_ENABLE_WARDEN_LUA) && sender->GetSession() && sender->GetSession()->_warden;
    if (type == CHAT_MSG_WHISPER && lang == LANG_ADDON && !msg.empty() && isWardenLuaEnabled)
    {
        auto const& war = sender->GetSession()->_warden;
        if (war->IsAddonMessageResult(to))
        {
            war->HandleAddonMessageResult(sender, std::move(msg));
            recvData.rfinish();
            return;
        }
    }

    //! Clean client will disconnect itself when sending messages over 255 characters
    if (sWorld->getBoolConfig(CONFIG_CHAT_CHECK_MESSAGE_SIZE) && msg.length() > CHAT_MESSAGE_MAXIMUM_SIZE)
    {
        if (sWorld->getBoolConfig(CONFIG_CHAT_REPORT_FAILURES_TO_GM))
            sWorld->SendGMText(LANG_CHAT_MESSAGE_SIZE_ANNOUNCE_GM, GetPlayer()->GetName().c_str());

        recvData.rfinish();
        return;
    }

    // Strip invisible characters for non-addon messages
    if (lang != LANG_ADDON && sWorld->getBoolConfig(CONFIG_CHAT_FAKE_MESSAGE_PREVENTING))
        stripLineInvisibleChars(msg);

    // pussywizard:
    if (lang != LANG_ADDON && msg.find("|0") != std::string::npos)
        return;

    if (!ignoreChecks)
    {
        if (msg.empty())
            return;

        if (ChatHandler(this).ParseCommands(msg.c_str()))
            return;

        if (!_player->CanSpeak())
        {
            std::string timeStr = secsToTimeString(m_muteTime - time(nullptr));
            SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
            return;
        }
    }

    if (lang != LANG_ADDON)
    {
        if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) && !ChatHandler(this).isValidChatMessage(msg.c_str()))
        {
            //sLog->outError("Player %s (GUID: %u) sent a chatmessage with an invalid link: %s", GetPlayer()->GetName().c_str(),
            //    GetPlayer()->GetGUIDLow(), msg.c_str());

            if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_KICK))
                KickPlayer();

            return;
        }
    }

    // exploit
    size_t found1 = msg.find("|Hquest");
    if (found1 != std::string::npos)
    {
        size_t found2 = msg.find(":", found1 + 8);
        size_t found3 = msg.find("|", found1 + 8);
        if (found3 == std::string::npos)
            return;
        else
        {
            if (found2 == std::string::npos)
                return;
            if (found2 > found3)
                return;
        }
    }

    Trinity::Containers::EraseIf(msg, [](char c) { return c == 0x2a; });
    if (msg.empty())
        return;

    if (lang != static_cast<uint32>(LANG_ADDON))
    {
        bool const shouldRestrictNonLatin = [this, type]
        {
            EnumFlag const chatRestrictionMask = static_cast<ChatRestrictionMask>(sWorld->getIntConfig(CONFIG_CHAT_RESTRICTION_MASK));
            if (chatRestrictionMask.HasFlag(ChatRestrictionMask::Channel) && type == CHAT_MSG_CHANNEL)
                return true;

            if (chatRestrictionMask.HasFlag(ChatRestrictionMask::Yell) && type == CHAT_MSG_YELL)
                return true;

            if (chatRestrictionMask.HasFlag(ChatRestrictionMask::Say) && type == CHAT_MSG_SAY)
                return true;

            if (chatRestrictionMask.HasFlag(ChatRestrictionMask::Whisper) && type == CHAT_MSG_WHISPER)
                return true;

            if (chatRestrictionMask.HasFlag(ChatRestrictionMask::Emote) && (type == CHAT_MSG_EMOTE || type == CHAT_MSG_TEXT_EMOTE))
                return true;

            return false;
        }();

        if (shouldRestrictNonLatin && !IsChatAllowedString(msg))
        {
            if (auto const chatRestrictedMessageInfo = sCustomMgr->GetCustomString(CustomString::ChatRestrictedMessage); chatRestrictedMessageInfo && !chatRestrictedMessageInfo->empty())
                ChatHandler(this).SendSysMessage(chatRestrictedMessageInfo->c_str());

            return;
        }
    }

    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL: {
            // Prevent cheating
            if (!sender->IsAlive())
                return;

            if (sender->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_SAY_REQ), sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ));
                return;
            }

            if (lang != LANG_ADDON)
            {
                if (sWorld->getBoolConfig(CONFIG_CHAT_PROCESSOR_ASYNC))
                    GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);
                else
                {
                    bool const result = GetChatProcessor().ProcessMessage(msg);
                    // if false then we have found a message we do not want to display
                    if (!result)
                    {
                        sWorld->SendGMText(LANG_POSSIBLE_GOLD_SELLER, sender->GetName().c_str(), msg.c_str());
                        recvData.rfinish();
                        return;
                    }
                }
            }

            if (!GetPlayer()->IsGameMaster() && GetPlayer()->SendBattleGroundChat(type, msg))
                return;

            if (type == CHAT_MSG_SAY)
                sender->Say(msg, lang);
            else if (type == CHAT_MSG_EMOTE)
                sender->TextEmote(msg);
            else if (type == CHAT_MSG_YELL)
                sender->Yell(msg, lang);
        }
        break;
        case CHAT_MSG_WHISPER: {
            if (sender->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_WHISPER_REQ), sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ));
                return;
            }

            if (!utf8::is_valid(to.begin(), to.end()))
            {
                std::string guidStr = std::to_string(GetPlayer()->GetGUID());
                sLog->outError("Player %s tried to whisper to a player with an invalid UTF8 sequence - blocked", guidStr.c_str());
                return;
            }

            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }

            Player* receiver      = ObjectAccessor::FindPlayerByName(to, false);
            bool senderIsPlayer   = AccountMgr::IsPlayerAccount(GetSecurity());
            bool receiverIsPlayer = AccountMgr::IsPlayerAccount(receiver ? receiver->GetSession()->GetSecurity() : SEC_PLAYER);
            if (!receiver || (senderIsPlayer && !receiverIsPlayer && !receiver->isAcceptWhispers() && !receiver->IsInWhisperWhiteList(sender->GetGUID())))
            {
                if (GetPlayer()->tryWhisperToWebCommand(to, msg))
                    return;

                SendPlayerNotFoundNotice(to);
                return;
            }

            if (senderIsPlayer && receiverIsPlayer)
            {
                if (GetPlayer()->GetTeam(CF_DENY) != receiver->GetTeam(CF_DENY))
                {
                    const bool isCrossfactionTarget = GetPlayer()->GetTeam() == receiver->GetTeam() && GetPlayer()->GetBattleground() == receiver->GetBattleground();
                    if (!sWorld->IsCrossfactionEnabled(Crossfaction::Chat) && !isCrossfactionTarget)
                    {
                        SendWrongFactionNotice();
                        return;
                    }
                }
                else if (lang != LANG_ADDON)
                {
                    if (GetPlayer()->GetTeam() != receiver->GetTeam() && GetPlayer()->GetBattleground() == receiver->GetBattleground())
                    {
                        SendWrongFactionNotice();
                        return;
                    }
                }
            }

            if (lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            // If player is a Gamemaster and doesn't accept whisper, we auto-whitelist every player that the Gamemaster is talking to
            if (!senderIsPlayer && !sender->isAcceptWhispers() && !sender->IsInWhisperWhiteList(receiver->GetGUID()))
                sender->AddWhisperWhiteList(receiver->GetGUID());

            GetPlayer()->Whisper(msg, lang, receiver->GetGUID());
        }
        break;
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER: {
            // if player is in battleground, he cannot say to battleground members by /p
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = sender->GetGroup();
                if (!group || (group->isBGGroup() && sender->GetArenaType() != ARENA_TYPE_SOLO_QUEUE))
                    return;
            }

            if (type == CHAT_MSG_PARTY_LEADER && !group->IsLeader(sender->GetGUID()))
                return;

            if (lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, ChatMsg(type), Language(lang), sender, nullptr, msg);
            group->BroadcastPacket(&data, false, group->GetMemberGroup(GetPlayer()->GetGUID()));
        }
        break;
        case CHAT_MSG_GUILD: {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    guild->BroadcastToGuild(this, false, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        }
        break;
        case CHAT_MSG_OFFICER: {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    guild->BroadcastToGuild(this, true, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        }
        break;
        case CHAT_MSG_RAID: {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup())
                    return;
            }

            if (lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID, Language(lang), sender, nullptr, msg);
            group->BroadcastPacket(&data, false);
        }
        break;
        case CHAT_MSG_RAID_LEADER: {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup() || !group->IsLeader(sender->GetGUID()))
                    return;
            }

            if (lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_LEADER, Language(lang), sender, nullptr, msg);
            group->BroadcastPacket(&data, false);
        }
        break;
        case CHAT_MSG_RAID_WARNING: {
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup() || !(group->IsLeader(GetPlayer()->GetGUID()) || group->IsAssistant(GetPlayer()->GetGUID())) || group->isBGGroup())
                return;

            if (lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            WorldPacket data;
            //in battleground, raid warning is sent only to players in battleground - code is ok
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_WARNING, Language(lang), sender, nullptr, msg);
            group->BroadcastPacket(&data, false);
        }
        break;
        case CHAT_MSG_BATTLEGROUND: {
            //battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup())
                return;

            if (lang != LANG_ADDON && sWorld->IsCrossfactionEnabled(Crossfaction::Battlegrounds))
            {
                lang = LANG_UNIVERSAL;
            }

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_BATTLEGROUND, Language(lang), sender, nullptr, msg);
            group->BroadcastPacket(&data, false);
        }
        break;
        case CHAT_MSG_BATTLEGROUND_LEADER: {
            // battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup() || !group->IsLeader(GetPlayer()->GetGUID()))
                return;

            if (lang != LANG_ADDON && sWorld->IsCrossfactionEnabled(Crossfaction::Battlegrounds))
            {
                lang = LANG_UNIVERSAL;
            }

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_BATTLEGROUND_LEADER, Language(lang), sender, nullptr, msg);
            group->BroadcastPacket(&data, false);
        }
        break;
        case CHAT_MSG_CHANNEL: {
            if (AccountMgr::IsPlayerAccount(GetSecurity()))
            {
                if (sender->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ))
                {
                    SendNotification(GetTrinityString(LANG_CHANNEL_REQ), sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ));
                    return;
                }
            }

            if (lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            auto GetChannelForSender = [&]() -> Channel* 
            {
                Channel* c = sChannelMgr->GetChannel(channel, sender);
                if (c && (!sender->IsPlayingCrossfaction() || c->HasFlag(CHANNEL_FLAG_CUSTOM)))
                    return c;

                if (sender->IsPlayingCrossfaction())
                {
                    Channel* c = sChannelMgr->GetChannel(channel, sender);
                    if (c && !c->HasFlag(CHANNEL_FLAG_CUSTOM))
                        return c;
                }

                return nullptr;
            };

            if (auto chn = GetChannelForSender())
                chn->Say(sender->GetGUID(), msg, (sender->IsPlayingCrossfaction() && chn->HasFlag(CHANNEL_FLAG_CUSTOM)) ? LANG_UNIVERSAL : lang);
        }
        break;
        case CHAT_MSG_AFK: {
            if (!msg.empty() && lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            if (!sender->IsInCombat())
            {
                if (sender->isAFK()) // Already AFK
                {
                    if (msg.empty())
                        sender->ToggleAFK(); // Remove AFK
                    else
                        sender->autoReplyMsg = msg; // Update message
                }
                else // New AFK mode
                {
                    sender->autoReplyMsg = msg.empty() ? GetTrinityString(LANG_PLAYER_AFK_DEFAULT) : msg;

                    if (sender->isDND())
                        sender->ToggleDND();

                    sender->ToggleAFK();
                }
            }
            break;
        }
        case CHAT_MSG_DND: {
            if (!msg.empty() && lang != LANG_ADDON)
                GetChatProcessor().ProcessMessageAsync(sender->GetName(), sender->GetSession()->GetAccountId(), msg);

            if (sender->isDND()) // Already DND
            {
                if (msg.empty())
                    sender->ToggleDND(); // Remove DND
                else
                    sender->autoReplyMsg = msg; // Update message
            }
            else // New DND mode
            {
                sender->autoReplyMsg = msg.empty() ? GetTrinityString(LANG_PLAYER_DND_DEFAULT) : msg;

                if (sender->isAFK())
                    sender->ToggleAFK();

                sender->ToggleDND();
            }

            break;
        }
        default:
            sLog->outError("CHAT: unknown message type %u, lang: %u", type, lang);
            break;
    }
}

void WorldSession::HandleEmoteOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->IsAlive() || GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        return;

    if (GetPlayer()->IsSpectator())
        return;

    uint32 emote;
    recvData >> emote;

    // restrict to the only emotes hardcoded in client
    if (emote != EMOTE_ONESHOT_NONE && emote != EMOTE_ONESHOT_WAVE)
        return;

    sScriptMgr->OnPlayerEmote(GetPlayer(), emote);
    GetPlayer()->HandleEmoteCommand(emote);
}

namespace Trinity {
class EmoteChatBuilder
{
public:
    EmoteChatBuilder(Player const& player, uint32 text_emote, uint32 emote_num, Unit const* target)
        : i_player(player)
        , i_text_emote(text_emote)
        , i_emote_num(emote_num)
        , i_target(target)
    {
    }

    void operator()(WorldPacket& data, LocaleConstant loc_idx)
    {
        std::string const name(i_target ? i_target->GetNameForLocaleIdx(loc_idx) : "");
        uint32 namlen = name.size();

        data.Initialize(SMSG_TEXT_EMOTE, 20 + namlen);
        data << i_player.GetGUID();
        data << uint32(i_text_emote);
        data << uint32(i_emote_num);
        data << uint32(namlen);
        if (namlen > 1)
            data << name;
        else
            data << uint8(0x00);
    }

private:
    Player const& i_player;
    uint32 i_text_emote;
    uint32 i_emote_num;
    Unit const* i_target;
};
} // namespace Trinity

void WorldSession::HandleTextEmoteOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->IsAlive())
        return;

    GetPlayer()->UpdateSpeakTime();

    if (!GetPlayer()->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(nullptr));
        SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    if (GetPlayer()->IsSpectator())
        return;

    uint32 text_emote, emoteNum;
    uint64 guid;

    recvData >> text_emote;
    recvData >> emoteNum;
    recvData >> guid;

    sScriptMgr->OnPlayerTextEmote(GetPlayer(), text_emote, emoteNum, guid);

    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(text_emote);
    if (!em)
        return;

    uint32 emote_anim = em->textid;

    switch (emote_anim)
    {
        case EMOTE_STATE_SLEEP:
        case EMOTE_STATE_SIT:
        case EMOTE_STATE_KNEEL:
        case EMOTE_ONESHOT_NONE:
            break;
        default:
            // Only allow text-emotes for "dead" entities (feign death included)
            if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
                break;
            GetPlayer()->HandleEmoteCommand(emote_anim);
            break;
    }

    Unit* unit = ObjectAccessor::GetUnit(*_player, guid);

    CellCoord p = Trinity::ComputeCellCoord(GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::EmoteChatBuilder emote_builder(*GetPlayer(), text_emote, emoteNum, unit);
    Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder> emote_do(emote_builder);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder> > emote_worker(GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), emote_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder> >, WorldTypeMapContainer> message(emote_worker);
    cell.Visit(p, message, *GetPlayer()->GetMap(), *GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));

    GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE, text_emote, 0, unit);

    //Send scripted event call
    if (unit && unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
        ((Creature*)unit)->AI()->ReceiveEmote(GetPlayer(), text_emote);
}

void WorldSession::HandleChatIgnoredOpcode(WorldPacket& recvData)
{
    uint64 iguid;
    uint8 unk;
    //sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: Received CMSG_CHAT_IGNORED");

    recvData >> iguid;
    recvData >> unk; // probably related to spam reporting

    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(iguid);
    if (!player)
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_IGNORED, LANG_UNIVERSAL, _player, _player, GetPlayer()->GetName());
    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleChannelDeclineInvite(WorldPacket& recvPacket)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "Opcode %u", recvPacket.GetOpcode());
}

void WorldSession::SendPlayerNotFoundNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_NOT_FOUND, name.size() + 1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendPlayerAmbiguousNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_AMBIGUOUS, name.size() + 1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendWrongFactionNotice()
{
    WorldPacket data(SMSG_CHAT_WRONG_FACTION, 0);
    SendPacket(&data);
}

void WorldSession::SendChatRestrictedNotice(ChatRestrictionType restriction)
{
    WorldPacket data(SMSG_CHAT_RESTRICTED, 1);
    data << uint8(restriction);
    SendPacket(&data);
}
