

#include "Define.h"
#include "ArenaTeamMgr.h"
#include "World.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SoloQueue.h"

ArenaTeamMgr::ArenaTeamMgr()
{
    NextArenaTeamId = 1;
    LastArenaLogId  = 0;
}

ArenaTeamMgr::~ArenaTeamMgr()
{
    for (ArenaTeamContainer::iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
        delete itr->second;
}

// Arena teams collection
ArenaTeam* ArenaTeamMgr::GetArenaTeamById(uint32 arenaTeamId) const
{
    ArenaTeamContainer::const_iterator itr = ArenaTeamStore.find(arenaTeamId);
    if (itr != ArenaTeamStore.end())
        return itr->second;

    return nullptr;
}

void ArenaTeamMgr::AddArenaTeam(ArenaTeam* arenaTeam)
{
    ArenaTeamStore[arenaTeam->GetId()] = arenaTeam;
}

void ArenaTeamMgr::RemoveArenaTeam(uint32 arenaTeamId)
{
    ArenaTeamStore.erase(arenaTeamId);
}

uint32 ArenaTeamMgr::GenerateArenaTeamId()
{
    if (NextArenaTeamId >= 0xFFFFFFFE)
    {
        sLog->outError("Arena team ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return NextArenaTeamId++;
}

void ArenaTeamMgr::LoadArenaTeams()
{
    uint32 oldMSTime = getMSTime();

    //                                                        0        1         2         3         4          5            6            7           8              9
    QueryResult result = CharacterDatabase.Query("SELECT arenaTeamId, name, charName, captainGuid, type, backgroundColor, emblemStyle, emblemColor, borderStyle, borderColor, "
                                                 //  10        11             12        13       14         15            16        17
                                                 "rating, matchmakerRating, maxMMR, weekGames, weekWins, seasonGames, seasonWins, `rank` FROM arena_team ORDER BY arenaTeamId ASC");

    if (!result)
    {
        sLog->outString(">> Loaded 0 arena teams. DB table `arena_team` is empty!");
        sLog->outString();
        return;
    }

    uint32 count = 0;
    do
    {
        ArenaTeam* newArenaTeam = new ArenaTeam;
        if (!newArenaTeam->LoadArenaTeamFromDB(result))
        {
            delete newArenaTeam;
            continue;
        }

        AddArenaTeam(newArenaTeam);
        sWorld->UpdateGlobalPlayerArenaTeam(GUID_LOPART(newArenaTeam->GetCaptain()), newArenaTeam->GetSlot(), newArenaTeam->GetId());

        ++count;
    } while (result->NextRow());

    sLog->outString(">> Loaded %u arena teams in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}

void ArenaTeamMgr::DistributeArenaPoints()
{
    // Used to distribute arena points based on last week's stats
    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_START);

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_ONLINE_START);

    // Temporary structure for storing maximum points to add values for all players
    std::map<uint32, uint32> PlayerPoints;

    // At first update all points for all team members
    for (ArenaTeamContainer::iterator teamItr = GetArenaTeamMapBegin(); teamItr != GetArenaTeamMapEnd(); ++teamItr)
        if (ArenaTeam* at = teamItr->second)
            at->UpdateArenaPointsHelper(PlayerPoints);

    auto trans = CharacterDatabase.BeginTransaction();

    PreparedStatement<CharacterDatabaseConnection>* stmt;

    // Cycle that gives points to all players
    for (std::map<uint32, uint32>::iterator playerItr = PlayerPoints.begin(); playerItr != PlayerPoints.end(); ++playerItr)
    {
        // Add points to player if online
        if (Player* player = ObjectAccessor::FindPlayer(playerItr->first))
            player->ModifyArenaPoints(playerItr->second, trans);
        else // Update database
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_ARENA_POINTS);
            stmt->setUInt32(0, playerItr->second);
            stmt->setUInt32(1, playerItr->first);
            trans->Append(stmt);
        }
    }

    CharacterDatabase.CommitTransaction(trans);

    PlayerPoints.clear();

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_ONLINE_END);

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_TEAM_START);
    for (ArenaTeamContainer::iterator titr = GetArenaTeamMapBegin(); titr != GetArenaTeamMapEnd(); ++titr)
    {
        if (ArenaTeam* at = titr->second)
        {
            at->FinishWeek();
            at->SaveToDB();
            at->NotifyStatsChanged();
        }
    }

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_TEAM_END);

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_END);
}
