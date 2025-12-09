#include "TransmogificationMgr.h"
#include "Bag.h"
#include "Chat.h"
#include "Common.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "Define.h"
#include "Field.h"
#include "Item.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QueryResult.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Transaction.h"
#include "World.h"
#include "WorldSession.h"
#include "DatabaseLog.h"

#include <sstream>
#include <string>

#include "CustomMgr.h"

/*
SELECT ItemVisual, GROUP_CONCAT(DBCNAME), GROUP_CONCAT(Name_Lang_enUS), GROUP_CONCAT(effect) FROM
(
SELECT a.ID, b.Effect_1 AS effect, a.ItemVisual, a.Name_Lang_enUS AS DBCNAME, b.Name_Lang_enUS FROM db_spellitemenchantment_12340 a LEFT JOIN db_spell_12340 b ON a.ID = b.EffectMiscValue_1 WHERE a.ItemVisual > 0
UNION
SELECT a.ID, b.Effect_2 AS effect, a.ItemVisual, a.Name_Lang_enUS AS DBCNAME, b.Name_Lang_enUS FROM db_spellitemenchantment_12340 a LEFT JOIN db_spell_12340 b ON a.ID = b.EffectMiscValue_2 WHERE a.ItemVisual > 0
UNION
SELECT a.ID, b.Effect_3 AS effect, a.ItemVisual, a.Name_Lang_enUS AS DBCNAME, b.Name_Lang_enUS FROM db_spellitemenchantment_12340 a LEFT JOIN db_spell_12340 b ON a.ID = b.EffectMiscValue_3 WHERE a.ItemVisual > 0
ORDER BY ID ASC
) x
WHERE effect IN (53, 54, 92) OR effect IS NULL GROUP BY ItemVisual ORDER BY ItemVisual ASC;

const Trinity::DenseMap<uint32, std::string> TransmogificationMgr::EnchantVisualToName =
{
    {1  , "Frozen Rune Weapon, Razorice, Shadow Oil"},
    {2  , "Impact, Striking"},
    {24 , "Superior Impact"},
    {25 , "Fiery, Demonslaying, Icebreaker, Giant Slayer, Lichbane"},
    {26 , "Poison"},
    {27 , "Frost Oil"},
    {28 , "Sharpened"},
    {29 , "Spirit, Intellect"},
    {31 , "Beastslayer, Elemental Slayer, Scourgebane, Righteous Weapon Coating"},
    {32 , "Flametongue"},
    {33 , "Frostbrand"},
    {42 , "Striking, Impact, Blessed Weapon Coating"},
    {61 , "Rockbiter"},
    {81 , "Windfury"},
    {101, "Savagery"},
    {102, "Healing"},
    {103, "Crusader, Lifeward"},
    {104, "Holy Glow"}, // Made up name. Not from spells. +150 Attack Power vs Undead and Demons
    {105, "Arcane Glow"}, // Made up name. MHTest02
    {106, "Potency, Earthliving"},
    {107, "Feedback, Spellpower"},
    {125, "Agility"},
    {126, "Icy Chill"},
    {128, "Arcane Surge"}, // Made up name. Not from spells. +69 Spell Power
    {139, "Dancing Rune Weapon"}, // Black Temple Dummy
    {151, "Radiance"}, // Made up name. +43 Spell Power,+43 Spell Power,QAEnchant Weapon +81 Healing
    {155, "Mongoose"},
    {156, "Savagery, Rune of Swordshattering, Rune of Swordbreaking"},
    {157, "Soulfrost"},
    {158, "Sunfire"},
    {159, "Battlemaster, Titanium Weapon Chain"},
    {160, "Spellsurge, Rune of the Fallen Crusader"},
    {161, "Unholy Weapon, Black Magic, Rune of Spellshattering, Rune of Spellbreaking"},
    {164, "Blood Draining"},
    {165, "Executioner"},
    {166, "Deathfrost, Rune of Cinderglacier"},
    {172, "Lichflame"}, // Made up name. Not from spells. +81 Spell Power
    {173, "Empower Rune Weapon"}, // Not from spells. Empower Rune Weapon
    {178, "Berserking"}, // Not from spells. Berserking,+110 Attack Power
    {186, "Blade Ward"},
};
*/
std::string TransmogificationMgr::GetSlotName(uint8 slot)
{
    switch (slot)
    {
        case EQUIPMENT_SLOT_HEAD:       return "Head";
        case EQUIPMENT_SLOT_SHOULDERS:  return "Shoulders";
        case EQUIPMENT_SLOT_BODY:       return "Shirt";
        case EQUIPMENT_SLOT_CHEST:      return "Chest";
        case EQUIPMENT_SLOT_WAIST:      return "Waist";
        case EQUIPMENT_SLOT_LEGS:       return "Legs";
        case EQUIPMENT_SLOT_FEET:       return "Feet";
        case EQUIPMENT_SLOT_WRISTS:     return "Wrists";
        case EQUIPMENT_SLOT_HANDS:      return "Hands";
        case EQUIPMENT_SLOT_BACK:       return "Back";
        case EQUIPMENT_SLOT_MAINHAND:   return "Main hand";
        case EQUIPMENT_SLOT_OFFHAND:    return "Off hand";
        case EQUIPMENT_SLOT_RANGED:     return "Ranged";
        case EQUIPMENT_SLOT_TABARD:     return "Tabard";
        default:                        return "";
    }
}

std::string TransmogificationMgr::GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y)
{
    std::ostringstream ss;
    ss << "|TInterface";

    ItemDisplayInfoEntry const* dispInfo = nullptr;
    if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(entry))
    {
        dispInfo = sItemDisplayInfoStore.LookupEntry(itemTemplate->DisplayInfoID);
        if (dispInfo)
            ss << "/ICONS/" << dispInfo->inventoryIcon; // what is in InventoryIcon[1]?
    }
    if (!dispInfo)
        ss << "/InventoryItems/WoWUnknownItem01";
    ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
    return ss.str();
}

std::string TransmogificationMgr::GetSlotIcon(uint8 slot, uint32 width, uint32 height, int x, int y)
{
    std::ostringstream ss;
    ss << "|TInterface/PaperDoll/";
    switch (slot)
    {
        case EQUIPMENT_SLOT_HEAD: ss << "UI-PaperDoll-Slot-Head"; break;
        case EQUIPMENT_SLOT_SHOULDERS: ss << "UI-PaperDoll-Slot-Shoulder"; break;
        case EQUIPMENT_SLOT_BODY: ss << "UI-PaperDoll-Slot-Shirt"; break;
        case EQUIPMENT_SLOT_CHEST: ss << "UI-PaperDoll-Slot-Chest"; break;
        case EQUIPMENT_SLOT_WAIST: ss << "UI-PaperDoll-Slot-Waist"; break;
        case EQUIPMENT_SLOT_LEGS: ss << "UI-PaperDoll-Slot-Legs"; break;
        case EQUIPMENT_SLOT_FEET: ss << "UI-PaperDoll-Slot-Feet"; break;
        case EQUIPMENT_SLOT_WRISTS: ss << "UI-PaperDoll-Slot-Wrists"; break;
        case EQUIPMENT_SLOT_HANDS: ss << "UI-PaperDoll-Slot-Hands"; break;
        case EQUIPMENT_SLOT_BACK: ss << "UI-PaperDoll-Slot-Chest"; break;
        case EQUIPMENT_SLOT_MAINHAND: ss << "UI-PaperDoll-Slot-MainHand"; break;
        case EQUIPMENT_SLOT_OFFHAND: ss << "UI-PaperDoll-Slot-SecondaryHand"; break;
        case EQUIPMENT_SLOT_RANGED: ss << "UI-PaperDoll-Slot-Ranged"; break;
        case EQUIPMENT_SLOT_TABARD: ss << "UI-PaperDoll-Slot-Tabard"; break;
        default: ss << "UI-Backpack-EmptySlot";
    }
    ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
    return ss.str();
}

std::string TransmogificationMgr::GetItemName(ItemTemplate const* itemTemplate, WorldSession const* session)
{
    if (!itemTemplate)
        return "";

    LocaleConstant const locale = session->GetSessionDbLocaleIndex();
    std::string name = itemTemplate->Name1;
    if (locale != DEFAULT_LOCALE)
        if (ItemLocale const* il = sObjectMgr->GetItemLocale(itemTemplate->ItemId))
            ObjectMgr::GetLocaleString(il->Name, locale, name);

    return name;
}

std::string TransmogificationMgr::GetItemName(Item const* item, WorldSession const* session)
{
    if (!item)
        return "";

    LocaleConstant const locale = session->GetSessionDbLocaleIndex();
    ItemTemplate const* itemTemplate = item->GetTemplate();
    std::string name = itemTemplate->Name1;
    if (locale != DEFAULT_LOCALE)
        if (ItemLocale const* il = sObjectMgr->GetItemLocale(itemTemplate->ItemId))
            ObjectMgr::GetLocaleString(il->Name, locale, name);

    if (int32 const itemRandomPropertyID = item->GetItemRandomPropertyId())
    {
        char const* suffix = nullptr;
        if (itemRandomPropertyID < 0)
        {
            if (ItemRandomSuffixEntry const* itemRandEntry = sItemRandomSuffixStore.LookupEntry(-itemRandomPropertyID))
                suffix = itemRandEntry->nameSuffix[locale];
        }
        else
        {
            if (ItemRandomPropertiesEntry const* itemRandEntry = sItemRandomPropertiesStore.LookupEntry(itemRandomPropertyID))
                suffix = itemRandEntry->nameSuffix[locale];
        }

        if (suffix)
        {
            name += ' ';
            name += suffix;
        }
    }

    return name;
}

std::string TransmogificationMgr::GetItemLink(Item const* item, WorldSession const* session) const
{
    ItemTemplate const* itemTemplate = item->GetTemplate();
    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[itemTemplate->Quality] << std::dec <<
        "|Hitem:" << itemTemplate->ItemId << ":" <<
        item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT) << ":" <<
        item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT) << ":" <<
        item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2) << ":" <<
        item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3) << ":" <<
        item->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT) << ":" <<
        item->GetItemRandomPropertyId() << ":" << item->GetItemSuffixFactor() << ":" <<
        static_cast<uint32>(item->GetOwner()->getLevel()) << "|h[" << GetItemName(item, session) << "]|h|r";

    return oss.str();
}

std::string TransmogificationMgr::GetItemLink(uint32 entry, WorldSession const* session) const
{
    if (entry == TransmogConstants::InvisibleEntry)
        return "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|t";

    ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(entry);
    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[itemTemplate->Quality] << std::dec <<
        "|Hitem:" << entry << ":0:0:0:0:0:0:0:0:0|h[" << GetItemName(itemTemplate, session) << "]|h|r";

    return oss.str();
}

void TransmogificationMgr::UpdateItem(Player* player, ItemRef const& item)
{
    if (!item->IsEquipped())
        return;

    player->SetVisibleItemSlot(item->GetSlot(), item);
    if (player->IsInWorld())
        item->SendUpdateToPlayer(player);
}

void TransmogificationMgr::RevertPendingTransmogifications(Player* player)
{
    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (ItemRef const& item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            UpdateItem(player, item);
}

TransmogResult TransmogificationMgr::CannotTransmogrifyItemWithItem(Player* player, ItemTemplate const* target, ItemTemplate const* source)
{
    using enum TransmogResult;
    if (source->Class != target->Class)
        return ItemTypesDontMatch;

    if (TransmogResult const transmogResult = CannotTransmogrifyItem(player, source); transmogResult != OK)
        return transmogResult;

    if (TransmogResult const transmogResult = CannotTransmogrifyItem(player, target); transmogResult != OK)
        return transmogResult;

    if (IsBowOrGunOrCrossbow(source) != IsBowOrGunOrCrossbow(target) || IsMeleeWeapon(source) != IsMeleeWeapon(target))
        return ItemTypesDontMatch;

    if (source->SubClass != target->SubClass && !IsBowOrGunOrCrossbow(target))
    {
        if (source->Class == ITEM_CLASS_ARMOR && !AllowMixedArmorTypes)
            return ArmorTypesDontMatch;

        if (source->Class == ITEM_CLASS_WEAPON && !AllowMixedWeaponTypes)
            return WeaponTypesDontMatch;

        if (source->Class == ITEM_CLASS_WEAPON && !IsMeleeWeapon(target)) // Wands and thrown should not be transmogrifiable with other items like swords etc.
            return WeaponTypesDontMatch;

    }

    if (source->InventoryType != target->InventoryType)
    {
        // TODO: Refactor this
        if (source->Class == ITEM_CLASS_WEAPON && !((IsBowOrGunOrCrossbow(target) || AllowMixedInventoryTypes
            || ((target->InventoryType == INVTYPE_WEAPON || target->InventoryType == INVTYPE_2HWEAPON) &&
            (source->InventoryType == INVTYPE_WEAPON || source->InventoryType == INVTYPE_2HWEAPON))
            || ((target->InventoryType == INVTYPE_WEAPONMAINHAND || target->InventoryType == INVTYPE_WEAPONOFFHAND) &&
            (source->InventoryType == INVTYPE_WEAPON || source->InventoryType == INVTYPE_2HWEAPON)))))
            return EquipSlotsDontMatch;

        if (source->Class == ITEM_CLASS_ARMOR &&
            !((source->InventoryType == INVTYPE_CHEST || source->InventoryType == INVTYPE_ROBE) &&
            (target->InventoryType == INVTYPE_CHEST || target->InventoryType == INVTYPE_ROBE)))
            return EquipSlotsDontMatch;
    }

    return OK;
}

// TODO: remove this?
// Item::IsFitToSpellRequirements copy paste
static bool IsFitToSpellRequirements(SpellInfo const* /*spellInfo*/, ItemTemplate const* /*proto*/)
{
    //bool const isEnchantSpell = spellInfo->HasEffect(SPELL_EFFECT_ENCHANT_ITEM) || spellInfo->HasEffect(SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) || spellInfo->HasEffect(SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC);
    //if (spellInfo->EquippedItemClass != -1)                 // -1 == any item class
    //{
    //    // Special case - accept vellum for armor/weapon requirements
    //    if (isEnchantSpell && ((spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR && proto->IsArmorVellum())
    //        || (spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON && proto->IsWeaponVellum())))
    //        return true;

    //    if (spellInfo->EquippedItemClass != int32(proto->Class))
    //        return false;                                   //  wrong item class

    //    if (spellInfo->EquippedItemSubClassMask != 0)        // 0 == any subclass
    //    {
    //        if ((spellInfo->EquippedItemSubClassMask & (1 << proto->SubClass)) == 0)
    //            return false;                               // subclass not present in mask
    //    }
    //}

    //if (isEnchantSpell && spellInfo->EquippedItemInventoryTypeMask != 0)       // 0 == any inventory type
    //{
    //    // Special case - accept weapon type for main and offhand requirements
    //    if (proto->InventoryType == INVTYPE_WEAPON &&
    //        (spellInfo->EquippedItemInventoryTypeMask & (1 << INVTYPE_WEAPONMAINHAND) ||
    //            spellInfo->EquippedItemInventoryTypeMask & (1 << INVTYPE_WEAPONOFFHAND)))
    //        return true;
    //    else if ((spellInfo->EquippedItemInventoryTypeMask & (1 << proto->InventoryType)) == 0)
    //        return false;                                   // inventory type not present in mask
    //}

    return true;
}

TransmogResult TransmogificationMgr::CannotTransmogrifyItemWithEnchant(Player* /*player*/, ItemTemplate const* destination, uint32 enchant)
{
    using enum TransmogResult;

    auto const it = enchant_to_spells.find(enchant);
    if (it == enchant_to_spells.end())
        return NonexistantTransmog;

    for (SpellInfo const* spellInfo : it->second)
        if (!IsFitToSpellRequirements(spellInfo, destination))
            return ItemNotFitForEnchantRequirements;

    //if (!HasVisual(player, TransmogType::Enchant, enchant))
        //return NoTransmogrifications;

    // Checks just gem requirements, not needed
    //if (!Ignore && source->EnchantmentCondition && !player->EnchantmentFitsRequirements(source->EnchantmentCondition, -1))
    //    return ??;

    //if (!IgnoreReqLevel && source->MinLevel > player->GetLevel())
    //    return TransmogResult_TooLowLevelPlayer;
    //if (!IgnoreReqLevel && source->RequiredSkillID > 0 && source->RequiredSkillRank > player->GetSkillValue(source->RequiredSkillID))
    //    return TransmogResult_TooLowSkill;
    //if (destination->ItemLevel < spellInfo->BaseLevel || (destination->RequiredLevel && destination->RequiredLevel < spellInfo->BaseLevel))
    //    return TransmogResult_TooLowLevelItem;
    //if (spellInfo->MaxLevel && destination->ItemLevel > spellInfo->MaxLevel)
    //    return TransmogResult_TooHighLevelItem;
    return OK;
}

TransmogResult TransmogificationMgr::CannotTransmogrifyItem(Player* player, ItemTemplate const* proto)
{
    using enum TransmogResult;

    if (TransmogResult const transmogResult = CannotTransmogrify(proto); transmogResult != OK)
        return transmogResult;

    if (TransmogResult const transmogResult = CannotEquip(player, proto); transmogResult != OK)
        return transmogResult;

    return OK;
}

uint32 TransmogificationMgr::GetSpecialPrice(ItemTemplate const* proto)
{
    return std::max(proto->SellPrice, 10000U);
}

bool TransmogificationMgr::IsBowOrGunOrCrossbow(ItemTemplate const* itemTemplate)
{
    uint32 const itemClass = itemTemplate->Class;
    uint32 const itemSubclass = itemTemplate->SubClass;
    return itemClass == ITEM_CLASS_WEAPON && (itemSubclass == ITEM_SUBCLASS_WEAPON_BOW || itemSubclass == ITEM_SUBCLASS_WEAPON_GUN || itemSubclass == ITEM_SUBCLASS_WEAPON_CROSSBOW);
}

bool TransmogificationMgr::IsMeleeWeapon(ItemTemplate const* itemTemplate)
{
    if (uint32 const itemClass = itemTemplate->Class; itemClass != ITEM_CLASS_WEAPON)
        return false;

    switch (uint32 const itemSubclass = itemTemplate->SubClass; itemSubclass)
    {
        case ITEM_SUBCLASS_WEAPON_AXE:
        case ITEM_SUBCLASS_WEAPON_AXE2:
        case ITEM_SUBCLASS_WEAPON_MACE:
        case ITEM_SUBCLASS_WEAPON_MACE2:
        case ITEM_SUBCLASS_WEAPON_POLEARM:
        case ITEM_SUBCLASS_WEAPON_SWORD:
        case ITEM_SUBCLASS_WEAPON_SWORD2:
        case ITEM_SUBCLASS_WEAPON_STAFF:
        case ITEM_SUBCLASS_WEAPON_FIST:
        case ITEM_SUBCLASS_WEAPON_DAGGER:
        case ITEM_SUBCLASS_WEAPON_SPEAR:
        case ITEM_SUBCLASS_WEAPON_FISHING_POLE:
            return true;
        default:
            return false;
    }
}

bool TransmogificationMgr::IsAllowed(uint32 entry) const
{
    return Allowed.contains(entry);
}

bool TransmogificationMgr::IsNotAllowed(uint32 entry) const
{
    return NotAllowed.contains(entry);
}

bool TransmogificationMgr::IsAllowedQuality(uint32 quality) const
{
    switch (quality)
    {
        case ITEM_QUALITY_POOR: return AllowPoor;
        case ITEM_QUALITY_NORMAL: return AllowCommon;
        case ITEM_QUALITY_UNCOMMON: return AllowUncommon;
        case ITEM_QUALITY_RARE: return AllowRare;
        case ITEM_QUALITY_EPIC: return AllowEpic;
        case ITEM_QUALITY_LEGENDARY: return AllowLegendary;
        case ITEM_QUALITY_ARTIFACT: return AllowArtifact;
        case ITEM_QUALITY_HEIRLOOM: return AllowHeirloom;
        default: return false;
    }
}

void TransmogificationMgr::LoadConfig()
{
    EnableTransmogrification = sConfigMgr->GetBoolDefault("Transmogrification.Enable", true);
    EnableEnchantTransmogrification = sConfigMgr->GetBoolDefault("Transmogrification.EnableEnchant", true);

    EnableSetInfo = sConfigMgr->GetBoolDefault("Transmogrification.EnableSetInfo", true);
    SetNpcText = static_cast<uint32>(sConfigMgr->GetIntDefault("Transmogrification.SetNpcText", 940001));

    EnableSets = sConfigMgr->GetBoolDefault("Transmogrification.EnableSets", true);
    MaxSets = static_cast<uint8>(sConfigMgr->GetIntDefault("Transmogrification.MaxSets", 25));

    if (MaxSets > TransmogConstants::AbsoluteMaxSets)
        MaxSets = TransmogConstants::AbsoluteMaxSets;

    EnableTransmogInfo = sConfigMgr->GetBoolDefault("Transmogrification.EnableTransmogInfo", true);
    TransmogNpcText = static_cast<uint32>(sConfigMgr->GetIntDefault("Transmogrification.TransmogNpcText", 940000));

    std::istringstream issAllowed(sConfigMgr->GetStringDefault("Transmogrification.Allowed", ""));
    std::istringstream issNotAllowed(sConfigMgr->GetStringDefault("Transmogrification.NotAllowed", ""));
    while (issAllowed.good())
    {
        uint32 entry;
        issAllowed >> entry;
        if (issAllowed.fail())
            break;
        Allowed.insert(entry);
    }
    while (issNotAllowed.good())
    {
        uint32 entry;
        issNotAllowed >> entry;
        if (issNotAllowed.fail())
            break;
        NotAllowed.insert(entry);
    }

    ScaledCostModifier = sConfigMgr->GetFloatDefault("Transmogrification.ScaledCostModifier", 1.0f);
    CopperCost         = sConfigMgr->GetIntDefault(  "Transmogrification.CopperCost"        , 0);

    AllowPoor      = sConfigMgr->GetBoolDefault("Transmogrification.AllowPoor",      true);
    AllowCommon    = sConfigMgr->GetBoolDefault("Transmogrification.AllowCommon",    true);
    AllowUncommon  = sConfigMgr->GetBoolDefault("Transmogrification.AllowUncommon",  true);
    AllowRare      = sConfigMgr->GetBoolDefault("Transmogrification.AllowRare",      true);
    AllowEpic      = sConfigMgr->GetBoolDefault("Transmogrification.AllowEpic",      true);
    AllowLegendary = sConfigMgr->GetBoolDefault("Transmogrification.AllowLegendary", true);
    AllowArtifact  = sConfigMgr->GetBoolDefault("Transmogrification.AllowArtifact",  true);
    AllowHeirloom  = sConfigMgr->GetBoolDefault("Transmogrification.AllowHeirloom",  true);

    AllowMixedArmorTypes     = sConfigMgr->GetBoolDefault("Transmogrification.AllowMixedArmorTypes",     true);
    AllowMixedWeaponTypes    = sConfigMgr->GetBoolDefault("Transmogrification.AllowMixedWeaponTypes",    true);
    AllowMixedInventoryTypes = sConfigMgr->GetBoolDefault("Transmogrification.AllowMixedInventoryTypes", true);
    AllowFishingPoles        = sConfigMgr->GetBoolDefault("Transmogrification.AllowFishingPoles",        true);

    IgnoreReqRace  = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqRace",  true);
    IgnoreReqClass = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqClass", true);
    IgnoreReqSkill = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqSkill", true);
    IgnoreReqSpell = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqSpell", true);
    IgnoreReqLevel = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqLevel", true);
    IgnoreReqEvent = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqEvent", true);
    IgnoreReqStats = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqStats", true);
    IgnorePlayerMissingProfiency = sConfigMgr->GetBoolDefault("Transmogrification.IgnorePlayerMissingProfiency", true);
    IgnoreReqFaction = sWorld->IsCrossfactionEnabled(Crossfaction::Item);
    IgnoreReqBound   = sConfigMgr->GetIntDefault("Transmogrification.IgnoreReqBound", 2);
}

void TransmogificationMgr::LoadEnchants(bool reload /* = false*/)
{
    uint32 const oldMSTime = getMSTime();
    EnchantVisualToName.clear();

    auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_TRANSMOG_ENCHANT_NAMES);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            uint32 const enchantmentID = fields[0].GetUInt32();
            std::string const enchantmentName = fields[1].GetString();
            if (!sSpellItemEnchantmentStore.LookupEntry(enchantmentID))
            {
                sLog->outError("%s", fmt::format("Invalid enchantment ID {} in `transmog_enchant_names` table, skipping...", enchantmentID).c_str());
                continue;
            }

            EnchantVisualToName[enchantmentID] = enchantmentName;
        } while (result->NextRow());
    }

    sLog->outString("%s", fmt::format(">> Loaded {} transmog enchantment names in {} ms", EnchantVisualToName.size(), GetMSTimeDiffToNow(oldMSTime)).c_str());

    if (!reload)
    {
        enchant_to_spells.clear();
        for (uint32 entryId = 0; entryId < sSpellItemEnchantmentStore.GetNumRows(); ++entryId)
        {
            SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(entryId);
            if (enchant && enchant->aura_id && enchant->aura_id < static_cast<uint32>(-1)) // Visual can be -1 also
                enchant_to_spells[enchant->ID]; // initialize
        }

        for (uint32 spellid = 0; spellid < sSpellMgr->GetSpellInfoStoreSize(); ++spellid)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
            if (!spellInfo)
                continue;

            for (auto& effect : spellInfo->Effects)
            {
                if (effect.Effect == SPELL_EFFECT_ENCHANT_ITEM || effect.Effect == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY || effect.Effect == SPELL_EFFECT_ENCHANT_HELD_ITEM)
                {
                    if (enchant_to_spells.contains(effect.MiscValue))
                    {
                        enchant_to_spells[effect.MiscValue].push_back(spellInfo);
                    }
                }
            }
        }
    }
}

void TransmogificationMgr::LoadTransmogSets()
{
    uint32 oldMSTime = getMSTime();
    TransmogSets.clear();

    auto stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_TRANSMOG_SETS);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            uint32 const transmogSetID = fields[0].GetUInt32();
            std::string transmogSetName = fields[1].GetString();

            TransmogSetData transmogSetData;
            transmogSetData.SetName = std::move(transmogSetName);

            TransmogSets[transmogSetID] = std::move(transmogSetData);
        } while (result->NextRow());
    }

    sLog->outString("%s", fmt::format(">> Loaded {} transmog sets in {} ms", TransmogSets.size(), GetMSTimeDiffToNow(oldMSTime)).c_str());

    uint32 loadedItemCount = 0;
    oldMSTime = getMSTime();

    stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_TRANSMOG_SET_ITEMS);
    if (PreparedQueryResult const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            uint32 const transmogSetID = fields[0].GetUInt32();
            auto const it = TransmogSets.find(transmogSetID);
            if (it == std::end(TransmogSets))
            {
                sLog->outError("%s", fmt::format("Invalid transmog set ID {} in `transmog_set_items` table, skipping...", transmogSetID).c_str());
                continue;
            }

            uint32 const itemID = fields[1].GetUInt32();
            uint32 const slotID = fields[2].GetUInt32();
            std::optional<uint32> itemSlot;
            if (slotID == 1000)
            {
                ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemID);
                if (!itemTemplate)
                {
                    sLog->outError("%s", fmt::format("Invalid item ID {} in `transmog_set_items` table, skipping...", itemID).c_str());
                    continue;
                }

                switch (itemTemplate->InventoryType)
                {
                    case INVTYPE_HEAD:
                        itemSlot = EQUIPMENT_SLOT_HEAD;
                        break;
                    case INVTYPE_SHOULDERS:
                        itemSlot = EQUIPMENT_SLOT_SHOULDERS;
                        break;
                    case INVTYPE_BODY:
                        itemSlot = EQUIPMENT_SLOT_BODY;
                        break;
                    case INVTYPE_CHEST:
                        itemSlot = EQUIPMENT_SLOT_CHEST;
                        break;
                    case INVTYPE_WAIST:
                        itemSlot = EQUIPMENT_SLOT_WAIST;
                        break;
                    case INVTYPE_LEGS:
                        itemSlot = EQUIPMENT_SLOT_LEGS;
                        break;
                    case INVTYPE_FEET:
                        itemSlot = EQUIPMENT_SLOT_FEET;
                        break;
                    case INVTYPE_WRISTS:
                        itemSlot = EQUIPMENT_SLOT_WRISTS;
                        break;
                    case INVTYPE_HANDS:
                        itemSlot = EQUIPMENT_SLOT_HANDS;
                        break;
                    case INVTYPE_SHIELD:
                        itemSlot = EQUIPMENT_SLOT_OFFHAND;
                        break;
                    case INVTYPE_RANGED:
                        itemSlot = EQUIPMENT_SLOT_RANGED;
                        break;
                    case INVTYPE_CLOAK:
                        itemSlot = EQUIPMENT_SLOT_BACK;
                        break;
                    case INVTYPE_WEAPON:
                    case INVTYPE_2HWEAPON:
                    case INVTYPE_WEAPONMAINHAND:
                        itemSlot = EQUIPMENT_SLOT_MAINHAND;
                        break;
                    case INVTYPE_WEAPONOFFHAND:
                        itemSlot = EQUIPMENT_SLOT_OFFHAND;
                        break;
                    case INVTYPE_ROBE:
                        itemSlot = EQUIPMENT_SLOT_CHEST;
                        break;
                    case INVTYPE_THROWN:
                    case INVTYPE_RANGEDRIGHT:
                        itemSlot = EQUIPMENT_SLOT_RANGED;
                        break;
                    default:
                        break;
                }

                if (!itemSlot.has_value())
                {
                    sLog->outError("%s", fmt::format("Invalid item slot for item ID {} in `transmog_set_items` table, skipping...", itemID).c_str());
                    continue;
                }
            }
            else
            {
                itemSlot = fields[2].GetUInt32();
                if (itemSlot.value() >= EQUIPMENT_SLOT_END)
                {
                    sLog->outError("%s", fmt::format("Invalid item slot {} for item ID {} in `transmog_set_items` table, skipping...", itemSlot.value(), itemID).c_str());
                    continue;
                }
            }


            it->second.ItemData.push_back(std::make_pair(itemID, itemSlot.value()));
            ++loadedItemCount;
        } while (result->NextRow());
    }

    sLog->outString("%s", fmt::format(">> Loaded {} transmog set items in {} ms", loadedItemCount, GetMSTimeDiffToNow(oldMSTime)).c_str());
}

void TransmogificationMgr::AddTransmogSetForPlayer(uint32 accountID, uint32 transmogSetID)
{
    auto const it = TransmogSets.find(transmogSetID);
    if (it == std::end(TransmogSets))
        return;

    bool const isSmallSet = it->second.ItemData.size() <= 2;
    if (Player* player = sWorld->GetPlayerByAccountID(accountID))
    {
        SetTransmogs items;

        for (auto[itemID, itemSlot] : it->second.ItemData)
        {
            AddToCollection(player, sObjectMgr->GetItemTemplate(itemID), true);

            if (!isSmallSet)
            {
                SetTransmog setTransmog{};
                setTransmog.transmog = itemID;
                setTransmog.slot = static_cast<uint8>(itemSlot);
                setTransmog.type = TransmogType::Item;
                setTransmog.IsPurchased = true;
                items.emplace_back(setTransmog);
            }
        }

        if (!isSmallSet)
        {
            if (items.empty())
                return;

            PresetData preset;
            preset.Name = it->second.SetName;
            preset.Transmogs = items;
            player->PurchasedTransmogPresets.emplace_back(std::move(preset));

            ChatHandler(player->GetSession()).SendFormattedSysMessage("Transmogification set {} has been added to your collection.", it->second.SetName);
        }
    }
    else
    {
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_HIGHEST_PURCHASED_TRANSMOG_SET);
        stmt->setUInt32(0, accountID);

        auto const selMaxTransmogResult = CharacterDatabase.Query(stmt);
        auto const& transmogSetData = it->second;
        uint8 presetID = 0;
        if (selMaxTransmogResult)
        {
            Field const* fields = selMaxTransmogResult->Fetch();
            uint32 const maxTransmogResult = fields[0].GetUInt32();
            presetID = std::min<uint8>(static_cast<uint8>(maxTransmogResult), 254) + 1;
        }

        if (presetID > MaxSets)
        {
            DBLog::OutStore("Player with account {} has too many sets ({}, maximum {}), skipping order", accountID, presetID, MaxSets);
            return;
        }

        auto trans = CharacterDatabase.BeginTransaction();
        std::ostringstream ss;
        for (auto const&[itemID, itemSlot] : transmogSetData.ItemData)
        {
            ss << itemSlot << ' ' << itemID << ' ' << static_cast<uint32>(TransmogType::Item) << ' ';
            auto stmt3 = CharacterDatabase.GetPreparedStatement(CHAR_INS_ACCOUNT_TRANSMOG);
            stmt3->setUInt32(0, accountID);
            stmt3->setUInt32(1, static_cast<uint32>(TransmogType::Item));
            stmt3->setUInt32(2, itemID);
            stmt3->setBool(3, true);
            trans->Append(stmt3);
        }

        CharacterDatabase.CommitTransaction(trans);

        if (!isSmallSet)
        {
            auto stmt2 = CharacterDatabase.GetPreparedStatement(CHAR_REP_PURCHASED_TRANSMOG_SETS);
            stmt2->setUInt32(0, accountID);
            stmt2->setUInt32(1, presetID);
            stmt2->setString(2, transmogSetData.SetName);
            stmt2->setString(3, ss.str());
            CharacterDatabase.Query(stmt2);
        }
    }
}

std::vector<uint64> TransmogificationMgr::GetItemList(Player const* player)
{
    std::vector<uint64> itemList;

    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (ItemRef const& item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemList.emplace_back(item->GetGUID());

    for (int i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
        if (ItemRef const& item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemList.emplace_back(item->GetGUID());

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        if (Bag const* bag = player->GetBagByPos(i))
            for (uint32 j = 0; j < bag->GetBagSize(); ++j)
                if (ItemRef const& item = bag->GetItemByPos(j))
                    itemList.emplace_back(item->GetGUID());

    for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
        if (Bag const* bag = player->GetBagByPos(i))
            for (uint32 j = 0; j < bag->GetBagSize(); ++j)
                if (ItemRef const& item = bag->GetItemByPos(j))
                    itemList.emplace_back(item->GetGUID());

    return itemList;
}

TransmogResult TransmogificationMgr::TrySetPendingTransmog(Player* player, uint32 slot, uint32 entry)
{
    using enum TransmogResult;

    // slot of the transmogrified item
    if (slot >= EQUIPMENT_SLOT_END)
        return InvalidSlot;

    ItemTemplate const* itemtemplate = nullptr;
    bool const hasTemplate = entry != TransmogConstants::NormalEntry && entry != TransmogConstants::InvisibleEntry;
    if (hasTemplate)
    {
        itemtemplate = sObjectMgr->GetItemTemplate(entry);
        if (!itemtemplate)
            return NonexistantTransmog;
    }

    // transmogrified item
    ItemRef const& itemTransmogrified = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!itemTransmogrified)
        return EmptySlot;

    if (hasTemplate)
        if (TransmogResult const transmogResult = CannotTransmogrifyItemWithItem(player, itemTransmogrified->GetTemplate(), itemtemplate); transmogResult != OK)
            return transmogResult;

    if (entry == TransmogConstants::InvisibleEntry && (slot == SLOT_MAIN_HAND || slot == SLOT_OFF_HAND || slot == SLOT_RANGED))
        return NonexistantTransmog;

    if (entry == TransmogConstants::InvisibleEntry)
        entry = 0;
    if (entry == TransmogConstants::NormalEntry)
        entry = itemTransmogrified->GetEntry();

    // Set pending transmog
    player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), entry);
    return OK;
}

TransmogResult TransmogificationMgr::TrySetPendingEnchant(Player* player, uint32 slot, uint32 entry)
{
    using enum TransmogResult;

    // slot of the transmogrified item
    if (slot >= EQUIPMENT_SLOT_END)
        return InvalidSlot;

    bool const hasTemplate = entry != TransmogConstants::NormalEntry && entry != TransmogConstants::InvisibleEntry;

    // transmogrified item
    ItemRef const& itemTransmogrified = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!itemTransmogrified)
        return EmptySlot;

    if (hasTemplate)
        if (TransmogResult const transmogResult = CannotTransmogrifyItemWithEnchant(player, itemTransmogrified->GetTemplate(), entry); transmogResult != OK)
            return transmogResult;

    if (entry == TransmogConstants::InvisibleEntry)
        entry = 0;

    if (entry == TransmogConstants::NormalEntry)
        entry = itemTransmogrified->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);

    // Set pending transmog
    player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0, entry);
    return OK;
}

void TransmogificationMgr::Transmogrify(Player* player, ItemRef const& itemTransmogrified, TransmogType type, uint32 entry)
{
    if (entry == TransmogConstants::InvisibleEntry)
        entry = TransmogConstants::InvisibleEntry;

    if (entry == TransmogConstants::NormalEntry)
        entry = 0;

    using enum TransmogType;
    switch (type)
    {
        case Item:
            itemTransmogrified->SetTransmog(entry);
            break;
        case Enchant:
            itemTransmogrified->SetTransmogEnchant(entry);
            break;
        case Max:
            return;
    }

    itemTransmogrified->UpdatePlayedTime(player);
    itemTransmogrified->SetOwnerGUID(player->GetGUID());
    itemTransmogrified->SetNotRefundable(player);
    itemTransmogrified->ClearSoulboundTradeable(player);
    itemTransmogrified->SetBinding(true);
    itemTransmogrified->SetState(ITEM_CHANGED, player);
    UpdateItem(player, itemTransmogrified);
}

TransmogResult TransmogificationMgr::CannotEquip(Player const* player, ItemTemplate const* proto) const
{
    using enum TransmogResult;

    if (!IgnoreReqFaction)
    {
        TeamId const teamID = player->GetTeam(CF_DENY);
        if ((proto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY) && teamID != TEAM_HORDE)
            return InvalidFaction;

        if ((proto->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY) && teamID != TEAM_ALLIANCE)
            return InvalidFaction;
    }

    if (!IgnoreReqClass && (proto->AllowableClass & player->getClassMask()) == 0)
        return InvalidClass;

    if (!IgnoreReqRace && (proto->AllowableRace & player->getRaceMask()) == 0)
        return InvalidRace;

    if (!IgnoreReqSkill && proto->RequiredSkill != 0)
    {
        if (player->GetSkillValue(proto->RequiredSkill) == 0)
            return MissingSkill;

        if (player->GetSkillValue(proto->RequiredSkill) < proto->RequiredSkillRank)
            return MissingSkill;
    }

    if (!IgnoreReqSpell && proto->RequiredSpell != 0 && !player->HasSpell(proto->RequiredSpell))
        return MissingSpell;

    if (!IgnoreReqLevel && player->getLevel() < proto->RequiredLevel)
        return TooLowLevelPlayer;

    return OK;
}

TransmogResult TransmogificationMgr::CannotTransmogrify(ItemTemplate const* proto) const
{
    using enum TransmogResult;

    // select * from item_template where class not in (2,4) and inventorytype in (1,34,5,6,7,8,9,10,13,14,1516,17,19,20,21,22,23,25,26,28)
    if (proto->Class != ITEM_CLASS_ARMOR && proto->Class != ITEM_CLASS_WEAPON)
        return InvalidItemType;

    switch (proto->InventoryType)
    {
        case INVTYPE_HEAD:
        case INVTYPE_SHOULDERS:
        case INVTYPE_BODY:
        case INVTYPE_CHEST:
        case INVTYPE_WAIST:
        case INVTYPE_LEGS:
        case INVTYPE_FEET:
        case INVTYPE_WRISTS:
        case INVTYPE_HANDS:
        case INVTYPE_WEAPON:
        case INVTYPE_SHIELD:
        case INVTYPE_RANGED:
        case INVTYPE_CLOAK:
        case INVTYPE_2HWEAPON:
        case INVTYPE_TABARD:
        case INVTYPE_ROBE:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
        case INVTYPE_HOLDABLE:
        case INVTYPE_THROWN:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_RELIC:
            break;
        default:
            return InvalidItemType;
    }

    // Skip all checks for allowed items
    if (IsAllowed(proto->ItemId))
        return OK;

    if (IsNotAllowed(proto->ItemId))
        return ItemBlocked;

    if (!AllowFishingPoles && proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        return FishingPoleBlocked;

    if (!IsAllowedQuality(proto->Quality))
        return InvalidItemQuality;

    if (!IgnoreReqStats)
    {
        bool const hasStat = std::ranges::any_of(proto->ItemStat, [](_ItemStat const& itemStat) { return itemStat.ItemStatValue != 0; });
        if (!proto->RandomProperty && !proto->RandomSuffix && !hasStat)
            return ItemMustHaveStats;
    }

    return OK;
}

bool IsBound(ItemRef const& item)
{
    if (item->HasFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_BOP_TRADEABLE))
        return false;

    if (!item->IsSoulBound() && !item->IsBoundAccountWide())
        return false;

    return true;
}

bool TransmogificationMgr::CanAddToCollection(Player const* player, ItemRef const& item) const
{
    if (IgnoreReqBound == 0 && !IsBound(item))
        return false;

    ItemTemplate const* temp = item->GetTemplate();
    if (IgnoreReqBound == 1 && !IsBound(item) && temp->Bonding != NO_BIND)
        return false;

    return CanAddToCollection(player, temp);
}

bool TransmogificationMgr::CanAddEnchantToCollection(Player* /*player*/, ItemRef const& /*item*/)
{
    //if (!IgnoreReqBound && !IsBound(item))
    //    return false;
    //if (CannotEquip(player, item->GetTemplate()))
    //    return false;
    return true;
}

bool TransmogificationMgr::CanAddToCollection(Player const* player, ItemTemplate const* itemTemplate) const
{
    if (TransmogResult const transmogResult = CannotTransmogrify(itemTemplate); transmogResult != TransmogResult::OK)
        return false;

    if (TransmogResult const transmogResult = CannotEquip(player, itemTemplate); transmogResult != TransmogResult::OK)
        return false;

    return true;
}

void TransmogificationMgr::SaveToDB(Player const* player, TransmogType transmogtype, uint32 visual, bool purchased)
{
    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ACCOUNT_TRANSMOG);
    stmt->setUInt32(0, player->GetSession()->GetAccountId());
    stmt->setUInt32(1, to_underlying(transmogtype));
    stmt->setUInt32(2, visual);
    stmt->setBool(3, purchased);
    CharacterDatabase.Execute(stmt);
}

bool TransmogificationMgr::HasVisual(Player* player, TransmogType transmogtype, uint32 visual)
{
    /*if (transmogtype == TransmogType::Enchant)
    {
        // Enchants are currently not all saved to appearances
        if (auto it = enchant_to_spells.find(visual); it != enchant_to_spells.end())
            return true;
    }*/

    auto& cont = player->TransmogAppearances[to_underlying(transmogtype)];
    return cont.find(visual) != cont.end();
}

uint32 TransmogificationMgr::Save(Player* player, TransmogType transmogtype, uint32 visual, bool purchased)
{
    if (player->TransmogAppearances[to_underlying(transmogtype)].emplace(visual, purchased).second)
    {
        SaveToDB(player, transmogtype, visual, purchased);
        return visual;
    }

    return 0;
}

uint32 TransmogificationMgr::AddItemVisualToCollection(Player* player, ItemRef const& item, bool purchased)
{
    uint32 const visual = item->GetEntry();
    if (visual == TransmogConstants::InvisibleEntry)
        return 0;

    if (!visual || !CanAddToCollection(player, item))
        return 0;

    return Save(player, TransmogType::Item, visual, purchased);
}

uint32 TransmogificationMgr::AddItemVisualToCollection(Player* player, const ItemTemplate* itemtemplate, bool purchased)
{
    if (!itemtemplate || !CanAddToCollection(player, itemtemplate))
        return 0;

    return Save(player, TransmogType::Item, itemtemplate->ItemId, purchased);
}

uint32 TransmogificationMgr::AddEnchantVisualToCollection(Player* player, uint32 enchant_id)
{
    //// only weapon enchants
    //auto item_template = item->GetTemplate();
    //if (!IsMeleeWeapon(item_template) && !IsBowOrGunOrCrossbow(item_template))
    //    return 0;

    //// only permanent enchants
    //uint32 enchant_id = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
    if (!enchant_id)
        return 0;
    //if (!CanAddEnchantToCollection(player, item))
    //    return 0;

    return Save(player, TransmogType::Enchant, enchant_id, false);
}

void TransmogificationMgr::AddToCollection(Player* player, ItemRef const& item, bool purchased /* = false */)
{
    if (!player)
        return;
    if (!item)
        return;
    if (uint32 const transmog = AddItemVisualToCollection(player, item, purchased))
        ChatHandler(player->GetSession()).SendFormattedSysMessage(TransmogConstants::AddToCollectionMessageFmt, GetItemLink(transmog, player->GetSession()));
}

void TransmogificationMgr::AddToCollection(Player* player, const ItemTemplate* itemtemplate, bool purchased /* = false */)
{
    if (!player)
        return;

    if (!itemtemplate)
        return;

    if (uint32 const transmog = AddItemVisualToCollection(player, itemtemplate, purchased))
        ChatHandler(player->GetSession()).SendFormattedSysMessage(TransmogConstants::AddToCollectionMessageFmt, GetItemLink(transmog, player->GetSession()));
}

void TransmogificationMgr::AddToCollectionEnchant(Player* player, uint32 enchant_id)
{
    if (!player)
        return;

    if (uint32 const enchant = AddEnchantVisualToCollection(player, enchant_id))
    {
        if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant))
        {
            auto const it = EnchantVisualToName.find(enchantEntry->aura_id);
            if (it != EnchantVisualToName.end())
                ChatHandler(player->GetSession()).SendFormattedSysMessage(TransmogConstants::AddToCollectionMessageFmt, it->second);
        }
    }
}

void TransmogificationMgr::RemoveAllTransmogrifications(Player* player)
{
    for (ItemRef const& item : GetEquippedItems(player))
    {
        player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (item->GetSlot() * 2), item->GetEntry());
        player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (item->GetSlot() * 2), 0, item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT));
    }
}

void TransmogificationMgr::RevertAllTransmogrifications(Player* player)
{
    for (auto const& pending : GetPendingTransmogs(player))
        UpdateItem(player, pending.item);
}

bool TransmogificationMgr::RevertTransmogrification(Player* player, uint8 slot)
{
    ItemRef item;
    if (HasPendingTransmog(player, slot, &item))
    {
        uint32 const enchant = player->GetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
        UpdateItem(player, item); // removes pending enchant and transmog
        player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0, enchant); // restore pending enchant
        return true;
    }

    return false;
}

bool TransmogificationMgr::RevertEnchant(Player* player, uint8 slot)
{
    ItemRef item;
    if (HasPendingEnchant(player, slot, &item))
    {
        uint32 const transmog = player->GetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2));
        UpdateItem(player, item); // removes pending enchant and transmog
        player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), transmog); // restore pending transmog
        return true;
    }

    return false;
}

ItemRef TransmogificationMgr::GetEquippedItem(Player const* player, uint8 slot)
{
    return player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
}

std::vector<ItemRef> TransmogificationMgr::GetEquippedItems(Player const* player)
{
    std::vector<ItemRef> items;
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        if (ItemRef item = GetEquippedItem(player, slot))
            items.push_back(item);

    return items;
}

bool TransmogificationMgr::HasPendingTransmog(Player* player, uint8 slot, ItemRef* retItem, uint32* retPending, uint32* retCurrent)
{
    ItemRef const& item = GetEquippedItem(player, slot);
    if (!item)
        return false;

    uint32 current = GetCurrentVisual(item);
    uint32 pending = player->GetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2));
    if (current == item->GetEntry())
        current = TransmogConstants::NormalEntry;

    if (current == 0)
        pending = TransmogConstants::InvisibleEntry;

    if (pending == item->GetEntry())
        pending = TransmogConstants::NormalEntry;

    if (pending == 0)
        pending = TransmogConstants::InvisibleEntry;

    if (pending != current)
    {
        if (pending == TransmogConstants::NormalEntry || pending == TransmogConstants::InvisibleEntry)
        {
            if (TransmogResult const transmogResult = CannotTransmogrifyItem(player, item->GetTemplate()); transmogResult != TransmogResult::OK)
                return false;
        }
        else
        {
            ItemTemplate const* sourceTemplate = sObjectMgr->GetItemTemplate(pending);
            if (!sourceTemplate)
                return false;

            if (TransmogResult const transmogResult = CannotTransmogrifyItemWithItem(player, item->GetTemplate(), sourceTemplate); transmogResult != TransmogResult::OK)
                return false;

        }

        if (retItem)
            *retItem = item;

        if (retPending)
            *retPending = pending;

        if (retCurrent)
            *retCurrent = current;

        return true;
    }
    return false;
}

bool TransmogificationMgr::HasPendingEnchant(Player* player, uint8 slot, ItemRef* retItem, uint32* retPending, uint32* retCurrent)
{
    ItemRef const& item = GetEquippedItem(player, slot);
    if (!item)
        return false;
    uint32 current = GetCurrentVisualEnchant(item);
    uint32 pending = player->GetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (item->GetSlot() * 2), 0);
    if (current == item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT))
        current = TransmogConstants::NormalEntry;

    if (current == 0)
        pending = TransmogConstants::InvisibleEntry;

    if (pending == item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT))
        pending = TransmogConstants::NormalEntry;

    if (pending == 0)
        pending = TransmogConstants::InvisibleEntry;

    if (pending != current)
    {
        if (pending == TransmogConstants::NormalEntry || pending == TransmogConstants::InvisibleEntry)
        {
            if (TransmogResult const transmogResult = CannotTransmogrifyItem(player, item->GetTemplate()); transmogResult != TransmogResult::OK)
                return false;
        }
        else
        {
            SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(pending);
            if (!enchantEntry)
                return false;

            if (TransmogResult const transmogResult = CannotTransmogrifyItem(player, item->GetTemplate()); transmogResult != TransmogResult::OK)
                return false;
        }
        if (retItem)
            *retItem = item;
        if (retPending)
            *retPending = pending;
        if (retCurrent)
            *retCurrent = current;
        return true;
    }
    return false;
}

PendingTransmogs TransmogificationMgr::GetPendingTransmogs(Player* player)
{
    PendingTransmogs pending_transmogs;
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
    {
        ItemRef item;
        uint32 pending;
        if (HasPendingTransmog(player, slot, &item, &pending))
        {
            PendingTransmog pendingTransmog{};
            pendingTransmog.item = item;
            pendingTransmog.transmog = pending;
            pendingTransmog.type = TransmogType::Item;
            pending_transmogs.push_back(pendingTransmog);
        }

        if (HasPendingEnchant(player, slot, &item, &pending))
        {
            PendingTransmog pendingTransmog{};
            pendingTransmog.item = item;
            pendingTransmog.transmog = pending;
            pendingTransmog.type = TransmogType::Enchant;
            pending_transmogs.push_back(pendingTransmog);
        }
    }
    return pending_transmogs;
}

uint32 TransmogificationMgr::GetCurrentVisual(ItemRef const& item)
{
    if (item->GetTransmog())
        return item->GetTransmog();

    return item->GetEntry();
}

uint32 TransmogificationMgr::GetCurrentVisualEnchant(ItemRef const& item)
{
    if (item->GetTransmogEnchant())
        return item->GetTransmogEnchant();

    return item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
}

int32 TransmogificationMgr::CalculateTransmogCost(PendingTransmogs const& items) const
{
    int32 cost = 0;
    for (auto const& pending : items)
    {
        uint32 const entry = pending.transmog;
        switch (pending.type)
        {
            case TransmogType::Item:
                cost += CalculateTransmogCost(entry);
                break;
            case TransmogType::Enchant:
                if (entry == TransmogConstants::InvisibleEntry)
                    break;
                if (entry == TransmogConstants::NormalEntry)
                    break;
                cost += 1 * GOLD;
                break;
            case TransmogType::Max:
                break;
        }
    }

    return cost;
}

int32 TransmogificationMgr::CalculateTransmogCost(uint32 entry) const
{
    if (entry == TransmogConstants::InvisibleEntry)
        return 0;

    if (entry == TransmogConstants::NormalEntry)
        return 0;

    ItemTemplate const* temp = sObjectMgr->GetItemTemplate(entry);
    if (!temp)
        return 0;

    int32 cost = 0;
    cost += GetSpecialPrice(temp);
    cost *= ScaledCostModifier;
    cost += CopperCost;
    return std::max<int32>(cost, 0);
}

TransmogResult TransmogificationMgr::TransmogrifyPending(Player* player, int32 expectedCost /*= -1*/)   
{
    using enum TransmogResult;

    auto const pending = GetPendingTransmogs(player);

    if (pending.empty())
        return NoPendingTransmogs;

    int32 const cost = CalculateTransmogCost(pending);

    if (cost >= 0 && cost != expectedCost)
        return CostChangedDuringTransaction;

    if (cost > 0 && !player->HasEnoughMoney(cost, player->IsPathOfWar()))
        return NotEnoughMoney;

    player->ModifyMoney(-cost);

    for (auto const& [item, transmog, type] : pending)
        Transmogrify(player, item, type, transmog);

    return OK;
}
