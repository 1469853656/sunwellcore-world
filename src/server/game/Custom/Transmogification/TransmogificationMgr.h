#ifndef TRANSMOGRIFICATION_H
#define TRANSMOGRIFICATION_H

#include "Define.h"
#include "TransmogificationDefines.h"
#include "Singleton.h"

#include <vector>
#include <unordered_map>

class Item;
class Player;
class WorldSession;
class SpellInfo;
struct ItemTemplate;

struct TransmogSetData
{
    std::string SetName;
    std::vector<std::pair<uint32, uint32>> ItemData;
};

struct TransmogificationMgr : Sunwell::Singleton<TransmogificationMgr>
{
    friend struct Sunwell::Singleton<TransmogificationMgr>;

    bool EnableTransmogrification;
    bool EnableEnchantTransmogrification;

    bool EnableSetInfo;
    uint32 SetNpcText;

    bool EnableSets;
    uint8 MaxSets;

    bool EnableTransmogInfo;
    uint32 TransmogNpcText;

    // Use IsAllowed() and IsNotAllowed()
    // these are thread unsafe, but assumed to be data so it should be safe
    std::set<uint32> Allowed;
    std::set<uint32> NotAllowed;

    float ScaledCostModifier;
    int32 CopperCost;

    bool AllowPoor;
    bool AllowCommon;
    bool AllowUncommon;
    bool AllowRare;
    bool AllowEpic;
    bool AllowLegendary;
    bool AllowArtifact;
    bool AllowHeirloom;

    bool AllowMixedArmorTypes;
    bool AllowMixedWeaponTypes;
    bool AllowMixedInventoryTypes;
    bool AllowFishingPoles;

    bool IgnoreReqRace;
    bool IgnoreReqClass;
    bool IgnoreReqSkill;
    bool IgnoreReqSpell;
    bool IgnoreReqLevel;
    bool IgnoreReqEvent;
    bool IgnoreReqStats;
    bool IgnorePlayerMissingProfiency;
    bool IgnoreReqFaction;
    int IgnoreReqBound;

    std::unordered_map<uint32, std::string> EnchantVisualToName; // contains all enchant visuals (not enchants) that should be viewable in menus

    std::unordered_map<uint32, TransmogSetData> TransmogSets;

    bool IsAllowed(uint32 entry) const;
    bool IsNotAllowed(uint32 entry) const;
    bool IsAllowedQuality(uint32 quality) const;
    static bool IsBowOrGunOrCrossbow(ItemTemplate const* itemTemplate);
    static bool IsMeleeWeapon(ItemTemplate const* itemTemplate);

    std::unordered_map<uint32, std::vector<SpellInfo const*>> enchant_to_spells; // contains all enchants. Some spell lists can be empty

    void LoadConfig(); // thread unsafe
    void LoadEnchants(bool reload = false); // thread unsafe
    void LoadTransmogSets();

    void AddTransmogSetForPlayer(uint32 accountID, uint32 transmogSetID);

    static std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y);
    static std::string GetSlotIcon(uint8 slot, uint32 width, uint32 height, int x, int y);
    static std::string GetSlotName(uint8 slot);
    static std::string GetItemName(ItemTemplate const* itemTemplate, WorldSession const* session);
    static std::string GetItemName(Item const* item, WorldSession const* session);
    std::string GetItemLink(Item const* item, WorldSession const* session) const;
    std::string GetItemLink(uint32 entry, WorldSession const* session) const;
    static ItemRef GetEquippedItem(Player const* player, uint8 slot);
    static void UpdateItem(Player* player, ItemRef const& item);
    static void RevertPendingTransmogifications(Player* player);

    TransmogResult CannotTransmogrifyItemWithItem(Player* player, ItemTemplate const* target, ItemTemplate const* source);
    TransmogResult CannotTransmogrifyItemWithEnchant(Player* player, ItemTemplate const* destination, uint32 enchant);
    TransmogResult CannotTransmogrifyItem(Player* player, ItemTemplate const* proto);
    TransmogResult CannotEquip(Player const* player, ItemTemplate const* proto) const;
    TransmogResult CannotTransmogrify(ItemTemplate const* proto) const;
    bool HasPendingTransmog(Player* player, uint8 slot, ItemRef* retItem = nullptr, uint32* retPending = nullptr, uint32* retCurrent = nullptr);
    bool HasPendingEnchant(Player* player, uint8 slot, ItemRef* retItem = nullptr, uint32* retPending = nullptr, uint32* retCurrent = nullptr);

    // Call these from core to add visuals to player's collection
    void AddToCollection(Player* player, ItemRef const& item, bool purchased = false);
    void AddToCollection(Player* player, const ItemTemplate* itemtemplate, bool purchased = false);
    void AddToCollectionEnchant(Player* player, uint32 enchant_id);

    uint32 AddItemVisualToCollection(Player* player, ItemRef const& item, bool purchased);
    uint32 AddItemVisualToCollection(Player* player, const ItemTemplate* itemtemplate, bool purchased);
    uint32 AddEnchantVisualToCollection(Player* player, uint32 enchant_id);
    bool CanAddToCollection(Player const* player, ItemRef const& item) const;
    bool CanAddToCollection(Player const* player, ItemTemplate const* itemTemplate) const;
    static bool CanAddEnchantToCollection(Player* player, ItemRef const& item);
    uint32 Save(Player* player, TransmogType transmogtype, uint32 visual, bool purchased);
    static void SaveToDB(Player const* player, TransmogType transmogtype, uint32 visual, bool purchased);

    bool HasVisual(Player* player, TransmogType transmogtype, uint32 visual);

    static std::vector<uint64> GetItemList(Player const* player);
    static std::vector<ItemRef> GetEquippedItems(Player const* player);
    PendingTransmogs GetPendingTransmogs(Player* player);
    static uint32 GetCurrentVisual(ItemRef const& item);
    static uint32 GetCurrentVisualEnchant(ItemRef const& item);

    static uint32 GetSpecialPrice(ItemTemplate const* proto);
    int32 CalculateTransmogCost(uint32 entry) const;
    int32 CalculateTransmogCost(PendingTransmogs const& items) const;

    TransmogResult TrySetPendingTransmog(Player* player, uint32 slot, uint32 entry);
    TransmogResult TrySetPendingEnchant(Player* player, uint32 slot, uint32 entry);
    TransmogResult TransmogrifyPending(Player* player, int32 expectedCost = -1);
    static void Transmogrify(Player* player, ItemRef const& transmogrified, TransmogType type, uint32 entry);

    static void RemoveAllTransmogrifications(Player* player);
    void RevertAllTransmogrifications(Player* player);
    bool RevertTransmogrification(Player* player, uint8 slot);
    bool RevertEnchant(Player* player, uint8 slot);
};

#define sTransmogificationMgr TransmogificationMgr::instance()

#endif
