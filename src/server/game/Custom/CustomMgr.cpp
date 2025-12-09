#include "CustomMgr.h"

#include "BoostMgr.h"
#include "Chat.h"
#include "DatabaseLog.h"
#include "DisableMgr.h"
#include "Language.h"
#include "Pet.h"
#include "ReputationMgr.h"
#include "ScriptedGossip.h"
#include "TransmogificationMgr.h"

void PremadeSet::SaveToDB() const
{
    auto trans = CustomDatabase.BeginTransaction();
    trans->FMTAppend("REPLACE `premade_sets` (`SetName`, `SetLevel`, `SetClass`, `SetSpec`, `SetMask`) VALUES ('{}', {}, {}, {}, {})", SetName, SetLevel, static_cast<uint32>(SetClass), static_cast<uint32>(SetSpec), PresetTypeMask);

    for (auto const& equipItem : EquipmentItems)
        trans->PAppend("REPLACE `premade_set_equipment_items` (`SetName`, `EquipmentSlot`, `ItemID`, `EnchantmentID`, `GemID_0`, `GemID_1`, `GemID_2`, `PrismaticGemID`) VALUES ('%s', %u, %u, %u, %u, %u, %u, %u)",
            SetName, equipItem.EquipmentSlot, equipItem.ItemID, equipItem.ItemInfo.EnchantmentID, equipItem.ItemInfo.GemIDs[0], equipItem.ItemInfo.GemIDs[1], equipItem.ItemInfo.GemIDs[2], equipItem.ItemInfo.PrismaticGemID);

    for (auto const& bagItem : BagItems)
        trans->PAppend("REPLACE `premade_set_bag_items` (`SetName`, `ItemID`, `ItemCount`, `EnchantmentID`, `GemID_0`, `GemID_1`, `GemID_2`, `PrismaticGemID`) VALUES ('%s', %u, %u, %u, %u, %u, %u, %u)",
            SetName, bagItem.ItemID, bagItem.ItemCount, bagItem.ItemInfo.EnchantmentID, bagItem.ItemInfo.GemIDs[0], bagItem.ItemInfo.GemIDs[1], bagItem.ItemInfo.GemIDs[2], bagItem.ItemInfo.PrismaticGemID);

    for (auto const&[talentID, talentRank] : TalentData)
        trans->PAppend("REPLACE `premade_set_talents` (`SetName`, `TalentID`, `TalentRank`) VALUES ('%s', %u, %u)", SetName, talentID, talentRank);

    for (auto const&[glyphID, glyphIndex] : GlyphData)
        trans->PAppend("REPLACE `premade_set_glyphs` (`SetName`, `GlyphID`, `GlyphIndex`) VALUES ('%s', %u, %u)", SetName, glyphID, glyphIndex);

    CustomDatabase.CommitTransaction(trans);
}

void PremadeSet::DeleteFromDB() const
{
    auto trans = CustomDatabase.BeginTransaction();
    trans->PAppend("DELETE FROM `premade_sets` WHERE `SetName` LIKE '%s'", SetName);
    trans->PAppend("DELETE FROM `premade_set_equipment_items` WHERE `SetName` LIKE '%s'", SetName);
    trans->PAppend("DELETE FROM `premade_set_bag_items` WHERE `SetName` LIKE '%s'", SetName);
    trans->PAppend("DELETE FROM `premade_set_talents` WHERE `SetName` LIKE '%s'", SetName);
    trans->PAppend("DELETE FROM `premade_set_glyphs` WHERE `SetName` LIKE '%s'", SetName);
    CustomDatabase.CommitTransaction(trans);
}

CustomData::CustomData(Field const* fields)
{
    if (!fields)
        return;

    CurrentRacial           = fields[0].GetInt32();
    _customCharacterFlags   = static_cast<CustomPlayerFlags>(fields[1].GetUInt64());
    MultipleSpecsFlag       = fields[2].GetUInt32();
    PremiumBoostFlag        = fields[3].GetUInt32();
    TokenUsePhase           = fields[4].GetUInt32();
    ProfessionTokenUsePhase = fields[5].GetUInt32();
    if (fields[6].IsNull())
        XPRate = {};
    else
        XPRate = fields[6].GetFloat();
}

void CustomData::SaveToDB(uint32 characterID) const
{
    auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_REP_CHARACTER_DATA);
    stmt->setUInt32(0, characterID);
    stmt->setUInt32(1, CurrentRacial);
    stmt->setUInt64(2, _customCharacterFlags.AsUnderlyingType());
    stmt->setUInt32(3, MultipleSpecsFlag);
    stmt->setUInt32(4, PremiumBoostFlag);
    stmt->setUInt32(5, TokenUsePhase);
    stmt->setUInt32(6, ProfessionTokenUsePhase);
    if (XPRate)
        stmt->setFloat(7, XPRate.value());
    else
        stmt->setNull(7);

    CustomDatabase.Execute(stmt);
}

void CustomMgr::LoadCustomMgr()
{
    sLog->outString("Loading Instance Boost Values...");
    LoadInstanceBoostValues();
    sLog->outString();

    sLog->outString("Loading Custom Strings...");
    LoadCustomStrings();
    sLog->outString();

    sLog->outString("Loading Presets...");
    LoadPremadeSets();
    sLog->outString();

    sLog->outString("Loading Boost Values...");
    sBoostMgr->LoadFromDB();
}

void CustomMgr::LoadConfigSettings(bool reload)
{
    // Racial Shift
    _customBoolConfigs[CONFIG_RACIAL_SHIFT_FEATURE_ENABLED] = sConfigMgr->GetBoolDefault("RacialShift.Enable", true);
    _customIntConfigs[CONFIG_RACIAL_SHIFT_ENABLED_ALLIANCE] = sConfigMgr->GetIntDefault("RacialShift.EnabledAllianceMask", RACEMASK_ALL);
    _customIntConfigs[CONFIG_RACIAL_SHIFT_ENABLED_HORDE]    = sConfigMgr->GetIntDefault("RacialShift.EnabledHordeMask", RACEMASK_ALL);

    // Premium Store
    _customBoolConfigs[CONFIG_PREMIUM_STORE_ENABLED]              = sConfigMgr->GetBoolDefault("PremiumStore.Enable", true);
    _customBoolConfigs[CONFIG_PREMIUM_STORE_PROCESS_SERVICE_PACK] = sConfigMgr->GetBoolDefault("PremiumStore.Process.ServicePack", true);

    _customIntConfigs[CONFIG_PREMIUM_STORE_UPDATE_TIME]       = sConfigMgr->GetIntDefault("PremiumStore.UpdateTime", 60);

    // Autolearn
    _customBoolConfigs[CONFIG_AUTO_LEARN_FEATURE_ENABLE] = sConfigMgr->GetBoolDefault("AutoLearn.Enable", true);

    // Shared Quest Loot
    _customBoolConfigs[CONFIG_ENABLE_SHARED_QUEST_LOOT] = sConfigMgr->GetBoolDefault("SharedQuestLoot.Enable", true);

    // Loot
    _customFloatConfigs[CONFIG_QUEST_ITEM_LOOT_DROP_CHANCE] = sConfigMgr->GetFloatDefault("QuestLoot.DropChance", 100.0f);

    // Multispec
    _customBoolConfigs[CONFIG_MULTISPEC_ENABLED]            = sConfigMgr->GetBoolDefault("Multispec.Enable", true);
    _customBoolConfigs[CONFIG_MULTISPEC_FREE]               = sConfigMgr->GetBoolDefault("Multispec.Free.Enable", true);

    // Dynamic Spawn
    _customBoolConfigs[CONFIG_DYNAMIC_RESPAWN_ENABLED]                      = sConfigMgr->GetBoolDefault("DynamicSpawn.Enable", true);
    _customBoolConfigs[CONFIG_DYNAMIC_RESPAWN_ENABLE_CORPSEDELAY_CALC]      = sConfigMgr->GetBoolDefault("DynamicSpawn.CorpseDelay.Enable", true);
    _customFloatConfigs[CONFIG_RESPAWN_DYNAMICRATE_CREATURE]                = sConfigMgr->GetFloatDefault("DynamicSpawn.Rate.Creature", 5.0f);
    _customFloatConfigs[CONFIG_RESPAWN_DYNAMICRATE_GAMEOBJECT]              = sConfigMgr->GetFloatDefault("DynamicSpawn.Rate.GameObject", 50.0f);
    _customFloatConfigs[CONFIG_RESPAWN_DYNAMICRATE_CREATURE_CORPSEDELAY]    = sConfigMgr->GetFloatDefault("DynamicSpawn.Rate.CreatureCorpseDelay", 5.0f);
    _customIntConfigs[CONFIG_RESPAWN_DYNAMICMINIMUM_CREATURE]               = sConfigMgr->GetIntDefault("DynamicSpawn.MinimalTime.Creature", 20);
    _customIntConfigs[CONFIG_RESPAWN_DYNAMICMINIMUM_GAMEOBJECT]             = sConfigMgr->GetIntDefault("DynamicSpawn.MinimalTime.GameObject", 10);
    _customIntConfigs[CONFIG_RESPAWN_DYNAMICMINIMUM_CREATURE_CORPSEDELAY]   = sConfigMgr->GetIntDefault("DynamicSpawn.MinimalTime.CreatureCorpseDelay", 20);

    /*********************************************************/
    /***                 NAME RESERVATION                  ***/
    /*********************************************************/
    _customBoolConfigs[CONFIG_NAME_RESERVATION]                  = sConfigMgr->GetBoolDefault("NameReservation.Enable", false);
    _customBoolConfigs[CONFIG_NAME_RESERVATION_DISABLE_DELETION] = sConfigMgr->GetBoolDefault("NameReservation.Disable.Deletion", false);
    _customIntConfigs[CONFIG_NAME_RESERVATION_LIMIT]             = sConfigMgr->GetIntDefault("NameReservation.Limit", 2);

    /*********************************************************/
    /***            CHANNELS FAST CONFIGURATION            ***/
    /*********************************************************/
    _customBoolConfigs[CONFIG_ALLOW_CHANNEL_OWNER]            = sConfigMgr->GetBoolDefault("Channels.Allow.Owner", false);
    _customBoolConfigs[CONFIG_ALLOW_CHANNEL_PASSWORD]         = sConfigMgr->GetBoolDefault("Channels.Allow.Password", false);
    _customBoolConfigs[CONFIG_ALLOW_CHANNEL_NOTIFICATIONS]    = sConfigMgr->GetBoolDefault("Channels.Allow.Notification", false);
    _customBoolConfigs[CONFIG_ALLOW_CHANNEL_SPEAK_DELAY]      = sConfigMgr->GetBoolDefault("Channels.Allow.SpeakDelay", false);

    // Mute History
    _customBoolConfigs[CONFIG_MUTE_HISTORY_ENABLED] = sConfigMgr->GetBoolDefault("MuteHistory.Enable", true);

    /*********************************************************/
    /***                    REALM PHASE                    ***/
    /*********************************************************/
    _customIntConfigs[CONFIG_REALM_PHASE]                         = sConfigMgr->GetIntDefault("Realm.Phase", static_cast<int>(RealmPhase::REALM_PHASE_RELEASED));

    // Misc
    _customIntConfigs[CONFIG_PTR_BAG_ITEM_ID] = sConfigMgr->GetIntDefault("PTR.BagItemID", 23162);
    _customIntConfigs[CONFIG_PTR_BAG_ITEM_COUNT] = sConfigMgr->GetIntDefault("PTR.BagItemCount", 4);
    _customBoolConfigs[CONFIG_TELEPORT_PLAYERS_NEAR_NAXXRAMAS] = sConfigMgr->GetBoolDefault("TeleportPlayersNearNaxxramas.Enable", true);
    _customBoolConfigs[CONFIG_ENCOUNTER_STATS_ENABLED] = sConfigMgr->GetBoolDefault("EncounterStats.Enable", true);
    _customBoolConfigs[CONFIG_ANNOUNCE_ENCOUNTER_STATS_RECORD] = sConfigMgr->GetBoolDefault("EncounterStats.AnnounceRecord", true);

    _customBoolConfigs[CONFIG_SUNWELL_PLATEAU_TIMEWALKING_ENABLED] = sConfigMgr->GetBoolDefault("Timewalking.SunwellPlateau.Enable", true);
    _customBoolConfigs[CONFIG_NAXXRAMAS_10_BOOST_ENABLED] = sConfigMgr->GetBoolDefault("Boost.NAXX10.Enable", true);
    _customBoolConfigs[CONFIG_NAXXRAMAS_25_BOOST_ENABLED] = sConfigMgr->GetBoolDefault("Boost.NAXX25.Enable", true);
    _customBoolConfigs[CONFIG_OBSIDIAN_SANCTUM_25_BOOST_ENABLED] = sConfigMgr->GetBoolDefault("Boost.ObsidianSanctum25.Enable", true);
    _customBoolConfigs[CONFIG_OBSIDIAN_SANCTUM_10_BOOST_ENABLED] = sConfigMgr->GetBoolDefault("Boost.ObsidianSanctum10.Enable", true);
    _customBoolConfigs[CONFIG_EYE_OF_ETERNITY_25_BOOST_ENABLED] = sConfigMgr->GetBoolDefault("Boost.EyeOfEternity25.Enable", true);
    _customBoolConfigs[CONFIG_EYE_OF_ETERNITY_10_BOOST_ENABLED] = sConfigMgr->GetBoolDefault("Boost.EyeOfEternity10.Enable", true);

    // Cooldown Resets
    _customBoolConfigs[CONFIG_RESET_COOLDOWN_ON_WIPE] = sConfigMgr->GetBoolDefault("CooldownReset.OnWipe", true);
    _customIntConfigs[CONFIG_RESET_COOLDOWN_REQUIRED_ENCOUNTER_TIME] = sConfigMgr->GetIntDefault("CooldownReset.RequiredEncounterTime", 45);
    _customBoolConfigs[CONFIG_RESET_COOLDOWN_ON_ENCOUNTER_KILL] = sConfigMgr->GetBoolDefault("CooldownReset.OnEncounterKill", true);
    _customIntConfigs[CONFIG_RESET_COOLDOWN_ON_ENCOUNTER_KILL_MASK] = sConfigMgr->GetIntDefault("CooldownReset.OnEncounterKillMask", static_cast<uint32>(EncounterResetOnKillOption::All));
    _customBoolConfigs[CONFIG_RESET_COOLDOWN_ON_ARENA_END] = sConfigMgr->GetBoolDefault("CooldownReset.OnArenaEnd", true);
    _customBoolConfigs[CONFIG_RESET_COOLDOWN_ON_DUEL_START] = sConfigMgr->GetBoolDefault("CooldownReset.OnDuelStart", true);

    // Server info features
    _customBoolConfigs[CONFIG_SERVER_INFO_DISPLAY_REVISION]       = sConfigMgr->GetBoolDefault("ServerInfo.DisplayRevision", true);
    _customBoolConfigs[CONFIG_SERVER_INFO_DISPLAY_ACTIVE_PLAYERS] = sConfigMgr->GetBoolDefault("ServerInfo.DisplayActive", true);

    // Population Booster
    _customBoolConfigs[CONFIG_POPULATION_BOOSTER_ENABLED]        = sConfigMgr->GetBoolDefault("PopulationBooster.Enable", true);
    _customBoolConfigs[CONFIG_POPULATION_BOOSTER_FAKE_WHO_CLASS] = sConfigMgr->GetBoolDefault("PopulationBooster.Fake.Who.Class", true);
    _customBoolConfigs[CONFIG_POPULATION_BOOSTER_FAKE_WHO_RACE]  = sConfigMgr->GetBoolDefault("PopulationBooster.Fake.Who.Race", true);
    _customBoolConfigs[CONFIG_POPULATION_BOOSTER_FAKE_WHO_LEVEL] = sConfigMgr->GetBoolDefault("PopulationBooster.Fake.Who.Level", true);
    _customBoolConfigs[CONFIG_POPULATION_BOOSTER_FAKE_WHO_GUILD] = sConfigMgr->GetBoolDefault("PopulationBooster.Fake.Who.Guild", false);
    _customFloatConfigs[CONFIG_POPULATION_BOOSTER_VALUE]         = sConfigMgr->GetFloatDefault("PopulationBooster.Value", 0.20f);
    _customIntConfigs[CONFIG_POPULATION_BOOSTER_MIN_LEVEL]       = sConfigMgr->GetIntDefault("PopulationBooster.Fake.Who.Min.Level", 1);

    // XP Rates
    _customFloatConfigs[CONFIG_BASE_XP_RATE] = sConfigMgr->GetFloatDefault("XP.Rate.Base", 3.0f);
    _customFloatConfigs[CONFIG_HARDCORE_XP_RATE] = sConfigMgr->GetFloatDefault("XP.Rate.Hardcore", 1.0f);
    _customFloatConfigs[CONFIG_BOOST_XP_RATE_MULTIPLIER] = sConfigMgr->GetFloatDefault("XP.Rate.BoostMultiplier", 2.0f);
    _customFloatConfigs[CONFIG_MIN_COMMAND_XP_RATE] = sConfigMgr->GetFloatDefault("XP.Rate.MinCommand", 0.5f);
    _customFloatConfigs[CONFIG_XP_RATE_ANCESTRAL_WISDOM_MULTIPLIER] = sConfigMgr->GetFloatDefault("XP.Rate.AncestralWisdomMultiplier", 0.15f);
    _customFloatConfigs[CONFIG_PROFESSION_RATE] = sConfigMgr->GetFloatDefault("Profession.Rate", 2.0f);
    _customFloatConfigs[CONFIG_REPUTATION_RATE_1_68] = sConfigMgr->GetFloatDefault("Reputation.Rate.1_68", 1.25f);

    // AOE Loot
    _customBoolConfigs[CONFIG_AOE_LOOT_ENABLED] = sConfigMgr->GetBoolDefault("AOELoot.Enable", true);
    _customFloatConfigs[CONFIG_AOE_LOOT_RANGE] = sConfigMgr->GetFloatDefault("AOELoot.Range", 30.0f);

    // Path of War
    _customIntConfigs[CONFIG_PATH_OF_WAR_HONOR_REQUIRED] = sConfigMgr->GetIntDefault("PathOfWar.HonorRequired", 4000);
    _customIntConfigs[CONFIG_PATH_OF_WAR_AREA_ID] = sConfigMgr->GetIntDefault("PathOfWar.AreaID", 0);
    if (std::string const teleportLocation = sConfigMgr->GetStringDefault("PathOfWar.TeleportLocation", ""); !teleportLocation.empty())
    {
        // (MapID;x;y;z;o)
        Tokenizer tokens(teleportLocation, ';');
        if (tokens.size() == 5)
        {
            _pathOfWarTeleportLocation = WorldLocation();

            bool error = false;
            if (auto const mapID = char_cast<uint32>(tokens[0]))
                _pathOfWarTeleportLocation->m_mapId = mapID.value();
            else
                error = true;

            if (auto const teleportX = char_cast<float>(tokens[1]))
                _pathOfWarTeleportLocation->m_positionX = teleportX.value();
            else
                error = true;

            if (auto const teleportY = char_cast<float>(tokens[2]))
                _pathOfWarTeleportLocation->m_positionY = teleportY.value();
            else
                error = true;

            if (auto const teleportZ = char_cast<float>(tokens[3]))
                _pathOfWarTeleportLocation->m_positionZ = teleportZ.value();
            else
                error = true;

            if (auto const teleportO = char_cast<float>(tokens[4]))
                _pathOfWarTeleportLocation->m_orientation = teleportO.value();
            else
                error = true;

            if (error)
                _pathOfWarTeleportLocation.reset();
        }
    }

    // Ascended Mode
    TokenizeConfigStringValueFromFile(_ascendedModeAllowedMapIDs, "AscendedMode.AllowedMapIDs", "533;615;616");

    // Misc
    _customBoolConfigs[CONFIG_TEMPORARY_CHECK] = sConfigMgr->GetBoolDefault("TemporaryCheck.Enable", true);

    sTransmogificationMgr->LoadConfig();
}

CustomData* CustomMgr::GetCustomData(uint32 const characterID)
{
    auto const it = _playerCustomData.find(characterID);
    if (it == std::end(_playerCustomData))
        return nullptr;

    return &(*it).second;
}

CustomData const* CustomMgr::GetCustomData(uint32 characterID) const
{
    auto const it = _playerCustomData.find(characterID);
    if (it == std::end(_playerCustomData))
        return nullptr;

    return &(*it).second;
}

void CustomMgr::InsertCharacterCustomData(uint32 characterID, Field const* fields)
{
    _playerCustomData[characterID] = CustomData(fields);
}

void CustomMgr::LoadCharacterCustomData(WorldSession* session, uint32 const characterID)
{
    auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_CHARACTER_DATA);
    stmt->setUInt32(0, characterID);

    session->GetQueryProcessor().AddCallback(CustomDatabase.AsyncQuery(stmt).WithPreparedCallback([session, characterID](PreparedQueryResult characterDataResult)
    {
        Player* player = session->GetPlayer();
        if (!characterDataResult)
        {
            sLog->outError("Player (GUID: %u) not found in table `characters_data`, creating new entry...", characterID);
            sCustomMgr->SaveCharacterCustomData(characterID, false);
            sCustomMgr->UpdateRacialSpellsForPlayer(player);
            return;
        }

        Field const* fields = characterDataResult->Fetch();
        sCustomMgr->InsertCharacterCustomData(characterID, fields);
        sCustomMgr->UpdateRacialSpellsForPlayer(player);
    }));
}

void CustomMgr::SaveCharacterCustomData(uint32 const characterID, bool const logout)
{
    auto const& customData = _playerCustomData[characterID];
    customData.SaveToDB(characterID);
    if (logout)
        _playerCustomData.erase(characterID);
}

void CustomMgr::DeleteCharacterCustomData(uint32 const characterID)
{
    if (GetCustomData(characterID))
        _playerCustomData.erase(characterID);

    auto stmt = CustomDatabase.GetPreparedStatement(CUSTOM_DEL_CHARACTER_DATA);
    stmt->setUInt32(0, characterID);
    CustomDatabase.Execute(stmt);
}

bool CustomMgr::HasCustomCharacterFlag(uint32 characterID, CustomPlayerFlags customPlayerFlag) const
{
    if (CustomData const* customData = GetCustomData(characterID))
        return customData->HasCharacterFlag(customPlayerFlag);

    return false;
}

void CustomMgr::SetCustomCharacterFlag(uint32 characterID, CustomPlayerFlags customPlayerFlag, bool apply)
{
    auto const it = _playerCustomData.find(characterID);
    if (it == std::end(_playerCustomData))
    {
        auto const stmt = CustomDatabase.GetPreparedStatement(apply ? CUSTOM_SET_CHARACTER_DATA_FLAG : CUSTOM_UNSET_CHARACTER_DATA_FLAG);
        stmt->setInt64(0, static_cast<int64>(customPlayerFlag));
        stmt->setUInt32(1, characterID);
        CustomDatabase.Execute(stmt);
        return;
    }

    if (auto& customData = it->second; apply)
        customData.ApplyCharacterFlag(customPlayerFlag);
    else
        customData.RemoveCharacterFlag(customPlayerFlag);

    SaveCharacterCustomData(characterID, false);
}

void CustomMgr::LoadCustomStrings()
{
    _customStrings.clear();
    uint32 const oldMSTime = getMSTime();

    auto stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_CUSTOM_STRING);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            uint32 const customStringEntry = fields[0].GetUInt32();
            std::string customStringContent = fields[1].GetString();
            if (customStringContent.empty())
                continue;

            _customStrings[customStringEntry] = std::move(customStringContent);
        } while (result->NextRow());

        sLog->outString(">> Loaded %zu custom strings in %u ms", _customStrings.size(), GetMSTimeDiffToNow(oldMSTime));
    }
}

std::string const* CustomMgr::GetCustomString(CustomString const entry) const
{
    auto const it = _customStrings.find(to_underlying(entry));
    if (it == std::end(_customStrings))
    {
        sLog->outError("%s", fmt::format("WhitemaneMgr::GetCustomString called with unknown custom string entry {}.", to_underlying(entry)).c_str());
        return nullptr;
    }

    return &it->second;
}

void HandleSubscriptionMessages(WorldSession* session)
{
    time_t const subscriptionTime = session->GetPremiumService(PremiumServices::WhitemanePlus);
    if (subscriptionTime == static_cast<time_t>(0))
    {
        auto const stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PREMIUM_ACCOUNT_EXPIRED_SUBSCRIPTION);
        stmt->setUInt32(0, session->GetAccountId());
        stmt->setUInt32(1, realmID);
        session->GetQueryProcessor().AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([session](PreparedQueryResult expiredSubscriptionResult)
        {
            if (!expiredSubscriptionResult)
            {
                if (auto const notActiveMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusNotActiveMessage))
                    ChatHandler(session).SendSysMessage(notActiveMessage->data());

                return;
            }

            if (auto const expiredMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusExpiredMessage))
                ChatHandler(session).SendSysMessage(expiredMessage->data());

            if (Player const* player = session->GetPlayer())
            {
                sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow())
                    .SetIsStoreMail(false).SetDraftTitle("Your Whitemane+ has expired!")
                    .SetDraftMessage(CustomString::WhitemanePlusExpiredMailMessage)
                    .SetCreatureID(0)); // TODO: add sally whitemane creature id?
            }

            auto const stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_UPD_PREMIUM_ACCOUNT_EXPIRED_MESSAGE_DISPLAYED);
            stmt2->setUInt32(0, session->GetAccountId());
            stmt2->setUInt32(1, realmID);
            LoginDatabase.Execute(stmt2);
        }));
    }
    else
    {
        auto const stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PREMIUM_ACCOUNT_BOUGHT_SUBSCRIPTION);
        stmt->setUInt32(0, session->GetAccountId());
        stmt->setUInt32(1, realmID);
        session->GetQueryProcessor().AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([session, subscriptionTime](PreparedQueryResult boughtSubscriptionResult)
        {
            if (!boughtSubscriptionResult)
            {
                auto const remainingTime = subscriptionTime - time(nullptr);
                if (remainingTime < DAY)
                {
                    if (auto const expires1DayMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusExpires1DayMessage))
                        ChatHandler(session).SendSysMessage(expires1DayMessage->data());
                }
                else if (remainingTime < WEEK)
                {
                    if (auto const expires1WeekMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusExpires1WeekMessage))
                        ChatHandler(session).SendSysMessage(expires1WeekMessage->data());
                }
                else
                {
                    if (auto const activeMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusActiveMessge))
                        ChatHandler(session).SendFormattedSysMessage(activeMessage->data(), TimeToHumanReadable(subscriptionTime));
                }

                return;
            }

            if (auto const expiredMessage = sCustomMgr->GetCustomString(CustomString::WhitemanePlusBoughtMessage))
                ChatHandler(session).SendSysMessage(expiredMessage->data());

            if (Player const* player = session->GetPlayer())
            {
                sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow())
                    .SetIsStoreMail(false).SetDraftTitle("Your Whitemane+ is active!")
                    .SetDraftMessage(CustomString::WhitemanePlusBoughtMailMessage)
                    .SetCreatureID(0)); // TODO: add sally whitemane creature id?
            }

            auto const stmt2 = LoginDatabase.GetPreparedStatement(LOGIN_UPD_PREMIUM_ACCOUNT_BOUGHT_MESSAGE_DISPLAYED);
            stmt2->setUInt32(0, session->GetAccountId());
            stmt2->setUInt32(1, realmID);
            LoginDatabase.Execute(stmt2);
        }));
    }
}

void CustomMgr::LoadPremiumServicesForSession(WorldSession* session, bool const reset)
{
    if (reset)
        session->ResetPremiumServices();

    auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PREMIUM_ACCOUNT);
    stmt->setUInt32(0, session->GetAccountId());
    stmt->setUInt32(1, realmID);
    session->GetQueryProcessor().AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([session](PreparedQueryResult premiumData)
    {
        if (premiumData)
        {
            do
            {
                Field const* fields     = premiumData->Fetch();
                auto const premiumType = static_cast<PremiumServices>(fields[0].GetUInt8());
                if (static_cast<uint32>(premiumType) == 0 || premiumType >= PremiumServices::Max)
                    continue;

                auto const premiumExpiryTime = static_cast<time_t>(fields[1].GetUInt32());
                if (premiumExpiryTime < time(nullptr))
                    continue;

                session->SetPremiumService(premiumType, premiumExpiryTime);
            } while (premiumData->NextRow());
        }

        HandleSubscriptionMessages(session);
    }));
}

void CustomMgr::HandleHearthStoneService(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    if (!player->GetSession()->HasPremiumService(PremiumServices::UnlimitedHearthstone))
        return;

    player->RemoveSpellCooldown(PREMIUM_HEARTHSTONE_SPELL_ID, true);
}

std::map<uint32, uint32> const AutolearnReplacementSpells = {
    { 24131, 19386 }, // Wyvern Sting
    { 24134, 24132 },
    { 24135, 24133 },
    { 27069, 27068 },
    { 49009, 49011 },
    { 49010, 49012 }
};

struct CustomLevelUpData
{
    CustomLevelUpData(uint32 spellid, uint8 playerClass, uint8 teamid, uint8 level, bool learntomaxrank = true, std::function<bool(Player const*)> pred = {})
        : SpellID(spellid)
        , Class(playerClass)
        , TeamID(teamid)
        , Level(level)
        , ShouldLearnToMaxRank(learntomaxrank)
        , Predicate(std::move(pred))
    {
    }

    bool ShouldLearnSpell(Player const* player, uint8 level) const
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SpellID);
        if (!spellInfo)
            return false;

        if (player->HasActiveSpell(SpellID))
            return false;

        // If we didn't pass level then use spelllevel
        if ((!Level && level < spellInfo->SpellLevel) || level < Level)
            return false;
        // Don't check for class if we didn't specify one
        if (Class && Class != player->getClass())
            return false;
        // If TEAM_NEUTRAL then we allow both factions to pass
        if (TeamID != TEAM_NEUTRAL && TeamID != player->GetTeam())
            return false;
        if (Predicate && !Predicate(player))
            return false;

        return true;
    }

    uint32 SpellID            = 0;
    uint8 Class               = CLASS_NONE;
    uint8 TeamID              = TEAM_NEUTRAL;
    uint8 Level               = 0;
    bool ShouldLearnToMaxRank = true;
    std::function<bool(Player const*)> Predicate;
};

const std::vector<CustomLevelUpData> AdditionalSpellsForLevelUp = {
    CustomLevelUpData(29858, CLASS_WARLOCK, TEAM_NEUTRAL, 66),
    CustomLevelUpData(2825, CLASS_SHAMAN, TEAM_HORDE, 70),
    CustomLevelUpData(32182, CLASS_SHAMAN, TEAM_ALLIANCE, 70),

    CustomLevelUpData(3127, CLASS_ROGUE, TEAM_NEUTRAL, 12),
    CustomLevelUpData(3127, CLASS_WARRIOR, TEAM_NEUTRAL, 6),
    CustomLevelUpData(3127, CLASS_HUNTER, TEAM_NEUTRAL, 8),
    CustomLevelUpData(3127, CLASS_PALADIN, TEAM_NEUTRAL, 8),

    CustomLevelUpData(674, CLASS_HUNTER, TEAM_NEUTRAL, 20),
    CustomLevelUpData(66842, CLASS_SHAMAN, TEAM_NEUTRAL, 30),
    CustomLevelUpData(66843, CLASS_SHAMAN, TEAM_NEUTRAL, 40),
    CustomLevelUpData(66844, CLASS_SHAMAN, TEAM_NEUTRAL, 50),
    CustomLevelUpData(7328, CLASS_PALADIN, TEAM_NEUTRAL, 12),
    CustomLevelUpData(3599, CLASS_SHAMAN, TEAM_NEUTRAL, 10),

    CustomLevelUpData(2836, CLASS_ROGUE, TEAM_NEUTRAL, 24),

    // [Apprentice Riding] 60%
    CustomLevelUpData(33388, CLASS_NONE, TEAM_NEUTRAL, 20, false),
    // [Journeyman Riding] 100%
    //CustomLevelUpData(33391, CLASS_NONE, TEAM_NEUTRAL, 40, false),
    // [Expert Riding] Fly 60%
    //CustomLevelUpData(34090, CLASS_NONE, TEAM_NEUTRAL, 60, false),
    // [Artisan Riding] Fly 280%
    // CustomLevelUpData(34091, CLASS_NONE, TEAM_NEUTRAL, 70, false),
    // [Cold Weather Flying] Northrend
    //CustomLevelUpData(54197, CLASS_NONE, TEAM_NEUTRAL, 68, false),
    //CustomLevelUpData(54197, CLASS_NONE, TEAM_NEUTRAL, 77, false),
    //CustomLevelUpData(54197, CLASS_NONE, TEAM_NEUTRAL, 70, false),

    // DK
    CustomLevelUpData(3714, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 61), // Path of Frost
    CustomLevelUpData(54447, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 57), // Rune of Spellbreaking
    CustomLevelUpData(53342, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 57), // Rune of Spellshattering
    CustomLevelUpData(53331, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 60), // Rune of Lichbane
    CustomLevelUpData(54446, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 63), // Rune of Swordbreaking
    CustomLevelUpData(53323, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 63), // Rune of Swordshattering
    CustomLevelUpData(53344, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 70), // Rune of the Fallen Crusader
    CustomLevelUpData(70164, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 72), // Rune of the Nerubian Carapace
    CustomLevelUpData(62158, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 72), // Rune of the Stoneskin Gargoyle

    // Druid
    CustomLevelUpData(29166, CLASS_DRUID, TEAM_NEUTRAL, 40), // Innervate
    CustomLevelUpData(5225, CLASS_DRUID, TEAM_NEUTRAL, 40),  // Track Humanoids
    CustomLevelUpData(20719, CLASS_DRUID, TEAM_NEUTRAL, 40), // Feline Grace (Passive)
    CustomLevelUpData(62600, CLASS_DRUID, TEAM_NEUTRAL, 40), // Savage Defense (Passive)
    // Warrior
    CustomLevelUpData(12678, CLASS_WARRIOR, TEAM_NEUTRAL, 20), // Stance Mastery (Passive)
    // Shaman
    CustomLevelUpData(51505, CLASS_SHAMAN, TEAM_NEUTRAL, 75, false), // Lava Burst (Rank 1)
    CustomLevelUpData(60043, CLASS_SHAMAN, TEAM_NEUTRAL, 80, false), // Lava Burst (Rank 2)
    // Hunter
    CustomLevelUpData(1462, CLASS_HUNTER, TEAM_NEUTRAL, 24), // Beast Lore
    // Paladin
    CustomLevelUpData(25899, CLASS_PALADIN, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->HasSpell(20911); }),

    // Quest Spells
    // Warrior
    // Defensive stance ID: 71 Level: 10
    CustomLevelUpData(71, CLASS_WARRIOR, TEAM_NEUTRAL, 10),
    // Sunder Armor ID: 7386 Level: 10
    CustomLevelUpData(7386, CLASS_WARRIOR, TEAM_NEUTRAL, 10),
    // Taunt ID: 355 Level: 10
    CustomLevelUpData(355, CLASS_WARRIOR, TEAM_NEUTRAL, 10),
    // Berserker Stance ID: 2458 Level: 30
    CustomLevelUpData(2458, CLASS_WARRIOR, TEAM_NEUTRAL, 30),
    // Intercept ID: 20252 Level: 30
    CustomLevelUpData(20252, CLASS_WARRIOR, TEAM_NEUTRAL, 30),

    // Warlock
    // Summon Imp ID: 688 Level: 5?
    CustomLevelUpData(688, CLASS_WARLOCK, TEAM_NEUTRAL, 5),
    // Summon Voidwalker ID: 697 Level: 10
    CustomLevelUpData(697, CLASS_WARLOCK, TEAM_NEUTRAL, 10),
    // Summon Succubus ID: 712 Level: 20
    CustomLevelUpData(712, CLASS_WARLOCK, TEAM_NEUTRAL, 20),
    // Summon Felhunter ID: 691 Level: 30
    CustomLevelUpData(691, CLASS_WARLOCK, TEAM_NEUTRAL, 30),
    // Inferno ID: 1122 Level: 50
    CustomLevelUpData(1122, CLASS_WARLOCK, TEAM_NEUTRAL, 50),
    // Seed of Corruption (rank 3) ID: x Level: 80
    CustomLevelUpData(47836, CLASS_WARLOCK, TEAM_NEUTRAL, 80),
    // Summon Felsteed ID: 5784 Level: 20
    CustomLevelUpData(5784, CLASS_WARLOCK, TEAM_NEUTRAL, 20),
    // Summon Dreadsteed ID: 23161 Level: 40
    CustomLevelUpData(23161, CLASS_WARLOCK, TEAM_NEUTRAL, 40),
    // Sense Demons ID: 5500 Level: 24
    CustomLevelUpData(5500, CLASS_WARLOCK, TEAM_NEUTRAL, 24),

    // Paladin
    // Redemption ID: 7328 Level: 10
    CustomLevelUpData(7328, CLASS_PALADIN, TEAM_ALLIANCE, 10),
    // [Alliance] Seal of Vengeance ID: 31801 Level: 64
    CustomLevelUpData(31801, CLASS_PALADIN, TEAM_ALLIANCE, 64),
    // [HORDE] Seal of Corruption ID: 53736 Level: 66
    CustomLevelUpData(53736, CLASS_PALADIN, TEAM_HORDE, 66),
    // [Alliance] Summon Warhorse ID: 13819 Level: 20
    CustomLevelUpData(13819, CLASS_PALADIN, TEAM_ALLIANCE, 20),
    // [Alliance] Summon Charger ID: 23214 Level: 40
    CustomLevelUpData(23214, CLASS_PALADIN, TEAM_ALLIANCE, 40),
    // [HORDE] Summon Warhorse ID: 34769 Level: 20
    CustomLevelUpData(34769, CLASS_PALADIN, TEAM_HORDE, 20),
    // [HORDE] Summon Charger ID: 34767 Level: 40
    CustomLevelUpData(34767, CLASS_PALADIN, TEAM_HORDE, 40),

    // Hunter
    // Call Pet ID: 883 Level: 10
    CustomLevelUpData(883, CLASS_HUNTER, TEAM_NEUTRAL, 10),
    // Tame Beast ID: 1515 Level: 10
    CustomLevelUpData(1515, CLASS_HUNTER, TEAM_NEUTRAL, 10),
    // Dismiss Pet ID: 2641 Level: 10
    CustomLevelUpData(2641, CLASS_HUNTER, TEAM_NEUTRAL, 10),
    // Feed Pet ID: 6991 Level: 10
    CustomLevelUpData(6991, CLASS_HUNTER, TEAM_NEUTRAL, 10),
    // Revive Pet ID: 982 Level: 10
    CustomLevelUpData(982, CLASS_HUNTER, TEAM_NEUTRAL, 10),
    // Eagle Eye ID: 6197 Level: 14
    CustomLevelUpData(6197, CLASS_HUNTER, TEAM_NEUTRAL, 14),
    // Aimed Shot (Rank 2) ID: 20900 Level: 28
    CustomLevelUpData(20900, CLASS_HUNTER, TEAM_NEUTRAL, 28, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 3) ID: 20901 Level: 36
    CustomLevelUpData(20901, CLASS_HUNTER, TEAM_NEUTRAL, 36, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 4) ID: 20902 Level: 44
    CustomLevelUpData(20902, CLASS_HUNTER, TEAM_NEUTRAL, 44, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 5) ID: 20903 Level: 52
    CustomLevelUpData(20903, CLASS_HUNTER, TEAM_NEUTRAL, 52, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 6) ID: 20904 Level: 60
    CustomLevelUpData(20904, CLASS_HUNTER, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 7) ID: 27065 Level: 70
    CustomLevelUpData(27065, CLASS_HUNTER, TEAM_NEUTRAL, 70, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 8) ID: 49049 Level: 75
    CustomLevelUpData(49049, CLASS_HUNTER, TEAM_NEUTRAL, 75, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Aimed Shot (Rank 9) ID: 49050 Level: 80
    CustomLevelUpData(49050, CLASS_HUNTER, TEAM_NEUTRAL, 80, true, [](Player const* player) { return player->HasSpell(19434); }),
    // Wyvern Sting (Rank 2) ID: 24132 Level: 50
    CustomLevelUpData(24132, CLASS_HUNTER, TEAM_NEUTRAL, 50, true, [](Player const* player) { return player->HasSpell(19386); }),
    // Wyvern Sting (Rank 3) ID: 24133 Level: 60
    CustomLevelUpData(24133, CLASS_HUNTER, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->HasSpell(19386); }),
    // Wyvern Sting (Rank 4) ID: 27068 Level: 70
    CustomLevelUpData(27068, CLASS_HUNTER, TEAM_NEUTRAL, 70, true, [](Player const* player) { return player->HasSpell(19386); }),
    // Wyvern Sting (Rank 5) ID: 49011 Level: 75
    CustomLevelUpData(49011, CLASS_HUNTER, TEAM_NEUTRAL, 75, true, [](Player const* player) { return player->HasSpell(19386); }),
    // Wyvern Sting (Rank 6) ID: 49012 Level: 80
    CustomLevelUpData(49012, CLASS_HUNTER, TEAM_NEUTRAL, 80, true, [](Player const* player) { return player->HasSpell(19386); }),
    // Track Hidden ID: 19885 Level: 24
    CustomLevelUpData(19885, CLASS_HUNTER, TEAM_NEUTRAL, 24),

    // Shaman
    // Stoneskin Totem ID: 8071 Level: 4
    CustomLevelUpData(8071, CLASS_SHAMAN, TEAM_NEUTRAL, 4, false),
    CustomLevelUpData(8154, CLASS_SHAMAN, TEAM_NEUTRAL, 14, false),
    CustomLevelUpData(8155, CLASS_SHAMAN, TEAM_NEUTRAL, 24, false),
    CustomLevelUpData(10406, CLASS_SHAMAN, TEAM_NEUTRAL, 34, false),
    CustomLevelUpData(10407, CLASS_SHAMAN, TEAM_NEUTRAL, 44, false),
    CustomLevelUpData(10408, CLASS_SHAMAN, TEAM_NEUTRAL, 54, false),
    CustomLevelUpData(25508, CLASS_SHAMAN, TEAM_NEUTRAL, 63, false),
    CustomLevelUpData(25509, CLASS_SHAMAN, TEAM_NEUTRAL, 70, false),
    CustomLevelUpData(58751, CLASS_SHAMAN, TEAM_NEUTRAL, 73, false),
    CustomLevelUpData(58753, CLASS_SHAMAN, TEAM_NEUTRAL, 78, false),
    // Searing Totem ID: 3599 Level: 10
    CustomLevelUpData(3599, CLASS_SHAMAN, TEAM_NEUTRAL, 10, false),
    CustomLevelUpData(6363, CLASS_SHAMAN, TEAM_NEUTRAL, 20, false),
    CustomLevelUpData(6364, CLASS_SHAMAN, TEAM_NEUTRAL, 30, false),
    CustomLevelUpData(6365, CLASS_SHAMAN, TEAM_NEUTRAL, 40, false),
    CustomLevelUpData(10437, CLASS_SHAMAN, TEAM_NEUTRAL, 50, false),
    CustomLevelUpData(10438, CLASS_SHAMAN, TEAM_NEUTRAL, 60, false),
    CustomLevelUpData(25533, CLASS_SHAMAN, TEAM_NEUTRAL, 69, false),
    CustomLevelUpData(58699, CLASS_SHAMAN, TEAM_NEUTRAL, 71, false),
    CustomLevelUpData(58703, CLASS_SHAMAN, TEAM_NEUTRAL, 75, false),
    CustomLevelUpData(58704, CLASS_SHAMAN, TEAM_NEUTRAL, 80, false),
    // Healing Stream Totem ID: 5394 Level: 20
    CustomLevelUpData(5394, CLASS_SHAMAN, TEAM_NEUTRAL, 20, false),
    CustomLevelUpData(6375, CLASS_SHAMAN, TEAM_NEUTRAL, 30, false),
    CustomLevelUpData(6377, CLASS_SHAMAN, TEAM_NEUTRAL, 40, false),
    CustomLevelUpData(10462, CLASS_SHAMAN, TEAM_NEUTRAL, 50, false),
    CustomLevelUpData(10463, CLASS_SHAMAN, TEAM_NEUTRAL, 60, false),
    CustomLevelUpData(25567, CLASS_SHAMAN, TEAM_NEUTRAL, 69, false),
    CustomLevelUpData(58755, CLASS_SHAMAN, TEAM_NEUTRAL, 71, false),
    CustomLevelUpData(58756, CLASS_SHAMAN, TEAM_NEUTRAL, 76, false),
    CustomLevelUpData(58757, CLASS_SHAMAN, TEAM_NEUTRAL, 80, false),
    // Far Sight ID: 6196 Level: 28
    CustomLevelUpData(6196, CLASS_SHAMAN, TEAM_NEUTRAL, 28),

    // Druid
    // Teleport: Moonglade ID: 18960 Level: 10
    CustomLevelUpData(18960, CLASS_DRUID, TEAM_NEUTRAL, 10),
    // Bear Form ID: 5487 Level: 10
    CustomLevelUpData(5487, CLASS_DRUID, TEAM_NEUTRAL, 10),
    CustomLevelUpData(9634, CLASS_DRUID, TEAM_NEUTRAL, 40),
    // Maul ID: 6807 Level: 10
    CustomLevelUpData(6807, CLASS_DRUID, TEAM_NEUTRAL, 10),
    // Growl ID: 6795 Level: 10
    CustomLevelUpData(6795, CLASS_DRUID, TEAM_NEUTRAL, 10),
    // Cure Poison ID: 8946 Level: 14
    CustomLevelUpData(8946, CLASS_DRUID, TEAM_NEUTRAL, 14),
    // Aquatic Form ID: 1066 Level: 16
    CustomLevelUpData(1066, CLASS_DRUID, TEAM_NEUTRAL, 16),
    // Swift Flight Form ID: 40120 Level: 70
    // CustomLevelUpData({40120, CLASS_DRUID, TEAM_NEUTRAL, 70),
    // Mangle (Bear) (Rank 2) ID: 33986 Level: 58
    CustomLevelUpData(33986, CLASS_DRUID, TEAM_NEUTRAL, 58, true, [](Player const* player) { return player->HasSpell(33878); }),
    // Mangle (Bear) (Rank 3) ID: 33987 Level: 68
    CustomLevelUpData(33987, CLASS_DRUID, TEAM_NEUTRAL, 68, true, [](Player const* player) { return player->HasSpell(33878); }),
    // Mangle (Bear) (Rank 4) ID: 48563 Level: 75
    CustomLevelUpData(48563, CLASS_DRUID, TEAM_NEUTRAL, 75, true, [](Player const* player) { return player->HasSpell(33878); }),
    // Mangle (Bear) (Rank 5) ID: 48564 Level: 80
    CustomLevelUpData(48564, CLASS_DRUID, TEAM_NEUTRAL, 80, true, [](Player const* player) { return player->HasSpell(33878); }),
    // Mangle (Cat) (Rank 2) ID: 33982 Level: 58
    CustomLevelUpData(33982, CLASS_DRUID, TEAM_NEUTRAL, 58, true, [](Player const* player) { return player->HasSpell(33876); }),
    // Mangle (Cat) (Rank 3) ID: 33983 Level: 68
    CustomLevelUpData(33983, CLASS_DRUID, TEAM_NEUTRAL, 68, true, [](Player const* player) { return player->HasSpell(33876); }),
    // Mangle (Cat) (Rank 4) ID: 48565 Level: 75
    CustomLevelUpData(48565, CLASS_DRUID, TEAM_NEUTRAL, 75, true, [](Player const* player) { return player->HasSpell(33876); }),
    // Mangle (Cat) (Rank 5) ID: 48566 Level: 80
    CustomLevelUpData(48566, CLASS_DRUID, TEAM_NEUTRAL, 80, true, [](Player const* player) { return player->HasSpell(33876); }),
    // Track Humanoids ID: 5225 Level: 32
    CustomLevelUpData(5225, CLASS_DRUID, TEAM_NEUTRAL, 32),

    // Rogue
    // Mutilate
    CustomLevelUpData(1329, CLASS_ROGUE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->HasSpell(1329); }),
    CustomLevelUpData(34411, CLASS_ROGUE, TEAM_NEUTRAL, 50, true, [](Player const* player) { return player->HasSpell(1329); }),
    CustomLevelUpData(34412, CLASS_ROGUE, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->HasSpell(1329); }),
    CustomLevelUpData(34413, CLASS_ROGUE, TEAM_NEUTRAL, 70, true, [](Player const* player) { return player->HasSpell(1329); }),
    CustomLevelUpData(48663, CLASS_ROGUE, TEAM_NEUTRAL, 75, true, [](Player const* player) { return player->HasSpell(1329); }),
    CustomLevelUpData(48666, CLASS_ROGUE, TEAM_NEUTRAL, 80, true, [](Player const* player) { return player->HasSpell(1329); }),

    // Mage
    CustomLevelUpData(3562, CLASS_MAGE, TEAM_NEUTRAL, 20, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Teleport: Ironforge
    CustomLevelUpData(3561, CLASS_MAGE, TEAM_NEUTRAL, 20, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Teleport: Stormwind
    CustomLevelUpData(3565, CLASS_MAGE, TEAM_NEUTRAL, 30, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Teleport: Darnassus
    CustomLevelUpData(32271, CLASS_MAGE, TEAM_NEUTRAL, 20, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Teleport: Exodar
    CustomLevelUpData(49359, CLASS_MAGE, TEAM_NEUTRAL, 35, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Teleport: Theramore
    CustomLevelUpData(3567, CLASS_MAGE, TEAM_NEUTRAL, 20, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),  // Teleport: Orgrimmar
    CustomLevelUpData(3563, CLASS_MAGE, TEAM_NEUTRAL, 20, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),  // Teleport: Undercity
    CustomLevelUpData(3566, CLASS_MAGE, TEAM_NEUTRAL, 30, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),  // Teleport: Thunder Bluff
    CustomLevelUpData(32272, CLASS_MAGE, TEAM_NEUTRAL, 20, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }), // Teleport: Silvermoon
    CustomLevelUpData(49358, CLASS_MAGE, TEAM_NEUTRAL, 35, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }), // Teleport: Stonard
    CustomLevelUpData(35715, CLASS_MAGE, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Teleport: Shattrath
    CustomLevelUpData(33690, CLASS_MAGE, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Teleport: Shattrath
    CustomLevelUpData(53140, CLASS_MAGE, TEAM_NEUTRAL, 71, true), // Teleport: Dalaran

    CustomLevelUpData(11416, CLASS_MAGE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Portal: Ironforge
    CustomLevelUpData(10059, CLASS_MAGE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Portal: Stormwind
    CustomLevelUpData(11419, CLASS_MAGE, TEAM_NEUTRAL, 50, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Portal: Darnassus
    CustomLevelUpData(32266, CLASS_MAGE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Portal: Exodar
    CustomLevelUpData(49360, CLASS_MAGE, TEAM_NEUTRAL, 35, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Portal: Theramore
    CustomLevelUpData(11417, CLASS_MAGE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Portal: Orgrimmar
    CustomLevelUpData(11418, CLASS_MAGE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Portal: Undercity
    CustomLevelUpData(11420, CLASS_MAGE, TEAM_NEUTRAL, 50, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Portal: Thunder Bluff
    CustomLevelUpData(32267, CLASS_MAGE, TEAM_NEUTRAL, 40, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Portal: Silvermoon
    CustomLevelUpData(49361, CLASS_MAGE, TEAM_NEUTRAL, 35, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Portal: Stonard
    CustomLevelUpData(35717, CLASS_MAGE, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_HORDE; }),    // Portal: Shattrath
    CustomLevelUpData(33691, CLASS_MAGE, TEAM_NEUTRAL, 60, true, [](Player const* player) { return player->GetTeam(CF_DENY) == TEAM_ALLIANCE; }), // Portal: Shattrath
    CustomLevelUpData(53142, CLASS_MAGE, TEAM_NEUTRAL, 74, true),                                                                                 // Portal: Dalaran

    // Weapon Skills
    // Druid
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_DRUID, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_MACE, CLASS_DRUID, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_POLEARMS, CLASS_DRUID, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_DRUID, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_DRUID, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_FIST, CLASS_DRUID, TEAM_NEUTRAL, 10),
    // Hunter
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_AXE, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_SWORD, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_AXE, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_POLEARMS, CLASS_HUNTER, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_BOWS, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_GUNS, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_CROSSBOWS, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_THROWNS, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_HUNTER, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_MAIL, CLASS_HUNTER, TEAM_NEUTRAL, 40),

    // Mage
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_MAGE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_MAGE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_MAGE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_WANDS, CLASS_MAGE, TEAM_NEUTRAL, 2),

    // Paladin
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_PALADIN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_AXE, CLASS_PALADIN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_PALADIN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_SWORD, CLASS_PALADIN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_AXE, CLASS_PALADIN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_MACE, CLASS_PALADIN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_POLEARMS, CLASS_PALADIN, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_PLATE, CLASS_PALADIN, TEAM_NEUTRAL, 40),

    // Priest
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_PRIEST, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_PRIEST, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_PRIEST, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_WANDS, CLASS_PRIEST, TEAM_NEUTRAL, 2),

    // Rogue
    CustomLevelUpData(I_SKILL_ONE_HAND_AXE, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_THROWNS, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_BOWS, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_GUNS, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_CROSSBOWS, CLASS_ROGUE, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DUAL_WIELD, CLASS_ROGUE, TEAM_NEUTRAL, 10),
    CustomLevelUpData(I_SKILL_FIST, CLASS_ROGUE, TEAM_NEUTRAL, 20),
    // Shaman
    CustomLevelUpData(I_SKILL_FIST, CLASS_SHAMAN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_SHAMAN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_SHAMAN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_SHAMAN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_AXE, CLASS_SHAMAN, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_AXE, CLASS_SHAMAN, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_TWO_HAND_MACE, CLASS_SHAMAN, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_MAIL, CLASS_SHAMAN, TEAM_NEUTRAL, 40),
    // CustomLevelUpData(I_SKILL_DUAL_WIELD, CLASS_SHAMAN, TEAM_NEUTRAL, 40), learned from skilltree

    // Warlock
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_WARLOCK, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_WARLOCK, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_WARLOCK, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_WANDS, CLASS_WARLOCK, TEAM_NEUTRAL, 2),

    // Warrior
    CustomLevelUpData(I_SKILL_ONE_HAND_AXE, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_AXE, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_MACE, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_SWORD, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_STAVES, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_DAGGERS, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_POLEARMS, CLASS_WARRIOR, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_BOWS, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_THROWNS, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_GUNS, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_CROSSBOWS, CLASS_WARRIOR, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_PLATE, CLASS_WARRIOR, TEAM_NEUTRAL, 40),
    CustomLevelUpData(I_SKILL_DUAL_WIELD, CLASS_WARRIOR, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_FIST, CLASS_WARRIOR, TEAM_NEUTRAL, 10),

    // Death Knight
    CustomLevelUpData(I_SKILL_ONE_HAND_AXE, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_MACE, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_ONE_HAND_SWORD, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_AXE, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_MACE, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_TWO_HAND_SWORD, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 2),
    CustomLevelUpData(I_SKILL_POLEARMS, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 20),
    CustomLevelUpData(I_SKILL_PLATE, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 40),
    CustomLevelUpData(I_SKILL_DUAL_WIELD, CLASS_DEATH_KNIGHT, TEAM_NEUTRAL, 20),
};

void CustomMgr::LearnSpellsForNewLevel(Player* player, uint8 level) const
{
    if (!sCustomMgr->GetBoolConfig(CONFIG_AUTO_LEARN_FEATURE_ENABLE))
        return;

    // Client expected level limitation, like as used in DBC item max levels for "until max player level"
    if (player->getLevel() > DEFAULT_MAX_LEVEL)
        return;

    if (level == player->getLevel() + 1)
        return;

    if (level == 4 && player->getClass() == CLASS_SHAMAN && !player->HasItemCount(46978, 1, true))
        player->AddItem(46978, 1);

    uint32 family;
    switch (player->getClass())
    {
        case CLASS_ROGUE:
            family = SPELLFAMILY_ROGUE;
            break;
        case CLASS_DEATH_KNIGHT:
            family = SPELLFAMILY_DEATHKNIGHT;
            break;
        case CLASS_WARRIOR:
            family = SPELLFAMILY_WARRIOR;
            break;
        case CLASS_PRIEST:
            family = SPELLFAMILY_PRIEST;
            break;
        case CLASS_MAGE:
            family = SPELLFAMILY_MAGE;
            break;
        case CLASS_PALADIN:
            family = SPELLFAMILY_PALADIN;
            break;
        case CLASS_HUNTER:
            family = SPELLFAMILY_HUNTER;
            break;
        case CLASS_DRUID:
            family = SPELLFAMILY_DRUID;
            break;
        case CLASS_SHAMAN:
            family = SPELLFAMILY_SHAMAN;
            break;
        case CLASS_WARLOCK:
            family = SPELLFAMILY_WARLOCK;
            break;
        default:
            family = 2;
            break;
    }

    if (family == 2)
        return;

    // Handling for AdditionalSpellsForLevelUp array
    for (CustomLevelUpData const& levelUpData : AdditionalSpellsForLevelUp)
    {
        if (levelUpData.ShouldLearnSpell(player, level))
        {
            player->learnSpell(levelUpData.SpellID, false);
            if (levelUpData.ShouldLearnToMaxRank)
                player->learnSpellHighRank(levelUpData.SpellID, true, false);
        }
    }

    for (SpellInfo const* spellInfo : sSpellMgr->LevelUpSpellsCache)
    {
        if (!spellInfo)
            continue;

        if (spellInfo->SpellFamilyName != family)
            continue;
        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, spellInfo->Id, player))
            continue;
        if ((spellInfo->AttributesEx7 & SPELL_ATTR7_ALLIANCE_ONLY && player->GetTeam(CF_DENY) != TEAM_ALLIANCE) || (spellInfo->AttributesEx7 & SPELL_ATTR7_HORDE_ONLY && player->GetTeam(CF_DENY) != TEAM_HORDE))
            continue;
        if (spellInfo->BaseLevel != level && SpellMgr::IsSpellValid(spellInfo))
            continue;
        if (spellInfo->Id == 54968) // disable glyph of holy light
            continue;

        bool valid = false;

        SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellInfo->Id);
        for (SkillLineAbilityMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
        {
            if (itr->second->spellId == spellInfo->Id && itr->second->racemask == 0 && itr->second->learnOnGetSkill == 0)
            {
                valid                      = true;
                SpellInfo const* prevSpell = spellInfo->GetPrevRankSpell();
                if (prevSpell && !player->HasSpell(prevSpell->Id))
                {
                    valid = false;
                    break;
                }

                if (GetTalentSpellPos(itr->second->spellId))
                    if (!prevSpell || !player->HasSpell(prevSpell->Id) || spellInfo->GetRank() == 1)
                        valid = false;

                break;
            }
        }

        uint32 spellID = spellInfo->Id;
        auto it        = AutolearnReplacementSpells.find(spellID);
        if (it != std::end(AutolearnReplacementSpells))
            spellID = it->second;

        // temp fix for wyvern sting, check this for all replacement spells
        if (SpellInfo const* realSpellInfo = sSpellMgr->GetSpellInfo(spellID))
            if (realSpellInfo->SpellFamilyFlags == flag96(0x0, 0x1000, 0x0))
                if (!player->HasSpell(19386))
                    valid = false;

        if (player->HasActiveSpell(spellID))
            continue;

        if (valid)
        {
            player->learnSpell(spellID, false);
            player->learnSpellHighRank(spellID, true, false);
        }
    }

    constexpr std::array WeaponSkillsForLevelUp = {
        SKILL_AXES, SKILL_POLEARMS, SKILL_SWORDS, SKILL_2H_AXES, SKILL_2H_SWORDS, SKILL_BOWS, SKILL_CROSSBOWS,
        SKILL_DAGGERS, SKILL_DEFENSE, SKILL_GUNS, SKILL_MACES, SKILL_STAVES, SKILL_THROWN, SKILL_2H_MACES,
        SKILL_UNARMED, SKILL_WANDS
    };

    for (SkillType const skillID : WeaponSkillsForLevelUp)
        if (player->GetSkillValue(static_cast<uint32>(skillID)))
            player->SetSkill(static_cast<uint16>(skillID), 0, player->getLevel() * 5, player->getLevel() * 5);

    LearnSpellsForNewLevel(player, ++level);
}

bool CustomMgr::DoBasicChecksForCommandOrItem(Player const* player)
{
    if (!player || !player->IsInWorld())
        return false;

    if (player->IsInCombat())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while in combat.");
        return false;
    }
    else if (player->GetMap()->IsBattleground())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while in a battleground.");
        return false;
    }
    else if (player->GetMap()->IsBattleArena())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while in an arena.");
        return false;
    }
    else if (player->GetMap()->IsDungeon() || player->GetMap()->IsRaid())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this in a raid or dungeon.");
        return false;
    }
    else if (player->IsCasting())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while casting.");
        return false;
    }
    else if (player->IsMounted())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while mounted.");
        return false;
    }
    else if (player->IsInFlight() || player->IsFlying())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while flying.");
        return false;
    }
    else if (player->isMoving() || player->IsFalling())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while moving or falling.");
        return false;
    }
    else if (player->IsCharmed())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while charmed.");
        return false;
    }
    else if (player->IsSpectator())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You cannot use this while spectating.");
        return false;
    }

    return true;
}

std::optional<float> CustomMgr::GetQuestItemDropChance() const
{
    float const dropChance = sCustomMgr->GetFloatConfig(CONFIG_QUEST_ITEM_LOOT_DROP_CHANCE);
    if (dropChance <= 0.0f || dropChance > 100.0f)
        return {};

    return dropChance;
}

bool CustomMgr::SendMail(SendMailOptions const& mailOptions, bool isCreatureSender, uint32 OrderID)
{
    MailSender const sender(isCreatureSender ? MAIL_CREATURE : MAIL_NORMAL, isCreatureSender ? mailOptions.CreatureID : mailOptions.CharacterID, isCreatureSender ? MAIL_STATIONERY_DEFAULT : MAIL_STATIONERY_GM);
    uint64 const playerGUID = MAKE_NEW_GUID(mailOptions.CharacterID, 0, HIGHGUID_PLAYER);

    // fill mail
    MailDraft draft(mailOptions.DraftTitle, mailOptions.DraftMessage);
    auto trans = CharacterDatabase.BeginTransaction();

    if (mailOptions.MoneyAmount > 0)
        draft.AddMoney(mailOptions.MoneyAmount * GOLD);

    for (auto const& [itemEntry, itemData] : mailOptions.MailItems)
    {
        if (Item* item = Item::CreateItem(itemEntry, itemData.Count, nullptr))
        {
            if (itemData.RandomEnchantID)
                item->SetItemRandomProperties(itemData.RandomEnchantID.value());

            item->SetUInt64Value(ITEM_FIELD_OWNER, playerGUID);
            item->SetUInt64Value(ITEM_FIELD_CONTAINED, playerGUID);
            item->SetBinding(true);
            item->SaveToDB(trans); // save for prevent lost at next mail load, if send fail then item will be deleted
            draft.AddItem(item);
        }
    }

    draft.SendMailTo(trans, MailReceiver(ObjectAccessor::FindPlayer(playerGUID), mailOptions.CharacterID), sender, MAIL_CHECK_MASK_RETURNED, 0);
    CharacterDatabase.CommitTransaction(trans);

    // fill log
    if (mailOptions.IsStoreMail)
        DBLog::OutStore("[PREMIUM STORE] Processing Order ID: {} of Realm ID: {} - Mail sent with mailOptions: {}", OrderID, realmID, mailOptions.ToString());
    else
        DBLog::OutMisc("[CUSTOM MAIL] Realm ID: {} - Mail sent with mailOptions: {}", realmID, mailOptions.ToString());

    return true;
}

std::string CustomMgr::GetClassString(Classes classID)
{
    switch (classID)
    {
        case CLASS_WARRIOR:
            return "Warrior";
        case CLASS_PALADIN:
            return "Paladin";
        case CLASS_HUNTER:
            return "Hunter";
        case CLASS_ROGUE:
            return "Rogue";
        case CLASS_PRIEST:
            return "Priest";
        case CLASS_DEATH_KNIGHT:
            return "Death Knight";
        case CLASS_SHAMAN:
            return "Shaman";
        case CLASS_MAGE:
            return "Mage";
        case CLASS_WARLOCK:
            return "Warlock";
        case CLASS_DRUID:
            return "Druid";
        default:
            return "Unknown";
    }
}

std::string CustomMgr::GetSpecString(Classes classID, uint8 specID)
{
    switch (classID)
    {
        case CLASS_WARRIOR: 
        {
            switch (specID)
            {
                case 0:
                    return "Arms";
                case 1:
                    return "Fury";
                case 2:
                    return "Protection";
                default:
                    break;
            }

            break;
        }
        case CLASS_PALADIN: 
        {
            switch (specID)
            {
                case 0:
                    return "Holy";
                case 1:
                    return "Protection";
                case 2:
                    return "Retribution";
                default:
                    break;
            }

            break;
        }
        case CLASS_HUNTER: 
        {
            switch (specID)
            {
                case 0:
                    return "Beast Mastery";
                case 1:
                    return "Marksmanship";
                case 2:
                    return "Survival";
                default:
                    break;
            }

            break;
        }
        case CLASS_ROGUE: 
        {
            switch (specID)
            {
                case 0:
                    return "Assassination";
                case 1:
                    return "Combat";
                case 2:
                    return "Subtlety";
                default:
                    break;
            }

            break;
        }
        case CLASS_PRIEST: 
        {
            switch (specID)
            {
                case 0:
                    return "Discipline";
                case 1:
                    return "Holy";
                case 2:
                    return "Shadow";
                default:
                    break;
            }

            break;
        }
        case CLASS_DEATH_KNIGHT: 
        {
            switch (specID)
            {
                case 0:
                    return "Blood";
                case 1:
                    return "Frost";
                case 2:
                    return "Unholy";
                default:
                    break;
            }

            break;
        }
        case CLASS_SHAMAN: 
        {
            switch (specID)
            {
                case 0:
                    return "Elemental";
                case 1:
                    return "Enhancement";
                case 2:
                    return "Restoration";
                default:
                    break;
            }

            break;
        }
        case CLASS_MAGE: 
        {
            switch (specID)
            {
                case 0:
                    return "Arcane";
                case 1:
                    return "Fire";
                case 2:
                    return "Frost";
                default:
                    break;
            }

            break;
        }
        case CLASS_WARLOCK: 
        {
            switch (specID)
            {
                case 0:
                    return "Affliction";
                case 1:
                    return "Demonology";
                case 2:
                    return "Destruction";
                default:
                    break;
            }

            break;
        }
        case CLASS_DRUID: 
        {
            switch (specID)
            {
                case 0:
                    return "Balance";
                case 1:
                    return "Feral (DPS)";
                case 2:
                    return "Restoration";
                case 3:
                    return "Feral (Tank)";
                default:
                    break;
            }

            break;
        }
        default:
            return "Unknown";
    }

    return "Unknown";
}

void CustomMgr::HandleFirstLogin(Player* player)
{
    if (!sCustomMgr->CheckRealmPhase(RealmPhase::REALM_PHASE_PTR))
        return;

    if (uint32 const itemID = sCustomMgr->GetIntConfig(CONFIG_PTR_BAG_ITEM_ID))
        if (uint32 const itemCount = sCustomMgr->GetIntConfig(CONFIG_PTR_BAG_ITEM_COUNT))
            player->StoreNewItemInBestSlots(itemID, itemCount);

    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1106), 42999); // Argent Crusade
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1090), 42999); // Kirin Tor
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1098), 42999); // Knights of the Ebon Blade
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1091), 42999); // The Wyrmrest Accord
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1119), 42999); // The Sons of Hodir
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1073), 42999); // The Kalu'ak
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1104), 42999); // Frenzyheart Tribe
    player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1105), 42999); // The Oracles
    if (player->GetTeam(CF_DENY) == TEAM_ALLIANCE)
        player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1037), 42999); // Alliance Vanguard
    else
        player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(1052), 42999); // Horde Expedition
}

std::optional<float> CustomMgr::GetAncestralWisdomXPBonusForPlayer(Player const* player) const
{
    float const ancestralWisdomMultiplier = GetFloatConfig(CONFIG_XP_RATE_ANCESTRAL_WISDOM_MULTIPLIER);
    if (G3D::fuzzyEq(ancestralWisdomMultiplier, 0.0f))
        return {};

    if (uint32 const maxLevelCharacterCount = player->GetSession()->GetMaxLevelCharacterCount())
        return ancestralWisdomMultiplier * static_cast<float>(maxLevelCharacterCount);

    return {};
}

std::optional<float> CustomMgr::GetSpecifiedXPRateForPlayer(Player const* player) const
{
    CustomData const* customData = GetCustomData(player->GetGUIDLow());
    if (!customData)
        return {};

    return customData->XPRate;
}

void CustomMgr::SetSpecifiedXPRateForPlayer(Player const* player, float newXPRate)
{
    uint32 const characterID = player->GetGUIDLow();
    CustomData* customData = GetCustomData(characterID);
    if (!customData)
        return;

    customData->XPRate = newXPRate;
    customData->SaveToDB(characterID);
}

bool CustomMgr::VerifyAndCorrectXPRate(float& xpRate, uint8 playerLevel) const
{
    auto const xpRateForLevel = [this, playerLevel]() -> std::optional<float>
    {
        if (!IsValidLevel(playerLevel))
            return {};

        if (_xpRateBrackets.empty())
            return {};

        // vector is sorted in descending order
        for (auto const&[bracketLevel, bracketXPRate] : _xpRateBrackets)
        {
            if (playerLevel < bracketLevel)
                continue;

            return bracketXPRate;
        }

        return {};
    }();

    float maxXPRateAllowed = GetFloatConfig(CONFIG_BASE_XP_RATE);
    if (xpRateForLevel)
        maxXPRateAllowed = xpRateForLevel.value();

    float const oldXPRate = xpRate;
    xpRate = std::clamp(xpRate, GetFloatConfig(CONFIG_MIN_COMMAND_XP_RATE), maxXPRateAllowed);
    return G3D::fuzzyEq(xpRate, oldXPRate);
}

float CustomMgr::GetAndUpdateXPRateForPlayer(Player const* player)
{
    if (player->HasHardcoreEnabled())
        return GetFloatConfig(CONFIG_HARDCORE_XP_RATE);

    // Apply base rate (or chosen by player)
    float xpRate = GetSpecifiedXPRateForPlayer(player).value_or(GetFloatConfig(CONFIG_BASE_XP_RATE));
    if (!VerifyAndCorrectXPRate(xpRate, player->getLevel()))
        SetSpecifiedXPRateForPlayer(player, xpRate);
    float const baseXPRate = xpRate;

    // Apply premium XP bonus if we have service enabled
    if (player->GetSession()->HasPremiumService(PremiumServices::RapidExperience))
        xpRate += GetFloatConfig(CONFIG_BOOST_XP_RATE_MULTIPLIER);

    // Apply Ancestral Wisdom bonus (as base rate x Ancestral Wisdom multiplier)
    if (auto const ancestralWisdomMultiplier = GetAncestralWisdomXPBonusForPlayer(player))
        xpRate += baseXPRate * ancestralWisdomMultiplier.value();

    return xpRate;
}

std::string CustomMgr::GetDetailedXPRateInfoForPlayer(Player const* player) const
{
    // TODO: add weekend XP bonus
    std::ostringstream str;

    float xpRate = GetSpecifiedXPRateForPlayer(player).value_or(GetFloatConfig(CONFIG_BASE_XP_RATE));
    static_cast<void>(VerifyAndCorrectXPRate(xpRate, player->getLevel()));
    float const baseXPRate = xpRate;
    str << fmt::format("(Base XP Rate: {}x", baseXPRate);

    // 2. Apply premium XP multiplier if we have service enabled
    if (player->GetSession()->HasPremiumService(PremiumServices::RapidExperience))
    {
        float const boostMultiplier = GetFloatConfig(CONFIG_BOOST_XP_RATE_MULTIPLIER);
        xpRate += boostMultiplier;
        str << fmt::format(", with Rapid Experience bonus {}x", boostMultiplier);
    }

    // 3. Apply Ancestral Wisdom bonus
    if (uint32 const maxLevelCharacterCount = player->GetSession()->GetMaxLevelCharacterCount())
    {
        if (auto const ancestralWisdomMultiplier = GetAncestralWisdomXPBonusForPlayer(player))
        {
            float const ancestralWisdomBonus = std::ceil(baseXPRate * ancestralWisdomMultiplier.value() * 100.0f) / 100.0f; // round to 2 decimal places
            xpRate += ancestralWisdomBonus;
            str << fmt::format(", with Ancestral Wisdom bonus {}x from {} max level {}.", ancestralWisdomBonus, maxLevelCharacterCount, maxLevelCharacterCount == 1 ? "character" : "characters");
        }
    }

    str << ")";
    return fmt::format("XP Rate: {}x {}", roundf(xpRate * 100.0f) / 100.0f, str.str());
}

uint32 CustomMgr::CalculateBonusXPWithRateUpdate(Player const* player, uint32 xp)
{
    return static_cast<uint32>(roundf(static_cast<float>(xp) * GetAndUpdateXPRateForPlayer(player)));
}

void CustomMgr::LoadXPRateBrackets()
{
    uint32 const oldMSTime = getMSTime();
    _xpRateBrackets.clear();

    auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_XP_RATE_BRACKETS);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            uint8 const bracketLevel = fields[0].GetUInt8();
            if (!IsValidLevel(bracketLevel))
            {
                sLog->outError("%s", fmt::format("XP Rate Bracket passed with invalid level {}. Skipping...", bracketLevel).c_str());
                continue;
            }

            float const bracketXPRate = fields[1].GetFloat();
            _xpRateBrackets.emplace_back(bracketLevel, bracketXPRate);
        } while (result->NextRow());
    }

    sLog->outString("%s", fmt::format(">> Loaded {} xp rate brackets in {} ms", _xpRateBrackets.size(), GetMSTimeDiffToNow(oldMSTime)).c_str());
}

void CustomMgr::UpdateRacialSpellsForPlayer(Player* player, bool create /* = false */) const
{
    auto const racialID = GetCurrentRacialsForPlayer(player);
    for (auto const& customRacialData : RacialsLookupTable)
    {
        uint32 const racialAllianceMask = GetIntConfig(CONFIG_RACIAL_SHIFT_ENABLED_ALLIANCE);
        uint32 const racialHordeMask    = GetIntConfig(CONFIG_RACIAL_SHIFT_ENABLED_HORDE);
        TeamId const playerTeamID       = player->GetTeam();
        bool raceCheck                  = false;
        if ((playerTeamID == TEAM_HORDE && racialHordeMask & 1 << player->getRace()) || (playerTeamID == TEAM_ALLIANCE && racialAllianceMask & 1 << player->getRace()))
            raceCheck = true;

        if (customRacialData.Race == racialID && raceCheck && (customRacialData.ClassMask == 1 || customRacialData.ClassMask & 1 << player->getClass()))
            player->learnSpell(customRacialData.SpellID, false);
        else
            player->removeSpell(customRacialData.SpellID, SPEC_MASK_ALL, false);
    }

}

bool CustomMgr::IsRacialShiftEnabledForRace(TeamId teamID, Races race) const
{
    if (teamID == TEAM_HORDE)
        return (1 << (race - 1)) & GetIntConfig(CONFIG_RACIAL_SHIFT_ENABLED_HORDE);

    if (teamID == TEAM_ALLIANCE)
        return (1 << (race - 1)) & GetIntConfig(CONFIG_RACIAL_SHIFT_ENABLED_ALLIANCE);

    return true;
}

Races CustomMgr::GetCurrentRacialsForPlayer(Player const* player) const
{
    auto const playerRace = static_cast<Races>(player->getRace());
    if (CustomData const* customData = GetCustomData(player->GetGUIDLow()))
        if (auto const chosenRacial = static_cast<Races>(customData->CurrentRacial); chosenRacial != RACE_NONE)
            return chosenRacial;

    return playerRace;
}

void CustomMgr::ApplyRacialShift(Player* player, Races race)
{
    uint32 const characterID = player->GetGUIDLow();
    auto const it = _playerCustomData.find(characterID);
    if (it == std::end(_playerCustomData))
        return;

    auto& customData = it->second;
    customData.CurrentRacial = race;
    customData.ApplyCharacterFlag(CustomPlayerFlags::UsedRacialShift);
    customData.SaveToDB(characterID);
    UpdateRacialSpellsForPlayer(player);
}

void CustomMgr::RemoveRacialShift(uint32 guid)
{
    auto const it = _playerCustomData.find(guid);
    if (it == std::end(_playerCustomData))
    {
        auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_UPD_CHARACTER_DATA_RACIAL);
        stmt->setUInt64(0, EnumFlag<CustomPlayerFlags>(CustomPlayerFlags::UsedRacialShift).AsUnderlyingType());
        stmt->setUInt8(1, RACE_NONE);
        stmt->setUInt32(2, guid);
        CustomDatabase.Execute(stmt);
        return;
    }

    auto& customData = it->second;
    customData.CurrentRacial = RACE_NONE;
    customData.RemoveCharacterFlag(CustomPlayerFlags::UsedRacialShift);
    customData.SaveToDB(guid);
}

void CustomMgr::LoadInstanceBoostValues()
{
    uint32 const oldMSTime = getMSTime();
    _instanceBoostValues.clear();

    auto stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_INSTANCE_BOOST_VALUES);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        _instanceBoostValues.clear();
        do
        {
            Field const* fields = result->Fetch();
            InstanceBoostValue instanceBoostValue;
            instanceBoostValue.InstanceID = fields[0].GetUInt32();
            instanceBoostValue.BossID = fields[1].GetUInt32();
            instanceBoostValue.BoostID = fields[2].GetUInt32();
            instanceBoostValue.Values = std::make_pair(fields[3].GetUInt32(), fields[4].GetUInt32());
            _instanceBoostValues.emplace_back(instanceBoostValue);
        } while (result->NextRow());

        sLog->outString(">> Loaded %zu instance boost values in %u ms", _instanceBoostValues.size(), GetMSTimeDiffToNow(oldMSTime));
    }
}

std::optional<std::pair<uint32, uint32>> CustomMgr::GetBoostValues(Raids const instanceID, uint32 const bossID, uint32 const boostID) const
{
    for (const auto& [InstanceID, BossID, BoostID, Values] : _instanceBoostValues)
    {
        if (to_underlying(instanceID) == InstanceID && BossID == bossID && BoostID == boostID)
            return Values;
    }

    DBLog::OutMisc("No InstanceBoostValue found in GetBoostValues, InstanceID:{}, BossID:{}, BoostID:{}", to_underlying(instanceID), bossID, boostID);
    return {};
}

std::optional<uint32> CustomMgr::GetBoostValue1(Raids const instanceID, uint32 const bossID, uint32 const boostID) const
{
    if (auto const pair = GetBoostValues(instanceID, bossID, boostID))
        return pair->first;

    return {};
}

std::optional<uint32> CustomMgr::GetBoostValue2(Raids const instanceID, uint32 const bossID, uint32 const boostID) const
{
    if (auto const pair = GetBoostValues(instanceID, bossID, boostID))
        return pair->second;

    return {};
}

void CustomMgr::ApplySharedQuestLoot(LootItem& lootItem, Player const* player)
{
    if (!sCustomMgr->GetBoolConfig(CONFIG_ENABLE_SHARED_QUEST_LOOT))
        return;

    if (lootItem.freeforall)
        return;

    if (!player || !player->IsInWorld())
        return;

    Map const* map = player->GetMap();
    if (map->IsRaid() || map->IsBattlegroundOrArena())
        return;

    if (lootItem.needs_quest)
        lootItem.freeforall = true;
    else if (ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(lootItem.itemid))
        if (sObjectMgr->IsQuestItem(itemProto, player))
            lootItem.freeforall = true;
}

/*********************************************************/
/***                 NAME RESERVATION                  ***/
/*********************************************************/
void CustomMgr::KickPlayerWhileNameReservationIsEnabled(WorldSession* session) const
{
    if (!GetBoolConfig(CONFIG_NAME_RESERVATION) || !session)
        return;

    if (session->GetSecurity() >= SEC_MODERATOR)
        return;

    sLog->outError("Player tried to login on realm while name reservation is enabled - kick to prevent login, AccountID = %u", session->GetAccountId());
    session->KickPlayer();
}

uint32 CustomMgr::BoostPopulationByValue(uint32 currentPopulation, bool fakeWho) const
{
    if (!currentPopulation)
        return currentPopulation;

    if (!GetBoolConfig(CONFIG_POPULATION_BOOSTER_ENABLED) || !fakeWho)
        return currentPopulation;

    float const populationBoost = GetFloatConfig(CONFIG_POPULATION_BOOSTER_VALUE);
    return currentPopulation + static_cast<uint32>(currentPopulation * populationBoost);
}

bool CustomMgr::CheckPlayerDataForToken(Player const* player, uint32 spellId, bool cooldownCheck, bool professionCheck) const
{
    if (!player || !player->IsInWorld())
        return false;

    if (cooldownCheck && player->HasSpellCooldown(spellId))
    {
        ChatHandler(player->GetSession()).SendFormattedSysMessage("You are doing it too fast: please wait a moment before proceeding.");
        return false;
    }

    if (professionCheck)
    {
        ChatHandler(player->GetSession()).SendFormattedSysMessage("You can't use more tokens!");
        return false;
    }

    if (!cooldownCheck && !DoBasicChecksForCommandOrItem(player))
        return false;

    return true;
}

bool CustomMgr::AddItemWihoutNotify(Player* player, uint32 itemId, uint32 count)
{
    if (!player || !player->IsInWorld() || !itemId || !count)
        return false;

    uint32 noSpaceForCount = 0;
    ItemPosCountVec dest;
    InventoryResult const msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
    if (msg != EQUIP_ERR_OK)
        count -= noSpaceForCount;

    if (count == 0 || dest.empty())
    {
        // -- TODO: Send to mailbox if no space
        ChatHandler(player->GetSession()).PSendSysMessage("You don't have any space in your bags.");
        return false;
    }

    ItemRef const item = player->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
    if (item)
    {
        // last check 2.0.10
        WorldPacket data(SMSG_ITEM_PUSH_RESULT, (8 + 4 + 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4));
        data << static_cast<uint64>(player->GetGUID());                                                                 // player GUID
        data << static_cast<uint32>(true);                                                                              // 0=looted, 1=from npc
        data << static_cast<uint32>(false);                                                                             // 0=received, 1=created
        data << static_cast<uint32>(0);                                                                                 // bool print error to chat
        data << static_cast<uint8>(item->GetBagSlot());                                                                 // bagslot
        data << static_cast<uint32>(item->GetCount() == count ? item->GetSlot() : std::numeric_limits<uint32>::max()); // item slot, but when added to stack: 0xFFFFFFFF
        data << static_cast<uint32>(item->GetEntry());                                                                  // item id
        data << static_cast<uint32>(item->GetItemSuffixFactor());                                                       // SuffixFactor
        data << static_cast<int32>(item->GetItemRandomPropertyId());                                                    // random item property id
        data << static_cast<uint32>(count);                                                                             // count of items
        data << static_cast<uint32>(player->GetItemCount(item->GetEntry()));                                            // count of items in inventory
        player->GetSession()->SendPacket(&data);
    }
    else
        return false;

    return true;
}

bool CustomMgr::CheckRealmPhase(RealmPhase const CheckPhase) const
{
    if (static_cast<uint32>(CheckPhase) == GetIntConfig(CONFIG_REALM_PHASE))
        return true;

    return false;
}

void CustomMgr::HandleHardcoreModePlayerDeath(Player const* player, Unit const* killer) const
{
    std::string const* message = GetCustomString(CustomString::HardcoreModeKilled);
    if (!message)
        return;

    std::string playerRace = [race = player->getRace(CF_DENY)]()
    {
        switch (race)
        {
            case RACE_HUMAN:
                return "Human";
            case RACE_ORC:
                return "Orc";
            case RACE_DWARF:
                return "Dwarf";
            case RACE_NIGHTELF:
                return "Night Elf";
            case RACE_UNDEAD_PLAYER:
                return "Undead";
            case RACE_TAUREN:
                return "Tauren";
            case RACE_GNOME:
                return "Gnome";
            case RACE_TROLL:
                return "Troll";
            case RACE_BLOODELF:
                return "Blood Elf";
            case RACE_DRAENEI:
                return "Draenei";
            default:
                return "";
        }
    }();

    std::string playerClass = [pClass = player->getClass()]()
    {
        switch (pClass)
        {
            case CLASS_WARRIOR:
                return "Warrior";
            case CLASS_PALADIN:
                return "Paladin";
            case CLASS_HUNTER:
                return "Hunter";
            case CLASS_ROGUE:
                return "Rogue";
            case CLASS_PRIEST:
                return "Priest";
            case CLASS_DEATH_KNIGHT:
                return "Death Knight";
            case CLASS_SHAMAN:
                return "Shaman";
            case CLASS_MAGE:
                return "Mage";
            case CLASS_WARLOCK:
                return "Warlock";
            case CLASS_DRUID:
                return "Druid";
            default:
                return "";
        }
    }();

    std::string areaName;
    if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(player->GetAreaId()))
        areaName = areaEntry->area_name[sWorld->GetDefaultDbcLocale()];

    sWorld->SendText(fmt::format(*message, player->GetName(), player->getLevel(), playerRace, playerClass, killer->GetName(), areaName).c_str(), [](Player const* player2) { return player2->CanDisplayHardcoreMessages(); });
}

void CustomMgr::LoadPremadeSets()
{
    uint32 const oldMSTime = getMSTime();
    _premadeSets.clear();

    auto stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_PREMADE_SETS);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            std::string setName = fields[0].GetString();
            uint32 const setLevel = fields[1].GetUInt32();
            uint32 const setClass = fields[2].GetUInt32();
            uint32 const setSpec = fields[3].GetUInt32();
            uint64 const setMask = fields[4].GetUInt64();

            PremadeSet premadeSet;
            premadeSet.SetName = setName;
            premadeSet.SetLevel = setLevel;
            premadeSet.SetClass = static_cast<Classes>(setClass);
            premadeSet.SetSpec = setSpec;
            premadeSet.PresetTypeMask = setMask;

            _premadeSets[std::move(setName)] = std::move(premadeSet);
        } while (result->NextRow());
    }

    stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_PREMADE_SET_EQUIPMENT_ITEMS);
    uint32 equipmentItemCount = 0;
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            std::string setName = fields[0].GetString();
            auto const premadeSetIt = _premadeSets.find(setName);
            if (premadeSetIt == _premadeSets.end())
            {
                sLog->outError("Error during loading data from `premade_set_equipment_items`, couldn't find set with name %s", setName.c_str());
                continue;
            }

            auto const equipmentSlot = static_cast<EquipmentSlots>(fields[1].GetUInt32());
            uint32 const itemID = fields[2].GetUInt32();
            if (!sObjectMgr->GetItemTemplate(itemID))
            {
                sLog->outError("Error during loading data from `premade_set_equipment_items`, couldn't find item with id %u", itemID);
                continue;
            }

            PremadeSet::EquipmentItemInfo equipmentItemInfo;
            equipmentItemInfo.ItemID = itemID;
            equipmentItemInfo.EquipmentSlot = equipmentSlot;

            PremadeSet::ItemInfo itemInfo;
            itemInfo.EnchantmentID = fields[3].GetUInt32();
            itemInfo.GemIDs = { fields[4].GetUInt32(), fields[5].GetUInt32(), fields[6].GetUInt32() };
            itemInfo.PrismaticGemID = fields[7].GetUInt32();

            equipmentItemInfo.ItemInfo = itemInfo;

            ++equipmentItemCount;

            premadeSetIt->second.EquipmentItems.emplace_back(equipmentItemInfo);
        } while (result->NextRow());
    }

    stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_PREMADE_SET_BAG_ITEMS);
    uint32 bagItemCount = 0;
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            std::string setName = fields[0].GetString();
            auto const premadeSetIt = _premadeSets.find(setName);
            if (premadeSetIt == _premadeSets.end())
            {
                sLog->outError("Error during loading data from `premade_set_bag_items`, couldn't find set with name %s", setName.c_str());
                continue;
            }

            uint32 const itemID = fields[1].GetUInt32();
            if (!sObjectMgr->GetItemTemplate(itemID))
            {
                sLog->outError("Error during loading data from `premade_set_bag_items`, couldn't find item with id %u", itemID);
                continue;
            }

            PremadeSet::BagItemInfo bagItemInfo;
            bagItemInfo.ItemID = itemID;
            bagItemInfo.ItemCount = fields[2].GetUInt32();
            ++bagItemCount;

            premadeSetIt->second.BagItems.emplace_back(bagItemInfo);
        } while (result->NextRow());
    }

    stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_PREMADE_SET_TALENTS);
    uint32 talentsCount = 0;
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            std::string setName = fields[0].GetString();
            auto const premadeSetIt = _premadeSets.find(setName);
            if (premadeSetIt == _premadeSets.end())
            {
                sLog->outError("Error during loading data from `premade_set_talents`, couldn't find set with name %s", setName.c_str());
                continue;
            }

            uint32 const talentID = fields[1].GetUInt32();
            TalentEntry const* talentEntry = sTalentStore.LookupEntry(talentID);
            if (!talentEntry)
            {
                sLog->outError("Error during loading data from `premade_set_talents`, couldn't find talent ID %u", talentID);
                continue;
            }

            uint32 const talentRank = fields[2].GetUInt32();
            if (!sSpellMgr->GetSpellInfo(talentEntry->RankID[talentRank]))
            {
                sLog->outError("Error during loading data from `premade_set_talents`, couldn't find talent rank %u for talent ID %u", talentRank, talentID);
                continue;
            }

            ++talentsCount;

            PremadeSet::TalentInfo talentData;
            talentData.TalentID = talentID;
            talentData.TalentRank = talentRank;
            premadeSetIt->second.TalentData.emplace_back(talentData);
        } while (result->NextRow());
    }

    stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_PREMADE_SET_GLYPHS);
    uint32 glyphsCount = 0;
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            std::string setName = fields[0].GetString();
            auto const premadeSetIt = _premadeSets.find(setName);
            if (premadeSetIt == _premadeSets.end())
            {
                sLog->outError("Error during loading data from `premade_set_glyphs`, couldn't find set with name %s", setName.c_str());
                continue;
            }

            uint32 const glyphID = fields[1].GetUInt32();
            uint32 const glyphIndex = fields[2].GetUInt32();
            if (!sGlyphPropertiesStore.LookupEntry(glyphID))
            {
                sLog->outError("Error during loading data from `premade_set_glyphs`, couldn't find glyph ID %u", glyphID);
                continue;
            }

            ++glyphsCount;

            PremadeSet::GlyphInfo glyphData;
            glyphData.GlyphID = glyphID;
            glyphData.GlyphIndex = glyphIndex;

            premadeSetIt->second.GlyphData.emplace_back(glyphData);
        } while (result->NextRow());
    }

    _presetGroups.clear();
    stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_PRESET_GROUPS);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            uint32 const groupID = fields[0].GetUInt32();
            std::string groupName = fields[1].GetString();
            _presetGroups[groupID] = std::move(groupName);
        } while (result->NextRow());
    }

    sLog->outString(">> Loaded %zu premade sets (with %u equipment items, %u bag items, %u talents, %u glyphs, %u groups) in %u ms", _premadeSets.size(), equipmentItemCount, bagItemCount, talentsCount, glyphsCount, _presetGroups.size(), GetMSTimeDiffToNow(oldMSTime));
}

PremadeSet const* CustomMgr::GetPremadeSet(std::string_view setName) const
{
    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
        return nullptr;

    return &it->second;
}

void CustomMgr::CreatePremadeSet(std::string const& setName, Player const* player, uint64 presetTypeMask)
{
    if (!player || !player->IsInWorld())
        return;

    if (setName.empty())
    {
        ChatHandler(player->GetSession()).SendSysMessage("You must specify a name for the set.");
        return;
    }

    if (setName.size() > 50)
    {
        ChatHandler(player->GetSession()).SendSysMessage("The name of the set must be less than 50 characters.");
        return;
    }

    if (_premadeSets.contains(setName))
    {
        ChatHandler(player->GetSession()).SendSysMessage("A set with this name already exists.");
        return;
    }

    PremadeSet premadeSet;
    premadeSet.SetName = setName;
    premadeSet.SetLevel = player->getLevel();
    premadeSet.SetClass = static_cast<Classes>(player->getClass());
    premadeSet.SetSpec = player->GetActiveSpec();
    premadeSet.PresetTypeMask = presetTypeMask;

    for (uint8 i = EQUIPMENT_SLOT_HEAD; i < EQUIPMENT_SLOT_END; i++)
    {
        if (ItemRef const item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            PremadeSet::EquipmentItemInfo equipmentItemInfo;
            equipmentItemInfo.ItemID = item->GetEntry();
            equipmentItemInfo.EquipmentSlot = static_cast<EquipmentSlots>(i);

            PremadeSet::ItemInfo itemInfo;
            itemInfo.EnchantmentID = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
            itemInfo.GemIDs = { item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3) };
            itemInfo.PrismaticGemID = item->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT);

            equipmentItemInfo.ItemInfo = itemInfo;

            premadeSet.EquipmentItems.emplace_back(equipmentItemInfo);
        }
    }

    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        ItemRef const item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!item)
            continue;

        PremadeSet::BagItemInfo itemInfo;
        itemInfo.ItemID = item->GetEntry();
        itemInfo.ItemCount = item->GetCount();
        premadeSet.BagItems.emplace_back(itemInfo);
    }

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if (Bag const* bag = player->GetBagByPos(i))
        {
            for (uint8 j = 0; j < bag->GetBagSize(); ++j)
            {
                if (ItemRef const item = bag->GetItemByPos(j))
                {
                    PremadeSet::BagItemInfo itemInfo;
                    itemInfo.ItemID = item->GetEntry();
                    itemInfo.ItemCount = item->GetCount();
                    premadeSet.BagItems.emplace_back(itemInfo);
                }
            }
        }
    }

    for (auto const&[spellID, talent] : player->GetTalentMap())
    {
        if (!talent)
            continue;

        if (talent->State == PLAYERSPELL_REMOVED || !talent->IsInSpec(premadeSet.SetSpec))
            continue;

        TalentSpellPos const* talentPos = GetTalentSpellPos(spellID);
        if (!talentPos)
            continue;

        PremadeSet::TalentInfo talentData;
        talentData.TalentID = talentPos->talent_id;
        talentData.TalentRank = talentPos->rank;
        premadeSet.TalentData.emplace_back(talentData);
    }

    // sort by rows, so that talents are added in correct order, otherwise some talents may not be added
    std::ranges::sort(premadeSet.TalentData, [](PremadeSet::TalentInfo const& lhs, PremadeSet::TalentInfo const& rhs) 
    {
        TalentEntry const* talentLhs = sTalentStore.LookupEntry(lhs.TalentID);
        TalentEntry const* talentRhs = sTalentStore.LookupEntry(rhs.TalentID);
        return talentLhs->Row < talentRhs->Row;
    });

    for (uint8 i = 0; i < MAX_GLYPH_SLOT_INDEX; ++i)
    {
        uint32 const glyph = player->GetGlyph(i);
        if (glyph)
        {
            PremadeSet::GlyphInfo glyphData;
            glyphData.GlyphID = glyph;
            glyphData.GlyphIndex = i;
            premadeSet.GlyphData.emplace_back(glyphData);
        }
    }

    premadeSet.SaveToDB();
    _premadeSets[setName] = std::move(premadeSet);
    ChatHandler(player->GetSession()).SendFormattedSysMessage("Set with name {} created.", setName);
}

void CustomMgr::EquipPremadeSet(std::string const& setName, Player* player) const
{
    if (!player)
        return;

    PremadeSet const* premadeSet = GetPremadeSet(setName);
    if (!premadeSet)
    {
        ChatHandler(player->GetSession()).SendSysMessage("A set with this name does not exist.");
        return;
    }

    if (premadeSet->SetLevel != player->getLevel())
    {
        ChatHandler(player->GetSession()).SendFormattedSysMessage("This set is for {} level, you have {}.", premadeSet->SetLevel, player->getLevel());
        return;
    }

    if (premadeSet->SetClass != player->getClass())
    {
        ChatHandler(player->GetSession()).SendSysMessage("This set is not for your class.");
        return;
    }

    uint32 freeSlotsInBags = player->GetFreeSlotsNumber(true);
    auto neededSlotsInBags = static_cast<uint32>(premadeSet->BagItems.size());
    for (uint8 i = EQUIPMENT_SLOT_HEAD; i < EQUIPMENT_SLOT_END; i++)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        ++neededSlotsInBags;
    }

    if (neededSlotsInBags > freeSlotsInBags)
    {
        ChatHandler(player->GetSession()).SendFormattedSysMessage("You don't have enough space in your bags (you have {}, {} needed).", freeSlotsInBags, neededSlotsInBags);
        return;
    }

    uint64 const playerGUID = player->GetGUID();
    auto const ApplyItemData = [player, playerGUID](ItemRef const& item, PremadeSet::ItemInfo const& itemInfo)
    {
        item->SetEnchantment(PERM_ENCHANTMENT_SLOT, itemInfo.EnchantmentID, 0, 0, playerGUID);
        if (itemInfo.EnchantmentID)
            player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
        item->SetEnchantment(SOCK_ENCHANTMENT_SLOT, itemInfo.GemIDs[0], 0, 0, playerGUID);
        if (itemInfo.GemIDs[0])
            player->ApplyEnchantment(item, SOCK_ENCHANTMENT_SLOT, true);
        item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_2, itemInfo.GemIDs[1], 0, 0, playerGUID);
        if (itemInfo.GemIDs[1])
            player->ApplyEnchantment(item, SOCK_ENCHANTMENT_SLOT_2, true);
        item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_3, itemInfo.GemIDs[2], 0, 0, playerGUID);
        if (itemInfo.GemIDs[2])
            player->ApplyEnchantment(item, SOCK_ENCHANTMENT_SLOT_3, true);
        item->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, itemInfo.PrismaticGemID, 0, 0, playerGUID);
        if (itemInfo.PrismaticGemID)
            player->ApplyEnchantment(item, PRISMATIC_ENCHANTMENT_SLOT, true);

        item->SendUpdateToPlayer(player);
    };

    player->resetTalents(true);

    for (auto const&[talentID, talentRank] : premadeSet->TalentData)
        player->LearnTalent(talentID, talentRank, true);

    for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
        if (uint32 const glyphID = player->GetGlyph(slot))
            if (GlyphPropertiesEntry const* glyphEntry = sGlyphPropertiesStore.LookupEntry(glyphID))
                player->RemoveAurasDueToSpell(glyphEntry->SpellId);
    player->ResetGlyphsForActiveSpec(true);

    for (auto const&[glyphID, glyphIndex] : premadeSet->GlyphData)
    {
        if (glyphID)
            if (GlyphPropertiesEntry const* glyphEntry = sGlyphPropertiesStore.LookupEntry(glyphID))
                player->CastSpell(player, glyphEntry->SpellId, static_cast<TriggerCastFlags>(TRIGGERED_FULL_MASK & ~(TRIGGERED_IGNORE_SHAPESHIFT | TRIGGERED_IGNORE_CASTER_AURASTATE)));

        player->SetGlyph(glyphIndex, glyphID, true);
    }

    player->SendTalentsInfoData(false);

    for (uint8 i = EQUIPMENT_SLOT_HEAD; i < EQUIPMENT_SLOT_END; i++)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        ItemRef item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!item)
            continue;

        ItemPosCountVec dest;
        if (player->CanStoreItem(NULL_BAG, NULL_SLOT, dest, item, false) != EQUIP_ERR_OK)
        {
            player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, i, true);
            auto trans = CharacterDatabase.BeginTransaction();
            item->DeleteFromInventoryDB(trans);
            item->SaveToDB(trans);
            std::string subject = player->GetSession()->GetTrinityString(LANG_NOT_EQUIPPED_ITEM);
            MailDraft(subject, "There were problems with storing one or several items").AddItem(item.Release()).SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
            CharacterDatabase.CommitTransaction(trans);
            continue;
        }

        player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
        player->StoreItem(dest, item, true);
    }

    for (auto const& equipItem : premadeSet->EquipmentItems)
    {
        uint32 const itemID = equipItem.ItemID;

        EquipmentSlots const equipmentSlot = equipItem.EquipmentSlot;
        uint16 eDest;
        InventoryResult const msg = player->CanEquipNewItem(equipmentSlot, eDest, itemID, false);
        if (msg != EQUIP_ERR_OK)
        {
            ChatHandler(player->GetSession()).SendFormattedSysMessage("Couldn't equip item with id {} in slot {}.", itemID, equipmentSlot);
            continue;
        }

        ItemRef const item = player->EquipNewItem(eDest, itemID, true);
        if (!item)
        {
            ChatHandler(player->GetSession()).SendFormattedSysMessage("Couldn't create item with id {} in slot {}.", itemID, equipmentSlot);
            continue;
        }

        ApplyItemData(item, equipItem.ItemInfo);
    }

    for (auto const& bagItem : premadeSet->BagItems)
    {
        uint32 const itemID = bagItem.ItemID;
        uint32 const itemCount = bagItem.ItemCount;
        ItemRef const item = player->CreateNewItem(itemID, itemCount);
        if (!item)
        {
            ChatHandler(player->GetSession()).SendFormattedSysMessage("Couldn't create item with id {} and count {}.", itemID, itemCount);
            continue;
        }

        ApplyItemData(item, bagItem.ItemInfo);
    }

    ChatHandler(player->GetSession()).SendFormattedSysMessage("Set with name {} equipped.", setName);
}

void CustomMgr::DeletePremadeSet(std::string_view setName, Player const* source)
{
    if (setName.empty())
        return;

    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
    {
        if (source)
            ChatHandler(source->GetSession()).SendSysMessage("A set with this name does not exist.");

        return;
    }

    it->second.DeleteFromDB();
    _premadeSets.erase(it);
    if (source)
        ChatHandler(source->GetSession()).SendFormattedSysMessage("Set with name {} deleted.", setName);
}

void CustomMgr::DisplayPremadeSetList(ChatHandler* handler) const
{
    if (!handler)
        return;

    if (_premadeSets.empty())
    {
        handler->SendSysMessage("There are no premade sets.");
        return;
    }

    handler->SendSysMessage("Displaying all premade sets...");
    for (auto const&[setName, setInfo] : _premadeSets)
        handler->SendFormattedSysMessage("{} {} {} - {} (Type: {})", setInfo.SetLevel, GetClassString(setInfo.SetClass), GetSpecString(setInfo.SetClass, setInfo.SetSpec), setName, GetPresetMaskToString(setInfo.PresetTypeMask));
}

void CustomMgr::DisplayPremadeSetDetails(std::string_view setName, ChatHandler* handler) const
{
    if (!handler)
        return;

    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
    {
        handler->SendSysMessage("A set with this name does not exist.");
        return;
    }

    auto const& setInfo = it->second;
    handler->SendFormattedSysMessage("Displaying detailed info for set with type {} ({} {} {} - {})...", GetPresetMaskToString(setInfo.PresetTypeMask), setInfo.SetLevel, GetClassString(setInfo.SetClass), GetSpecString(setInfo.SetClass, setInfo.SetSpec), setName);

    for (auto const& equipItem : setInfo.EquipmentItems)
        handler->SendFormattedSysMessage("Equipment item: {}", ObjectMgr::FormatItemLink(equipItem.ItemID, equipItem.ItemInfo.GemIDs, equipItem.ItemInfo.EnchantmentID));
    
    for (auto const& bagItem : setInfo.BagItems)
        handler->SendFormattedSysMessage("Bag item: {} (count: {})", ObjectMgr::FormatItemLink(bagItem.ItemID, bagItem.ItemInfo.GemIDs, bagItem.ItemInfo.EnchantmentID), bagItem.ItemCount);

    for (auto const&[talentID, talentRank] : setInfo.TalentData)
    {
        TalentEntry const* talentEntry = sTalentStore.LookupEntry(talentID);
        uint32 const spellID = talentEntry->RankID[talentRank];
        handler->SendFormattedSysMessage("Talent: {} (Rank: {})", ObjectMgr::FormatSpellLink(spellID), talentRank + 1);
    }

    for (auto const&[glyphID, glyphIndex] : setInfo.GlyphData)
        handler->SendFormattedSysMessage("Glyph: {} (Index: {})", ObjectMgr::FormatGlyphLink(glyphID, glyphIndex), glyphIndex);
}

void CustomMgr::RenamePremadeSet(ChatHandler* handler, std::string_view setName, std::string const& newName)
{
    if (!handler)
        return;

    if (setName.empty())
    {
        handler->SendSysMessage("You must specify a name for the set.");
        return;
    }

    if (setName.size() > 50)
    {
        handler->SendSysMessage("The name of the set must be less than 50 characters.");
        return;
    }

    if (newName.empty())
    {
        handler->SendSysMessage("You must specify a new name for the set.");
        return;
    }

    if (newName.size() > 50)
    {
        handler->SendSysMessage("The new name of the set must be less than 50 characters.");
        return;
    }

    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
    {
        handler->SendSysMessage("A set with this name does not exist.");
        return;
    }

    PremadeSet const& setInfo = it->second;
    if (setInfo.SetName == newName)
    {
        handler->SendSysMessage("The new name must be different from the old one.");
        return;
    }

    if (_premadeSets.contains(newName))
    {
        handler->SendSysMessage("A set with this name already exists.");
        return;
    }

    it->second.DeleteFromDB();
    it->second.SetName = newName;
    it->second.SaveToDB();
    _premadeSets[newName] = std::move(it->second);
    _premadeSets.erase(it);
    handler->SendFormattedSysMessage("Set with name {} renamed to {}.", setName, newName);
}

void CustomMgr::SendGossipsForPremadeSets(Player const* player, uint64 sourceGUID, uint64 presetTypeMask) const
{
    if (!player || !player->IsInWorld())
        return;

    if (_premadeSets.empty())
        return;

    uint32 action = 0;
    for (auto const&[setName, setInfo] : _premadeSets)
    {
        if (setInfo.SetClass != player->getClass())
            continue;

        if (setInfo.SetLevel != player->getLevel())
            continue;

        if ((setInfo.PresetTypeMask & presetTypeMask) == 0)
            continue;

        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(-1, GOSSIP_ICON_CHAT, GetSpecString(setInfo.SetClass, setInfo.SetSpec), GOSSIP_SENDER_MAIN, action, "Are you sure you want to use this preset?", 0);
        ++action;
    }

    if (action)
        player->PlayerTalkClass->SendGossipMenu(25000, sourceGUID);
}

void CustomMgr::HandlePremadeSetGossipSelect(Player* player, uint32 action, uint64 presetTypeMask) const
{
    if (!player || !player->IsInWorld())
        return;

    if (_premadeSets.empty())
        return;

    uint32 currentAction = 0;
    for (auto const&[setName, setInfo] : _premadeSets)
    {
        if (setInfo.SetClass != player->getClass())
            continue;

        if (setInfo.SetLevel != player->getLevel())
            continue;

        if ((setInfo.PresetTypeMask & presetTypeMask) == 0)
            continue;

        if (currentAction == action)
        {
            EquipPremadeSet(setName, player);
            break;
        }

        ++currentAction;
    }
}

void CustomMgr::AddPresetGroupToSet(ChatHandler* handler, std::string_view setName, uint64 presetTypeMask)
{
    if (!handler)
        return;

    if (setName.empty())
    {
        handler->SendSysMessage("You must specify a name for the set.");
        return;
    }

    if (setName.size() > 50)
    {
        handler->SendSysMessage("The name of the set must be less than 50 characters.");
        return;
    }

    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
    {
        handler->SendSysMessage("A set with this name does not exist.");
        return;
    }

    if ((it->second.PresetTypeMask & presetTypeMask) != 0)
    {
        handler->SendSysMessage("This set already has this preset type.");
        return;
    }

    it->second.PresetTypeMask |= presetTypeMask;
    it->second.SaveToDB();
    handler->SendFormattedSysMessage("Preset type {} added to set with name {}.", GetPresetMaskToString(presetTypeMask), setName);
}

void CustomMgr::RemovePresetGroupFromSet(ChatHandler* handler, std::string_view setName, uint64 presetTypeMask)
{
    if (!handler)
        return;

    if (setName.empty())
    {
        handler->SendSysMessage("You must specify a name for the set.");
        return;
    }

    if (setName.size() > 50)
    {
        handler->SendSysMessage("The name of the set must be less than 50 characters.");
        return;
    }

    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
    {
        handler->SendSysMessage("A set with this name does not exist.");
        return;
    }

    if ((it->second.PresetTypeMask & presetTypeMask) == 0)
    {
        handler->SendSysMessage("This set does not have this preset type.");
        return;
    }

    it->second.PresetTypeMask &= ~presetTypeMask;
    it->second.SaveToDB();
    handler->SendFormattedSysMessage("Preset type {} removed from set with name {}.", GetPresetMaskToString(presetTypeMask), setName);
}

void CustomMgr::OverridePresetGroupToSet(ChatHandler* handler, std::string_view setName, uint64 presetTypeMask)
{
    if (!handler)
        return;

    if (setName.empty())
    {
        handler->SendSysMessage("You must specify a name for the set.");
        return;
    }

    if (setName.size() > 50)
    {
        handler->SendSysMessage("The name of the set must be less than 50 characters.");
        return;
    }

    auto const it = _premadeSets.find(setName.data());
    if (it == _premadeSets.end())
    {
        handler->SendSysMessage("A set with this name does not exist.");
        return;
    }

    it->second.PresetTypeMask = presetTypeMask;
    it->second.SaveToDB();
    handler->SendFormattedSysMessage("Preset type {} overriden for set with name {}.", GetPresetMaskToString(presetTypeMask), setName);
}

std::string CustomMgr::GetPresetMaskToString(uint64 presetTypeMask) const
{
    std::string presetMaskString;
    for (auto const&[presetGroupID, presetGroupName] : _presetGroups)
    {
        if ((presetTypeMask & (1 << presetGroupID)) == 0)
            continue;

        presetMaskString += presetGroupName;
        presetMaskString += ", ";
    }

    if (!presetMaskString.empty())
        presetMaskString = presetMaskString.substr(0, presetMaskString.size() - 2);

    return presetMaskString;
}

uint64 CustomMgr::GetPresetMaskFromString(std::string_view presetTypeMaskString) const
{
    uint64 presetTypeMask = 0;
    for (auto const& presetGroup : _presetGroups)
        if (presetTypeMaskString.find(presetGroup.second) != std::string::npos)
            presetTypeMask |= presetGroup.first;

    return presetTypeMask;
}

bool CustomMgr::IsValidPresetMask(uint64 presetTypeMask) const
{
    for (auto const& presetGroup : _presetGroups)
        if ((presetTypeMask & (1 << presetGroup.first)) != 0)
            return true;

    return false;
}

void CustomMgr::DisplayPresetGroupList(ChatHandler* handler) const
{
    if (!handler)
        return;

    if (_presetGroups.empty())
    {
        handler->SendSysMessage("There are no preset groups.");
        return;
    }

    handler->SendSysMessage("Displaying all preset groups...");
    for (auto const&[presetGroupID, presetGroupName] : _presetGroups)
        handler->SendFormattedSysMessage("{} [Mask value: {}] - {}", presetGroupID, 1 << presetGroupID, presetGroupName);
}

void CustomMgr::AddPresetGroup(ChatHandler* handler, std::string_view groupName, uint32 groupID)
{
    if (!handler)
        return;

    if (groupName.empty())
    {
        handler->SendSysMessage("You must specify a name for the preset group.");
        return;
    }

    if (groupName.size() > 50)
    {
        handler->SendSysMessage("The name of the preset group must be less than 50 characters.");
        return;
    }

    if (_presetGroups.contains(groupID))
    {
        handler->SendSysMessage("A preset group with this ID already exists.");
        return;
    }

    _presetGroups[groupID] = groupName;
    CustomDatabase.PExecute("REPLACE INTO `preset_groups` (`preset_group_id`, `preset_group_name`) VALUES (?, ?)", groupID, groupName);
    handler->SendFormattedSysMessage("Preset group with name {} and ID {} added.", groupName, groupID);
}

void CustomMgr::RemovePresetGroup(ChatHandler* handler, std::string_view groupName)
{
    if (!handler)
        return;

    if (groupName.empty())
    {
        handler->SendSysMessage("You must specify a name for the preset group.");
        return;
    }

    if (groupName.size() > 50)
    {
        handler->SendSysMessage("The name of the preset group must be less than 50 characters.");
        return;
    }

    auto it = std::ranges::find_if(_presetGroups, [groupName](auto const& presetGroup) { return presetGroup.second == groupName; });
    if (it == _presetGroups.end())
    {
        handler->SendSysMessage("A preset group with this name does not exist.");
        return;
    }

    _presetGroups.erase(it);
    CustomDatabase.PExecute("DELETE FROM `preset_groups` WHERE `GroupID` = ?", it->first);
    handler->SendFormattedSysMessage("Preset group with name {} and ID {} removed.", groupName, it->first);
}

void CustomMgr::AddPresetToNPC(ChatHandler* handler, Creature const* creature, uint64 presetTypeMask) const
{
    CreatureTemplate* creatureTemplate = const_cast<CreatureTemplate*>(creature->GetCreatureTemplate());
    creatureTemplate->PresetGossipFlags |= presetTypeMask;
    WorldDatabase.FMTExecute("UPDATE `creature_template` SET `PresetGossipFlags`=`PresetGossipFlags`|{} WHERE `entry`={}", creatureTemplate->PresetGossipFlags, creatureTemplate->Entry);
    handler->SendFormattedSysMessage("Preset type {} added to NPC with entry {}.", GetPresetMaskToString(presetTypeMask), creatureTemplate->Entry);
}

void CustomMgr::RemovePresetFromNPC(ChatHandler* handler, Creature const* creature, uint64 presetTypeMask) const
{
    CreatureTemplate* creatureTemplate = const_cast<CreatureTemplate*>(creature->GetCreatureTemplate());
    creatureTemplate->PresetGossipFlags &= ~presetTypeMask;
    WorldDatabase.FMTExecute("UPDATE `creature_template` SET `PresetGossipFlags`=`PresetGossipFlags`&~{} WHERE `entry`={}", presetTypeMask, creatureTemplate->Entry);
    handler->SendFormattedSysMessage("Preset type {} removed from NPC with entry {}.", GetPresetMaskToString(presetTypeMask), creatureTemplate->Entry);
}

void CustomMgr::OverrideNPCPreset(ChatHandler* handler, Creature const* creature, uint64 presetTypeMask) const
{
    CreatureTemplate* creatureTemplate = const_cast<CreatureTemplate*>(creature->GetCreatureTemplate());
    creatureTemplate->PresetGossipFlags = presetTypeMask;
    WorldDatabase.FMTExecute("UPDATE `creature_template` SET `PresetGossipFlags`={} WHERE `entry`={}", presetTypeMask, creatureTemplate->Entry);
    handler->SendFormattedSysMessage("Preset type {} overriden for NPC with entry {}.", GetPresetMaskToString(presetTypeMask), creatureTemplate->Entry);
}

bool CustomMgr::IsAscendedModeAllowedMapID(uint32 mapID) const
{
    return _ascendedModeAllowedMapIDs.contains(mapID);
}

void CustomMgr::HandleCooldownResetOnWipe(InstanceScript const* instance, Milliseconds encounterElapsedTime) const
{
    if (!GetBoolConfig(CONFIG_RESET_COOLDOWN_ON_WIPE))
        return;

    if (!instance)
        return;

    if (!instance->instance->IsRaid())
        return;

    if (encounterElapsedTime <= Seconds(GetIntConfig(CONFIG_RESET_COOLDOWN_REQUIRED_ENCOUNTER_TIME)))
        return;

    for (auto const& ref : instance->instance->GetPlayers())
    {
        Player* player = ref.GetSource();
        if (!player)
            continue;

        // TODO: remove with other rules than arena ones?
        player->RemoveArenaSpellCooldowns(true);
        for (uint32 const satedAuraID : { 57723, 57724 })
        {
            player->RemoveAurasDueToSpell(satedAuraID);
            if (Pet* pet = player->GetPet())
                pet->RemoveAurasDueToSpell(satedAuraID);
            else
            {
                // Clear auras in DB for pet classes
                switch (player->getClass())
                {
                    case CLASS_HUNTER:
                    case CLASS_WARLOCK:
                    case CLASS_DEATH_KNIGHT:
                    case CLASS_SHAMAN:
                    case CLASS_MAGE: 
                    {
                        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_AURA_BY_ID_AND_OWNER);
                        stmt->setUInt32(0, satedAuraID);
                        stmt->setUInt32(1, GUID_LOPART(player->GetGUID()));
                        CharacterDatabase.Execute(stmt);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
}

void CustomMgr::HandleCooldownResetOnEncounterDone(InstanceScript const* instance) const
{
    if (!GetBoolConfig(CONFIG_RESET_COOLDOWN_ON_ENCOUNTER_KILL))
        return;

    if (!instance)
        return;

    auto const enableMask = static_cast<EncounterResetOnKillOption>(GetIntConfig(CONFIG_RESET_COOLDOWN_ON_ENCOUNTER_KILL_MASK));
    if (enableMask == EncounterResetOnKillOption::None)
        return;

    if (instance->instance->IsDungeon() && (to_underlying(enableMask) & to_underlying(EncounterResetOnKillOption::Dungeon)) == 0)
        return;

    if (instance->instance->IsRaid() && (to_underlying(enableMask) & to_underlying(EncounterResetOnKillOption::Raid)) == 0)
        return;

    for (auto const& ref : instance->instance->GetPlayers())
    {
        Player* player = ref.GetSource();
        if (!player || !player->IsAlive())
            continue;

        player->RemoveArenaSpellCooldowns(true);
        for (uint32 const satedAuraID : { 57723, 57724 })
        {
            player->RemoveAurasDueToSpell(satedAuraID);
            if (Pet* pet = player->GetPet())
                pet->RemoveAurasDueToSpell(satedAuraID);
        }
    }
}

std::string SendMailOptions::ToString() const
{
    std::ostringstream str;

    str << fmt::format("CharacterID: {}\n", CharacterID);

    if (MoneyAmount > 0)
        str << fmt::format("MoneyAmount: {}\n", MoneyAmount);

    for (auto const& [itemEntry, itemData] : MailItems)
        str << fmt::format("ItemEntry: {}, ItemCount: {}{}\n", itemEntry, itemData.Count, itemData.RandomEnchantID ? fmt::format(" RandomPropertyID: {}", itemData.RandomEnchantID.value()) : "");

    return str.str();
}

template <HasEmplaceBack Container>
void CustomMgr::TokenizeConfigStringValueFromFile(Container& container, std::string const& propertyName, std::string const& defaultValue /* = "" */, char const separator /* = ';' */)
{
    container.clear();
    using ContainerValueType = typename Container::value_type;

    std::string configValue = sConfigMgr->GetStringDefault(propertyName, defaultValue);
    if (configValue.empty())
        return;

    std::erase_if(configValue, [](char const c) { return isspace(c); });
    Tokenizer const tokens(configValue, separator);
    for (char const* token : tokens)
    {
        if constexpr (std::is_same_v<ContainerValueType, std::string>)
        {
            ContainerValueType valueStr(token);
            // transform to lowercase
            std::ranges::transform(valueStr, valueStr.begin(), ::tolower);
            container.emplace_back(std::move(valueStr));
        }
        else
        {
            if (auto const valueOptional = char_cast<ContainerValueType>(token))
                if (ContainerValueType const newValue = valueOptional.value())
                    container.emplace_back(newValue);
        }
    }

    if (!container.empty())
        std::sort(std::begin(container), std::end(container));
}

template <HasEmplace Container>
void CustomMgr::TokenizeConfigStringValueFromFile(Container& container, std::string const& propertyName, std::string const& defaultValue /* = "" */, char const separator /* = ';' */)
{
    container.clear();
    using ContainerValueType = typename Container::value_type;

    std::string configValue = sConfigMgr->GetStringDefault(propertyName, defaultValue);
    if (configValue.empty())
        return;

    std::erase_if(configValue, [](char const c) { return isspace(c); });
    Tokenizer const tokens(configValue, separator);
    for (char const* token : tokens)
    {
        if constexpr (std::is_same_v<ContainerValueType, std::string>)
        {
            ContainerValueType valueStr(token);
            // transform to lowercase
            std::ranges::transform(valueStr, valueStr.begin(), ::tolower);
            container.emplace(std::move(valueStr));
        }
        else
        {
            if (auto const valueOptional = char_cast<ContainerValueType>(token))
                if (ContainerValueType const newValue = valueOptional.value())
                    container.emplace(newValue);
        }
    }
}