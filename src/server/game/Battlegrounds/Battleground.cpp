#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "Creature.h"
#include "Chat.h"
#include "Formulas.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "MapManager.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "ArenaSpectator.h"
#include "BattlegroundBE.h"
#include "BattlegroundDS.h"
#include "BattlegroundNA.h"
#include "BattlegroundRL.h"
#include "BattlegroundRV.h"
#include "Transport.h"
#include "SoloQueue.h"

#include <boost/date_time/posix_time/conversion.hpp>

namespace Trinity {
class BattlegroundChatBuilder
{
public:
    BattlegroundChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, va_list* args = nullptr)
        : _msgtype(msgtype)
        , _textId(textId)
        , _source(source)
        , _args(args)
    {
    }

    void operator()(WorldPacket& data, LocaleConstant loc_idx)
    {
        char const* text = sObjectMgr->GetTrinityString(_textId, loc_idx);
        if (_args)
        {
            // we need copy va_list before use or original va_list will corrupted
            va_list ap;
            va_copy(ap, *_args);

            char str[2048];
            vsnprintf(str, 2048, text, ap);
            va_end(ap);

            do_helper(data, &str[0]);
        }
        else
            do_helper(data, text);
    }

private:
    void do_helper(WorldPacket& data, char const* text)
    {
        ChatHandler::BuildChatPacket(data, _msgtype, LANG_UNIVERSAL, _source, _source, text);
    }

    ChatMsg _msgtype;
    int32 _textId;
    Player const* _source;
    va_list* _args;
};

class Battleground2ChatBuilder
{
public:
    Battleground2ChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, int32 arg1, int32 arg2)
        : _msgtype(msgtype)
        , _textId(textId)
        , _source(source)
        , _arg1(arg1)
        , _arg2(arg2)
    {
    }

    void operator()(WorldPacket& data, LocaleConstant loc_idx)
    {
        char const* text    = sObjectMgr->GetTrinityString(_textId, loc_idx);
        char const* arg1str = _arg1 ? sObjectMgr->GetTrinityString(_arg1, loc_idx) : "";
        char const* arg2str = _arg2 ? sObjectMgr->GetTrinityString(_arg2, loc_idx) : "";

        char str[2048];
        snprintf(str, 2048, text, arg1str, arg2str);

        ChatHandler::BuildChatPacket(data, _msgtype, LANG_UNIVERSAL, _source, _source, str);
    }

private:
    ChatMsg _msgtype;
    int32 _textId;
    Player const* _source;
    int32 _arg1;
    int32 _arg2;
};
} // namespace Trinity

template<class Do>
void Battleground::BroadcastWorker(Do& _do)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        _do(itr->second);
}

Battleground::Battleground()
{
    m_cfbgTimer.SetInterval(10000);

    m_RealTypeID              = BATTLEGROUND_TYPE_NONE;
    m_InstanceID              = 0;
    m_Status                  = STATUS_NONE;
    m_ClientInstanceID        = 0;
    m_EndTime                 = 0;
    m_LastResurrectTime       = 0;
    m_ArenaType               = 0;
    m_IsArena                 = false;
    m_WinnerId                = TEAM_NEUTRAL;
    m_StartTime               = 0;
    m_ResetStatTimer          = 0;
    m_ValidStartPositionTimer = 0;
    m_Events                  = 0;
    m_StartDelayTime          = 0;
    m_IsRated                 = false;
    m_BuffChange              = false;
    m_Name                    = "";
    m_LevelMin                = 0;
    m_LevelMax                = 0;
    m_SetDeleteThis           = false;

    m_MaxPlayersPerTeam = 0;
    m_MinPlayersPerTeam = 0;

    m_MapId        = 0;
    m_Map          = nullptr;
    m_StartMaxDist = 0.0f;
    ScriptId       = 0;

    for (uint8 i = TEAM_ALLIANCE; i <= TEAM_HORDE; ++i)
    {
        m_BgInvitedPlayers[i] = 0;
        m_TeamStartLocX[i] = 0;
        m_TeamStartLocY[i] = 0;
        m_TeamStartLocZ[i] = 0;
        m_TeamStartLocO[i] = 0;
        m_BgRaids[i] = nullptr;
        m_PlayersCount[i] = 0;
        m_TeamScores[i] = 0;
        m_ArenaTeamIds[i].clear();
        m_ArenaTeamMMR[i] = 0;
        m_ArenaTeamRating[i] = 0;
        m_ArenaTeamRatingChanges[i] = 0;
        m_ArenaTeamLastDPSList[i].clear();
    }

    m_PrematureCountDown      = false;
    m_PrematureCountDownTimer = 0;

    m_HonorMode = BG_NORMAL;

    bool debugBG                              = false;
    StartDelayTimes[BG_STARTING_EVENT_FIRST]  = debugBG ? BattlegroundStartTimeIntervals(5000) : BG_START_DELAY_2M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND] = debugBG ? BattlegroundStartTimeIntervals(5000) : BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]  = debugBG ? BattlegroundStartTimeIntervals(5000) : BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;

    //we must set to some default existing values
    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_WS_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;

    // pussywizard:
    m_UpdateTimer = 0;

    _arenaFightID = 0;
}

Battleground::~Battleground()
{
    // remove objects and creatures
    // (this is done automatically in mapmanager update, when the instance is reset after the reset time)
    uint32 size = uint32(BgCreatures.size());
    for (uint32 i = 0; i < size; ++i)
        DelCreature(i);

    size = uint32(BgObjects.size());
    for (uint32 i = 0; i < size; ++i)
        DelObject(i);

    sBattlegroundMgr->RemoveBattleground(GetBgTypeID(), GetInstanceID());
    // unload map
    if (m_Map)
    {
        m_Map->SetUnload();
        //unlink to prevent crash, always unlink all pointer reference before destruction
        m_Map->SetBG(nullptr);
        m_Map = nullptr;
    }

    for (BattlegroundScoreMap::const_iterator itr = PlayerScores.begin(); itr != PlayerScores.end(); ++itr)
        delete itr->second;
}

void Battleground::Update(uint32 diff)
{
    // pussywizard:
    m_UpdateTimer += diff;
    if (m_UpdateTimer < BATTLEGROUND_UPDATE_INTERVAL)
        return;

    diff = BATTLEGROUND_UPDATE_INTERVAL; // just change diff value, no need to replace variable name in many places
    m_UpdateTimer -= BATTLEGROUND_UPDATE_INTERVAL;

    if (!PreUpdateImpl(diff))
        return;

    if (m_cfbgTimer.Update(diff))
    {
        m_cfbgTimer.Reset();

        std::vector<Player const*> cfPlayers;
        cfPlayers.reserve(GetPlayersSize());

        for (auto& [guid, player] : GetPlayers())
        {
            if (player->IsPlayingCrossfaction())
                cfPlayers.push_back(player);
        }

        for (Player const* cfPlayer : cfPlayers)
        {
            for (auto& [guid, player] : GetPlayers())
            {
                if (player == cfPlayer)
                    continue;

                player->GetSession()->SendInvalidatePlayer(cfPlayer->GetGUID());
                player->GetSession()->SendNameQueryOpcode(cfPlayer->GetGUID());
            }
        }
    }

    if (!GetPlayersSize())
    {
        if (!GetInvitedCount(TEAM_HORDE) && !GetInvitedCount(TEAM_ALLIANCE))
            m_SetDeleteThis = true;
        return;
    }

    switch (GetStatus())
    {
        case STATUS_WAIT_JOIN:
            if (GetPlayersSize())
            {
                _ProcessJoin(diff);
                _CheckSafePositions(diff);
            }
            break;
        case STATUS_IN_PROGRESS:
            if (isArena())
            {
                if (GetStartTime() >= 46 * MINUTE * IN_MILLISECONDS) // pussywizard: 1min startup + 45min allowed duration
                {
                    UpdateArenaWorldState();
                    CheckArenaAfterTimerConditions();
                    return;
                }
            }
            else
            {
                _ProcessResurrect(diff);
                if (sBattlegroundMgr->GetPrematureFinishTime() && (GetPlayersCountByTeam(TEAM_ALLIANCE) < GetMinPlayersPerTeam() || GetPlayersCountByTeam(TEAM_HORDE) < GetMinPlayersPerTeam()))
                    _ProcessProgress(diff);
                else if (m_PrematureCountDown)
                    m_PrematureCountDown = false;
            }

            break;
        case STATUS_WAIT_LEAVE:
            _ProcessLeave(diff);
            break;
        default:
            break;
    }

    // Update start time and reset stats timer
    m_StartTime += diff;
    m_ResetStatTimer += diff;

    PostUpdateImpl(diff);
}

inline void Battleground::_CheckSafePositions(uint32 diff)
{
    float maxDist = GetStartMaxDist();
    if (!maxDist)
        return;

    m_ValidStartPositionTimer += diff;
    if (m_ValidStartPositionTimer >= CHECK_PLAYER_POSITION_INVERVAL)
    {
        m_ValidStartPositionTimer = 0;

        Position pos;
        float x, y, z, o;
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        {
            itr->second->GetPosition(&pos);
            GetTeamStartLoc(itr->second->GetBgTeam(), x, y, z, o);
            if (pos.GetExactDistSq(x, y, z) > maxDist)
            {
                ; //sLog->outDebug(LOG_FILTER_BATTLEGROUND, "BATTLEGROUND: Sending %s back to start location (map: %u) (possible exploit)", player->GetName().c_str(), GetMapId());
                itr->second->TeleportTo(GetMapId(), x, y, z, o);
            }
        }
    }
}

inline void Battleground::_ProcessResurrect(uint32 diff)
{
    // *********************************************************
    // ***        BATTLEGROUND RESURRECTION SYSTEM           ***
    // *********************************************************
    // this should be handled by spell system
    m_LastResurrectTime += diff;
    if (m_LastResurrectTime >= RESURRECTION_INTERVAL)
    {
        if (GetReviveQueueSize())
        {
            for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
            {
                Creature* sh = nullptr;
                for (std::vector<uint64>::const_iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
                {
                    Player* player = ObjectAccessor::FindPlayer(*itr2);
                    if (!player)
                        continue;

                    if (!sh && player->IsInWorld())
                    {
                        sh = player->GetMap()->GetCreature(itr->first);
                        // only for visual effect
                        if (sh)
                            // Spirit Heal, effect 117
                            sh->CastSpell(sh, SPELL_SPIRIT_HEAL, true);
                    }

                    // Resurrection visual
                    player->CastSpell(player, SPELL_RESURRECTION_VISUAL, true);
                    m_ResurrectQueue.push_back(*itr2);
                }
                (itr->second).clear();
            }

            m_ReviveQueue.clear();
            m_LastResurrectTime = 0;
        }
        else
            // queue is clear and time passed, just update last resurrection time
            m_LastResurrectTime = 0;
    }
    else if (m_LastResurrectTime > 500) // Resurrect players only half a second later, to see spirit heal effect on NPC
    {
        for (std::vector<uint64>::const_iterator itr = m_ResurrectQueue.begin(); itr != m_ResurrectQueue.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;
            player->ResurrectPlayer(1.0f);
            player->CastSpell(player, 6962, true);
            player->CastSpell(player, SPELL_SPIRIT_HEAL_MANA, true);
            sObjectAccessor->ConvertCorpseForPlayer(*itr);
        }
        m_ResurrectQueue.clear();
    }
}

TeamId Battleground::GetPrematureWinner()
{
    if (GetPlayersCountByTeam(TEAM_ALLIANCE) >= GetMinPlayersPerTeam())
        return TEAM_ALLIANCE;
    else if (GetPlayersCountByTeam(TEAM_HORDE) >= GetMinPlayersPerTeam())
        return TEAM_HORDE;

    return TEAM_NEUTRAL;
}

uint8 Battleground::GetUniqueBracketId() const
{
    return GetMinLevel() / 10;
}

inline void Battleground::_ProcessProgress(uint32 diff)
{
    // *********************************************************
    // ***           BATTLEGROUND BALLANCE SYSTEM            ***
    // *********************************************************
    // if less then minimum players are in on one side, then start premature finish timer
    if (!m_PrematureCountDown)
    {
        m_PrematureCountDown      = true;
        m_PrematureCountDownTimer = sBattlegroundMgr->GetPrematureFinishTime();
    }
    else if (m_PrematureCountDownTimer < diff)
    {
        // time's up!
        EndBattleground(GetPrematureWinner());
        m_PrematureCountDown = false;
    }
    else if (!sBattlegroundMgr->isTesting())
    {
        uint32 newtime = m_PrematureCountDownTimer - diff;
        // announce every minute
        if (newtime > (MINUTE * IN_MILLISECONDS))
        {
            if (newtime / (MINUTE * IN_MILLISECONDS) != m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS))
                PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING, CHAT_MSG_SYSTEM, nullptr, (uint32)(m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS)));
        }
        else
        {
            //announce every 15 seconds
            if (newtime / (15 * IN_MILLISECONDS) != m_PrematureCountDownTimer / (15 * IN_MILLISECONDS))
                PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING_SECS, CHAT_MSG_SYSTEM, nullptr, (uint32)(m_PrematureCountDownTimer / IN_MILLISECONDS));
        }
        m_PrematureCountDownTimer = newtime;
    }
}

inline void Battleground::_ProcessJoin(uint32 diff)
{
    // *********************************************************
    // ***           BATTLEGROUND STARTING SYSTEM            ***
    // *********************************************************
    ModifyStartDelayTime(diff);

    if (m_ResetStatTimer > 5000)
    {
        m_ResetStatTimer = 0;
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            itr->second->ResetAllPowers();
    }

    if (!(m_Events & BG_STARTING_EVENT_1))
    {
        m_Events |= BG_STARTING_EVENT_1;

        if (!FindBgMap())
        {
            sLog->outError("Battleground::_ProcessJoin: map (map id: %u, instance id: %u) is not created!", m_MapId, m_InstanceID);
            EndNow();
            return;
        }

        // Setup here, only when at least one player has ported to the map
        if (!SetupBattleground())
        {
            EndNow();
            return;
        }

        StartingEventCloseDoors();
        SetStartDelayTime(StartDelayTimes[BG_STARTING_EVENT_FIRST]);
        // First start warning - 2 or 1 minute
        SendMessageToAll(StartMessageIds[BG_STARTING_EVENT_FIRST], CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }
    // After 1 minute or 30 seconds, warning is signaled
    else if (GetStartDelayTime() <= StartDelayTimes[BG_STARTING_EVENT_SECOND] && !(m_Events & BG_STARTING_EVENT_2))
    {
        m_Events |= BG_STARTING_EVENT_2;
        SendMessageToAll(StartMessageIds[BG_STARTING_EVENT_SECOND], CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }
    // After 30 or 15 seconds, warning is signaled
    else if (GetStartDelayTime() <= StartDelayTimes[BG_STARTING_EVENT_THIRD] && !(m_Events & BG_STARTING_EVENT_3))
    {
        m_Events |= BG_STARTING_EVENT_3;
        SendMessageToAll(StartMessageIds[BG_STARTING_EVENT_THIRD], CHAT_MSG_BG_SYSTEM_NEUTRAL);

        if (isArena())
            switch (GetBgTypeID())
            {
                case BATTLEGROUND_NA:
                    DelObject(BG_NA_OBJECT_READY_MARKER_1);
                    DelObject(BG_NA_OBJECT_READY_MARKER_2);
                    break;
                case BATTLEGROUND_BE:
                    DelObject(BG_BE_OBJECT_READY_MARKER_1);
                    DelObject(BG_BE_OBJECT_READY_MARKER_2);
                    break;
                case BATTLEGROUND_RL:
                    DelObject(BG_RL_OBJECT_READY_MARKER_1);
                    DelObject(BG_RL_OBJECT_READY_MARKER_2);
                    break;
                case BATTLEGROUND_DS:
                    DelObject(BG_DS_OBJECT_READY_MARKER_1);
                    DelObject(BG_DS_OBJECT_READY_MARKER_2);
                    break;
                case BATTLEGROUND_RV:
                    DelObject(BG_RV_OBJECT_READY_MARKER_1);
                    DelObject(BG_RV_OBJECT_READY_MARKER_2);
                    break;
                default:
                    break;
            }
    }
    // Delay expired (after 2 or 1 minute)
    else if (GetStartDelayTime() <= 0 && !(m_Events & BG_STARTING_EVENT_4))
    {
        m_Events |= BG_STARTING_EVENT_4;

        StartingEventOpenDoors();

        SendWarningToAll(StartMessageIds[BG_STARTING_EVENT_FOURTH]);
        SetStatus(STATUS_IN_PROGRESS);
        SetStartDelayTime(StartDelayTimes[BG_STARTING_EVENT_FOURTH]);

        // Remove preparation
        if (isArena())
        {
            // pussywizard: initial visibility range is 30yd, set it to a proper value now:
            if (BattlegroundMap* map = GetBgMap())
                map->UpdateVisibilityRange();

            for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                if (Player* player = itr->second)
                {
                    WorldPacket status;
                    sBattlegroundMgr->BuildBattlegroundStatusPacket(&status, this, player->GetCurrentBattlegroundQueueSlot(), STATUS_IN_PROGRESS, 0, GetStartTime(), GetArenaType(), player->GetBgTeam());
                    player->GetSession()->SendPacket(&status);

                    player->RemoveAurasDueToSpell(SPELL_ARENA_PREPARATION);
                    player->ResetAllPowers();
                    // remove auras with duration lower than 30s
                    Unit::AuraApplicationMap& auraMap = player->GetAppliedAuras();
                    for (Unit::AuraApplicationMap::iterator iter = auraMap.begin(); iter != auraMap.end();)
                    {
                        AuraApplication* aurApp = iter->second;
                        Aura* aura              = aurApp->GetBase();
                        if (!aura->IsPermanent()
                            && aura->GetDuration() <= 30 * IN_MILLISECONDS
                            && aurApp->IsPositive()
                            // && (!aura->GetSpellInfo()->HasAttribute(SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)) Xinef: bullshit condition, ALL buffs should be removed
                            && (!aura->HasEffectType(SPELL_AURA_MOD_INVISIBILITY)))
                            player->RemoveAura(iter);
                        else
                            ++iter;
                    }

                    player->UpdateObjectVisibility(true);
                }

            for (SpectatorList::const_iterator itr = m_Spectators.begin(); itr != m_Spectators.end(); ++itr)
                ArenaSpectator::HandleResetCommand(*itr);

            CheckArenaWinConditions();

            // pussywizard: arena spectator stuff
            if (GetStatus() == STATUS_IN_PROGRESS)
            {
                if (GetArenaType() == ARENA_TYPE_SOLO_QUEUE)
                {
                    if (GetPlayersCountByTeam(TEAM_ALLIANCE) + GetPlayersCountByTeam(TEAM_HORDE) < 6)
                        EndBattleground(TEAM_NEUTRAL);
                    else
                    {
                        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                                if (ArenaTeam* soloQueueArenaTeam = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE))))
                                    soloQueueArenaTeam->SetLastDPSList(GetArenaTeamLastDPSList(itr->second->GetBgTeam()));
                    }
                }

                for (ToBeTeleportedMap::const_iterator itr = m_ToBeTeleported.begin(); itr != m_ToBeTeleported.end(); ++itr)
                    if (Player* p     = ObjectAccessor::GetObjectInOrOutOfWorld(itr->first, (Player*)nullptr))
                        if (Player* t = ObjectAccessor::FindPlayer(itr->second))
                        {
                            if (!t->FindMap() || t->FindMap() != GetBgMap())
                                continue;

                            p->SetSummonPoint(t->GetMapId(), t->GetPositionX(), t->GetPositionY(), t->GetPositionZ(), 15, true);

                            WorldPacket data(SMSG_SUMMON_REQUEST, 8 + 4 + 4);
                            data << uint64(t->GetGUID());
                            data << uint32(t->GetZoneId());
                            data << uint32(15 * IN_MILLISECONDS);
                            p->GetSession()->SendPacket(&data);
                        }
                m_ToBeTeleported.clear();
            }
        }
        else
        {
            PlaySoundToAll(SOUND_BG_START);

            for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            {
                itr->second->RemoveAurasDueToSpell(SPELL_PREPARATION);
                itr->second->ResetAllPowers();
            }

            // Announce BG starting
            if (sWorld->getBoolConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE))
            {
                if (GetMinLevel() == 80)
                    sWorld->SendPvPWorldText(true, LANG_BG_STARTED_80_ANNOUNCE_WORLD, GetName());
                else
                    sWorld->SendPvPWorldText(true, LANG_BG_STARTED_ANNOUNCE_WORLD, GetName(), std::min(GetMinLevel(), (uint32)80), std::min(GetMaxLevel(), (uint32)80));
            }
            _CheckSafePositions(diff);
        }
    }
}

inline void Battleground::_ProcessLeave(uint32 diff)
{
    // *********************************************************
    // ***           BATTLEGROUND ENDING SYSTEM              ***
    // *********************************************************
    // remove all players from battleground after 2 minutes
    m_EndTime -= diff;
    if (m_EndTime <= 0)
    {
        m_EndTime = TIME_TO_AUTOREMOVE; // pussywizard: 0 -> TIME_TO_AUTOREMOVE
        BattlegroundPlayerMap::iterator itr, next;
        for (itr = m_Players.begin(); itr != m_Players.end(); itr = next)
        {
            next = itr;
            ++next;
            itr->second->LeaveBattleground(this); //itr is erased here!
        }
    }
}

void Battleground::SetTeamStartLoc(TeamId teamId, float X, float Y, float Z, float O)
{
    m_TeamStartLocX[teamId] = X;
    m_TeamStartLocY[teamId] = Y;
    m_TeamStartLocZ[teamId] = Z;
    m_TeamStartLocO[teamId] = O;
}

void Battleground::SendPacketToAll(WorldPacket* packet)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        itr->second->GetSession()->SendPacket(packet);
}

void Battleground::SendPacketToTeam(TeamId teamId, WorldPacket* packet, Player* sender, bool self)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (itr->second->GetBgTeam() == teamId && (self || sender != itr->second))
            itr->second->GetSession()->SendPacket(packet);
}

void Battleground::PlaySoundToAll(uint32 soundID)
{
    WorldPacket data;
    sBattlegroundMgr->BuildPlaySoundPacket(&data, soundID);
    SendPacketToAll(&data);
}

void Battleground::CastSpellOnTeam(uint32 spellId, TeamId teamId)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (itr->second->GetBgTeam() == teamId)
            itr->second->CastSpell(itr->second, spellId, true);
}

void Battleground::RemoveAuraOnTeam(uint32 spellId, TeamId teamId)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (itr->second->GetBgTeam() == teamId)
            itr->second->RemoveAura(spellId);
}

void Battleground::YellToAll(Creature* creature, char const* text, uint32 language)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_YELL, itr->second->ConvertLanguage(Language(language)), creature, itr->second, text);
        itr->second->SendDirectMessage(&data);
    }
}

void Battleground::RewardHonorToTeam(uint32 honor, TeamId teamId)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (itr->second->GetBgTeam() == teamId)
            UpdatePlayerScore(itr->second, SCORE_BONUS_HONOR, honor);
}

void Battleground::RewardReputationToTeam(uint32 factionId, uint32 reputation, TeamId teamId)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* player = itr->second;
        if (player->GetBgTeam() == teamId)
        {
            auto realFactionId = player->ConvertFactionForReputationReward(factionId);
            uint32 repGain     = reputation;
            AddPct(repGain, itr->second->GetTotalAuraModifier(SPELL_AURA_MOD_REPUTATION_GAIN));
            AddPct(repGain, itr->second->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_FACTION_REPUTATION_GAIN, realFactionId));
            player->GetReputationMgr().ModifyReputation(realFactionId, repGain);
        }
    }
}

void Battleground::UpdateWorldState(uint32 Field, uint32 Value)
{
    WorldPacket data;
    sBattlegroundMgr->BuildUpdateWorldStatePacket(&data, Field, Value);
    SendPacketToAll(&data);
}

void Battleground::UpdateWorldStateForPlayer(uint32 Field, uint32 Value, Player* player)
{
    WorldPacket data;
    sBattlegroundMgr->BuildUpdateWorldStatePacket(&data, Field, Value);
    player->GetSession()->SendPacket(&data);
}

void Battleground::EndBattleground(TeamId winnerTeamId)
{
    // xinef: if this is true, it means that endbattleground is called second time
    // skip to avoid double rating reduce / add
    // can bug out due to multithreading ?
    // set as fast as possible
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    SetStatus(STATUS_WAIT_LEAVE);

    std::unordered_map<uint64, uint32> winnerTeamdIds;
    std::unordered_map<uint64, uint32> loserTeamdIds;
    uint32 loserMatchmakerRating  = 0;
    int32 loserMatchmakerChange   = 0;
    uint32 winnerMatchmakerRating = 0;
    int32 winnerMatchmakerChange  = 0;

    int32 winmsg_id = 0;

    if (winnerTeamId == TEAM_ALLIANCE)
    {
        SetWinner(TEAM_HORDE); // reversed in packet
        winmsg_id = isBattleground() ? LANG_BG_A_WINS : LANG_ARENA_GOLD_WINS;
        PlaySoundToAll(SOUND_ALLIANCE_WINS); // alliance wins sound
    }
    else if (winnerTeamId == TEAM_HORDE)
    {
        SetWinner(TEAM_ALLIANCE); // reversed in packet
        winmsg_id = isBattleground() ? LANG_BG_H_WINS : LANG_ARENA_GREEN_WINS;
        PlaySoundToAll(SOUND_HORDE_WINS); // horde wins sound
    }
    else
        SetWinner(TEAM_NEUTRAL);

    bool soloQueueEndedPrematerely = false;
    if (winnerTeamId == TEAM_NEUTRAL)
        soloQueueEndedPrematerely = true;

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PVPSTATS_MAXID);
    uint64 battlegroundId   = 1;
    if (isBattleground() && sWorld->getBoolConfig(CONFIG_BATTLEGROUND_STORE_STATISTICS_ENABLE))
    {
        stmt                       = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PVPSTATS_MAXID);
        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        if (result)
        {
            Field* fields  = result->Fetch();
            battlegroundId = fields[0].GetUInt64() + 1;
        }

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVPSTATS_BATTLEGROUND);
        stmt->setUInt64(0, battlegroundId);
        stmt->setUInt8(1, GetWinner());
        stmt->setUInt8(2, GetUniqueBracketId());
        stmt->setUInt8(3, GetBgTypeID());
        CharacterDatabase.Execute(stmt);
    }
    //we must set it this way, because end time is sent in packet!
    m_EndTime = TIME_TO_AUTOREMOVE;

    uint32 startDelay = StartDelayTimes[BG_STARTING_EVENT_FIRST]; // = BG_START_DELAY_1M = 60000 for all arenas

    // arena rating calculation
    if (isArena() && isRated())
    {
        auto arenaRes = ValidateRatedArena(winnerTeamId);
        //sWorld->AddArenastreakForTeam(loserArenaTeam->GetId(), winnerArenaTeam->GetId());
        //sWorld->ClearArenastreakForTeam(loserArenaTeam->GetId());

        if (winnerTeamId != TEAM_NEUTRAL && arenaRes == ValidArenaResult::RESULT_OK)
        {
            winnerTeamdIds = GetArenaTeamIdsForTeam(winnerTeamId);
            loserTeamdIds  = GetArenaTeamIdsForTeam(GetOtherTeamId(winnerTeamId));

            winnerMatchmakerRating = GetArenaTeamMatchmakerRating(winnerTeamId);
            loserMatchmakerRating  = GetArenaTeamMatchmakerRating(GetOtherTeamId(winnerTeamId));

            winnerMatchmakerChange = ArenaTeam::GetMatchmakerRatingMod(winnerMatchmakerRating, loserMatchmakerRating, true);
            loserMatchmakerChange  = ArenaTeam::GetMatchmakerRatingMod(loserMatchmakerRating, winnerMatchmakerRating, false);

            SetArenaTeamMatchmakerRating(winnerTeamId, winnerMatchmakerRating + winnerMatchmakerChange);
            SetArenaTeamMatchmakerRating(GetOtherTeamId(winnerTeamId), loserMatchmakerRating + loserMatchmakerChange);

            SetArenaTeamRatingChangeForTeam(winnerTeamId, 0);
            SetArenaTeamRatingChangeForTeam(GetOtherTeamId(winnerTeamId), 0);

            // pussywizard: arena logs in database
            if (!_arenaFightID)
                _arenaFightID = sArenaTeamMgr->GetNextArenaLogId();
            uint32 currOnline = (uint32)(sWorld->GetActiveSessionCount());

            auto trans = CharacterDatabase.BeginTransaction();
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARENA_LOG_FIGHT);
            stmt->setUInt32(0, _arenaFightID);
            stmt->setUInt8(1, m_ArenaType);
            stmt->setUInt32(2, ((GetStartTime() <= startDelay ? 0 : GetStartTime() - startDelay) / 1000));
            stmt->setUInt16(3, (uint16)winnerMatchmakerRating);
            stmt->setUInt16(4, (uint16)loserMatchmakerRating);
            stmt->setUInt32(5, currOnline);
            trans->Append(stmt);

            uint8 memberId = 0;
            for (Battleground::ArenaLogEntryDataMap::const_iterator itr = ArenaLogEntries.begin(); itr != ArenaLogEntries.end(); ++itr)
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARENA_LOG_MEMBERSTATS);
                stmt->setUInt32(0, _arenaFightID);
                stmt->setUInt8(1, ++memberId);
                stmt->setString(2, itr->second.Name);
                stmt->setUInt32(3, itr->second.Guid);
                stmt->setUInt32(4, itr->second.Acc);
                stmt->setString(5, itr->second.IP);
                stmt->setUInt32(6, itr->second.DamageDone);
                stmt->setUInt32(7, itr->second.HealingDone);
                stmt->setUInt32(8, itr->second.KillingBlows);
                trans->Append(stmt);

                // AntiWintrading system - notifies GMs when player didn't do enough damage/healing
                if (sWorld->getBoolConfig(CONFIG_ANTIWINTRADING_ENABLE) && itr->second.DamageDone < sWorld->getIntConfig(CONFIG_ANTIWINTRADING_MINDMG) && itr->second.HealingDone < sWorld->getIntConfig(CONFIG_ANTIWINTRADING_MINHEALING))
                    sWorld->SendGMText(LANG_POSSIBLE_WINTRADER, itr->second.Name.c_str(), itr->second.Guid, itr->second.DamageDone, itr->second.HealingDone);
            }

            CharacterDatabase.CommitTransaction(trans);
        }
        else if (soloQueueEndedPrematerely)
        {
            SetArenaTeamRatingChangeForTeam(winnerTeamId, 0);
            SetArenaTeamRatingChangeForTeam(GetOtherTeamId(winnerTeamId), 0);
        }
        // Deduct 16 points from each teams arena-rating if there are no winners after 45+2 minutes
        else
        {
            // pussywizard: in case of a draw, the following is always true:
            // winnerArenaTeam => TEAM_HORDE, loserArenaTeam => TEAM_ALLIANCE

            int32 adjustedChange = ARENA_TIMELIMIT_POINTS_LOSS;
            if (arenaRes != ValidArenaResult::RESULT_OVERTIME)
                adjustedChange = 0;

            winnerMatchmakerRating = GetArenaTeamMatchmakerRating(TEAM_ALLIANCE);
            loserMatchmakerRating  = GetArenaTeamMatchmakerRating(TEAM_HORDE);
            winnerMatchmakerChange = 0;
            loserMatchmakerChange  = 0;

            SetArenaTeamRatingChangeForTeam(TEAM_ALLIANCE, adjustedChange);
            SetArenaTeamRatingChangeForTeam(TEAM_HORDE, adjustedChange);

            for (uint8 i = TEAM_ALLIANCE; i <= TEAM_HORDE; ++i)
                for (auto const& itr : m_ArenaTeamIds[i])
                    if (ArenaTeam* arenaTeam = sArenaTeamMgr->GetArenaTeamById(itr.second))
                        arenaTeam->FinishGame(adjustedChange, GetBgMap());

            // pussywizard: arena logs in database
            if (!_arenaFightID)
                _arenaFightID = sArenaTeamMgr->GetNextArenaLogId();
            uint32 currOnline = (uint32)(sWorld->GetActiveSessionCount());

            auto trans    = CharacterDatabase.BeginTransaction();
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARENA_LOG_FIGHT);
            stmt->setUInt32(0, _arenaFightID);
            stmt->setUInt8(1, m_ArenaType);
            stmt->setUInt32(2, ((GetStartTime() <= startDelay ? 0 : GetStartTime() - startDelay) / 1000));
            stmt->setUInt16(3, (uint16)winnerMatchmakerRating);
            stmt->setUInt16(4, (uint16)loserMatchmakerRating);
            stmt->setUInt32(5, currOnline);
            trans->Append(stmt);

            uint8 memberId = 0;
            for (Battleground::ArenaLogEntryDataMap::const_iterator itr = ArenaLogEntries.begin(); itr != ArenaLogEntries.end(); ++itr)
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARENA_LOG_MEMBERSTATS);
                stmt->setUInt32(0, _arenaFightID);
                stmt->setUInt8(1, ++memberId);
                stmt->setString(2, itr->second.Name);
                stmt->setUInt32(3, itr->second.Guid);
                stmt->setUInt32(4, itr->second.Acc);
                stmt->setString(5, itr->second.IP);
                stmt->setUInt32(6, itr->second.DamageDone);
                stmt->setUInt32(7, itr->second.HealingDone);
                stmt->setUInt32(8, itr->second.KillingBlows);
                trans->Append(stmt);
            }

            CharacterDatabase.CommitTransaction(trans);
        }

        uint8 aliveWinners = GetAlivePlayersCountByTeam(winnerTeamId);
        for (auto const& i : GetPlayers())
        {
            Player* player = i.second;
            if (!player)
                continue;

            TeamId teamId = player->GetBgTeam();

            // Last standing - Rated 5v5 arena & be solely alive player
            if (teamId == winnerTeamId && GetArenaType() == ARENA_TYPE_SOLO_QUEUE && aliveWinners == 1 && player->IsAlive())
                player->CastSpell(player, SPELL_THE_LAST_STANDING, true);
        }
    }

    for (BattlegroundPlayerMap::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* player  = itr->second;
        TeamId bgTeamId = player->GetBgTeam();

        // should remove spirit of redemption
        if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

        if (!player->IsAlive())
        {
            player->ResurrectPlayer(1.0f);
            player->SpawnCorpseBones();
        }
        else
        {
            //needed cause else in av some creatures will kill the players at the end
            player->CombatStop();
            player->getHostileRefManager().deleteReferences();
        }

        if (sCustomMgr->GetBoolConfig(CONFIG_RESET_COOLDOWN_ON_ARENA_END))
            player->RemoveArenaSpellCooldowns(true);

        // per player calculation
        if (isArena() && isRated() && winnerTeamId != TEAM_NEUTRAL)
        {
            if (bgTeamId == winnerTeamId)
            {
                // update achievement BEFORE personal rating update
                uint32 rating = player->GetArenaPersonalRating(ArenaTeam::GetSlotByType(GetArenaType()));
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, rating ? rating : 1);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA, GetMapId());
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA, 618 /* Ring of Valor */);

               auto iter = winnerTeamdIds.find(player->GetGUID());
                if (iter != winnerTeamdIds.end())
                    if (ArenaTeam* winnerArenaTeam = sArenaTeamMgr->GetArenaTeamById(iter->second))
                    {
                        int32 winnerChange = winnerArenaTeam->MemberWon(player, GetArenaTeamRating(bgTeamId), loserMatchmakerRating, winnerMatchmakerChange);
                        SetArenaTeamRatingChangeForTeam(bgTeamId, winnerChange);
                    }

                // Daily arena pvp
                if (player->GetQuestStatus(40000) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(350000, 0);
            }
            else
            {
                auto iter = loserTeamdIds.find(player->GetGUID());
                if (iter != loserTeamdIds.end())
                    if (ArenaTeam* loserArenaTeam = sArenaTeamMgr->GetArenaTeamById(iter->second))
                    {
                        int32 loserChange = loserArenaTeam->MemberLost(player, GetArenaTeamRating(bgTeamId), winnerMatchmakerRating, loserMatchmakerChange);
                        SetArenaTeamRatingChangeForTeam(bgTeamId, loserChange);
                    }

                // Arena lost => reset the win_rated_arena having the "no_lose" condition
                player->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_CONDITION_NO_LOSE, 0);
            }
        }

        uint32 winner_kills = player->GetRandomWinner() ? BG_REWARD_WINNER_HONOR_LAST : BG_REWARD_WINNER_HONOR_FIRST;
        uint32 loser_kills  = player->GetRandomWinner() ? BG_REWARD_LOSER_HONOR_LAST : BG_REWARD_LOSER_HONOR_FIRST;
        uint32 winner_arena = player->GetRandomWinner() ? BG_REWARD_WINNER_ARENA_LAST : BG_REWARD_WINNER_ARENA_FIRST;

        if (isBattleground() && sWorld->getBoolConfig(CONFIG_BATTLEGROUND_STORE_STATISTICS_ENABLE))
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVPSTATS_PLAYER);
            BattlegroundScoreMap::const_iterator score = PlayerScores.find(player->GetGUIDLow());

            // battleground_id, character_guid, score_killing_blows, score_deaths, score_honorable_kills, score_bonus_honor, score_damage_done, score_healing_done

            stmt->setUInt32(0, battlegroundId);
            stmt->setUInt32(1, player->GetGUIDLow());
            stmt->setBool(2, itr->second->GetBgTeam() == GetWinner());
            stmt->setUInt32(3, score->second->GetKillingBlows());
            stmt->setUInt32(4, score->second->GetDeaths());
            stmt->setUInt32(5, score->second->GetHonorableKills());
            stmt->setUInt32(6, score->second->GetBonusHonor());
            stmt->setUInt32(7, score->second->GetDamageDone());
            stmt->setUInt32(8, score->second->GetHealingDone());
            stmt->setUInt32(9, score->second->GetAttr1());
            stmt->setUInt32(10, score->second->GetAttr2());
            stmt->setUInt32(11, score->second->GetAttr3());
            stmt->setUInt32(12, score->second->GetAttr4());
            stmt->setUInt32(13, score->second->GetAttr5());

            CharacterDatabase.Execute(stmt);
        }

        if (sWorld->getBoolConfig(CONFIG_SPECIAL_ANGRATHAR) && !player->GetRandomWinner())
            winner_arena += 25;

        // Reward winner team
        if (bgTeamId == winnerTeamId)
        {
            if (player->IsCurrentBattlegroundRandom() || BattlegroundMgr::IsBGWeekend(GetBgTypeID()))
            {
                UpdatePlayerScore(player, SCORE_BONUS_HONOR, GetBonusHonorFromKill(winner_kills));

                // Xinef: check player level and not bracket level if (CanAwardArenaPoints())
                if (player->getLevel() >= BG_AWARD_ARENA_POINTS_MIN_LEVEL)
                    player->ModifyArenaPoints(winner_arena);

                if (!player->GetRandomWinner())
                    player->SetRandomWinner(true);
            }

            // @bgreward
            if (sWorld->getBoolConfig(CONFIG_BG_REWARD_ENABLE) && !isArena())
            {
                uint32 itemId             = sWorld->getIntConfig(CONFIG_BG_ITEM_REWARD_ID);
                int32 count               = sWorld->getIntConfig(CONFIG_BG_ITEM_REWARD_COUNT);
                std::string winnerMessage = sWorld->GetWinnerMessage();

                if (player->AddItem(itemId, count))
                    ChatHandler(player->GetSession()).SendSysMessage(winnerMessage.c_str());
            }

            // Reward Call to Arms quests - here due to fact those quests gonna be removed ;)
            switch (player->GetMapId())
            {
                // Call to Arms: WSG
                case 489: 
                {
                    if (player->GetQuestStatus(11338) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11338);

                    if (player->GetQuestStatus(11342) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11342);
                    break;
                }
                // Call to Arms: AB
                case 529: 
                {
                    if (player->GetQuestStatus(11335) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11335);

                    if (player->GetQuestStatus(11339) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11339);
                    break;
                }
                // Call to Arms: AV
                case 30: 
                {
                    if (player->GetQuestStatus(11336) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11336);

                    if (player->GetQuestStatus(11340) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11340);
                    break;
                }
                // Call to Arms: EotS
                case 566: 
                {
                    if (player->GetQuestStatus(11337) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11337);

                    if (player->GetQuestStatus(11341) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(11341);
                    break;
                }
                // Call to Arms: SotA
                case 607: 
                {
                    if (player->GetQuestStatus(13405) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(13405);

                    if (player->GetQuestStatus(13407) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(13407);
                    break;
                }
                // Call to Arms : IoC
                case 628:
                {
                    if (player->GetQuestStatus(14163) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(14163);

                    if (player->GetQuestStatus(14164) == QUEST_STATUS_INCOMPLETE)
                        player->AreaExploredOrEventHappens(14164);
                    break;
                }
                default:
                    break;
            }

            // Battleground weekly
            if (!isArena())
            {
                if (player->GetQuestStatus(40001) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(350001, 0);
            }

            //! Mythic quest
            if (player->IsCurrentBattlegroundRandom())
                if (player->GetQuestStatus(26036) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(360002, 0);

            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_BG, player->GetMapId());

            if (player->getLevel() < 80)
            {
                uint32 nextLvlXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
                float levelModifier = 0.012108f;
                float baseModifier = 1.0f;
                float exp = nextLvlXP * (baseModifier - ((player->getLevel() - 10) * levelModifier));

                if (exp > 0)
                    player->GiveXP(exp, nullptr);
            }
        }
        else
        {
            if (player->IsCurrentBattlegroundRandom() || BattlegroundMgr::IsBGWeekend(GetBgTypeID()))
                UpdatePlayerScore(player, SCORE_BONUS_HONOR, GetBonusHonorFromKill(loser_kills));

            if (player->getLevel() < 80)
            {
                uint32 nextLvlXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
                float levelModifier = 0.005357f;
                float baseModifier = 0.45f;
                float exp = nextLvlXP * (baseModifier - ((player->getLevel() - 10) * levelModifier));

                if (exp > 0)
                    player->GiveXP(exp, nullptr);
            }
        }

        // BG Reward
        if (sWorld->getBoolConfig(CONFIG_ARENA_REWARD_ENABLE) && bgTeamId == winnerTeamId && ((isArena() && isRated()) || (/*isBattleground() &&*/ player->IsCurrentBattlegroundRandom())))
        {
            uint32 rewardId = sWorld->getIntConfig(CONFIG_ARENA_WIN_ITEM_REWARD_ID);
            uint32 count    = sWorld->getIntConfig(CONFIG_ARENA_WIN_ITEM_REWARD_COUNT);
            if (player->AddItem(rewardId, count))
            {
                ChatHandler handler(player->GetSession());
                handler.PSendSysMessage(LANG_ARENA_WIN_REWARD, isArena() ? "arena match" : "battleground");
            }
        }

        player->ResetAllPowers();
        player->CombatStopWithPets(true);

        BlockMovement(player);

        WorldPacket data;
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player->GetCurrentBattlegroundQueueSlot(), STATUS_IN_PROGRESS, TIME_TO_AUTOREMOVE, GetStartTime(), GetArenaType(), player->GetBgTeam());
        player->GetSession()->SendPacket(&data);

        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND, player->GetMapId());
    }

    WorldPacket pvpLogData;
    sBattlegroundMgr->BuildPvpLogDataPacket(&pvpLogData, this);
    SendPacketToAll(&pvpLogData);

    if (winmsg_id)
        SendMessageToAll(winmsg_id, CHAT_MSG_BG_SYSTEM_NEUTRAL);
}

uint32 Battleground::GetBonusHonorFromKill(uint32 kills) const
{
    //variable kills means how many honorable kills you scored (so we need kills * honor_for_one_kill)
    uint32 maxLevel = std::min<uint32>(GetMaxLevel(), 80U);
    return Trinity::Honor::hk_honor_at_level(maxLevel, float(kills));
}

void Battleground::BlockMovement(Player* player)
{
    player->SetClientControl(player, 0); // movement disabled NOTE: the effect will be automatically removed by client when the player is teleported from the battleground, so no need to send with uint8(1) in RemovePlayerAtLeave()
}

void Battleground::RemovePlayerAtLeave(Player* player)
{
    TeamId teamId = player->GetBgTeam();

    // check if the player was a participant of the match, or only entered through gm command
    bool participant                    = false;
    BattlegroundPlayerMap::iterator itr = m_Players.find(player->GetGUID());
    if (itr != m_Players.end())
    {
        UpdatePlayersCountByTeam(teamId, true); // -1 player
        m_Players.erase(itr);
        participant = true;
    }

    // delete player score if exists
    BattlegroundScoreMap::iterator itr2 = PlayerScores.find(player->GetGUID());
    if (itr2 != PlayerScores.end())
    {
        delete itr2->second;
        PlayerScores.erase(itr2);
    }

    RemovePlayerFromResurrectQueue(player);

    // resurrect on exit
    bool wasDead = false;
    if (!player->IsAlive())
    {
        player->ResurrectPlayer(1.0f);
        player->SpawnCorpseBones();
        wasDead = true;
    }

    player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // if the player was a match participant
    if (participant)
    {
        WorldPacket data;

        player->ClearAfkReports();

        //left a rated match in progress, consider as loser
        if (isArena() && isRated() && (GetStatus() == STATUS_IN_PROGRESS || GetStatus() == STATUS_WAIT_JOIN) && teamId != TEAM_NEUTRAL)
        {
            TeamId winner = TEAM_NEUTRAL;
            TeamId loser  = TEAM_NEUTRAL;

            if (teamId == TEAM_HORDE)
            {
                winner = TEAM_ALLIANCE;
                loser  = TEAM_HORDE;
            }
            else
            {
                winner = TEAM_HORDE;
                loser  = TEAM_ALLIANCE;
            }

            if (winner != TEAM_NEUTRAL && loser != TEAM_NEUTRAL)
            {
                int32 winnerMatchmakerRating = GetArenaTeamMatchmakerRating(winner);
                int32 loserMatchmakerRating = GetArenaTeamMatchmakerRating(GetOtherTeamId(winner));
                int32 loserMatchmakerChange = ArenaTeam::GetMatchmakerRatingMod(loserMatchmakerRating, winnerMatchmakerRating, false);

                if (ArenaTeam* loserArenaTeam = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(GetArenaType()))))
                {
                    loserArenaTeam->MemberLost(player, GetArenaTeamRating(teamId), winnerMatchmakerRating, loserMatchmakerChange);

                    if (GetArenaType() == ARENA_TYPE_SOLO_QUEUE && GetAlivePlayersCountByTeam(teamId) == 3 && !player->HasAura(26013))
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_AURA_DURATION, 5 * MINUTE * IN_MILLISECONDS);
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, 1);
                        player->CastCustomSpell(26013, values, player, TRIGGERED_FULL_MASK);

                        ChatHandler(player->GetSession()).SendSysMessage("You have been removed from the solo arena and punished with Deserter!");
                    }
                }
            }
        }

        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player->GetCurrentBattlegroundQueueSlot(), STATUS_NONE, 0, 0, 0, TEAM_NEUTRAL);
        player->GetSession()->SendPacket(&data);

        // remove from raid group if player is member
        if (Group* group = GetBgRaid(teamId))
            if (group->IsMember(player->GetGUID()))
                if (!group->RemoveMember(player->GetGUID())) // group was disbanded
                    SetBgRaid(teamId, nullptr);

        // let others know
        sBattlegroundMgr->BuildPlayerLeftBattlegroundPacket(&data, player->GetGUID());
        SendPacketToTeam(teamId, &data, player, false);

        // cast deserter
        if (isBattleground() && !player->IsGameMaster() && sWorld->getBoolConfig(CONFIG_BATTLEGROUND_CAST_DESERTER))
        {
            if (GetStatus() == STATUS_IN_PROGRESS || GetStatus() == STATUS_WAIT_JOIN)
            {
                // lets check if player was teleported from BG and schedule delayed Deserter spell cast
                if (player->IsBeingTeleportedFar())
                    player->ScheduleDelayedOperation(DELAYED_SPELL_CAST_DESERTER);
                else if (GetArenaType() != ARENA_TYPE_SOLO_QUEUE)
                    player->CastSpell(player, 26013, true); // Deserter
            }
        }
    }

    // BG subclass specific code
    RemovePlayer(player);

    // Remove shapeshift auras
    if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

    player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE, PLAYER_MAX_BATTLEGROUND_QUEUES, false, false, TEAM_NEUTRAL, ARENA_TYPE_NONE);

    // Xinef: remove all criterias on bg leave
    player->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_CONDITION_BG_MAP, GetMapId(), true);
}

// this method is called when creating bg
void Battleground::Init()
{
    SetWinner(TEAM_NEUTRAL);
    SetStatus(STATUS_WAIT_QUEUE);
    SetStartTime(0);
    SetEndTime(0);
    SetLastResurrectTime(0);

    m_Events = 0;

    if (m_BgInvitedPlayers[TEAM_ALLIANCE] > 0 || m_BgInvitedPlayers[TEAM_HORDE] > 0)
    {
        sLog->outError("Battleground::Reset: one of the counters is not 0 (alliance: %u, horde: %u) for BG (map: %u, instance id: %u)!", m_BgInvitedPlayers[TEAM_ALLIANCE], m_BgInvitedPlayers[TEAM_HORDE], m_MapId, m_InstanceID);
        ASSERT(false);
    }

    m_BgInvitedPlayers[TEAM_ALLIANCE] = 0;
    m_BgInvitedPlayers[TEAM_HORDE]    = 0;

    m_Players.clear();

    for (BattlegroundScoreMap::const_iterator itr = PlayerScores.begin(); itr != PlayerScores.end(); ++itr)
        delete itr->second;
    PlayerScores.clear();

    ResetBGSubclass();
}

void Battleground::StartBattleground()
{
    SetStartTime(0);
    SetLastResurrectTime(0);

    // add bg to update list
    // this must be done here, because we need to have already invited some players when first Battleground::Update() method is executed
    sBattlegroundMgr->AddBattleground(this);
}

void Battleground::AddPlayer(Player* player)
{
    // remove afk from player
    if (player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK))
        player->ToggleAFK();

    //If player was in group (from WG) and doesn't leave
    if (Group* group = player->GetGroup())
    {
        if (group->isRaidGroup() && (group->isBGGroup() || group->isBFGroup()))
            player->RemoveFromGroup();
    }

    // score struct must be created in inherited class

    TeamId teamId = player->GetBgTeam();
    uint64 guid   = player->GetGUID();

    // Add to list/maps
    m_Players[guid] = player;

    UpdatePlayersCountByTeam(teamId, false); // +1 player

    for (auto&& it : m_Players)
    {
        if (player->IsPlayingCrossfaction())
            it.second->GetSession()->SendInvalidatePlayer(player->GetGUID());

        if (it.second->IsPlayingCrossfaction())
            player->GetSession()->SendInvalidatePlayer(it.second->GetGUID());
    }

    WorldPacket data;
    sBattlegroundMgr->BuildPlayerJoinedBattlegroundPacket(&data, player);
    SendPacketToTeam(teamId, &data, player, false);

    for (auto&& it : m_Players)
    {
        if (player->IsPlayingCrossfaction())
            it.second->GetSession()->SendNameQueryOpcode(player->GetGUID());

        if (it.second->IsPlayingCrossfaction())
            player->GetSession()->SendNameQueryOpcode(it.second->GetGUID());
    }

    player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // add arena specific auras
    if (isArena())
    {
        player->RemoveArenaEnchantments(TEMP_ENCHANTMENT_SLOT);
        if (teamId == TEAM_ALLIANCE) // gold
        {
            if (player->GetTeam() == TEAM_HORDE)
                player->CastSpell(player, SPELL_HORDE_GOLD_FLAG, true);
            else
                player->CastSpell(player, SPELL_ALLIANCE_GOLD_FLAG, true);
        }
        else // green
        {
            if (player->GetTeam() == TEAM_HORDE)
                player->CastSpell(player, SPELL_HORDE_GREEN_FLAG, true);
            else
                player->CastSpell(player, SPELL_ALLIANCE_GREEN_FLAG, true);
        }

        // restore pets health before remove
        if (Pet* pet = player->GetPet())
            if (pet->IsAlive())
                pet->SetHealth(pet->GetMaxHealth());

        player->DestroyConjuredItems(true);
        player->UnsummonPetTemporaryIfAny();
        if (uint32 spellId = player->GetLastPetSpell())
            player->RemoveSpellCooldown(spellId, true);

        if (GetStatus() == STATUS_WAIT_JOIN) // not started yet
        {
            player->CastSpell(player, SPELL_ARENA_PREPARATION, true);
            player->ResetAllPowers();
        }
    }
    else
    {
        // not started yet
        if (GetStatus() == STATUS_WAIT_JOIN)
        {
            if (Pet* pet = player->GetPet())
                if (pet->IsAlive() && pet->IsInWorld())
                    pet->SetHealth(pet->GetMaxHealth());

            player->CastSpell(player, SPELL_PREPARATION, true); // reduces all mana cost of spells.
        }
    }

    // Xinef: reset all map criterias on map enter
    player->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_CONDITION_BG_MAP, GetMapId(), true);

    // setup BG group membership
    PlayerAddedToBGCheckIfBGIsRunning(player);
    AddOrSetPlayerToCorrectBgGroup(player, teamId);
}

// this method adds player to his team's bg group, or sets his correct group if player is already in bg group
void Battleground::AddOrSetPlayerToCorrectBgGroup(Player* player, TeamId teamId)
{
    if (player->GetGroup() && (player->GetGroup()->isBGGroup() || player->GetGroup()->isBFGroup()))
    {
        sLog->outMisc("Battleground::AddOrSetPlayerToCorrectBgGroup - player is already in %s group!", (player->GetGroup()->isBGGroup() ? "BG" : "BF"));
        return;
    }

    if (GetArenaType() == ARENA_TYPE_SOLO_QUEUE)
        if (Group* group = player->GetGroup())
            group->RemoveMember(player->GetGUID());

    uint64 playerGuid = player->GetGUID();
    Group* group      = GetBgRaid(teamId);
    if (!group) // first player joined
    {
        group = new Group;
        SetBgRaid(teamId, group);
        group->Create(player);
    }
    else if (group->IsMember(playerGuid))
    {
        uint8 subgroup = group->GetMemberGroup(playerGuid);
        player->SetBattlegroundOrBattlefieldRaid(group, subgroup);
    }
    else
    {
        group->AddMember(player);

        if (Group* originalGroup = player->GetOriginalGroup())
            if (originalGroup->IsLeader(playerGuid))
            {
                group->ChangeLeader(playerGuid);
                group->SendUpdate();
            }
    }
}

uint32 Battleground::GetFreeSlotsForTeam(TeamId teamId) const
{
    return GetInvitedCount(teamId) < GetMaxPlayersPerTeam() ? GetMaxPlayersPerTeam() - GetInvitedCount(teamId) : 0;
}

uint32 Battleground::GetMaxFreeSlots() const
{
    return std::max<uint32>(GetFreeSlotsForTeam(TEAM_ALLIANCE), GetFreeSlotsForTeam(TEAM_HORDE));
}

bool Battleground::HasFreeSlots() const
{
    if (GetStatus() != STATUS_WAIT_JOIN && GetStatus() != STATUS_IN_PROGRESS)
        return false;
    for (uint8 i = 0; i < BG_TEAMS_COUNT; ++i)
        if (GetFreeSlotsForTeam((TeamId)i) > 0)
            return true;
    return false;
}

void Battleground::SpectatorsSendPacket(WorldPacket& data)
{
    for (SpectatorList::const_iterator itr = m_Spectators.begin(); itr != m_Spectators.end(); ++itr)
        (*itr)->GetSession()->SendPacket(&data);
}

void Battleground::ReadyMarkerClicked(Player* p)
{
    if (!isArena() || GetStatus() >= STATUS_IN_PROGRESS || GetStartDelayTime() <= BG_START_DELAY_15S || (m_Events & BG_STARTING_EVENT_3) || p->IsSpectator())
        return;

    readyMarkerClickedSet.insert(p->GetGUIDLow());
    uint32 count = readyMarkerClickedSet.size();

    uint32 req = GetArenaType() * 2;
    if (GetArenaType() == ARENA_TYPE_SOLO_QUEUE)
        req = 6;

    p->GetSession()->SendNotification("You are marked as ready %u/%u", count, req);
    if (count == req)
    {
        m_Events |= BG_STARTING_EVENT_2;
        m_StartTime += GetStartDelayTime() - BG_START_DELAY_15S;
        SetStartDelayTime(BG_START_DELAY_15S);
    }
}

void Battleground::UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool doAddHonor)
{
    //this procedure is called from virtual function implemented in bg subclass
    BattlegroundScoreMap::const_iterator itr = PlayerScores.find(player->GetGUID());
    if (itr == PlayerScores.end()) // player not found...
        return;

    switch (type)
    {
        case SCORE_KILLING_BLOWS: // Killing blows
            itr->second->KillingBlows += value;
            if (isArena() && isRated())
            {
                ArenaLogEntryDataMap::iterator itr2 = ArenaLogEntries.find(player->GetGUID());
                if (itr2 != ArenaLogEntries.end())
                    itr2->second.KillingBlows += value;
            }
            break;
        case SCORE_DEATHS: // Deaths
            itr->second->Deaths += value;
            break;
        case SCORE_HONORABLE_KILLS: // Honorable kills
            itr->second->HonorableKills += value;
            break;
        case SCORE_BONUS_HONOR: // Honor bonus
            // do not add honor in arenas
            if (isBattleground())
            {
                // reward honor instantly
                if (doAddHonor)
                    player->RewardHonor(nullptr, 1, value); // RewardHonor calls UpdatePlayerScore with doAddHonor = false
                else
                    itr->second->BonusHonor += value;
            }
            break;
            // used only in EY, but in MSG_PVP_LOG_DATA opcode
        case SCORE_DAMAGE_DONE: // Damage Done
            itr->second->DamageDone += value;
            if (isArena() && isRated() && GetStatus() == STATUS_IN_PROGRESS)
            {
                ArenaLogEntryDataMap::iterator itr2 = ArenaLogEntries.find(player->GetGUID());
                if (itr2 != ArenaLogEntries.end())
                    itr2->second.DamageDone += value;
            }
            break;
        case SCORE_HEALING_DONE: // Healing Done
            itr->second->HealingDone += value;
            if (isArena() && isRated() && GetStatus() == STATUS_IN_PROGRESS)
            {
                ArenaLogEntryDataMap::iterator itr2 = ArenaLogEntries.find(player->GetGUID());
                if (itr2 != ArenaLogEntries.end())
                    itr2->second.HealingDone += value;
            }
            break;
        default:
            sLog->outError("Battleground::UpdatePlayerScore: unknown score type (%u) for BG (map: %u, instance id: %u)!",
                type, m_MapId, m_InstanceID);
            break;
    }
}

void Battleground::AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid)
{
    m_ReviveQueue[npc_guid].push_back(player_guid);

    Player* player = ObjectAccessor::FindPlayer(player_guid);
    if (!player)
        return;

    player->CastSpell(player, SPELL_WAITING_FOR_RESURRECT, true);
}

void Battleground::RemovePlayerFromResurrectQueue(Player* player)
{
    for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
        for (std::vector<uint64>::iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
            if (*itr2 == player->GetGUID())
            {
                (itr->second).erase(itr2);
                player->RemoveAurasDueToSpell(SPELL_WAITING_FOR_RESURRECT);
                return;
            }
}

void Battleground::RelocateDeadPlayers(uint64 queueIndex)
{
    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<uint64>& ghostList = m_ReviveQueue[queueIndex];
    if (!ghostList.empty())
    {
        WorldSafeLocsEntry const* closestGrave = nullptr;
        for (std::vector<uint64>::const_iterator itr = ghostList.begin(); itr != ghostList.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;

            if (!closestGrave)
                closestGrave = GetClosestGraveyard(player);

            if (closestGrave)
                player->TeleportTo(GetMapId(), closestGrave->x, closestGrave->y, closestGrave->z, player->GetOrientation());
        }
        ghostList.clear();
    }
}

bool Battleground::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3, uint32 /*respawnTime*/, GOState goState)
{
    // If the assert is called, means that BgObjects must be resized!
    ASSERT(type < BgObjects.size());

    Map* map = FindBgMap();
    if (!map)
        return false;
    // Must be created this way, adding to godatamap would add it to the base map of the instance
    // and when loading it (in go::LoadFromDB()), a new guid would be assigned to the object, and a new object would be created
    // So we must create it specific for this instance
    GameObject* go = sObjectMgr->IsGameObjectStaticTransport(entry) ? new StaticTransport() : new GameObject();
    if (!go->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), entry, GetBgMap(),
            PHASEMASK_NORMAL, x, y, z, o, G3D::Quat(rotation0, rotation1, rotation2, rotation3), 100, goState))
    {
        sLog->outErrorDb("Battleground::AddObject: cannot create gameobject (entry: %u) for BG (map: %u, instance id: %u)!",
            entry, m_MapId, m_InstanceID);
        sLog->outError("Battleground::AddObject: cannot create gameobject (entry: %u) for BG (map: %u, instance id: %u)!",
            entry, m_MapId, m_InstanceID);
        delete go;
        return false;
    }
    /*
    uint32 guid = go->GetGUIDLow();

    // without this, UseButtonOrDoor caused the crash, since it tried to get go info from godata
    // iirc that was changed, so adding to go data map is no longer required if that was the only function using godata from GameObject without checking if it existed
    GameObjectData& data = sObjectMgr->NewGOData(guid);

    data.id             = entry;
    data.mapid          = GetMapId();
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
    data.rotation0      = rotation0;
    data.rotation1      = rotation1;
    data.rotation2      = rotation2;
    data.rotation3      = rotation3;
    data.spawntimesecs  = respawnTime;
    data.spawnMask      = 1;
    data.animprogress   = 100;
    data.go_state       = 1;
*/
    // Add to world, so it can be later looked up from HashMapHolder
    if (!map->AddToMap(go))
    {
        delete go;
        return false;
    }
    BgObjects[type] = go->GetGUID();
    return true;
}

// Some doors aren't despawned so we cannot handle their closing in gameobject::update()
// It would be nice to correctly implement GO_ACTIVATED state and open/close doors in gameobject code
void Battleground::DoorClose(uint32 type)
{
    if (GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]))
    {
        // If doors are open, close it
        if (obj->getLootState() == GO_ACTIVATED && obj->GetGoState() != GO_STATE_READY)
        {
            obj->SetLootState(GO_READY);
            obj->SetGoState(GO_STATE_READY);
        }
    }
    else
        sLog->outError("Battleground::DoorClose: door gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, GUID_LOPART(BgObjects[type]), m_MapId, m_InstanceID);
}

void Battleground::DoorOpen(uint32 type)
{
    if (GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]))
    {
        obj->SetLootState(GO_ACTIVATED);
        obj->SetGoState(GO_STATE_ACTIVE);
    }
    else
        sLog->outError("Battleground::DoorOpen: door gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, GUID_LOPART(BgObjects[type]), m_MapId, m_InstanceID);
}

GameObject* Battleground::GetBGObject(uint32 type)
{
    GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]);
    if (!obj)
        sLog->outError("Battleground::GetBGObject: gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, GUID_LOPART(BgObjects[type]), m_MapId, m_InstanceID);
    return obj;
}

Creature* Battleground::GetBGCreature(uint32 type)
{
    Creature* creature = GetBgMap()->GetCreature(BgCreatures[type]);
    if (!creature)
        sLog->outError("Battleground::GetBGCreature: creature (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, GUID_LOPART(BgCreatures[type]), m_MapId, m_InstanceID);
    return creature;
}

void Battleground::SpawnBGObject(uint32 type, uint32 respawntime)
{
    if (Map* map = FindBgMap())
        if (GameObject* obj = map->GetGameObject(BgObjects[type]))
        {
            if (respawntime)
            {
                obj->SendObjectDeSpawnAnim(obj->GetGUID());
                obj->SetLootState(GO_JUST_DEACTIVATED);
            }
            else if (obj->getLootState() == GO_JUST_DEACTIVATED)
            {
                // Change state from GO_JUST_DEACTIVATED to GO_READY in case battleground is starting again
                obj->SetLootState(GO_READY);
            }
            obj->SetRespawnTime(respawntime);
            map->AddToMap(obj);
        }
}

Creature* Battleground::AddCreature(uint32 entry, uint32 type, float x, float y, float z, float o, uint32 respawntime, MotionTransport* transport)
{
    // If the assert is called, means that BgCreatures must be resized!
    ASSERT(type < BgCreatures.size());

    Map* map = FindBgMap();
    if (!map)
        return nullptr;

    if (transport)
    {
        transport->CalculatePassengerPosition(x, y, z, &o);
        if (Creature* creature = transport->SummonCreature(entry, x, y, z, o, TEMPSUMMON_MANUAL_DESPAWN))
        {
            transport->AddPassenger(creature, true);
            BgCreatures[type] = creature->GetGUID();
            return creature;
        }

        return nullptr;
    }

    Creature* creature = new Creature();
    if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), map, PHASEMASK_NORMAL, entry, 0, x, y, z, o))
    {
        sLog->outError("Battleground::AddCreature: cannot create creature (entry: %u) for BG (map: %u, instance id: %u)!",
            entry, m_MapId, m_InstanceID);
        delete creature;
        return nullptr;
    }

    creature->SetHomePosition(x, y, z, o);

    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(entry);
    if (!cinfo)
    {
        sLog->outError("Battleground::AddCreature: creature template (entry: %u) does not exist for BG (map: %u, instance id: %u)!",
            entry, m_MapId, m_InstanceID);
        delete creature;
        return nullptr;
    }
    // Force using DB speeds
    creature->SetSpeedRate(MOVE_WALK, cinfo->speed_walk);
    creature->SetSpeedRate(MOVE_RUN, cinfo->speed_run);

    if (!map->AddToMap(creature))
    {
        delete creature;
        return nullptr;
    }

    BgCreatures[type] = creature->GetGUID();

    if (respawntime)
        creature->SetRespawnDelay(respawntime);

    // Xinef: Set PVP state for vehicles, should be for all creatures in bg?
    if (creature->IsVehicle())
        creature->SetPvP(true);

    return creature;
}

bool Battleground::DelCreature(uint32 type)
{
    if (!BgCreatures[type])
        return true;

    if (Creature* creature = GetBgMap()->GetCreature(BgCreatures[type]))
    {
        creature->AddObjectToRemoveList();
        BgCreatures[type] = 0;
        return true;
    }

    sLog->outError("Battleground::DelCreature: creature (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
        type, GUID_LOPART(BgCreatures[type]), m_MapId, m_InstanceID);
    BgCreatures[type] = 0;
    return false;
}

bool Battleground::DelObject(uint32 type)
{
    if (!BgObjects[type])
        return true;

    if (GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]))
    {
        obj->SetRespawnTime(0); // not save respawn time
        obj->Delete();
        BgObjects[type] = 0;
        return true;
    }
    sLog->outError("Battleground::DelObject: gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
        type, GUID_LOPART(BgObjects[type]), m_MapId, m_InstanceID);
    BgObjects[type] = 0;
    return false;
}

bool Battleground::AddSpiritGuide(uint32 type, float x, float y, float z, float o, TeamId teamId)
{
    uint32 entry = (teamId == TEAM_ALLIANCE) ? BG_CREATURE_ENTRY_A_SPIRITGUIDE : BG_CREATURE_ENTRY_H_SPIRITGUIDE;

    if (Creature* creature = AddCreature(entry, type, x, y, z, o))
    {
        creature->setDeathState(DEAD);
        creature->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, creature->GetGUID());
        // aura
        // TODO: Fix display here
        // creature->SetVisibleAura(0, SPELL_SPIRIT_HEAL_CHANNEL);
        // casting visual effect
        creature->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
        // correct cast speed
        creature->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
        //creature->CastSpell(creature, SPELL_SPIRIT_HEAL_CHANNEL, true);
        return true;
    }
    sLog->outError("Battleground::AddSpiritGuide: cannot create spirit guide (type: %u, entry: %u) for BG (map: %u, instance id: %u)!",
        type, entry, m_MapId, m_InstanceID);
    EndNow();
    return false;
}

void Battleground::SendMessageToAll(int32 entry, ChatMsg type, Player const* source)
{
    if (!entry)
        return;

    Trinity::BattlegroundChatBuilder bg_builder(type, entry, source);
    Trinity::LocalizedPacketDo<Trinity::BattlegroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void Battleground::PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...)
{
    if (!entry)
        return;

    va_list ap;
    va_start(ap, source);

    Trinity::BattlegroundChatBuilder bg_builder(type, entry, source, &ap);
    Trinity::LocalizedPacketDo<Trinity::BattlegroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);

    va_end(ap);
}

Optional<WorldPacket> Battleground::BuildBattlegroundPlayerPositionPacket(Player* player)
{
    WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, 4 + 4 + 16 * 2);
    data << 0; // CGBattlefieldInfo__m_numPlayerPositions

    uint32_t flagsCount = 0u;

    size_t offset = data.wpos();
    data << flagsCount;

    for (TeamId team : { TEAM_ALLIANCE, TEAM_HORDE })
    {
        if (Player* carrier = ObjectAccessor::FindPlayer(GetFlagPickerGUID(team)))
        {
            data << uint64(carrier->GetGUID());
            data << float(carrier->GetPositionX());
            data << float(carrier->GetPositionY());

            ++flagsCount;
        }
    }

    data.put(offset, flagsCount);

    return data;
}

void Battleground::SendWarningToAll(int32 entry, ...)
{
    if (!entry)
        return;

    std::map<uint32, WorldPacket> localizedPackets;
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (localizedPackets.find(itr->second->GetSession()->GetSessionDbLocaleIndex()) == localizedPackets.end())
        {
            char const* format = sObjectMgr->GetTrinityString(entry, itr->second->GetSession()->GetSessionDbLocaleIndex());

            char str[1024];
            va_list ap;
            va_start(ap, entry);
            vsnprintf(str, 1024, format, ap);
            va_end(ap);

            ChatHandler::BuildChatPacket(localizedPackets[itr->second->GetSession()->GetSessionDbLocaleIndex()], CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, nullptr, nullptr, str);
        }

        itr->second->SendDirectMessage(&localizedPackets[itr->second->GetSession()->GetSessionDbLocaleIndex()]);
    }
}

void Battleground::SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 arg1, int32 arg2)
{
    Trinity::Battleground2ChatBuilder bg_builder(type, entry, source, arg1, arg2);
    Trinity::LocalizedPacketDo<Trinity::Battleground2ChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void Battleground::EndNow()
{
    SetStatus(STATUS_WAIT_LEAVE);
    SetEndTime(0);
}

// To be removed
char const* Battleground::GetTrinityString(int32 entry)
{
    // FIXME: now we have different DBC locales and need localized message for each target client
    return sObjectMgr->GetTrinityStringForDBCLocale(entry);
}

void Battleground::HandleTriggerBuff(GameObject* gameObject)
{
    // Xinef: crash fix?
    if (GetStatus() != STATUS_IN_PROGRESS || !GetPlayersSize() || BgObjects.empty())
        return;

    uint32 index = 0;
    for (; index < BgObjects.size() && BgObjects[index] != gameObject->GetGUID(); ++index)
        ;
    if (BgObjects[index] != gameObject->GetGUID())
        return;

    if (m_BuffChange)
    {
        uint8 buff = urand(0, 2);
        if (gameObject->GetEntry() != Buff_Entries[buff])
        {
            SpawnBGObject(index, RESPAWN_ONE_DAY);
            for (uint8 currBuffTypeIndex = 0; currBuffTypeIndex < 3; ++currBuffTypeIndex)
                if (gameObject->GetEntry() == Buff_Entries[currBuffTypeIndex])
                {
                    index -= currBuffTypeIndex;
                    index += buff;
                }
        }
    }

    SpawnBGObject(index, BUFF_RESPAWN_TIME);
}

void Battleground::HandleKillPlayer(Player* victim, Player* killer)
{
    // Keep in mind that for arena this will have to be changed a bit

    // Add +1 deaths
    UpdatePlayerScore(victim, SCORE_DEATHS, 1);
    // Add +1 kills to group and +1 killing_blows to killer
    if (killer)
    {
        // Don't reward credit for killing ourselves, like fall damage of hellfire (warlock)
        if (killer == victim)
            return;

        UpdatePlayerScore(killer, SCORE_HONORABLE_KILLS, 1);
        UpdatePlayerScore(killer, SCORE_KILLING_BLOWS, 1);

        for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            Player* creditedPlayer = itr->second;
            if (creditedPlayer == killer)
                continue;

            if (creditedPlayer->GetBgTeam() == killer->GetBgTeam() && (creditedPlayer == killer || creditedPlayer->IsAtGroupRewardDistance(victim)))
                UpdatePlayerScore(creditedPlayer, SCORE_HONORABLE_KILLS, 1);
        }
    }

    if (!isArena())
    {
        // To be able to remove insignia -- ONLY IN Battlegrounds
        victim->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
        RewardXPAtKill(killer, victim);
    }
}

TeamId Battleground::GetOtherTeamId(TeamId teamId)
{
    return teamId != TEAM_NEUTRAL ? (teamId == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE) : TEAM_NEUTRAL;
}

bool Battleground::IsPlayerInBattleground(uint64 guid) const
{
    BattlegroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return true;
    return false;
}

void Battleground::PlayerAddedToBGCheckIfBGIsRunning(Player* player)
{
    if (GetStatus() != STATUS_WAIT_LEAVE)
        return;

    WorldPacket data;
    BlockMovement(player);

    for (auto& it : m_Players)
    {
        if (player->IsPlayingCrossfaction())
            it.second->GetSession()->SendInvalidatePlayer(player->GetGUID());

        if (it.second->IsPlayingCrossfaction())
            player->GetSession()->SendInvalidatePlayer(it.first);
    }

    sBattlegroundMgr->BuildPvpLogDataPacket(&data, this);
    player->GetSession()->SendPacket(&data);

    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player->GetCurrentBattlegroundQueueSlot(), STATUS_IN_PROGRESS, GetEndTime(), GetStartTime(), GetArenaType(), player->GetBgTeam());
    player->GetSession()->SendPacket(&data);
}

uint32 Battleground::GetAlivePlayersCountByTeam(TeamId teamId) const
{
    uint32 count = 0;
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (itr->second->IsAlive() && !itr->second->HasByteFlag(UNIT_FIELD_BYTES_2, 3, FORM_SPIRITOFREDEMPTION) && itr->second->GetBgTeam() == teamId)
            ++count;

    return count;
}

void Battleground::SetHoliday(bool is_holiday)
{
    m_HonorMode = is_holiday ? BG_HOLIDAY : BG_NORMAL;
}

int32 Battleground::GetObjectType(uint64 guid)
{
    for (uint32 i = 0; i < BgObjects.size(); ++i)
        if (BgObjects[i] == guid)
            return i;
    sLog->outError("Battleground::GetObjectType: player used gameobject (GUID: %u) which is not in internal data for BG (map: %u, instance id: %u), cheating?",
        GUID_LOPART(guid), m_MapId, m_InstanceID);
    return -1;
}

void Battleground::HandleKillUnit(Creature* /*victim*/, Player* /*killer*/)
{
}

void Battleground::CheckArenaAfterTimerConditions()
{
    EndBattleground(TEAM_NEUTRAL);
}

void Battleground::CheckArenaWinConditions()
{
    if (isArena() && GetStatus() <= STATUS_WAIT_JOIN) // pussywizard
        return;
    if (!GetAlivePlayersCountByTeam(TEAM_ALLIANCE) && GetPlayersCountByTeam(TEAM_HORDE))
        EndBattleground(TEAM_HORDE);
    else if (GetPlayersCountByTeam(TEAM_ALLIANCE) && !GetAlivePlayersCountByTeam(TEAM_HORDE))
        EndBattleground(TEAM_ALLIANCE);
}

void Battleground::UpdateArenaWorldState()
{
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(TEAM_HORDE));
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(TEAM_ALLIANCE));
}

void Battleground::SetBgRaid(TeamId teamId, Group* bg_raid)
{
    Group*& old_raid = m_BgRaids[teamId];
    if (old_raid)
        old_raid->SetBattlegroundGroup(nullptr);
    if (bg_raid)
        bg_raid->SetBattlegroundGroup(this);
    old_raid = bg_raid;
}

WorldSafeLocsEntry const* Battleground::GetClosestGraveyard(Player* player)
{
    return sObjectMgr->GetClosestGraveyard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetBgTeam());
}

void Battleground::StartTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry)
{
    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        itr->second->StartTimedAchievement(type, entry);
}

uint32 Battleground::GetTeamScore(TeamId teamId) const
{
    if (teamId == TEAM_ALLIANCE || teamId == TEAM_HORDE)
        return m_TeamScores[teamId];

    sLog->outError("GetTeamScore with wrong Team %u for BG %u", teamId, GetBgTypeID());
    return 0;
}

void Battleground::RewardXPAtKill(Player* killer, Player* victim)
{
    if (sWorld->getBoolConfig(CONFIG_BG_XP_FOR_KILL) && killer && victim)
        killer->RewardPlayerAndGroupAtKill(victim, true);
}

ValidArenaResult Battleground::ValidateRatedArena(TeamId winnerTeamId) const
{
    /*
    uint32 winnerArenaTeamId = GetArenaTeamIdForTeam(winnerTeamId == TEAM_NEUTRAL ? TEAM_HORDE : winnerTeamId);
    uint32 loserArenaTeamId  = GetArenaTeamIdForTeam(winnerTeamId == TEAM_NEUTRAL ? TEAM_ALLIANCE : GetOtherTeamId(winnerTeamId));
    uint32 matchingIPs       = 0;

    if (sWorld->getBoolConfig(CONFIG_ARENA_NO_MATCHING_IP_ADDRESSES))
    {

        for (auto const& winners : ArenaLogEntries)
        {
            if (winners.second.ArenaTeamId != winnerArenaTeamId)
                continue;

            for (auto const& opposite : ArenaLogEntries)
            {
                if (opposite.second.ArenaTeamId == winnerArenaTeamId)
                    continue;

                if (winners.second.IP == opposite.second.IP)
                    ++matchingIPs;
            }
        }
    }

    uint32 sameArenaOpponentCount = 0;
    if (sWorld->getBoolConfig(CONFIG_ARENA_NO_REPEATING_TEAMS))
    {
        std::vector<uint32> recentTeams = sWorld->GetArenaWinstreakForTeam(winnerArenaTeamId);
        if (!recentTeams.empty())
        {
            for (uint32 team : recentTeams)
                if (team == loserArenaTeamId)
                    ++sameArenaOpponentCount;
        }
    }

    {
        auto currentTime = std::chrono::system_clock::now();
        tm currTm        = boost::posix_time::to_tm(boost::posix_time::from_time_t(std::chrono::system_clock::to_time_t(currentTime)));

        int32 aSince = sWorld->m_arenaAvailabilityHours.AvailableSince;
        int32 aTo    = sWorld->m_arenaAvailabilityHours.AvailableTo;
        if ((aSince + aTo) != 0 && (aSince > currTm.tm_hour || aTo > currTm.tm_hour))
            return ValidArenaResult::RESULT_WRONG_HOUR;
    }

    if (matchingIPs)
        return ValidArenaResult::RESULT_SAME_IP_ADDRESS_IN_OPPOSITE_TEAM;

    if (sameArenaOpponentCount >= MAXIMUM_REPEATING_ARENA_TEAMS)
        return ValidArenaResult::RESULT_REPEATING_ARENA_OPPONENTS;
        */
    return ValidArenaResult::RESULT_OK;
}

void Battleground::SetArenaTeamRatingChangeForTeam(TeamId teamId, int32 RatingChange)
{ 
    m_ArenaTeamRatingChanges[teamId] = RatingChange;
}

int32 Battleground::GetArenaTeamRatingChangeForTeam(TeamId teamId) const 
{ 
    return m_ArenaTeamRatingChanges[teamId]; 
}

void Battleground::SetArenaTeamMatchmakerRating(TeamId teamId, uint32 MMR) 
{ 
    m_ArenaTeamMMR[teamId] = MMR; 
}

uint32 Battleground::GetArenaTeamMatchmakerRating(TeamId teamId) const 
{ 
    return m_ArenaTeamMMR[teamId]; 
}

void Battleground::SetArenaTeamRating(TeamId teamId, uint32 Rating) 
{ 
    m_ArenaTeamRating[teamId] = Rating; 
}

uint32 Battleground::GetArenaTeamRating(TeamId teamId) const 
{ 
    return m_ArenaTeamRating[teamId]; 
}

void Battleground::SetArenaTeamIdForTeams(TeamId teamId, std::unordered_map<uint64, uint32> arenaTeams)
{
    m_ArenaTeamIds[teamId] = arenaTeams;
}

std::unordered_map<uint64, uint32> const& Battleground::GetArenaTeamIdsForTeam(TeamId teamId) const
{
    return m_ArenaTeamIds[teamId];
}

void Battleground::SetArenaTeamLastDPSList(TeamId teamId, std::set<uint64> const& list)
{
    if (!list.empty())
    {
        m_ArenaTeamLastDPSList[teamId].clear();
        m_ArenaTeamLastDPSList[teamId].insert(list.begin(), list.end());
    }
}

std::set<uint64> const& Battleground::GetArenaTeamLastDPSList(TeamId teamId) const
{
    return m_ArenaTeamLastDPSList[teamId];
}

void Battleground::SetArenaFightID(uint32 arenaFightID)
{
    _arenaFightID = arenaFightID;
}
