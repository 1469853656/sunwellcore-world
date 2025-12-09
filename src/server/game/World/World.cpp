

/** \file
    \ingroup world
*/

#include "Common.h"
#include "DatabaseEnv.h"
#include "Config.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Player.h"
#include "Vehicle.h"
#include "SkillExtraItems.h"
#include "SkillDiscovery.h"
#include "World.h"
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"
#include "ArenaTeamMgr.h"
#include "GuildMgr.h"
#include "TicketMgr.h"
#include "SpellMgr.h"
#include "GroupMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "LootMgr.h"
#include "ItemEnchantmentMgr.h"
#include "MapManager.h"
#include "MythicMgr.hpp"
#include "CreatureAIRegistry.h"
#include "BattlegroundMgr.h"
#include "BattlefieldMgr.h"
#include "AnticheatMgr.h"
#include "OutdoorPvPMgr.h"
#include "TemporarySummon.h"
#include "WaypointMovementGenerator.h"
#include "VMapFactory.h"
#include "MMapFactory.h"
#include "GameEventMgr.h"
#include "PoolMgr.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "InstanceSaveMgr.h"
#include "Util.h"
#include "Language.h"
#include "CreatureGroups.h"
#include "Transport.h"
#include "ScriptMgr.h"
#include "AddonMgr.h"
#include "LFGMgr.h"
#include "ConditionMgr.h"
#include "DisableMgr.h"
#include "CharacterDatabaseCleaner.h"
#include "ScriptMgr.h"
#include "WeatherMgr.h"
#include "CreatureTextMgr.h"
#include "SmartAI.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "WardenCheckMgr.h"
#include "Warden.h"
#include "CalendarMgr.h"
#include "PetitionMgr.h"
#include "LootItemStorage.h"
#include "TransportMgr.h"
#include "AvgDiffTracker.h"
#include "DynamicVisibility.h"
#include "WhoListCache.h"
#include "AsyncAuctionListing.h"
#include "SavingSystem.h"
#include "CustomEventMgr.h"
#include "ThreadedPathGenerator.hpp"
#include "FollowMovementGenerator.hpp"
#include "WorldCache.h"
#include "fmt/format.h"
#include "Profiler.h"
#include "ChatProcessor.hpp"
#include "GitRevision.h"
#include "SoloQueue.h"
#include "Configuration/Config.h"
#include "PremiumStoreMgr.h"
#include "TransmogificationMgr.h"

std::atomic_bool World::m_stopEvent(false);

uint8 World::m_ExitCode          = SHUTDOWN_EXIT_CODE;
uint32 World::m_worldLoopCounter = 0;
uint32 World::m_gameMSTime       = 0;

float World::m_MaxVisibleDistanceOnContinents    = VisibilityConstants::DEFAULT_VISIBILITY_DISTANCE;
float World::m_MaxVisibleDistanceInInstances     = VisibilityConstants::DEFAULT_VISIBILITY_INSTANCE;
float World::m_MaxVisibleDistanceInBattlegrounds = VisibilityConstants::DEFAULT_VISIBILITY_BATTLEGROUNDS;
float World::m_MaxVisibleDistanceInArenas        = VisibilityConstants::DEFAULT_VISIBILITY_ARENAS;

/// World constructor
World::World()
{
    m_playerLimit           = 0;
    m_allowedSecurityLevel  = SEC_PLAYER;
    m_allowMovement         = true;
    m_ShutdownMask          = 0;
    m_ShutdownTimer         = 0;
    m_gameTime              = time(nullptr);
    m_gameMSTime            = getMSTime();
    m_startTime             = m_gameTime;
    m_maxQueuedSessionCount = 0;
    m_PlayerCount           = 0;
    m_MaxPlayerCount        = 0;
    m_NextDailyQuestReset   = 0;
    m_NextWeeklyQuestReset  = 0;
    m_NextMonthlyQuestReset = 0;
    m_NextRandomBGReset     = 0;
    m_NextGuildReset        = 0;
    m_sessions.clear();
    m_offlineSessions.clear();
    m_disconnects.clear();
    m_QueuedPlayer.clear();

    m_characterScreenScript.clear();
    m_characterScreenScript.reserve(20);

    m_defaultDbcLocale = LOCALE_enUS;

    mail_expire_check_timer = 0;
    m_updateTime            = 0;
    m_updateTimeSum         = 0;

    m_isClosed = false;

    m_CleaningFlags = 0;

    memset(rate_values, 0, sizeof(rate_values));
    memset(m_int_configs, 0, sizeof(m_int_configs));
    memset(m_bool_configs, 0, sizeof(m_bool_configs));
    memset(m_float_configs, 0, sizeof(m_float_configs));
    m_Autobroadcasts.clear();
    m_AutobroadcastsWeights.clear();
}

/// World destructor
World::~World()
{
    ///- Empty the kicked session set
    while (!m_sessions.empty())
    {
        // not remove from queue, prevent loading new sessions
        delete m_sessions.begin()->second;
        m_sessions.erase(m_sessions.begin());
    }

    while (!m_offlineSessions.empty())
    {
        delete m_offlineSessions.begin()->second;
        m_offlineSessions.erase(m_offlineSessions.begin());
    }

    cliCmdQueue.close();

    VMAP::VMapFactory::clear();
    MMAP::MMapFactory::clear();
}

/// Find a player in a specified zone
Player* World::FindPlayerInZone(uint32 zone)
{
    ///- circle through active sessions and return the first player found in the zone
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second)
            continue;

        Player* player = itr->second->GetPlayer();
        if (!player)
            continue;

        if (player->IsInWorld() && player->GetZoneId() == zone)
            return player;
    }
    return nullptr;
}

bool World::IsClosed() const
{
    return m_isClosed;
}

void World::SetClosed(bool val)
{
    m_isClosed = val;

    // Invert the value, for simplicity for scripters.
    sScriptMgr->OnOpenStateChange(!val);
}

void World::SetMotd(const std::string& motd)
{
    m_motd = motd;

    sScriptMgr->OnMotdChange(m_motd);
}

const char* World::GetMotd() const
{
    return m_motd.c_str();
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second; // also can return NULL for kicked session
    else
        return nullptr;
}

WorldSession* World::FindOfflineSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_offlineSessions.find(id);
    if (itr != m_offlineSessions.end())
        return itr->second;
    else
        return nullptr;
}

WorldSession* World::FindOfflineSessionForCharacterGUID(uint32 guidLow) const
{
    if (m_offlineSessions.empty())
        return nullptr;
    for (SessionMap::const_iterator itr = m_offlineSessions.begin(); itr != m_offlineSessions.end(); ++itr)
        if (itr->second->GetGuidLow() == guidLow)
            return itr->second;
    return nullptr;
}

/// Remove a given session
bool World::KickSession(uint32 id)
{
    ///- Find the session, kick the user, but we can't delete session at this moment to prevent iterator invalidation
    SessionMap::const_iterator itr = m_sessions.find(id);
    if (itr != m_sessions.end() && itr->second)
    {
        WorldSession* session = itr->second;
        if (!session->_transactionCallbacks.empty())
            return false;

        if (itr->second->PlayerLoading())
            return false;

        itr->second->KickPlayer(false);
    }

    return true;
}

void World::QueueNewSession(WorldSession* session)
{
    m_newSessions.Enqueue(session);
}

void World::AddSession(WorldSession* s)
{
    ASSERT(s);

    // kick existing session with same account (if any)
    // if character on old session is being loaded, then return
    if (!KickSession(s->GetAccountId()))
    {
        s->KickPlayer();
        delete s; // session not added yet in session list, so not listed in queue
        return;
    }

    SessionMap::const_iterator old = m_sessions.find(s->GetAccountId());
    if (old != m_sessions.end())
    {
        WorldSession* oldSession = old->second;

        if (!RemoveQueuedPlayer(oldSession) && getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
            m_disconnects[s->GetAccountId()] = time(nullptr);

        // pussywizard:
        if (oldSession->HandleSocketClosed())
        {
            // there should be no offline session if current one is logged onto a character
            SessionMap::iterator iter;
            if ((iter = m_offlineSessions.find(oldSession->GetAccountId())) != m_offlineSessions.end())
            {
                WorldSession* tmp = iter->second;
                m_offlineSessions.erase(iter);
                tmp->SetShouldSetOfflineInDB(false);
                delete tmp;
            }
            oldSession->SetOfflineTime(time(nullptr));
            m_offlineSessions[oldSession->GetAccountId()] = oldSession;
        }
        else
        {
            oldSession->SetShouldSetOfflineInDB(false); // pussywizard: don't set offline in db because new session for that acc is already created
            delete oldSession;
        }
    }

    m_sessions[s->GetAccountId()] = s;

    uint32 Sessions = GetActiveAndQueuedSessionCount();
    uint32 pLimit   = GetPlayerAmountLimit();

    // don't count this session when checking player limit
    --Sessions;

    if (pLimit > 0 && Sessions >= pLimit && AccountMgr::IsPlayerAccount(s->GetSecurity()) && !s->CanSkipQueue() && !HasRecentlyDisconnected(s))
    {
        AddQueuedPlayer(s);
        UpdateMaxSessionCounters();
        return;
    }

    if (sWorld->getBoolConfig(CONFIG_ACCOUNT_HISTORY))
    {
        AddAccountHistory(s->GetAccountId(), s->GetRemoteAddress(), time(nullptr));
    }

    s->InitializeSession();
    UpdateMaxSessionCounters();
}

bool World::HasRecentlyDisconnected(WorldSession* session)
{
    if (!session)
        return false;

    if (uint32 tolerance = getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
    {
        for (DisconnectMap::iterator i = m_disconnects.begin(); i != m_disconnects.end();)
        {
            if ((time(nullptr) - i->second) < tolerance)
            {
                if (i->first == session->GetAccountId())
                    return true;
                ++i;
            }
            else
                m_disconnects.erase(i++);
        }
    }
    return false;
}

int32 World::GetQueuePos(WorldSession* sess)
{
    uint32 position = 1;

    for (Queue::const_iterator iter = m_QueuedPlayer.begin(); iter != m_QueuedPlayer.end(); ++iter, ++position)
        if ((*iter) == sess)
            return position;

    return 0;
}

void World::AddQueuedPlayer(WorldSession* sess)
{
    sess->SetInQueue(true);
    m_QueuedPlayer.push_back(sess);

    // The 1st SMSG_AUTH_RESPONSE needs to contain other info too.
    sess->SendAuthResponse(AUTH_WAIT_QUEUE, false, GetQueuePos(sess));
}

bool World::RemoveQueuedPlayer(WorldSession* sess)
{
    uint32 sessions = GetActiveSessionCount();

    uint32 position      = 1;
    Queue::iterator iter = m_QueuedPlayer.begin();

    // search to remove and count skipped positions
    bool found = false;

    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
    {
        if (*iter == sess)
        {
            sess->SetInQueue(false);
            sess->ResetTimeOutTime();
            iter  = m_QueuedPlayer.erase(iter);
            found = true;
            break;
        }
    }

    // if session not queued then it was an active session
    if (!found)
    {
        ASSERT(sessions > 0);
        --sessions;
    }

    // accept first in queue
    if ((!GetPlayerAmountLimit() || sessions < GetPlayerAmountLimit()) && !m_QueuedPlayer.empty())
    {
        WorldSession* pop_sess = m_QueuedPlayer.front();
        pop_sess->InitializeSession();

        m_QueuedPlayer.pop_front();

        // update iter to point first queued socket or end() if queue is empty now
        iter     = m_QueuedPlayer.begin();
        position = 1;
    }

    // update queue position from iter to end()
    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
        (*iter)->SendAuthWaitQue(position);

    return found;
}

/// Initialize config values
void World::LoadConfigSettings(bool reload)
{
    if (reload)
    {
        if (!sConfigMgr->Reload())
        {
            sLog->outError("World settings reload fail: can't read settings from %s.", sConfigMgr->GetMainConfigFileName().c_str());
            return;
        }

        sLog->ReloadConfig(); // Reload log levels and filters
    }

    ///- Read the player limit and the Message of the day from the config file
    SetPlayerAmountLimit(sConfigMgr->GetIntDefault("PlayerLimit", 100));
    SetMotd(sConfigMgr->GetStringDefault("Motd", "Welcome to a Sunwell Core Server."));

    ///- Read ticket system setting from the config file
    m_bool_configs[CONFIG_ALLOW_TICKETS] = sConfigMgr->GetBoolDefault("AllowTickets", true);

    ///- Get string for new logins (newly created characters)
    SetNewCharString(sConfigMgr->GetStringDefault("PlayerStart.String", ""));

    ///- Send server info on login?
    m_int_configs[CONFIG_ENABLE_SINFO_LOGIN] = sConfigMgr->GetIntDefault("Server.LoginInfo", 0);

    ///- Read all rates from the config file
    rate_values[RATE_HEALTH] = sConfigMgr->GetFloatDefault("Rate.Health", 1);
    if (rate_values[RATE_HEALTH] < 0)
    {
        sLog->outError("Rate.Health (%f) must be > 0. Using 1 instead.", rate_values[RATE_HEALTH]);
        rate_values[RATE_HEALTH] = 1;
    }
    rate_values[RATE_POWER_MANA] = sConfigMgr->GetFloatDefault("Rate.Mana", 1);
    if (rate_values[RATE_POWER_MANA] < 0)
    {
        sLog->outError("Rate.Mana (%f) must be > 0. Using 1 instead.", rate_values[RATE_POWER_MANA]);
        rate_values[RATE_POWER_MANA] = 1;
    }
    rate_values[RATE_POWER_RAGE_INCOME] = sConfigMgr->GetFloatDefault("Rate.Rage.Income", 1);
    rate_values[RATE_POWER_RAGE_LOSS]   = sConfigMgr->GetFloatDefault("Rate.Rage.Loss", 1);
    if (rate_values[RATE_POWER_RAGE_LOSS] < 0)
    {
        sLog->outError("Rate.Rage.Loss (%f) must be > 0. Using 1 instead.", rate_values[RATE_POWER_RAGE_LOSS]);
        rate_values[RATE_POWER_RAGE_LOSS] = 1;
    }
    rate_values[RATE_POWER_RUNICPOWER_INCOME] = sConfigMgr->GetFloatDefault("Rate.RunicPower.Income", 1);
    rate_values[RATE_POWER_RUNICPOWER_LOSS]   = sConfigMgr->GetFloatDefault("Rate.RunicPower.Loss", 1);
    if (rate_values[RATE_POWER_RUNICPOWER_LOSS] < 0)
    {
        sLog->outError("Rate.RunicPower.Loss (%f) must be > 0. Using 1 instead.", rate_values[RATE_POWER_RUNICPOWER_LOSS]);
        rate_values[RATE_POWER_RUNICPOWER_LOSS] = 1;
    }
    rate_values[RATE_POWER_FOCUS]  = sConfigMgr->GetFloatDefault("Rate.Focus", 1.0f);
    rate_values[RATE_POWER_ENERGY] = sConfigMgr->GetFloatDefault("Rate.Energy", 1.0f);

    rate_values[RATE_SKILL_DISCOVERY] = sConfigMgr->GetFloatDefault("Rate.Skill.Discovery", 1.0f);

    rate_values[RATE_DROP_ITEM_POOR]              = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Poor", 1.0f);
    rate_values[RATE_DROP_ITEM_NORMAL]            = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Normal", 1.0f);
    rate_values[RATE_DROP_ITEM_UNCOMMON]          = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Uncommon", 1.0f);
    rate_values[RATE_DROP_ITEM_RARE]              = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Rare", 1.0f);
    rate_values[RATE_DROP_ITEM_EPIC]              = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Epic", 1.0f);
    rate_values[RATE_DROP_ITEM_LEGENDARY]         = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Legendary", 1.0f);
    rate_values[RATE_DROP_ITEM_ARTIFACT]          = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Artifact", 1.0f);
    rate_values[RATE_DROP_ITEM_REFERENCED]        = sConfigMgr->GetFloatDefault("Rate.Drop.Item.Referenced", 1.0f);
    rate_values[RATE_DROP_ITEM_REFERENCED_AMOUNT] = sConfigMgr->GetFloatDefault("Rate.Drop.Item.ReferencedAmount", 1.0f);
    rate_values[RATE_DROP_MONEY]                  = sConfigMgr->GetFloatDefault("Rate.Drop.Money", 1.0f);

    rate_values[RATE_REPAIRCOST]                  = sConfigMgr->GetFloatDefault("Rate.RepairCost", 1.0f);
    if (rate_values[RATE_REPAIRCOST] < 0.0f)
    {
        sLog->outError("Rate.RepairCost (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_REPAIRCOST]);
        rate_values[RATE_REPAIRCOST] = 0.0f;
    }
    rate_values[RATE_REPUTATION_GAIN]                      = sConfigMgr->GetFloatDefault("Rate.Reputation.Gain", 1.0f);
    rate_values[RATE_REPUTATION_LOWLEVEL_KILL]             = sConfigMgr->GetFloatDefault("Rate.Reputation.LowLevel.Kill", 1.0f);
    rate_values[RATE_REPUTATION_LOWLEVEL_QUEST]            = sConfigMgr->GetFloatDefault("Rate.Reputation.LowLevel.Quest", 1.0f);
    rate_values[RATE_REPUTATION_RECRUIT_A_FRIEND_BONUS]    = sConfigMgr->GetFloatDefault("Rate.Reputation.RecruitAFriendBonus", 0.1f);
    rate_values[RATE_CREATURE_NORMAL_DAMAGE]               = sConfigMgr->GetFloatDefault("Rate.Creature.Normal.Damage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_ELITE_DAMAGE]          = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.Elite.Damage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_RAREELITE_DAMAGE]      = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.RAREELITE.Damage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE]      = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.WORLDBOSS.Damage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_RARE_DAMAGE]           = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.RARE.Damage", 1.0f);
    rate_values[RATE_CREATURE_NORMAL_HP]                   = sConfigMgr->GetFloatDefault("Rate.Creature.Normal.HP", 1.0f);
    rate_values[RATE_CREATURE_ELITE_ELITE_HP]              = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.Elite.HP", 1.0f);
    rate_values[RATE_CREATURE_ELITE_RAREELITE_HP]          = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.RAREELITE.HP", 1.0f);
    rate_values[RATE_CREATURE_ELITE_WORLDBOSS_HP]          = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.WORLDBOSS.HP", 1.0f);
    rate_values[RATE_CREATURE_ELITE_RARE_HP]               = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.RARE.HP", 1.0f);
    rate_values[RATE_CREATURE_NORMAL_SPELLDAMAGE]          = sConfigMgr->GetFloatDefault("Rate.Creature.Normal.SpellDamage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE]     = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.Elite.SpellDamage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE] = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.RAREELITE.SpellDamage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE] = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.WORLDBOSS.SpellDamage", 1.0f);
    rate_values[RATE_CREATURE_ELITE_RARE_SPELLDAMAGE]      = sConfigMgr->GetFloatDefault("Rate.Creature.Elite.RARE.SpellDamage", 1.0f);
    rate_values[RATE_CREATURE_AGGRO]                       = sConfigMgr->GetFloatDefault("Rate.Creature.Aggro", 1.0f);
    rate_values[RATE_REST_INGAME]                          = sConfigMgr->GetFloatDefault("Rate.Rest.InGame", 1.0f);
    rate_values[RATE_REST_OFFLINE_IN_TAVERN_OR_CITY]       = sConfigMgr->GetFloatDefault("Rate.Rest.Offline.InTavernOrCity", 1.0f);
    rate_values[RATE_REST_OFFLINE_IN_WILDERNESS]           = sConfigMgr->GetFloatDefault("Rate.Rest.Offline.InWilderness", 1.0f);
    rate_values[RATE_DAMAGE_FALL]                          = sConfigMgr->GetFloatDefault("Rate.Damage.Fall", 1.0f);
    rate_values[RATE_AUCTION_TIME]                         = sConfigMgr->GetFloatDefault("Rate.Auction.Time", 1.0f);
    rate_values[RATE_AUCTION_DEPOSIT]                      = sConfigMgr->GetFloatDefault("Rate.Auction.Deposit", 1.0f);
    rate_values[RATE_AUCTION_CUT]                          = sConfigMgr->GetFloatDefault("Rate.Auction.Cut", 1.0f);
    rate_values[RATE_HONOR]                                = sConfigMgr->GetFloatDefault("Rate.Honor", 1.0f);
    rate_values[RATE_ARENA_POINTS]                         = sConfigMgr->GetFloatDefault("Rate.ArenaPoints", 1.0f);
    rate_values[RATE_INSTANCE_RESET_TIME]                  = sConfigMgr->GetFloatDefault("Rate.InstanceResetTime", 1.0f);
    rate_values[RATE_TALENT]                               = sConfigMgr->GetFloatDefault("Rate.Talent", 1.0f);
    if (rate_values[RATE_TALENT] < 0.0f)
    {
        sLog->outError("Rate.Talent (%f) must be > 0. Using 1 instead.", rate_values[RATE_TALENT]);
        rate_values[RATE_TALENT] = 1.0f;
    }
    rate_values[RATE_MOVESPEED] = sConfigMgr->GetFloatDefault("Rate.MoveSpeed", 1.0f);
    if (rate_values[RATE_MOVESPEED] < 0)
    {
        sLog->outError("Rate.MoveSpeed (%f) must be > 0. Using 1 instead.", rate_values[RATE_MOVESPEED]);
        rate_values[RATE_MOVESPEED] = 1.0f;
    }
    for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i) playerBaseMoveSpeed[i] = baseMoveSpeed[i] * rate_values[RATE_MOVESPEED];
    rate_values[RATE_CORPSE_DECAY_LOOTED] = sConfigMgr->GetFloatDefault("Rate.Corpse.Decay.Looted", 0.5f);

    rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] = sConfigMgr->GetFloatDefault("TargetPosRecalculateRange", 1.5f);
    if (rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] < CONTACT_DISTANCE)
    {
        sLog->outError("TargetPosRecalculateRange (%f) must be >= %f. Using %f instead.", rate_values[RATE_TARGET_POS_RECALCULATION_RANGE], CONTACT_DISTANCE, CONTACT_DISTANCE);
        rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] = CONTACT_DISTANCE;
    }
    else if (rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] > NOMINAL_MELEE_RANGE)
    {
        sLog->outError("TargetPosRecalculateRange (%f) must be <= %f. Using %f instead.",
            rate_values[RATE_TARGET_POS_RECALCULATION_RANGE], NOMINAL_MELEE_RANGE, NOMINAL_MELEE_RANGE);
        rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] = NOMINAL_MELEE_RANGE;
    }

    rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = sConfigMgr->GetFloatDefault("DurabilityLoss.OnDeath", 10.0f);
    if (rate_values[RATE_DURABILITY_LOSS_ON_DEATH] < 0.0f)
    {
        sLog->outError("DurabilityLoss.OnDeath (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_ON_DEATH]);
        rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = 0.0f;
    }
    if (rate_values[RATE_DURABILITY_LOSS_ON_DEATH] > 100.0f)
    {
        sLog->outError("DurabilityLoss.OnDeath (%f) must be <= 100. Using 100.0 instead.", rate_values[RATE_DURABILITY_LOSS_ON_DEATH]);
        rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = rate_values[RATE_DURABILITY_LOSS_ON_DEATH] / 100.0f;

    rate_values[RATE_DURABILITY_LOSS_DAMAGE] = sConfigMgr->GetFloatDefault("DurabilityLossChance.Damage", 0.5f);
    if (rate_values[RATE_DURABILITY_LOSS_DAMAGE] < 0.0f)
    {
        sLog->outError("DurabilityLossChance.Damage (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_DAMAGE]);
        rate_values[RATE_DURABILITY_LOSS_DAMAGE] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_ABSORB] = sConfigMgr->GetFloatDefault("DurabilityLossChance.Absorb", 0.5f);
    if (rate_values[RATE_DURABILITY_LOSS_ABSORB] < 0.0f)
    {
        sLog->outError("DurabilityLossChance.Absorb (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_ABSORB]);
        rate_values[RATE_DURABILITY_LOSS_ABSORB] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_PARRY] = sConfigMgr->GetFloatDefault("DurabilityLossChance.Parry", 0.05f);
    if (rate_values[RATE_DURABILITY_LOSS_PARRY] < 0.0f)
    {
        sLog->outError("DurabilityLossChance.Parry (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_PARRY]);
        rate_values[RATE_DURABILITY_LOSS_PARRY] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_BLOCK] = sConfigMgr->GetFloatDefault("DurabilityLossChance.Block", 0.05f);
    if (rate_values[RATE_DURABILITY_LOSS_BLOCK] < 0.0f)
    {
        sLog->outError("DurabilityLossChance.Block (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_BLOCK]);
        rate_values[RATE_DURABILITY_LOSS_BLOCK] = 0.0f;
    }
    ///- Read other configuration items from the config file

    bool debugItemDestroy = sConfigMgr->GetBoolDefault("Debug.VerifyItemDestroy", false);
    ObjectMgr::SetDebugItemDestroyEnabled(debugItemDestroy);

    m_bool_configs[CONFIG_DURABILITY_LOSS_IN_PVP] = sConfigMgr->GetBoolDefault("DurabilityLoss.InPvP", false);

    m_int_configs[CONFIG_COMPRESSION] = sConfigMgr->GetIntDefault("Compression", 1);
    if (m_int_configs[CONFIG_COMPRESSION] < 1 || m_int_configs[CONFIG_COMPRESSION] > 9)
    {
        sLog->outError("Compression level (%i) must be in range 1..9. Using default compression level (1).", m_int_configs[CONFIG_COMPRESSION]);
        m_int_configs[CONFIG_COMPRESSION] = 1;
    }
    m_bool_configs[CONFIG_ADDON_CHANNEL]                   = sConfigMgr->GetBoolDefault("AddonChannel", true);
    m_bool_configs[CONFIG_CLEAN_CHARACTER_DB]              = sConfigMgr->GetBoolDefault("CleanCharacterDB", false);
    m_int_configs[CONFIG_PERSISTENT_CHARACTER_CLEAN_FLAGS] = sConfigMgr->GetIntDefault("PersistentCharacterCleanFlags", 0);
    m_int_configs[CONFIG_CHAT_CHANNEL_LEVEL_REQ]           = sConfigMgr->GetIntDefault("ChatLevelReq.Channel", 1);
    m_int_configs[CONFIG_CHAT_WHISPER_LEVEL_REQ]           = sConfigMgr->GetIntDefault("ChatLevelReq.Whisper", 1);
    m_int_configs[CONFIG_CHAT_SAY_LEVEL_REQ]               = sConfigMgr->GetIntDefault("ChatLevelReq.Say", 1);
    m_int_configs[CONFIG_TRADE_LEVEL_REQ]                  = sConfigMgr->GetIntDefault("LevelReq.Trade", 1);
    m_int_configs[CONFIG_TICKET_LEVEL_REQ]                 = sConfigMgr->GetIntDefault("LevelReq.Ticket", 1);
    m_int_configs[CONFIG_AUCTION_LEVEL_REQ]                = sConfigMgr->GetIntDefault("LevelReq.Auction", 1);
    m_int_configs[CONFIG_MAIL_LEVEL_REQ]                   = sConfigMgr->GetIntDefault("LevelReq.Mail", 1);
    m_bool_configs[CONFIG_MAIL_SPAM_ENABLE]                = sConfigMgr->GetBoolDefault("Spam.Mail", 0);
    m_bool_configs[CONFIG_ALLOW_PLAYER_COMMANDS]           = sConfigMgr->GetBoolDefault("AllowPlayerCommands", 1);
    m_bool_configs[CONFIG_PRESERVE_CUSTOM_CHANNELS]        = sConfigMgr->GetBoolDefault("PreserveCustomChannels", false);
    m_int_configs[CONFIG_PRESERVE_CUSTOM_CHANNEL_DURATION] = sConfigMgr->GetIntDefault("PreserveCustomChannelDuration", 14);
    m_int_configs[CONFIG_INTERVAL_DISCONNECT_TOLERANCE]    = sConfigMgr->GetIntDefault("DisconnectToleranceInterval", 0);
    m_bool_configs[CONFIG_STATS_SAVE_ONLY_ON_LOGOUT]       = sConfigMgr->GetBoolDefault("PlayerSave.Stats.SaveOnlyOnLogout", true);

    m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] = sConfigMgr->GetIntDefault("PlayerSave.Stats.MinLevel", 0);
    if (m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] > MAX_LEVEL)
    {
        sLog->outError("PlayerSave.Stats.MinLevel (%i) must be in range 0..80. Using default, do not save character stats (0).", m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE]);
        m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] = 0;
    }

    m_int_configs[CONFIG_INTERVAL_MAPUPDATE] = sConfigMgr->GetIntDefault("MapUpdateInterval", 100);
    if (m_int_configs[CONFIG_INTERVAL_MAPUPDATE] < MIN_MAP_UPDATE_DELAY)
    {
        sLog->outError("MapUpdateInterval (%i) must be greater %u. Use this minimal value.", m_int_configs[CONFIG_INTERVAL_MAPUPDATE], MIN_MAP_UPDATE_DELAY);
        m_int_configs[CONFIG_INTERVAL_MAPUPDATE] = MIN_MAP_UPDATE_DELAY;
    }
    if (reload)
        sMapMgr->SetMapUpdateInterval(m_int_configs[CONFIG_INTERVAL_MAPUPDATE]);

    m_int_configs[CONFIG_INTERVAL_CHANGEWEATHER] = sConfigMgr->GetIntDefault("ChangeWeatherInterval", 10 * MINUTE * IN_MILLISECONDS);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("WorldServerPort", 8085);
        if (val != m_int_configs[CONFIG_PORT_WORLD])
            sLog->outError("WorldServerPort option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_PORT_WORLD]);
    }
    else
        m_int_configs[CONFIG_PORT_WORLD] = sConfigMgr->GetIntDefault("WorldServerPort", 8085);

    m_int_configs[CONFIG_SOCKET_TIMEOUTTIME] = sConfigMgr->GetIntDefault("SocketTimeOutTime", 900000);

    m_int_configs[CONFIG_SESSION_ADD_DELAY] = sConfigMgr->GetIntDefault("Network.SessionAddDelay", 10 * IN_MILLISECONDS);
    m_timers[WUPDATE_COLLECTSESSIONS].SetInterval(m_int_configs[CONFIG_SESSION_ADD_DELAY]);

    m_float_configs[CONFIG_GROUP_XP_DISTANCE]             = sConfigMgr->GetFloatDefault("MaxGroupXPDistance", 74.0f);
    m_float_configs[CONFIG_MAX_RECRUIT_A_FRIEND_DISTANCE] = sConfigMgr->GetFloatDefault("MaxRecruitAFriendBonusDistance", 100.0f);

    /// \todo Add MonsterSight and GuarderSight (with meaning) in worldserver.conf or put them as define
    m_float_configs[CONFIG_SIGHT_MONSTER] = sConfigMgr->GetFloatDefault("MonsterSight", 50);
    m_float_configs[CONFIG_SIGHT_GUARDER] = sConfigMgr->GetFloatDefault("GuarderSight", 50);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("GameType", 0);
        if (val != m_int_configs[CONFIG_GAME_TYPE])
            sLog->outError("GameType option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_GAME_TYPE]);
    }
    else
        m_int_configs[CONFIG_GAME_TYPE] = sConfigMgr->GetIntDefault("GameType", 0);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("RealmZone", REALM_ZONE_DEVELOPMENT);
        if (val != m_int_configs[CONFIG_REALM_ZONE])
            sLog->outError("RealmZone option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_REALM_ZONE]);
    }
    else
        m_int_configs[CONFIG_REALM_ZONE] = sConfigMgr->GetIntDefault("RealmZone", REALM_ZONE_DEVELOPMENT);

    m_int_configs[CONFIG_STRICT_PLAYER_NAMES]  = sConfigMgr->GetIntDefault("StrictPlayerNames", 0);
    m_int_configs[CONFIG_STRICT_CHARTER_NAMES] = sConfigMgr->GetIntDefault("StrictCharterNames", 0);
    m_int_configs[CONFIG_STRICT_CHANNEL_NAMES] = sConfigMgr->GetIntDefault("StrictChannelNames", 0);
    m_int_configs[CONFIG_STRICT_PET_NAMES]     = sConfigMgr->GetIntDefault("StrictPetNames", 0);

    m_int_configs[CONFIG_MIN_PLAYER_NAME] = sConfigMgr->GetIntDefault("MinPlayerName", 2);
    if (m_int_configs[CONFIG_MIN_PLAYER_NAME] < 1 || m_int_configs[CONFIG_MIN_PLAYER_NAME] > MAX_PLAYER_NAME)
    {
        sLog->outError("MinPlayerName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_PLAYER_NAME], MAX_PLAYER_NAME);
        m_int_configs[CONFIG_MIN_PLAYER_NAME] = 2;
    }

    m_int_configs[CONFIG_MIN_CHARTER_NAME] = sConfigMgr->GetIntDefault("MinCharterName", 2);
    if (m_int_configs[CONFIG_MIN_CHARTER_NAME] < 1 || m_int_configs[CONFIG_MIN_CHARTER_NAME] > MAX_CHARTER_NAME)
    {
        sLog->outError("MinCharterName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_CHARTER_NAME], MAX_CHARTER_NAME);
        m_int_configs[CONFIG_MIN_CHARTER_NAME] = 2;
    }

    m_int_configs[CONFIG_MIN_PET_NAME] = sConfigMgr->GetIntDefault("MinPetName", 2);
    if (m_int_configs[CONFIG_MIN_PET_NAME] < 1 || m_int_configs[CONFIG_MIN_PET_NAME] > MAX_PET_NAME)
    {
        sLog->outError("MinPetName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_PET_NAME], MAX_PET_NAME);
        m_int_configs[CONFIG_MIN_PET_NAME] = 2;
    }

    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED]           = sConfigMgr->GetIntDefault("CharacterCreating.Disabled", 0);
    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED_RACEMASK]  = sConfigMgr->GetIntDefault("CharacterCreating.Disabled.RaceMask", 0);
    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED_CLASSMASK] = sConfigMgr->GetIntDefault("CharacterCreating.Disabled.ClassMask", 0);

    m_int_configs[CONFIG_CHARACTERS_PER_REALM] = sConfigMgr->GetIntDefault("CharactersPerRealm", 10);
    if (m_int_configs[CONFIG_CHARACTERS_PER_REALM] < 1 || m_int_configs[CONFIG_CHARACTERS_PER_REALM] > 10)
    {
        sLog->outError("CharactersPerRealm (%i) must be in range 1..10. Set to 10.", m_int_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_CHARACTERS_PER_REALM] = 10;
    }

    // must be after CONFIG_CHARACTERS_PER_REALM
    m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = sConfigMgr->GetIntDefault("CharactersPerAccount", 50);
    if (m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] < m_int_configs[CONFIG_CHARACTERS_PER_REALM])
    {
        sLog->outError("CharactersPerAccount (%i) can't be less than CharactersPerRealm (%i).", m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT], m_int_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = m_int_configs[CONFIG_CHARACTERS_PER_REALM];
    }

    m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] = sConfigMgr->GetIntDefault("HeroicCharactersPerRealm", 1);
    if (int32(m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM]) < 0 || m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] > 10)
    {
        sLog->outError("HeroicCharactersPerRealm (%i) must be in range 0..10. Set to 1.", m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] = 1;
    }

    m_int_configs[CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER] = sConfigMgr->GetIntDefault("CharacterCreating.MinLevelForHeroicCharacter", 55);

    m_int_configs[CONFIG_SKIP_CINEMATICS] = sConfigMgr->GetIntDefault("SkipCinematics", 0);
    if (int32(m_int_configs[CONFIG_SKIP_CINEMATICS]) < 0 || m_int_configs[CONFIG_SKIP_CINEMATICS] > 2)
    {
        sLog->outError("SkipCinematics (%i) must be in range 0..2. Set to 0.", m_int_configs[CONFIG_SKIP_CINEMATICS]);
        m_int_configs[CONFIG_SKIP_CINEMATICS] = 0;
    }

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("MaxPlayerLevel", DEFAULT_MAX_LEVEL);
        if (val != m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
            sLog->outError("MaxPlayerLevel option can't be changed at config reload, using current value (%u).", m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
    }
    else
        m_int_configs[CONFIG_MAX_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("MaxPlayerLevel", DEFAULT_MAX_LEVEL);

    if (m_int_configs[CONFIG_MAX_PLAYER_LEVEL] > MAX_LEVEL)
    {
        sLog->outError("MaxPlayerLevel (%i) must be in range 1..%u. Set to %u.", m_int_configs[CONFIG_MAX_PLAYER_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_int_configs[CONFIG_MAX_PLAYER_LEVEL] = MAX_LEVEL;
    }

    m_int_configs[CONFIG_MIN_DUALSPEC_LEVEL] = sConfigMgr->GetIntDefault("MinDualSpecLevel", 40);

    m_int_configs[CONFIG_START_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("StartPlayerLevel", 1);
    if (m_int_configs[CONFIG_START_PLAYER_LEVEL] < 1)
    {
        sLog->outError("StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 1.", m_int_configs[CONFIG_START_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_PLAYER_LEVEL] = 1;
    }
    else if (m_int_configs[CONFIG_START_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog->outError("StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.", m_int_configs[CONFIG_START_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_PLAYER_LEVEL] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("StartHeroicPlayerLevel", 55);
    if (m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] < 1)
    {
        sLog->outError("StartHeroicPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 55.",
            m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = 55;
    }
    else if (m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog->outError("StartHeroicPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.",
            m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    m_int_configs[CONFIG_START_PLAYER_MONEY] = sConfigMgr->GetIntDefault("StartPlayerMoney", 0);
    if (int32(m_int_configs[CONFIG_START_PLAYER_MONEY]) < 0)
    {
        sLog->outError("StartPlayerMoney (%i) must be in range 0..%u. Set to %u.", m_int_configs[CONFIG_START_PLAYER_MONEY], MAX_MONEY_AMOUNT, 0);
        m_int_configs[CONFIG_START_PLAYER_MONEY] = 0;
    }
    else if (m_int_configs[CONFIG_START_PLAYER_MONEY] > MAX_MONEY_AMOUNT)
    {
        sLog->outError("StartPlayerMoney (%i) must be in range 0..%u. Set to %u.",
            m_int_configs[CONFIG_START_PLAYER_MONEY], MAX_MONEY_AMOUNT, MAX_MONEY_AMOUNT);
        m_int_configs[CONFIG_START_PLAYER_MONEY] = MAX_MONEY_AMOUNT;
    }

    m_int_configs[CONFIG_MAX_HONOR_POINTS] = sConfigMgr->GetIntDefault("MaxHonorPoints", 75000);
    if (int32(m_int_configs[CONFIG_MAX_HONOR_POINTS]) < 0)
    {
        sLog->outError("MaxHonorPoints (%i) can't be negative. Set to 0.", m_int_configs[CONFIG_MAX_HONOR_POINTS]);
        m_int_configs[CONFIG_MAX_HONOR_POINTS] = 0;
    }

    m_int_configs[CONFIG_START_HONOR_POINTS] = sConfigMgr->GetIntDefault("StartHonorPoints", 0);
    if (int32(m_int_configs[CONFIG_START_HONOR_POINTS]) < 0)
    {
        sLog->outError("StartHonorPoints (%i) must be in range 0..MaxHonorPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_HONOR_POINTS], m_int_configs[CONFIG_MAX_HONOR_POINTS], 0);
        m_int_configs[CONFIG_START_HONOR_POINTS] = 0;
    }
    else if (m_int_configs[CONFIG_START_HONOR_POINTS] > m_int_configs[CONFIG_MAX_HONOR_POINTS])
    {
        sLog->outError("StartHonorPoints (%i) must be in range 0..MaxHonorPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_HONOR_POINTS], m_int_configs[CONFIG_MAX_HONOR_POINTS], m_int_configs[CONFIG_MAX_HONOR_POINTS]);
        m_int_configs[CONFIG_START_HONOR_POINTS] = m_int_configs[CONFIG_MAX_HONOR_POINTS];
    }

    m_int_configs[CONFIG_MAX_ARENA_POINTS] = sConfigMgr->GetIntDefault("MaxArenaPoints", 10000);
    if (int32(m_int_configs[CONFIG_MAX_ARENA_POINTS]) < 0)
    {
        sLog->outError("MaxArenaPoints (%i) can't be negative. Set to 0.", m_int_configs[CONFIG_MAX_ARENA_POINTS]);
        m_int_configs[CONFIG_MAX_ARENA_POINTS] = 0;
    }

    m_int_configs[CONFIG_START_ARENA_POINTS] = sConfigMgr->GetIntDefault("StartArenaPoints", 0);
    if (int32(m_int_configs[CONFIG_START_ARENA_POINTS]) < 0)
    {
        sLog->outError("StartArenaPoints (%i) must be in range 0..MaxArenaPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_ARENA_POINTS], m_int_configs[CONFIG_MAX_ARENA_POINTS], 0);
        m_int_configs[CONFIG_START_ARENA_POINTS] = 0;
    }
    else if (m_int_configs[CONFIG_START_ARENA_POINTS] > m_int_configs[CONFIG_MAX_ARENA_POINTS])
    {
        sLog->outError("StartArenaPoints (%i) must be in range 0..MaxArenaPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_ARENA_POINTS], m_int_configs[CONFIG_MAX_ARENA_POINTS], m_int_configs[CONFIG_MAX_ARENA_POINTS]);
        m_int_configs[CONFIG_START_ARENA_POINTS] = m_int_configs[CONFIG_MAX_ARENA_POINTS];
    }

    m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("RecruitAFriend.MaxLevel", 60);
    if (m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog->outError("RecruitAFriend.MaxLevel (%i) must be in the range 0..MaxLevel(%u). Set to %u.",
            m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], 60);
        m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] = 60;
    }

    m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL_DIFFERENCE] = sConfigMgr->GetIntDefault("RecruitAFriend.MaxDifference", 4);
    m_bool_configs[CONFIG_ALL_TAXI_PATHS]                                    = sConfigMgr->GetBoolDefault("AllFlightPaths", false);
    m_bool_configs[CONFIG_INSTANT_TAXI]                                      = sConfigMgr->GetBoolDefault("InstantFlightPaths", false);

    m_int_configs[CONFIG_SERVER_RESET_TIME_HOUR]                 = sConfigMgr->GetIntDefault("Server.ResetTimeHour", 11);
    m_int_configs[CONFIG_SERVER_RESET_TIME_MINUTE]               = sConfigMgr->GetIntDefault("Server.ResetTimeMinute", 0);

    m_bool_configs[CONFIG_INSTANCE_IGNORE_LEVEL]                 = sConfigMgr->GetBoolDefault("Instance.IgnoreLevel", false);
    m_bool_configs[CONFIG_INSTANCE_IGNORE_RAID]                  = sConfigMgr->GetBoolDefault("Instance.IgnoreRaid", false);
    m_int_configs[CONFIG_INSTANCE_UNLOAD_DELAY]                  = sConfigMgr->GetIntDefault("Instance.UnloadDelay", 30 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_INSTANCE_RESET_TIME_RELATIVE_TIMESTAMP] = sConfigMgr->GetIntDefault("Instance.ResetTimeRelativeTimestamp", 1135814400);
    m_int_configs[CONFIG_INSTANCE_RESET_TIME_ZONE]               = sConfigMgr->GetIntDefault("Instance.ResetTimeZone", 2);

    m_int_configs[CONFIG_MAX_PRIMARY_TRADE_SKILL] = sConfigMgr->GetIntDefault("MaxPrimaryTradeSkill", 2);
    m_int_configs[CONFIG_MIN_PETITION_SIGNS]      = sConfigMgr->GetIntDefault("MinPetitionSigns", 9);
    if (m_int_configs[CONFIG_MIN_PETITION_SIGNS] > 9)
    {
        sLog->outError("MinPetitionSigns (%i) must be in range 0..9. Set to 9.", m_int_configs[CONFIG_MIN_PETITION_SIGNS]);
        m_int_configs[CONFIG_MIN_PETITION_SIGNS] = 9;
    }

    m_bool_configs[CONFIG_HK_REWARDS_ENABLE] = sConfigMgr->GetBoolDefault("PvPRank.Rewards", false);
    rate_values[RATE_PVP_RANK_EXTRA_HONOR]   = sConfigMgr->GetFloatDefault("PvPRank.Rate.ExtraHonor", 0);
    std::string s_pvp_ranks                  = sConfigMgr->GetStringDefault("PvPRank.HKPerRank", "10,50,100,200,450,750,1300,2000,3500,6000,9500,15000,21000,30000");
    char* c_pvp_ranks                        = const_cast<char*>(s_pvp_ranks.c_str());
    for (int i = 0; i != HKRANKMAX; i++)
    {
        if (i == 0)
            pvp_ranks[0] = 0;
        else if (i == 1)
            pvp_ranks[1] = atoi(strtok(c_pvp_ranks, ","));
        else
            pvp_ranks[i] = atoi(strtok(nullptr, ","));
    }

    m_int_configs[CONFIG_GM_LOGIN_STATE]   = sConfigMgr->GetIntDefault("GM.LoginState", 2);
    m_int_configs[CONFIG_GM_VISIBLE_STATE] = sConfigMgr->GetIntDefault("GM.Visible", 2);
    m_int_configs[CONFIG_GM_CHAT]          = sConfigMgr->GetIntDefault("GM.Chat", 2);
    m_int_configs[CONFIG_GM_WHISPERING_TO] = sConfigMgr->GetIntDefault("GM.WhisperingTo", 2);

    m_int_configs[CONFIG_GM_LEVEL_IN_GM_LIST]  = sConfigMgr->GetIntDefault("GM.InGMList.Level", SEC_ADMINISTRATOR);
    m_int_configs[CONFIG_GM_LEVEL_IN_WHO_LIST] = sConfigMgr->GetIntDefault("GM.InWhoList.Level", SEC_ADMINISTRATOR);
    m_bool_configs[CONFIG_GM_LOG_TRADE]        = sConfigMgr->GetBoolDefault("GM.LogTrade", false);
    m_int_configs[CONFIG_START_GM_LEVEL]       = sConfigMgr->GetIntDefault("GM.StartLevel", 1);
    if (m_int_configs[CONFIG_START_GM_LEVEL] < m_int_configs[CONFIG_START_PLAYER_LEVEL])
    {
        sLog->outError("GM.StartLevel (%i) must be in range StartPlayerLevel(%u)..%u. Set to %u.",
            m_int_configs[CONFIG_START_GM_LEVEL], m_int_configs[CONFIG_START_PLAYER_LEVEL], MAX_LEVEL, m_int_configs[CONFIG_START_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_GM_LEVEL] = m_int_configs[CONFIG_START_PLAYER_LEVEL];
    }
    else if (m_int_configs[CONFIG_START_GM_LEVEL] > MAX_LEVEL)
    {
        sLog->outError("GM.StartLevel (%i) must be in range 1..%u. Set to %u.", m_int_configs[CONFIG_START_GM_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_int_configs[CONFIG_START_GM_LEVEL] = MAX_LEVEL;
    }
    m_bool_configs[CONFIG_ALLOW_GM_GROUP]       = sConfigMgr->GetBoolDefault("GM.AllowInvite", false);
    m_bool_configs[CONFIG_ALLOW_GM_FRIEND]      = sConfigMgr->GetBoolDefault("GM.AllowFriend", false);
    m_bool_configs[CONFIG_GM_LOWER_SECURITY]    = sConfigMgr->GetBoolDefault("GM.LowerSecurity", false);
    m_float_configs[CONFIG_CHANCE_OF_GM_SURVEY] = sConfigMgr->GetFloatDefault("GM.TicketSystem.ChanceOfGMSurvey", 50.0f);

    m_int_configs[CONFIG_GROUP_VISIBILITY] = sConfigMgr->GetIntDefault("Visibility.GroupMode", 1);

    m_int_configs[CONFIG_MAIL_DELIVERY_DELAY] = sConfigMgr->GetIntDefault("MailDeliveryDelay", MINUTE);

    m_int_configs[CONFIG_UPTIME_UPDATE] = sConfigMgr->GetIntDefault("UpdateUptimeInterval", 10);
    if (int32(m_int_configs[CONFIG_UPTIME_UPDATE]) <= 0)
    {
        sLog->outError("UpdateUptimeInterval (%i) must be > 0, set to default 10.", m_int_configs[CONFIG_UPTIME_UPDATE]);
        m_int_configs[CONFIG_UPTIME_UPDATE] = 1;
    }

    // log db cleanup interval
    m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] = sConfigMgr->GetIntDefault("LogDB.Opt.ClearInterval", 10);
    if (int32(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]) <= 0)
    {
        sLog->outError("LogDB.Opt.ClearInterval (%i) must be > 0, set to default 10.", m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]);
        m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] = 10;
    }
    if (reload)
    {
        m_timers[WUPDATE_CLEANDB].SetInterval(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] * MINUTE * IN_MILLISECONDS);
        m_timers[WUPDATE_CLEANDB].Reset();
    }

    m_int_configs[CONFIG_TELEPORT_TIMEOUT_NEAR]             = sConfigMgr->GetIntDefault("TeleportTimeoutNear", 25);             // pussywizard
    m_int_configs[CONFIG_TELEPORT_TIMEOUT_FAR]              = sConfigMgr->GetIntDefault("TeleportTimeoutFar", 45);              // pussywizard
    m_int_configs[CONFIG_MAX_ALLOWED_MMR_DROP]              = sConfigMgr->GetIntDefault("MaxAllowedMMRDrop", 500);              // pussywizard
    m_bool_configs[CONFIG_ENABLE_LOGIN_AFTER_DC]            = sConfigMgr->GetBoolDefault("EnableLoginAfterDC", true);           // pussywizard
    m_bool_configs[CONFIG_DONT_CACHE_RANDOM_MOVEMENT_PATHS] = sConfigMgr->GetBoolDefault("DontCacheRandomMovementPaths", true); // pussywizard

    m_int_configs[CONFIG_SKILL_CHANCE_ORANGE] = sConfigMgr->GetIntDefault("SkillChance.Orange", 100);
    m_int_configs[CONFIG_SKILL_CHANCE_YELLOW] = sConfigMgr->GetIntDefault("SkillChance.Yellow", 75);
    m_int_configs[CONFIG_SKILL_CHANCE_GREEN]  = sConfigMgr->GetIntDefault("SkillChance.Green", 25);
    m_int_configs[CONFIG_SKILL_CHANCE_GREY]   = sConfigMgr->GetIntDefault("SkillChance.Grey", 0);

    m_int_configs[CONFIG_SKILL_CHANCE_MINING_STEPS]   = sConfigMgr->GetIntDefault("SkillChance.MiningSteps", 75);
    m_int_configs[CONFIG_SKILL_CHANCE_SKINNING_STEPS] = sConfigMgr->GetIntDefault("SkillChance.SkinningSteps", 75);

    m_bool_configs[CONFIG_SKILL_PROSPECTING] = sConfigMgr->GetBoolDefault("SkillChance.Prospecting", false);
    m_bool_configs[CONFIG_SKILL_MILLING]     = sConfigMgr->GetBoolDefault("SkillChance.Milling", false);

    m_int_configs[CONFIG_SKILL_GAIN_CRAFTING] = sConfigMgr->GetIntDefault("SkillGain.Crafting", 1);

    m_int_configs[CONFIG_SKILL_GAIN_DEFENSE] = sConfigMgr->GetIntDefault("SkillGain.Defense", 1);

    m_int_configs[CONFIG_SKILL_GAIN_GATHERING] = sConfigMgr->GetIntDefault("SkillGain.Gathering", 1);

    m_int_configs[CONFIG_SKILL_GAIN_WEAPON] = sConfigMgr->GetIntDefault("SkillGain.Weapon", 1);

    m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] = sConfigMgr->GetIntDefault("MaxOverspeedPings", 2);
    if (m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] != 0 && m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] < 2)
    {
        sLog->outError("MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check). Set to 2.", m_int_configs[CONFIG_MAX_OVERSPEED_PINGS]);
        m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] = 2;
    }

    m_bool_configs[CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY] = sConfigMgr->GetBoolDefault("SaveRespawnTimeImmediately", true);
    m_bool_configs[CONFIG_WEATHER]                       = sConfigMgr->GetBoolDefault("ActivateWeather", true);

    m_int_configs[CONFIG_DISABLE_BREATHING] = sConfigMgr->GetIntDefault("DisableWaterBreath", SEC_CONSOLE);

    m_bool_configs[CONFIG_ALWAYS_MAX_SKILL_FOR_LEVEL] = sConfigMgr->GetBoolDefault("AlwaysMaxSkillForLevel", false);

    if (reload)
    {
        uint32 val = sConfigMgr->GetIntDefault("Expansion", 2);
        if (val != m_int_configs[CONFIG_EXPANSION])
            sLog->outError("Expansion option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_EXPANSION]);
    }
    else
        m_int_configs[CONFIG_EXPANSION] = sConfigMgr->GetIntDefault("Expansion", 2);

    m_int_configs[CONFIG_CHATFLOOD_MESSAGE_COUNT] = sConfigMgr->GetIntDefault("ChatFlood.MessageCount", 10);
    m_int_configs[CONFIG_CHATFLOOD_MESSAGE_DELAY] = sConfigMgr->GetIntDefault("ChatFlood.MessageDelay", 1);
    m_int_configs[CONFIG_CHATFLOOD_MUTE_TIME]     = sConfigMgr->GetIntDefault("ChatFlood.MuteTime", 10);

    m_int_configs[CONFIG_EVENT_ANNOUNCE] = sConfigMgr->GetIntDefault("Event.Announce", 0);

    m_float_configs[CONFIG_CREATURE_EVADE_HOMEDIST]                = sConfigMgr->GetFloatDefault("Creature.Evade.DistanceToHome", 60.0f);
    m_float_configs[CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS] = sConfigMgr->GetFloatDefault("CreatureFamilyFleeAssistanceRadius", 30.0f);
    m_float_configs[CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS]      = sConfigMgr->GetFloatDefault("CreatureFamilyAssistanceRadius", 10.0f);
    m_int_configs[CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY]         = sConfigMgr->GetIntDefault("CreatureFamilyAssistanceDelay", 1500);
    m_int_configs[CONFIG_CREATURE_FAMILY_FLEE_DELAY]               = sConfigMgr->GetIntDefault("CreatureFamilyFleeDelay", 7000);

    m_int_configs[CONFIG_WORLD_BOSS_LEVEL_DIFF] = sConfigMgr->GetIntDefault("WorldBossLevelDiff", 3);

    // note: disable value (-1) will assigned as 0xFFFFFFF, to prevent overflow at calculations limit it to max possible player level MAX_LEVEL(100)
    m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = sConfigMgr->GetIntDefault("Quests.LowLevelHideDiff", 4);
    if (m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = sConfigMgr->GetIntDefault("Quests.HighLevelHideDiff", 7);
    if (m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    m_bool_configs[CONFIG_QUEST_IGNORE_RAID]          = sConfigMgr->GetBoolDefault("Quests.IgnoreRaid", false);
    m_bool_configs[CONFIG_QUEST_IGNORE_AUTO_ACCEPT]   = sConfigMgr->GetBoolDefault("Quests.IgnoreAutoAccept", false);
    m_bool_configs[CONFIG_QUEST_IGNORE_AUTO_COMPLETE] = sConfigMgr->GetBoolDefault("Quests.IgnoreAutoComplete", false);

    m_bool_configs[CONFIG_DETECT_POS_COLLISION] = sConfigMgr->GetBoolDefault("DetectPosCollision", true);

    m_bool_configs[CONFIG_RESTRICTED_LFG_CHANNEL]      = sConfigMgr->GetBoolDefault("Channel.RestrictedLfg", true);
    m_bool_configs[CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL] = sConfigMgr->GetBoolDefault("Channel.SilentlyGMJoin", false);

    m_bool_configs[CONFIG_TALENTS_INSPECTING]                = sConfigMgr->GetBoolDefault("TalentsInspecting", true);
    m_bool_configs[CONFIG_CHAT_FAKE_MESSAGE_PREVENTING]      = sConfigMgr->GetBoolDefault("ChatFakeMessagePreventing", false);
    m_int_configs[CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY] = sConfigMgr->GetIntDefault("ChatStrictLinkChecking.Severity", 0);
    m_int_configs[CONFIG_CHAT_STRICT_LINK_CHECKING_KICK]     = sConfigMgr->GetIntDefault("ChatStrictLinkChecking.Kick", 0);

    m_int_configs[CONFIG_CORPSE_DECAY_NORMAL]    = sConfigMgr->GetIntDefault("Corpse.Decay.NORMAL", 60);
    m_int_configs[CONFIG_CORPSE_DECAY_RARE]      = sConfigMgr->GetIntDefault("Corpse.Decay.RARE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_ELITE]     = sConfigMgr->GetIntDefault("Corpse.Decay.ELITE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_RAREELITE] = sConfigMgr->GetIntDefault("Corpse.Decay.RAREELITE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_WORLDBOSS] = sConfigMgr->GetIntDefault("Corpse.Decay.WORLDBOSS", 3600);

    m_int_configs[CONFIG_DEATH_SICKNESS_LEVEL]            = sConfigMgr->GetIntDefault("Death.SicknessLevel", 11);
    m_bool_configs[CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVP] = sConfigMgr->GetBoolDefault("Death.CorpseReclaimDelay.PvP", true);
    m_bool_configs[CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVE] = sConfigMgr->GetBoolDefault("Death.CorpseReclaimDelay.PvE", true);
    m_bool_configs[CONFIG_DEATH_BONES_WORLD]              = sConfigMgr->GetBoolDefault("Death.Bones.World", true);
    m_bool_configs[CONFIG_DEATH_BONES_BG_OR_ARENA]        = sConfigMgr->GetBoolDefault("Death.Bones.BattlegroundOrArena", true);

    m_bool_configs[CONFIG_DIE_COMMAND_MODE] = sConfigMgr->GetBoolDefault("Die.Command.Mode", true);

    // always use declined names in the russian client
    m_bool_configs[CONFIG_DECLINED_NAMES_USED] =
        (m_int_configs[CONFIG_REALM_ZONE] == REALM_ZONE_RUSSIAN) ? true : sConfigMgr->GetBoolDefault("DeclinedNames", false);

    m_float_configs[CONFIG_LISTEN_RANGE_SAY]       = sConfigMgr->GetFloatDefault("ListenRange.Say", 25.0f);
    m_float_configs[CONFIG_LISTEN_RANGE_TEXTEMOTE] = sConfigMgr->GetFloatDefault("ListenRange.TextEmote", 25.0f);
    m_float_configs[CONFIG_LISTEN_RANGE_YELL]      = sConfigMgr->GetFloatDefault("ListenRange.Yell", 300.0f);

    m_bool_configs[CONFIG_BATTLEGROUND_CAST_DESERTER]               = sConfigMgr->GetBoolDefault("Battleground.CastDeserter", true);
    m_bool_configs[CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE]      = sConfigMgr->GetBoolDefault("Battleground.QueueAnnouncer.Enable", false);
    m_int_configs[CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER]       = sConfigMgr->GetIntDefault("Battleground.PrematureFinishTimer", 5 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH] = sConfigMgr->GetIntDefault("Battleground.PremadeGroupWaitForMatch", 30 * MINUTE * IN_MILLISECONDS);
    m_bool_configs[CONFIG_BATTLEGROUND_STORE_STATISTICS_ENABLE]     = sConfigMgr->GetBoolDefault("Battleground.StoreStatistics.Enable", false);
    m_bool_configs[CONFIG_BG_XP_FOR_KILL]                           = sConfigMgr->GetBoolDefault("Battleground.GiveXPForKills", false);

    // Arenas
    m_int_configs[CONFIG_ARENA_MAX_RATING_DIFFERENCE]               = sConfigMgr->GetIntDefault("Arena.MaxRatingDifference", 150);
    m_int_configs[CONFIG_ARENA_RATING_DISCARD_TIMER]                = sConfigMgr->GetIntDefault("Arena.RatingDiscardTimer", 10 * MINUTE * IN_MILLISECONDS);
    m_bool_configs[CONFIG_ARENA_AUTO_DISTRIBUTE_POINTS]             = sConfigMgr->GetBoolDefault("Arena.AutoDistributePoints", false);
    m_int_configs[CONFIG_ARENA_AUTO_DISTRIBUTE_DAY]                 = sConfigMgr->GetIntDefault("Arena.AutoDistributeDay", 3);
    m_int_configs[CONFIG_ARENA_AUTO_DISTRIBUTE_HOUR]                = sConfigMgr->GetIntDefault("Arena.AutoDistributeHour", 11);
    m_int_configs[CONFIG_ARENA_AUTO_DISTRIBUTE_MINUTE]              = sConfigMgr->GetIntDefault("Arena.AutoDistributeMinute", 0);
    m_int_configs[CONFIG_ARENA_SEASON_ID]                           = sConfigMgr->GetIntDefault("Arena.ArenaSeason.ID", 1);
    m_int_configs[CONFIG_ARENA_START_RATING]                        = sConfigMgr->GetIntDefault("Arena.ArenaStartRating", 1000);
    m_int_configs[CONFIG_ARENA_START_PERSONAL_RATING]               = sConfigMgr->GetIntDefault("Arena.ArenaStartPersonalRating", 1000);
    m_int_configs[CONFIG_ARENA_START_MATCHMAKER_RATING]             = sConfigMgr->GetIntDefault("Arena.ArenaStartMatchmakerRating", 1500);
    m_bool_configs[CONFIG_ARENA_SEASON_IN_PROGRESS]                 = sConfigMgr->GetBoolDefault("Arena.ArenaSeason.InProgress", true);
    m_float_configs[CONFIG_ARENA_WIN_RATING_MODIFIER_1]             = sConfigMgr->GetFloatDefault("Arena.ArenaWinRatingModifier1", 48.0f);
    m_float_configs[CONFIG_ARENA_WIN_RATING_MODIFIER_2]             = sConfigMgr->GetFloatDefault("Arena.ArenaWinRatingModifier2", 24.0f);
    m_float_configs[CONFIG_ARENA_LOSE_RATING_MODIFIER]              = sConfigMgr->GetFloatDefault("Arena.ArenaLoseRatingModifier", 24.0f);
    m_float_configs[CONFIG_ARENA_MATCHMAKER_RATING_MODIFIER]        = sConfigMgr->GetFloatDefault("Arena.ArenaMatchmakerRatingModifier", 24.0f);
    m_bool_configs[CONFIG_ARENA_WORLD_ANNOUNCER]                    = sConfigMgr->GetBoolDefault("Arena.World.Announcer", true);
    m_int_configs[CONFIG_ARENA_MAX_WIN_MMR]                         = sConfigMgr->GetIntDefault("Arena.MaxWinMMR", 0);
    m_int_configs[CONFIG_ARENA_MAX_LOSE_MMR]                        = sConfigMgr->GetIntDefault("Arena.MaxLoseMMR", 12);
    m_int_configs[CONFIG_ARENA_MAX_WIN_RATING]                      = sConfigMgr->GetIntDefault("Arena.MaxWinRating", 0);
    m_int_configs[CONFIG_ARENA_MAX_LOSE_RATING]                     = sConfigMgr->GetIntDefault("Arena.MaxLoseRating", 12);
    m_int_configs[CONFIG_ARENA_MAX_MMR_DIFF]                        = sConfigMgr->GetIntDefault("Arena.MaxMMRDiff", 300);

    m_bool_configs[CONFIG_OFFHAND_CHECK_AT_SPELL_UNLEARN] = sConfigMgr->GetBoolDefault("OffhandCheckAtSpellUnlearn", true);

    if (int32 clientCacheId = sConfigMgr->GetIntDefault("ClientCacheVersion", 0))
    {
        // overwrite DB/old value
        if (clientCacheId > 0)
        {
            m_int_configs[CONFIG_CLIENTCACHE_VERSION] = clientCacheId;
            sLog->outString("Client cache version set to: %u", clientCacheId);
        }
        else
            sLog->outError("ClientCacheVersion can't be negative %d, ignored.", clientCacheId);
    }

    m_int_configs[CONFIG_INSTANT_LOGOUT] = sConfigMgr->GetIntDefault("InstantLogout", SEC_MODERATOR);

    m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] = sConfigMgr->GetIntDefault("Guild.EventLogRecordsCount", GUILD_EVENTLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] > GUILD_EVENTLOG_MAX_RECORDS)
        m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] = GUILD_EVENTLOG_MAX_RECORDS;
    m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] = sConfigMgr->GetIntDefault("Guild.BankEventLogRecordsCount", GUILD_BANKLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] > GUILD_BANKLOG_MAX_RECORDS)
        m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] = GUILD_BANKLOG_MAX_RECORDS;

    m_bool_configs[CONFIG_MYTHIC_ENABLE] = sConfigMgr->GetBoolDefault("Mythic.Enable", false);

    m_bool_configs[CONFIG_ACTIVE_WAYPOINT_CREATURES_ENABLE] = sConfigMgr->GetBoolDefault("ActiveWaypointCreatures.Enable", false);

    m_int_configs[CONFIG_CROSSFACTION_ENABLED_FEATURES] = sConfigMgr->GetIntDefault("Crossfaction.EnabledFeatures", to_underlying(Crossfaction::All));

    m_int_configs[CONFIG_SPELL_BATCHING_DELAY] = sConfigMgr->GetIntDefault("SpellBatching.Delay", 400);
    m_int_configs[CONFIG_SPELL_QUEUE_MAX_WINDOW] = sConfigMgr->GetIntDefault("SpellQueue.MaxWindow", 400);

    m_int_configs[CONFIG_CHAT_RESTRICTION_MASK] = sConfigMgr->GetIntDefault("ChatRestrictionMask", 0);

    //visibility on continents
    m_MaxVisibleDistanceOnContinents = sConfigMgr->GetFloatDefault("Visibility.Distance.Continents", VisibilityConstants::DEFAULT_VISIBILITY_DISTANCE);
    if (m_MaxVisibleDistanceOnContinents < 45 * sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        sLog->outError("Visibility.Distance.Continents can't be less max aggro radius %f", 45 * sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceOnContinents = 45 * sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceOnContinents > VisibilityConstants::MAX_VISIBILITY_DISTANCE)
    {
        sLog->outError("Visibility.Distance.Continents can't be greater %f", VisibilityConstants::MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceOnContinents = VisibilityConstants::MAX_VISIBILITY_DISTANCE;
    }

    //visibility in instances
    m_MaxVisibleDistanceInInstances = sConfigMgr->GetFloatDefault("Visibility.Distance.Instances", VisibilityConstants::DEFAULT_VISIBILITY_INSTANCE);
    if (m_MaxVisibleDistanceInInstances < 45 * sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        sLog->outError("Visibility.Distance.Instances can't be less max aggro radius %f", 45 * sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInInstances = 45 * sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInInstances > VisibilityConstants::MAX_VISIBILITY_DISTANCE)
    {
        sLog->outError("Visibility.Distance.Instances can't be greater %f", VisibilityConstants::MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInInstances = VisibilityConstants::MAX_VISIBILITY_DISTANCE;
    }

    //visibility in Battlegrounds
    m_MaxVisibleDistanceInBattlegrounds = sConfigMgr->GetFloatDefault("Visibility.Distance.Battlegrounds", VisibilityConstants::DEFAULT_VISIBILITY_BATTLEGROUNDS);
    if (m_MaxVisibleDistanceInBattlegrounds < 45 * sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        sLog->outError("Visibility.Distance.BGArenas can't be less max aggro radius %f", 45 * sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInBattlegrounds = 45 * sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInBattlegrounds > VisibilityConstants::MAX_VISIBILITY_DISTANCE)
    {
        sLog->outError("Visibility.Distance.Battlegrounds can't be greater %f", VisibilityConstants::MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInBattlegrounds = VisibilityConstants::MAX_VISIBILITY_DISTANCE;
    }

    // Visibility in arenas
    m_MaxVisibleDistanceInArenas = sConfigMgr->GetFloatDefault("Visibility.Distance.Arenas", VisibilityConstants::DEFAULT_VISIBILITY_ARENAS);
    if (m_MaxVisibleDistanceInArenas > VisibilityConstants::MAX_VISIBILITY_DISTANCE)
    {
        sLog->outError("Visibility.Distance.Arenas can't be greater %f", VisibilityConstants::MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInArenas = VisibilityConstants::MAX_VISIBILITY_DISTANCE;
    }

    if (reload)
    {
        sMapMgr->DoForAllMaps([](Map* map) {
            map->UpdateVisibilityRange();
        });
    }

    ///- Load the CharDelete related config options
    m_int_configs[CONFIG_CHARDELETE_METHOD]    = sConfigMgr->GetIntDefault("CharDelete.Method", 0);
    m_int_configs[CONFIG_CHARDELETE_MIN_LEVEL] = sConfigMgr->GetIntDefault("CharDelete.MinLevel", 0);
    m_int_configs[CONFIG_CHARDELETE_KEEP_DAYS] = sConfigMgr->GetIntDefault("CharDelete.KeepDays", 30);

    sWorldCache.LoadAuctionHouseLoggedItems();

    std::string luaScriptsPath = sConfigMgr->GetStringDefault("LuaScriptsDir", "./");
    if (luaScriptsPath.empty() || (luaScriptsPath.at(luaScriptsPath.length() - 1) != '/' && luaScriptsPath.at(luaScriptsPath.length() - 1) != '\\'))
        luaScriptsPath.push_back('/');

    ///- Read the "Data" directory from the config file
    std::string dataPath = sConfigMgr->GetStringDefault("DataDir", "./");
    if (dataPath.empty() || (dataPath.at(dataPath.length() - 1) != '/' && dataPath.at(dataPath.length() - 1) != '\\'))
        dataPath.push_back('/');

#if PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_APPLE
    if (dataPath[0] == '~')
    {
        const char* home = getenv("HOME");
        if (home)
            dataPath.replace(0, 1, home);
    }
#endif

    if (!reload)
    {
        m_luaScriptsPath = luaScriptsPath;
        sLog->outString("Using LuaScriptPath %s", luaScriptsPath.c_str());
    }

    if (reload)
    {
        if (dataPath != m_dataPath)
            sLog->outError("DataDir option can't be changed at worldserver.conf reload, using current value (%s).", m_dataPath.c_str());
    }
    else
    {
        m_dataPath = dataPath;
        sLog->outString("Using DataDir %s", m_dataPath.c_str());
    }

    m_bool_configs[CONFIG_VMAP_INDOOR_CHECK] = sConfigMgr->GetBoolDefault("vmap.enableIndoorCheck", 0);
    bool enableIndoor                        = sConfigMgr->GetBoolDefault("vmap.enableIndoorCheck", true);
    bool enableLOS                           = sConfigMgr->GetBoolDefault("vmap.enableLOS", true);
    bool enableHeight                        = sConfigMgr->GetBoolDefault("vmap.enableHeight", true);
    bool enablePetLOS                        = sConfigMgr->GetBoolDefault("vmap.petLOS", true);

    if (!enableHeight)
        sLog->outError("VMap height checking disabled! Creatures movements and other various things WILL be broken! Expect no support.");

    VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(enableLOS);
    VMAP::VMapFactory::createOrGetVMapManager()->setEnableHeightCalc(enableHeight);
    sLog->outString("WORLD: VMap support included. LineOfSight:%i, getHeight:%i, indoorCheck:%i PetLOS:%i", enableLOS, enableHeight, enableIndoor, enablePetLOS);

    m_bool_configs[CONFIG_PET_LOS]          = sConfigMgr->GetBoolDefault("vmap.petLOS", true);
    m_bool_configs[CONFIG_START_ALL_SPELLS] = sConfigMgr->GetBoolDefault("PlayerStart.AllSpells", false);
    if (m_bool_configs[CONFIG_START_ALL_SPELLS])
        sLog->outString("WORLD: WARNING: PlayerStart.AllSpells enabled - may not function as intended!");
    m_int_configs[CONFIG_HONOR_AFTER_DUEL]      = sConfigMgr->GetIntDefault("HonorPointsAfterDuel", 0);
    m_bool_configs[CONFIG_START_ALL_EXPLORED]   = sConfigMgr->GetBoolDefault("PlayerStart.MapsExplored", false);
    m_bool_configs[CONFIG_START_ALL_REP]        = sConfigMgr->GetBoolDefault("PlayerStart.AllReputation", false);
    m_bool_configs[CONFIG_ALWAYS_MAXSKILL]      = sConfigMgr->GetBoolDefault("AlwaysMaxWeaponSkill", false);
    m_bool_configs[CONFIG_PVP_TOKEN_ENABLE]     = sConfigMgr->GetBoolDefault("PvPToken.Enable", false);
    m_int_configs[CONFIG_PVP_TOKEN_MAP_TYPE]    = sConfigMgr->GetIntDefault("PvPToken.MapAllowType", 4);
    m_int_configs[CONFIG_PVP_TOKEN_ID]          = sConfigMgr->GetIntDefault("PvPToken.ItemID", 29434);
    m_int_configs[CONFIG_PVP_TOKEN_COUNT]       = sConfigMgr->GetIntDefault("PvPToken.ItemCount", 1);
    if (m_int_configs[CONFIG_PVP_TOKEN_COUNT] < 1)
        m_int_configs[CONFIG_PVP_TOKEN_COUNT] = 1;

    m_bool_configs[CONFIG_BG_REWARD_ENABLE]    = sConfigMgr->GetBoolDefault("BattlegroundReward.Enable", 0);
    m_int_configs[CONFIG_BG_ITEM_REWARD_ID]    = sConfigMgr->GetIntDefault("BattlegroundReward.ItemID", 29434);
    m_int_configs[CONFIG_BG_ITEM_REWARD_COUNT] = sConfigMgr->GetIntDefault("BattlegroundReward.ItemCount", 1);
    if (m_int_configs[CONFIG_BG_ITEM_REWARD_COUNT] < 1)
        m_int_configs[CONFIG_BG_ITEM_REWARD_COUNT] = 1;

    SetWinnerMessage(sConfigMgr->GetStringDefault("BattleGroundReward.Message", "You have been awarded for winning the battleground."));

    m_bool_configs[CONFIG_ARENA_REWARD_ENABLE]        = sConfigMgr->GetBoolDefault("ArenaReward.Enable", 0);
    m_int_configs[CONFIG_ARENA_WIN_ITEM_REWARD_ID]    = sConfigMgr->GetIntDefault("ArenaRewardWin.ItemID", 1672);
    m_int_configs[CONFIG_ARENA_LOSS_ITEM_REWARD_ID]   = sConfigMgr->GetIntDefault("ArenaRewardLoss.ItemID", 4868);
    m_int_configs[CONFIG_ARENA_WIN_ITEM_REWARD_COUNT] = sConfigMgr->GetIntDefault("ArenaRewardWin.ItemCount", 1);
    if (m_int_configs[CONFIG_ARENA_WIN_ITEM_REWARD_COUNT] < 1)
        m_int_configs[CONFIG_ARENA_WIN_ITEM_REWARD_COUNT] = 1;
    m_int_configs[CONFIG_ARENA_LOSS_ITEM_REWARD_COUNT] = sConfigMgr->GetIntDefault("ArenaRewardLoss.ItemCount", 1);
    if (m_int_configs[CONFIG_ARENA_LOSS_ITEM_REWARD_COUNT] < 1)
        m_int_configs[CONFIG_ARENA_LOSS_ITEM_REWARD_COUNT] = 1;

    m_bool_configs[CONFIG_NO_RESET_TALENT_COST]       = sConfigMgr->GetBoolDefault("NoResetTalentsCost", false);
    m_bool_configs[CONFIG_SHOW_KICK_IN_WORLD]         = sConfigMgr->GetBoolDefault("ShowKickInWorld", false);
    m_int_configs[CONFIG_INTERVAL_LOG_UPDATE]         = sConfigMgr->GetIntDefault("RecordUpdateTimeDiffInterval", 60000);
    m_int_configs[CONFIG_MIN_LOG_UPDATE]              = sConfigMgr->GetIntDefault("MinRecordUpdateTimeDiff", 100);
    m_int_configs[CONFIG_MAPUPDATER_NUMTHREADS]       = sConfigMgr->GetIntDefault("MapUpdate.Threads", 1);
    m_int_configs[CONFIG_PATHGENERATOR_NUMTHREADS]    = sConfigMgr->GetIntDefault("PathGenerator.Threads", 2);
    m_int_configs[CONFIG_MAX_RESULTS_LOOKUP_COMMANDS] = sConfigMgr->GetIntDefault("Command.LookupMaxResults", 0);

    // Warden
    m_bool_configs[CONFIG_WARDEN_ENABLED]              = sConfigMgr->GetBoolDefault("Warden.Enabled", true);
    m_int_configs[CONFIG_WARDEN_NUM_MEM_CHECKS]        = sConfigMgr->GetIntDefault("Warden.NumMemChecks", 3);
    m_int_configs[CONFIG_WARDEN_NUM_OTHER_CHECKS]      = sConfigMgr->GetIntDefault("Warden.NumOtherChecks", 7);
    m_int_configs[CONFIG_WARDEN_CLIENT_BAN_DURATION]   = sConfigMgr->GetIntDefault("Warden.BanDuration", 86400);
    m_int_configs[CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF]  = sConfigMgr->GetIntDefault("Warden.ClientCheckHoldOff", 30);
    m_int_configs[CONFIG_WARDEN_CLIENT_FAIL_ACTION]    = sConfigMgr->GetIntDefault("Warden.ClientCheckFailAction", 0);
    m_int_configs[CONFIG_WARDEN_CLIENT_RESPONSE_DELAY] = sConfigMgr->GetIntDefault("Warden.ClientResponseDelay", 600);

    // Packet spoof punishment
    m_int_configs[CONFIG_PACKET_SPOOF_POLICY]  = sConfigMgr->GetIntDefault("PacketSpoof.Policy", (uint32)WorldSession::DosProtection::Policy::POLICY_KICK);
    m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] = sConfigMgr->GetIntDefault("PacketSpoof.BanMode", (uint32)BAN_ACCOUNT);
    if (m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] == BAN_CHARACTER || m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] > BAN_IP)
        m_int_configs[CONFIG_PACKET_SPOOF_BANMODE] = BAN_ACCOUNT;
    m_int_configs[CONFIG_PACKET_SPOOF_BANDURATION] = sConfigMgr->GetIntDefault("PacketSpoof.BanDuration", 86400);

    // @SunwellCheat
    m_bool_configs[CONFIG_SUNWELL_CHEAT]              = sConfigMgr->GetBoolDefault("SunwellCheat.Enable", true);
    m_bool_configs[CONFIG_SUNWELL_CHEAT_NOTIFY]       = sConfigMgr->GetBoolDefault("SunwellCheat.Notify", true);
    m_int_configs[CONFIG_SUNWELL_CHEAT_DURATION]      = sConfigMgr->GetIntDefault("SunwellCheat.Duration", 86400);
    m_int_configs[CONFIG_SUNWELL_CHEAT_CLEAN_TIMER]   = sConfigMgr->GetIntDefault("SunwellCheat.CleanReportsTimer", 45);
    m_int_configs[CONFIG_SUNWELL_CHEAT_OPCODE_ACTION] = sConfigMgr->GetIntDefault("SunwellCheat.opCode.Action", 0);
    m_int_configs[CONFIG_SUNWELL_CHEAT_OPCODE_LIMIT]  = sConfigMgr->GetIntDefault("SunwellCheat.opCode.Limit", 100);
    m_int_configs[CONFIG_SUNWELL_CHEAT_CAST_ACTION]   = sConfigMgr->GetIntDefault("SunwellCheat.Cast.Action", 0);
    m_int_configs[CONFIG_SUNWELL_CHEAT_CAST_LIMIT]    = sConfigMgr->GetIntDefault("SunwellCheat.Cast.Limit", 2000);

    // Dungeon finder
    m_int_configs[CONFIG_LFG_OPTIONSMASK] = sConfigMgr->GetIntDefault("DungeonFinder.OptionsMask", 3);

    // DBC_ItemAttributes
    m_bool_configs[CONFIG_DBC_ENFORCE_ITEM_ATTRIBUTES] = sConfigMgr->GetBoolDefault("DBC.EnforceItemAttributes", true);

    // Max instances per hour
    m_int_configs[CONFIG_MAX_INSTANCES_PER_HOUR] = sConfigMgr->GetIntDefault("AccountInstancesPerHour", 5);

    // AutoBroadcast
    m_bool_configs[CONFIG_AUTOBROADCAST]         = sConfigMgr->GetBoolDefault("AutoBroadcast.On", false);
    m_int_configs[CONFIG_AUTOBROADCAST_INTERVAL] = sConfigMgr->GetIntDefault("AutoBroadcast.Timer", 60000);
    if (reload)
    {
        m_timers[WUPDATE_AUTOBROADCAST].SetInterval(m_int_configs[CONFIG_AUTOBROADCAST_INTERVAL]);
        m_timers[WUPDATE_AUTOBROADCAST].Reset();
    }

    // MySQL ping time interval
    m_int_configs[CONFIG_DB_PING_INTERVAL] = sConfigMgr->GetIntDefault("MaxPingTime", 30);

    // Misc
    m_bool_configs[CONFIG_PDUMP_NO_PATHS]     = sConfigMgr->GetBoolDefault("PlayerDump.DisallowPaths", true);
    m_bool_configs[CONFIG_PDUMP_NO_OVERWRITE] = sConfigMgr->GetBoolDefault("PlayerDump.DisallowOverwrite", true);
    m_bool_configs[CONFIG_FREE_DUAL_SPEC]     = sConfigMgr->GetBoolDefault("FreeDualTalentSpecialization", false);
    m_bool_configs[CONFIG_ENABLE_MMAPS]       = sConfigMgr->GetBoolDefault("MoveMaps.Enable", true);
    m_bool_configs[CONFIG_KRUUL_EVENT]        = sConfigMgr->GetBoolDefault("KruulEvent.Enable", true);
    m_int_configs[CONFIG_KRUUL_EVENT_DAY]     = sConfigMgr->GetIntDefault("KruulEvent.Day", DAY_SATURDAY);
    m_int_configs[CONFIG_KRUUL_EVENT_HOUR]    = sConfigMgr->GetIntDefault("KruulEvent.Hour", 14);
    m_int_configs[CONFIG_KRUUL_EVENT_MINUTE]  = sConfigMgr->GetIntDefault("KruulEvent.Minute", 30);

    // Wintergrasp
    m_bool_configs[CONFIG_WINTERGRASP_ENABLE]             = sConfigMgr->GetBoolDefault("Wintergrasp.Enable", true);
    m_int_configs[CONFIG_WINTERGRASP_PLR_MAX]             = sConfigMgr->GetIntDefault("Wintergrasp.PlayerMax", 100);
    m_int_configs[CONFIG_WINTERGRASP_PLR_MIN]             = sConfigMgr->GetIntDefault("Wintergrasp.PlayerMin", 0);
    m_int_configs[CONFIG_WINTERGRASP_PLR_MIN_LVL]         = sConfigMgr->GetIntDefault("Wintergrasp.PlayerMinLvl", 77);
    m_int_configs[CONFIG_WINTERGRASP_BATTLETIME]          = sConfigMgr->GetIntDefault("Wintergrasp.BattleTimer", 30);
    m_int_configs[CONFIG_WINTERGRASP_NOBATTLETIME]        = sConfigMgr->GetIntDefault("Wintergrasp.NoBattleTimer", 150);
    m_int_configs[CONFIG_WINTERGRASP_RESTART_AFTER_CRASH] = sConfigMgr->GetIntDefault("Wintergrasp.CrashRestartTimer", 10);
    m_int_configs[CONFIG_WINTERGRASP_MAX_FACTION_DIFF]    = sConfigMgr->GetIntDefault("Wintergrasp.Max.Faction.Diff", 5);

    // Misc ...
    m_int_configs[CONFIG_BIRTHDAY_TIME] = sConfigMgr->GetIntDefault("BirthdayTime", 1222964635);
    m_int_configs[CONFIG_CURRENT_BUILD] = sConfigMgr->GetIntDefault("CurrentBuild", 12340); // Maczuga

    m_bool_configs[CONFIG_DUEL_FULL_POWER] = sConfigMgr->GetBoolDefault("DuelMod.PowerReset", false);

    m_bool_configs[CONFIG_ANTICHEAT_ENABLE]                      = sConfigMgr->GetBoolDefault("Anticheat.Enable", true);
    m_int_configs[CONFIG_ANTICHEAT_REPORTS_INGAME_NOTIFICATION]  = sConfigMgr->GetIntDefault("Anticheat.ReportsForIngameWarnings", 70);
    m_int_configs[CONFIG_ANTICHEAT_DETECTIONS_ENABLED]           = sConfigMgr->GetIntDefault("Anticheat.DetectionsEnabled", 31);
    m_int_configs[CONFIG_ANTICHEAT_MAX_REPORTS_FOR_DAILY_REPORT] = sConfigMgr->GetIntDefault("Anticheat.MaxReportsForDailyReport", 70);

    m_bool_configs[CONFIG_SAVE_LOOT_SYSTEM]   = sConfigMgr->GetBoolDefault("SaveLootSystem.Enable", true);
    m_bool_configs[CONFIG_EXTRA_CHANCE_EVENT] = sConfigMgr->GetBoolDefault("ExtraChanceEvent.Enable", false);

    m_bool_configs[CONFIG_PREMIUM_TELEPORT_ENABLE]       = sConfigMgr->GetBoolDefault("Premium.Teleport.Enable", true);
    m_bool_configs[CONFIG_PREMIUM_INSTANT_FLYING_ENABLE] = sConfigMgr->GetBoolDefault("Premium.InstantFlying.Enable", true);

    m_bool_configs[CONFIG_PHASED_DUELS_ENABLE] = sConfigMgr->GetBoolDefault("PhasedDuels.Enable", false);
    SetPhasedDuelsZones(sConfigMgr->GetStringDefault("PhasedDuels.Zones", ""));

    m_bool_configs[CONFIG_AUTO_GLOBAL_INVITE_ENABLE] = sConfigMgr->GetBoolDefault("AutoGlobalInvite.Enable", false);
    m_bool_configs[CONFIG_AUTO_GLOBAL_ALWAYS_ENABLE] = sConfigMgr->GetBoolDefault("AutoGlobalInviteAlways.Enable", false);
    m_int_configs[CONFIG_AUTO_GLOBAL_GUID]           = sConfigMgr->GetIntDefault("AutoGlobalInvite.GUID", 1);

    SetCountryName(sConfigMgr->GetStringDefault("Country.Name", "Poland"));

    m_bool_configs[CONFIG_CUSTOM_AFK_REPORT]      = sConfigMgr->GetBoolDefault("Custom.AFK.Report", false);
    m_int_configs[CONFIG_CUSTOM_AFK_REPORT_TIMER] = sConfigMgr->GetIntDefault("Custom.AFK.Report.PvP.Timer", 2);

    // ICC progress stage
    // flags:
    // 1 - 1st wing heroic (marrowgar, deathwhisper, gunship, deathbringer)
    // 2 - 2nd wing heroic (festergut, rotface, putricide)
    // 4 - 3rd wing heroic (council, lana'thel)
    // 8 - 4th wing heroic (valithria, sindragosa)
    // 16 - lich king heroic
    m_int_configs[CONFIG_ICC_STAGE] = sConfigMgr->GetIntDefault("ICC.Heroic.Stage", 1);

    // ICC heroic attempts
    m_int_configs[CONFIG_ICC_ATTEMPTS] = sConfigMgr->GetIntDefault("ICC.Attempts", 5);

    m_bool_configs[CONFIG_ENCOUNTER_LOG] = sConfigMgr->GetBoolDefault("EncounterLog.Enable", false);

    m_bool_configs[CONFIG_EXPERIMENTAL_FEATURE]          = sConfigMgr->GetBoolDefault("Experimental.Enable", false);
    m_bool_configs[CONFIG_CUSTOM_EVENTS_FEATURES_ENABLE] = sConfigMgr->GetBoolDefault("CustomEventFeatures.Enable", false);
    m_bool_configs[CONFIG_HUNGER_GAMES_ENABLE]           = sConfigMgr->GetBoolDefault("HungerGames.Enable", false);
    m_int_configs[CONFIG_HUNGER_GAMES_LIMIT]             = sConfigMgr->GetIntDefault("HungerGames.Limit", 500);
    m_bool_configs[CONFIG_BOOST_NAXXRAMAS]               = sConfigMgr->GetBoolDefault("Naxxramas.Boost", false);

    m_bool_configs[CONFIG_SPECIAL_ANGRATHAR]      = sConfigMgr->GetBoolDefault("AngratharSpecial.Enable", false);

    m_bool_configs[CONFIG_ACCOUNT_HISTORY] = sConfigMgr->GetBoolDefault("AccountHistory.Enable", false);

    /* Increased honor gains per faction config values */
    m_float_configs[CONFIG_BONUS_HONOR_FOR_FACTION_RATE]   = sConfigMgr->GetFloatDefault("FactionHonorBonusRate", 1.0f);
    m_bool_configs[CONFIG_ENABLE_HONOR_BOOST_FOR_HORDE]    = sConfigMgr->GetBoolDefault("HordeHonorBoost.Enable", false);
    m_bool_configs[CONFIG_ENABLE_HONOR_BOOST_FOR_ALLIANCE] = sConfigMgr->GetBoolDefault("AllianceHonorBoost.Enable", false);

    m_int_configs[CONFIG_WINTERGRASP_REMINDER]          = sConfigMgr->GetIntDefault("Wintergrasp.Reminder", 0);
    m_bool_configs[CONFIG_ADDITIONAL_MALYGOS_BOOST]     = sConfigMgr->GetBoolDefault("MalygosAdditionalBoost.Enable", false);
    m_bool_configs[CONFIG_ITEM_RESTORE]                 = sConfigMgr->GetBoolDefault("ItemRestore.Enable", false);
    m_bool_configs[CONFIG_LATENCY_RECORD]               = sConfigMgr->GetBoolDefault("LatencyRecorder.Enable", false);
    m_bool_configs[CONFIG_STATS_LATENCY_ONLY_ON_LOGOUT] = sConfigMgr->GetBoolDefault("LatencyRecorder.onlyLogout.Enable", false);
    m_bool_configs[CONFIG_ULDUAR_PRE_NERF]              = sConfigMgr->GetBoolDefault("UlduarPreNerf.Enable", false);
    m_bool_configs[CONFIG_NINJA_LOOTER_LIST]            = sConfigMgr->GetBoolDefault("NinjaList.Enable", false);
    m_bool_configs[CONFIG_ANTICHEAT_WINTERGRASP]        = sConfigMgr->GetBoolDefault("Anticheat.Wintergrasp.Enable", false);
    m_bool_configs[CONFIG_GM_ANTIABUSE_MONEY]           = sConfigMgr->GetBoolDefault("GM.AntiAbuse.Money", true);

    /* Follow movegen params */
    m_int_configs[CONFIG_MOVEMENT_FOLLOWUPDATE_INTERVAL] = sConfigMgr->GetIntDefault("Movement.FollowUpdateInterval", 300);
    m_int_configs[CONFIG_MOVEMENT_FOLLOWSTART_TIMER]     = sConfigMgr->GetIntDefault("Movement.FollowStartTimer", 100);
    m_float_configs[CONFIG_MOVEMENT_FOLLOWPATH_LENGTH]   = sConfigMgr->GetFloatDefault("Movement.FollowMaxPathLength", 60.0f);

    m_bool_configs[CONFIG_EXTERNAL_MAIL]         = sConfigMgr->GetBoolDefault("Mail.External", false);
    m_int_configs[CONFIG_EXTERNAL_MAIL_INTERVAL] = sConfigMgr->GetIntDefault("Mail.ExternalInterval", 1);
    m_timers[WUPDATE_EXTERNAL_MAILS].SetInterval(m_int_configs[CONFIG_EXTERNAL_MAIL_INTERVAL]);

    Movement::FollowMovementGenerator::FOLLOW_UPDATE_TIMER    = m_int_configs[CONFIG_MOVEMENT_FOLLOWUPDATE_INTERVAL];
    Movement::FollowMovementGenerator::FOLLOW_START_TIMER     = m_int_configs[CONFIG_MOVEMENT_FOLLOWSTART_TIMER];
    Movement::FollowMovementGenerator::FOLLOW_MAX_PATH_LENGTH = m_float_configs[CONFIG_MOVEMENT_FOLLOWPATH_LENGTH];
    m_bool_configs[CONFIG_DUEL_DEBUFF_RESET]                  = sConfigMgr->GetBoolDefault("DuelResetDebuffs", false);

    m_bool_configs[CONFIG_ANTIWINTRADING_ENABLE]    = sConfigMgr->GetBoolDefault("AntiWintrading.Enable", true);
    m_int_configs[CONFIG_ANTIWINTRADING_MINDMG]     = sConfigMgr->GetIntDefault("AntiWintrading.MinDMG", 3000);
    m_int_configs[CONFIG_ANTIWINTRADING_MINHEALING] = sConfigMgr->GetIntDefault("AntiWintrading.MinHealing", 3000);

    m_bool_configs[CONFIG_LOG_CREATURE_CANT_REACH]          = sConfigMgr->GetBoolDefault("CreatureCantReachLog.Enable", true);
    m_int_configs[CONFIG_LOG_CREATURE_CANT_REACH_THRESHOLD] = sConfigMgr->GetIntDefault("CreatureCantReachLogThreshold.Amount", 5);

    // Chain Pull
    m_bool_configs[CONFIG_CHAIN_PULL_ENABLED] = sConfigMgr->GetBoolDefault("CreatureChainPull.Enable", true);
    m_int_configs[CONFIG_CHAIN_PULL_TIMER]    = sConfigMgr->GetIntDefault("CreatureChainPullTimer.Amount", 4000);
    m_float_configs[CONFIG_CHAIN_PULL_RANGE]  = sConfigMgr->GetFloatDefault("CreatureChainPullRange.Amount", 8.0f);

    // Anti hk farm system
    m_bool_configs[CONFIG_ANTI_HK_FARM_ENABLE] = sConfigMgr->GetBoolDefault("AntiHkFarm.Enable", false);
    m_int_configs[CONFIG_ANTI_HK_FARM_COUNT]   = sConfigMgr->GetIntDefault("AntiHkFarm.Count", 10);
    m_int_configs[CONFIG_ANTI_HK_FARM_EXPIRE]  = sConfigMgr->GetIntDefault("AntiHkFarm.Expire", 600000);

    m_bool_configs[CONFIG_ENABLE_CHAT_PROCESSOR]         = sConfigMgr->GetBoolDefault("ChatProcessor.Enable", true);
    m_int_configs[CONFIG_CHAT_PROCESSOR_THREADS]         = sConfigMgr->GetIntDefault("ChatProcessor.Threads", 2);
    m_int_configs[CONFIG_CHAT_PROCESSOR_REPORT_COOLDOWN] = sConfigMgr->GetIntDefault("ChatProcessor.ReportCooldown", 5000);
    m_bool_configs[CONFIG_CHAT_PROCESSOR_ASYNC]          = sConfigMgr->GetBoolDefault("ChatProcessor.Async", true);

    if (m_bool_configs[CONFIG_ENABLE_CHAT_PROCESSOR])
        GetChatProcessor().Initialize(m_int_configs[CONFIG_CHAT_PROCESSOR_THREADS]);
    else
        GetChatProcessor().Shutdown();

    /* World Relay */
    m_bool_configs[CONFIG_ENABLE_WEBHOOK_RELAY]  = sConfigMgr->GetBoolDefault("WebhookRelay.Enable", false);
    m_int_configs[CONFIG_WORLD_RELAY_NUMTHREADS] = sConfigMgr->GetIntDefault("WebhookRelay.NumThreads", 1);

    // Warden
    m_int_configs[CONFIG_ENABLE_WARDEN_LUA]               = sConfigMgr->GetIntDefault("Warden.Lua.Enabled", 0);
    m_int_configs[CONFIG_WARDEN_LUA_MINIMUM_PLAYER_LEVEL] = sConfigMgr->GetIntDefault("Warden.Lua.MinimumPlayerLevel", 15);
    m_int_configs[CONFIG_WARDEN_LUA_ENABLE_CHARACTER_SCREEN_MSG] = sConfigMgr->GetIntDefault("Warden.Lua.CharacterScreenMessage", 0);
    uint32 _wardenCollectInterval                         = sConfigMgr->GetIntDefault("Warden.Lua.TransactionInterval", 10000);
    m_timers[WUPDATE_WARDEN_REPORTS].SetInterval(_wardenCollectInterval);
    m_timers[WUPDATE_WARDEN_REPORTS].Reset();

    m_bool_configs[CONFIG_DUNGEON_FINDER_NEW_BRACKET_SYSTEM] = sConfigMgr->GetBoolDefault("DungeonFinder.NewBracketSystem.Enable", false);

    // Xp Rates
    m_bool_configs[CONFIG_XP_RATE_EVENT]            = sConfigMgr->GetBoolDefault("Rate.XP.Event.Enable", false);
    m_bool_configs[CONFIG_BG_DF_XP_BOOST_ENABLE]    = sConfigMgr->GetBoolDefault("BgDf.Xp.Boost.Enable", 0);
    m_float_configs[CONFIG_BG_DF_XP_BOOST_VALUE]    = sConfigMgr->GetFloatDefault("BgDf.Xp.Boost.Value", 0.25f);

    m_bool_configs[CONFIG_CHECK_GAMEOBJECT_LEVEL_REQ]   = sConfigMgr->GetBoolDefault("GameObject.CheckLevelRequirements.Enable", false);

    m_bool_configs[CONFIG_WINTERGRASP_BALANCE_SYSTEM]     = sConfigMgr->GetBoolDefault("Wintergrasp.BalanceSystem.Enable", true);
    m_bool_configs[CONFIG_RAID_INVITE_RESTRICTION_ENABLE] = sConfigMgr->GetBoolDefault("Raid.InviteRestriction.Enable", true);
    m_int_configs[CONFIG_RAID_INVITE_RESTRICTION_LEVEL]   = sConfigMgr->GetIntDefault("Raid.InviteRestriction.Level", 10);

    m_bool_configs[CONFIG_WINTERGRASP_AUTOSWITCH_ENABLE] = sConfigMgr->GetBoolDefault("Wintergrasp.Autoswitch.Enable", false);
    m_int_configs[CONFIG_WINTERGRASP_AUTOSWITCH_COUNT]   = sConfigMgr->GetIntDefault("Wintergrasp.Autoswitch.Count", 4500);

    m_int_configs[CONFIG_REPUTATION_BOOST_PERCENT]      = sConfigMgr->GetIntDefault("Reputation.Boost.Percent", 0);
    m_int_configs[CONFIG_REPUTATION_BOOST_FACTION_MASK] = sConfigMgr->GetIntDefault("Reputation.Boost.FactionMask", 0);

    m_int_configs[CONFIG_WINTERGRASP_ABSOLUTE_LIMIT]           = sConfigMgr->GetIntDefault("Wintergrasp.AbsoluteLimit", 180);
    m_bool_configs[CONFIG_NAXXRAMAS_MEETING_STONE_SANCTUARY]   = sConfigMgr->GetBoolDefault("NaxxramasSanctuary", 0);
    m_bool_configs[CONFIG_BATTLEGROUNDS_SEPARATE_TWINK_QUEUES] = sConfigMgr->GetBoolDefault("Battlegrounds.Separate.Twink.Queues", false);
    m_int_configs[CONFIG_CLIENT_ACTION_PUNISH_POLICY]          = sConfigMgr->GetIntDefault("ClientActionOffense.Policy", 4);
    m_int_configs[CONFIG_CLIENT_ACTION_INVITE_MAX_AMOUNT]      = sConfigMgr->GetIntDefault("ClientAction.Invites.MaximumAmount", 150);
    //! time is in minutes
    m_int_configs[CONFIG_CLIENT_ACTION_INVITE_RESET_TIME]              = sConfigMgr->GetIntDefault("ClientAction.Invites.ResetTime", 5);
    m_int_configs[CONFIG_CLIENT_ACTION_CHEST_CHEATER_RESET_TIME]       = sConfigMgr->GetIntDefault("ClientAction.ChestCheater.ResetTime", 5);
    m_int_configs[CONFIG_CLIENT_ACTION_CHEST_CHEATER_MAX_AMOUNT]       = sConfigMgr->GetIntDefault("ClientAction.ChestCheater.MaximumAmount", 5);
    m_int_configs[CONFIG_CLIENT_ACTION_SYSTEM]                         = sConfigMgr->GetIntDefault("ClientAction.Enable", 0);
    m_int_configs[CONFIG_LOG_NEW_ACCOUNTS]                             = sConfigMgr->GetIntDefault("AccountLogging.LogNewAccounts", 1);
    m_int_configs[CONFIG_OLD_ACCOUNT_TIMESTAMP]                        = sConfigMgr->GetIntDefault("AccountLogging.OldAccountTimestamp", 1581675120);
    m_int_configs[CONFIG_LOG_NEW_ACCOUNTS_HIGHEST_CHAR_LEVEL]          = sConfigMgr->GetIntDefault("AccountLogging.HighestCharacterLevel", 20);
    m_int_configs[CONFIG_ARENA_WORLD_ANNOUNCER_COOLDOWN]               = sConfigMgr->GetIntDefault("Arena.World.Announcer.Cooldown", 30);
    m_bool_configs[CONFIG_BOOL_PREMIUM_SKIPQUEUE]                      = sConfigMgr->GetBoolDefault("Premium.SkipQueue", false);
    m_bool_configs[CONFIG_CREATURE_FULL_HEALTH_AT_EVADE]               = sConfigMgr->GetBoolDefault("Creature.Misc.FullHealthAtEvadeForBosses", false);
    m_bool_configs[CONFIG_UPDATE_INSTANCE_BATTLEGROUNDS_EVERY_UPDATE]  = sConfigMgr->GetBoolDefault("MapUpdates.UpdateInstancesEachCycle", false);
    m_int_configs[CONFIG_UPDATE_SMARTCAST_COMBAT_MOVE_SPELLS_INTERVAL] = sConfigMgr->GetIntDefault("SmartAI.SmartCastCombatMoveInterval", 1000);
    m_bool_configs[CONFIG_STRICT_IPLOCK]                               = sConfigMgr->GetBoolDefault("StrictIPLock", false);
    m_bool_configs[CONFIG_LOOT_CHECK_GOBJECT_LOCK]                     = sConfigMgr->GetBoolDefault("Loot.CheckGobjectLock", false);
    m_int_configs[CONFIG_AUTH_HANDLER_NUM_THREADS]                     = sConfigMgr->GetIntDefault("Network.ThreadedAuth.NumThreads", 6);
    m_int_configs[CONFIG_BAN_CHECK_TIME_DIFFERENCE]                    = sConfigMgr->GetIntDefault("StrictIPLock.TimeDifference", 10);
    m_bool_configs[CONFIG_CHAT_CHECK_MESSAGE_SIZE]                     = sConfigMgr->GetBoolDefault("Chat.CheckMessageSize", 0);
    //! reports to gamemasters if player failed messages size check
    m_bool_configs[CONFIG_CHAT_REPORT_FAILURES_TO_GM] = sConfigMgr->GetBoolDefault("Chat.Report.MessageSize", 0);

    m_bool_configs[CONFIG_ENABLE_TOGC25_BOOST]             = sConfigMgr->GetBoolDefault("ToGC25.Boost.Enabled", 0);
    m_bool_configs[CONFIG_ENABLE_ICC_BOOST]                = sConfigMgr->GetBoolDefault("ICC.Boost.Enabled", 0);
    m_bool_configs[CONFIG_LOTTERY_ENABLE]                      = sConfigMgr->GetBoolDefault("Lottery.Enable", false);
    m_int_configs[CONFIG_LOTTERY_MIN_LVL]                      = sConfigMgr->GetIntDefault("Lottery.MinLvl", 25);

    m_int_configs[CONFIG_MYTHIC_DIMINISHING_RETURNS_KEY_LEVEL] = sConfigMgr->GetIntDefault("Mythic.DiminishingReturnsKeyLevel", 3);

    m_bool_configs[CONFIG_FREE_SPELLS]            = sConfigMgr->GetBoolDefault("FreeSpells.Enable", false);
    m_bool_configs[CONFIG_CHARACTER_BOOST_ENABLE] = sConfigMgr->GetBoolDefault("CharacterBoost.Enable", false);

    m_int_configs[CONFIG_SHADOWMOURNE_DISABLE_MASK] = sConfigMgr->GetIntDefault("Shadowmourne.Disable.Mask", 0);

    m_bool_configs[CONFIG_ARENA_NO_MATCHING_IP_ADDRESSES] = sConfigMgr->GetBoolDefault("Arena.Settings.NoMatchingIpAddresses", false);
    m_bool_configs[CONFIG_ARENA_NO_REPEATING_TEAMS]       = sConfigMgr->GetBoolDefault("Arena.Settings.NoRepeatingTeams", false);
    m_int_configs[CONFIG_ARENA_NEW_TEAM_JOIN_DELAY]       = sConfigMgr->GetIntDefault("Arena.Settings.TeamJoinDelay", 0);
    std::string strArenaHours                             = sConfigMgr->GetStringDefault("Arena.Settings.AvailabilityHours", "0;0");

    Tokenizer tokens(strArenaHours, ';', 2U);
    if (tokens.size() < 2)
    {
        m_arenaAvailabilityHours.AvailableSince = 0;
        m_arenaAvailabilityHours.AvailableTo    = 0;
    }
    else
    {
        try
        {
            m_arenaAvailabilityHours.AvailableSince = std::stoi(tokens[0]);
            m_arenaAvailabilityHours.AvailableTo = std::stoi(tokens[1]);
        }
        catch (...)
        {
        }
    }

    m_int_configs[CONFIG_PRELOAD_GRID_ENABLED_MASK] = sConfigMgr->GetIntDefault("PreloadGrid.EnabledMask", 0);

    // SoloQueue
    m_bool_configs[CONFIG_SOLO_QUEUE_ENABLE]                     = sConfigMgr->GetBoolDefault("SoloQueue.Enable",                       1);
    m_bool_configs[CONFIG_SOLO_QUEUE_ENABLE_MMR_MATCHING]        = sConfigMgr->GetBoolDefault("SoloQueue.EnableMMRMatching",            1);
    m_int_configs[CONFIG_SOLOQUEUE_MMR_TOLERANCE_INITIAL]        = sConfigMgr->GetIntDefault("SoloQueue.MMR.Tolerance.Initial",       100);
    m_int_configs[CONFIG_SOLOQUEUE_MMR_TOLERANCE_INTERVAL_SECS]  = sConfigMgr->GetIntDefault("SoloQueue.MMR.Tolerance.Interval.Secs",  30);
    m_int_configs[CONFIG_SOLOQUEUE_MMR_TOLERANCE_INTERVAL_VALUE] = sConfigMgr->GetIntDefault("SoloQueue.MMR.Tolerance.Interval.Value", 25);
    m_int_configs[CONFIG_SOLOQUEUE_REQUIRED_ITEM_LEVEL]          = sConfigMgr->GetIntDefault("SoloQueue.Required.Item.Level",         353);
    m_int_configs[CONFIG_SOLOQUEUE_ARENA_RATING_DISCARD_TIMER]   = sConfigMgr->GetIntDefault ("SoloQueue.Arena.RatingDiscardTimer", 25 * MINUTE * IN_MILLISECONDS);
    m_bool_configs[CONFIG_SOLO_QUEUE_ENABLE_CLASS_STACKING]      = sConfigMgr->GetBoolDefault("SoloQueue.EnableClassStacking",          1);
    m_bool_configs[CONFIG_SOLO_QUEUE_GEMS_SOCKET_REQ]            = sConfigMgr->GetBoolDefault("SoloQueue.GemsSocketReq",                0);
    m_int_configs[CONFIG_SOLOQUEUE_RESILIENCE_REQ]               = sConfigMgr->GetIntDefault ("SoloQueue.ResilienceReq",                0);
    m_bool_configs[CONFIG_SOLO_QUEUE_PERSONAL_RATING]            = sConfigMgr->GetBoolDefault("SoloQueue.PersonalRating",               1);
    m_int_configs[CONFIG_SOLO_QUEUE_MAX_MMR_DIFF]                = sConfigMgr->GetIntDefault("SoloQueue.MaxMMRDiff",                  300);
    m_bool_configs[CONFIG_SOLO_QUEUE_LAST_DPS_CLASS_MASK]        = sConfigMgr->GetBoolDefault("SoloQueue.LastDPSClassMask",             0);
    m_bool_configs[CONFIG_SOLO_QUEUE_ALTERNATIVE_DPS_SPECS]      = sConfigMgr->GetBoolDefault("SoloQueue.AlternativeDPSSpecs",          0);
    m_bool_configs[CONFIG_SOLO_QUEUE_OPTIMIZED_QUEUES]           = sConfigMgr->GetBoolDefault("SoloQueue.OptimizedQueues",              1);
    m_int_configs[CONFIG_SOLO_QUEUE_MAX_RATING_DIFFERENCE_1500]  = sConfigMgr->GetIntDefault ("SoloQueue.MaxRatingDifference1500",    200);
    m_int_configs[CONFIG_SOLO_QUEUE_MAX_RATING_DIFFERENCE_2000]  = sConfigMgr->GetIntDefault ("SoloQueue.MaxRatingDifference2000",    300);
    m_int_configs[CONFIG_SOLO_QUEUE_MAX_RATING_DIFFERENCE_3000]  = sConfigMgr->GetIntDefault ("SoloQueue.MaxRatingDifference3000",    500);
    
    // Call To Arms
    m_bool_configs[CONFIG_CALL_TO_ARMS_ENABLE]                  = sConfigMgr->GetBoolDefault("CallToArms.Enable", true);
    m_int_configs[CONFIG_CALL_TO_ARMS_MESSAGE_COOLDOWN]         = sConfigMgr->GetIntDefault("CallToArms.MessageCooldown", 120);

    sCustomMgr->LoadConfigSettings(reload);

    sLog->outString("%s", GetCrossfactionConfigInfo().c_str());

    // call ScriptMgr if we're reloading the configuration
    if (reload)
        sScriptMgr->OnConfigLoad(reload);
}

void World::SendText(std::string_view const text) const
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(text.data());
    char* pos = buf;

    while (char const* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, nullptr, nullptr, line);
        for (auto&& session : m_sessions)
            if (session.second && session.second->GetPlayer() && session.second->GetPlayer()->IsInWorld())
                session.second->SendPacket(&data);
    }

    free(buf);
}

void World::SendText(const char* text, std::function<bool(Player*)> const& cond)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(text);
    char* pos = buf;

    while (char* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, nullptr, nullptr, line);
        for (auto&& session : m_sessions)
            if (session.second && session.second->GetPlayer() && session.second->GetPlayer()->IsInWorld() && cond(session.second->GetPlayer()))
                session.second->SendPacket(&data);
    }

    free(buf);
}

extern void LoadGameObjectModelList(std::string const&);

/// Initialize the World
void World::SetInitialWorldSettings()
{
    ///- Server startup begin
    uint32 startupBegin = getMSTime();

    ///- Initialize the random number generator
    srand((unsigned int)time(nullptr));

    ///- Initialize detour memory management
    dtAllocSetCustom(dtCustomAlloc, dtCustomFree);

    ///- Initialize config settings
    LoadConfigSettings();

    ///- Initialize Allowed Security Level
    LoadDBAllowedSecurityLevel();

    ///- Init highest guids before any table loading to prevent using not initialized guids in some code.
    sObjectMgr->SetHighestGuids();

    ///- Check the existence of the map files for all races' startup areas.
    if (!MapManager::ExistMapAndVMap(0, -6240.32f, 331.033f)
        || !MapManager::ExistMapAndVMap(0, -8949.95f, -132.493f)
        || !MapManager::ExistMapAndVMap(1, -618.518f, -4251.67f)
        || !MapManager::ExistMapAndVMap(0, 1676.35f, 1677.45f)
        || !MapManager::ExistMapAndVMap(1, 10311.3f, 832.463f)
        || !MapManager::ExistMapAndVMap(1, -2917.58f, -257.98f)
        || (m_int_configs[CONFIG_EXPANSION] && (!MapManager::ExistMapAndVMap(530, 10349.6f, -6357.29f) || !MapManager::ExistMapAndVMap(530, -3961.64f, -13931.2f))))
    {
        exit(1);
    }

    ///- Initialize pool manager
    sPoolMgr->Initialize();

    ///- Initialize game event manager
    sGameEventMgr->Initialize();

    ///- Loading strings. Getting no records means core load has to be canceled because no error message can be output.
    sLog->outString();
    sLog->outString("Loading Trinity strings...");
    if (!sObjectMgr->LoadTrinityStrings())
        exit(1); // Error message displayed in function already

    ///- Update the realm entry in the database with the realm type from the config file
    //No SQL injection as values are treated as integers

    // not send custom type REALM_FFA_PVP to realm list
    uint32 server_type;
    if (IsFFAPvPRealm())
        server_type = REALM_TYPE_PVP;
    else
        server_type = getIntConfig(CONFIG_GAME_TYPE);

    uint32 realm_zone = getIntConfig(CONFIG_REALM_ZONE);

    LoginDatabase.PExecute("UPDATE realmlist SET icon = %u, timezone = %u WHERE id = '%d'", server_type, realm_zone, realmID); // One-time query

    auto realmNameStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALM_NAME);
    realmNameStmt->setUInt32(0, realmID);
    PreparedQueryResult realmNameResult = LoginDatabase.Query(realmNameStmt);
    if (!realmNameResult)
        ASSERT(false);
    else
    {
        Field* fields = realmNameResult->Fetch();
        SetRealmName(fields[0].GetString());
    }

    ///- Remove the bones (they should not exist in DB though) and old corpses after a restart
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_OLD_CORPSES);
    stmt->setUInt32(0, 3 * DAY);
    CharacterDatabase.Execute(stmt);

    ///- Load the DBC files
    sLog->outString("Initialize data stores...");
    LoadDBCStores(m_dataPath);
    DetectDBCLang();

    sLog->outString("Loading spell dbc data corrections...");
    sSpellMgr->LoadDbcDataCorrections();

    sLog->outString("Loading SpellInfo store...");
    sSpellMgr->LoadSpellInfoStore();

    sLog->outString("Loading Spell Rank Data...");
    sSpellMgr->LoadSpellRanks();

    sLog->outString("Loading Spell Specific And Aura State...");
    sSpellMgr->LoadSpellSpecificAndAuraState();

    sLog->outString("Loading SkillLineAbilityMultiMap Data...");
    sSpellMgr->LoadSkillLineAbilityMap();

    sLog->outString("Loading spell custom attributes...");
    sSpellMgr->LoadSpellCustomAttr();

    sLog->outString("Loading GameObject models...");
    LoadGameObjectModelList(GetDataPath());

    sLog->outString("Loading Script Names...");
    sObjectMgr->LoadScriptNames();

    sLog->outString("Loading Instance Template...");
    sObjectMgr->LoadInstanceTemplate();

    // xinef: Global Storage, should be loaded asap
    //! Riztazz: very slow currently due to loading of accountNames during global player data load
    sLog->outString("Load Global Player Data...(This will take a while to load!)");
    sWorld->LoadGlobalPlayerDataStore();

    sLog->outString("Loading account and ip history...");
    sWorld->LoadAccountAndIpHistory();

    // Must be called before `creature_respawn`/`gameobject_respawn` tables
    sLog->outString("Loading instances...");
    sInstanceSaveMgr->LoadInstances();

    sLog->outString("Loading Localization strings...");
    uint32 oldMSTime = getMSTime();
    sObjectMgr->LoadCreatureLocales();
    sObjectMgr->LoadGameObjectLocales();
    sObjectMgr->LoadItemLocales();
    sObjectMgr->LoadItemSetNameLocales();
    sObjectMgr->LoadQuestLocales();
    sObjectMgr->LoadNpcTextLocales();
    sObjectMgr->LoadPageTextLocales();
    sObjectMgr->LoadGossipMenuItemsLocales();
    sObjectMgr->LoadPointOfInterestLocales();

    sObjectMgr->SetDBCLocaleIndex(GetDefaultDbcLocale()); // Get once for all the locale index of DBC language (console/broadcasts)
    sLog->outString(">> Localization strings loaded in %u ms", GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();

    sLog->outString("Loading Page Texts...");
    sObjectMgr->LoadPageTexts();

    sLog->outString("Loading Game Object Templates..."); // must be after LoadPageTexts
    sObjectMgr->LoadGameObjectTemplate();

    sLog->outString("Loading Transport templates...");
    sTransportMgr->LoadTransportTemplates();

    sLog->outString("Loading Spell Required Data...");
    sSpellMgr->LoadSpellRequired();

    sLog->outString("Loading Spell Group types...");
    sSpellMgr->LoadSpellGroups();

    sLog->outString("Loading Spell Learn Skills...");
    sSpellMgr->LoadSpellLearnSkills(); // must be after LoadSpellRanks

    sLog->outString("Loading Spell Proc Event conditions...");
    sSpellMgr->LoadSpellProcEvents();

    sLog->outString("Loading Spell Proc conditions and data...");
    sSpellMgr->LoadSpellProcs();

    sLog->outString("Loading Spell Bonus Data...");
    sSpellMgr->LoadSpellBonusess();

    sLog->outString("Loading Aggro Spells Definitions...");
    sSpellMgr->LoadSpellThreats();

    sLog->outString("Loading Mixology bonuses...");
    sSpellMgr->LoadSpellMixology();

    sLog->outString("Loading Spell Group Stack Rules...");
    sSpellMgr->LoadSpellGroupStackRules();

    sLog->outString("Loading NPC Texts...");
    sObjectMgr->LoadGossipText();

    sLog->outString("Loading Enchant Spells Proc datas...");
    sSpellMgr->LoadSpellEnchantProcData();

    sLog->outString("Loading Item Random Enchantments Table...");
    LoadRandomEnchantmentsTable();

    sLog->outString("Loading Disables");
    DisableMgr::LoadDisables(); // must be before loading quests and items

    sLog->outString("Loading Items..."); // must be after LoadRandomEnchantmentsTable and LoadPageTexts
    sObjectMgr->LoadItemTemplates();

    sLog->outString("Loading Item set names..."); // must be after LoadItemPrototypes
    sObjectMgr->LoadItemSetNames();

    sLog->outString("Loading Creature Model Based Info Data...");
    sObjectMgr->LoadCreatureModelInfo();

    sLog->outString("Loading Creature template outfits..."); // must be before LoadCreatureTemplates
    sObjectMgr->LoadCreatureOutfits();

    sMythicMgr.Initialize();

    sLog->outString("Loading Creature templates...");
    sObjectMgr->LoadCreatureTemplates();

    sLog->outString("Loading Equipment templates..."); // must be after LoadCreatureTemplates
    sObjectMgr->LoadEquipmentTemplates();

    sLog->outString("Loading Creature template addons...");
    sObjectMgr->LoadCreatureTemplateAddons();

    sLog->outString("Loading Reputation Reward Rates...");
    sObjectMgr->LoadReputationRewardRate();

    sLog->outString("Loading Creature Reputation OnKill Data...");
    sObjectMgr->LoadReputationOnKill();

    sLog->outString("Loading Reputation Spillover Data...");
    sObjectMgr->LoadReputationSpilloverTemplate();

    sLog->outString("Loading Points Of Interest Data...");
    sObjectMgr->LoadPointsOfInterest();

    sLog->outString("Loading Creature Base Stats...");
    sObjectMgr->LoadCreatureClassLevelStats();

    sLog->outString("Loading Creature Data...");
    sObjectMgr->LoadCreatures();

    sLog->outString("Loading Temporary Summon Data...");
    sObjectMgr->LoadTempSummons(); // must be after LoadCreatureTemplates() and LoadGameObjectTemplates()

    sLog->outString("Loading pet levelup spells...");
    sSpellMgr->LoadPetLevelupSpellMap();

    sLog->outString("Loading pet default spells additional to levelup spells...");
    sSpellMgr->LoadPetDefaultSpells();

    sLog->outString("Loading Creature Addon Data...");
    sObjectMgr->LoadCreatureAddons(); // must be after LoadCreatureTemplates() and LoadCreatures()

    sLog->outString("Loading Gameobject Data...");
    sObjectMgr->LoadGameobjects();

    sLog->outString("Loading GameObject Addon Data...");
    sObjectMgr->LoadGameObjectAddons(); // must be after LoadGameObjectTemplate() and LoadGameobjects()

    sLog->outString("Loading Creature Linked Respawn...");
    sObjectMgr->LoadLinkedRespawn(); // must be after LoadCreatures(), LoadGameObjects()

    sLog->outString("Loading Weather Data...");
    WeatherMgr::LoadWeatherData();

    sLog->outString("Loading Quests...");
    sObjectMgr->LoadQuests(); // must be loaded after DBCs, creature_template, item_template, gameobject tables

    sLog->outString("Checking Quest Disables");
    DisableMgr::CheckQuestDisables(); // must be after loading quests

    sLog->outString("Loading Quest POI");
    sObjectMgr->LoadQuestPOI();

    sLog->outString("Loading Quests Starters and Enders...");
    sObjectMgr->LoadQuestStartersAndEnders(); // must be after quest load

    sLog->outString("Loading Objects Pooling Data...");
    sPoolMgr->LoadFromDB();

    sLog->outString("Loading Game Event Data..."); // must be after loading pools fully
    sGameEventMgr->LoadHolidayDates();             // Must be after loading DBC
    sGameEventMgr->LoadFromDB();                   // Must be after loading holiday dates

    sLog->outString("Loading UNIT_NPC_FLAG_SPELLCLICK Data..."); // must be after LoadQuests
    sObjectMgr->LoadNPCSpellClickSpells();

    sLog->outString("Loading Vehicle Template Accessories...");
    sObjectMgr->LoadVehicleTemplateAccessories(); // must be after LoadCreatureTemplates() and LoadNPCSpellClickSpells()

    sLog->outString("Loading Vehicle Accessories...");
    sObjectMgr->LoadVehicleAccessories(); // must be after LoadCreatureTemplates() and LoadNPCSpellClickSpells()

    sLog->outString("Loading SpellArea Data..."); // must be after quest load
    sSpellMgr->LoadSpellAreas();

    sLog->outString("Loading AreaTrigger definitions...");
    sObjectMgr->LoadAreaTriggerTeleports();

    sLog->outString("Loading Access Requirements...");
    sObjectMgr->LoadAccessRequirements(); // must be after item template load

    sLog->outString("Loading Quest Area Triggers...");
    sObjectMgr->LoadQuestAreaTriggers(); // must be after LoadQuests

    sLog->outString("Loading Tavern Area Triggers...");
    sObjectMgr->LoadTavernAreaTriggers();

    sLog->outString("Loading AreaTrigger script names...");
    sObjectMgr->LoadAreaTriggerScripts();

    sLog->outString("Loading LFG entrance positions..."); // Must be after areatriggers
    sLFGMgr->LoadLFGDungeons();

    sLog->outString("Loading Dungeon boss data...");
    sObjectMgr->LoadInstanceEncounters();

    sLog->outString("Loading LFG rewards...");
    sLFGMgr->LoadRewards();

    sLog->outString("Loading Graveyard-zone links...");
    sObjectMgr->LoadGraveyardZones();

    sLog->outString("Loading spell pet auras...");
    sSpellMgr->LoadSpellPetAuras();

    sLog->outString("Loading Spell target coordinates...");
    sSpellMgr->LoadSpellTargetPositions();

    sLog->outString("Loading enchant custom attributes...");
    sSpellMgr->LoadEnchantCustomAttr();

    sLog->outString("Loading linked spells...");
    sSpellMgr->LoadSpellLinked();

    sLog->outString("Loading Player Create Data...");
    sObjectMgr->LoadPlayerInfo();

    sLog->outString("Loading Exploration BaseXP Data...");
    sObjectMgr->LoadExplorationBaseXP();

    sLog->outString("Loading Pet Name Parts...");
    sObjectMgr->LoadPetNames();

    CharacterDatabaseCleaner::CleanDatabase();

    sLog->outString("Loading the max pet number...");
    sObjectMgr->LoadPetNumber();

    sLog->outString("Loading pet level stats...");
    sObjectMgr->LoadPetLevelInfo();

    sLog->outString("Loading Player Corpses...");
    sObjectMgr->LoadCorpses();

    sLog->outString("Loading Player level dependent mail rewards...");
    sObjectMgr->LoadMailLevelRewards();

    // Loot tables
    LoadLootTables();

    sLog->outString("Loading Skill Discovery Table...");
    LoadSkillDiscoveryTable();

    sLog->outString("Loading Skill Extra Item Table...");
    LoadSkillExtraItemTable();

    sLog->outString("Loading Skill Fishing base level requirements...");
    sObjectMgr->LoadFishingBaseSkillLevel();

    sLog->outString("Loading Achievements...");
    sAchievementMgr->LoadAchievementReferenceList();
    sLog->outString("Loading Achievement Criteria Lists...");
    sAchievementMgr->LoadAchievementCriteriaList();
    sLog->outString("Loading Achievement Criteria Data...");
    sAchievementMgr->LoadAchievementCriteriaData();
    sLog->outString("Loading Achievement Rewards...");
    sAchievementMgr->LoadRewards();
    sLog->outString("Loading Achievement Reward Locales...");
    sAchievementMgr->LoadRewardLocales();
    sLog->outString("Loading Completed Achievements...");
    sAchievementMgr->LoadCompletedAchievements();

    ///- Load dynamic data tables from the database
    sLog->outString("Loading Item Auctions...");
    sAuctionMgr->LoadAuctionItems();
    sLog->outString("Loading Auctions...");
    sAuctionMgr->LoadAuctions();

    sGuildMgr->LoadGuilds();

    sLog->outString("Loading ArenaTeams...");
    sArenaTeamMgr->LoadArenaTeams();

    sLog->outString("Loading Groups...");
    sGroupMgr->LoadGroups();

    sLog->outString("Loading ReservedNames...");
    sObjectMgr->LoadReservedPlayersNames();

    sLog->outString("Loading GameObjects for quests...");
    sObjectMgr->LoadGameObjectForQuests();

    sLog->outString("Loading BattleMasters...");
    sBattlegroundMgr->LoadBattleMastersEntry();

    sLog->outString("Loading GameTeleports...");
    sObjectMgr->LoadGameTele();

    sLog->outString("Loading Gossip menu...");
    sObjectMgr->LoadGossipMenu();

    sLog->outString("Loading Gossip menu options...");
    sObjectMgr->LoadGossipMenuItems();

    sLog->outString("Loading Vendors...");
    sObjectMgr->LoadVendors(); // must be after load CreatureTemplate and ItemTemplate

    sLog->outString("Loading Trainers...");
    sObjectMgr->LoadTrainerSpell(); // must be after load CreatureTemplate

    sLog->outString("Loading Waypoints...");
    sWaypointMgr->Load();

    sLog->outString("Loading SmartAI Waypoints...");
    sSmartWaypointMgr->LoadFromDB();

    sLog->outString("Loading Creature Formations...");
    sFormationMgr->LoadCreatureFormations();

    sLog->outString("Loading World States..."); // must be loaded before battleground, outdoor PvP and conditions
    LoadWorldStates();

    sLog->outString("Loading Conditions...");
    sConditionMgr->LoadConditions();

    sLog->outString("Loading faction change achievement pairs...");
    sObjectMgr->LoadFactionChangeAchievements();

    sLog->outString("Loading faction change spell pairs...");
    sObjectMgr->LoadFactionChangeSpells();

    sLog->outString("Loading faction change item pairs...");
    sObjectMgr->LoadFactionChangeItems();

    sLog->outString("Loading faction change reputation pairs...");
    sObjectMgr->LoadFactionChangeReputations();

    sLog->outString("Loading faction change title pairs...");
    sObjectMgr->LoadFactionChangeTitles();

    sLog->outString("Loading faction change quest pairs...");
    sObjectMgr->LoadFactionChangeQuests();

    sLog->outString("Loading GM tickets...");
    sTicketMgr->LoadTickets();

    sLog->outString("Loading GM surveys...");
    sTicketMgr->LoadSurveys();

    sLog->outString("Loading client addons...");
    AddonMgr::LoadFromDB();

    sTransmogificationMgr->LoadEnchants();
    sTransmogificationMgr->LoadTransmogSets();

    // pussywizard:
    sLog->outString("Deleting invalid mail items...\n");
    CharacterDatabase.Query("DELETE mi FROM mail_items mi LEFT JOIN item_instance ii ON mi.item_guid = ii.guid WHERE ii.guid IS NULL");
    CharacterDatabase.Query("DELETE mi FROM mail_items mi LEFT JOIN mail m ON mi.mail_id = m.id WHERE m.id IS NULL");
    CharacterDatabase.Query("UPDATE mail m LEFT JOIN mail_items mi ON m.id = mi.mail_id SET m.has_items=0 WHERE m.has_items<>0 AND mi.mail_id IS NULL");

    ///- Handle outdated emails (delete/return)
    sLog->outString("Returning old mails...");
    sObjectMgr->ReturnOrDeleteOldMails(false);

    sLog->outString("Loading Autobroadcasts...");
    LoadAutobroadcasts();

    sLog->outString("Loading Ip2nation...");
    //LoadIp2nation();

    ///- Load and initialize scripts
    sObjectMgr->LoadSpellScripts(); // must be after load Creature/Gameobject(Template/Data)
    sObjectMgr->LoadEventScripts(); // must be after load Creature/Gameobject(Template/Data)
    sObjectMgr->LoadWaypointScripts();

    sLog->outString("Loading Scripts text locales..."); // must be after Load*Scripts calls
    sObjectMgr->LoadDbScriptStrings();

    sLog->outString("Loading spell script names...");
    sObjectMgr->LoadSpellScriptNames();

    sLog->outString("Loading Creature Texts...");
    sCreatureTextMgr->LoadCreatureTexts();

    sLog->outString("Loading Creature Text Locales...");
    sCreatureTextMgr->LoadCreatureTextLocales();

    sLog->outString("Initializing Scripts...");
    sScriptMgr->Initialize();
    sScriptMgr->OnConfigLoad(false); // must be done after the ScriptMgr has been properly initialized

    sLog->outString("Validating spell scripts...");
    sObjectMgr->ValidateSpellScripts();

    sLog->outString("Loading SmartAI scripts...");
    sSmartScriptMgr->LoadSmartAIFromDB();

    sLog->outString("Deleting calendar data that is at least 1 month old...");

    auto calendarDeleteCount = CharacterDatabase.PQuery("SELECT COUNT(id) FROM calendar_events WHERE eventtime < (UNIX_TIMESTAMP(NOW()) - %u)", CALENDAR_OLD_EVENTS_DELETION_TIME);
    if (calendarDeleteCount)
    {
        Field* fields = calendarDeleteCount->Fetch();
        if (auto count = fields[0].GetUInt64())
            sLog->outString("About to delete %u month old events from calendar_events", count);
    }
    CharacterDatabase.PQuery("DELETE FROM calendar_events WHERE eventtime < (UNIX_TIMESTAMP(NOW()) - %u)", CALENDAR_OLD_EVENTS_DELETION_TIME);
    CharacterDatabase.Query("DELETE FROM calendar_invites WHERE (event) NOT IN (SELECT id FROM calendar_events)");

    sLog->outString("Loading Calendar data...");
    sCalendarMgr->LoadFromDB();

    sLog->outString("Initializing SpellInfo precomputed data..."); // must be called after loading items, professions, spells and pretty much anything
    sObjectMgr->InitializeSpellInfoPrecomputedData();

    ///- Initialize game time and timers
    sLog->outString("Initialize game time and timers");
    m_gameTime  = time(nullptr);
    m_startTime = m_gameTime;

    LoginDatabase.PExecute("INSERT INTO uptime (realmid, starttime, uptime, revision) VALUES(%u, %u, 0, 'SunwellCore rev. %s')",
        realmID, uint32(m_startTime), GitRevision::GetHash()); // One-time query

    m_timers[WUPDATE_WEATHERS].SetInterval(1 * IN_MILLISECONDS);
    m_timers[WUPDATE_AUCTIONS].SetInterval(MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_AUCTIONS].SetCurrent(MINUTE * IN_MILLISECONDS);

    m_timers[WUPDATE_UPTIME].SetInterval(m_int_configs[CONFIG_UPTIME_UPDATE] * MINUTE * IN_MILLISECONDS);
    //Update "uptime" table based on configuration entry in minutes.

    m_timers[WUPDATE_INGAMESTATS].SetInterval(20 * MINUTE * IN_MILLISECONDS);

    m_timers[WUPDATE_CORPSES].SetInterval(20 * MINUTE * IN_MILLISECONDS);
    //erase corpses every 20 minutes
    m_timers[WUPDATE_CLEANDB].SetInterval(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] * MINUTE * IN_MILLISECONDS);
    // clean logs table every 14 days by default
    m_timers[WUPDATE_AUTOBROADCAST].SetInterval(getIntConfig(CONFIG_AUTOBROADCAST_INTERVAL));

    // our speed up
    m_timers[WUPDATE_5_SECS].SetInterval(5 * IN_MILLISECONDS);

    mail_expire_check_timer = time(nullptr) + 6 * 3600;

    ///- Initilize static helper structures
    AIRegistry::Initialize();

    Movement::GetPathGenerator().Initialize(getIntConfig(CONFIG_MAPUPDATER_NUMTHREADS));

    LoadCharacterScreenScript();

    ///- Initialize MapManager
    sLog->outString("Starting Map System");
    sMapMgr->Initialize();

    sLog->outString("Starting Game Event system...");
    uint32 nextGameEvent = sGameEventMgr->StartSystem();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent); //depend on next event

    // Delete all characters which have been deleted X days before
    Player::DeleteOldCharacters();

    // Delete all custom channels which haven't been used for PreserveCustomChannelDuration days.
    Channel::CleanOldChannelsInDB();

    sLog->outString("Starting Arena Season...");
    sGameEventMgr->StartArenaSeason();

    sTicketMgr->Initialize();

    //- Initialize World Zone Scripts
    sLog->outString("Starting WorldZoneScripts");
    sObjectMgr->LoadWorldZoneScripts();

    ///- Initialize Battlegrounds
    sLog->outString("Starting Battleground System");
    sBattlegroundMgr->CreateInitialBattlegrounds();
    sBattlegroundMgr->InitAutomaticArenaPointDistribution();

    ///- Initialize outdoor pvp
    sLog->outString("Starting Outdoor PvP System");
    sOutdoorPvPMgr->InitOutdoorPvP();

    ///- Initialize Battlefield
    sLog->outString("Starting Battlefield System");
    sBattlefieldMgr->InitBattlefield();

    sLog->outString("Loading Transports...");
    sTransportMgr->SpawnContinentTransports();

    ///- Initialize Warden
    sLog->outString("Loading Warden Checks...");
    sWardenCheckMgr->LoadWardenChecks();

    sLog->outString("Loading Warden Action Overrides...");
    sWardenCheckMgr->LoadWardenOverrides();

    sLog->outString("Deleting expired bans...");
    LoginDatabase.Execute(LoginDatabase.GetPreparedStatement(LOGIN_DEL_EXPIRED_IP_BANS));

    sLog->outString("Calculate next daily quest reset time...");
    InitDailyQuestResetTime();

    sLog->outString("Calculate next weekly quest reset time...");
    InitWeeklyQuestResetTime();

    sLog->outString("Calculate next monthly quest reset time...");
    InitMonthlyQuestResetTime();

    sLog->outString("Calculate random battleground reset time...");
    InitRandomBGResetTime();

    sLog->outString("Calculate Guild cap reset time...");
    InitGuildResetTime();

    sLog->outString("Load Petitions...");
    sPetitionMgr->LoadPetitions();

    sLog->outString("Load Petition Signs...");
    sPetitionMgr->LoadSignatures();

    sLog->outString("Load Stored Loot Items...");
    sLootItemStorage->LoadStorageFromDB();

    sLog->outString("Load Channel Rights...");
    sChannelMgr->LoadChannelRights();

    sLog->outString("Load Channels...");
    sChannelMgr->LoadChannels();

    sLog->outString("Load duel reset auras...");
    sWorldCache.LoadDuelResetAuras();

    sLog->outString("Load hellforge boss stats...");
    sWorldCache.LoadHellforgeBossValues();

    sLog->outString("Loading lottery rewards...");
    sWorldCache.LoadLotteryRewards();

    sLog->outString("Loading boost items...");
    sWorldCache.LoadBoostItems();

    // Preload all cells, if required for the base maps
    if (IsGridPreloadEnabled(GridPreload::World))
    {
        sMapMgr->DoForAllMaps([](Map* map)
        {
            if (!map->Instanceable())
            {
                sLog->outString("Pre-loading base map data for map %u", map->GetId());
                map->LoadAllCells();
            }
        });
    }

    //if (m_bool_configs[CONFIG_ENABLE_WEBHOOK_RELAY])
    //{
    //    GetRelay().LoadRelayAddresses();
    //    GetRelay().LoadJsonStrings();
    //    GetRelay().InitializeRelay(size_t(m_int_configs[CONFIG_WORLD_RELAY_NUMTHREADS]));
    //}

    GetChatProcessor().LoadChatProcessorFilters();

        CleanupWardenDatabase();

    CharacterDatabase.Execute("DELETE FROM client_action_count");
    //! Everything is now nice and loaded, we can distrubte lost mythic keys due to crash
    QueryResult res = CharacterDatabase.PQuery("SELECT PlayerGUID, Time FROM character_mythic_key_store");
    if (res)
    {
        do
        {
            Field* field       = res->Fetch();
            uint64 _playerGUID = field[0].GetUInt64();
            time_t _time       = static_cast<uint32>(field[1].GetUInt32());
            time_t _now        = time(nullptr);
            auto res           = _now - _time;
            if (res < 40 * MINUTE)
                RestoreMythicKeyFor(_playerGUID);

        } while (res->NextRow());
    }

    CharacterDatabase.PQuery("TRUNCATE TABLE character_mythic_key_store");

    uint32 startupDuration = GetMSTimeDiffToNow(startupBegin);
    sLog->outString();
    sLog->outError("WORLD: World initialized in %u minutes %u seconds", (startupDuration / 60000), ((startupDuration % 60000) / 1000));
    sLog->outString();

    // possibly enable db logging; avoid massive startup spam by doing it here.
    if (sConfigMgr->GetBoolDefault("EnableLogDB", false))
    {
        sLog->outString("Enabling database logging...");
        sLog->SetLogDB(true);
    }

    sCustomEventMgr->InitCustomEventMgr();

    stmt                       = CharacterDatabase.GetPreparedStatement(CHAR_SEL_MAIL_EXTERNAL_MAX);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (result)
    {
        Field* fields       = result->Fetch();
        m_maxExternalMailId = fields[0].GetUInt64();
    }
    else
        m_maxExternalMailId = 0;

    sCustomMgr->LoadCustomMgr();

    UpdateActivePlayersCount();
    m_timers[WUPDATE_ACTIVE_PLAYERS].SetInterval(600 * IN_MILLISECONDS);
}

void World::DetectDBCLang()
{
    ChrRacesEntry const* race = sChrRacesStore.LookupEntry(1);
    std::string availableLocalsStr;

    uint8 locale = TOTAL_LOCALES;
    for (uint8 i = locale - 1; i < TOTAL_LOCALES; --i) // -1 will be 255 due to uint8
    {
        if (race->name[i][0] != '\0') // check by race names
        {
            locale = i;
            availableLocalsStr += localeNames[i];
            availableLocalsStr += " ";
        }
    }

    if (locale != GetDefaultDbcLocale())
    {
        sLog->outError("Unable to determine your DBC Locale! (corrupt DBC?)");
        exit(1);
    }

    sLog->outString("Using %s DBC Locale as default. All available DBC locales: %s", localeNames[GetDefaultDbcLocale()], availableLocalsStr.empty() ? "<none>" : availableLocalsStr.c_str());
    sLog->outString();
}

void World::LoadAutobroadcasts()
{
    uint32 oldMSTime = getMSTime();

    m_Autobroadcasts.clear();
    m_AutobroadcastsWeights.clear();

    uint32 realmId          = sConfigMgr->GetIntDefault("RealmID", 0);
    auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTOBROADCAST);
    stmt->setInt32(0, realmId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
    {
        sLog->outString(">> Loaded 0 autobroadcasts definitions. DB table `autobroadcast` is empty for this realm!");
        sLog->outString();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 id     = fields[0].GetUInt8();

        m_Autobroadcasts[id]              = fields[2].GetString();
        m_AutobroadcastsWeights[id]       = fields[1].GetUInt8();

        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u autobroadcast definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void World::LoadIp2nation()
{
    /*
    uint32 oldMSTime = getMSTime();

    auto stmt    = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IP2NATION_COUNT);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    uint32 count = 0;

    if (result)
    {
        Field* fields = result->Fetch();
        count         = fields[0].GetUInt32();
    }

    sLog->outString(">> Loaded %u ip2nation definitions", count);
    sLog->outString();
    */
}

/// Update the World !
void World::Update(uint32 diff)
{
    PROFILE_SCOPE("World::Update");

    m_updateTime = diff;

    if (m_int_configs[CONFIG_INTERVAL_LOG_UPDATE])
    {
        m_updateTimeSum += diff;
        if (m_updateTimeSum > m_int_configs[CONFIG_INTERVAL_LOG_UPDATE])
        {
            sLog->outBasic("Average update time diff: %u. Players online: %u.", avgDiffTracker.getAverage(), (uint32)GetActiveSessionCount());
            m_updateTimeSum = 0;
        }
    }

    {
        PROFILE_SCOPE("World::ExecuteFunctions");

        std::lock_guard lock(m_executeMutex);
        for (auto&& function : m_executeQueue)
        {
            function();
        }

        m_executeQueue.clear();
    }

    DynamicVisibilityMgr::Update(GetActiveSessionCount());

    GetChatProcessor().Update(diff);

    ///- Update the different timers
    for (int i = 0; i < WUPDATE_COUNT; ++i)
    {
        if (m_timers[i].GetCurrent() >= 0)
            m_timers[i].Update(diff);
        else
            m_timers[i].SetCurrent(0);
    }

    sPremiumStoreMgr->Update();

    // pussywizard: our speed up and functionality
    if (m_timers[WUPDATE_5_SECS].Passed())
    {
        m_timers[WUPDATE_5_SECS].Reset();

        CharacterDatabase.KeepAlive();
        LoginDatabase.KeepAlive();
        WorldDatabase.KeepAlive();
        CustomDatabase.KeepAlive();
        LogDatabase.KeepAlive();

        // moved here from HandleCharEnumOpcode
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_EXPIRED_BANS);
        CharacterDatabase.Execute(stmt);

        // copy players hashmapholder to avoid mutex
        WhoListCacheMgr::Update();

        sObjectMgr->UpdateItemDestroyQueue();

        auto result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_WEB_COMMANDS));
        if (result)
        {
            do
            {
                Field* fields       = result->Fetch();
                std::string command = fields[1].GetString();
                uint32 target       = fields[2].GetUInt32();
                uint32 source       = fields[3].GetUInt32();
                uint32 access       = fields[4].GetUInt32();
                WebCommandHandler(source, target, access).CommandWrapper(command.c_str());
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_WEB_COMMANDS);
                stmt->setUInt32(0, fields[0].GetUInt32());
                CharacterDatabase.Execute(stmt);
            } while (result->NextRow());
        }
    }

    ///- Update the game time and check for shutdown time
    _UpdateGameTime();

    /// Handle daily quests reset time
    if (m_gameTime > m_NextDailyQuestReset)
        ResetDailyQuests();

    /// Handle weekly quests reset time
    if (m_gameTime > m_NextWeeklyQuestReset)
        ResetWeeklyQuests();

    /// Handle monthly quests reset time
    if (m_gameTime > m_NextMonthlyQuestReset)
        ResetMonthlyQuests();

    if (m_gameTime > m_NextRandomBGReset)
        ResetRandomBG();

    if (m_gameTime > m_NextGuildReset)
        ResetGuildCap();

    // pussywizard:
    // acquire mutex now, this is kind of waiting for listing thread to finish it's work (since it can't process next packet)
    // so we don't have to do it in every packet that modifies auctions
    AsyncAuctionListingMgr::SetAuctionListingAllowed(false);
    {
        std::lock_guard lg(AsyncAuctionListingMgr::GetLock());

        // pussywizard: handle auctions when the timer has passed
        if (m_timers[WUPDATE_AUCTIONS].Passed())
        {
            m_timers[WUPDATE_AUCTIONS].Reset();

            // pussywizard: handle expired auctions, auctions expired when realm was offline are also handled here (not during loading when many required things aren't loaded yet)
            sAuctionMgr->Update();
        }

        AsyncAuctionListingMgr::Update(diff);

        if (m_gameTime > mail_expire_check_timer)
        {
            sObjectMgr->ReturnOrDeleteOldMails(true);
            mail_expire_check_timer = m_gameTime + 6 * 3600;
        }

        UpdateSessions(diff);
    }
    // end of section with mutex
    AsyncAuctionListingMgr::SetAuctionListingAllowed(true);

    /// <li> Handle weather updates when the timer has passed
    if (m_timers[WUPDATE_WEATHERS].Passed())
    {
        m_timers[WUPDATE_WEATHERS].Reset();
        WeatherMgr::Update(uint32(m_timers[WUPDATE_WEATHERS].GetInterval()));
    }

    /// <li> Update uptime table
    if (m_timers[WUPDATE_UPTIME].Passed())
    {
        uint32 tmpDiff          = uint32(m_gameTime - m_startTime);
        uint32 maxOnlinePlayers = GetMaxPlayerCount();

        m_timers[WUPDATE_UPTIME].Reset();

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_UPTIME_PLAYERS);

        stmt->setUInt32(0, tmpDiff);
        stmt->setUInt16(1, uint16(maxOnlinePlayers));
        stmt->setUInt32(2, realmID);
        stmt->setUInt32(3, uint32(m_startTime));

        LoginDatabase.Execute(stmt);
    }

    /*
    /// <li> Update ingame stats table
    if (m_timers[WUPDATE_INGAMESTATS].Passed())
    {
        m_timers[WUPDATE_INGAMESTATS].Reset();

        uint32 connectedPlayers = sWorld->GetActiveSessionCount();
        uint32 playersCount     = sWorld->GetPlayerCount();
        uint32 updateTime       = sWorld->GetUpdateTime();
        uint32 avgUpdateTime    = avgDiffTracker.getAverage();
        uint32 rId              = sWorld->getBoolConfig(CONFIG_SPECIAL_ANGRATHAR) ? 2 : 1;

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_INGAME_STATISTICS);

        stmt->setUInt32(0, connectedPlayers);
        stmt->setUInt32(1, playersCount);
        stmt->setUInt32(2, updateTime);
        stmt->setUInt32(3, avgUpdateTime);
        stmt->setString(4, TimeToTimestampStr(sWorld->GetGameTime()));
        stmt->setUInt32(5, rId);

        LoginDatabase.Execute(stmt);
    }
    */

    sLFGMgr->Update(diff, 0); // pussywizard: remove obsolete stuff before finding compatibility during map update

    sMapMgr->Update(diff);

    if (sWorld->getBoolConfig(CONFIG_AUTOBROADCAST))
    {
        if (m_timers[WUPDATE_AUTOBROADCAST].Passed())
        {
            m_timers[WUPDATE_AUTOBROADCAST].Reset();
            SendAutoBroadcast();
        }
    }

    sSoloQueueMgr->Update(diff);
    sBattlegroundMgr->Update(diff);

    sOutdoorPvPMgr->Update(diff);

    sBattlefieldMgr->Update(diff);

    sLFGMgr->Update(diff, 2); // pussywizard: handle created proposals

    sCustomEventMgr->Update(diff, CUSTOM_EVENT_KRUUL);
    sCustomEventMgr->Update(diff, CUSTOM_EVENT_PVP);
    sCustomEventMgr->Update(diff, CUSTOM_EVENT_HUNGER_GAMES);
    sCustomEventMgr->Update(diff, CUSTOM_EVENT_LOTTERY);

    // execute callbacks from sql queries that were queued recently
    ProcessQueryCallbacks();

    ///- Erase corpses once every 20 minutes
    if (m_timers[WUPDATE_CORPSES].Passed())
    {
        m_timers[WUPDATE_CORPSES].Reset();
        sObjectAccessor->RemoveOldCorpses();
    }

    ///- Process Game events when necessary
    if (m_timers[WUPDATE_EVENTS].Passed())
    {
        m_timers[WUPDATE_EVENTS].Reset(); // to give time for Update() to be processed
        uint32 nextGameEvent = sGameEventMgr->Update();
        m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
        m_timers[WUPDATE_EVENTS].Reset();
    }

    // update the instance reset times
    sInstanceSaveMgr->Update();

    // And last, but not least handle the issued cli commands
    ProcessCliCommands();

    sScriptMgr->OnWorldUpdate(diff);
    sWorldCache.OnWorldUpdate(diff);
    sMythicMgr.HandleAsyncQueries();

    SavingSystemMgr::Update(diff);

    // update external mails
    if (getBoolConfig(CONFIG_EXTERNAL_MAIL) && m_timers[WUPDATE_EXTERNAL_MAILS].Passed())
    {
        m_timers[WUPDATE_EXTERNAL_MAILS].Reset();

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_MAIL_EXTERNAL_WORLD);
        stmt->setUInt64(0, m_maxExternalMailId);
        _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery(stmt).WithPreparedCallback([this](PreparedQueryResult resultExternalMail)
        {
            if (!resultExternalMail)
                return;

            Field* fields;
            do
            {
                fields = resultExternalMail->Fetch();
                if (Player* player = ObjectAccessor::FindPlayer(fields[6].GetUInt64()))
                    player->SendExternalMail(fields);

            } while (resultExternalMail->NextRow());
            m_maxExternalMailId = fields[0].GetUInt64();
        }));
    }

    if (m_timers[WUPDATE_WARDEN_REPORTS].Passed())
    {
        auto trans = CharacterDatabase.BeginTransaction();

        std::unique_lock<std::mutex> lock(m_wardenReportMutex);
        PreparedStatement<CharacterDatabaseConnection>* stmt = nullptr;

        for (auto const& ref : m_wardenReports)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_WARDEN_CHECK_FAILURE);
            if (!stmt)
                continue;

            stmt->setUInt32(0, ref.AccountId);
            stmt->setUInt32(1, ref.GuidLow);
            stmt->setString(2, ref.CharacterName);
            stmt->setUInt32(3, ref.FailureTime);
            stmt->setString(4, ref.FailureMessage);

            trans->Append(stmt);
        }

        CharacterDatabase.CommitTransaction(trans);
        m_wardenReports.clear();
        m_timers[WUPDATE_WARDEN_REPORTS].Reset();
    }

    if (m_timers[WUPDATE_ACTIVE_PLAYERS].Passed())
    {
        UpdateActivePlayersCount();
        m_timers[WUPDATE_ACTIVE_PLAYERS].Reset();
    }
}

void World::ForceGameEventUpdate()
{
    m_timers[WUPDATE_EVENTS].Reset(); // to give time for Update() to be processed
    uint32 nextGameEvent = sGameEventMgr->Update();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
    m_timers[WUPDATE_EVENTS].Reset();
}

/// Send a packet to all players (except self if mentioned)
void World::SendGlobalMessage(WorldPacket* packet, WorldSession* self, TeamId teamId)
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld() && itr->second != self && (teamId == TEAM_NEUTRAL || itr->second->GetPlayer()->GetTeam(CF_DENY) == teamId))
        {
            itr->second->SendPacket(packet);
        }
    }
}

/// Send a packet to all GMs (except self if mentioned)
void World::SendGlobalGMMessage(WorldPacket* packet, WorldSession* self, TeamId teamId)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld() && itr->second != self && !AccountMgr::IsPlayerAccount(itr->second->GetSecurity()) && (teamId == TEAM_NEUTRAL || itr->second->GetPlayer()->GetTeam(CF_DENY) == teamId))
        {
            itr->second->SendPacket(packet);
        }
    }
}

namespace Trinity {
class WorldWorldTextBuilder
{
public:
    typedef std::vector<WorldPacket*> WorldPacketList;
    explicit WorldWorldTextBuilder(int32 textId, va_list* args = nullptr)
        : i_textId(textId)
        , i_args(args)
    {
    }
    void operator()(WorldPacketList& data_list, LocaleConstant loc_idx)
    {
        char const* text = sObjectMgr->GetTrinityString(i_textId, loc_idx);

        if (i_args)
        {
            // we need copy va_list before use or original va_list will corrupted
            va_list ap;
            va_copy(ap, *i_args);

            char str[2048];
            vsnprintf(str, 2048, text, ap);
            va_end(ap);

            do_helper(data_list, &str[0]);
        }
        else
            do_helper(data_list, (char*)text);
    }

private:
    char* lineFromMessage(char*& pos)
    {
        char* start = strtok(pos, "\n");
        pos         = nullptr;
        return start;
    }
    void do_helper(WorldPacketList& data_list, char* text)
    {
        char* pos = text;
        while (char* line = lineFromMessage(pos))
        {
            WorldPacket* data = new WorldPacket();
            ChatHandler::BuildChatPacket(*data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, nullptr, nullptr, line);
            data_list.push_back(data);
        }
    }


    int32 i_textId;
    va_list* i_args;
};
} // namespace Trinity

/// Send a System Message to all players (except self if mentioned)
void World::SendWorldText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}

/// Send a System Message to all players (except pvp announcer option disabled)
void World::SendPvPWorldText(bool sendToGms, int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || !itr->second->GetPlayer()->PvPAnnounces())
            continue;

        if (!sendToGms && itr->second->GetPlayer()->GetSession()->GetSecurity() > SEC_PLAYER)
            continue;

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}

/// Send a System Message to all GMs (except self if mentioned)
void World::SendGMText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        if (AccountMgr::IsPlayerAccount(itr->second->GetSecurity()))
            continue;

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}

/// DEPRECATED, only for debug purpose. Send a System Message to all players (except self if mentioned)
void World::SendGlobalText(const char* text, WorldSession* self)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(text);
    char* pos = buf;

    while (char* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, nullptr, nullptr, line);
        SendGlobalMessage(&data, self);
    }

    free(buf);
}

/// Send a packet to all players (or players selected team) in the zone (except self if mentioned)
bool World::SendZoneMessage(uint32 zone, WorldPacket* packet, WorldSession* self, TeamId teamId)
{
    bool foundPlayerToSend = false;
    SessionMap::const_iterator itr;

    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld() && itr->second->GetPlayer()->GetZoneId() == zone && itr->second != self && (teamId == TEAM_NEUTRAL || itr->second->GetPlayer()->GetTeam() == teamId))
        {
            itr->second->SendPacket(packet);
            foundPlayerToSend = true;
        }
    }

    return foundPlayerToSend;
}

/// Send a System Message to all players in the zone (except self if mentioned)
void World::SendZoneText(uint32 zone, const char* text, WorldSession* self, TeamId teamId)
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, nullptr, nullptr, text);
    SendZoneMessage(zone, &data, self, teamId);
}

/// Kick (and save) all players
void World::KickAll()
{
    m_QueuedPlayer.clear(); // prevent send queue update packet and login queued sessions

    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        itr->second->KickPlayer();

    // pussywizard: kick offline sessions
    for (SessionMap::const_iterator itr = m_offlineSessions.begin(); itr != m_offlineSessions.end(); ++itr)
        itr->second->KickPlayer();
}

/// Kick (and save) all players with security level less `sec`
void World::KickAllLess(AccountTypes sec)
{
    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetSecurity() < sec)
            itr->second->KickPlayer();
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string const& nameOrIP, std::string const& duration, std::string const& reason, std::string const& author)
{
    uint32 duration_secs = TimeStringToSecs(duration);
    return BanAccount(mode, nameOrIP, duration_secs, reason, author);
}

/// Ban an account or ban an IP address, duration is in seconds if positive, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string const& nameOrIP, uint32 duration_secs, std::string const& reason, std::string const& author)
{
    PreparedQueryResult resultAccounts = PreparedQueryResult(nullptr); //used for kicking
    PreparedStatement<LoginDatabaseConnection>* stmt = nullptr;

    ///- Update the database with ban information
    switch (mode)
    {
        case BAN_IP:
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BY_IP);
            stmt->setString(0, nameOrIP);
            resultAccounts = LoginDatabase.Query(stmt);
            stmt           = LoginDatabase.GetPreparedStatement(LOGIN_INS_IP_BANNED);
            stmt->setString(0, nameOrIP);
            stmt->setUInt32(1, duration_secs);
            stmt->setString(2, author);
            stmt->setString(3, reason);
            LoginDatabase.Execute(stmt);
            break;
        case BAN_ACCOUNT:
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
            stmt->setString(0, nameOrIP);
            resultAccounts = LoginDatabase.Query(stmt);
            break;
        case BAN_CHARACTER: 
        {
            auto charStmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_BY_NAME);
            charStmt->setString(0, nameOrIP);
            resultAccounts = CharacterDatabase.Query(charStmt);
            break;
        }
        default:
            return BAN_SYNTAX_ERROR;
    }

    if (!resultAccounts)
    {
        if (mode == BAN_IP)
            return BAN_SUCCESS; // ip correctly banned but nobody affected (yet)
        else
            return BAN_NOTFOUND; // Nobody to ban
    }

    ///- Disconnect all affected players (for IP it can be several)
    auto trans = LoginDatabase.BeginTransaction();
    do
    {
        Field* fieldsAccount = resultAccounts->Fetch();
        uint32 account       = fieldsAccount[0].GetUInt32();

        if (mode != BAN_IP)
        {
            // pussywizard: check existing ban to prevent overriding by a shorter one! >_>
            auto stmtx = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BANNED);
            stmtx->setUInt32(0, account);
            PreparedQueryResult banresultx = LoginDatabase.Query(stmtx);
            if (banresultx && ((*banresultx)[0].GetUInt32() == (*banresultx)[1].GetUInt32() || ((*banresultx)[1].GetUInt32() > time(nullptr) + duration_secs && duration_secs)))
                return BAN_LONGER_EXISTS;

            // make sure there is only one active ban
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED);
            stmt->setUInt32(0, account);
            trans->Append(stmt);
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_BANNED);
            stmt->setUInt32(0, account);
            stmt->setUInt32(1, duration_secs);
            stmt->setString(2, author);
            stmt->setString(3, reason);
            trans->Append(stmt);
        }

        if (WorldSession* sess = FindSession(account))
            if (sess->GetPlayerName() != author)
                sess->KickPlayer();
        if (WorldSession* sess = FindOfflineSession(account))
            if (sess->GetPlayerName() != author)
                sess->KickPlayer();
    } while (resultAccounts->NextRow());

    LoginDatabase.CommitTransaction(trans);

    return BAN_SUCCESS;
}

/// Remove a ban from an account or IP address
bool World::RemoveBanAccount(BanMode mode, std::string const& nameOrIP)
{
    PreparedStatement<LoginDatabaseConnection>* stmt = nullptr;
    if (mode == BAN_IP)
    {
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_IP_NOT_BANNED);
        stmt->setString(0, nameOrIP);
        LoginDatabase.Execute(stmt);
    }
    else
    {
        uint32 account = 0;
        if (mode == BAN_ACCOUNT)
            account = AccountMgr::GetId(nameOrIP);
        else if (mode == BAN_CHARACTER)
            account = sObjectMgr->GetPlayerAccountIdByPlayerName(nameOrIP);

        if (!account)
            return false;

        //NO SQL injection as account is uint32
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED);
        stmt->setUInt32(0, account);
        LoginDatabase.Execute(stmt);
    }
    return true;
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanCharacter(std::string const& name, std::string const& duration, std::string const& reason, std::string const& author)
{
    Player* pBanned = ObjectAccessor::FindPlayerByName(name, false);
    uint32 guid     = 0;

    uint32 duration_secs = TimeStringToSecs(duration);

    /// Pick a player to ban if not online
    if (!pBanned)
    {
        guid = sWorld->GetGlobalPlayerGUID(name);
        if (!guid)
            return BAN_NOTFOUND;
    }
    else
        guid = pBanned->GetGUIDLow();

    // make sure there is only one active ban
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_BAN);
    stmt->setUInt32(0, guid);
    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_BAN);
    stmt->setUInt32(0, guid);
    stmt->setUInt32(1, duration_secs);
    stmt->setString(2, author);
    stmt->setString(3, reason);
    CharacterDatabase.Execute(stmt);

    if (pBanned)
        pBanned->GetSession()->KickPlayer();

    return BAN_SUCCESS;
}

/// Remove a ban from a character
bool World::RemoveBanCharacter(std::string const& name)
{
    Player* pBanned = ObjectAccessor::FindPlayerByName(name, false);
    uint32 guid     = 0;

    /// Pick a player to ban if not online
    if (!pBanned)
        guid = sWorld->GetGlobalPlayerGUID(name);
    else
        guid = pBanned->GetGUIDLow();

    if (!guid)
        return false;

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_BAN);
    stmt->setUInt32(0, guid);
    CharacterDatabase.Execute(stmt);
    return true;
}

void World::SetPhasedDuelsZones(std::string zones)
{
    m_phasedDuelsZones.clear();
    if (zones.empty())
        return;

    std::stringstream ss(zones);
    while (!ss.eof())
    {
        std::string buff;
        ss >> buff;
        try
        {
            m_phasedDuelsZones.push_back(std::stoi(buff));
        }
        catch (std::invalid_argument)
        {
            m_phasedDuelsZones.clear();
            return;
        }
    }
}

bool World::PatchNotes(ContentPatches patchSince, ContentPatches patchTo) const
{
    return getIntConfig(CONFIG_CURRENT_BUILD) >= uint32(patchSince) && getIntConfig(CONFIG_CURRENT_BUILD) <= uint32(patchTo);
}

void World::CleanupWardenDatabase()
{
    uint32 count        = 0;
    uint32 removedCount = 0;
    time_t _curTime     = time(nullptr);
    uint64 twoMonths    = uint64(_curTime - (2 * MONTH));
    uint64 threeMonths  = uint64(_curTime - (3 * MONTH));

    //! get amount of rows that are 2 months old and younger than 3 months
    auto expiring = CharacterDatabase.GetPreparedStatement(CHAR_SEL_WARDEN_CHECKS_COUNT);
    expiring->setUInt64(0, (uint64)twoMonths);
    expiring->setUInt64(1, (uint64)threeMonths);

    PreparedQueryResult result = CharacterDatabase.Query(expiring);
    if (result)
    {
        Field* fields = result->Fetch();
        count         = fields[0].GetUInt64();
    }

    //! get amount of rows that are 3 months old
    auto expired = CharacterDatabase.GetPreparedStatement(CHAR_SEL_WARDEN_CHECKS_OLD_COUNT);
    expired->setUInt64(0, threeMonths);

    result = CharacterDatabase.Query(expired);
    if (result)
    {
        Field* fields = result->Fetch();
        removedCount  = fields[0].GetUInt64();
    }

    //! delete 3 months old+ rows
    auto remove = CharacterDatabase.GetPreparedStatement(CHAR_DEL_WARDEN_CHECKS_TOO_OLD);
    remove->setUInt64(0, threeMonths);
    CharacterDatabase.Execute(remove);

    //RelayData data;
    //data.message = fmt::format("Analysis of warden_lua_failure for realm: {} \n"
    //                           "* Found {} rows older than 2 months and younger than 3 months.\n"
    //                           "* Removed {} rows that were at least 3 months old.",
    //    sWorld->GetRealmName(), count, removedCount);

    //sLog->outString("%s", data.message.c_str());

    //GetRelay().Add(std::make_pair(TYPE_LUA_FAILURES_INFORM, data));
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    time_t thisTime = time(nullptr);
    uint32 elapsed  = uint32(thisTime - m_gameTime);
    m_gameTime      = thisTime;
    m_gameMSTime    = getMSTime();

    ///- if there is a shutdown timer
    if (!IsStopped() && m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if (m_ShutdownTimer <= elapsed)
        {
            if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
                m_stopEvent.store(true, std::memory_order_relaxed); // exist code already set
            else
                m_ShutdownTimer = 1; // minimum timer value to wait idle state
        }
        ///- ... else decrease it and if necessary display a shutdown countdown to the users
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
}

void World::OnShutdown()
{
    UpdateSessions(1); // real players unload required UpdateSessions call
    Movement::GetPathGenerator().Shutdown();
}

/// Shutdown the server
void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode, char const* exitmsg)
{
    // ignore if server shutdown at next tick
    if (IsStopped())
        return;

    m_ShutdownMask   = options;
    m_ExitCode       = exitcode;
    m_ShutdownReason = exitmsg;

    ///- If the shutdown time is 0, set m_stopEvent (except if shutdown is 'idle' with remaining sessions)
    if (time == 0)
    {
        if (!(options & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
            m_stopEvent.store(true, std::memory_order_relaxed); // exist code already set
        else
            m_ShutdownTimer = 1; //So that the session count is re-evaluated at next world tick
    }
    ///- Else set the shutdown timer and warn users
    else
    {
        m_ShutdownTimer = time;
        ShutdownMsg(true);
    }

    sScriptMgr->OnShutdownInitiate(ShutdownExitCode(exitcode), ShutdownMask(options));
}

/// Display a shutdown message to the user(s)
void World::ShutdownMsg(bool show, Player* player)
{
    // not show messages for idle shutdown mode
    if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
        return;

    ///- Display a message every 12 hours, hours, 5 minutes, minute, 5 seconds and finally seconds
    if (show || (m_ShutdownTimer < 1 * MINUTE && (m_ShutdownTimer % 15) == 0) ||    // < 1 min; every 15 sec
        (m_ShutdownTimer < 15 * MINUTE && (m_ShutdownTimer % MINUTE) == 0) ||       // < 15 min ; every 1 min
        (m_ShutdownTimer < 30 * MINUTE && (m_ShutdownTimer % (5 * MINUTE)) == 0) || // < 30 min ; every 5 min
        (m_ShutdownTimer < 12 * HOUR && (m_ShutdownTimer % HOUR) == 0) ||           // < 12 h ; every 1 h
        (m_ShutdownTimer > 12 * HOUR && (m_ShutdownTimer % (12 * HOUR)) == 0))      // > 12 h ; every 12 h
    {
        std::string str = secsToTimeString(m_ShutdownTimer).append(".");
        str += " Reason: " + m_ShutdownReason + ".";

        ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_TIME : SERVER_MSG_SHUTDOWN_TIME;

        SendServerMessage(msgid, str.c_str(), player);
        ; //sLog->outStaticDebug("Server is %s in %s", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"), str.c_str());
    }
}

/// Cancel a planned server shutdown
void World::ShutdownCancel()
{
    // nothing cancel or too later
    if (!m_ShutdownTimer || IsStopped())
        return;

    ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_CANCELLED : SERVER_MSG_SHUTDOWN_CANCELLED;

    m_ShutdownMask   = 0;
    m_ShutdownTimer  = 0;
    m_ShutdownReason = "no-reason";

    m_ExitCode = SHUTDOWN_EXIT_CODE; // to default value
    SendServerMessage(msgid);

    ; //sLog->outStaticDebug("Server %s cancelled.", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"));

    sScriptMgr->OnShutdownCancel();
}

/// Send a server message to the user(s)
void World::SendServerMessage(ServerMessageType type, const char* text, Player* player, TeamId teamId)
{
    WorldPacket data(SMSG_SERVER_MESSAGE, 50); // guess size
    data << uint32(type);
    if (type <= SERVER_MSG_STRING)
        data << text;

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        SendGlobalMessage(&data, nullptr, teamId);
}

void World::UpdateSessions(uint32 diff)
{
    PROFILE_SCOPE("World::UpdateSessions");
    if (m_timers[WUPDATE_COLLECTSESSIONS].Passed())
    {
        m_timers[WUPDATE_COLLECTSESSIONS].Reset();

        WorldSession* session;
        while (m_newSessions.Dequeue(session))
        {
            AddSession(session);
        }
    }

    ///- Then send an update signal to remaining ones
    for (SessionMap::iterator itr = m_sessions.begin(), next; itr != m_sessions.end(); itr = next)
    {
        next = itr;
        ++next;

        ///- and remove not active sessions from the list
        WorldSession* pSession = itr->second;
        WorldSessionFilter updater(pSession);

        // pussywizard:
        if (pSession->HandleSocketClosed())
        {
            if (!RemoveQueuedPlayer(pSession) && getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
                m_disconnects[pSession->GetAccountId()] = time(nullptr);
            m_sessions.erase(itr);
            // there should be no offline session if current one is logged onto a character
            SessionMap::iterator iter;
            if ((iter = m_offlineSessions.find(pSession->GetAccountId())) != m_offlineSessions.end())
            {
                WorldSession* tmp = iter->second;
                m_offlineSessions.erase(iter);
                tmp->SetShouldSetOfflineInDB(false);
                delete tmp;
            }
            pSession->SetOfflineTime(time(nullptr));
            m_offlineSessions[pSession->GetAccountId()] = pSession;
            continue;
        }

        if (!pSession->Update(diff, updater))
        {
            if (!RemoveQueuedPlayer(pSession) && getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
                m_disconnects[pSession->GetAccountId()] = time(nullptr);
            m_sessions.erase(itr);
            if (m_offlineSessions.find(pSession->GetAccountId()) != m_offlineSessions.end()) // pussywizard: don't set offline in db because offline session for that acc is present (character is in world)
                pSession->SetShouldSetOfflineInDB(false);
            delete pSession;
        }
    }

    // pussywizard:
    if (m_offlineSessions.empty())
        return;

    uint32 currTime = time(nullptr);
    for (SessionMap::iterator itr = m_offlineSessions.begin(), next; itr != m_offlineSessions.end(); itr = next)
    {
        next = itr;
        ++next;
        WorldSession* pSession = itr->second;
        if (!pSession->GetPlayer() || pSession->GetOfflineTime() + 60 < currTime || pSession->IsKicked())
        {
            m_offlineSessions.erase(itr);
            if (m_sessions.find(pSession->GetAccountId()) != m_sessions.end())
                pSession->SetShouldSetOfflineInDB(false); // pussywizard: don't set offline in db because new session for that acc is already created
            delete pSession;
        }
    }
}

// This handles the issued and queued CLI commands
void World::ProcessCliCommands()
{
    while (!cliCmdQueue.empty())
    {
        auto command = cliCmdQueue.pop();
        if (!command) break;
    CliCommandHolder::Print* zprint = nullptr;
    void* callbackArg               = nullptr;
        ; //sLog->outDetail("CLI command under processing...");
        zprint      = command->m_print;
        callbackArg = command->m_callbackArg;
        CliHandler handler(callbackArg, zprint);
        handler.ParseCommands(command->m_command.c_str());
        if (command->m_commandFinished)
            command->m_commandFinished(callbackArg, !handler.HasSentErrorMessage());
    }
}

void World::SendAutoBroadcast()
{
    if (m_Autobroadcasts.empty())
        return;

    uint32 weight = 0;
    AutobroadcastsWeightMap selectionWeights;
    std::string msg = "";

    for (AutobroadcastsWeightMap::const_iterator it = m_AutobroadcastsWeights.begin(); it != m_AutobroadcastsWeights.end(); ++it)
    {
        if (it->second)
        {
            weight += it->second;
            selectionWeights[it->first] = it->second;
        }
    }

    if (weight)
    {
        uint32 selectedWeight = urand(0, weight - 1);
        weight                = 0;
        for (AutobroadcastsWeightMap::const_iterator it = selectionWeights.begin(); it != selectionWeights.end(); ++it)
        {
            weight += it->second;
            if (selectedWeight < weight)
            {
                msg           = m_Autobroadcasts[it->first];
                break;
            }
        }
    }
    else
    {
        uint8 broadcastId = urand(0, m_Autobroadcasts.size());
        msg               = m_Autobroadcasts[broadcastId];
    }

    sWorld->SendWorldText(LANG_AUTO_BROADCAST, msg.c_str());
    sLog->outBasic("AutoBroadcast: '%s'", msg.c_str());
}

void World::UpdateRealmCharCount(uint32 accountId)
{
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_COUNT);
    stmt->setUInt32(0, accountId);

    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&World::_UpdateRealmCharCount, this, std::placeholders::_1)));
}

void World::_UpdateRealmCharCount(PreparedQueryResult resultCharCount)
{
    // unused
    if (resultCharCount)
    {
        Field* fields    = resultCharCount->Fetch();
        uint32 accountId = fields[0].GetUInt32();
        uint8 charCount  = uint8(fields[1].GetUInt64());

        auto trans = LoginDatabase.BeginTransaction();

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHARACTERS_BY_REALM);
        stmt->setUInt32(0, accountId);
        stmt->setUInt32(1, realmID);
        trans->Append(stmt);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_REALM_CHARACTERS);
        stmt->setUInt8(0, charCount);
        stmt->setUInt32(1, accountId);
        stmt->setUInt32(2, realmID);
        trans->Append(stmt);

        LoginDatabase.CommitTransaction(trans);
    }
}

// int8 dayOfWeek: 0 (sunday) to 6 (saturday)
time_t World::GetNextTimeWithDayAndHourAndMinute(int8 dayOfWeek, int8 hour, int8 minute)
{
    if (hour < 0 || hour > 23)
        hour = 0;

    if (minute < 0 || minute > 59)
        minute = 0;

    tm localTm = tm_from_time_t(time(nullptr));
    localTm.tm_hour = hour;
    localTm.tm_min  = minute;
    localTm.tm_sec  = 0;
    uint32 add;
    if (dayOfWeek < 0 || dayOfWeek > 6)
        dayOfWeek = (localTm.tm_wday + 1) % 7;
    if (localTm.tm_wday >= dayOfWeek)
        add = (7 - (localTm.tm_wday - dayOfWeek)) * DAY;
    else
        add = (dayOfWeek - localTm.tm_wday) * DAY;
    return mktime(&localTm) + add;
}

// int8 month: 0 (january) to 11 (december)
time_t World::GetNextTimeWithMonthAndHourAndMinute(int8 month, int8 hour, int8 minute)
{
    if (hour < 0 || hour > 23)
        hour = 0;

    if (minute < 0 || minute > 59)
        minute = 0;

    tm localTm = tm_from_time_t(time(nullptr));
    localTm.tm_mday = 1;
    localTm.tm_hour = hour;
    localTm.tm_min  = minute;
    localTm.tm_sec  = 0;
    if (month < 0 || month > 11)
    {
        month = (localTm.tm_mon + 1) % 12;
        if (month == 0)
            localTm.tm_year += 1;
    }
    else if (localTm.tm_mon >= month)
        localTm.tm_year += 1;
    localTm.tm_mon = month;
    return mktime(&localTm);
}

void World::InitWeeklyQuestResetTime()
{
    time_t wstime          = time_t(sWorld->getWorldState(WS_WEEKLY_QUEST_RESET_TIME));
    m_NextWeeklyQuestReset = wstime ? wstime : GetNextTimeWithDayAndHourAndMinute(3, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    if (!wstime)
        sWorld->setWorldState(WS_WEEKLY_QUEST_RESET_TIME, uint64(m_NextWeeklyQuestReset));
}

void World::InitDailyQuestResetTime()
{
    time_t wstime         = time_t(sWorld->getWorldState(WS_DAILY_QUEST_RESET_TIME));
    m_NextDailyQuestReset = wstime ? wstime : GetNextTimeWithDayAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    if (!wstime)
        sWorld->setWorldState(WS_DAILY_QUEST_RESET_TIME, uint64(m_NextDailyQuestReset));
}

void World::InitMonthlyQuestResetTime()
{
    time_t wstime           = time_t(sWorld->getWorldState(WS_MONTHLY_QUEST_RESET_TIME));
    m_NextMonthlyQuestReset = wstime ? wstime : GetNextTimeWithMonthAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    if (!wstime)
        sWorld->setWorldState(WS_MONTHLY_QUEST_RESET_TIME, uint64(m_NextMonthlyQuestReset));
}

void World::InitRandomBGResetTime()
{
    time_t wstime       = time_t(sWorld->getWorldState(WS_BG_DAILY_RESET_TIME));
    m_NextRandomBGReset = wstime ? wstime : GetNextTimeWithDayAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    if (!wstime)
        sWorld->setWorldState(WS_BG_DAILY_RESET_TIME, uint64(m_NextRandomBGReset));
}

void World::InitGuildResetTime()
{
    time_t wstime    = time_t(getWorldState(WS_GUILD_DAILY_RESET_TIME));
    m_NextGuildReset = wstime ? wstime : GetNextTimeWithDayAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    if (!wstime)
        sWorld->setWorldState(WS_GUILD_DAILY_RESET_TIME, uint64(m_NextGuildReset));
}

void World::ResetDailyQuests()
{
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_DAILY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetDailyQuestStatus();

    m_NextDailyQuestReset = GetNextTimeWithDayAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    sWorld->setWorldState(WS_DAILY_QUEST_RESET_TIME, uint64(m_NextDailyQuestReset));

    // change available dailies
    sPoolMgr->ChangeDailyQuests();

    sAnticheatMgr->ResetDailyReportStates();
}

void World::LoadDBAllowedSecurityLevel()
{
    auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALMLIST_SECURITY_LEVEL);
    stmt->setInt32(0, int32(realmID));
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (result)
        SetPlayerSecurityLimit(AccountTypes(result->Fetch()->GetUInt8()));
}

void World::SetPlayerSecurityLimit(AccountTypes _sec)
{
    AccountTypes sec       = _sec < SEC_CONSOLE ? _sec : SEC_PLAYER;
    bool update            = sec > m_allowedSecurityLevel;
    m_allowedSecurityLevel = sec;
    if (update)
        KickAllLess(m_allowedSecurityLevel);
}

void World::ResetWeeklyQuests()
{
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_WEEKLY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetWeeklyQuestStatus();

    m_NextWeeklyQuestReset = GetNextTimeWithDayAndHourAndMinute(3, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    sWorld->setWorldState(WS_WEEKLY_QUEST_RESET_TIME, uint64(m_NextWeeklyQuestReset));

    // change available weeklies
    sPoolMgr->ChangeWeeklyQuests();
}

void World::ResetMonthlyQuests()
{
    sLog->outString("Monthly quests reset for all characters.");

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_MONTHLY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetMonthlyQuestStatus();

    m_NextMonthlyQuestReset = GetNextTimeWithMonthAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    sWorld->setWorldState(WS_MONTHLY_QUEST_RESET_TIME, uint64(m_NextMonthlyQuestReset));
}

void World::ResetEventSeasonalQuests(uint16 event_id)
{
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_QUEST_STATUS_SEASONAL);
    stmt->setUInt16(0, event_id);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetSeasonalQuestStatus(event_id);
}

void World::ResetRandomBG()
{
    ; //sLog->outDetail("Random BG status reset for all characters.");

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BATTLEGROUND_RANDOM);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->SetRandomWinner(false);

    m_NextRandomBGReset = GetNextTimeWithDayAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    sWorld->setWorldState(WS_BG_DAILY_RESET_TIME, uint64(m_NextRandomBGReset));
}

void World::ResetGuildCap()
{
    sLog->outString("Guild Daily Cap reset.");

    m_NextGuildReset = GetNextTimeWithDayAndHourAndMinute(-1, sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_HOUR), sWorld->getIntConfig(CONFIG_SERVER_RESET_TIME_MINUTE));
    sWorld->setWorldState(WS_GUILD_DAILY_RESET_TIME, uint64(m_NextGuildReset));

    sGuildMgr->ResetTimes();
}

void World::LoadDBVersion()
{
    QueryResult result = WorldDatabase.Query("SELECT db_version, cache_id FROM version LIMIT 1");
    if (result)
    {
        Field* fields = result->Fetch();
        m_DBVersion   = fields[0].GetString();

        // will be overwrite by config values if different and non-0
        m_int_configs[CONFIG_CLIENTCACHE_VERSION] = fields[1].GetUInt32();
    }

    if (m_DBVersion.empty())
        m_DBVersion = "Unknown world database.";
}

void World::UpdateAreaDependentAuras()
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
        {
            itr->second->GetPlayer()->UpdateAreaDependentAuras(itr->second->GetPlayer()->GetAreaId());
            itr->second->GetPlayer()->UpdateZoneDependentAuras(itr->second->GetPlayer()->GetZoneId());
        }
}

void World::LoadWorldStates()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT entry, value FROM worldstates");

    if (!result)
    {
        sLog->outString(">> Loaded 0 world states. DB table `worldstates` is empty!");
        sLog->outString();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields                        = result->Fetch();
        m_worldstates[fields[0].GetUInt32()] = fields[1].GetUInt32();
        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u world states in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

// Setting a worldstate will save it to DB
void World::setWorldState(uint32 index, uint64 value)
{
    WorldStatesMap::const_iterator it = m_worldstates.find(index);
    if (it != m_worldstates.end())
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_WORLDSTATE);

        stmt->setUInt32(0, uint32(value));
        stmt->setUInt32(1, index);

        CharacterDatabase.Execute(stmt);
    }
    else
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_WORLDSTATE);

        stmt->setUInt32(0, index);
        stmt->setUInt32(1, uint32(value));

        CharacterDatabase.Execute(stmt);
    }
    m_worldstates[index] = value;
}

uint64 World::getWorldState(uint32 index) const
{
    WorldStatesMap::const_iterator it = m_worldstates.find(index);
    return it != m_worldstates.end() ? it->second : 0;
}

void World::ProcessQueryCallbacks()
{
    _queryProcessor.ProcessReadyCallbacks();
}

void World::InvalidatePlayerGlobalData(uint32 guid)
{
    WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
    data << MAKE_NEW_GUID(guid, 0, HIGHGUID_PLAYER);
    SendGlobalMessage(&data);
}

std::string World::GetCrossfactionConfigInfo() const
{
    if (getIntConfig(CONFIG_CROSSFACTION_ENABLED_FEATURES) == 0)
        return ">> Crossfaction system disabled.";

    constexpr std::array CrossfactionTypes
    {
        Crossfaction::WhoCommand,
        Crossfaction::Mail,
        Crossfaction::DungeonFinder,
        Crossfaction::Battlegrounds,
        Crossfaction::AuctionHouse,
        Crossfaction::Channels,
        Crossfaction::Group,
        Crossfaction::Item,
        Crossfaction::Trade,
        Crossfaction::Chat,
        Crossfaction::Guild,
        Crossfaction::ArenaTeam,
        Crossfaction::HonorableKillHonorGain,
        Crossfaction::CharacterCreation,
        Crossfaction::Calendar,
        Crossfaction::FriendList
    };

    auto const GetCrossfactionTypeName = [](Crossfaction const crossfactionType) -> std::string
    {
        switch (crossfactionType)
        {
            case Crossfaction::WhoCommand:
                return "Who Command";
            case Crossfaction::Mail:
                return "Mail";
            case Crossfaction::DungeonFinder:
                return "Dungeon Finder";
            case Crossfaction::Battlegrounds:
                return "Battlegrounds";
            case Crossfaction::AuctionHouse:
                return "Auction House";
            case Crossfaction::Channels:
                return "Channels";
            case Crossfaction::Group:
                return "Group";
            case Crossfaction::Item:
                return "Item";
            case Crossfaction::Trade:
                return "Trade";
            case Crossfaction::Chat:
                return "Chat";
            case Crossfaction::Guild:
                return "Guild";
            case Crossfaction::ArenaTeam:
                return "Arena Team";
            case Crossfaction::HonorableKillHonorGain:
                return "Honorable Kill Honor Gain";
            case Crossfaction::CharacterCreation:
                return "Character Creation";
            case Crossfaction::Calendar:
                return "Calendar";
            case Crossfaction::FriendList:
                return "Friend List";
            default:
                return "";
        }
    };

    std::string message = "Crossfaction Config:\n";
    for (Crossfaction const crossfactionType : CrossfactionTypes)
        message += fmt::format(">>>> {} : {}\n", GetCrossfactionTypeName(crossfactionType), IsCrossfactionEnabled(crossfactionType) ? "Enabled" : "Disabled");

    return message;
}

void World::LoadGlobalPlayerDataStore()
{
    uint32 oldMSTime = getMSTime();

    _globalPlayerDataStore.clear();
    QueryResult result = CharacterDatabase.Query("SELECT guid, account, name, gender, race, class, level, extra_flags FROM characters WHERE deleteDate IS NULL");
    if (!result)
    {
        sLog->outString();
        sLog->outErrorDb(">>  Loaded 0 Players data!");
        return;
    }

    uint32 count = 0;

    // query to load number of mails by receiver
    std::map<uint32, uint16> _mailCountMap;
    QueryResult mailCountResult = CharacterDatabase.Query("SELECT receiver, COUNT(receiver) FROM mail GROUP BY receiver");
    if (mailCountResult)
    {
        do
        {
            Field* fields                        = mailCountResult->Fetch();
            _mailCountMap[fields[0].GetUInt32()] = uint16(fields[1].GetUInt64());
        } while (mailCountResult->NextRow());
    }

    do
    {
        Field* fields    = result->Fetch();
        uint32 guidLow   = fields[0].GetUInt32();
        uint32 accountId = fields[1].GetUInt32();

        // count mails
        uint16 mailCount                             = 0;
        std::map<uint32, uint16>::const_iterator itr = _mailCountMap.find(guidLow);
        if (itr != _mailCountMap.end())
            mailCount = itr->second;

        uint32 const extraFlags = fields[7].GetUInt32();
        AddGlobalPlayerData(
            guidLow,               /*guid*/
            accountId,             /*accountId*/
            fields[2].GetString(), /*name*/
            fields[3].GetUInt8(),  /*gender*/
            fields[4].GetUInt8(),  /*race*/
            fields[5].GetUInt8(),  /*class*/
            fields[6].GetUInt8(),  /*level*/
            mailCount,             /*mail count*/
            0,                     /*guild id*/
            /*AccountMgr::GetName(accountId)*/ "" /*account name*/,
            extraFlags & PLAYER_EXTRA_HARDCORE,
            extraFlags & PLAYER_EXTRA_PATH_OF_WAR);

        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %d Players data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void World::AddGlobalPlayerData(uint32 guid, uint32 accountId, std::string const& name, uint8 gender, uint8 race, uint8 playerClass, uint8 level, uint16 mailCount, uint32 guildId, std::string const& accName, bool isHardcore, bool isPathOfWar)
{
    GlobalPlayerData data;

    data.guidLow        = guid;
    data.accountId      = accountId;
    data.name           = name;
    data.accountName    = accName;
    data.level          = level;
    data.race           = race;
    data.playerClass    = playerClass;
    data.gender         = gender;
    data.mailCount      = mailCount;
    data.guildId        = guildId;
    data.groupId        = 0;
    data.arenaTeamId[0] = 0;
    data.arenaTeamId[1] = 0;
    data.arenaTeamId[2] = 0;
    data.IsHardcore     = isHardcore;
    data.IsPathOfWar    = isPathOfWar;

    _globalPlayerDataStore[guid] = data;
    _globalPlayerNameStore[name] = guid;
}

void World::UpdateGlobalPlayerData(uint32 guid, uint8 mask, std::string const& name, uint8 level, uint8 gender, uint8 race, uint8 playerClass)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    if (mask & PLAYER_UPDATE_DATA_LEVEL)
        itr->second.level = level;
    if (mask & PLAYER_UPDATE_DATA_RACE)
        itr->second.race = race;
    if (mask & PLAYER_UPDATE_DATA_CLASS)
        itr->second.playerClass = playerClass;
    if (mask & PLAYER_UPDATE_DATA_GENDER)
        itr->second.gender = gender;
    if (mask & PLAYER_UPDATE_DATA_NAME)
        itr->second.name = name;

    if ((mask & ~PLAYER_UPDATE_DATA_LEVEL) > 0)
        InvalidatePlayerGlobalData(guid);
}

uint32 World::GetGlobalDataAccountId(uint32 guid)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return 0;

    return itr->second.accountId;
}

void World::UpdateGlobalPlayerDataHardcore(uint32 guid, bool isHardcore)
{
    auto const itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    itr->second.IsHardcore = isHardcore;
}

void World::UpdateGlobalPlayerDataPathOfWar(uint32 guid, bool isPathOfWar)
{
    auto const itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    itr->second.IsPathOfWar = isPathOfWar;
}

void World::UpdateGlobalPlayerAccountId(uint32 guid, uint32 account)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    itr->second.accountId = account;

    InvalidatePlayerGlobalData(guid);
}

void World::UpdateGlobalPlayerMails(uint32 guid, int16 count, bool add)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    if (!add)
    {
        itr->second.mailCount = count;
        return;
    }

    int16 icount = (int16)itr->second.mailCount;
    if (count < 0 && abs(count) > icount)
        count = -icount;
    itr->second.mailCount = uint16(icount + count); // addition or subtraction
}

void World::UpdateGlobalPlayerGuild(uint32 guid, uint32 guildId)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    itr->second.guildId = guildId;
}
void World::UpdateGlobalPlayerGroup(uint32 guid, uint32 groupId)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    itr->second.groupId = groupId;
}

void World::UpdateGlobalPlayerArenaTeam(uint32 guid, uint8 slot, uint32 arenaTeamId)
{
    GlobalPlayerDataMap::iterator itr = _globalPlayerDataStore.find(guid);
    if (itr == _globalPlayerDataStore.end())
        return;

    itr->second.arenaTeamId[slot] = arenaTeamId;
}

void World::UpdateGlobalNameData(uint32 guidLow, std::string const& oldName, std::string const& newName)
{
    _globalPlayerNameStore.erase(oldName);
    _globalPlayerNameStore[newName] = guidLow;
}

void World::DeleteGlobalPlayerData(uint32 guid, std::string const& name)
{
    if (guid)
        _globalPlayerDataStore.erase(guid);
    if (!name.empty())
        _globalPlayerNameStore.erase(name);
}

GlobalPlayerData const* World::GetGlobalPlayerData(uint32 guid) const
{
    GlobalPlayerDataMap::const_iterator itr = _globalPlayerDataStore.find(guid);
    if (itr != _globalPlayerDataStore.end())
        return &itr->second;
    return nullptr;
}

uint32 World::GetGlobalPlayerGUID(std::string const& name) const
{
    GlobalPlayerNameMap::const_iterator itr = _globalPlayerNameStore.find(name);
    if (itr != _globalPlayerNameStore.end())
        return itr->second;
    return 0;
}

std::string World::GetGlobalPlayerName(uint32 guid) const
{
    GlobalPlayerDataMap::const_iterator itr = _globalPlayerDataStore.find(guid);
    if (itr != _globalPlayerDataStore.end())
        return itr->second.name;

    return "";
}

void World::setGmWebCommandWhisper(std::string nickname, bool on)
{
    if (!on)
    {
        m_gamemastersWebCommandWhisper.erase(nickname);
        return;
    }

    m_gamemastersWebCommandWhisper[nickname] = time(nullptr) + 5 * MINUTE;
}

bool World::isGmWebCommandWhisperEnabled(std::string nickname) const
{
    if (m_gamemastersWebCommandWhisper.find(nickname) != m_gamemastersWebCommandWhisper.end())
        return (time(nullptr) < m_gamemastersWebCommandWhisper.at(nickname));
    return false;
}

void World::LoadAccountAndIpHistory()
{
    std::unique_lock lock(m_playerHistoryMutex);
    sLog->outString(">> Loading account and ip history...");

    /*
    auto result = LoginDatabase.Query("SELECT AccountId, IpAddress, LoginDate FROM account_history");
    if (!result)
        return;

    do
    {
        Field* field                               = result->Fetch();
        uint32 accountId                           = field[0].GetUInt32();
        std::string ipAddress                      = field[1].GetString();
        uint32 date                                = field[2].GetUInt32();
        _accountHistoryStore[accountId][ipAddress] = date;
        _ipHistoryStore[ipAddress][accountId]      = date;
    } while (result->NextRow());
    */
}

void World::AddAccountHistory(uint32 accountId, std::string const& ipAddress, time_t date)
{
    {
        std::unique_lock lock(m_playerHistoryMutex);
        _accountHistoryStore[accountId][ipAddress] = date;
        _ipHistoryStore[ipAddress][accountId]      = date;
    }

    /*
    auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACCOUNT_HISTORY);
    stmt->setUInt32(0, accountId);
    stmt->setString(1, ipAddress);
    stmt->setUInt32(2, date);

    LoginDatabase.Execute(stmt);
    */
}

bool World::IsPathfindingEnabled(const Map* map, bool force)
{
    if (!map) return false;
    if (map->GetId() == 616 /*EoE*/ || map->GetId() == 649 /*ToC25*/ || map->GetId() == 650 /*ToC5*/) return false;
    return (sWorld->getBoolConfig(CONFIG_ENABLE_MMAPS) ? true : map->IsBattlegroundOrArena());
}

void World::RestoreMythicKeyFor(uint64 playerGUID)
{
    GlobalPlayerData const* data = GetGlobalPlayerData(GUID_LOPART(playerGUID));
    if (!data)
        return;

    // from console show not existed sender
    MailSender sender(MAIL_NORMAL, data->guidLow, MAIL_STATIONERY_GM);

    // fill mail
    MailDraft draft("Mythic companion", "This is your lost key.");

    auto trans = CharacterDatabase.BeginTransaction();

    if (Item* item = Item::CreateItem(Mythic::ITEM_MYTHIC_KEY_ENTRY, 1, nullptr))
    {
        item->SetUInt64Value(ITEM_FIELD_OWNER, playerGUID);
        item->SetUInt64Value(ITEM_FIELD_CONTAINED, playerGUID);
        item->SetBinding(true);
        item->SaveToDB(trans); // save for prevent lost at next mail load, if send fail then item will deleted
        draft.AddItem(item);
    }

    draft.SendMailTo(trans, MailReceiver(nullptr, GUID_LOPART(playerGUID)), sender, MAIL_CHECK_MASK_RETURNED);
    CharacterDatabase.CommitTransaction(trans);
}

void World::LoadCharacterScreenScript()
{
    sLog->outString(">> Loading character screen script ...");

    QueryResult res = WorldDatabase.PQuery("SELECT Script FROM character_screen_script");
    if (!res)
        return;

    std::vector<std::string> _temp;
    std::string fullScript = res->Fetch()->GetString();
    if (fullScript.empty())
        return;

    uint32 splitCount = 188;
    uint32 splitNum   = fullScript.length() / splitCount;
    for (uint32 i = 0; i < splitNum; ++i)
        _temp.push_back(fullScript.substr(static_cast<size_t>(i) * splitCount, static_cast<size_t>(splitCount)));

    if (fullScript.length() % splitCount != 0)
        _temp.push_back(fullScript.substr(static_cast<size_t>(splitCount) * splitNum));

    std::lock_guard<std::mutex> lock(m_characterScriptMutex);

    m_characterScreenScript.clear();

    m_characterScreenScript.push_back("_G['sunnews']=''");

    for (auto const& ref : _temp)
        m_characterScreenScript.push_back(fmt::format(S_WARDEN_GET_GLOBAL_NEWS, ref));

    m_characterScreenScript.push_back("local f=CreateFrame(\'Frame\',\'sunf\', CharacterSelectCharacterFrame)f:SetSize(375, 420)f:SetPoint(\'TOPLEFT\',\'GlueParent\',20,-150)");
    m_characterScreenScript.push_back("local sf=CreateFrame(\'ScrollFrame\',\'sunsf\',sunf,\'GlueScrollFrameTemplate\')sf:SetSize(310,350)sf:SetPoint(\'TOPLEFT\', 20,-46)");
    m_characterScreenScript.push_back("getglobal(\'sunf\'):SetBackdrop({bgFile=\'Interface\\\\Glues\\\\Common\\\\Glue-Tooltip-Background\',edgeFile=\'Interface\\\\DialogFrame\\\\UI-DialogBox-Border\',tile=1,tileSize=32,edgeSize=32,insets={left=11,right=12,top=12,bottom=11}})");
    m_characterScreenScript.push_back("local html=CreateFrame(\'SimpleHTML\', \'sunh\', sunsf) html:SetPoint(\'CENTER\')html:SetSize(310,350)html:SetFontObject(\'GlueFontHighlightSmall\')html:SetHyperlinksEnabled(true)html:SetHyperlinkFormat(\"|cff0099ff|H%s|h[%s]|h|r\")");
    m_characterScreenScript.push_back("getglobal(\'sunh\'):SetScript(\"OnHyperlinkClick\", function(self, link, text, button) LaunchURL(link) end) getglobal('sunsf'):SetScrollChild(getglobal('sunh')) getglobal(\"sunh\"):SetText(getglobal('sunnews'))");
}

void World::AddWardenReport(WardenReport const&& report)
{
    std::time_t cTime = std::chrono::system_clock::to_time_t(report.ReportTime);
    Player* player    = ObjectAccessor::FindPlayerInOrOutOfWorld(report.Guid);
    if (!player || !player->GetSession())
        return;

    std::unique_lock<std::mutex> lock(m_wardenReportMutex);
    WardenFailureReport rep(std::move(report.Message), player->GetName(), player->GetGUIDLow(), player->GetSession()->GetAccountId(), cTime);
    m_wardenReports.emplace_back(rep);
}

std::vector<uint32> const World::GetArenaWinstreakForTeam(uint32 teamId)
{
    std::unique_lock<std::mutex> lock(m_arenaStreakStoreMutex);
    auto it = m_arenaWinStreakStore.find(teamId);
    if (it == m_arenaWinStreakStore.end())
        return {};

    return it->second.RecentTeams;
}

void World::ClearArenastreakForTeam(uint32 winnerteam)
{
    std::unique_lock<std::mutex> lock(m_arenaStreakStoreMutex);
    m_arenaWinStreakStore[winnerteam].RecentTeams.clear();
}

void World::AddArenastreakForTeam(uint32 loserteam, uint32 winnerteam)
{
    std::unique_lock<std::mutex> lock(m_arenaStreakStoreMutex);
    m_arenaWinStreakStore[winnerteam].Add(loserteam);
}

void World::AddToArenaActivity(uint64 guid)
{
    if (!getIntConfig(CONFIG_ARENA_NEW_TEAM_JOIN_DELAY))
        return;

    std::unique_lock<std::mutex> m_arenaActivityMutex;
    m_arenaActivityStore[guid] = time(nullptr);
}

bool World::HasRecentlyLeftArenaTeam(uint64 guid)
{
    uint32 delay = getIntConfig(CONFIG_ARENA_NEW_TEAM_JOIN_DELAY);
    if (!delay)
        return false;

    std::unique_lock<std::mutex> m_arenaActivityMutex;
    auto it = m_arenaActivityStore.find(guid);
    if (it == m_arenaActivityStore.end())
        return false;

    time_t now = time(nullptr);
    if ((now - it->second) < delay)
        return true;

    return false;
}

void World::UpdateActivePlayersCount()
{
    QueryResult result = LoginDatabase.PQuery("SELECT COUNT(*) FROM account WHERE TIME_TO_SEC(TIMEDIFF(NOW(), last_login)) <= 86400");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        uint32 activePlayers       = uint32(fields[0].GetUInt64());
        _activePlayersCount        = activePlayers;
    } while (result->NextRow());
}

void World::UpdateGlobalPlayerAccountIDToGuid(uint32 accountID, uint64 guid)
{
    if (guid == 0)
    {
        _globalPlayerAccountIDToGuidStore.erase(accountID);
        return;
    }

    _globalPlayerAccountIDToGuidStore[accountID] = guid;
}

Player* World::GetPlayerByAccountID(uint32 accountID)
{
    auto const it = _globalPlayerAccountIDToGuidStore.find(accountID);
    if (it == _globalPlayerAccountIDToGuidStore.end())
        return nullptr;

    uint64 const guid = it->second;
    if (guid == 0)
        return nullptr;

    return ObjectAccessor::FindPlayer(guid);
}
