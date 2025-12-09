#include "Language.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "World.h"
#include "ArenaTeamMgr.h"
#include "Player.h"
#include "SoloQueue.h"
#include "BattlegroundMgr.h"

class soloq_commandscript : public CommandScript
{
public:
    soloq_commandscript() : CommandScript("soloq_commandscript")
    {
    }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> soloqCheckCommandTable =
        {
            { "team",   SEC_GAMEMASTER, CMD_INGAME,  &HandleSoloqCheckTeamCommand,   "" },
            { "queue",  SEC_GAMEMASTER, CMD_INGAME,  &HandleSoloqCheckQueueCommand,  "" }
        };

        static std::vector<ChatCommand> soloqCommandTable =
        {
            { "check",  SEC_GAMEMASTER, CMD_INGAME, nullptr, "", soloqCheckCommandTable },
            { "stats",  SEC_GAMEMASTER, CMD_INGAME,  &HandleSoloqStatsCommand,       "" },
            { "join",   SEC_PLAYER,     CMD_INGAME,  &HandleSoloqJoinCommand,        "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "soloq",  SEC_PLAYER,     CMD_INGAME,  nullptr,     "", soloqCommandTable }
        };

        return commandTable;
    }

    static bool HandleSoloqStatsCommand(ChatHandler* handler, char const* /*args*/)
    {
        sSoloQueueMgr->PrintSoloQueueStats(handler);
        return true;
    }

    static bool HandleSoloqCheckTeamCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->getSelectedPlayer();
        if (!player)
        {
            handler->SendSysMessage("Player not found");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 arenaTeamId = player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE));
        if (!arenaTeamId)
        {
            handler->PSendSysMessage("Player %s doesn't have soloq team.", player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(arenaTeamId);
        if (!team)
        {
            handler->PSendSysMessage("Player %s doesn't have soloq team.", player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string specialization[3] = { "DPS", "Heal", "Tank" };
        handler->SendSysMessage("------------------------");
        handler->PSendSysMessage("|cffff9933SoloQueue team stats for player: |cFFFFD700%s|r", player->GetName().c_str());
        handler->PSendSysMessage("|cffff9933Talents Specialization: |cFFFFD700%s|r", specialization[SoloQueueMgr::GetTalentsSpecialization(player)].c_str());
        handler->PSendSysMessage("|cffff9933Week Games: |cFFFFD700%u|r", team->GetStats().WeekGames);
        handler->PSendSysMessage("|cffff9933Week Wins: |cFFFFD700%u|r", team->GetStats().WeekWins);
        handler->PSendSysMessage("|cffff9933Season Games: |cFFFFD700%u|r", team->GetStats().SeasonGames);
        handler->PSendSysMessage("|cffff9933Season Wins: |cFFFFD700%u|r", team->GetStats().SeasonWins);
        handler->PSendSysMessage("|cffff9933Team Rating: |cFFFFD700%u|r", team->GetStats().Rating);
        handler->PSendSysMessage("|cffff9933MMR: |cFFFFD700%u|r", team->GetStats().MatchMakerRating);
        handler->SendSysMessage("------------------------");

        return true;
    }

    static bool HandleSoloqCheckQueueCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->getSelectedPlayer();
        if (!player)
        {
            handler->SendSysMessage("Player not found");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 arenaTeamId = player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE));
        if (!arenaTeamId)
        {
            handler->PSendSysMessage("Player %s doesn't have soloq team.", player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(arenaTeamId);
        if (!team)
        {
            handler->PSendSysMessage("Player %s doesn't have soloq team.", player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        SoloQueue const* soloQueue = sSoloQueueMgr->GetSoloQueue(player);
        if (!soloQueue)
        {
            handler->PSendSysMessage("Player %s doesn't have soloq data.", player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 queueIndex = player->GetBattlegroundQueueIndex(BATTLEGROUND_QUEUE_SOLO_QUEUE);
        if (queueIndex == PLAYER_MAX_BATTLEGROUND_QUEUES)
        {
            handler->PSendSysMessage("Player %s is not in soloqueue.", player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        bool isInPreparationQueue = sSoloQueueMgr->IsInPreparationQueue(player);
        uint32 secsInQueue = getMSTimeDiff(soloQueue->joinTime, World::GetGameTimeMS()) / 1000;

        handler->SendSysMessage("------------------------");
        handler->PSendSysMessage("|cffff9933SoloQueue info for player: |cFFFFD700%s|r", player->GetName().c_str());
        handler->PSendSysMessage("|cffff9933MMR: |cFFFFD700%u|r", team->GetStats().MatchMakerRating);
        handler->PSendSysMessage("|cffff9933MMR tolerance: |cFFFFD700+/- %u|r", soloQueue->mmrTolerance);
        handler->PSendSysMessage("|cffff9933Time in queue: |cFFFFD700%s|r", secsToTimeString(secsInQueue).c_str());
        handler->PSendSysMessage("|cffff9933Is in preparation queue: |cFFFFD700%s|r", (isInPreparationQueue ? "yes" : "no"));

        if (!isInPreparationQueue)
        {
            BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_SOLO_QUEUE);
            GroupQueueInfo ginfo;
            if (bgQueue.GetPlayerGroupInfoData(player->GetGUID(), &ginfo))
            {
                handler->SendSysMessage("------------------------");
                handler->PSendSysMessage("|cffff9933Avg MMR: |cFFFFD700%u|r", ginfo.ArenaMatchmakerRating);
                handler->PSendSysMessage("|cffff9933Avg Rating: |cFFFFD700%u|r", ginfo.ArenaTeamRating);
                handler->SendSysMessage("|cffff9933Players in formed team:|r");
                uint32 index = 0;
                for (auto const& iter : ginfo.Players)
                {
                    if (Player* teammate = ObjectAccessor::FindPlayerInOrOutOfWorld(iter))
                    {
                        if (ArenaTeam* team2 = sArenaTeamMgr->GetArenaTeamById(teammate->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_SOLO_QUEUE))))
                        {
                            handler->PSendSysMessage("|cffff9933%u. |cFFFFD700%s|cffff9933 (GUID: |cFFFFD700%u|cffff9933, MMR: |cFFFFD700%u,|cffff9933 Rating: |cFFFFD700%u)|r", (++index), teammate->GetName().c_str(), GUID_LOPART(teammate->GetGUID()), team2->GetStats().MatchMakerRating, team2->GetStats().Rating);
                        }
                    }
                }
            }
        }

        handler->SendSysMessage("------------------------");
        return true;
    }

    static bool HandleSoloqJoinCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
        {
            handler->SendSysMessage("Player not found");
            handler->SetSentErrorMessage(true);
            return false;
        }

        WorldPacket data;
        data << uint8(2);
        player->GetSession()->HandleBattlemasterJoinArena(data);

        return true;
    }
};

void AddSC_soloq_commandscript()
{
    new soloq_commandscript();
}
