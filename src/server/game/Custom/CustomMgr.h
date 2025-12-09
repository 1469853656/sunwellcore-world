#ifndef _CUSTOMMGR_H
#define _CUSTOMMGR_H

#include "Common.h"
#include "SharedDefines.h"
#include "Configuration/Config.h"
#include "ObjectMgr.h"
#include "DBCStructure.h"
#include "Battleground.h"
#include "EnumFlag.h"

#include <array>
#include <optional>
#include <span>

#ifndef _CUSTOM_CONFIG
#define _CUSTOM_CONFIG "worldcustom.conf"
#endif

class ChatHandler;
struct SendMailOptions;

enum class Raids
{
    Naxxramas,
    ObsidianSanctum,
    EyeOfEternity,
    Ulduar,
    TrialOfTheCrusader,
    IcecrownCitadel,
    RubySanctum
};

enum class RealmPhase
{
    REALM_PHASE_PTR,
    REALM_PHASE_LAUNCH,
    REALM_PHASE_RELEASED
};

enum CustomIntConfigs
{
    // Racial Shift
    CONFIG_RACIAL_SHIFT_ENABLED_ALLIANCE,
    CONFIG_RACIAL_SHIFT_ENABLED_HORDE,

    // Premium Store
    CONFIG_PREMIUM_STORE_UPDATE_TIME,

    // Dynamic Spawn
    CONFIG_RESPAWN_DYNAMICMINIMUM_CREATURE,
    CONFIG_RESPAWN_DYNAMICMINIMUM_GAMEOBJECT,
    CONFIG_RESPAWN_DYNAMICMINIMUM_CREATURE_CORPSEDELAY,

    // Path of War
    CONFIG_PATH_OF_WAR_HONOR_REQUIRED,
    CONFIG_PATH_OF_WAR_AREA_ID,

    /*********************************************************/
    /***                 NAME RESERVATION                  ***/
    /*********************************************************/
    CONFIG_NAME_RESERVATION_LIMIT,

    // Population Booster
    CONFIG_POPULATION_BOOSTER_MIN_LEVEL,

    /*********************************************************/
    /***                    REALM PHASE                    ***/
    /*********************************************************/
    CONFIG_REALM_PHASE,

    // Cooldown Resets
    CONFIG_RESET_COOLDOWN_ON_ENCOUNTER_KILL_MASK,
    CONFIG_RESET_COOLDOWN_REQUIRED_ENCOUNTER_TIME,

    // Misc
    CONFIG_PTR_BAG_ITEM_ID,
    CONFIG_PTR_BAG_ITEM_COUNT,

    CUSTOM_CONFIG_INT_VALUE_COUNT
};

enum CustomBoolConfigs
{
    // Racial Shift
    CONFIG_RACIAL_SHIFT_FEATURE_ENABLED,

    // Premium Store
    CONFIG_PREMIUM_STORE_ENABLED,
    CONFIG_PREMIUM_STORE_PROCESS_SERVICE_PACK,

    // Autolearn
    CONFIG_AUTO_LEARN_FEATURE_ENABLE,

    // Shared Quest Loot
    CONFIG_ENABLE_SHARED_QUEST_LOOT,

    // Multispec
    CONFIG_MULTISPEC_ENABLED,
    CONFIG_MULTISPEC_FREE,

    // Dynamic Spawn
    CONFIG_DYNAMIC_RESPAWN_ENABLED,
    CONFIG_DYNAMIC_RESPAWN_ENABLE_CORPSEDELAY_CALC,

    /*********************************************************/
    /***                 NAME RESERVATION                  ***/
    /*********************************************************/
    CONFIG_NAME_RESERVATION,
    CONFIG_NAME_RESERVATION_DISABLE_DELETION,

    /*********************************************************/
    /***            CHANNELS FAST CONFIGURATION            ***/
    /*********************************************************/
    CONFIG_ALLOW_CHANNEL_OWNER,
    CONFIG_ALLOW_CHANNEL_PASSWORD,
    CONFIG_ALLOW_CHANNEL_NOTIFICATIONS,
    CONFIG_ALLOW_CHANNEL_SPEAK_DELAY,

    // Mute History
    CONFIG_MUTE_HISTORY_ENABLED,

    // Server info features
    CONFIG_SERVER_INFO_DISPLAY_REVISION,
    CONFIG_SERVER_INFO_DISPLAY_ACTIVE_PLAYERS,

    // Population Booster
    CONFIG_POPULATION_BOOSTER_ENABLED,
    CONFIG_POPULATION_BOOSTER_FAKE_WHO_CLASS,
    CONFIG_POPULATION_BOOSTER_FAKE_WHO_RACE,
    CONFIG_POPULATION_BOOSTER_FAKE_WHO_LEVEL,
    CONFIG_POPULATION_BOOSTER_FAKE_WHO_GUILD,

    // AOE Loot
    CONFIG_AOE_LOOT_ENABLED,

    // Boosts
    CONFIG_SUNWELL_PLATEAU_TIMEWALKING_ENABLED,
    CONFIG_NAXXRAMAS_10_BOOST_ENABLED,
    CONFIG_NAXXRAMAS_25_BOOST_ENABLED,
    CONFIG_OBSIDIAN_SANCTUM_25_BOOST_ENABLED,
    CONFIG_OBSIDIAN_SANCTUM_10_BOOST_ENABLED,
    CONFIG_EYE_OF_ETERNITY_25_BOOST_ENABLED,
    CONFIG_EYE_OF_ETERNITY_10_BOOST_ENABLED,

    // Cooldown Resets
    CONFIG_RESET_COOLDOWN_ON_WIPE,
    CONFIG_RESET_COOLDOWN_ON_ENCOUNTER_KILL,
    CONFIG_RESET_COOLDOWN_ON_ARENA_END,
    CONFIG_RESET_COOLDOWN_ON_DUEL_START,

    // Misc
    CONFIG_TEMPORARY_CHECK,
    CONFIG_TELEPORT_PLAYERS_NEAR_NAXXRAMAS,
    CONFIG_ENCOUNTER_STATS_ENABLED,
    CONFIG_ANNOUNCE_ENCOUNTER_STATS_RECORD,

    CUSTOM_CONFIG_BOOL_VALUE_COUNT
};

enum CustomFloatConfigs
{
    // Loot
    CONFIG_QUEST_ITEM_LOOT_DROP_CHANCE,

    // Dynamic Spawn
    CONFIG_RESPAWN_DYNAMICRATE_CREATURE,
    CONFIG_RESPAWN_DYNAMICRATE_GAMEOBJECT,
    CONFIG_RESPAWN_DYNAMICRATE_CREATURE_CORPSEDELAY,

    // Population Booster
    CONFIG_POPULATION_BOOSTER_VALUE,

    // XP Rates
    CONFIG_BASE_XP_RATE,
    CONFIG_HARDCORE_XP_RATE,
    CONFIG_BOOST_XP_RATE_MULTIPLIER,
    CONFIG_MIN_COMMAND_XP_RATE,
    CONFIG_XP_RATE_ANCESTRAL_WISDOM_MULTIPLIER,
    CONFIG_PROFESSION_RATE,
    CONFIG_REPUTATION_RATE_1_68,

    // AOE Loot
    CONFIG_AOE_LOOT_RANGE,

    CUSTOM_CONFIG_FLOAT_VALUE_COUNT
};

enum class EncounterResetOnKillOption : uint32
{
    None                            = 0x0,
    Dungeon                         = 0x1,
    Raid                            = 0x2,
    All                             = Dungeon | Raid
};

enum class CustomString : uint32
{
    // Premium Store
    PremiumStoreMailTitle               = 1,
    PremiumStoreMailMessage,
    PremiumStoreMarketTitle,
    PremiumStoreMarketMessage,

    // Whitemane+
    WhitemanePlusNotActiveMessage,
    WhitemanePlusActiveMessge,
    WhitemanePlusExpires1WeekMessage,
    WhitemanePlusExpires1DayMessage,
    WhitemanePlusExpiredMessage,
    WhitemanePlusBoughtMessage,
    WhitemanePlusBoughtMailMessage,
    WhitemanePlusExpiredMailMessage,

    // Hardcore Mode
    HardcoreModeReachedMaxLevel,
    HardcoreModeCantBeResurrected,
    HardcoreModeKilled,
    HardcoreModeCantUseThisAHInHardcore,
    HardcoreModeCantUseThisAHInNormal,
    HardcoreModeCantUseBattlegrounds,
    HardcoreModeCantInviteNonHardcore,
    HardcoreModeCantInviteHardcore,
    HardcoreModeCantUseLFG,
    HardcoreModeCantSendMailsToNormal,
    HardcoreModeCantSendMailsToHardcore,
    HardcoreModeCantTradeWithNormal,
    HardcoreModeCantTradeWithHardcore,

    // Misc
    ChatRestrictedMessage
};

enum ClassMask
{
    CLASSMASK_NONE         = 1 << 0,
    CLASSMASK_WARRIOR      = 1 << 1,
    CLASSMASK_PALADIN      = 1 << 2,
    CLASSMASK_HUNTER       = 1 << 3,
    CLASSMASK_ROGUE        = 1 << 4,
    CLASSMASK_PRIEST       = 1 << 5,
    CLASSMASK_DEATH_KNIGHT = 1 << 6,
    CLASSMASK_SHAMAN       = 1 << 7,
    CLASSMASK_MAGE         = 1 << 8,
    CLASSMASK_WARLOCK      = 1 << 9,
    CLASSMASK_UNK          = 1 << 10,
    CLASSMASK_DRUID        = 1 << 11
};

enum RaceMask
{
    RACEMASK_NONE          = 1 << 0,  // 0
    RACEMASK_HUMAN         = 1 << 1,  // 2
    RACEMASK_ORC           = 1 << 2,  // 4
    RACEMASK_DWARF         = 1 << 3,  // 8
    RACEMASK_NIGHTELF      = 1 << 4,  // 16
    RACEMASK_UNDEAD_PLAYER = 1 << 5,  // 32
    RACEMASK_TAUREN        = 1 << 6,  // 64
    RACEMASK_GNOME         = 1 << 7,  // 128
    RACEMASK_TROLL         = 1 << 8,  // 256
    RACEMASK_BLOODELF      = 1 << 10, // 1024
    RACEMASK_DRAENEI       = 1 << 11, // 2048
                                      // 3582
    RACEMASK_ALL = RACEMASK_HUMAN | RACEMASK_ORC | RACEMASK_DWARF | RACEMASK_NIGHTELF | RACEMASK_UNDEAD_PLAYER | RACEMASK_TAUREN | RACEMASK_GNOME | RACEMASK_TROLL | RACEMASK_BLOODELF | RACEMASK_DRAENEI
};

enum WeaponSkills
{
    I_SKILL_ONE_HAND_AXE   = 196,
    I_SKILL_TWO_HAND_AXE   = 197,
    I_SKILL_ONE_HAND_MACE  = 198,
    I_SKILL_TWO_HAND_MACE  = 199,
    I_SKILL_POLEARMS       = 200,
    I_SKILL_ONE_HAND_SWORD = 201,
    I_SKILL_TWO_HAND_SWORD = 202,
    I_SKILL_STAVES         = 227,
    I_SKILL_BOWS           = 264,
    I_SKILL_GUNS           = 266,
    I_SKILL_DAGGERS        = 1180,
    I_SKILL_THROWNS        = 2567,
    I_SKILL_WANDS          = 5009,
    I_SKILL_CROSSBOWS      = 5011,
    I_SKILL_FIST           = 15590,
    I_SKILL_PLATE          = 750,
    I_SKILL_MAIL           = 8737,
    I_SKILL_DUAL_WIELD     = 674
};

// Autolearn
inline const std::vector<uint32> IgnoredSpellsForNewLevel = {
    64380, 23885, 23880, 44461, 25346, 10274, 10273, 8418,
    8419, 7270, 7269, 7268, 54648, 12536, 24530, 70909,
    12494, 57933, 24224, 27095, 27096, 27097, 27099, 32841,
    56131, 56160, 56161, 48153, 34754, 64844, 64904, 48085,
    33110, 48084, 28276, 27874, 27873, 7001, 49821, 53022,
    47757, 47750, 47758, 47666, 53001, 52983, 52998, 52986,
    52987, 52999, 52984, 53002, 53003, 53000, 52988, 52985,
    42208, 42209, 42210, 42211, 42212, 42213, 42198, 42937,
    42938, 12484, 12485, 12486, 44461, 55361, 55362, 34913,
    43043, 43044, 38703, 38700, 27076, 42844, 42845, 64891,
    25912, 25914, 25911, 25913, 25902, 25903, 27175, 27176,
    33073, 33074, 48822, 48820, 48823, 48821, 20154, 25997,
    20467, 20425, 67, 26017, 34471, 53254, 13812, 14314,
    14315, 27026, 49064, 49065, 60202, 60210, 13797, 14298,
    14299, 14300, 14301, 27024, 49053, 49054, 52399, 1742,
    24453, 53548, 53562, 52016, 26064, 35346, 57386, 57389,
    57390, 57391, 57392, 57393, 55509, 35886, 43339, 45297,
    45298, 45299, 45300, 45301, 45302, 49268, 49269, 8349,
    8502, 8503, 11306, 11307, 25535, 25537, 61650, 61654,
    63685, 45284, 45286, 45287, 45288, 45289, 45290, 45291,
    45292, 45293, 45294, 45295, 45296, 49239, 49240, 26364,
    26365, 26366, 26367, 26369, 26370, 26363, 26371, 26372,
    49278, 49279, 32176, 32175, 21169, 47206, 27285, 47833,
    47836, 42223, 42224, 42225, 42226, 42218, 47817, 47818,
    42231, 42232, 42233, 42230, 48466, 44203, 44205, 44206,
    44207, 44208, 48444, 48445, 33891, 52374, 57532, 59921,
    52372, 49142, 52375, 47633, 47632, 52373, 50536, 27214,
    47822, 11682, 11681, 5857, 1010, 24907, 24905, 53227,
    61391, 61390, 61388, 61387, 64801, 5421, 9635, 1178,
    20186, 20185, 20184, 20187, 25899, 24406, 50581, 30708,
    53654, 53653, 53652, 53563,
    // Provided later
    23455, 23458, 23459, 27803, 27804, 27805, 25329, 48075,
    48076, 20647, 2458, 355, 7386, 71, 62902, 62904,
    62903, 62901, 62900, 18960, 8946, 6795, 9634, 49376,
    16979, 48480, 40120, 48566, 48564, 48563, 48565, 33987,
    33983, 33986, 33982, 33878, 33876, 49804, 27007, 9826,
    9824, 9007, 49010, 982, 42243, 42244, 42245, 42234,
    58432, 58433, 53142, 53140, 28271, 61780, 61305, 28272,
    61025, 61721, 61316, 61024, 48950, 48949, 20773, 20772,
    10324, 10322, 7328, 691, 697, 712, 1122, 59671,
    18540, 50589, 18848, 58704, 58703, 58699, 25533, 10438,
    10437, 6365, 6364, 6363, 3599, 58753, 58751, 25509,
    25508, 10408, 10407, 10406, 8155, 8154, 8071, 58757,
    58756, 58755, 25567, 10463, 10462, 6377, 6375, 5394, 48778,
    66907, 66906
};

// Racial Shift
struct CustomRacialData
{
    constexpr CustomRacialData(uint8 race, uint32 classMask, uint32 spellID)
        : Race(race)
        , ClassMask(classMask)
        , SpellID(spellID)
    {
    }

    uint8 Race       = RACE_NONE;
    uint32 ClassMask = CLASSMASK_NONE;
    uint32 SpellID   = 0;
};

// TODO: move to DB?
constexpr std::array RacialsLookupTable{
    CustomRacialData(RACE_HUMAN, CLASSMASK_NONE, 58985), // Perception
    CustomRacialData(RACE_HUMAN, CLASSMASK_NONE, 20597), // Sword Specialization
    CustomRacialData(RACE_HUMAN, CLASSMASK_NONE, 20598), // The Human Spirit
    CustomRacialData(RACE_HUMAN, CLASSMASK_NONE, 20599), // Diplomacy
    CustomRacialData(RACE_HUMAN, CLASSMASK_NONE, 20864), // Mace Specialization
    CustomRacialData(RACE_HUMAN, CLASSMASK_NONE, 59752), // Every Man for Himself

    CustomRacialData(RACE_DWARF, CLASSMASK_NONE, 59224), // Mace Specialization
    CustomRacialData(RACE_DWARF, CLASSMASK_NONE, 20596), // Frost Resistance
    CustomRacialData(RACE_DWARF, CLASSMASK_NONE, 20595), // Gun Specialization
    CustomRacialData(RACE_DWARF, CLASSMASK_NONE, 2481),  // Find Treasure
    CustomRacialData(RACE_DWARF, CLASSMASK_NONE, 20594), // Stoneform

    CustomRacialData(RACE_NIGHTELF, CLASSMASK_NONE, 20585), // Wisp Spirit
    CustomRacialData(RACE_NIGHTELF, CLASSMASK_NONE, 21009), // Elusiveness
    CustomRacialData(RACE_NIGHTELF, CLASSMASK_NONE, 20583), // Nature Resistance
    CustomRacialData(RACE_NIGHTELF, CLASSMASK_NONE, 20582), // Quickness
    CustomRacialData(RACE_NIGHTELF, CLASSMASK_NONE, 58984), // Shadowmeld

    CustomRacialData(RACE_GNOME, CLASSMASK_NONE, 20592), // Arcane Resistance
    CustomRacialData(RACE_GNOME, CLASSMASK_NONE, 20591), // Expansive Mind
    CustomRacialData(RACE_GNOME, CLASSMASK_NONE, 20593), // Engineering Specialization
    CustomRacialData(RACE_GNOME, CLASSMASK_NONE, 20589), // Escape Artist

    CustomRacialData(RACE_DRAENEI, CLASSMASK_WARRIOR + CLASSMASK_ROGUE + CLASSMASK_WARLOCK + CLASSMASK_DRUID, 59221),                                            // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_SHAMAN, 59540),                                                                                                     // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_PRIEST, 59538),                                                                                                     // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_MAGE, 59541),                                                                                                       // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_DEATH_KNIGHT, 59539),                                                                                               // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_HUNTER, 59536),                                                                                                     // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_PALADIN, 59535),                                                                                                    // Shadow Resistance
    CustomRacialData(RACE_DRAENEI, CLASSMASK_NONE, 28875),                                                                                                       // Gemcutting
    CustomRacialData(RACE_DRAENEI, CLASSMASK_PRIEST + CLASSMASK_SHAMAN + CLASSMASK_MAGE + CLASSMASK_WARLOCK, 28878),                                             // Heroic Presence
    CustomRacialData(RACE_DRAENEI, CLASSMASK_WARRIOR + CLASSMASK_PALADIN + CLASSMASK_HUNTER + CLASSMASK_ROGUE + CLASSMASK_DEATH_KNIGHT + CLASSMASK_DRUID, 6562), // Heroic Presence
    CustomRacialData(RACE_DRAENEI, CLASSMASK_WARRIOR, 28880),                                                                                                    // Gift of the Naaru (Attack Power)
    CustomRacialData(RACE_DRAENEI, CLASSMASK_PALADIN, 59542),                                                                                                    // Gift of the Naaru (Attack Power or Spell Power)
    CustomRacialData(RACE_DRAENEI, CLASSMASK_HUNTER, 59543),                                                                                                     // Gift of the Naaru (Attack Power)
    CustomRacialData(RACE_DRAENEI, CLASSMASK_PRIEST, 59544),                                                                                                     // Gift of the Naaru (Spell Power)
    CustomRacialData(RACE_DRAENEI, CLASSMASK_DEATH_KNIGHT + CLASSMASK_ROGUE, 59545),                                                                             // Gift of the Naaru (Attack Power)
    CustomRacialData(RACE_DRAENEI, CLASSMASK_SHAMAN + CLASSMASK_DRUID, 59547),                                                                                   // Gift of the Naaru (Attack Power or Spell Power)
    CustomRacialData(RACE_DRAENEI, CLASSMASK_MAGE + CLASSMASK_WARLOCK, 59548),                                                                                   // Gift of the Naaru (Spell Power)

    CustomRacialData(RACE_ORC, CLASSMASK_NONE, 20573),                                                                                                                   // Hardiness
    CustomRacialData(RACE_ORC, CLASSMASK_NONE, 20574),                                                                                                                   // Axe Specialization
    CustomRacialData(RACE_ORC, CLASSMASK_HUNTER, 20576),                                                                                                                 // Command
    CustomRacialData(RACE_ORC, CLASSMASK_WARRIOR + CLASSMASK_ROGUE + CLASSMASK_SHAMAN + CLASSMASK_PALADIN + CLASSMASK_PRIEST + CLASSMASK_MAGE + CLASSMASK_DRUID, 21563), // Command
    CustomRacialData(RACE_ORC, CLASSMASK_WARLOCK, 20575),                                                                                                                // Command
    CustomRacialData(RACE_ORC, CLASSMASK_DEATH_KNIGHT, 54562),                                                                                                           // Command
    CustomRacialData(RACE_ORC, CLASSMASK_WARRIOR + CLASSMASK_HUNTER + CLASSMASK_ROGUE + CLASSMASK_DEATH_KNIGHT, 20572),                                                  // Blood Fury (Attack Power)
    CustomRacialData(RACE_ORC, CLASSMASK_SHAMAN + CLASSMASK_DRUID + CLASSMASK_PALADIN, 33697),                                                                           // Blood Fury (Spell Power and Attack Power)
    CustomRacialData(RACE_ORC, CLASSMASK_WARLOCK + CLASSMASK_PRIEST + CLASSMASK_MAGE, 33702),                                                                            // Blood Fury (Spell Power)

    CustomRacialData(RACE_UNDEAD_PLAYER, CLASSMASK_NONE, 5227),  // Underwater Breathing
    CustomRacialData(RACE_UNDEAD_PLAYER, CLASSMASK_NONE, 20579), // Shadow Resistance
    CustomRacialData(RACE_UNDEAD_PLAYER, CLASSMASK_NONE, 20577), // Cannibalize
    CustomRacialData(RACE_UNDEAD_PLAYER, CLASSMASK_NONE, 7744),  // Will of the Forsaken

    CustomRacialData(RACE_TAUREN, CLASSMASK_NONE, 20551), // Nature Resistance
    CustomRacialData(RACE_TAUREN, CLASSMASK_NONE, 20550), // Endurance
    CustomRacialData(RACE_TAUREN, CLASSMASK_NONE, 20552), // Cultivation
    CustomRacialData(RACE_TAUREN, CLASSMASK_NONE, 20549), // War Stomp

    CustomRacialData(RACE_TROLL, CLASSMASK_NONE, 20555), // Regeneration
    CustomRacialData(RACE_TROLL, CLASSMASK_NONE, 20557), // Beast Slaying
    CustomRacialData(RACE_TROLL, CLASSMASK_NONE, 20558), // Throwing Specialization
    CustomRacialData(RACE_TROLL, CLASSMASK_NONE, 26290), // Bow Specialization
    CustomRacialData(RACE_TROLL, CLASSMASK_NONE, 58943), // Da Voodoo Shuffle
    CustomRacialData(RACE_TROLL, CLASSMASK_NONE, 26297), // Berserking

    CustomRacialData(RACE_BLOODELF, CLASSMASK_NONE, 28877),                                                                                                                                        // Arcane Affinity
    CustomRacialData(RACE_BLOODELF, CLASSMASK_NONE, 822),                                                                                                                                          // Magic Resistance
    CustomRacialData(RACE_BLOODELF, CLASSMASK_WARRIOR + CLASSMASK_PALADIN + CLASSMASK_HUNTER + CLASSMASK_PRIEST + CLASSMASK_SHAMAN + CLASSMASK_MAGE + CLASSMASK_WARLOCK + CLASSMASK_DRUID, 28730), // Arcane Torrent (Mana)
    CustomRacialData(RACE_BLOODELF, CLASSMASK_ROGUE, 25046),                                                                                                                                       // Arcane Torrent (Energy)
    CustomRacialData(RACE_BLOODELF, CLASSMASK_DEATH_KNIGHT, 50613),                                                                                                                                // Arcane Torrent (Runic Power)
};

// Boost Values
struct InstanceBoostValue
{
    uint32 InstanceID = 0;
    uint32 BossID     = 0;
    uint32 BoostID    = std::numeric_limits<uint32>::max();
    std::pair<uint32, uint32> Values{};
};

// Multispec
enum MultipleSpecsFlags
{
    MULTIPLE_SPECS_FLAG_NONE    = 0x0,
    MULTIPLE_SPECS_FLAG_GROUP_1 = 0x1,
    MULTIPLE_SPECS_FLAG_GROUP_2 = 0x2,
    MULTIPLE_SPECS_FLAG_GROUP_3 = 0x4
};

// Tokens
enum PremiumBoostFlag
{
    PREMIUM_BOOST_FLAG_LEVELUP               = 0x0001,
    PREMIUM_BOOST_FLAG_RIDING                = 0x0002,
    PREMIUM_BOOST_FLAG_ITEM_SET              = 0x0004,
    PREMIUM_BOOST_FLAG_LEVEL_BOOST_DONE      = 0x0008,
    PREMIUM_BOOST_FLAG_PROFESSION_BOOST_DONE = 0x0010
};

// Tokens (NEW)
enum class TokenPhases
{
    NotStarted,
    LevelUp,
    Riding,
    ItemSet,
    Done
};
enum class ProfessionTokenPhases
{
    NotStarted,
    Done
};

namespace CustomConstants 
{
    constexpr uint32 MultispecItemEntry{ 47246 };
    constexpr uint32 HardcoreAuctionHouseEntry{ 420001 };
}

struct PremadeSet
{
    struct ItemInfo
    {
        uint32 EnchantmentID            = 0;
        std::array<uint32, 3> GemIDs    = { 0, 0, 0 };
        uint32 PrismaticGemID           = 0;
    };

    struct EquipmentItemInfo
    {
        uint32 ItemID                   = 0;
        EquipmentSlots EquipmentSlot    = EQUIPMENT_SLOT_START;
        ItemInfo ItemInfo               = {};
    };

    struct BagItemInfo
    {
        uint32 ItemID                   = 0;
        uint32 ItemCount                = 0;
        ItemInfo ItemInfo               = {};
    };

    struct TalentInfo
    {
        uint32 TalentID                 = 0;
        uint32 TalentRank               = 0;
    };

    struct GlyphInfo
    {
        uint32 GlyphID                  = 0;
        uint32 GlyphIndex               = 0;
    };

    void SaveToDB() const;
    void DeleteFromDB() const;

    std::string SetName;
    uint32 SetLevel = 0;
    Classes SetClass = CLASS_NONE;
    uint8 SetSpec = 0;
    uint64 PresetTypeMask = 1;

    std::vector<EquipmentItemInfo> EquipmentItems;
    std::vector<BagItemInfo> BagItems;
    std::vector<TalentInfo> TalentData;
    std::vector<GlyphInfo> GlyphData;
};

enum class CustomPlayerFlags : uint64
{
    None                          = 0x0000000000000000,
    UsedRacialShift               = 0x0000000000000001,
};
DEFINE_ENUM_FLAG(CustomPlayerFlags);

struct CustomData
{
    explicit CustomData(Field const* fields = nullptr);
    void SaveToDB(uint32 characterID) const;

    uint32 CurrentRacial            = 0;
    uint32 MultipleSpecsFlag        = 0;
    uint32 TokenUsePhase            = 0;
    uint32 ProfessionTokenUsePhase  = 0;
    uint32 PremiumBoostFlag         = 0; // old

    void ApplyCharacterFlag(CustomPlayerFlags const flag) { _customCharacterFlags |= flag; }
    void RemoveCharacterFlag(CustomPlayerFlags const flag) { _customCharacterFlags &= ~flag; }
    bool HasCharacterFlag(CustomPlayerFlags const flag) const { return _customCharacterFlags.HasFlag(flag); }

    std::optional<float> XPRate;

    private:
        EnumFlag<CustomPlayerFlags> _customCharacterFlags = CustomPlayerFlags::None;
};

template <typename Container>
concept HasEmplace = requires(Container c)
{
    c.emplace();
};

template <typename Container>
concept HasEmplaceBack = requires(Container c)
{
    c.emplace_back();
};

struct CustomMgr : public Sunwell::Singleton<CustomMgr>
{
    CustomMgr() = default;

    friend class Singleton<CustomMgr>;

    void LoadCustomMgr();

    // Config
    void LoadConfigSettings(bool reload);
    void SetBoolConfig(CustomBoolConfigs const index, bool const value) { _customBoolConfigs.at(index) = value; }
    bool GetBoolConfig(CustomBoolConfigs const index) const { return _customBoolConfigs.at(index); }
    void SetFloatConfig(CustomFloatConfigs const index, float const value) { _customFloatConfigs.at(index) = value; }
    float GetFloatConfig(CustomFloatConfigs const index) const { return _customFloatConfigs.at(index); }
    void SetIntConfig(CustomIntConfigs const index, uint32 const value) { _customIntConfigs.at(index) = value; }
    uint32 GetIntConfig(CustomIntConfigs const index) const { return _customIntConfigs.at(index); }

    // Custom Data
    CustomData* GetCustomData(uint32 characterID);
    CustomData const* GetCustomData(uint32 characterID) const;
    void InsertCharacterCustomData(uint32 characterID, Field const* fields);
    static void LoadCharacterCustomData(WorldSession* session, uint32 characterID);
    void SaveCharacterCustomData(uint32 characterID, bool logout);
    void DeleteCharacterCustomData(uint32 characterID);
    bool HasCustomCharacterFlag(uint32 characterID, CustomPlayerFlags customPlayerFlag) const;
    void SetCustomCharacterFlag(uint32 characterID, CustomPlayerFlags customPlayerFlag, bool apply);

    // Custom String
    void LoadCustomStrings();
    std::string const* GetCustomString(CustomString entry) const;

    // Racial Shift
    void UpdateRacialSpellsForPlayer(Player* player, bool create = false) const;
    bool IsRacialShiftEnabledForRace(TeamId teamID, Races race) const;
    Races GetCurrentRacialsForPlayer(Player const* player) const;
    void ApplyRacialShift(Player* player, Races race);
    void RemoveRacialShift(uint32 guid);

    // Misc
    static bool DoBasicChecksForCommandOrItem(Player const* player);
    std::optional<float> GetQuestItemDropChance() const;
    static bool SendMail(SendMailOptions const& mailOptions, bool isCreatureSender = false, uint32 OrderID = 0);
    static std::string GetClassString(Classes classID);
    static std::string GetSpecString(Classes classID, uint8 specID);
    static void HandleFirstLogin(Player* player);

    // XP Rates
    static constexpr bool IsValidLevel(uint8 const level) { return level == std::clamp(level, static_cast<uint8>(1), static_cast<uint8>(DEFAULT_MAX_LEVEL)); }
    std::optional<float> GetAncestralWisdomXPBonusForPlayer(Player const* player) const;
    std::optional<float> GetSpecifiedXPRateForPlayer(Player const* player) const;
    void SetSpecifiedXPRateForPlayer(Player const* player, float newXPRate);
    bool VerifyAndCorrectXPRate(float& xpRate, uint8 playerLevel) const;
    float GetAndUpdateXPRateForPlayer(Player const* player); // for calculating bonus XP
    std::string GetDetailedXPRateInfoForPlayer(Player const* player) const; // readable format for commands
    uint32 CalculateBonusXPWithRateUpdate(Player const* player, uint32 xp);
    void LoadXPRateBrackets();

    // Boost Values
    void LoadInstanceBoostValues();
    std::optional<std::pair<uint32, uint32>> GetBoostValues(Raids instanceID, uint32 bossID, uint32 boostID) const;
    std::optional<uint32> GetBoostValue1(Raids instanceID, uint32 bossID, uint32 boostID) const;
    std::optional<uint32> GetBoostValue2(Raids instanceID, uint32 bossID, uint32 boostID) const;

    // Premium Services
    constexpr static std::string_view GetPremiumServiceName(PremiumServices const premiumServicesType) { return PremiumServicesNames[to_underlying(premiumServicesType)]; }
    static void LoadPremiumServicesForSession(WorldSession* session, bool reset);
    static void HandleHearthStoneService(Player* player);

    // Autolearn
    void LearnSpellsForNewLevel(Player* player, uint8 level) const;

    // Shared Quest Loot
    static void ApplySharedQuestLoot(LootItem& lootItem, Player const* player);

    /*********************************************************/
    /***                 NAME RESERVATION                  ***/
    /*********************************************************/
    void KickPlayerWhileNameReservationIsEnabled(WorldSession* session) const;

    // Population Booster
    uint32 BoostPopulationByValue(uint32 currentPopulation, bool fakeWho = false) const;

    // Tokens
    bool CheckPlayerDataForToken(Player const* player, uint32 spellId = 0, bool cooldownCheck = false, bool professionCheck = false) const;
    static bool AddItemWihoutNotify(Player* player, uint32 itemId, uint32 count = 1);

    /*********************************************************/
    /***                    REALM PHASE                    ***/
    /*********************************************************/
    bool CheckRealmPhase(RealmPhase const CheckPhase) const;

    // Hardcore Mode
    void HandleHardcoreModePlayerDeath(Player const* player, Unit const* killer) const;

    // Path of War
    std::optional<WorldLocation> GetPathOfWarTeleportLocation() const { return _pathOfWarTeleportLocation; }

    // Premade Sets
    void LoadPremadeSets();
    PremadeSet const* GetPremadeSet(std::string_view setName) const;
    void CreatePremadeSet(std::string const& setName, Player const* player, uint64 presetTypeMask);
    void EquipPremadeSet(std::string const& setName, Player* player) const;
    void DeletePremadeSet(std::string_view setName, Player const* source);
    void DisplayPremadeSetList(ChatHandler* handler) const;
    void DisplayPremadeSetDetails(std::string_view setName, ChatHandler* handler) const;
    void RenamePremadeSet(ChatHandler* handler, std::string_view setName, std::string const& newName);
    void SendGossipsForPremadeSets(Player const* player, uint64 sourceGUID, uint64 presetTypeMask) const;
    void HandlePremadeSetGossipSelect(Player* player, uint32 action, uint64 presetTypeMask) const;
    void AddPresetGroupToSet(ChatHandler* handler, std::string_view setName, uint64 presetTypeMask);
    void RemovePresetGroupFromSet(ChatHandler* handler, std::string_view setName, uint64 presetTypeMask);
    void OverridePresetGroupToSet(ChatHandler* handler, std::string_view setName, uint64 presetTypeMask);
    std::string GetPresetMaskToString(uint64 presetTypeMask) const;
    uint64 GetPresetMaskFromString(std::string_view presetTypeMaskString) const;
    bool IsValidPresetMask(uint64 presetTypeMask) const;
    void DisplayPresetGroupList(ChatHandler* handler) const;
    void AddPresetGroup(ChatHandler* handler, std::string_view groupName, uint32 groupID);
    void RemovePresetGroup(ChatHandler* handler, std::string_view groupName);
    void AddPresetToNPC(ChatHandler* handler, Creature const* creature, uint64 presetTypeMask) const;
    void RemovePresetFromNPC(ChatHandler* handler, Creature const* creature, uint64 presetTypeMask) const;
    void OverrideNPCPreset(ChatHandler* handler, Creature const* creature, uint64 presetTypeMask) const;

    // Ascended Mode
    bool IsAscendedModeAllowedMapID(uint32 mapID) const;

    // Cooldown Resets
    void HandleCooldownResetOnWipe(InstanceScript const* instance, Milliseconds encounterElapsedTime) const;
    void HandleCooldownResetOnEncounterDone(InstanceScript const* instance) const;

    private:
        // Config
        std::array<uint32, CUSTOM_CONFIG_INT_VALUE_COUNT> _customIntConfigs = {};
        std::array<bool, CUSTOM_CONFIG_BOOL_VALUE_COUNT> _customBoolConfigs = {};
        std::array<float, CUSTOM_CONFIG_FLOAT_VALUE_COUNT> _customFloatConfigs = {};
        template <HasEmplaceBack Container>
        void TokenizeConfigStringValueFromFile(Container& container, std::string const& propertyName, std::string const& defaultValue = "", char separator = ';');
        template <HasEmplace Container>
        void TokenizeConfigStringValueFromFile(Container& container, std::string const& propertyName, std::string const& defaultValue = "", char separator = ';');

        // Custom Data
        using CustomDataMap = std::unordered_map<uint32, CustomData>;
        CustomDataMap _playerCustomData;

        // Custom String
        using CustomStringMap = std::unordered_map<uint32, std::string>;
        CustomStringMap _customStrings;

        // Boost Values
        std::vector<InstanceBoostValue> _instanceBoostValues;

        // XP Rates
        using XPRateBrackets = std::vector<std::pair<uint32, float>>;
        XPRateBrackets _xpRateBrackets;

        // Path of War
        std::optional<WorldLocation> _pathOfWarTeleportLocation;

        // Ascended Mode
        std::set<uint32> _ascendedModeAllowedMapIDs;

        // Presets
        using PremadeSetsMap = std::unordered_map<std::string, PremadeSet>;
        PremadeSetsMap _premadeSets;
        using PresetGroupMap = std::unordered_map<uint32, std::string>;
        PresetGroupMap _presetGroups;
};

#define sCustomMgr CustomMgr::instance()

struct SendMailOptions
{
    struct ItemData
    {
        uint32 Count = 0;
        std::optional<int32> RandomEnchantID = std::nullopt;
    };

    SendMailOptions() = default;
    explicit SendMailOptions(uint32 const characterID) : CharacterID(characterID) { }

    SendMailOptions& AddItem(uint32 const itemEntry, uint32 const itemCount = 1, std::optional<int32> randomEnchantID = std::nullopt)
    {
        ItemData const data { itemCount, randomEnchantID };
        MailItems.emplace_back(itemEntry, data);
        return *this;
    }
    SendMailOptions& AddItems(std::span<uint32 const> items)
    {
        for (uint32 const itemID : items)
           AddItem(itemID);
        return *this;
    }
    SendMailOptions& AddMoney(uint32 const moneyAmount) { MoneyAmount = moneyAmount; return *this; }     // in gold
    SendMailOptions& SetIsStoreMail(bool const storeMail) { IsStoreMail = storeMail; return *this; }
    SendMailOptions& SetDraftTitle(std::string_view const draftTitle) { DraftTitle = draftTitle; return *this; }
    SendMailOptions& SetDraftMessage(std::string_view const draftMessage) { DraftMessage = draftMessage; return *this; }
    SendMailOptions& SetDraftTitle(CustomString const draftTitle)
    {
        if (std::string const* draftTitleString = sCustomMgr->GetCustomString(draftTitle))
            return SetDraftTitle(*draftTitleString);

        return SetDraftTitle("");
    }
    SendMailOptions& SetDraftMessage(CustomString const draftMessage)
    {
        if (std::string const* draftMessageString = sCustomMgr->GetCustomString(draftMessage))
            return SetDraftMessage(*draftMessageString);

        return SetDraftMessage("");
    }
    SendMailOptions& SetCreatureID(uint32 const creatureID) { CreatureID = creatureID; return *this;  }

    std::string ToString() const;

    uint32 CharacterID = 0;
    uint32 CreatureID  = 0;
    uint32 MoneyAmount = 0; // in gold
    std::vector<std::pair<uint32 /* ItemEntry */, ItemData>> MailItems;
    bool IsStoreMail = false;
    std::string DraftTitle;
    std::string DraftMessage;
};

#endif
