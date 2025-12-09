

/* ScriptData
Name: custom_commandscript
%Complete: 100
Comment: All account related commands
Category: commandscripts
EndScriptData */

#include "AccountMgr.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "StringFormat.h"
#include "DatabaseLog.h"

class custom_commandscript : public CommandScript
{
public:
    custom_commandscript()
        : CommandScript("custom_commandscript")
    {
    }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> xpCommandTable =
        {
            { "set",                  SEC_PLAYER,  CMD_INGAME, &HandleXPSetCommand,                "" },
            { "reset",               SEC_PLAYER,  CMD_INGAME, &HandleXPResetCommand,              "" },
            { "display",            SEC_PLAYER,  CMD_INGAME, &HandleXPDisplayCommand,           "" },
        };

        static std::vector<ChatCommand> presetSetGroupCommandTable =
        {
            { "add",                  SEC_GAMEMASTER,  CMD_CLI, &HandlePresetSetGroupAddCommand,      "" },
            { "remove",               SEC_GAMEMASTER,  CMD_CLI, &HandlePresetSetGroupRemoveCommand,   "" },
            { "override",            SEC_GAMEMASTER,  CMD_CLI, &HandlePresetSetGroupOverrideCommand,  "" },
        };

        static std::vector<ChatCommand> presetGroupCommandTable =
        {
            { "list",              SEC_GAMEMASTER,  CMD_CLI, &HandlePresetGroupListCommand,      "" },
            { "add",               SEC_GAMEMASTER,  CMD_CLI, &HandlePresetGroupAddCommand,   "" },
            { "remove",            SEC_GAMEMASTER,  CMD_CLI, &HandlePresetGroupRemoveCommand,  "" },
        };

        static std::vector<ChatCommand> presetNPCCommandTable =
        {
            { "display",                SEC_GAMEMASTER,  CMD_INGAME, &HandlePresetNPCDisplayCommand,      "" },
            { "add",                    SEC_GAMEMASTER,  CMD_CLI, &HandlePresetNPCAddCommand,   "" },
            { "remove",                 SEC_GAMEMASTER,  CMD_CLI, &HandlePresetNPCRemoveCommand,  "" },
            { "override",               SEC_GAMEMASTER,  CMD_CLI, &HandlePresetNPCOverrideCommand,  "" },
        };

        static std::vector<ChatCommand> premadeCommandTable =
        {
            { "save",            SEC_GAMEMASTER,  CMD_INGAME, &HandlePremadeSaveCommand,        "" },
            { "equip",           SEC_GAMEMASTER,  CMD_INGAME, &HandlePremadeEquipCommand,       "" },
            { "delete",          SEC_GAMEMASTER,  CMD_INGAME, &HandlePremadeDeleteCommand,      "" },
            { "list",            SEC_GAMEMASTER,  CMD_CLI,    &HandlePremadeListCommand,        "" },
            { "details",         SEC_GAMEMASTER,  CMD_CLI,    &HandlePremadeDetailsCommand,     "" },
            { "rename",          SEC_GAMEMASTER,  CMD_CLI,    &HandlePremadeRenameCommand,     "" },
            { "setgroup",          SEC_GAMEMASTER, CMD_INGAME, nullptr,        "", presetSetGroupCommandTable },
            { "group",          SEC_GAMEMASTER, CMD_INGAME, nullptr,        "", presetGroupCommandTable },
            { "npc",          SEC_GAMEMASTER, CMD_INGAME, nullptr,        "", presetNPCCommandTable },
        };

        static std::vector<ChatCommand> creloadCommandTable =
        {
            { "premade_sets",            SEC_ADMINISTRATOR,  CMD_INGAME, &HandleCReloadPremadeSetsCommand,        "" },
        };

        static std::vector<ChatCommand> commandTable = {
            { "service", SEC_ADMINISTRATOR, CMD_CLI, &HandleServiceCommand, "" },
            { "pvpinfo", SEC_PLAYER, CMD_INGAME, HandlePvPInfoCommand, "" },
            { "dodge", SEC_PLAYER, CMD_INGAME, HandleDodgeModeCommand, "" },
            { "goldban", SEC_MODERATOR, CMD_INGAME, &HandleGoldBanCommand, "" },
            { "speedban", SEC_MODERATOR, CMD_INGAME, &HandleSpeedBanCommand, "" },
            { "englishmute", SEC_MODERATOR, CMD_INGAME, &HandleEnglishMuteCommand, "" },
            { "iccreset", SEC_MODERATOR, CMD_INGAME, HandleICCResetCommand, "" },
            { "bonusxp", SEC_ADMINISTRATOR, CMD_CLI, &HandleBonusXpCommand, "" },
            { "ninjains", SEC_GAMEMASTER, CMD_INGAME, &HandleInsertNinjaCommand, "" },
            { "ninjalist", SEC_PLAYER, CMD_INGAME, &HandleSelectNinjaCommand, "" },
            { "ninjadel", SEC_GAMEMASTER, CMD_INGAME, &HandleDeleteNinjaCommand, "" },
            { "xp",         SEC_PLAYER, CMD_INGAME, nullptr,        "", xpCommandTable },
            { "portgm", SEC_PLAYER, CMD_INGAME, &HandlePortGMCommand, "" },
            { "hcmsg", SEC_PLAYER, CMD_INGAME, &HandleHCMSGCommand, "" },
            // Multispec
            { "multispec", SEC_ADMINISTRATOR, CMD_INGAME, &HandleMultispec, "" },
            // Mute History
            { "mutehistory", SEC_ADMINISTRATOR, CMD_INGAME, &HandleMuteHistoryCommand, "" },
            { "boost", SEC_PLAYER, CMD_INGAME, &HandleBoostCommand, "" },
            { "preset",         SEC_GAMEMASTER, CMD_INGAME, nullptr,        "", premadeCommandTable },
            { "creload",         SEC_ADMINISTRATOR, CMD_INGAME, nullptr,        "", creloadCommandTable },
            { "spellqueue", SEC_PLAYER, CMD_INGAME, &HandleSpellQueueCommand, "" },
        };

        return commandTable;
    }

    static bool HandleServiceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        /*
        // Account Name
        char* aname = strtok((char*)args, " ");
        if (!aname)
            return false;

        std::string accName = aname;
        std::string serviceName; // To display on output command and log

        // Premium Type
        char* typeStr = strtok(nullptr, " ");
        if (!typeStr || !atoi(typeStr) && typeStr != "0")
            return false;

        // duration of premium service
        char* durationStr = strtok(nullptr, "");
        if (!durationStr)
            return false;

        // convert accountname to account ID
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
        stmt->setString(0, accName);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (!result)
            return false;

        uint32 accId  = 0;
        Field* fields = result->Fetch();
        accId         = fields[0].GetUInt32();

        WorldSession* session = sWorld->FindSession(accId);

        uint8 serviceType = atoi(typeStr);
        switch (serviceType)
        {
            case SERVICE_TELEPORT_INPUT: // -> SERVICE_TELEPORT
                serviceType = SERVICE_TELEPORT;
                serviceName = "SERVICE_TELEPORT";
                break;
            case SERVICE_NO_RESSURECTION_SICKNESS:
                serviceName = "SERVICE_NO_RESSURECTION_SICKNESS";
                break;
            case SERVICE_EXP_BOOST:
                serviceName = "SERVICE_EXP_BOOST";
                break;
            case SERVICE_NO_DURABILITY_LOSS:
                serviceName = "SERVICE_NO_DURABILITY_LOSS";
                break;
            case SERVICE_INSTANT_FLIGHT_PATHS:
                serviceName = "SERVICE_INSTANT_FLIGHT_PATHS";
                break;
            case SERVICE_EXP_BOOST_X4:
                serviceName = "SERVICE_DOUBLE_EXP_BOOST";
                break;
        }

        // if input of time is less than 0 that means we wanna delete service from account
        if (atoi(durationStr) < 0)
        {
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_PREMIUM_ID);
            stmt->setUInt32(0, accId);
            stmt->setUInt32(1, realmID);
            stmt->setUInt8(2, serviceType);
            LoginDatabase.Execute(stmt);

            if (session)
            {
                session->SetPremiumServiceTime(PremiumServiceTypes(serviceType), time_t(0));
                ChatHandler(session).PSendSysMessage("'%s' service has been disabled on your account!", serviceName.c_str());
            }

            handler->PSendSysMessage("You have disabled service '%s' on account %s", serviceName.c_str(), accName.c_str());
            sLog->outPremium("Service '%s' has been disabled on account %s", serviceName.c_str(), accName.c_str());
            return true;
        }

        uint32 extraTime   = TimeStringToSecs(durationStr); // duration of premium service
        time_t premiumTime = time(nullptr) + extraTime;

        // Query check for previous premium service, if there is result stack the time of premium service
        auto stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PREMIUM_TIME_BY_ID);
        stmt2->setUInt32(0, accId);
        stmt2->setUInt32(1, realmID);
        stmt2->setUInt8(2, serviceType);
        PreparedQueryResult timeResult = LoginDatabase.Query(stmt2);
        if (timeResult)
        {
            Field* timeFields = timeResult->Fetch();
            premiumTime       = time_t(timeFields[1].GetUInt32()) + extraTime;
        }

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACCOUNT_PREMIUM);
        stmt->setUInt32(0, accId);
        stmt->setUInt32(1, realmID);
        stmt->setUInt8(2, serviceType);
        stmt->setUInt32(3, time(nullptr));
        stmt->setUInt32(4, premiumTime);
        LoginDatabase.Execute(stmt);

        if (session)
        {
            session->SetPremiumServiceTime(PremiumServiceTypes(serviceType), premiumTime);

            char buff[20];
            strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&premiumTime));

            std::ostringstream ss;
            ss << serviceName << " service";
            if (timeResult)
                ss << " has been extended by ";
            else
                ss << " has been activated for ";

            ss << secsToTimeString(extraTime) << ".";
            ss << "The service ends at " << buff;

            ChatHandler(session).PSendSysMessage(ss.str().c_str());
        }

        handler->PSendSysMessage("You have enabled service '%s' on account %s for %s", serviceName.c_str(), accName.c_str(), secsToTimeString(extraTime, true).c_str());
        sLog->outPremium("Service '%s' has been enabled on account %s for %s", serviceName.c_str(), accName.c_str(), secsToTimeString(extraTime, true).c_str());
        */
        return true;
    }

    static bool HandleAccountCommand(ChatHandler* handler, char const* /*args*/)
    {
        AccountTypes gmLevel = handler->GetSession()->GetSecurity();
        handler->PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmLevel));
        return true;
    }

    static bool HandlePvPInfoCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        if (!player->PvPAnnounces())
        {
            player->SetPvPAnnounces(true);
            handler->PSendSysMessage(LANG_PVP_ANNOUNCEMENTS_ON);
            handler->SetSentErrorMessage(true);
        }
        else
        {
            player->SetPvPAnnounces(false);
            handler->PSendSysMessage(LANG_PVP_ANNOUNCEMENTS_OFF);
            handler->SetSentErrorMessage(true);
        }

        return true;
    }

    static bool HandleDodgeModeCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        if (player->GetMaxPersonalArenaRatingRequirement(0) < 2000)
        {
            handler->PSendSysMessage(LANG_DODGE_NO_RATING);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!player->IsInDodgeMode())
        {
            player->SetDodgeMode(true);
            handler->PSendSysMessage(LANG_DODGE_ON);
            handler->SetSentErrorMessage(true);
        }
        else
        {
            player->SetDodgeMode(false);
            handler->PSendSysMessage(LANG_DODGE_OFF);
            handler->SetSentErrorMessage(true);
        }

        return true;
    }

    // GoldBan!
    static bool HandleGoldBanCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        std::string accountName;

        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        if (target == handler->GetSession()->GetPlayer())
            return false;

        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, 0))
                return false;

            if (AccountMgr::GetName(accountId, accountName))
                sWorld->BanAccount(BAN_ACCOUNT, accountName.c_str(), "-1", "Gold selling.", handler->GetSession() ? handler->GetSession()->GetPlayerName() : "");
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(nullptr, targetGuid))
                return false;

            if (AccountMgr::GetName(sObjectMgr->GetPlayerAccountIdByPlayerName(targetName.c_str()), accountName))
                sWorld->BanAccount(BAN_ACCOUNT, accountName.c_str(), "-1", "Gold selling.", handler->GetSession() ? handler->GetSession()->GetPlayerName() : "");
        }

        sLog->outBan("Game Master: [%s] has banned player: [%s] for gold selling. (30 days)", handler->GetSession()->GetPlayerName().c_str(), targetName.c_str());
        sWorld->SendGMText(LANG_GOLD_BAN, handler->GetSession()->GetPlayerName().c_str(), targetName.c_str());

        return true;
    }

    // SpeedBan!
    static bool HandleSpeedBanCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        std::string accountName;

        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        if (target == handler->GetSession()->GetPlayer())
            return false;

        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, 0))
                return false;

            if (AccountMgr::GetName(target->GetSession()->GetAccountId(), accountName))
                sWorld->BanAccount(BAN_ACCOUNT, accountName.c_str(), "7d", "Speed Hack.", handler->GetSession() ? handler->GetSession()->GetPlayerName() : "");
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(nullptr, targetGuid))
                return false;

            if (AccountMgr::GetName(accountId, accountName))
                sWorld->BanAccount(BAN_ACCOUNT, accountName.c_str(), "7d", "Speed Hack.", handler->GetSession() ? handler->GetSession()->GetPlayerName() : "");
        }

        sLog->outBan("Game Master: [%s] has banned player: [%s] for speed hack. (7 days)", handler->GetSession()->GetPlayerName().c_str(), targetName.c_str());
        sWorld->SendGMText(LANG_SPEED_BAN, handler->GetSession()->GetPlayerName().c_str(), targetName.c_str());

        return true;
    }

    // EnglishMute!
    static bool HandleEnglishMuteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* nameStr = strtok((char*)args, " ");
        if (!nameStr)
            return false;

        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget(nameStr, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        // find only player from same account if any
        if (!target)
            if (WorldSession* session = sWorld->FindSession(accountId))
                target = session->GetPlayer();

        // must have strong lesser security level
        if (handler->HasLowerSecurity(target, targetGuid, true))
            return false;

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
        std::string muteBy      = "";
        if (handler->GetSession())
            muteBy = handler->GetSession()->GetPlayerName();
        else
            muteBy = "Console";

        std::string reason = "Inappropriate language, speak English!";

        if (target)
        {
            // Target is online, mute will be in effect right away.
            int64 muteTime                   = time(nullptr) + 30 * MINUTE;
            target->GetSession()->m_muteTime = muteTime;
            stmt->setInt64(0, muteTime);
            ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, 30, muteBy.c_str(), reason.c_str());
        }
        else
        {
            // Target is offline, mute will be in effect starting from the next login.
            int32 muteTime = -int32(30 * MINUTE);
            stmt->setInt64(0, muteTime);
        }

        stmt->setString(1, reason.c_str());
        stmt->setString(2, muteBy.c_str());
        stmt->setUInt32(3, accountId);
        LoginDatabase.Execute(stmt);
        std::string nameLink = handler->playerLink(targetName);

        if (sCustomMgr->GetBoolConfig(CONFIG_MUTE_HISTORY_ENABLED))
            DBLog::OutMute(accountId, targetName, reason, muteBy, 30);

        sWorld->SendGMText(target ? LANG_YOU_DISABLE_CHAT : LANG_COMMAND_DISABLE_CHAT_DELAYED, (handler->GetSession() ? handler->GetSession()->GetPlayerName().c_str() : "Console"), nameLink.c_str(), 30, reason.c_str());

        return true;
    }

    static bool HandleICCResetCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player  = handler->GetSession()->GetPlayer();
        uint32 iccMapID = 631;

        if (!player)
            return false;

        if (player->GetMap()->Instanceable())
        {
            handler->SendSysMessage("You can't do that here.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
            sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUIDLow(), iccMapID, Difficulty(i), true, player);

        handler->SendSysMessage("ICC has been successfully reset.");
        return true;
    }

    // Event x4
    static bool HandleBonusXpCommand(ChatHandler* handler, char const* args)
    {
        /*
        if (!*args)
            return false;

        // Account Name
        char* aname = strtok((char*)args, " ");
        if (!aname)
            return false;

        std::string accName = aname;
        std::string serviceName; // To display on output command and log

        // convert accountname to account ID
        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
        stmt->setString(0, accName);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (!result)
            return false;

        uint32 accId  = 0;
        Field* fields = result->Fetch();
        accId         = fields[0].GetUInt32();

        uint32 premiumTime = 4 * DAY; // duration of premium service
        time_t extraTime;             // is time left of previous active service, if no result then null

        // Query check for previous premium service, if there is result stack the time of premium service
        auto stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PREMIUM_TIME_BY_ID);
        stmt2->setUInt32(0, accId);
        stmt2->setUInt32(1, realmID);
        stmt2->setUInt8(2, SERVICE_EXP_BOOST_X4);
        PreparedQueryResult timeResult = LoginDatabase.Query(stmt2);

        if (timeResult)
        {
            Field* timeFields = timeResult->Fetch();
            extraTime         = time_t(timeFields[1].GetUInt32());
            premiumTime       = premiumTime + extraTime;
            handler->PSendSysMessage("Warning! Account: %s already got X4 bonus.", accName.c_str());
        }
        else
            premiumTime = time(nullptr) + premiumTime;

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACCOUNT_PREMIUM);
        stmt->setUInt32(0, accId);
        stmt->setUInt32(1, realmID);
        stmt->setUInt8(2, SERVICE_EXP_BOOST_X4);
        stmt->setUInt32(3, time(nullptr));
        stmt->setUInt32(4, premiumTime);
        LoginDatabase.Execute(stmt);
        serviceName = "SERVICE_EXP_BOOST_X4";

        handler->PSendSysMessage("You have enabled service '%s' on account %s for 4 days.", serviceName.c_str(), accName.c_str());
        sLog->outPremium("Service '%s' has been enabled on account %s for 4 days.", serviceName.c_str(), accName.c_str());
        */
        return true;
    }

    static PreparedQueryResult SelectNinjaLooter(uint32 loGuid)
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NINJA_LOOTER_PER_GUID);
        stmt->setUInt32(0, loGuid);

        return std::move(CharacterDatabase.Query(stmt));
    };

    static bool HandleInsertNinjaCommand(ChatHandler* handler, char const* args)
    {
        if (!sWorld->getBoolConfig(CONFIG_NINJA_LOOTER_LIST))
        {
            handler->SendSysMessage("Feature disabled.");
            return true;
        }

        char* nameStr;
        char* postStr;
        handler->extractOptFirstArg((char*)args, &nameStr, &postStr);

        if (!postStr || (!atoi(postStr)))
            return false;

        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget(nameStr, nullptr, &targetGuid, &targetName))
            return false;

        uint32 loGuid = GUID_LOPART(targetGuid);
        uint32 postId = atoi(postStr);

        if (PreparedQueryResult result = SelectNinjaLooter(loGuid))
        {
            Field* fields = result->Fetch();

            uint32 postId = fields[2].GetUInt32();
            handler->PSendSysMessage("Player: %s (Guid: %u) has already been added to ninja looter list with forum report ID: %u", targetName.c_str(), loGuid, postId);
            return true;
        }

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_NINJA_LOOTER);
        stmt->setUInt32(0, loGuid);
        stmt->setString(1, targetName);
        stmt->setUInt32(2, postId);
        CharacterDatabase.DirectExecute(stmt);

        if (PreparedQueryResult result = SelectNinjaLooter(loGuid))
            handler->PSendSysMessage("Player: %s (Guid: %u) has been added to ninja looter list with forum report ID: %u", targetName.c_str(), loGuid, postId);
        else
            handler->SendSysMessage("Something went wrong! Probably wrong name or missing report Id?");

        return true;
    }

    static bool HandleDeleteNinjaCommand(ChatHandler* handler, char const* args)
    {
        if (!sWorld->getBoolConfig(CONFIG_NINJA_LOOTER_LIST))
        {
            handler->SendSysMessage("Feature disabled.");
            return true;
        }

        if (!*args)
            return false;

        char* input = strtok((char*)args, " ");
        if (!input || !atoi(input))
            return false;

        uint32 loGuid = GUID_LOPART(atoi(input));

        if (PreparedQueryResult result = SelectNinjaLooter(loGuid))
            handler->PSendSysMessage("Player GUID: %u going to be removed from the Ninja List.", loGuid);
        else
        {
            handler->PSendSysMessage("Cannot find player with GUID: %u on the list, maybe already removed?", loGuid);
            return true;
        }

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_NINJA_LOOTER);
        stmt->setUInt32(0, loGuid);
        CharacterDatabase.Execute(stmt);
        return true;
    }

    static bool HandleSelectNinjaCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!sWorld->getBoolConfig(CONFIG_NINJA_LOOTER_LIST))
        {
            handler->SendSysMessage("Feature disabled.");
            return true;
        }

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NINJA_LOOTER);

        handler->SendSysMessage("--- Ninja List begin ---");
        if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
        {
            do
            {
                Field* fields = result->Fetch();

                uint32 guid      = fields[0].GetUInt32();
                std::string name = fields[1].GetString();
                uint32 reportId  = fields[2].GetUInt32();

                //! Check for real player name
                sObjectMgr->GetPlayerNameByGUID(guid, name);

                WorldSession* session = handler->GetSession();
                if (session != nullptr && session->GetSecurity() > SEC_PLAYER)
                {
                    handler->PSendSysMessage("Player: %s (GUID: %u) is on Ninja List! Report ID: %u", name.c_str(), guid, reportId);
                }
                else
                {
                    handler->PSendSysMessage("Player: %s is on Ninja List! Report ID: %u", name.c_str(), reportId);
                }

            } while (result->NextRow());
        }

        handler->SendSysMessage("--- Ninja List end ---");

        return true;
    }
    
    static bool HandlePortGMCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!sCustomMgr->CheckRealmPhase(RealmPhase::REALM_PHASE_PTR))
        {
            handler->SendSysMessage("This is a PTR-only command.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        player->TeleportTo(1, 16226.299f, 16269.099f, 13.09f, 4.61f);
        return true;
    }

    static bool HandleHCMSGCommand(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        if (strcmp(tokens[0], "on") == 0)
        {
            player->SetDisplayHardcoreMessages(true);
            handler->SendSysMessage("Hardcore messages enabled!");
        }
        else if (strcmp(tokens[0], "off") == 0)
        {
            player->SetDisplayHardcoreMessages(false);
            handler->SendSysMessage("Hardcore messages disabled!");
        }
        else
            return false;

        return true;
    }

    static bool HandleMultispec(ChatHandler* handler, const char* args)
    {
        Player* player = handler->getSelectedPlayer();
        if (!player)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        auto const specValue = char_cast<uint32>(tokens[0]);
        if (!specValue)
            return false;

        auto const flagToSet = [specValue]() -> std::optional<MultipleSpecsFlags> 
        {
            switch (specValue.value())
            {
                case 0: return MULTIPLE_SPECS_FLAG_NONE;
                case 1: return MULTIPLE_SPECS_FLAG_GROUP_1;
                case 2: return MULTIPLE_SPECS_FLAG_GROUP_2;
                case 3: return MULTIPLE_SPECS_FLAG_GROUP_3;
                default:
                    return std::nullopt;
            }
        }();

        if (flagToSet)
        {
            if (CustomData* customData = sCustomMgr->GetCustomData(player->GetGUIDLow()))
            {
                if (flagToSet.value() == MULTIPLE_SPECS_FLAG_NONE)
                {
                    customData->MultipleSpecsFlag = 0;
                    handler->SendSysMessage("Specs reset!");
                }
                else
                {
                    customData->MultipleSpecsFlag |= flagToSet.value();
                    handler->PSendSysMessage("Spec %u enabled!", specValue.value());
                }
            }

            sCustomMgr->SaveCharacterCustomData(handler->GetSession()->GetPlayer()->GetGUIDLow(), false);
            return true;
        }

        handler->PSendSysMessage("Wrong value passed!", specValue.value());
        return false;
    }

    static bool HandleMuteHistoryCommand(ChatHandler* handler, const char* args)
    {
        if (!sCustomMgr->GetBoolConfig(CONFIG_MUTE_HISTORY_ENABLED))
        {
            handler->SendSysMessage("Mute History disabled!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        uint32 accountID = 0;
        std::string accountName;
        if (auto const accountIDOpt = char_cast<uint32>(tokens[0]); accountIDOpt.has_value())
        {
            accountID = accountIDOpt.value();
            AccountMgr::GetName(accountID, accountName);
            if (accountName.empty())
            {
                handler->SendFormattedSysMessage("Account with id {} not found!", accountID);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            accountName = tokens[0];
            if (!AccountMgr::normalizeString(accountName))
                return false;

            auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
            stmt->setString(0, accountName);
            PreparedQueryResult const result = LoginDatabase.Query(stmt);

            if (!result)
            {
                handler->SendFormattedSysMessage("Account with name {} not found!", accountName);
                handler->SetSentErrorMessage(true);
                return false;
            }

            Field const* fields = result->Fetch();
            accountID           = fields[0].GetUInt32();
        }

        handler->SendFormattedSysMessage("Displaying mute history for account {} (ID: {})...", accountName, accountID);
        handler->GetSession()->ScheduleMuteHistoryQuery(accountID);
        return true;
    }

    static bool HandleXPDisplayCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player const* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        handler->SendSysMessage(sCustomMgr->GetDetailedXPRateInfoForPlayer(player).c_str());
        return true;
    }

    static bool HandleXPSetCommand(ChatHandler* handler, char const* args)
    {
        Player const* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        Tokenizer tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        auto const xpRateOverride = char_cast<float>(tokens[0]);
        if (!xpRateOverride.has_value())
            return false;

        if (std::isnan(xpRateOverride.value()))
            return false;

        float newXPRate = xpRateOverride.value();
        static_cast<void>(sCustomMgr->VerifyAndCorrectXPRate(newXPRate, player->getLevel()));
        newXPRate = roundf(newXPRate * 100.0f) / 100.0f;

        sCustomMgr->SetSpecifiedXPRateForPlayer(player, newXPRate);
        handler->SendFormattedSysMessage("XP Rate changed to {}!", newXPRate);
        return true;
    }

    static bool HandleXPResetCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player const* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        float newXPRate = sCustomMgr->GetFloatConfig(CONFIG_BASE_XP_RATE);
        static_cast<void>(sCustomMgr->VerifyAndCorrectXPRate(newXPRate, player->getLevel()));

        sCustomMgr->SetSpecifiedXPRateForPlayer(player, newXPRate);
        handler->SendFormattedSysMessage("XP Rate changed to {}!", newXPRate);
        return true;
    }

    static bool HandleBoostCommand(ChatHandler* handler, char const* /*args*/)
    {
        /*
        Player* player  = handler->GetSession()->GetPlayer();

        if (!player)
            return false;

        auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_NORTHREND_TRANSFER_STATUS);
        stmt->setUInt32(0, player->GetSession()->GetAccountId());
        PreparedQueryResult const result = LoginDatabase.Query(stmt);

        if (!result)
        {
            handler->SendSysMessage("Error.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Field const* fields  = result->Fetch();
        uint8 TransferStatus = fields[0].GetUInt8();

        // Flag Check
        if (TransferStatus == 0)
        {
            handler->SendSysMessage("You can't use this command (level 80 on Northrend realm is required).");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (TransferStatus == 2)
        {
            handler->SendSysMessage("You have already used the boost!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Level Check
        if (player->getLevel() > 1)
        {
            handler->SendSysMessage("You can use this command only on a level 1 character.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Bag Free slot check
        if (player->GetFreeSlotsNumber(false) <= 2)
        {
            handler->SendSysMessage("You don't have enough free bag slots (3 required).");
            handler->SetSentErrorMessage(true);
            return false;
        }

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_NORTHREND_TRANSFER_STATUS);
        stmt->setUInt32(0, player->GetSession()->GetAccountId());
        LoginDatabase.Execute(stmt);

        player->AddItem(41605, 1); // 1x Level Up token
        player->AddItem(41606, 2); // 2x Profession token

        handler->SendSysMessage("Success! Boost tokens have been added to your bags.");
        */
        return true;
    }

    static bool HandlePremadeSaveCommand(ChatHandler* handler, char const* args)
    {
        Player const* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1 && tokens.size() != 2)
            return false;

        uint64 presetTypeMask = 1;
        if (tokens.size() == 2)
        {
            if (auto const presetTypeUint = char_cast<uint64>(tokens[1]))
                presetTypeMask = presetTypeUint.value();
            else
            {
                std::string const presetTypeStr = tokens[1];
                presetTypeMask = sCustomMgr->GetPresetMaskFromString(presetTypeStr);
            }
        }

        sCustomMgr->CreatePremadeSet(tokens[0], player, presetTypeMask);
        return true;
    }

    static bool HandlePremadeEquipCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        sCustomMgr->EquipPremadeSet(tokens[0], player);
        return true;
    }

    static bool HandlePremadeDeleteCommand(ChatHandler* handler, char const* args)
    {
        Player const* player = handler->GetSession()->GetPlayer();
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        sCustomMgr->DeletePremadeSet(tokens[0], player);
        return true;
    }

    static bool HandlePremadeListCommand(ChatHandler* handler, char const* /*args*/)
    {
        sCustomMgr->DisplayPremadeSetList(handler);
        return true;
    }

    static bool HandlePremadeDetailsCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        sCustomMgr->DisplayPremadeSetDetails(tokens[0], handler);
        return true;
    }

    static bool HandlePremadeRenameCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 2)
            return false;

        sCustomMgr->RenamePremadeSet(handler, tokens[0], tokens[1]);
        return true;
    }

    static bool HandlePresetSetGroupAddCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 2)
            return false;

        auto const presetTypeUint = char_cast<uint64>(tokens[1]);
        if (!presetTypeUint)
            return false;

        sCustomMgr->AddPresetGroupToSet(handler, tokens[0], presetTypeUint.value());
        return true;
    }

    static bool HandlePresetSetGroupRemoveCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 2)
            return false;

        auto const presetTypeUint = char_cast<uint64>(tokens[1]);
        if (!presetTypeUint)
            return false;

        sCustomMgr->RemovePresetGroupFromSet(handler, tokens[0], presetTypeUint.value());
        return true;
    }

    static bool HandlePresetSetGroupOverrideCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 2)
            return false;

        auto const presetTypeUint = char_cast<uint64>(tokens[1]);
        if (!presetTypeUint)
            return false;

        sCustomMgr->OverridePresetGroupToSet(handler, tokens[0], presetTypeUint.value());
        return true;
    }

    static bool HandlePresetGroupListCommand(ChatHandler* handler, char const* /*args*/)
    {
        sCustomMgr->DisplayPresetGroupList(handler);
        return true;
    }

    static bool HandlePresetGroupAddCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 2)
            return false;

        auto const groupID = char_cast<uint32>(tokens[0]);
        if (!groupID)
            return false;

        std::string const groupName = tokens[1];
        sCustomMgr->AddPresetGroup(handler, groupName, groupID.value());
        return true;
    }

    static bool HandlePresetGroupRemoveCommand(ChatHandler* handler, char const* args)
    {
        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        std::string const groupName = tokens[0];
        sCustomMgr->RemovePresetGroup(handler, groupName);
        return true;
    }

    static bool HandlePresetNPCDisplayCommand(ChatHandler* handler, char const* /*args*/)
    {
        Creature const* creature = handler->getSelectedCreature();
        if (!creature)
            return false;

        handler->SendFormattedSysMessage("Creature '{}' displays presets with type: {}", creature->GetName(), sCustomMgr->GetPresetMaskToString(creature->GetCreatureTemplate()->PresetGossipFlags));
        return true;
    }

    static bool HandlePresetNPCAddCommand(ChatHandler* handler, char const* args)
    {
        Creature const* creature = handler->getSelectedCreature();
        if (!creature)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        uint64 presetTypeMask = 1;
        if (auto const presetTypeUint = char_cast<uint64>(tokens[0]))
            presetTypeMask = presetTypeUint.value();
        else
        {
            std::string const presetTypeStr = tokens[0];
            presetTypeMask = sCustomMgr->GetPresetMaskFromString(presetTypeStr);
        }

        if (!sCustomMgr->IsValidPresetMask(presetTypeMask))
            return false;

        sCustomMgr->AddPresetToNPC(handler, creature, presetTypeMask);
        return true;
    }

    static bool HandlePresetNPCRemoveCommand(ChatHandler* handler, char const* args)
    {
        Creature const* creature = handler->getSelectedCreature();
        if (!creature)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        uint64 presetTypeMask = 1;
        if (auto const presetTypeUint = char_cast<uint64>(tokens[0]))
            presetTypeMask = presetTypeUint.value();
        else
        {
            std::string const presetTypeStr = tokens[0];
            presetTypeMask = sCustomMgr->GetPresetMaskFromString(presetTypeStr);
        }

        if (!sCustomMgr->IsValidPresetMask(presetTypeMask))
            return false;

        sCustomMgr->RemovePresetFromNPC(handler, creature, presetTypeMask);
        return true;
    }

    static bool HandlePresetNPCOverrideCommand(ChatHandler* handler, char const* args)
    {
        Creature const* creature = handler->getSelectedCreature();
        if (!creature)
            return false;

        Tokenizer const tokens(args, ' ');
        if (tokens.size() != 1)
            return false;

        uint64 presetTypeMask = 1;
        if (auto const presetTypeUint = char_cast<uint64>(tokens[0]))
            presetTypeMask = presetTypeUint.value();
        else
        {
            std::string const presetTypeStr = tokens[0];
            presetTypeMask = sCustomMgr->GetPresetMaskFromString(presetTypeStr);
        }

        if (!sCustomMgr->IsValidPresetMask(presetTypeMask))
            return false;

        sCustomMgr->OverrideNPCPreset(handler, creature, presetTypeMask);
        return true;
    }

    static bool HandleCReloadPremadeSetsCommand(ChatHandler* handler, char const* /*args*/)
    {
        sLog->outString("Re-Loading Premade Sets...");
        sCustomMgr->LoadPremadeSets();
        handler->SendGlobalGMSysMessage("DB tables `premade_sets`, `premade_set_bag_items`, `premade_set_equipment_items`, `premade_set_talents`, `premade_set_glyphs`, `preset_groups` reloaded.");
        return true;
    }

    static bool HandleSpellQueueCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Tokenizer const tokens(args, ' ');
        if (tokens.size() == 0)
        {
            handler->SendFormattedSysMessage("Your spell queue window is {}ms", player->GetSpellQueueWindow());
            return true;
        }

        if (tokens.size() == 1)
        {
            auto const spellQueueValue = char_cast<uint32>(tokens[0]);
            if (!spellQueueValue)
                return false;

            if (spellQueueValue.value() > sWorld->getIntConfig(CONFIG_SPELL_QUEUE_MAX_WINDOW))
            {
                handler->SendFormattedSysMessage("Spell queue window cannot be set to more than {}ms", sWorld->getIntConfig(CONFIG_SPELL_QUEUE_MAX_WINDOW));
                return true;
            }

            player->SetSpellQueueWindow(spellQueueValue.value());
            handler->SendFormattedSysMessage("Spellqueue window was set to {}ms", spellQueueValue.value());
            return true;
        }

        return false;
    }
};

void AddSC_custom_commandscript()
{
    new custom_commandscript();
}
