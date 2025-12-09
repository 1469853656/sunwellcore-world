

/* ScriptData
Name: account_commandscript
%Complete: 100
Comment: All account related commands
Category: commandscripts
EndScriptData */

#include "AccountMgr.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"

class account_commandscript : public CommandScript
{
public:
    account_commandscript()
        : CommandScript("account_commandscript")
    {
    }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> commandTable = {
            { "account", SEC_PLAYER, CMD_INGAME, &HandleAccountCommand, "" },
        };
        return commandTable;
    }

    static bool HandleAccountCommand(ChatHandler* handler, char const* /*args*/)
    {
        AccountTypes gmLevel = handler->GetSession()->GetSecurity();
        handler->PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmLevel));
        return true;
    }
};

void AddSC_account_commandscript()
{
    new account_commandscript();
}
