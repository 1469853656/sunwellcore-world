

#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "ArenaTeam.h"
#include "World.h"
#include "Group.h"
#include "ArenaTeamMgr.h"
#include "Player.h"
#include "WorldSession.h"
#include "Opcodes.h"

ArenaTeam::ArenaTeam() : TeamId(0), Type(0), CharClass(CLASS_NONE), TeamName(), CaptainGuid(0), BackgroundColor(0), EmblemStyle(0), EmblemColor(0), BorderStyle(0), BorderColor(0)
{
    Stats.WeekGames = 0;
    Stats.SeasonGames = 0;
    Stats.Rank = 0;
    Stats.Rating = sWorld->getIntConfig(CONFIG_ARENA_START_RATING);
    Stats.WeekWins = 0;
    Stats.SeasonWins = 0;
    Stats.MatchMakerRating = sWorld->getIntConfig(CONFIG_ARENA_START_MATCHMAKER_RATING);
    Stats.MaxMMR = Stats.MatchMakerRating;
}

ArenaTeam::~ArenaTeam()
{
}

bool ArenaTeam::Create(Player* player, uint8 type)
{
    // Generate new arena team id
    TeamId = sArenaTeamMgr->GenerateArenaTeamId();

    // Assign member variables
    CaptainGuid = player->GetGUID();
    Type = type;
    CharName = player->GetName();
    TeamName = fmt::format("{}vs{}", type, type);
    if (type == ARENA_TYPE_SOLO_QUEUE)
        TeamName = "SoloQueue";
    CharClass = player->getClass();

    BackgroundColor = 0;
    EmblemStyle = 0;
    EmblemColor = 0;
    BorderStyle = 0;
    BorderColor = 0;

    uint32 captainLowGuid = GUID_LOPART(CaptainGuid);
    sWorld->UpdateGlobalPlayerArenaTeam(captainLowGuid, GetSlot(), TeamId);

    player->SetInArenaTeam(TeamId, GetSlot(), Type);
    player->SetArenaTeamInfoField(Type, ARENA_TEAM_MEMBER, 0);
    player->SetArenaTeamInfoField(Type, ARENA_TEAM_GAMES_WEEK, Stats.WeekGames);
    player->SetArenaTeamInfoField(Type, ARENA_TEAM_GAMES_SEASON, Stats.SeasonGames);
    player->SetArenaTeamInfoField(Type, ARENA_TEAM_WINS_SEASON, Stats.SeasonWins);
    player->SetArenaTeamInfoField(Type, ARENA_TEAM_PERSONAL_RATING, Stats.Rating);

    // Save arena team to db
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARENA_TEAM);
    stmt->setUInt32(0, TeamId);
    stmt->setString(1, TeamName);
    stmt->setString(2, CharName);
    stmt->setUInt32(3, captainLowGuid);
    stmt->setUInt8(4, Type);
    stmt->setUInt16(5, Stats.Rating);
    stmt->setUInt16(6, Stats.MatchMakerRating);
    stmt->setUInt16(7, Stats.MaxMMR);
    stmt->setUInt32(8, BackgroundColor);
    stmt->setUInt8(9, EmblemStyle);
    stmt->setUInt32(10, EmblemColor);
    stmt->setUInt8(11, BorderStyle);
    stmt->setUInt32(12, BorderColor);
    CharacterDatabase.Execute(stmt);

    return true;
}

bool ArenaTeam::LoadArenaTeamFromDB(QueryResult result)
{
    if (!result)
        return false;

    Field* fields = result->Fetch();

    TeamId = fields[0].GetUInt32();
    TeamName = fields[1].GetString();
    CharName = fields[2].GetString();
    CaptainGuid = MAKE_NEW_GUID(fields[3].GetUInt32(), 0, HIGHGUID_PLAYER);
    Type = fields[4].GetUInt8();
    BackgroundColor = fields[5].GetUInt32();
    EmblemStyle = fields[6].GetUInt8();
    EmblemColor = fields[7].GetUInt32();
    BorderStyle = fields[8].GetUInt8();
    BorderColor = fields[9].GetUInt32();
    Stats.Rating = fields[10].GetUInt16();
    Stats.MatchMakerRating = fields[11].GetUInt16();
    Stats.MaxMMR = std::max(Stats.MatchMakerRating, fields[12].GetUInt16());
    Stats.WeekGames = fields[13].GetUInt16();
    Stats.WeekWins = fields[14].GetUInt16();
    Stats.SeasonGames = fields[15].GetUInt16();
    Stats.SeasonWins = fields[16].GetUInt16();
    Stats.Rank = fields[17].GetUInt32();

    return true;
}

void ArenaTeam::Disband()
{
    // Update database
    auto trans = CharacterDatabase.BeginTransaction();

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARENA_TEAM);
    stmt->setUInt32(0, TeamId);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    // Remove arena team from ObjectMgr
    sArenaTeamMgr->RemoveArenaTeam(TeamId);

    delete this;
}

void ArenaTeam::Roster(WorldSession* session)
{
    uint8 unk308 = 0;

    WorldPacket data(SMSG_ARENA_TEAM_ROSTER, 100);
    data << uint32(GetId());          // team id
    data << uint8(unk308);            // 308 unknown value but affect packet structure
    data << uint32(1);                // members count
    data << uint32(GetType());        // arena team type?

    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(CaptainGuid);

    data << uint64(CaptainGuid);                        // guid
    data << uint8((player ? 1 : 0));                    // online flag
    data << CharName;                                   // member name
    data << uint32(0);                                  // captain flag 0 captain 1 member
    data << uint8((player ? player->getLevel() : 0));   // unknown, level?
    data << uint8(CharClass);                           // class
    data << uint32(Stats.WeekGames);                    // played this week
    data << uint32(Stats.WeekWins);                     // wins this week
    data << uint32(Stats.SeasonGames);                  // played this season
    data << uint32(Stats.SeasonWins);                   // wins this season
    data << uint32(Stats.Rating);                       // personal rating
    if (unk308)
    {
        data << float(0.0f); // 308 unk
        data << float(0.0f); // 308 unk
    }

    session->SendPacket(&data);
    ; //sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_ARENA_TEAM_ROSTER");
}

void ArenaTeam::Query(WorldSession* session)
{
    WorldPacket data(SMSG_ARENA_TEAM_QUERY_RESPONSE, 4 * 7 + GetName().size() + 1);
    data << uint32(GetId());         // team id
    data << GetName();               // team name
    data << uint32(GetType());       // arena team type (2=2x2, 3=3x3 or 5=5x5)
    data << uint32(BackgroundColor); // background color
    data << uint32(EmblemStyle);     // emblem style
    data << uint32(EmblemColor);     // emblem color
    data << uint32(BorderStyle);     // border style
    data << uint32(BorderColor);     // border color
    session->SendPacket(&data);
    ; //sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_ARENA_TEAM_QUERY_RESPONSE");
}

void ArenaTeam::SendStats(WorldSession* session)
{
    WorldPacket data(SMSG_ARENA_TEAM_STATS, 4 * 7);
    data << uint32(GetId());           // team id
    data << uint32(Stats.Rating);      // rating
    data << uint32(Stats.WeekGames);   // games this week
    data << uint32(Stats.WeekWins);    // wins this week
    data << uint32(Stats.SeasonGames); // played this season
    data << uint32(Stats.SeasonWins);  // wins this season
    data << uint32(Stats.Rank);        // rank
    session->SendPacket(&data);
}

void ArenaTeam::NotifyStatsChanged()
{
    // This is called after a rated match ended
    // Updates arena team stats for every member of the team (not only the ones who participated!)
    if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(CaptainGuid))
        SendStats(player->GetSession());
}

void ArenaTeam::Inspect(WorldSession* session, uint64 guid)
{
    WorldPacket data(MSG_INSPECT_ARENA_TEAMS, 8 + 1 + 4 * 6);
    data << uint64(guid);                   // player guid
    data << uint8(GetSlot());               // slot (0...1)
    data << uint32(GetId());                // arena team id
    data << uint32(Stats.Rating);           // rating
    data << uint32(Stats.SeasonGames);      // season played
    data << uint32(Stats.SeasonWins);       // season wins
    data << uint32(Stats.SeasonGames);      // played (count of all games, that the inspected member participated...)
    data << uint32(Stats.Rating);           // personal rating
    session->SendPacket(&data);
}

void ArenaTeamStats::ModifyRating(Player* player, int32 mod, uint32 type)
{
    if (int32(Rating) + mod < 0)
        Rating = 0;
    else
        Rating += mod;

    if (player)
    {
        player->SetArenaTeamInfoField(type, ARENA_TEAM_PERSONAL_RATING, Rating);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, Rating, type);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING, Rating, type);
    }
}

void ArenaTeamStats::ModifyMatchmakerRating(int32 mod, uint32 /*type*/)
{
    if (mod < 0)
    {
        // pussywizard: prevent lowering MMR too much from max achieved MMR
        int32 maxAllowedDrop = (int32)sWorld->getIntConfig(CONFIG_MAX_ALLOWED_MMR_DROP);
        mod = std::min<int32>(std::max<int32>(-((int32)MatchMakerRating - (int32)MaxMMR + maxAllowedDrop), mod), 0);
    }

    if (int32(MatchMakerRating) + mod < 0)
        MatchMakerRating = 0;
    else
        MatchMakerRating += mod;

    if (MaxMMR < MatchMakerRating)
        MaxMMR = MatchMakerRating;
}

void ArenaTeam::ForcePersonalRatingChange(Player* player, uint32 val, uint32 type)
{
    if (CaptainGuid == player->GetGUID())
    {
        Stats.Rating = val;
        Stats.MatchMakerRating = val;
        if (Stats.MaxMMR < val)
            Stats.MaxMMR = val;

        player->SetArenaTeamInfoField(type, ARENA_TEAM_PERSONAL_RATING, Stats.Rating);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, Stats.Rating, type);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING, Stats.Rating, type);
    }
}

void ArenaTeam::BroadcastPacket(WorldPacket* packet)
{
    if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(CaptainGuid))
        player->GetSession()->SendPacket(packet);
}

void ArenaTeam::BroadcastEvent(ArenaTeamEvents event, uint64 guid, uint8 strCount, std::string const& str1, std::string const& str2, std::string const& str3)
{
    WorldPacket data(SMSG_ARENA_TEAM_EVENT, 1 + 1 + 1);
    data << uint8(event);
    data << uint8(strCount);
    switch (strCount)
    {
        case 0:
            break;
        case 1:
            data << str1;
            break;
        case 2:
            data << str1 << str2;
            break;
        case 3:
            data << str1 << str2 << str3;
            break;
        default:
            sLog->outError("Unhandled strCount %u in ArenaTeam::BroadcastEvent", strCount);
            return;
    }

    if (guid)
        data << uint64(guid);

    BroadcastPacket(&data);

    ; //sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Sent SMSG_ARENA_TEAM_EVENT");
}

void ArenaTeam::MassInviteToEvent(WorldSession* session)
{
    WorldPacket data(SMSG_CALENDAR_ARENA_TEAM, 1 * (4 + 8 + 1));
    data << uint32(1);

    if (CaptainGuid != session->GetPlayer()->GetGUID())
    {
        data.appendPackGUID(CaptainGuid);
        data << uint8(0); // unk
    }

    session->SendPacket(&data);
}

uint8 ArenaTeam::GetSlotByType(uint32 type)
{
    switch (type)
    {
        case ARENA_TEAM_2v2: 
            return 0;
        case ARENA_TEAM_3v3: 
            return 1;
        case ARENA_TEAM_SOLO_QUEUE:
            return 2;
        default:
            break;
    }

    return 0xFF;
}

uint32 ArenaTeam::GetPoints(uint32 memberRating)
{
    // Returns how many points would be awarded with this team type with this rating
    float points = 0.f;
    uint32 rating = memberRating + 150 < Stats.Rating ? memberRating : Stats.Rating;

    if (rating <= 1500)
    {
        if (sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID) < 6)
            points = (float)rating * 0.22f + 14.0f;
        else
            points = 344;
    }
    else
        points = 1511.26f / (1.0f + 1639.28f * exp(-0.00412f * (float)rating));

    // Type penalties for teams < 5v5
    if (Type == ARENA_TEAM_2v2)
        points *= 0.76f;
    else if (Type == ARENA_TEAM_3v3)
        points *= 0.88f;

    points *= sWorld->getRate(RATE_ARENA_POINTS);

    return (uint32)points;
}

float ArenaTeam::GetChanceAgainst(uint32 ownRating, uint32 opponentRating)
{
    // Returns the chance to win against a team with the given rating, used in the rating adjustment calculation
    // ELO system
    return 1.0f / (1.0f + exp(log(10.0f) * (float)((float)opponentRating - (float)ownRating) / 650.0f));
}

int32 ArenaTeam::GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float& confidence_factor*/)
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance  = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;
    float mod     = won_mod - chance;

    // Work in progress:
    /*
    // This is a simulation, as there is not much info on how it really works
    float confidence_mod = min(1.0f - fabs(mod), 0.5f);

    // Apply confidence factor to the mod:
    mod *= confidence_factor

    // And only after that update the new confidence factor
    confidence_factor -= ((confidence_factor - 1.0f) * confidence_mod) / confidence_factor;
    */

    // Real rating modification
    mod *= sWorld->getFloatConfig(CONFIG_ARENA_MATCHMAKER_RATING_MODIFIER);

    auto finalModifier = static_cast<int32>(ceil(mod));
    if (won)
    {
        if (uint32 const maxGainAmount = sWorld->getIntConfig(CONFIG_ARENA_MAX_WIN_MMR))
            finalModifier = std::min(finalModifier, static_cast<int32>(maxGainAmount));
    }
    else
    {
        if (uint32 const maxLossAmount = sWorld->getIntConfig(CONFIG_ARENA_MAX_LOSE_MMR))
            finalModifier = std::max(finalModifier, -static_cast<int32>(maxLossAmount));
    }

    return finalModifier;
}

int32 ArenaTeam::GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float confidence_factor*/)
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);

    // Calculate the rating modification
    float mod;

    // TODO: Replace this hack with using the confidence factor (limiting the factor to 2.0f)
    if (won)
    {
        if (ownRating < 1300)
        {
            float win_rating_modifier1 = sWorld->getFloatConfig(CONFIG_ARENA_WIN_RATING_MODIFIER_1);

            if (ownRating < 1000)
                mod = win_rating_modifier1 * (1.0f - chance);
            else
                mod = ((win_rating_modifier1 / 2.0f) + ((win_rating_modifier1 / 2.0f) * (1300.0f - float(ownRating)) / 300.0f)) * (1.0f - chance);
        }
        else
            mod = sWorld->getFloatConfig(CONFIG_ARENA_WIN_RATING_MODIFIER_2) * (1.0f - chance);
    }
    else
        mod = sWorld->getFloatConfig(CONFIG_ARENA_LOSE_RATING_MODIFIER) * (-chance);

    auto finalModifier = static_cast<int32>(ceil(mod));
    if (won)
    {
        if (uint32 const maxGainAmount = sWorld->getIntConfig(CONFIG_ARENA_MAX_WIN_RATING))
            finalModifier = std::min(finalModifier, static_cast<int32>(maxGainAmount));
    }
    else
    {
        if (uint32 const maxLossAmount = sWorld->getIntConfig(CONFIG_ARENA_MAX_LOSE_RATING))
            finalModifier = std::max(finalModifier, -static_cast<int32>(maxLossAmount));
    }

    return finalModifier;
}

void ArenaTeam::FinishGame(int32 mod, const Map* bgMap)
{
    // Rating can only drop to 0
    if (int32(Stats.Rating) + mod < 0)
        Stats.Rating = 0;
    else
    {
        Stats.Rating += mod;

        // Check if rating related achivements are met
        if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(CaptainGuid))
        {
            player->SetArenaTeamInfoField(Type, ARENA_TEAM_PERSONAL_RATING, Stats.Rating);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, Stats.Rating, Type);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING, Stats.Rating, Type);
        }
    }

    // Update number of games played per season or week
    Stats.WeekGames += 1;
    Stats.SeasonGames += 1;

    // Update team's rank, start with rank 1 and increase until no team with more rating was found
    Stats.Rank                                         = 1;
    ArenaTeamMgr::ArenaTeamContainer::const_iterator i = sArenaTeamMgr->GetArenaTeamMapBegin();
    for (; i != sArenaTeamMgr->GetArenaTeamMapEnd(); ++i)
    {
        if (i->second->GetType() == Type && i->second->GetStats().Rating > Stats.Rating)
            ++Stats.Rank;
    }
}

int32 ArenaTeam::MemberLost(Player* player, uint32 rating, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange)
{
    if (CaptainGuid == player->GetGUID())
    {
        uint32 oldRating = Stats.Rating;
        int32 ratingChange = GetRatingMod(rating, againstMatchmakerRating, false);

         // Update personal rating
        Stats.ModifyRating(player, ratingChange, GetType());

        // Update matchmaker rating
        Stats.ModifyMatchmakerRating(MatchmakerRatingChange, GetType());

        // Update personal played stats
        Stats.WeekGames += 1;
        Stats.SeasonGames += 1;

        // update the unit fields
        player->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_GAMES_WEEK, Stats.WeekGames);
        player->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_GAMES_SEASON, Stats.SeasonGames);

         // Update team's rank, start with rank 1 and increase until no team with more rating was found
        Stats.Rank = 1;
        ArenaTeamMgr::ArenaTeamContainer::const_iterator i = sArenaTeamMgr->GetArenaTeamMapBegin();
        for (; i != sArenaTeamMgr->GetArenaTeamMapEnd(); ++i)
            if (i->second->GetType() == Type && i->second->GetStats().Rating > Stats.Rating)
                ++Stats.Rank;

        SaveToDB();
        NotifyStatsChanged();

        if (Stats.Rating == 0 && oldRating == 0 && Stats.SeasonGames > 10 && GetType() == ARENA_TYPE_SOLO_QUEUE)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_DURATION, 6 * HOUR * IN_MILLISECONDS);
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 1);
            player->CastCustomSpell(26013, values, player, TRIGGERED_FULL_MASK);
        }

        return ratingChange;
    }

    return 0;
}

int32 ArenaTeam::OfflineMemberLost(uint64 guid, uint32 rating, uint32 againstMatchmakerRating, int32 matchmakerRatingChange)
{
    // Called for offline player after ending rated arena match!
    if (CaptainGuid == guid)
    {
        int32 ratingChange = GetRatingMod(rating, againstMatchmakerRating, false);

        // update personal rating
        Stats.ModifyRating(nullptr, ratingChange, GetType());

        // update matchmaker rating
        Stats.ModifyMatchmakerRating(matchmakerRatingChange, GetType());

        // update personal played stats
        Stats.WeekGames += 1;
        Stats.SeasonGames += 1;

        // Update team's rank, start with rank 1 and increase until no team with more rating was found
        Stats.Rank = 1;
        ArenaTeamMgr::ArenaTeamContainer::const_iterator i = sArenaTeamMgr->GetArenaTeamMapBegin();
        for (; i != sArenaTeamMgr->GetArenaTeamMapEnd(); ++i)
            if (i->second->GetType() == Type && i->second->GetStats().Rating > Stats.Rating)
                ++Stats.Rank;

        SaveToDB();
        NotifyStatsChanged();

        return ratingChange;
    }

    return 0;
}

int32 ArenaTeam::MemberWon(Player* player, uint32 rating, uint32 againstMatchmakerRating, int32 matchmakerRatingChange)
{
    if (CaptainGuid == player->GetGUID())
    {
        int32 ratingChange = GetRatingMod(rating, againstMatchmakerRating, true);

        // update personal rating
        Stats.ModifyRating(player, ratingChange, GetType());

        // update matchmaker rating
        Stats.ModifyMatchmakerRating(matchmakerRatingChange, GetType());

        // update personal stats
        Stats.WeekGames += 1;
        Stats.SeasonGames += 1;
        Stats.SeasonWins += 1;
        Stats.WeekWins += 1;

       // update unit fields
        player->SetArenaTeamInfoField(GetType(), ARENA_TEAM_GAMES_WEEK, Stats.WeekGames);
        player->SetArenaTeamInfoField(GetType(), ARENA_TEAM_GAMES_SEASON, Stats.SeasonGames);

        // Update team's rank, start with rank 1 and increase until no team with more rating was found
        Stats.Rank = 1;
        ArenaTeamMgr::ArenaTeamContainer::const_iterator i = sArenaTeamMgr->GetArenaTeamMapBegin();
        for (; i != sArenaTeamMgr->GetArenaTeamMapEnd(); ++i)
            if (i->second->GetType() == Type && i->second->GetStats().Rating > Stats.Rating)
                ++Stats.Rank;

        SaveToDB();
        NotifyStatsChanged();

        return ratingChange;
    }

    return 0;
}

void ArenaTeam::UpdateArenaPointsHelper(std::map<uint32, uint32>& playerPoints)
{
    // Called after a match has ended and the stats are already modified
    // Helper function for arena point distribution (this way, when distributing, no actual calculation is required, just a few comparisons)
    // 10 played games per week is a minimum
    if (Stats.WeekGames < 10)
        return;

    // To get points, a player has to participate in at least 30% of the matches
    uint32 requiredGames = (uint32)ceil(Stats.WeekGames * 0.3f);

    // The player participated in enough games, update his points
    uint32 pointsToAdd = 0;
    if (Stats.WeekGames >= requiredGames)
        pointsToAdd = GetPoints(Stats.Rating);

    std::map<uint32, uint32>::iterator plr_itr = playerPoints.find(GUID_LOPART(CaptainGuid));
    if (plr_itr != playerPoints.end())
    {
        // Check if there is already more points
        if (plr_itr->second < pointsToAdd)
            playerPoints[GUID_LOPART(CaptainGuid)] = pointsToAdd;
    }
    else
        playerPoints[GUID_LOPART(CaptainGuid)] = pointsToAdd;
}

void ArenaTeam::SaveToDB()
{
    // Save team and member stats to db
    // Called after a match has ended or when calculating arena_points

    auto trans = CharacterDatabase.BeginTransaction();

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ARENA_TEAM_STATS);
    stmt->setUInt16(0, Stats.Rating);
    stmt->setUInt16(1, Stats.MatchMakerRating);
    stmt->setUInt16(2, Stats.MaxMMR);
    stmt->setUInt16(3, Stats.WeekGames);
    stmt->setUInt16(4, Stats.WeekWins);
    stmt->setUInt16(5, Stats.SeasonGames);
    stmt->setUInt16(6, Stats.SeasonWins);
    stmt->setUInt32(7, Stats.Rank);
    stmt->setUInt32(8, GetId());
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
}

void ArenaTeam::FinishWeek()
{
    // Reset team stats
    Stats.WeekGames = 0;
    Stats.WeekWins  = 0;
}

bool ArenaTeam::IsFighting() const
{
    if (Player* player = ObjectAccessor::FindPlayer(CaptainGuid))
        if (player->GetMap()->IsBattleArena())
            return true;

    return false;
}

void ArenaTeam::SetLastDPSList(std::set<uint64> const& list)
{
    if (!list.empty())
    {
        LastDPSList.clear();
        LastDPSList.insert(list.begin(), list.end());
    }
}

std::set<uint64> const& ArenaTeam::GetLastDPSList() const
{
    return LastDPSList;
}
