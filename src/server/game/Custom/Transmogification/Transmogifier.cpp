
#include "TransmogificationMgr.h"
#include "Common.h"
#include "Config.h"
#include "Creature.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "Define.h"
#include "Field.h"
#include "GossipDef.h"
#include "Item.h"
#include "Log.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Transaction.h"
#include "WorldSession.h"

#include <string>

#include "Chat.h"
#include "CustomMgr.h"

std::array<uint8, 2> ENCHANT_SLOTS =
{
    EQUIPMENT_SLOT_MAINHAND,
    EQUIPMENT_SLOT_OFFHAND,
};

namespace TransmogifierConstants
{
    static constexpr uint32 MaxListItems = 100000; // absolute maximum amount of items in a list to paginate
    static constexpr uint32 MaxPageItems = 10; // maximum amount of items in a page from a paginated list
    static constexpr uint32 MaxPages = (MaxListItems / MaxPageItems) + 1; // Max amount of pages for a single paginated list
}

enum TransmogGossipSender
{
    // sender = EQUIPMENT_SLOT_X, action = transmog entry
    SENDER_TRANSMOG_START = EQUIPMENT_SLOT_START,
    SENDER_TRANSMOG_END = EQUIPMENT_SLOT_END,

    // action = 0 or transmog/enchant entry
    // sender = SENDER_PAGINATED_EQUIPMENT_START + (equipment_slot*max_pages) + page
    // sender = SENDER_PAGINATED_EQUIPMENT_START + ((EQUIPMENT_SLOT_END+1)*max_pages) + page // main hand enchant
    // sender = SENDER_PAGINATED_EQUIPMENT_START + ((EQUIPMENT_SLOT_END+2)*max_pages) + page // off hand enchant
    SENDER_PAGINATED_EQUIPMENT_START = SENDER_TRANSMOG_END,
    SENDER_PAGINATED_EQUIPMENT_END = SENDER_PAGINATED_EQUIPMENT_START + ((EQUIPMENT_SLOT_END - EQUIPMENT_SLOT_START)*TransmogifierConstants::MaxPages),
    SENDER_PAGINATED_ENCHANT_START = SENDER_PAGINATED_EQUIPMENT_END,
    SENDER_PAGINATED_ENCHANT_END = SENDER_PAGINATED_ENCHANT_START + (ENCHANT_SLOTS.size()*TransmogifierConstants::MaxPages),

    SENDER_SHOW_MAIN_MENU,
    SENDER_SHOW_ITEMS,
    SENDER_SHOW_ENCHANTS,
    SENDER_SHOW_SLOTS,
    SENDER_REVERT_ALL_TRANSMOG,
    SENDER_REMOVE_ALL_TRANSMOG,
    SENDER_REVERT_TRANSMOG,
    SENDER_REVERT_ENCHANT,
    SENDER_PRESET_LIST,
    SENDER_PRESET_PURCHASED_LIST,
    SENDER_PRESET_USE,
    SENDER_PRESET_PURCHASED_USE,
    SENDER_PRESET_VIEW,
    SENDER_PRESET_PURCHASED_VIEW,
    SENDER_PRESET_DELETE,
    SENDER_PRESET_SAVE,
    SENDER_PRESET_INFO,
    SENDER_PRESET_PURCHASED_INFO,
    SENDER_TRANSMOG_INFO,
    SENDER_TRANSMOG_SAVE,
    SENDER_CLOSE_MENU,
    SENDER_PRESET_PAGE_START,
    SENDER_PRESET_PAGE_END = SENDER_PRESET_PAGE_START + 1000,
    SENDER_PRESET_PURCHASED_PAGE_START,
    SENDER_PRESET_PURCHASED_PAGE_END = SENDER_PRESET_PURCHASED_PAGE_START + 1000,
};

enum TransmogGossipSenderCode
{
    SENDER_CODE_PRESET_SAVE,
    SENDER_CODE_PRESET_RENAME,
};

struct PageData
{
    PageData(uint32 page, uint32 total_actions)
    {
        start = page * TransmogifierConstants::MaxPageItems;
        end = std::min((page + 1) * TransmogifierConstants::MaxPageItems, total_actions);
        count = end - start;
        next = end < total_actions;
        prev = page > 0;
        ismainpage = page == 0;
        current_page = page + 1;
        total_pages = static_cast<decltype(total_pages)>(std::ceil(total_actions/static_cast<double>(TransmogifierConstants::MaxPageItems)));
    }
    uint32 start;
    uint32 end;
    uint32 count;
    uint32 current_page;
    uint32 total_pages;
    bool next;
    bool prev;
    bool ismainpage;
};

using TransmogFunc = std::function<void(Player* player, Creature* creature, uint32 base, uint32 page, uint32 action)>;

struct PendingItemsCheck final : BasicEvent
{
    PendingItemsCheck(Player* player, Creature const* creature) : guid(creature->GetGUID()), player(player)
    {
        //ASSERT(player->PendingTransmogCheck == nullptr, "Only one PendingTransmogCheck should be active at one time");
        player->PendingTransmogCheck = this;
        player->m_Events.AddEventAtOffset(this, 500ms);
    }

    bool Execute(uint64, uint32) override
    {
        auto const pending_transmogs = sTransmogificationMgr->GetPendingTransmogs(player);
        if (!pending_transmogs.empty())
        {
            Creature const* creature = ObjectAccessor::GetCreature(*player, guid);
            if (creature)
            {
                if (!creature->IsWithinDistInMap(player, creature->GetCombatReach()) && player->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_GOSSIP) == nullptr)
                {
                    player->GetSession()->SendNotification("You have not saved your transmogrifications!");
                    sTransmogificationMgr->RevertPendingTransmogifications(player);
                    player->PendingTransmogCheck = nullptr;
                    return true;
                }

                player->m_Events.AddEventAtOffset(this, 500ms);
                return false;
            }
        }

        player->PendingTransmogCheck = nullptr;
        for (auto const& pending : pending_transmogs)
            sTransmogificationMgr->UpdateItem(player, pending.item);

        return true;
    }

    uint64 guid;
    Player* player;
};

struct TransmogAction
{
    uint32 AppearanceID;
    std::string ItemLink;
    std::string ItemName;
    bool IsPurchased = false;
    uint32 ItemRarity = 0;

    uint32 GetSortPriority() const
    {
        if (IsPurchased)
            return 6;

        switch (ItemRarity)
        {
            case ITEM_QUALITY_LEGENDARY:
                return 5;
            case ITEM_QUALITY_EPIC:
                return 4;
            case ITEM_QUALITY_RARE:
                return 3;
            case ITEM_QUALITY_UNCOMMON:
                return 2;
            case ITEM_QUALITY_NORMAL:
                return 1;
            case ITEM_QUALITY_POOR:
                return 0;
            default:
                break;
        }

        return 0;
    }
};

class CS_Transmogrification : public CreatureScript
{
public:
    CS_Transmogrification() : CreatureScript("Creature_Transmogrify") { }

    class TransmogAI : public ScriptedAI
    {
    public:
        TransmogAI(Creature* creature) : ScriptedAI(creature) {}

        void sGossipSelect(Player* player, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            if (GossipMenuItem const* gossipItem = player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
            {
                uint32 const sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
                uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
                OnGossipSelect(player, me, sender, action, gossipItem->BoxMoney);
            }
            else
                sGossipHello(player);
        }

        bool CanBeSeen(Player const* /*player*/) override
        {
            return sTransmogificationMgr->EnableTransmogrification;
        }

        void sGossipSelectCode(Player* player, uint32 /*menu_id*/, uint32 gossipListId, const char* code) override
        {
            uint32 const sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            GossipSelectCode(player, me, sender, action, code);
        }

        static void AddItemTransmog(Player* player, Creature const* /*creature*/, uint32 base, uint32 page, uint32 action, std::string const & name, bool isPurchased)
        {
            uint32 const itementry = action;
            if (itementry == TransmogConstants::RemovePending)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|tRemove pending transmogrification", SENDER_REVERT_TRANSMOG, base + page);
            else if (itementry == TransmogConstants::InvisibleEntry)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|tHide item", base + page, TransmogConstants::InvisibleEntry);
            else if (itementry == TransmogConstants::NormalEntry)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_Holy_Restoration:30:30:-18:0|tRestore original look", base + page, TransmogConstants::NormalEntry);
            else
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, fmt::format("{}{}{}", sTransmogificationMgr->GetItemIcon(itementry, 30, 30, -18, 0), name, isPurchased ? "\n          [purchased]" : "").c_str(), base + page, itementry);
        }

        static void AddItemEnchant(Player* player, Creature const* /*creature*/, uint32 base, uint32 page, uint32 action, std::string const & name)
        {
            uint32 const enchantentry = action;
            if (enchantentry == TransmogConstants::RemovePending)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|tRemove pending enchant", SENDER_REVERT_ENCHANT, base + page);
            else if (enchantentry == TransmogConstants::InvisibleEntry)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|tHide enchant", base + page, TransmogConstants::InvisibleEntry);
            else if (enchantentry == TransmogConstants::NormalEntry)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_Holy_Restoration:30:30:-18:0|tRestore original look", base + page, TransmogConstants::NormalEntry);
            else
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, (std::string("|TInterface/ICONS/INV_Enchant_FormulaGood_01:30:30:-18:0|t") + name).c_str(), base + page, enchantentry);
        }

        // value in [min, max[
        static bool isbetween(uint32 value, uint32 min, uint32 max)
        {
            return value >= min && value < max;
        }

        static bool CanUseTransmog(Player const* player)
        {
            if (player->IsGameMaster())
                return true;

            if (sCustomMgr->CheckRealmPhase(RealmPhase::REALM_PHASE_PTR))
                return true;

            return player->GetSession()->HasPremiumService(PremiumServices::WhitemanePlus);
        }

        void sGossipHello(Player* player) override
        {
            if (!CanUseTransmog(player))
            {
                ChatHandler(player->GetSession()).SendSysMessage("To use limitless trasmogrification, you must have an active Whitemane+ status. Learn more at playerid.me");
                CloseGossipMenuFor(player);
                return;
            }

            ClearGossipMenuFor(player);
            if (sTransmogificationMgr->EnableTransmogInfo)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Book_11:30:30:-18:0|tHow does Limitless Transmog work?", SENDER_TRANSMOG_INFO, 0);
            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
            {
                if (std::string slotName = sTransmogificationMgr->GetSlotName(slot); !slotName.empty())
                {
                    ItemRef const& newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    {
                        std::string status;
                        uint32 pending;
                        uint32 entry = newItem ? newItem->GetTransmog() : 0;
                        bool hasPending = sTransmogificationMgr->HasPendingTransmog(player, slot, nullptr, &pending);
                        bool const hasTransmog = newItem && newItem->GetTransmog() != 0;
                        if (hasTransmog)
                            status += " [transmog]";
                        if (hasPending)
                        {
                            entry = pending;
                            status += " [pending]";
                        }
                        std::string icon;
                        if (entry == TransmogConstants::InvisibleEntry)
                            icon = "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|t";
                        else if (hasPending && entry == TransmogConstants::NormalEntry)
                            icon = "|TInterface/ICONS/Spell_Holy_Restoration:30:30:-18:0|t";
                        else
                            icon = entry ? sTransmogificationMgr->GetItemIcon(entry, 30, 30, -18, 0) : sTransmogificationMgr->GetSlotIcon(slot, 30, 30, -18, 0);
                        AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, fmt::format("{}{}{}", icon, slotName, status).c_str(), SENDER_SHOW_ITEMS, slot);
                    }

                    if (sTransmogificationMgr->EnableEnchantTransmogrification)
                    {
                        for (uint32 slotindex = 0; slotindex < ENCHANT_SLOTS.size(); ++slotindex)
                        {
                            if (slot == ENCHANT_SLOTS[slotindex])
                            {
                                std::string status;
                                uint32 pending;
                                uint32 entry = newItem ? newItem->GetTransmogEnchant() : 0;
                                bool hasPending = sTransmogificationMgr->HasPendingEnchant(player, slot, nullptr, &pending);
                                bool hasTransmog = newItem && newItem->GetTransmogEnchant() != 0;
                                if (hasTransmog)
                                    status += " [enchant]";
                                if (hasPending)
                                {
                                    entry = pending;
                                    status += " [pending]";
                                }
                                std::string icon;
                                if (entry == TransmogConstants::InvisibleEntry)
                                    icon = "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|t";
                                else if (hasPending && entry == TransmogConstants::NormalEntry)
                                    icon = "|TInterface/ICONS/Spell_Holy_Restoration:30:30:-18:0|t";
                                else if (hasPending)
                                    icon = "|TInterface/ICONS/INV_Scroll_06:30:30:-18:0|t";
                                else
                                    icon = "|TInterface/ICONS/INV_Scroll_03:30:30:-18:0|t";
                                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, fmt::format("{}{} enchant{}", icon, slotName, status).c_str(), SENDER_SHOW_ENCHANTS, slotindex);
                            }
                        }
                    }
                }
            }

            if (sTransmogificationMgr->EnableSets)
            {
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/RAIDFRAME/UI-RAIDFRAME-MAINASSIST:30:30:-18:0|tManage sets", SENDER_PRESET_LIST, 0);
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/RAIDFRAME/UI-RAIDFRAME-MAINASSIST:30:30:-18:0|tPurchased sets", SENDER_PRESET_PURCHASED_LIST, 0);
            }

            {
                decltype(auto) pending = sTransmogificationMgr->GetPendingTransmogs(player);
                int64 copperCost = sTransmogificationMgr->CalculateTransmogCost(pending);
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_EssenceCosmicGreater:30:30:-18:0|tSave pending transmogrifications", SENDER_TRANSMOG_SAVE, 0, "All equipped items with pending transmogrifications will save their appearance and are made soulbound, non-refundable and non-tradeable.\n\nDo you wish to continue?", static_cast<uint32>(copperCost), false);
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|tCancel pending transmogrifications", SENDER_REVERT_ALL_TRANSMOG, 0, "Cancel pending transmogrifications from all equipped items?", 0, false);
            }
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_Holy_Restoration:30:30:-18:0|tRestore original look", SENDER_REMOVE_ALL_TRANSMOG, 0, "Equipped items are reverted to their original look with a pending change.\n\nDo you want to continue?", 0, false);
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tUpdate menu", SENDER_SHOW_SLOTS, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me);
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action, uint32 boxMoney = 0)
        {
            ClearGossipMenuFor(player);
            WorldSession* session = player->GetSession();
            switch (sender)
            {
                case SENDER_SHOW_ITEMS:
                {
                    uint32 const slot = action;
                    constexpr uint32 page = 0;
                    ShowTransmogs(player, creature, SENDER_PAGINATED_EQUIPMENT_START, TransmogType::Item, slot, page);
                } break;
                case SENDER_SHOW_ENCHANTS:
                {
                    uint32 const slotindex = action;
                    constexpr uint32 page = 0;
                    ShowTransmogs(player, creature, SENDER_PAGINATED_ENCHANT_START, TransmogType::Enchant, slotindex, page);
                } break;
                case SENDER_SHOW_MAIN_MENU:
                case SENDER_SHOW_SLOTS:
                    sGossipHello(player);
                    break;
                case SENDER_REVERT_ALL_TRANSMOG:
                {
                    sTransmogificationMgr->RevertAllTransmogrifications(player);
                    sGossipHello(player);
                } break;
                case SENDER_REMOVE_ALL_TRANSMOG:
                {
                    sTransmogificationMgr->RemoveAllTransmogrifications(player);
                    sGossipHello(player);
                } break;
                case SENDER_REVERT_TRANSMOG:
                {
                    uint32 slot, page;
                    GetTransmogSlotAndPage(action, SENDER_PAGINATED_EQUIPMENT_START, slot, page);
                    sTransmogificationMgr->RevertTransmogrification(player, static_cast<uint8>(slot));
                    OnGossipSelect(player, creature, action, 0);
                } break;
                case SENDER_REVERT_ENCHANT:
                {
                    uint32 slotindex, page;
                    GetTransmogSlotAndPage(action, SENDER_PAGINATED_ENCHANT_START, slotindex, page);
                    sTransmogificationMgr->RevertEnchant(player, ENCHANT_SLOTS[slotindex]);
                    OnGossipSelect(player, creature, action, 0);
                } break;
                case SENDER_PRESET_LIST: // Presets menu
                    ShowPresets(player, creature, 0);
                    break;
                case SENDER_PRESET_PURCHASED_LIST:
                    ShowPurchasedPresets(player, creature, 0);
                    break;
                case SENDER_PRESET_USE: // Use preset
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }
                    // action = presetID
                    if (action >= player->TransmogPresets.size())
                    {
                        sGossipHello(player);
                        return true;
                    }

                    auto const& preset = player->TransmogPresets[action];
                    bool appliedAny = false;
                    for (auto const& transmog : preset.Transmogs)
                    {
                        switch (transmog.type)
                        {
                            case TransmogType::Item:
                                if (TransmogResult const transmogResult = sTransmogificationMgr->TrySetPendingTransmog(player, transmog.slot, transmog.transmog); transmogResult != TransmogResult::OK)
                                    session->SendNotification("%s", CanTransmogrifyResultMessage(transmogResult).data());
                                else
                                    appliedAny = true;

                                break;
                            case TransmogType::Enchant:
                                if (TransmogResult const transmogResult = sTransmogificationMgr->TrySetPendingEnchant(player, transmog.slot, transmog.transmog); transmogResult != TransmogResult::OK)
                                    session->SendNotification("%s", CanTransmogrifyResultMessage(transmogResult).data());
                                else
                                    appliedAny = true;

                                break;
                            case TransmogType::Max:
                                break;
                        }
                    }

                    if (appliedAny)
                        if (!player->PendingTransmogCheck)
                            new PendingItemsCheck(player, creature);

                    OnGossipSelect(player, creature, SENDER_PRESET_VIEW, action);
                } break;
                case SENDER_PRESET_PURCHASED_USE: // Use preset
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }

                    // action = presetID
                    if (action >= player->PurchasedTransmogPresets.size())
                    {
                        sGossipHello(player);
                        return true;
                    }

                    auto const& preset = player->PurchasedTransmogPresets[action];
                    bool appliedAny = false;
                    for (auto const& transmog : preset.Transmogs)
                    {
                        switch (transmog.type)
                        {
                            case TransmogType::Item:
                                if (TransmogResult const transmogResult = sTransmogificationMgr->TrySetPendingTransmog(player, transmog.slot, transmog.transmog); transmogResult != TransmogResult::OK)
                                    session->SendNotification("%s", CanTransmogrifyResultMessage(transmogResult).data());
                                else
                                    appliedAny = true;

                                break;
                            case TransmogType::Enchant:
                                if (TransmogResult const transmogResult = sTransmogificationMgr->TrySetPendingEnchant(player, transmog.slot, transmog.transmog); transmogResult != TransmogResult::OK)
                                    session->SendNotification("%s", CanTransmogrifyResultMessage(transmogResult).data());
                                else
                                    appliedAny = true;

                                break;
                            case TransmogType::Max:
                                break;
                        }
                    }

                    if (appliedAny)
                        if (!player->PendingTransmogCheck)
                            new PendingItemsCheck(player, creature);

                    OnGossipSelect(player, creature, SENDER_PRESET_PURCHASED_VIEW, action);
                } break;
                case SENDER_PRESET_VIEW: // view preset
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }
                    // action = presetID
                    if (action >= player->TransmogPresets.size())
                    {
                        OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                        return true;
                    }
                        
                    auto const& preset = player->TransmogPresets[action];
                    for (auto const& transmogPreset : preset.Transmogs)
                    {
                        switch (transmogPreset.type)
                        {
                            case TransmogType::Item:
                            {
                                std::string text;
                                if (transmogPreset.transmog == TransmogConstants::InvisibleEntry)
                                    text = "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|t Hidden";
                                else
                                    text = sTransmogificationMgr->GetItemIcon(transmogPreset.transmog, 30, 30, -18, 0) + sTransmogificationMgr->GetItemLink(transmogPreset.transmog, session);

                                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, text.c_str(), sender, action);
                                break;
                            }
                            case TransmogType::Enchant:
                            {
                                uint32 const enchant = transmogPreset.transmog;
                                if (SpellItemEnchantmentEntry const* source = sSpellItemEnchantmentStore.LookupEntry(enchant))
                                {
                                    auto enchantVisualIt = sTransmogificationMgr->EnchantVisualToName.find(source->aura_id);
                                    if (enchantVisualIt != sTransmogificationMgr->EnchantVisualToName.end())
                                    {
                                        if (std::string slotName = sTransmogificationMgr->GetSlotName(transmogPreset.slot); !slotName.empty())
                                            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, ("|TInterface/ICONS/INV_Scroll_05:30:30:-18:0|t" + std::string(slotName) + std::string(" enchant: ") + enchantVisualIt->second).c_str(), sender, action);
                                    }
                                }
                                break;
                            }
                            case TransmogType::Max:
                                break;
                        }
                    }

                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|tUse set", SENDER_PRESET_USE, action, ("Using a set will remove pending transmogrifications and add the transmogrifications from the set as pending.\nDo you wish to continue?\n\n" + preset.Name).c_str(), 0, false);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|tRename set", SENDER_CODE_PRESET_RENAME, action, ("Insert new name\n\nOld name: " + preset.Name).c_str(), 0, true);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-LeaveItem-Opaque:30:30:-18:0|tDelete set", SENDER_PRESET_DELETE, action, ("Are you sure you want to delete " + preset.Name + "?").c_str(), 0, false);

                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", SENDER_PRESET_LIST, 0);
                    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                } break;
                case SENDER_PRESET_PURCHASED_VIEW:
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }
                    // action = presetID
                    auto const& preset = player->PurchasedTransmogPresets[action];
                    if (action >= player->PurchasedTransmogPresets.size())
                    {
                        OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                        return true;
                    }

                    for (auto const& transmogPreset : preset.Transmogs)
                    {
                        switch (transmogPreset.type)
                        {
                            case TransmogType::Item:
                            {
                                std::string text;
                                if (transmogPreset.transmog == TransmogConstants::InvisibleEntry)
                                    text = "|TInterface/ICONS/Ability_Vanish:30:30:-18:0|t Hidden";
                                else
                                    text = sTransmogificationMgr->GetItemIcon(transmogPreset.transmog, 30, 30, -18, 0) + sTransmogificationMgr->GetItemLink(transmogPreset.transmog, session);

                                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, text.c_str(), sender, action);
                                break;
                            }
                            case TransmogType::Enchant:
                            {
                                uint32 const enchant = transmogPreset.transmog;
                                if (SpellItemEnchantmentEntry const* source = sSpellItemEnchantmentStore.LookupEntry(enchant))
                                {
                                    auto enchantVisualIt = sTransmogificationMgr->EnchantVisualToName.find(source->aura_id);
                                    if (enchantVisualIt != sTransmogificationMgr->EnchantVisualToName.end())
                                    {
                                        if (std::string slotName = sTransmogificationMgr->GetSlotName(transmogPreset.slot); !slotName.empty())
                                            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, ("|TInterface/ICONS/INV_Scroll_05:30:30:-18:0|t" + std::string(slotName) + std::string(" enchant: ") + enchantVisualIt->second).c_str(), sender, action);
                                    }
                                }
                                break;
                            }
                            case TransmogType::Max:
                                break;
                        }
                    }

                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|tUse set", SENDER_PRESET_PURCHASED_USE, action, ("Using a set will remove pending transmogrifications and add the transmogrifications from the set as pending.\nDo you wish to continue?\n\n" + preset.Name).c_str(), 0, false);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", SENDER_PRESET_PURCHASED_LIST, 0);
                    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                } break;
                case SENDER_PRESET_DELETE: // Delete preset
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }

                    // action = presetID
                    if (action >= player->TransmogPresets.size())
                    {
                        sGossipHello(player);
                        return true;
                    }

                    player->TransmogPresets.erase(player->TransmogPresets.begin() + action);

                    OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                } break;
                case SENDER_PRESET_INFO: // Set info
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", SENDER_PRESET_LIST, 0);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tGo to main menu..", SENDER_SHOW_SLOTS, 0);
                    SendGossipMenuFor(player, sTransmogificationMgr->SetNpcText, creature->GetGUID());
                } break;
                case SENDER_PRESET_PURCHASED_INFO:
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", SENDER_PRESET_PURCHASED_LIST, 0);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tGo to main menu..", SENDER_SHOW_SLOTS, 0);
                    SendGossipMenuFor(player, sTransmogificationMgr->SetNpcText, creature->GetGUID());
                } break;
                case SENDER_TRANSMOG_INFO: // Transmog info
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", SENDER_SHOW_SLOTS, 0);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tExit..", SENDER_CLOSE_MENU, 0);
                    SendGossipMenuFor(player, sTransmogificationMgr->TransmogNpcText, creature);
                } break;
                case SENDER_TRANSMOG_SAVE:
                {
                    TransmogResult const res = sTransmogificationMgr->TransmogrifyPending(player, static_cast<int32>(boxMoney));
                    if (res == TransmogResult::OK)
                        session->SendAreaTriggerMessage("%s", CanTransmogrifyResultMessage(res).data());
                    else
                        session->SendNotification("%s", CanTransmogrifyResultMessage(res).data());
                    sGossipHello(player);
                } break;
                default: // from 0 to EQUIPMENT_SLOT_END-1 TrySetPendingTransmog
                {
                    if (isbetween(sender, SENDER_PAGINATED_EQUIPMENT_START, SENDER_PAGINATED_EQUIPMENT_END))
                    {
                        uint32 slot, page;
                        GetTransmogSlotAndPage(sender, SENDER_PAGINATED_EQUIPMENT_START, slot, page);
                        if (action)
                        {
                            if (TrySetPendingTransmog(player, slot, action))
                                if (!player->PendingTransmogCheck)
                                    new PendingItemsCheck(player, creature);
                        }
                        ShowTransmogs(player, creature, SENDER_PAGINATED_EQUIPMENT_START, TransmogType::Item, slot, page);
                    }
                    if (isbetween(sender, SENDER_PAGINATED_ENCHANT_START, SENDER_PAGINATED_ENCHANT_END))
                    {
                        uint32 slotindex, page;
                        GetTransmogSlotAndPage(sender, SENDER_PAGINATED_ENCHANT_START, slotindex, page);
                        if (action && slotindex < ENCHANT_SLOTS.size())
                        {
                            if (TrySetPendingEnchant(player, ENCHANT_SLOTS[slotindex], action))
                                if (!player->PendingTransmogCheck)
                                    new PendingItemsCheck(player, creature);
                        }
                        ShowTransmogs(player, creature, SENDER_PAGINATED_ENCHANT_START, TransmogType::Enchant, slotindex, page);
                    }
                    if (isbetween(sender, SENDER_PRESET_PAGE_START, SENDER_PRESET_PAGE_END))
                    {
                        uint32 const page = sender - SENDER_PRESET_PAGE_START;
                        ShowPresets(player, creature, page);
                    }
                    if (isbetween(sender, SENDER_PRESET_PURCHASED_PAGE_START, SENDER_PRESET_PURCHASED_PAGE_END))
                    {
                        uint32 const page = sender - SENDER_PRESET_PURCHASED_PAGE_START;
                        ShowPurchasedPresets(player, creature, page);
                    }
                } break;
                case SENDER_CLOSE_MENU:
                    CloseGossipMenuFor(player);
                    break;
            }
            return true;
        }

        bool GossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
        {
            ClearGossipMenuFor(player);

            switch (sender)
            {
                case SENDER_CODE_PRESET_SAVE:
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }

                    // Make sure name doesnt mess up SQL!
                    std::string const name = code;

                    // length check
                    if (name.length() < 1 || name.length() > 255)
                    {
                        player->GetSession()->SendNotification("%s", CanTransmogrifyResultMessage(TransmogResult::TooLongSetName).data());
                        OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                        return true;
                    }

                    SetTransmogs items;
                    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                    {
                        uint32 displayeditem = player->GetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2));
                        uint32 displayedenchant = player->GetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
                        if (sTransmogificationMgr->HasVisual(player, TransmogType::Item, displayeditem))
                        {
                            SetTransmog setTransmog{};
                            setTransmog.slot = slot;
                            setTransmog.transmog = displayeditem;
                            setTransmog.type = TransmogType::Item;
                            items.push_back(setTransmog);
                        }

                        if (sTransmogificationMgr->HasVisual(player, TransmogType::Enchant, displayedenchant))
                        {
                            SetTransmog setTransmog{};
                            setTransmog.slot = slot;
                            setTransmog.transmog = displayedenchant;
                            setTransmog.type = TransmogType::Enchant;
                            items.push_back(setTransmog);
                        }
                    }

                    if (items.empty())
                    {
                        // no displays to save
                        player->GetSession()->SendNotification("%s", CanTransmogrifyResultMessage(TransmogResult::NoTransmogrifications).data());
                        OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                        return true;
                    }

                    PresetData preset;
                    preset.Name = name;
                    preset.Transmogs = items;
                    player->TransmogPresets.emplace_back(std::move(preset));

                    OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                    return true;
                }
                case SENDER_CODE_PRESET_RENAME:
                {
                    if (!sTransmogificationMgr->EnableSets)
                    {
                        sGossipHello(player);
                        return true;
                    }

                    // Make sure name doesnt mess up SQL!
                    std::string const name = code;

                    // length check
                    if (name.length() < 1 || name.length() > 255)
                    {
                        player->GetSession()->SendNotification("%s", CanTransmogrifyResultMessage(TransmogResult::TooLongSetName).data());
                        OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                        return true;
                    }

                    if (action >= player->TransmogPresets.size())
                    {
                        // nonexistant setid, cheating?
                        player->GetSession()->SendNotification("%s", CanTransmogrifyResultMessage(TransmogResult::NonexistantSet).data());
                        OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                        return true;
                    }

                    player->TransmogPresets[action].Name = name;

                    OnGossipSelect(player, creature, SENDER_PRESET_LIST, 0);
                    return true;
                }
                default:
                    break;
            }

            CloseGossipMenuFor(player);
            return true;
        }

        static void BuildAndSendGossipMenu(Player* player, Creature* creature, uint32 slot, uint32 base, uint32 page, std::vector<TransmogAction>const& actions, TransmogType transmogtype)
        {
            PageData const data(page, static_cast<uint32>(actions.size()));
            if (data.count == 0)
            {
                if (!data.ismainpage)
                {
                    BuildAndSendGossipMenu(player, creature, slot, base, page - 1, actions, transmogtype);
                    return;
                }
            }

            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, (std::string("|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|t") + sTransmogificationMgr->GetSlotName(static_cast<uint8>(slot)) + " - Page " + std::to_string(data.current_page) + "/" + std::to_string(data.total_pages)).c_str(), base + page, 0);
            if (data.next)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_ChargePositive:30:30:-18:0|tNext page", base + page + 1, 0);
            if (data.prev)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_ChargeNegative:30:30:-18:0|tPrevious page", base + page - 1, 0);
            if (data.ismainpage)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tShow main menu", SENDER_SHOW_MAIN_MENU, 0);
            for (uint32 i = data.start; i < data.end; ++i)
            {
                auto& value = actions.at(i);
                uint32 const action = value.AppearanceID;
                std::string const& name = value.ItemLink;
                switch (transmogtype)
                {
                    case TransmogType::Item:
                        AddItemTransmog(player, creature, base, page, action, name, value.IsPurchased);
                        break;
                    case TransmogType::Enchant:
                        AddItemEnchant(player, creature, base, page, action, name);
                        break;
                    case TransmogType::Max:
                        break;
                }
            }
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }

        void ShowTransmogs(Player* player, Creature* creature, TransmogGossipSender basesender, TransmogType transmogtype, uint32 slot_or_slotindex, uint32 page)
        {
            uint32 const sender = basesender + ((slot_or_slotindex)*TransmogifierConstants::MaxPages);
            uint32 const slot = transmogtype == TransmogType::Item ? slot_or_slotindex : ENCHANT_SLOTS[slot_or_slotindex];
            ItemRef const item = sTransmogificationMgr->GetEquippedItem(player, static_cast<uint8>(slot));
            if (!item)
            {
                player->GetSession()->SendNotification("No item equipped");
                sGossipHello(player);
                return;
            }
            const ItemTemplate* target = item->GetTemplate();
            if (TransmogResult const transmogResult = sTransmogificationMgr->CannotTransmogrify(target); transmogResult != TransmogResult::OK)
            {
                player->GetSession()->SendNotification("%s", CanTransmogrifyResultMessage(transmogResult).data());
                sGossipHello(player);
                return;
            }
            if (TransmogResult const transmogResult = sTransmogificationMgr->CannotEquip(player, target); transmogResult != TransmogResult::OK)
            {
                player->GetSession()->SendNotification("%s", CanTransmogrifyResultMessage(transmogResult).data());
                sGossipHello(player);
                return;
            }
            std::vector<TransmogAction> actions;
            auto const SortActions = [](TransmogAction const& lhs, TransmogAction const& rhs) -> bool
            {
                uint32 const lhsSortPriority = lhs.GetSortPriority();
                uint32 const rhsSortPriority = rhs.GetSortPriority();
                if (lhsSortPriority == rhsSortPriority)
                    return lhs.ItemName < rhs.ItemName;

                return lhsSortPriority > rhsSortPriority;
            };

            switch (transmogtype)
            {
                case TransmogType::Item:
                {
                    for (auto const&[appearance, purchased] : player->TransmogAppearances[to_underlying(transmogtype)])
                    {
                        ItemTemplate const* source = sObjectMgr->GetItemTemplate(appearance);
                        if (!source)
                            continue;

                        if (TransmogResult const transmogResult = sTransmogificationMgr->CannotTransmogrifyItemWithItem(player, target, source); transmogResult == TransmogResult::OK)
                        {
                            TransmogAction action;
                            action.AppearanceID = appearance;
                            action.ItemLink = sTransmogificationMgr->GetItemLink(appearance, player->GetSession());
                            action.ItemName = sTransmogificationMgr->GetItemName(source, player->GetSession());
                            action.ItemRarity = source->Quality;
                            action.IsPurchased = purchased;
                            actions.push_back(action);
                        }
                    }
                }
                break;
                case TransmogType::Enchant:
                {
                    std::set<uint32> added_enchant_visuals;
                    static const auto try_add_enchant = [](std::set<uint32>& added_enchant_visuals, std::vector<TransmogAction>& actions, Player* player, const ItemTemplate* target, uint32 enchant)
                    {
                        if (TransmogResult const transmogResult = sTransmogificationMgr->CannotTransmogrifyItemWithEnchant(player, target, enchant); transmogResult == TransmogResult::OK)
                        {
                            if (SpellItemEnchantmentEntry const* source = sSpellItemEnchantmentStore.LookupEntry(enchant))
                            {
                                auto const it = sTransmogificationMgr->EnchantVisualToName.find(source->aura_id);
                                if (it != sTransmogificationMgr->EnchantVisualToName.end())
                                {
                                    auto const result = added_enchant_visuals.insert(source->aura_id);
                                    if (result.second)
                                    {
                                        TransmogAction action;
                                        action.AppearanceID = enchant;
                                        action.ItemLink = it->second;
                                        action.ItemName = it->second;
                                        actions.push_back(action);
                                    }
                                }
                            }
                        }
                    };

                    for (decltype(auto) enchant_spells : sTransmogificationMgr->enchant_to_spells)
                        try_add_enchant(added_enchant_visuals, actions, player, target, enchant_spells.first);

                    for (std::pair<uint32, bool> pair : player->TransmogAppearances[to_underlying(transmogtype)])
                        try_add_enchant(added_enchant_visuals, actions, player, target, pair.first);
                }
                break;
                case TransmogType::Max:
                    sGossipHello(player);
                    return;
            }

            std::ranges::sort(actions, SortActions);

            TransmogAction removePendingAction;
            removePendingAction.AppearanceID = TransmogConstants::RemovePending;
            actions.emplace(actions.begin(), removePendingAction);

            if (!(slot == SLOT_MAIN_HAND || slot == SLOT_OFF_HAND || slot == SLOT_RANGED))
            {
                TransmogAction invisibleEntryAction;
                invisibleEntryAction.AppearanceID = TransmogConstants::InvisibleEntry;
                actions.emplace(actions.begin(), invisibleEntryAction);
            }

            TransmogAction normalEntryAction;
            normalEntryAction.AppearanceID = TransmogConstants::NormalEntry;
            actions.emplace(actions.begin(), normalEntryAction);

            BuildAndSendGossipMenu(player, creature, slot, sender, page, actions, transmogtype);
        }

        static void GetTransmogSlotAndPage(uint32 sender, TransmogGossipSender basesender, uint32 & slot_or_slotindex, uint32 & page)
        {
            slot_or_slotindex = (sender - basesender) / TransmogifierConstants::MaxPages;
            page = sender - basesender - (slot_or_slotindex*TransmogifierConstants::MaxPages);
        }

        static bool TrySetPendingTransmog(Player* player, uint32 slot, uint32 entry)
        {
            WorldSession* session = player->GetSession();
            TransmogResult const res = sTransmogificationMgr->TrySetPendingTransmog(player, slot, entry);
            if (res == TransmogResult::OK)
            {
                session->SendAreaTriggerMessage("%s", TransmogConstants::OkPendingMessage.data());
                return true;
            }

            session->SendNotification("%s", CanTransmogrifyResultMessage(res).data());
            return false;
        }

        static bool TrySetPendingEnchant(Player* player, uint32 slot, uint32 entry)
        {
            WorldSession* session = player->GetSession();
            TransmogResult const res = sTransmogificationMgr->TrySetPendingEnchant(player, slot, entry);
            if (res == TransmogResult::OK)
            {
                session->SendAreaTriggerMessage("%s", TransmogConstants::OkPendingMessage.data());
                return true;
            }
            session->SendNotification("%s", CanTransmogrifyResultMessage(res).data());
            return false;
        }

        void ShowPresets(Player* player, Creature const* creature, uint32 page)
        {
            if (!sTransmogificationMgr->EnableSets)
            {
                sGossipHello(player);
                return;
            }

            PageData const data(page, static_cast<uint32>(player->TransmogPresets.size()));
            ClearGossipMenuFor(player);
            if (sTransmogificationMgr->EnableSetInfo)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Book_11:30:30:-18:0|tHow sets work", SENDER_PRESET_INFO, 0);

            if (data.next)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_ChargePositive:30:30:-18:0|tNext page", SENDER_PRESET_PAGE_START + page + 1, 0);
            if (data.prev)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_ChargeNegative:30:30:-18:0|tPrevious page", SENDER_PRESET_PAGE_START + page - 1, 0);
            if (data.ismainpage)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tShow main menu", SENDER_SHOW_MAIN_MENU, 0);
            for (uint32 i = data.start; i < data.end; ++i)
            {
                auto const& presetData = player->TransmogPresets[i];
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, ("|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|t" + presetData.Name).c_str(), SENDER_PRESET_VIEW, i);
            }

            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/GuildBankFrame/UI-GuildBankFrame-NewTab:30:30:-18:0|tSave set ", SENDER_CODE_PRESET_SAVE, 0, "Insert set name and click accept", 0, true);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }

        void ShowPurchasedPresets(Player* player, Creature const* creature, uint32 page)
        {
            if (!sTransmogificationMgr->EnableSets)
            {
                sGossipHello(player);
                return;
            }

            PageData const data(page, static_cast<uint32>(player->PurchasedTransmogPresets.size()));
            ClearGossipMenuFor(player);
            if (sTransmogificationMgr->EnableSetInfo)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Book_11:30:30:-18:0|tHow sets work", SENDER_PRESET_PURCHASED_INFO, 0);

            if (data.next)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_ChargePositive:30:30:-18:0|tNext page", SENDER_PRESET_PURCHASED_PAGE_START + page + 1, 0);
            if (data.prev)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Spell_ChargeNegative:30:30:-18:0|tPrevious page", SENDER_PRESET_PURCHASED_PAGE_START + page - 1, 0);
            if (data.ismainpage)
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tShow main menu", SENDER_SHOW_MAIN_MENU, 0);
            for (uint32 i = data.start; i < data.end; ++i)
            {
                auto const& presetData = player->PurchasedTransmogPresets[i];
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, ("|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|t" + presetData.Name).c_str(), SENDER_PRESET_PURCHASED_VIEW, i);
            }

            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new TransmogAI(creature);
    }
};

void AddSC_Transmogrification()
{
    new CS_Transmogrification();
}
