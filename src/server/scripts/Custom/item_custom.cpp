

/* ScriptData
SDName: item_custom
SD%Complete: 100
SDComment: Items for a range of different items. See content below (in script)
SDCategory: NPCs
EndScriptData
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "Player.h"
#include "utf8.h"
#include "PremiumStoreMgr.h"

/*#####
# item_mysterious_time_pack
# Not used right now.

class item_mysterious_time_pack : public ItemScript
{
public:
    item_mysterious_time_pack() : ItemScript("item_mysterious_time_pack") { }

    bool OnExpire(Player* player, ItemTemplate const* pItemProto)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 82000, 1); // Mysterious Time Pack
        if (msg == EQUIP_ERR_OK)
            player->StoreNewItem(dest, 82000, true, Item::GenerateItemRandomPropertyId(82000));

        return true;
    }
};

#####*/

enum MultipleSpecsItemData
{
    MULTIPLE_SPECS_MESSAGE_MAIN       = 1001000,
    MULTIPLE_SPECS_MESSAGE_RENAME     = 1001001,
    MULTIPLE_SPECS_MESSAGE_LEARN_MORE = 1001002,

    GOSSIP_ACTION_EXIT        = GOSSIP_ACTION_INFO_DEF + 100,
    GOSSIP_ACTION_RENAME_LIST = GOSSIP_ACTION_INFO_DEF + 101,

    GOSSIP_ACTION_RENAME_1 = GOSSIP_ACTION_INFO_DEF + 102,
    GOSSIP_ACTION_RENAME_2 = GOSSIP_ACTION_INFO_DEF + 103,
    GOSSIP_ACTION_RENAME_3 = GOSSIP_ACTION_INFO_DEF + 104,
    GOSSIP_ACTION_RENAME_4 = GOSSIP_ACTION_INFO_DEF + 105,

    GOSSIP_ACTION_MAIN_MENU = GOSSIP_ACTION_INFO_DEF + 106,

    GOSSIP_ACTION_LEARN_MORE = GOSSIP_ACTION_INFO_DEF + 107
};

constexpr std::array<std::string_view, MAX_TALENT_SPECS / 2> SpecIcons{
    "|TInterface/ICONS/achievement_arena_5v5_1:30:30:-18:0|t",
    "|TInterface/ICONS/achievement_arena_5v5_3:30:30:-18:0|t",
    "|TInterface/ICONS/achievement_arena_5v5_4:30:30:-18:0|t",
    "|TInterface/ICONS/achievement_arena_5v5_5:30:30:-18:0|t",
};

class item_custom_multiple_specs : public ItemScript
{
public:
    item_custom_multiple_specs()
        : ItemScript("item_custom_multiple_specs")
    {
    }

    bool OnUse(Player* player, ItemRef const& item, SpellCastTargets const& /*targets*/) override
    {
        ClearGossipMenuFor(player);
        if (!sCustomMgr->GetBoolConfig(CONFIG_MULTISPEC_ENABLED))
        {
            ChatHandler(player->GetSession()).SendSysMessage("Multispec is disabled!");
            return false;
        }

        // Teach Dual Talent if player doesn't have it.
        if (player->GetSpecsCount() == 1 && player->getLevel() >= sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL))
        {
            player->CastSpell(player, 63680, true, nullptr, nullptr, player->GetGUID());
            player->CastSpell(player, 63624, true, nullptr, nullptr, player->GetGUID());
        }

        if (uint8 const currentSpecsCount = player->GetSpecsCount(); currentSpecsCount > 1)
        {
            if (CustomData const* customData = sCustomMgr->GetCustomData(player->GetGUIDLow()))
            {
                uint8 const currentSpecsGroup = player->GetSpecsGroup();
                AddGossipItemFor(player, GOSSIP_ICON_DOT, fmt::format("{} Mastery I: {} {}", SpecIcons[0], player->GetSpecName(0), currentSpecsGroup == 0 ? "(Active) " : "").c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

                uint8 newSpecsCount = 0;
                if (uint32 const multipleSpecsFlags = customData->MultipleSpecsFlag)
                {
                    if (multipleSpecsFlags & MULTIPLE_SPECS_FLAG_GROUP_1)
                    {
                        newSpecsCount = 2 * CLIENT_MAX_TALENT_SPECS;
                        AddGossipItemFor(player, GOSSIP_ICON_DOT, fmt::format("{} Mastery II: {} {}", SpecIcons[1], player->GetSpecName(1), currentSpecsGroup == 1 ? "(Active) " : "").c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    }

                    if (multipleSpecsFlags & MULTIPLE_SPECS_FLAG_GROUP_2)
                    {
                        newSpecsCount = 3 * CLIENT_MAX_TALENT_SPECS;
                        AddGossipItemFor(player, GOSSIP_ICON_DOT, fmt::format("{} Mastery III: {} {}", SpecIcons[2], player->GetSpecName(2), currentSpecsGroup == 2 ? "(Active) " : "").c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    }

                    if (multipleSpecsFlags & MULTIPLE_SPECS_FLAG_GROUP_3)
                    {
                        newSpecsCount = 4 * CLIENT_MAX_TALENT_SPECS;
                        AddGossipItemFor(player, GOSSIP_ICON_DOT, fmt::format("{} Mastery IV: {} {}", SpecIcons[3], player->GetSpecName(3), currentSpecsGroup == 3 ? "(Active) " : "").c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    }
                }

                if (newSpecsCount && newSpecsCount != currentSpecsCount)
                    player->UpdateSpecCount(newSpecsCount);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Rename my Masteries.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RENAME_LIST);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Learn more about Masteries.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_LEARN_MORE);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Exit.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_EXIT);
        SendGossipMenuFor(player, MULTIPLE_SPECS_MESSAGE_MAIN, item->GetGUID());
        return true;
    }

    void HandleGossipRename(Player* player, ItemRef const& item) const
    {
        ClearGossipMenuFor(player);
        if (CustomData const* customData = sCustomMgr->GetCustomData(player->GetGUIDLow()))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, fmt::format("Rename Mastery I: {}", player->GetSpecName(0)).c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RENAME_1, "Enter your Mastery name and press accept (don't press enter)", 0, true);
            if (uint32 const multipleSpecsFlags = customData->MultipleSpecsFlag)
            {
                if (multipleSpecsFlags & MULTIPLE_SPECS_FLAG_GROUP_1)
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, fmt::format("Rename Mastery II: {}", player->GetSpecName(1)).c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RENAME_2, "Enter your Mastery name and press accept (don't press enter)", 0, true);

                if (multipleSpecsFlags & MULTIPLE_SPECS_FLAG_GROUP_2)
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, fmt::format("Rename Mastery III: {}", player->GetSpecName(2)).c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RENAME_3, "Enter your Mastery name and press accept (don't press enter)", 0, true);

                if (multipleSpecsFlags & MULTIPLE_SPECS_FLAG_GROUP_3)
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, fmt::format("Rename Mastery IV: {}", player->GetSpecName(3)).c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RENAME_4, "Enter your Mastery name and press accept (don't press enter)", 0, true);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_MAIN_MENU);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Exit.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_EXIT);
        SendGossipMenuFor(player, MULTIPLE_SPECS_MESSAGE_RENAME, item->GetGUID()); // TODO: doesn't work with MULTIPLE_SPECS_MESSAGE, dunno why
    }

    void HandleGossipLearnMore(Player* player, ItemRef const& item) const
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_MAIN_MENU);
        SendGossipMenuFor(player, MULTIPLE_SPECS_MESSAGE_LEARN_MORE, item->GetGUID()); // TODO: doesn't work with MULTIPLE_SPECS_MESSAGE, dunno why
    }

    void OnGossipSelect(Player* player, ItemRef const& item, uint32 /*sender*/, uint32 action) override
    {
        if (!player || !player->IsInWorld() || !item || !action)
            return;

        if (!sCustomMgr->DoBasicChecksForCommandOrItem(player))
        {
            CloseGossipMenuFor(player);
            return;
        }

        if (action == GOSSIP_ACTION_EXIT)
        {
            CloseGossipMenuFor(player);
            return;
        }

        if (action == GOSSIP_ACTION_MAIN_MENU)
        {
            OnUse(player, item, SpellCastTargets());
            return;
        }

        if (action == GOSSIP_ACTION_RENAME_LIST)
        {
            HandleGossipRename(player, item);
            return;
        }

        if (action == GOSSIP_ACTION_LEARN_MORE)
        {
            HandleGossipLearnMore(player, item);
            return;
        }

        if (action < GOSSIP_ACTION_INFO_DEF)
        {
            CloseGossipMenuFor(player);
            return;
        }

        uint8 const specsGroup = action - GOSSIP_ACTION_INFO_DEF;
        if (specsGroup == player->GetSpecsGroup())
        {
            ChatHandler(player->GetSession()).SendSysMessage("This Mastery is already active!");
            CloseGossipMenuFor(player);
            return;
        }

        player->SetSpecsGroup(specsGroup);
        player->ActivateSpec(specsGroup * CLIENT_MAX_TALENT_SPECS);
        std::string_view const specName = player->GetSpecName(specsGroup);
        ChatHandler(player->GetSession()).SendFormattedSysMessage("Activated Mastery: {}", !specName.empty() ? specName : "<unnamed>");
        CloseGossipMenuFor(player);
    }

    void OnGossipSelectCode(Player* player, ItemRef const& item, uint32 sender, uint32 action, const char* code) override
    {
        auto const specID = [action]() -> uint32 
        {
            switch (action)
            {
                case GOSSIP_ACTION_RENAME_1: return 0;
                case GOSSIP_ACTION_RENAME_2: return 1;
                case GOSSIP_ACTION_RENAME_3: return 2;
                case GOSSIP_ACTION_RENAME_4: return 3;
                default: return std::numeric_limits<uint32>::max();
            }
        }();

        if (specID == std::numeric_limits<uint32>::max())
        {
            CloseGossipMenuFor(player);
            return;
        }

        std::string text = code;
        if (text.empty())
        {
            ChatHandler(player->GetSession()).SendSysMessage("Empty name!");
            CloseGossipMenuFor(player);
            return;
        }

        if (!utf8::is_valid(text.begin(), text.end()) || text == "Unnamed")
        {
            ChatHandler(player->GetSession()).SendSysMessage("Invalid name!");
            CloseGossipMenuFor(player);
            return;
        }

        if (text.length() > 10)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Name too long!");
            CloseGossipMenuFor(player);
            return;
        }

        ChatHandler(player->GetSession()).SendFormattedSysMessage("Mastery name changed to \"{}\"", text.c_str());
        player->SetSpecName(specID, std::move(text));
        HandleGossipRename(player, item);
    }
};

enum PremiumToken
{
    PREMIUM_CUSTOM_SPELL        = 53374,

    CHECK_SKILL_ONE_HAND_AXE    = 196,
    CHECK_SKILL_TWO_HAND_AXE    = 197,
    CHECK_SKILL_ONE_HAND_MACE   = 198,
    CHECK_SKILL_TWO_HAND_MACE   = 199,
    CHECK_SKILL_POLEARMS        = 200,
    CHECK_SKILL_ONE_HAND_SWORD  = 201,
    CHECK_SKILL_TWO_HAND_SWORD  = 202,
    CHECK_SKILL_STAVES          = 227,
    CHECK_SKILL_BOWS            = 264,
    CHECK_SKILL_GUNS            = 266,
    CHECK_SKILL_DAGGERS         = 1180,
    CHECK_SKILL_THROWNS         = 2567,
    CHECK_SKILL_WANDS           = 5009,
    CHECK_SKILL_CROSSBOWS       = 5011,
    CHECK_SKILL_FIST            = 15590,
    CHECK_SKILL_PLATE           = 750,
    CHECK_SKILL_MAIL            = 8737
};

inline const uint32 TokenBags[3][4]
{
    { 4500, 4500, 4500, 4500 }, // 4x Traveler's Backpack
    { 4500, 4500, 4500, 2663 }, // 3x Traveler's Backpack + Ribbly's Bandolier
    { 4500, 4500, 4500, 22244 } // 3x Traveler's Backpack + Box of Souls
};

inline const int32 TokenMounts[11]
{
    // Mount
    18778, // HUMAN
    18797, // ORC
    18785, // DWARF
    18902, // NIGHT ELF
    13334, // UNDEAD
    18793, // TAUREN
    18774, // GNOME
    18790, // TROLL
    66666, // GOBLIN - NOT USED
    29224, // BLOOD ELF
    29745  // DRAENEI
};

enum tokenRiding
{
    RIDING_1 = 33388,
    RIDING_2 = 33391
};

enum premiumMisc
{
    ITEM_BULLETS         = 41584,
    BULLETS_COUNT        = 500,
    PROFESSION_HEIGHT    = 15,
    PROFESSION_WIDTH     = 7,
    ITEM_SET_HEIGHT      = 23,
    ITEM_SET_WIDTH       = 17,

    PREMIUM_TOKEN_BOOST_LEVEL_ITEM_ID      = 41605,
    PREMIUM_TOKEN_BOOST_PROFESSION_ITEM_ID = 41606
};

inline const std::vector<uint32> PaladinHolyBoostSet         = { 37062, 38613, 35643, 37048, 36974, 36977, 37112, 37061, 38364, 41356, 39535, 35678, 37056, 37058, 44334, 37111, 44013 };        // Paladin: Holy
inline const std::vector<uint32> PaladinProtectionBoostSet = { 36969, 37053, 37115, 37084, 37052, 37040, 36984, 37107, 38363, 41357, 36952, 43353, 37082, 36961, 43306, 36993, 44015 }; // Paladin: Protection
inline const std::vector<uint32> PaladinRetributionBoostSet  = { 41344, 43309, 39534, 38614, 36950, 35593, 37108, 38362, 36995, 37088, 37066, 37068, 35683, 43251, 37064, 44014 };               // Paladin: Retribution

inline const std::vector<uint32> WarriorDPSBoostSet = { 41344, 43309, 39534, 38614, 36950, 35593, 37108, 36962, 36981, 36995, 37088, 37066, 37068, 35683, 43251, 37064, 44014 }; // Warrior: Dps
inline const std::vector<uint32> WarriorTankBoostSet = { 36969, 37053, 37115, 37084, 37052, 37040, 36984, 37107, 43187, 41357, 36952, 43353, 37082, 36961, 43306, 36993, 37064 }; // Warrior: Tank

inline const std::vector<uint32> HunterBoostSet = { 36971, 37096, 44372, 36947, 35675, 43131, 36962, 36994, 37043, 43167, 35585, 44438, 36979, 35683, 37064, 44014 }; // Hunter

inline const std::vector<uint32> RogueAssasinationBoostSet = { 40291, 37096, 35591, 38614, 37057, 44036, 36944, 36944, 36981, 35647, 36978, 35676, 43172, 43178, 43251, 37064, 44014 }; // Rogue: Assassination
inline const std::vector<uint32> RogueCombatBoostSet       = { 40291, 37096, 35591, 38614, 37057, 44036, 37065, 37065, 36981, 35647, 36978, 35676, 43172, 43178, 43251, 37064, 44014 }; // Rogue: Combat
inline const std::vector<uint32> RogueSubtletyBoostSet     = { 40291, 37096, 35591, 38614, 37057, 44036, 36944, 37065, 36981, 35647, 36978, 35676, 43172, 43178, 43251, 37064, 44014 }; // Rogue: Subtlety

inline const std::vector<uint32> PriestHolyBoostSet  = { 36985, 36943, 36982, 37048, 36991, 36945, 36975, 37038, 36949, 35594, 36976, 36954, 37079, 38611, 37111, 44013 }; // Priest: Holy
inline const std::vector<uint32> PriestDisciplineBoostSet = { 35679, 36943, 36982, 44020, 36973, 44338, 36975, 37038, 36949, 35663, 36976, 36954, 37079, 43252, 37111, 44013 }; // Priest: Discipline
inline const std::vector<uint32> PriestShadowBoostSet     = { 44019, 44017, 37055, 36983, 42844, 37760, 37099, 36989, 35646, 36997, 43375, 36954, 43253, 43177, 36972, 38765 }; // Priest: Shadow

inline const std::vector<uint32> DeathKnightTankBoostSet = { 36969, 37053, 37115, 37084, 37052, 37040, 37108, 40822, 41357, 36952, 43353, 42768, 36961, 43306, 36993, 37064 };        // Death Knight: Tank
inline const std::vector<uint32> DeathKnightUnholyBoostSet = { 41344, 43309, 39534, 36947, 36950, 35593, 37108, 40867, 37789, 37088, 37066, 35614, 35683, 43251, 37064, 44014 };        // Death Knight: Unholy
inline const std::vector<uint32> DeathKnightFrostBoostSet  = { 41344, 37823, 39534, 36947, 36950, 35593, 37065, 37065, 40875, 36995, 37088, 37066, 37082, 35683, 43251, 37064, 44014 }; // Death Knight: Frost

inline const std::vector<uint32> ShamanRestorationBoostSet = { 35672, 38613, 37116, 37048, 36946, 37069, 37112, 37061, 38368, 37110, 43133, 36986, 37105, 37058, 35589, 37111, 44015 }; // Shaman: Resto
inline const std::vector<uint32> ShamanEnhancementBoostSet = { 36971, 37096, 44372, 36947, 35675, 43131, 41824, 36944, 38367, 37043, 43167, 35585, 44438, 36979, 35683, 37064, 44014 }; // Shaman: Enhancement
inline const std::vector<uint32> ShamanElementalBoostSet   = { 35672, 44017, 37116, 36983, 36946, 37069, 37112, 37061, 38361, 37110, 43133, 36986, 35650, 43253, 43177, 36972, 38765 }; // Shaman: Elemental

inline const std::vector<uint32> MageBoostSet = { 44019, 44017, 37055, 36983, 42844, 37760, 37099, 36989, 35646, 36997, 43375, 36954, 43253, 43177, 36972, 38765 };    // Mage
inline const std::vector<uint32> WarlockBoostSet = { 44019, 44017, 37055, 36983, 42844, 37760, 37099, 36989, 35646, 36997, 43375, 36954, 43253, 43177, 36972, 38765 }; // Warlock

inline const std::vector<uint32> DruidBalanceBoostSet = { 36948, 44017, 43258, 36983, 35677, 37000, 37099, 38360, 37114, 37106, 37083, 37070, 43253, 43177, 36972, 38765 }; // Druid: Balance
inline const std::vector<uint32> DruidFeralBoostSet   = { 40291, 37096, 35591, 38614, 37057, 44036, 36980, 38365, 35647, 36978, 35676, 43172, 43178, 43251, 37064, 44014 }; // Druid: Feral
inline const std::vector<uint32> DruidRestorationBoostSet = { 35620, 36943, 44371, 37048, 43257, 44339, 36975, 38366, 37114, 44041, 43201, 35641, 37079, 38611, 37111, 44015 }; // Druid: Restoration

enum PremiumTokenMessages
{
    PREMIUM_TOKEN_MESSAGE                       = 1000001,

    PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_0    = 1000100,
    PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_1    = 1000101,
    PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_2    = 1000102,
    PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_3    = 1000103,

    BOOST_LEVEL_BOOST_PROFESSION_MESSAGE        = 1000200,

    GOSSIP_INFO_DEF_EXIT                        = 5000,
    GOSSIP_INFO_DEF_SET_1                       = 1100,
    GOSSIP_INFO_DEF_SET_2,
    GOSSIP_INFO_DEF_SET_3
};

class item_custom_premium_token_boost_level : public ItemScript
{
public:
    item_custom_premium_token_boost_level()
        : ItemScript("item_custom_premium_token_boost_level")
    {
    }

    bool OnUse(Player* player, ItemRef const& item, SpellCastTargets const& /*targets*/) override
    {
        ClearGossipMenuFor(player);
        CustomData* customData = sCustomMgr->GetCustomData(player->GetGUIDLow());
        if (!customData)
            return false;

        switch (customData->TokenUsePhase)
        {
            case to_underlying(TokenPhases::NotStarted):
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "I agree and want to use the token on this character.", GOSSIP_SENDER_MAIN, to_underlying(TokenPhases::NotStarted));
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Exit.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_EXIT);
                SendGossipMenuFor(player, PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_0, item->GetGUID());
                break;
            case to_underlying(TokenPhases::LevelUp):
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Process to next step (Level Up).", GOSSIP_SENDER_MAIN, to_underlying(TokenPhases::LevelUp));
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Exit.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_EXIT);
                SendGossipMenuFor(player, PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_1, item->GetGUID());
                break;
            case to_underlying(TokenPhases::Riding):
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Process to next step (Riding).", GOSSIP_SENDER_MAIN, to_underlying(TokenPhases::Riding));
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Exit.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_EXIT);
                SendGossipMenuFor(player, PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_2, item->GetGUID());
                break;
            case to_underlying(TokenPhases::ItemSet):
            {
                switch (player->getClass())
                {
                    case CLASS_PALADIN:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Holy.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Protection.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Retribution.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_3);
                        break;
                    case CLASS_WARRIOR:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Dps.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Tank.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        break;
                    case CLASS_HUNTER:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Dps.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        break;
                    case CLASS_ROGUE:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Assassination.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Combat.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Subtlety.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_3);
                        break;
                    case CLASS_PRIEST:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Holy.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Discipline.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Shadow.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_3);
                        break;
                    case CLASS_DEATH_KNIGHT:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Tank.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Unholy.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Frost.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_3);
                        break;
                    case CLASS_SHAMAN:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Restoration.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Enhancement.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Elemental.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_3);
                        break;
                    case CLASS_MAGE:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Dps.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        break;
                    case CLASS_WARLOCK:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Dps.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        break;
                    case CLASS_DRUID:
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Balance.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_1);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Feral.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_2);
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Item Set: Restoration.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_SET_3);
                        break;
                }

                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Exit.", GOSSIP_SENDER_MAIN, GOSSIP_INFO_DEF_EXIT);
                SendGossipMenuFor(player, PREMIUM_TOKEN_BOOST_LEVEL_MESSAGE_STEP_3, item->GetGUID());
                break;
            }
            case to_underlying(TokenPhases::Done):
                player->DestroyItemCount(PREMIUM_TOKEN_BOOST_LEVEL_ITEM_ID, 1, true, false);
                CloseGossipMenuFor(player);
                return false;
            default:
                break;
        }

        return true;
    }

    void OnGossipSelect(Player* player, ItemRef const& item, uint32 /*sender*/, uint32 action) override
    {
        if (!player->IsInWorld())
            return;

        ClearGossipMenuFor(player);
        if (!sCustomMgr->CheckPlayerDataForToken(player))
        {
            CloseGossipMenuFor(player);
            return;
        }

        if (!sCustomMgr->CheckPlayerDataForToken(player, PREMIUM_CUSTOM_SPELL, true))
        {
            OnUse(player, item, SpellCastTargets());
            return;
        }

        CustomData* customData = sCustomMgr->GetCustomData(player->GetGUIDLow());
        if (!customData)
            return;

        switch (action)
        {
            case to_underlying(TokenPhases::NotStarted):
                customData->TokenUsePhase = to_underlying(TokenPhases::LevelUp);
                break;
            case to_underlying(TokenPhases::LevelUp):
                LevelUp(player);
                customData->TokenUsePhase = to_underlying(TokenPhases::Riding);
                break;
            case to_underlying(TokenPhases::Riding):
                AddBagsAndTeachRiding(player);
                customData->TokenUsePhase = to_underlying(TokenPhases::ItemSet);
                break;
            case GOSSIP_INFO_DEF_SET_1:
            case GOSSIP_INFO_DEF_SET_2:
            case GOSSIP_INFO_DEF_SET_3:
                AddSet(player, action - GOSSIP_INFO_DEF_SET_1);
                customData->TokenUsePhase = to_underlying(TokenPhases::Done);
                ChatHandler(player->GetSession()).SendSysMessage("Congratulations! Your Level 80 boost is now finished. You can now grab items from the mailbox.");
                break;
            case GOSSIP_INFO_DEF_EXIT:
                CloseGossipMenuFor(player);
                return;
            default:
                break;
        }
                    
        player->CastSpell(player, PREMIUM_CUSTOM_SPELL, false);
        sCustomMgr->SaveCharacterCustomData(player->GetGUIDLow(), false);
        ClearGossipMenuFor(player);
        OnUse(player, item, SpellCastTargets());
    }

private:
    bool LevelUp(Player* player) const
    {
        player->GiveLevel(80);
        player->InitTalentForLevel();
        player->SetUInt32Value(PLAYER_XP, 0);

        // Update level and reset XP, everything else will be updated at login
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LEVEL);
        stmt->setUInt8(0, uint8(80));
        stmt->setUInt32(1, player->GetGUIDLow());
        CharacterDatabase.Execute(stmt);

        sWorld->UpdateGlobalPlayerData(GUID_LOPART(player->GetGUID()), PLAYER_UPDATE_DATA_LEVEL, "", 80);

        TeachWeaponSkills(player);
        player->UpdateSkillsToMaxSkillsForLevel();

        return true;
    }

    void LearnWeaponSkillsForSingleClass(Player* player, std::initializer_list<uint32> const& skillIDs) const
    {
        for (uint32 const skillID : skillIDs)
            if (!player->HasSpell(skillID))
                player->learnSpell(skillID);
    }

    void TeachWeaponSkills(Player* player) const
    {
        switch (player->getClass())
        {
            case CLASS_DRUID:
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_TWO_HAND_MACE, CHECK_SKILL_POLEARMS, CHECK_SKILL_DAGGERS, CHECK_SKILL_STAVES });
                break;
            case CLASS_HUNTER:
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_ONE_HAND_AXE, CHECK_SKILL_TWO_HAND_SWORD, CHECK_SKILL_TWO_HAND_AXE, CHECK_SKILL_POLEARMS, 
                    CHECK_SKILL_DAGGERS, CHECK_SKILL_BOWS, CHECK_SKILL_GUNS, CHECK_SKILL_CROSSBOWS, CHECK_SKILL_STAVES, CHECK_SKILL_MAIL });
                break;
            case CLASS_MAGE: 
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_DAGGERS, CHECK_SKILL_STAVES, CHECK_SKILL_WANDS, CHECK_SKILL_STAVES });
                break;
            case CLASS_PALADIN:
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_ONE_HAND_AXE, CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_TWO_HAND_SWORD, CHECK_SKILL_TWO_HAND_AXE, 
                    CHECK_SKILL_TWO_HAND_MACE, CHECK_SKILL_POLEARMS, CHECK_SKILL_PLATE });
                break;
            case CLASS_PRIEST: 
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_DAGGERS, CHECK_SKILL_STAVES, CHECK_SKILL_WANDS, CHECK_SKILL_STAVES });
                break;
            case CLASS_ROGUE:
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_AXE, CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_DAGGERS, CHECK_SKILL_THROWNS, CHECK_SKILL_CROSSBOWS });
                break;
            case CLASS_SHAMAN:
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_FIST, CHECK_SKILL_DAGGERS, CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_STAVES, CHECK_SKILL_ONE_HAND_AXE, CHECK_SKILL_TWO_HAND_AXE, CHECK_SKILL_MAIL });
                break;
            case CLASS_WARLOCK: 
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_DAGGERS, CHECK_SKILL_DAGGERS, CHECK_SKILL_STAVES, CHECK_SKILL_WANDS });
                break;
            case CLASS_WARRIOR: 
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_AXE, CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_TWO_HAND_AXE, CHECK_SKILL_TWO_HAND_MACE, 
                    CHECK_SKILL_TWO_HAND_SWORD, CHECK_SKILL_POLEARMS, CHECK_SKILL_BOWS, CHECK_SKILL_THROWNS, CHECK_SKILL_GUNS, CHECK_SKILL_CROSSBOWS, CHECK_SKILL_PLATE });
                break;
            case CLASS_DEATH_KNIGHT: 
                LearnWeaponSkillsForSingleClass(player, { CHECK_SKILL_ONE_HAND_AXE, CHECK_SKILL_ONE_HAND_MACE, CHECK_SKILL_ONE_HAND_SWORD, CHECK_SKILL_TWO_HAND_AXE, CHECK_SKILL_TWO_HAND_MACE, 
                    CHECK_SKILL_TWO_HAND_SWORD, CHECK_SKILL_POLEARMS });
                break;
            default:
                break;
        }
    }

    void AddBagsAndTeachRiding(Player* player) const
    {
        switch (player->getClass())
        {
            case CLASS_HUNTER: 
            {
                for (uint8 i = 0; i < 4; i++)
                    if (TokenBags[1][i] != 0)
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItem(TokenBags[1][i]));
            }
            break;
            case CLASS_WARLOCK: 
            {
                for (uint8 i = 0; i < 4; i++)
                    if (TokenBags[2][i] != 0)
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItem(TokenBags[2][i]));
            }
            break;
            default: 
            {
                for (uint8 i = 0; i < 4; i++)
                    if (TokenBags[0][i] != 0)
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItem(TokenBags[0][i]));
            }
            break;
        }

        player->learnSpell(RIDING_1);
        player->learnSpell(RIDING_2);

        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItem(TokenMounts[player->getRace() - 1]));

        if (player->getClass() == CLASS_HUNTER)
            sCustomMgr->AddItemWihoutNotify(player, ITEM_BULLETS, BULLETS_COUNT);
    }

    void AddSet(Player const* player, uint8 const setID) const
    {
        switch (player->getClass())
        {
            case CLASS_PALADIN:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(PaladinHolyBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(PaladinProtectionBoostSet));
                        break;
                    case 2:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(PaladinRetributionBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            case CLASS_WARRIOR:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(WarriorDPSBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(WarriorTankBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            case CLASS_HUNTER:
                sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(HunterBoostSet));
                break;
            case CLASS_ROGUE:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(RogueAssasinationBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(RogueCombatBoostSet));
                        break;
                    case 2:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(RogueSubtletyBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            case CLASS_PRIEST:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(PriestHolyBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(PriestDisciplineBoostSet));
                        break;
                    case 2:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(PriestShadowBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            case CLASS_DEATH_KNIGHT:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(DeathKnightTankBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(DeathKnightUnholyBoostSet));
                        break;
                    case 2:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(DeathKnightFrostBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            case CLASS_SHAMAN:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(ShamanRestorationBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(ShamanEnhancementBoostSet));
                        break;
                    case 2:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(ShamanElementalBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            case CLASS_MAGE:
                sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(MageBoostSet));
                break;
            case CLASS_WARLOCK:
                sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(WarlockBoostSet));
                break;
            case CLASS_DRUID:
                switch (setID)
                {
                    case 0:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(DruidFeralBoostSet));
                        break;
                    case 1:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(DruidFeralBoostSet));
                        break;
                    case 2:
                        sCustomMgr->SendMail(SendMailOptions(player->GetGUIDLow()).AddItems(DruidRestorationBoostSet));
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
};

enum TokenProfessions
{
    ENCHANTING      = 0,
    ALCHEMY         = 1,
    BLACKSMITHING   = 2,
    ENGINEERING     = 3,
    HERBALISM       = 4,
    LEATHERWORKING  = 5,
    MINING          = 6,
    SKINNING        = 7,
    TAILORING       = 8,
    JEWELCRAFTING   = 9,
    INSCRIPTION     = 10,
    // Secondary
    COOKING         = 11,
    FISHING         = 12,
    FIRST_AID       = 13,
    // Class only
    LOCKPICKING     = 14
};

inline const uint32 TokenProfessionSkill[PROFESSION_HEIGHT]
{
    // Skill Spell
    333, // Enchanting
    171, // Alchemy
    164, // Blacksmithing
    202, // Engineering
    182, // Herbalism
    165, // Leatherworking
    186, // Mining
    393, // Skinning
    197, // Tailoring
    755, // Jewelcrafting
    773, // Inscription
         // Secondary
    185, // Cooking
    356, // Fishing
    129, // First Aid
         // Class only
    633 // Lockpicking
};

inline const int32 TokenProfessionSpells[PROFESSION_HEIGHT][PROFESSION_WIDTH]
{
    // Skill Spell
    { 333, 7411, 7412, 7413, 13920, 28029, 51313 },    // Enchanting
    { 171, 2259, 3101, 3464, 11611, 28596, 51304 },    // Alchemy
    { 164, 2018, 3100, 3538, 9785, 29844, 51300 },     // Blacksmithing
    { 202, 4036, 4037, 4038, 12656, 30350, 51306 },    // Engineering
    { 182, 2366, 2368, 3570, 11993, 28695, 50300 },    // Herbalism
    { 165, 2108, 3104, 3811, 10662, 32549, 51302 },    // Leatherworking
    { 186, 2575, 2576, 3564, 10248, 29354, 50310 },    // Mining
    { 393, 8613, 8617, 8618, 10768, 32678, 50305 },    // Skinning
    { 197, 3908, 3909, 3910, 12180, 26790, 51309 },    // Tailoring
    { 755, 25229, 25230, 28894, 28895, 28897, 51311 }, // Jewelcrafting
    { 773, 45357, 45358, 45359, 45360, 45361, 45363 }, // Inscription
                                                       // Secondary
    { 185, 2550, 3102, 3413, 18260, 33359, 51296 }, // Cooking
    { 356, 7620, 7731, 7732, 18248, 33095, 51294 }, // Fishing
    { 129, 3273, 3274, 7924, 10846, 27028, 45542 }, // First Aid
                                                    // Class only
    { 633, 1809, 1810, 6460, 0, 0, 0 } // Lockpicking
};

class item_custom_premium_token_boost_profession : public ItemScript
{
    public:
        item_custom_premium_token_boost_profession()
            : ItemScript("item_custom_premium_token_boost_profession")
        {
        }

        bool OnUse(Player* player, ItemRef const& item, SpellCastTargets const& /*targets*/) override
        {
            ClearGossipMenuFor(player);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Enchanting.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Alchemy.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Blacksmithing.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Engineering.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Herbalism.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Leatherworking.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Mining.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Skinning.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Tailoring.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Jewelcrafting.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Inscription.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Cooking.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Fishing.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: First Aid.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);

            if (player->getClass() == CLASS_ROGUE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Profession: Lockpicking.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);

            SendGossipMenuFor(player, BOOST_LEVEL_BOOST_PROFESSION_MESSAGE, item->GetGUID());
            return true;
        }

        void OnGossipSelect(Player* player, ItemRef const& item, uint32 /*sender*/, uint32 action) override
        {
            if (!player || !player->IsInWorld() || !item || !action)
                return;

            ClearGossipMenuFor(player);

            uint32 professionId  = action - GOSSIP_ACTION_INFO_DEF - 1;
            bool professionCheck = professionId <= INSCRIPTION && player->GetFreePrimaryProfessionPoints() == 0;

            if (!sCustomMgr->CheckPlayerDataForToken(player, 0, false, professionCheck))
            {
                CloseGossipMenuFor(player);
                return;
            }

            if (player->HasSkill(TokenProfessionSkill[professionId]))
            {
                ChatHandler(player->GetSession()).SendSysMessage("You already have this skill.");
                CloseGossipMenuFor(player);
                return;
            }

            if (!sCustomMgr->CheckPlayerDataForToken(player, PREMIUM_CUSTOM_SPELL, true))
            {
                OnUse(player, item, SpellCastTargets());
                return;
            }

            for (uint8 i = 0; i < PROFESSION_WIDTH; i++)
                TeachProfession(player, professionId, TokenProfessionSpells[professionId][i]);

            player->SetSkill(TokenProfessionSkill[professionId], 0, 450, 450);
            player->CastSpell(player, PREMIUM_CUSTOM_SPELL, false);

            CloseGossipMenuFor(player);

            if (CustomData* customData = sCustomMgr->GetCustomData(player->GetGUIDLow()))
                customData->ProfessionTokenUsePhase = to_underlying(ProfessionTokenPhases::Done);

            player->DestroyItemCount(PREMIUM_TOKEN_BOOST_PROFESSION_ITEM_ID, 1, true, false);
            player->SaveToDB(false, false);

            ChatHandler(player->GetSession()).SendSysMessage("Profession boost done.");
        }

    private:
        void TeachProfession(Player* player, uint8 id, uint32 spellId)
        {
            if (!player)
                return;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
                return;

            if (player->getClass() != CLASS_ROGUE && id == LOCKPICKING)
                return;

            player->learnSpell(spellId);
        }
};

void AddSC_item_custom()
{
    new item_custom_multiple_specs();
    new item_custom_premium_token_boost_level();
    new item_custom_premium_token_boost_profession();
    // new item_mysterious_time_pack();
}
