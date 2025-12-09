

#ifndef SUNWELLCORE_ARENATEAM_H
#define SUNWELLCORE_ARENATEAM_H

#include "QueryResult.h"
#include "Map.h"
#include <list>
#include <map>

class WorldSession;
class WorldPacket;
class Player;
class Group;

enum ArenaTeamCommandTypes
{
    ERR_ARENA_TEAM_CREATE_S  = 0x00,
    ERR_ARENA_TEAM_INVITE_SS = 0x01,
    ERR_ARENA_TEAM_QUIT_S    = 0x03,
    ERR_ARENA_TEAM_FOUNDER_S = 0x0E
};

enum ArenaTeamCommandErrors
{
    ERR_ARENA_TEAM_INTERNAL              = 0x01,
    ERR_ALREADY_IN_ARENA_TEAM            = 0x02,
    ERR_ALREADY_IN_ARENA_TEAM_S          = 0x03,
    ERR_INVITED_TO_ARENA_TEAM            = 0x04,
    ERR_ALREADY_INVITED_TO_ARENA_TEAM_S  = 0x05,
    ERR_ARENA_TEAM_NAME_INVALID          = 0x06,
    ERR_ARENA_TEAM_NAME_EXISTS_S         = 0x07,
    ERR_ARENA_TEAM_LEADER_LEAVE_S        = 0x08,
    ERR_ARENA_TEAM_PERMISSIONS           = 0x08,
    ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM    = 0x09,
    ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS = 0x0A,
    ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S    = 0x0B,
    ERR_ARENA_TEAM_NOT_ALLIED            = 0x0C,
    ERR_ARENA_TEAM_IGNORING_YOU_S        = 0x13,
    ERR_ARENA_TEAM_TARGET_TOO_LOW_S      = 0x15,
    ERR_ARENA_TEAM_TARGET_TOO_HIGH_S     = 0x16,
    ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S    = 0x17,
    ERR_ARENA_TEAM_NOT_FOUND             = 0x1B,
    ERR_ARENA_TEAMS_LOCKED               = 0x1E
};

enum ArenaTeamEvents
{
    ERR_ARENA_TEAM_JOIN_SS            = 3, // player name + arena team name
    ERR_ARENA_TEAM_LEAVE_SS           = 4, // player name + arena team name
    ERR_ARENA_TEAM_REMOVE_SSS         = 5, // player name + arena team name + captain name
    ERR_ARENA_TEAM_LEADER_IS_SS       = 6, // player name + arena team name
    ERR_ARENA_TEAM_LEADER_CHANGED_SSS = 7, // old captain + new captain + arena team name
    ERR_ARENA_TEAM_DISBANDED_S        = 8  // captain name + arena team name
};

/*
need info how to send these ones:
ERR_ARENA_TEAM_YOU_JOIN_S - client show it automatically when accept invite
ERR_ARENA_TEAM_TARGET_TOO_LOW_S
ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S
ERR_ARENA_TEAM_LEVEL_TOO_LOW_I
*/

enum ArenaTeamTypes
{
    ARENA_TEAM_2v2          = 2,
    ARENA_TEAM_3v3          = 3,
    ARENA_TEAM_SOLO_QUEUE   = 5,
    ARENA_TEAM_MAX
};

struct ArenaTeamStats
{
    uint16 Rating;
    uint16 WeekGames;
    uint16 WeekWins;
    uint16 SeasonGames;
    uint16 SeasonWins;
    uint32 Rank;
    uint16 MatchMakerRating;
    uint16 MaxMMR;

    void ModifyRating(Player* player, int32 mod, uint32 type);
    void ModifyMatchmakerRating(int32 mod, uint32 type);
};

#define MAX_ARENA_SLOT 3                                    // 0..2 slots

class ArenaTeam
{
public:
    ArenaTeam();
    ~ArenaTeam();

    bool Create(Player* player, uint8 type);
    void Disband();

    uint32 GetId() const { return TeamId; }
    uint32 GetType() const { return Type; }
    uint8 GetSlot() const { return GetSlotByType(GetType()); }
    static uint8 GetSlotByType(uint32 type);

    uint64 GetCaptain() const { return CaptainGuid; }
    std::string GetName() const { return TeamName; }
    std::string GetCharName() const { return CharName; }
    ArenaTeamStats const& GetStats() const { return Stats; }

    uint32 GetRating() const { return Stats.Rating; }
    uint32 GetMatchMakerRating() const { return Stats.MatchMakerRating; }

    bool IsFighting() const;

    bool LoadArenaTeamFromDB(QueryResult arenaTeamDataResult);
    void SaveToDB();

    void BroadcastPacket(WorldPacket* packet);
    void BroadcastEvent(ArenaTeamEvents event, uint64 guid, uint8 strCount, std::string const& str1, std::string const& str2, std::string const& str3);
    void NotifyStatsChanged();

    void MassInviteToEvent(WorldSession* session);

    void Roster(WorldSession* session);
    void Query(WorldSession* session);
    void SendStats(WorldSession* session);
    void Inspect(WorldSession* session, uint64 guid);

    uint32 GetPoints(uint32 MemberRating);

    static int32 GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won);
    static int32 GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won);
    static float GetChanceAgainst(uint32 ownRating, uint32 opponentRating);

    int32 MemberWon(Player* player, uint32 rating, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange);
    int32 MemberLost(Player* player, uint32 rating, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange = -12);
    int32 OfflineMemberLost(uint64 guid, uint32 rating, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange = -12);

    void ForcePersonalRatingChange(Player* player, uint32 val, uint32 arenaType);

    void UpdateArenaPointsHelper(std::map<uint32, uint32>& PlayerPoints);

    void FinishWeek();
    void FinishGame(int32 mod, const Map* bgMap);

    void SetLastDPSList(std::set<uint64> const& list);
    std::set<uint64> const& GetLastDPSList() const;

protected:
    uint32 TeamId;
    uint8 Type;
    uint8 CharClass;
    std::string TeamName;
    std::string CharName;
    uint64 CaptainGuid;

    uint32 BackgroundColor; // ARGB format
    uint8 EmblemStyle;      // icon id
    uint32 EmblemColor;     // ARGB format
    uint8 BorderStyle;      // border image id
    uint32 BorderColor;     // ARGB format

    ArenaTeamStats Stats;

private:
    std::set<uint64> LastDPSList; // Only for SoloQueue
};
#endif
