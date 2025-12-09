#ifndef TRANSMOGRIFICATION_DEFINES_H
#define TRANSMOGRIFICATION_DEFINES_H

#include "Define.h"

#include <limits>

enum class TransmogType : uint32
{
    Item,
    Enchant,
    Max
};

enum class TransmogResult : uint32
{
    OK = 0,
    InvalidItemType,
    ItemBlocked,
    FishingPoleBlocked,
    InvalidItemQuality,
    RequiredEventNotActive,
    ItemMustHaveStats,
    InvalidFaction,
    InvalidClass,
    InvalidRace,
    MissingProfiency,
    MissingSkill,
    TooLowSkill,
    MissingSpell,
    TooLowLevelPlayer,
    TooLowLevelItem,
    TooHighLevelItem,
    ItemTypesDontMatch,
    ArmorTypesDontMatch,
    WeaponTypesDontMatch,
    EquipSlotsDontMatch,
    InvalidSlot,
    NonexistantTransmog,
    EmptySlot,
    NoPendingTransmogs,
    CostChangedDuringTransaction,
    NotEnoughMoney,
    TooLongSetName,
    NoTransmogrifications,
    AtMaxSets,
    NonexistantSet,
    ItemNotFitForEnchantRequirements
};

constexpr std::string_view CanTransmogrifyResultMessage(TransmogResult const result)
{
    using enum TransmogResult;
    switch (result)
    {
        case OK: return "Item transmogrified";
        case InvalidItemType: return "This type of item cannot be transmogrified";
        case ItemBlocked: return "This item cannot be transmogrified";
        case FishingPoleBlocked: return "Fishing poles cannot be transmogrified";
        case InvalidItemQuality: return "This item quality cannot be transmogrified";
        case RequiredEventNotActive: return "Required event to use this item is not active";
        case ItemMustHaveStats: return "Transmogrifiable item must have stat effects";
        case InvalidFaction: return "Your faction cannot use that item";
        case InvalidClass: return "Your class cannot use that item";
        case InvalidRace: return "Your race cannot use that item";
        case MissingProfiency: return "You do not have the profiency to use that item";
        case MissingSkill: return "You do not have the required skill to use that item";
        case TooLowSkill: return "You do not have the required skill level to use that item";
        case MissingSpell: return "You do not have the required spell to use that item";
        case TooLowLevelPlayer: return "Your level is too low to use that item";
        case TooLowLevelItem: return "The item is too low level";
        case TooHighLevelItem: return "The item is too high level";
        case ItemTypesDontMatch: return "The item types do not match";
        case ArmorTypesDontMatch: return "The armor types do not match";
        case WeaponTypesDontMatch: return "The weapon types do not match";
        case EquipSlotsDontMatch: return "The item equip slots do not allow transmogrifying";
        case InvalidSlot: return "Items equipped in this slot cannot be transmogrified";
        case NonexistantTransmog: return "The selected transmogrification does not exist";
        case EmptySlot: return "You do not have an item equipped in that slot";
        case NoPendingTransmogs: return "You have no pending transmogrifications";
        case CostChangedDuringTransaction: return "Cost changed during transaction";
        case NotEnoughMoney: return "Not enough money";
        case TooLongSetName: return "The set name is too long or too short";
        case NoTransmogrifications: return "You have no items equipped with pending or non-pending transmogrifications";
        case AtMaxSets: return "You cannot save more sets";
        case NonexistantSet: return "The set does not exist";
        case ItemNotFitForEnchantRequirements: return "The item is not fit for the enchant";
    }

    return "";
}

namespace TransmogConstants
{
    constexpr static std::string_view OkPendingMessage = "Pending transmog added";
    constexpr static std::string_view AddToCollectionMessageFmt = "{} has been added to your appearance collection";

    constexpr static uint32 InvisibleEntry = std::numeric_limits<uint32>::max();
    constexpr static uint32 NormalEntry = InvisibleEntry - 1;
    constexpr static uint32 RemovePending = NormalEntry - 1;
    constexpr static uint32 AbsoluteMaxSets = 25;
}


class Item;

struct PendingTransmog
{
    ItemRef item;
    uint32 transmog;
    TransmogType type;
};

using PendingTransmogs = std::vector<PendingTransmog>;

struct SetTransmog
{
    uint8 slot = 0;
    uint32 transmog = 0;
    TransmogType type = TransmogType::Item;
    bool IsPurchased = false;
};

using SetTransmogs = std::vector<SetTransmog>;

#endif
