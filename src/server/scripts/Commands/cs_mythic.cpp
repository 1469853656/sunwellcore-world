class mythic_commandscript : public CommandScript
{
public:
    mythic_commandscript() : CommandScript("go_commandscript") { }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> mythicCommandTable =
        {
            { "showtimer",       SEC_PLAYER,      CMD_INGAME, &HandleMythicShowertimer,          "" },
            { "companion",       SEC_GAMEMASTER,      CMD_INGAME, &HandleMythicCompanionSpawn,       "" },
            { "exp",             SEC_GAMEMASTER,  CMD_INGAME, &HandleMythicCompanionExp,         "" },
            { "keylevel",        SEC_GAMEMASTER,  CMD_INGAME, &HandleMythicKeyLevel,             "" },
            { "showlevel",       SEC_PLAYER,  CMD_INGAME,  &HandleMythicShowKeyLevel, "" }
        };

        static std::vector<ChatCommand> mythicStatsCommandTable =
        {
            { "level", SEC_GAMEMASTER, CMD_INGAME,      &HandleMythicStatLevel, "" },
            { "basehealth", SEC_GAMEMASTER, CMD_INGAME, &HandleMythicStatHealth, "" },
            { "basepower", SEC_GAMEMASTER, CMD_INGAME,  &HandleMythicStatPower, "" },
            { "minmelee", SEC_GAMEMASTER, CMD_INGAME,   &HandleMythicStatMelee, "" },
            { "maxmelee", SEC_GAMEMASTER, CMD_INGAME,   &HandleMythicStatMeleeMax, "" },
            { "meleescal", SEC_GAMEMASTER, CMD_INGAME,  &HandleMythicStatVarianceMelee, "" },
            { "statscal", SEC_GAMEMASTER, CMD_INGAME,   &HandleMythicStatVarianceStat, "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "mythic",         SEC_PLAYER,      CMD_INGAME, nullptr, "", mythicCommandTable },
            { "mythicstats",    SEC_GAMEMASTER,  CMD_INGAME, nullptr, "", mythicStatsCommandTable }
        };
        return commandTable;
    }

    static bool HandleMythicCompanionSpawn(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (!player->IsMythicCompanionLoaded())
            return false;

        //! 41 equals to mini thors summon property
        SummonPropertiesEntry const* prop = sSummonPropertiesStore.LookupEntry(41U);
        TempSummon* summon = player->GetMap()->SummonCreature(Mythic::MYTHIC_COMPANION_ENTRY, player->GetPosition(), prop, 0U, player, 0U, 0U);
        if (!summon)
            return false;

        summon->setFaction(35U);
        summon->SetLevel(player->m_companionData->m_mythicCompanionLevel);
        player->SetMythicCompanionGUID(summon->GetGUID());
        return true;
    }

    static bool HandleMythicCompanionExp(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        char* _expStr = strtok((char*)args, " ");
        if (!_expStr)
            return false;

        int32 expToGive = std::atoi(_expStr);
        if (expToGive == 0)
            return false;

        player->GiveXPToMythicCompanion(expToGive);
        return true;
    }

    static bool HandleMythicShowertimer(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance)
            return false;

        uint32 mythicTimeElapsed = instance->GetMythicTimeElapsed();
        player->MonsterYell(secsToTimeString(mythicTimeElapsed / 1000U).c_str(), 0, player);

        return true;
    }

    static bool HandleMythicKeyLevel(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        char* _keystr = strtok((char*)args, " ");
        if (!_keystr)
            return false;

        int32 keyToSet = std::atoi(_keystr);
        if (keyToSet == 0)
            return false;

        player->SetMythicKeyLevel(keyToSet);
        return true;
    }

    /** Functions that can set mythic stat and will save it to database **/
    static bool HandleMythicStatLevel(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        int32 val = std::atoi(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_LEVEL);
        if (!stmt)
            return false;

        stmt->setUInt32(0, static_cast<uint32>(val));
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Level changed to: %i", val);
        return true;
    }

    static bool HandleMythicStatHealth(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        int32 val = std::atoi(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_HEALTH);
        if (!stmt)
            return false;

        stmt->setUInt32(0, static_cast<uint32>(val));
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Base health changed to: %i", val);
        return true;
    }

    static bool HandleMythicStatPower(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        int32 val = std::atoi(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_POWER);
        if (!stmt)
            return false;

        stmt->setUInt32(0, static_cast<uint32>(val));
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Base power changed to: %i", val);
        return true;
    }

    static bool HandleMythicStatMelee(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->SendSysMessage("REMEMBER THAT MINIMUM VALUE HAS TO BE LOWER THAN MAX VALUE OR WE WILL CRASH");
        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        int32 val = std::atoi(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_MINMELEE);
        if (!stmt)
            return false;

        stmt->setUInt32(0, static_cast<uint32>(val));
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Minimum melee damage changed to: %i", val);
        return true;
    }

    static bool HandleMythicStatMeleeMax(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->SendSysMessage("REMEMBER THAT MAXIMUM VALUE HAS TO BE HIGHER THAN MIN VALUE OR WE WILL CRASH");
        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        int32 val = std::atoi(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_MAXMELEE);
        if (!stmt)
            return false;

        stmt->setUInt32(0, static_cast<uint32>(val));
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Maximum melee damage changed to: %i", val);
        return true;
    }

    static bool HandleMythicStatVarianceMelee(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        float val = std::atof(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_MELEEVAR);
        if (!stmt)
            return false;

        stmt->setFloat(0, val);
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Per level melee variance changed to: %f", val);
        return true;
    }

    static bool HandleMythicStatVarianceStat(ChatHandler* handler, char const* args)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(2/*LANG_SELECT_CREATURE*/);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling)
        {
            handler->SendSysMessage("This creature doesnt have mythic scaling or you're not on mythic.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->m_mythicScaling->m_mythicTemplateId)
        {
            handler->SendSysMessage("This creature doesnt have mythic template id assigned.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* newValue = strtok((char*)args, " ");
        if (!newValue)
            return false;

        float val = std::atof(newValue);
        if (val == 0)
            return false;

        auto stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_MYTHIC_TEMPLATE_STATVAR);
        if (!stmt)
            return false;

        stmt->setFloat(0, val);
        stmt->setUInt32(1, target->m_mythicScaling->m_mythicTemplateId);
        WorldDatabase.Execute(stmt);
        handler->PSendSysMessage("Per level stat variance changed to: %f", val);
        return true;
    }

    static bool HandleMythicShowKeyLevel(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        uint32 _keyLevel = 0;
        InstanceScript* instance = player->GetInstanceScript();
        if (instance && instance->GetMythicKeyLevel() > _keyLevel)
            _keyLevel = instance->GetMythicKeyLevel();

        handler->PSendSysMessage("Instance key level: %u", _keyLevel);
        return true;
    }
};

void AddSC_mythic_commandscript()
{
    new mythic_commandscript();
}
