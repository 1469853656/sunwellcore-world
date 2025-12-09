

#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Group.h"
#include "Player.h"

void GetPlayerInfo(ChatHandler* handler, Player* player)
{
    if (!player)
        return;

    uint64 guid                 = player->GetGUID();
    lfg::LfgDungeonSet dungeons = sLFGMgr->GetSelectedDungeons(guid);

    /*std::string const& state = lfg::GetStateString(sLFGMgr->GetState(guid));
    handler->PSendSysMessage(LANG_LFG_PLAYER_INFO, player->GetName().c_str(),
        state.c_str(), uint8(dungeons.size()), lfg::ConcatenateDungeons(dungeons).c_str(),
        lfg::GetRolesString(sLFGMgr->GetRoles(guid)).c_str(), sLFGMgr->GetComment(guid).c_str());*/
}

class lfg_commandscript : public CommandScript
{
public:
    lfg_commandscript()
        : CommandScript("lfg_commandscript")
    {
    }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> lfgCommandTable = 
        {
            { "player", SEC_GAMEMASTER, CMD_INGAME, &HandleLfgPlayerInfoCommand, "" },
            { "group", SEC_GAMEMASTER, CMD_INGAME, &HandleLfgGroupInfoCommand, "" },
            { "queue", SEC_GAMEMASTER, CMD_INGAME, &HandleLfgQueueInfoCommand, "" },
            { "clean", SEC_ADMINISTRATOR, CMD_INGAME, &HandleLfgCleanCommand, "" },
            { "options", SEC_ADMINISTRATOR, CMD_INGAME, &HandleLfgOptionsCommand, "" }
        };

        static std::vector<ChatCommand> commandTable = 
        {
            { "lfg", SEC_GAMEMASTER, CMD_INGAME, nullptr, "", lfgCommandTable },
            { "cta", SEC_PLAYER,     CMD_INGAME, &HandleCallToArmsMessageCommand, "" },
        };
        return commandTable;
    }

    static bool HandleLfgPlayerInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* target = nullptr;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, nullptr, &playerName))
            return false;

        GetPlayerInfo(handler, target);
        return true;
    }

    static bool HandleLfgGroupInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* target = nullptr;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, nullptr, &playerName))
            return false;

        Group* grp = target->GetGroup();
        if (!grp)
        {
            handler->PSendSysMessage(LANG_LFG_NOT_IN_GROUP, playerName.c_str());
            return true;
        }
        /*
        uint64 guid = grp->GetGUID();
        std::string const& state = lfg::GetStateString(sLFGMgr->GetState(guid));
        handler->PSendSysMessage(LANG_LFG_GROUP_INFO, grp->isLFGGroup(),
            state.c_str(), sLFGMgr->GetDungeon(guid));

        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
            GetPlayerInfo(handler, itr->GetSource());
        */
        return true;
    }

    static bool HandleLfgOptionsCommand(ChatHandler* handler, char const* args)
    {
        int32 options = -1;
        if (char* str = strtok((char*)args, " "))
        {
            int32 tmp = atoi(str);
            if (tmp > -1)
                options = tmp;
        }

        if (options != -1)
        {
            sLFGMgr->SetOptions(options);
            handler->PSendSysMessage(LANG_LFG_OPTIONS_CHANGED);
        }
        handler->PSendSysMessage(LANG_LFG_OPTIONS, sLFGMgr->GetOptions());
        return true;
    }

    static bool HandleLfgQueueInfoCommand(ChatHandler* handler, char const* args)
    {
        return true;
    }

    static bool HandleLfgCleanCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage(LANG_LFG_CLEAN);
        sLFGMgr->Clean();
        return true;
    }

    static bool HandleCallToArmsMessageCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string param = (char*)args;
        if (param == "on")
        {
            player->SetDisableCallToArmsMessage(false);
            handler->SendSysMessage("Call To Arms messages are now enabled!");
        }
        else if (param == "off")
        {
            player->SetDisableCallToArmsMessage(true);
            handler->SendSysMessage("Call To Arms messages are now disabled!");
        }
        else
            return false;

        return true;
    }
};

void AddSC_lfg_commandscript()
{
    new lfg_commandscript();
}
