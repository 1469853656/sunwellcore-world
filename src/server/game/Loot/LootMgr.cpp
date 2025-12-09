

#include "LootMgr.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Util.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "Group.h"
#include "Player.h"
#include "Containers.h"

static Rates const qualityToRate[MAX_ITEM_QUALITY] = {
    RATE_DROP_ITEM_POOR,      // ITEM_QUALITY_POOR
    RATE_DROP_ITEM_NORMAL,    // ITEM_QUALITY_NORMAL
    RATE_DROP_ITEM_UNCOMMON,  // ITEM_QUALITY_UNCOMMON
    RATE_DROP_ITEM_RARE,      // ITEM_QUALITY_RARE
    RATE_DROP_ITEM_EPIC,      // ITEM_QUALITY_EPIC
    RATE_DROP_ITEM_LEGENDARY, // ITEM_QUALITY_LEGENDARY
    RATE_DROP_ITEM_ARTIFACT,  // ITEM_QUALITY_ARTIFACT
};

LootStore LootTemplates_Creature("creature_loot_template", "creature entry", true);
LootStore LootTemplates_Disenchant("disenchant_loot_template", "item disenchant id", true);
LootStore LootTemplates_Fishing("fishing_loot_template", "area id", true);
LootStore LootTemplates_Gameobject("gameobject_loot_template", "gameobject entry", true);
LootStore LootTemplates_Item("item_loot_template", "item entry", true);
LootStore LootTemplates_Mail("mail_loot_template", "mail template id", false);
LootStore LootTemplates_Milling("milling_loot_template", "item entry (herb)", true);
LootStore LootTemplates_Pickpocketing("pickpocketing_loot_template", "creature pickpocket lootid", true);
LootStore LootTemplates_Prospecting("prospecting_loot_template", "item entry (ore)", true);
LootStore LootTemplates_Reference("reference_loot_template", "reference id", false);
LootStore LootTemplates_Skinning("skinning_loot_template", "creature skinning id", true);
LootStore LootTemplates_Spell("spell_loot_template", "spell id (random item creating)", false);

// Selects invalid loot items to be removed from group possible entries (before rolling)
struct LootGroupInvalidSelector
{
    explicit LootGroupInvalidSelector(Loot const& loot, uint16 lootMode)
        : _loot(loot)
        , _lootMode(lootMode)
    {
    }

    bool operator()(LootStoreItem* item) const
    {
        if (!(item->lootmode & _lootMode))
            return true;

        ItemTemplate const* _proto = sObjectMgr->GetItemTemplate(item->itemid);
        if (!_proto)
            return true;

        uint8 foundDuplicates = 0;
        for (std::vector<LootItem>::const_iterator itr = _loot.items.begin(); itr != _loot.items.end(); ++itr)
            if (itr->itemid == item->itemid)
            {
                ++foundDuplicates;
                if (_proto->InventoryType == 0 && foundDuplicates == 3 && _proto->ItemId != 47242 /*Trophy of the Crusade*/) // Non-equippable items are limited to 3 drops
                    return true;
                else if (_proto->InventoryType != 0 && foundDuplicates == 1) // Equippable item are limited to 1 drop
                    return true;
            }

        return false;
    }

private:
    Loot const& _loot;
    uint16 _lootMode;
};

class LootTemplate::LootGroup // A set of loot definitions for items (refs are not allowed)
{
public:
    LootGroup() { }
    ~LootGroup();

    void AddEntry(LootStoreItem* item); // Adds an entry to the group (at loading stage)
    bool HasQuestDrop() const;          // True if group includes at least 1 quest drop entry
    bool HasQuestDropForPlayer(Player const* player) const;
    // The same for active quests of the player
    void Process(Loot& loot, uint16 lootMode, std::optional<AdditionalLootSettings> settings = std::nullopt) const; // Rolls an item from the group (if any) and adds the item to the loot
    float RawTotalChance() const;                    // Overall chance for the group (without equal chanced items)
    float TotalChance() const;                       // Overall chance for the group

    void Verify(LootStore const& lootstore, uint32 id, uint8 group_id) const;
    void CollectLootIds(LootIdSet& set) const;
    void CheckLootRefs(LootTemplateMap const& store, LootIdSet* ref_set) const;
    LootStoreItemList* GetExplicitlyChancedItemList() { return &ExplicitlyChanced; }
    LootStoreItemList* GetEqualChancedItemList() { return &EqualChanced; }
    void CopyConditions(ConditionList conditions);

private:
    LootStoreItemList ExplicitlyChanced; // Entries with chances defined in DB
    LootStoreItemList EqualChanced;      // Zero chances - every entry takes the same chance

    LootStoreItem const* Roll(Loot& loot, uint16 lootMode, std::optional<AdditionalLootSettings> settings = std::nullopt) const; // Rolls an item from the group, returns NULL if all miss their chances

    // This class must never be copied - storing pointers
    LootGroup(LootGroup const&);
    LootGroup& operator=(LootGroup const&);
};

//Remove all data and free all memory
void LootStore::Clear()
{
    for (LootTemplateMap::const_iterator itr = m_LootTemplates.begin(); itr != m_LootTemplates.end(); ++itr)
        delete itr->second;
    m_LootTemplates.clear();
}

// Checks validity of the loot store
// Actual checks are done within LootTemplate::Verify() which is called for every template
void LootStore::Verify() const
{
    for (LootTemplateMap::const_iterator i = m_LootTemplates.begin(); i != m_LootTemplates.end(); ++i)
        i->second->Verify(*this, i->first);
}

// Loads a *_loot_template DB table into loot store
// All checks of the loaded template are called from here, no error reports at loot generation required
uint32 LootStore::LoadLootTable()
{
    LootTemplateMap::const_iterator tab;

    // Clearing store (for reloading case)
    Clear();

    //                                                  0     1            2               3         4         5             6
    QueryResult result = WorldDatabase.PQuery("SELECT entry, item, ChanceOrQuestChance, lootmode, groupid, mincountOrRef, maxcount FROM %s", GetName());

    if (!result)
        return 0;

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry              = fields[0].GetUInt32();
        uint32 item               = fields[1].GetUInt32();
        float chanceOrQuestChance = fields[2].GetFloat();
        uint16 lootmode           = fields[3].GetUInt16();
        uint8 group               = fields[4].GetUInt8();
        int32 mincountOrRef       = fields[5].GetInt32();
        int32 maxcount            = fields[6].GetUInt8();

        if (maxcount > std::numeric_limits<uint8>::max())
        {
            sLog->outErrorDb("Table '%s' entry %d item %d: maxcount value (%u) to large. must be less %u - skipped", GetName(), entry, item, maxcount, std::numeric_limits<uint8>::max());
            continue; // error already printed to log/console.
        }

        if (lootmode == 0)
        {
            sLog->outError("Table '%s' entry %d item %d: lootmode is equal to 0, item will never drop - setting mode 1", GetName(), entry, item);
            lootmode = 1;
        }

        LootStoreItem* storeitem = new LootStoreItem(item, chanceOrQuestChance, lootmode, group, mincountOrRef, maxcount);

        if (!storeitem->IsValid(*this, entry)) // Validity checks
        {
            delete storeitem;
            continue;
        }

        // Looking for the template of the entry
        // often entries are put together
        if (m_LootTemplates.empty() || tab->first != entry)
        {
            // Searching the template (in case template Id changed)
            tab = m_LootTemplates.find(entry);
            if (tab == m_LootTemplates.end())
            {
                std::pair<LootTemplateMap::iterator, bool> pr = m_LootTemplates.insert(LootTemplateMap::value_type(entry, new LootTemplate()));
                tab                                           = pr.first;
            }
        }
        // else is empty - template Id and iter are the same
        // finally iter refers to already existed or just created <entry, LootTemplate>

        // Adds current row to the template
        tab->second->AddEntry(storeitem);
        ++count;
    } while (result->NextRow());

    Verify(); // Checks validity of the loot store

    return count;
}

bool LootStore::HaveQuestLootFor(uint32 loot_id) const
{
    LootTemplateMap::const_iterator itr = m_LootTemplates.find(loot_id);
    if (itr == m_LootTemplates.end())
        return false;

    // scan loot for quest items
    return itr->second->HasQuestDrop(m_LootTemplates);
}

bool LootStore::HaveQuestLootForPlayer(uint32 loot_id, Player const* player) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);
    if (tab != m_LootTemplates.end())
        if (tab->second->HasQuestDropForPlayer(m_LootTemplates, player))
            return true;

    return false;
}

void LootStore::ResetConditions()
{
    for (LootTemplateMap::iterator itr = m_LootTemplates.begin(); itr != m_LootTemplates.end(); ++itr)
    {
        ConditionList empty;
        itr->second->CopyConditions(empty);
    }
}

LootTemplate const* LootStore::GetLootFor(uint32 loot_id) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);

    if (tab == m_LootTemplates.end())
        return nullptr;

    return tab->second;
}

LootTemplate* LootStore::GetLootForConditionFill(uint32 loot_id) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);

    if (tab == m_LootTemplates.end())
        return nullptr;

    return tab->second;
}

uint32 LootStore::LoadAndCollectLootIds(LootIdSet& lootIdSet)
{
    uint32 count = LoadLootTable();

    for (LootTemplateMap::const_iterator tab = m_LootTemplates.begin(); tab != m_LootTemplates.end(); ++tab)
        lootIdSet.insert(tab->first);

    return count;
}

void LootStore::CheckLootRefs(LootIdSet* ref_set) const
{
    for (LootTemplateMap::const_iterator ltItr = m_LootTemplates.begin(); ltItr != m_LootTemplates.end(); ++ltItr)
        ltItr->second->CheckLootRefs(m_LootTemplates, ref_set);
}

void LootStore::ReportUnusedIds(LootIdSet const& lootIdSet) const
{
    // all still listed ids isn't referenced
    for (LootIdSet::const_iterator itr = lootIdSet.begin(); itr != lootIdSet.end(); ++itr)
        sLog->outErrorDb("Table '%s' entry %d isn't %s and not referenced from loot, and then useless.", GetName(), *itr, GetEntryName());
}

void LootStore::ReportNotExistedId(uint32 id) const
{
    sLog->outErrorDb("Table '%s' entry %d (%s) does not exist but used as loot id in DB.", GetName(), id, GetEntryName());
}

//
// --------- LootStoreItem ---------
//

// Checks if the entry (quest, non-quest, reference) takes it's chance (at loot generation)
// RATE_DROP_ITEMS is no longer used for all types of entries
bool LootStoreItem::Roll(bool rate, std::optional<AdditionalLootSettings> settings) const
{
    if (chance >= 100.0f)
        return true;

    if (mincountOrRef < 0) // reference case
        return roll_chance_f(chance * (rate ? sWorld->getRate(RATE_DROP_ITEM_REFERENCED) : 1.0f));

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemid);

    float qualityModifier = 1.0f;

    if (pProto && rate)
    {
        //! Riztazz: only apply loot rates for loot below 90% chance
        //! everything else is considered high chance and shouldnt be modified (time-lost protodrake loot et cetera)
        if (chance < 90.0f)
            qualityModifier = sWorld->getRate(qualityToRate[pProto->Quality]);
    }

    float finalChance = chance * qualityModifier;
    bool const applyBonusChance = [settings, pProto, finalChance]()
    {
        if (!settings)
            return false;

        if (settings->IsInInstanceOrNorthrend)
            return false;

        return true;
    }();

    if (settings && !settings->IsInInstanceOrNorthrend)
    {
        finalChance *= 2.0f;
        if (sObjectMgr->IsQuestItem(pProto))
            if (auto const questItemDropChance = sCustomMgr->GetQuestItemDropChance())
                finalChance = questItemDropChance.value();
    }

    return roll_chance_f(std::min(finalChance, 100.0f));
}

// Checks correctness of values
bool LootStoreItem::IsValid(LootStore const& store, uint32 entry) const
{
    if (group >= 1 << 7) // it stored in 7 bit field
    {
        sLog->outErrorDb("Table '%s' entry %d item %d: group (%u) must be less %u - skipped", store.GetName(), entry, itemid, group, 1 << 7);
        return false;
    }

    if (mincountOrRef == 0)
    {
        sLog->outErrorDb("Table '%s' entry %d item %d: wrong mincountOrRef (%d) - skipped", store.GetName(), entry, itemid, mincountOrRef);
        return false;
    }

    if (mincountOrRef > 0) // item (quest or non-quest) entry, maybe grouped
    {
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid);
        if (!proto)
        {
            sLog->outErrorDb("Table '%s' entry %d item %d: item entry not listed in `item_template` - skipped", store.GetName(), entry, itemid);
            return false;
        }

        if (chance == 0 && group == 0) // Zero chance is allowed for grouped entries only
        {
            sLog->outErrorDb("Table '%s' entry %d item %d: equal-chanced grouped entry, but group not defined - skipped", store.GetName(), entry, itemid);
            return false;
        }

        if (chance != 0 && chance < 0.000001f) // loot with low chance
        {
            sLog->outErrorDb("Table '%s' entry %d item %d: low chance (%f) - skipped",
                store.GetName(), entry, itemid, chance);
            return false;
        }

        if (maxcount < mincountOrRef) // wrong max count
        {
            sLog->outErrorDb("Table '%s' entry %d item %d: max count (%u) less that min count (%i) - skipped", store.GetName(), entry, itemid, int32(maxcount), mincountOrRef);
            return false;
        }
    }
    else // mincountOrRef < 0
    {
        if (needs_quest)
            sLog->outErrorDb("Table '%s' entry %d item %d: quest chance will be treated as non-quest chance", store.GetName(), entry, itemid);
        else if (chance == 0) // no chance for the reference
        {
            sLog->outErrorDb("Table '%s' entry %d item %d: zero chance is specified for a reference, skipped", store.GetName(), entry, itemid);
            return false;
        }
    }
    return true; // Referenced template existence is checked at whole store level
}

//
// --------- LootItem ---------
//

LootItem::LootItem(uint32 id)
{
    itemid = id;
    count  = 1;
    conditions.clear();

    freeforall  = false;
    needs_quest = false;

    randomSuffix     = GenerateEnchSuffixFactor(itemid);
    randomPropertyId = Item::GenerateItemRandomPropertyId(itemid);

    is_looted         = 0;
    is_blocked        = 0;
    is_underthreshold = 0;
    is_counted        = 0;
    rollWinnerGUID    = 0;
}

// Constructor, copies most fields from LootStoreItem and generates random count
LootItem::LootItem(LootStoreItem const& li)
{
    itemid     = li.itemid;
    conditions = li.conditions;

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid);
    freeforall                = proto && (proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT);
    follow_loot_rules         = proto && (proto->FlagsCu & ITEM_FLAGS_CU_FOLLOW_LOOT_RULES);

    needs_quest = li.needs_quest;

    randomSuffix     = GenerateEnchSuffixFactor(itemid);
    randomPropertyId = Item::GenerateItemRandomPropertyId(itemid);

    count             = 0;
    is_looted         = 0;
    is_blocked        = 0;
    is_underthreshold = 0;
    is_counted        = 0;
    rollWinnerGUID    = 0;
}

// Basic checks for player/item compatibility - if false no chance to see the item in the loot
bool LootItem::AllowedForPlayer(Player const* player) const
{
    // DB conditions check
    if (!sConditionMgr->IsObjectMeetToConditions(const_cast<Player*>(player), conditions))
        return false;

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemid);
    if (!pProto)
        return false;

    // not show loot for players without profession or those who already know the recipe
    if ((pProto->Flags & ITEM_PROTO_FLAG_SMART_LOOT) && (!player->HasSkill(pProto->RequiredSkill) || player->HasSpell(pProto->Spells[1].SpellId)))
        return false;

    if (!sWorld->IsCrossfactionEnabled(Crossfaction::Item))
    {
        // not show loot for not own team
        if ((pProto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY) && player->GetTeam(CF_DENY) != TEAM_HORDE)
            return false;

        if ((pProto->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY) && player->GetTeam(CF_DENY) != TEAM_ALLIANCE)
            return false;
    }

    // check quest requirements
    if (!(pProto->FlagsCu & ITEM_FLAGS_CU_IGNORE_QUEST_STATUS) && ((needs_quest || (pProto->StartQuest && player->GetQuestStatus(pProto->StartQuest) != QUEST_STATUS_NONE)) && !player->HasQuestForItem(itemid)))
        return false;

    return true;
}

void LootItem::AddAllowedLooter(const Player* player)
{
    allowedGUIDs.insert(player->GetGUIDLow());
}

//
// --------- Loot ---------
//

// Inserts the item into the loot (called by LootTemplate processors)
// Inserts the item into the loot (called by LootTemplate processors)
void Loot::AddItem(LootStoreItem const& item)
{
    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item.itemid);
    if (!proto)
        return;

    uint32 count  = urand(item.mincountOrRef, item.maxcount);
    uint32 stacks = count / proto->GetMaxStackSize() + (count % proto->GetMaxStackSize() ? 1 : 0);

    std::vector<LootItem>& lootItems = item.needs_quest ? quest_items : items;
    uint32 limit                     = item.needs_quest ? MAX_NR_QUEST_ITEMS : MAX_NR_LOOT_ITEMS;

    for (uint32 i = 0; i < stacks && lootItems.size() < limit; ++i)
    {
        LootItem generatedLoot(item);
        generatedLoot.count = std::min(count, proto->GetMaxStackSize());

        Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(LootOwnerGUID);
        if (player)
            sCustomMgr->ApplySharedQuestLoot(generatedLoot, player);

        lootItems.push_back(generatedLoot);
        count -= proto->GetMaxStackSize();

        // In some cases, a dropped item should be visible/lootable only for some players in group
        bool canSeeItemInLootWindow = false;
        if (player)
        {
            if (Group* group = player->GetGroup())
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                    if (Player* member = itr->GetSource())
                        if (generatedLoot.AllowedForPlayer(member))
                            canSeeItemInLootWindow = true;
            }
            else if (generatedLoot.AllowedForPlayer(player))
                canSeeItemInLootWindow = true;
        }
        if (!canSeeItemInLootWindow)
            continue;

        // non-conditional one-player only items are counted here,
        // free for all items are counted in FillFFALoot(),
        // non-ffa conditionals are counted in FillNonQuestNonFFAConditionalLoot()
        if (!generatedLoot.needs_quest && generatedLoot.conditions.empty() && !generatedLoot.freeforall)
            ++unlootedCount;
    }
}

static bool IsBadgeOrEmblem(uint32 itemId)
{
    switch (itemId)
    {
        case 29434: // Badge of Justice
        case 40752: // Emblem of Heroism
        case 45624: // Emblem of Conquest
        case 40753: // Emblem of Valor
        case 47241: // Emblem of Triumph
        case 49426: // Emblem of Frost
        case 43228: // Stone Keeper's Shard
            return true;
        default:
            return false;
    }
}

bool Loot::IsPlayerAllowedToLoot(Player* player, WorldObject* object)
{
    return players_allowed_to_loot.empty() ? true : players_allowed_to_loot.find(player->GetGUID()) != players_allowed_to_loot.end();
}

void Loot::SetLootedGUID(WorldObject* object)
{
    if (!sWorld->getBoolConfig(CONFIG_SAVE_LOOT_SYSTEM))
        return;

    if (!object || !object->GetMap()->IsRaidOrHeroicDungeon())
        return;

    Creature* creature = object->ToCreature();
    GameObject* go = object->ToGameObject();

    // We care only about binds loot.
    if (creature && !creature->IsInstanceBind())
        return;

    // We care only about common chests (exclude herbs, mines, etc)
    if (go && (go->GetGoType() != GAMEOBJECT_TYPE_CHEST || go->GetGOInfo()->chest.minSuccessOpens))
        return;

    object->FillPlayersAllowedToLoot(&players_allowed_to_loot);

    m_lootedGUID = object->GetGUID();
    m_mapID = MapID(object->GetMapId(), object->GetInstanceId());
}

void Loot::FillLootFromDB(WorldObject* object)
{
    if (!sWorld->getBoolConfig(CONFIG_SAVE_LOOT_SYSTEM))
        return;

    clear();

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_LOOT_SYSTEM_LOOT);
    stmt->setUInt32(0, object->GetEntry());
    stmt->setUInt32(1, object->GetInstanceId());
    stmt->setBool(2, object->GetTypeId() == TYPEID_UNIT);

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        m_lootedGUID = object->GetGUID();
        m_mapID = MapID(object->GetMapId(), object->GetInstanceId());

        std::stringstream ss;
        ss << "Loaded LootedItems: ";

        do
        {
            Field* fields = result->Fetch();

            uint32 itemid = fields[0].GetUInt32();
            uint32 itemcount = fields[1].GetUInt32();
            for (uint8 i = 0; i < itemcount; ++i)
            {
                ss << "[" << itemid << "] ";

                LootItem item(itemid);
                items.push_back(item);
                unlootedCount++;
            }

            Tokenizer tokens(fields[2].GetString(), ' ');
            for (Tokenizer::const_iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
            {
                uint32 playerGuid = uint32(atol((*iter)));
                players_allowed_to_loot.insert(MAKE_NEW_GUID(playerGuid, 0, HIGHGUID_PLAYER));
            }
        } while (result->NextRow());

        if (unlootedCount == 0)
            return;

        ss << "- For Players: ";

        Map* pMap = object->GetMap();
        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const& PlayerList = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (Player* i_pl = i->GetSource())
                {
                    if (Group* group = i_pl->GetGroup())
                    {
                        Group::MemberSlotList gList = group->GetMemberSlots();
                        for (Group::member_citerator citr = gList.begin(); citr != gList.end(); ++citr)
                            ss << citr->name << " [" << citr->guid << "] ";

                        break;
                    }
                    else
                        ss << i_pl->GetName() << " [" << i_pl->GetGUID() << "] ";
                }
            }
        }
        else
            return;

        // Set variable to true even if we don't load anything so new loot won't be generated.
        m_lootLoadedFromDB = true;

        if (object->GetTypeId() == TYPEID_UNIT)
        {
            Creature* creature = object->ToCreature();

            // make body visible to loot
            creature->SetVisible(true);
            creature->setDeathState(JUST_DIED);
            creature->SetCorpseDelay(3600);

            creature->LowerPlayerDamageReq(creature->GetMaxHealth());
            creature->SetWasHitByPlayer(true);

            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            if (creature->GetCreatureTemplate() && creature->GetCreatureTemplate()->SkinLootId)
                if (LootTemplates_Skinning.HaveLootFor(creature->GetCreatureTemplate()->SkinLootId))
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

            creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (creature->CanFly())
            {
                float tz = creature->GetMap()->GetHeight(creature->GetPhaseMask(), creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), true, MAX_FALL_DISTANCE);
                if (creature->GetPositionZ() - tz > 0.1f)
                    creature->Relocate(creature->GetPositionX(), creature->GetPositionY(), tz);
            }
        }
        else if (object->GetTypeId() == TYPEID_GAMEOBJECT)
        {
            GameObject* go = object->ToGameObject();

            if (!go->isSpawnedByDefault() && go->GetRespawnDelay())
                go->SetRespawnTime(go->GetRespawnDelay());

            go->SetVisible(true);
            go->EnableCollision(true);
            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE);
            go->SetLootGenerationTime();
        }

        sLog->outLoot("%s", ss.str().c_str());
    }
}

void Loot::RemoveItemFromSavedLoot(LootItem* item)
{
    if (!sWorld->getBoolConfig(CONFIG_SAVE_LOOT_SYSTEM))
        return;

    if (!m_lootedGUID)
        return;

    // Badges and Emblems at WotLK expansion are automated pushed to character profile.
    if (IsBadgeOrEmblem(item->itemid))
        return;

    Map* pMap = sMapMgr->FindMap(m_mapID.nMapId, m_mapID.nInstanceId);
    if (!pMap || !pMap->IsDungeon())
        return;

    WorldObject* object = nullptr;

    if (IS_CRE_OR_VEH_GUID(m_lootedGUID))
    {
        object = pMap->GetCreature(m_lootedGUID);
        if (!object)
        {
            if (pMap->IsDungeon())
                sLog->outLoot("Loot::RemoveItemFromSavedLoot: Creature not found. GUID: [%u] Instance Id: [%u]", GUID_LOPART(m_lootedGUID), pMap->GetInstanceId());
            return;
        }
    }
    else if (IS_GAMEOBJECT_GUID(m_lootedGUID))
    {
        object = pMap->GetGameObject(m_lootedGUID);
        if (!object)
        {
            if (pMap->IsDungeon())
                sLog->outLoot("Loot::RemoveItemFromSavedLoot: GameObject not found. GUID: [%u] Instance Id [%u]", GUID_LOPART(m_lootedGUID), pMap->GetInstanceId());
            return;
        }
    }

    if (!object)
        return;

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_LOOT_SYSTEM_ITEM_COUNT);
    stmt->setUInt32(0, item->itemid);
    stmt->setUInt32(1, pMap->GetInstanceId());
    stmt->setUInt32(2, object->GetEntry());
    stmt->setBool(3, object->GetTypeId() == TYPEID_UNIT);

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        // it should be single line
        uint32 count  = 0;
        Field* fields = result->Fetch();
        count         = fields[0].GetUInt32();

        if (count > 1)
        {
            count--;

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LOOT_SYSTEM_ITEM_COUNT);
            stmt->setUInt32(0, count);
            stmt->setUInt32(1, pMap->GetInstanceId());
            stmt->setUInt32(2, item->itemid);
            stmt->setUInt32(3, object->GetEntry());
            stmt->setBool(4, object->GetTypeId() == TYPEID_UNIT);
            CharacterDatabase.Execute(stmt);
        }
        else
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_LOOT_SYSTEM_ITEM);
            stmt->setUInt32(0, pMap->GetInstanceId());
            stmt->setUInt32(1, item->itemid);
            stmt->setUInt32(2, object->GetEntry());
            stmt->setBool(3, object->GetTypeId() == TYPEID_UNIT);
            CharacterDatabase.Execute(stmt);
        }
    }
    else
    {
        if (pMap->IsDungeon())
            sLog->outLoot("Loot::RemoveItemFromSavedLoot: Result empty.");
    }
}

void Loot::ProcessAOELoot(Loot& otherLoot, Player* lootOwner)
{
    for (uint32 i = 0; i < otherLoot.quest_items.size(); ++i)
    {
        if (quest_items.size() < MAX_NR_QUEST_ITEMS)
        {
            LootItem& item = otherLoot.quest_items[i];
            item.isAOELoot = true;
            item.aoeIndex = i;
            quest_items.push_back(item);
        }
    }

    std::vector<LootItem> succeededAOEItems;
    for (uint32 i = 0; i < otherLoot.items.size(); ++i)
    {
        if (items.size() < MAX_NR_LOOT_ITEMS)
        {
            LootItem& item = otherLoot.items[i];
            item.isAOELoot = true;
            item.aoeIndex = i;
            items.push_back(item);

            if (item.conditions.empty())
            {
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item.itemid);
                if (!proto || (proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT) == 0)
                {
                    succeededAOEItems.push_back(item);
                    ++unlootedCount;
                }
            }
        }
    }

    gold += otherLoot.gold;
    otherLoot.gold = 0;

    Group* group = lootOwner->GetGroup();
    if (group)
    {
        bool raidGroup = group->isRaidGroup();

        for (uint8 i = 0; i < items.size(); ++i)
        {
            LootItem& item = items[i];
            if (item.isAOELoot)
            {
                if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item.itemid))
                    if (proto->Quality < uint32(group->GetLootThreshold()))
                        item.is_underthreshold = true;

                if (raidGroup)
                    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                        if (Player* player = itr->GetSource())   // should actually be looted object instead of lootOwner but looter has to be really close so doesnt really matter
                            if (player->IsAtLootRewardDistance(lootOwner))
                                item.AddAllowedLooter(player);
            }
        }

        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            if (Player* player = itr->GetSource())   // should actually be looted object instead of lootOwner but looter has to be really close so doesnt really matter
                FillNotNormalLootFor(player, player->IsAtLootRewardDistance(lootOwner), lootOwner, &succeededAOEItems);
    }
    // ... for personal loot
    else
        FillNotNormalLootFor(lootOwner, true, lootOwner, &succeededAOEItems);

    for (LootItem const& succeededItem : succeededAOEItems)
    {
        if (succeededItem.needs_quest)
        {
            bool questItemFollowGroupLootRules = (group && (group->GetLootMethod() == GROUP_LOOT || group->GetLootMethod() == ROUND_ROBIN));
            for (std::vector<LootItem>::iterator itr = otherLoot.quest_items.begin(); itr != otherLoot.quest_items.end();)
            {
                LootItem& item = *itr;
                if (succeededItem.aoeIndex == item.aoeIndex)
                {
                    item.ResetAOE();

                    bool initialCount = false;
                    for (auto& iter : otherLoot.PlayerQuestItems)
                    {
                        if (iter.second)
                        {
                            for (QuestItemList::iterator it = iter.second->begin(); it != iter.second->end();)
                            {
                                if (succeededItem.aoeIndex == it->index)
                                {
                                    it = iter.second->erase(it);
                                    if (otherLoot.unlootedCount && (!initialCount || item.freeforall || questItemFollowGroupLootRules))
                                    {
                                        --otherLoot.unlootedCount;
                                        initialCount = true;
                                    }
                                }
                                else
                                    ++it;
                            }
                        }
                    }

                    itr = otherLoot.quest_items.erase(itr);
                }

                else
                    ++itr;
            }
        }
        else
        {
            for (std::vector<LootItem>::iterator itr = otherLoot.items.begin(); itr != otherLoot.items.end();)
            {
                LootItem& item = *itr;
                if (succeededItem.aoeIndex == item.aoeIndex)
                {
                    item.ResetAOE();

                    if (itr->conditions.empty())
                    {
                        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itr->itemid);
                        if (!proto || (proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT) == 0)
                        {
                            if (otherLoot.unlootedCount)
                                --otherLoot.unlootedCount;
                        }
                        else
                        {
                            for (auto& iter : otherLoot.PlayerFFAItems)
                            {
                                if (iter.second)
                                {
                                    for (QuestItemList::iterator it = iter.second->begin(); it != iter.second->end();)
                                    {
                                        if (succeededItem.aoeIndex == it->index)
                                        {
                                            it = iter.second->erase(it);
                                            if (otherLoot.unlootedCount)
                                                --otherLoot.unlootedCount;
                                        }
                                        else
                                            ++it;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        bool initialCount = false;
                        for (auto& iter : otherLoot.PlayerNonQuestNonFFAConditionalItems)
                        {
                            if (iter.second)
                            {
                                for (QuestItemList::iterator it = iter.second->begin(); it != iter.second->end();)
                                {
                                    if (std::distance(otherLoot.items.begin(), itr) == it->index)
                                    {
                                        it = iter.second->erase(it);
                                        if (otherLoot.unlootedCount && !initialCount)
                                        {
                                            --otherLoot.unlootedCount;
                                            initialCount = true;
                                        }
                                    }
                                    else
                                        ++it;
                                }
                            }
                        }
                    }

                    itr = otherLoot.items.erase(itr);
                }
                else
                    ++itr;
            }
        }
    }
}

void Loot::RemoveSavedLootFromDB(CharacterDatabaseTransaction transaction, WorldObject* object)
{
    if (!sWorld->getBoolConfig(CONFIG_SAVE_LOOT_SYSTEM))
        return;

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_LOOT_SYSTEM_CREATURE_LOOT);
    stmt->setUInt32(0, object->GetEntry());
    stmt->setUInt32(1, object->GetInstanceId());
    stmt->setBool(2, object->GetTypeId() == TYPEID_UNIT);
    CharacterDatabase.ExecuteOrAppend(transaction, stmt);
}

void Loot::RemoveSavedLootFromDB()
{
    if (!sWorld->getBoolConfig(CONFIG_SAVE_LOOT_SYSTEM))
        return;

    if (!m_lootedGUID)
        return;

    Map* map = sMapMgr->FindMap(m_mapID.nMapId, m_mapID.nInstanceId);

    if (!map || !map->IsDungeon())
        return;

    Creature* creature = map->GetCreature(m_lootedGUID);

    if (!creature)
    {
        sLog->outLoot("Loot::RemoveSavedLootFromDB: Creature not found. Map Id [%u] Instance Id [%u]", m_mapID.nMapId, m_mapID.nInstanceId);
        return;
    }

    CharacterDatabaseTransaction transaction = CharacterDatabaseTransaction(nullptr);
    RemoveSavedLootFromDB(transaction, creature);
}

void Loot::SaveLootToDB(Player* owner)
{
    if (!sWorld->getBoolConfig(CONFIG_SAVE_LOOT_SYSTEM))
        return;

    if (!m_lootedGUID)
        return;

    Map* map = sMapMgr->FindMap(m_mapID.nMapId, m_mapID.nInstanceId);
    if (!map || !map->IsDungeon())
        return;

    WorldObject* object = nullptr;
    bool IsTempSummon   = false;

    if (IS_CRE_OR_VEH_GUID(m_lootedGUID))
    {
        object = map->GetCreature(m_lootedGUID);
        if (!object)
        {
            sLog->outLoot("Loot::SaveToDB: Creature not found. Player [%s] GUID [%u]", owner->GetName().c_str(), owner->GetGUIDLow());
            return;
        }

        if (object->ToCreature()->m_mythicScaling != nullptr)
            return;

        IsTempSummon = object->ToCreature()->ToTempSummon() != nullptr;
    }
    else if (IS_GAMEOBJECT_GUID(m_lootedGUID))
    {
        object = map->GetGameObject(m_lootedGUID);
        if (!object)
        {
            sLog->outLoot("Loot::SaveToDB: GameObject not found. Player [%s] GUID [%u]", owner->GetName().c_str(), owner->GetGUIDLow());
            return;
        }

        IsTempSummon = object->ToGameObject()->GetOwnerGUID() || object->ToGameObject()->GetSpellId();
    }

    if (!object)
        return;

    std::map<uint32, uint32> item_count;
    auto trans = CharacterDatabase.BeginTransaction();

    // delete old saved loot
    RemoveSavedLootFromDB(trans, object);

    std::stringstream ss;
    ss << owner->GetName() << "'s [" << owner->GetGUIDLow() << "] Group - Map Id: [" << m_mapID.nMapId
       << "] Instance Id: [" << m_mapID.nInstanceId << "] Generated loot: ";

    std::stringstream guids;
    for (uint64 guid : players_allowed_to_loot)
        guids << GUID_LOPART(guid) << ' ';

    for (std::vector<LootItem>::iterator iter = items.begin(); iter != items.end(); ++iter)
    {
        LootItem const* item = &(*iter);

        if (item->is_looted || !item->conditions.empty())
            continue;

        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid);

        if (!proto || proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT)
            continue;

        item_count[item->itemid] += 1;
        uint32 count = item_count[item->itemid];
        if (count > 1)
        {
            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LOOT_SYSTEM_ITEM_COUNT);
            stmt->setUInt32(0, count);
            stmt->setUInt32(1, map->GetInstanceId());
            stmt->setUInt32(2, item->itemid);
            stmt->setUInt32(3, object->GetEntry());
            stmt->setBool(4, object->GetTypeId() == TYPEID_UNIT);
            CharacterDatabase.Execute(stmt);
        }
        else
        {
            std::ostringstream positionString;
            if (IsTempSummon)
                positionString << object->GetPositionX() << " " << object->GetPositionY() << " " << object->GetPositionZ() << " " << object->GetOrientation() << " ";

            std::string position = positionString.str();

            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOOT_SYSTEM_ITEM);
            stmt->setUInt32(0, object->GetEntry());
            stmt->setBool(1, IS_CRE_OR_VEH_GUID(m_lootedGUID));
            stmt->setUInt32(2, m_mapID.nMapId);
            stmt->setUInt32(3, m_mapID.nInstanceId);
            stmt->setUInt32(4, item->itemid);
            stmt->setUInt32(5, count);

            if (guids.str().empty())
                stmt->setNull(6);
            else
                stmt->setString(6, guids.str());

            if (position.empty())
                stmt->setNull(7);
            else
                stmt->setString(7, position);

            CharacterDatabase.Execute(stmt);
        }

        ss << "[" << item->itemid << "] ";
    }

    sLog->outLoot("%s",ss.str().c_str());

    CharacterDatabase.CommitTransaction(trans);
}

// Calls processor of corresponding LootTemplate (which handles everything including references)
bool Loot::FillLoot(uint32 lootId, LootStore const& store, Player* lootOwner, bool personal, bool noEmptyError, uint16 lootMode /*= LOOT_MODE_DEFAULT*/, WorldObject* worldObject /* = nullptr */)
{
    // Must be provided
    if (!lootOwner)
        return false;

    LootOwnerGUID           = lootOwner->GetGUID();
    LootTemplate const* tab = store.GetLootFor(lootId);

    if (!tab)
    {
        if (!noEmptyError)
            sLog->outErrorDb("Table '%s' loot id #%u used but it doesn't have records.", store.GetName(), lootId);
        return false;
    }

    items.reserve(MAX_NR_LOOT_ITEMS);
    quest_items.reserve(MAX_NR_QUEST_ITEMS);

    AdditionalLootSettings settings;
    settings.IsInInstanceOrNorthrend = lootOwner->GetMap()->IsDungeon() || lootOwner->GetMap()->GetEntry()->Expansion() == EXPANSION_WRATH_OF_THE_LICH_KING;

    //! Do not apply config item rates to instance creatures
    bool ratesAllowed = store.IsRatesAllowed();
    if (Map* map = lootOwner->GetMap())
        if (map->IsDungeon())
            ratesAllowed = false;

    if (!m_lootedGUID)
        ratesAllowed = false;

    tab->Process(*this, ratesAllowed, lootMode, 0, settings); // Processing is done there, callback via Loot::AddItem()

    // Setting access rights for group loot case
    Group* group = lootOwner->GetGroup();
    if (!personal && group)
    {
        roundRobinPlayer = lootOwner->GetGUID();

        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            if (Player* player = itr->GetSource()) // should actually be looted object instead of lootOwner but looter has to be really close so doesnt really matter
            {
                WorldObject const* playerObject = player->GetCorpse();
                if (!playerObject || player->IsAlive())
                    playerObject = player;

                if (playerObject->IsInMap(lootOwner)) // pussywizard: multithreading crashfix;
                    FillNotNormalLootFor(player, player->IsAtGroupRewardDistance(lootOwner), lootOwner);
            }
        }

        std::set<uint32> allowedLooters;
        std::set<uint64> allowedLooters64;
        if (worldObject)
        {
            if (worldObject->IsCreature() && worldObject->ToCreature()->HasPlayersAllowedToLoot())
                allowedLooters64 = worldObject->ToCreature()->GetPlayersAllowedToLoot();
            else if (worldObject->IsGameObject() && worldObject->ToGameObject()->HasPlayersAllowedToLoot())
                allowedLooters64 = worldObject->ToGameObject()->GetPlayersAllowedToLoot();

            if (!allowedLooters64.empty())
            {
                for (auto guid : allowedLooters64)
                    allowedLooters.insert(GUID_LOPART(guid));
            }
            else if (Group* group = lootOwner->GetGroup())
            {
                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                {
                    Player* looter = itr->GetSource();
                    if (!looter->IsInWorld())
                        continue;

                    if (looter->IsAtGroupRewardDistance(worldObject))
                    {
                        allowedLooters.insert(looter->GetGUIDLow());
                    }
                }
            }
        }

        for (uint8 i = 0; i < items.size(); ++i)
        {
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(items[i].itemid))
                if (proto->Quality < uint32(group->GetLootThreshold()))
                    items[i].is_underthreshold = true;
            if (!allowedLooters.empty())
                items[i].SetAllowedLooters(allowedLooters);
        }
    }
    // ... for personal loot
    else
        FillNotNormalLootFor(lootOwner, true, lootOwner);

    if (m_lootedGUID)
    {
        LogLootToDB();
        SaveLootToDB(lootOwner);
    }

    return true;
}

void Loot::FillNotNormalLootFor(Player* player, bool presentAtLooting, Player const* lootOwner, std::vector<LootItem>* succeededAOEItems /* = nullptr */)
{
    uint32 plguid = player->GetGUIDLow();

    QuestItemMap::const_iterator qmapitr = PlayerQuestItems.find(plguid);
    if (qmapitr == PlayerQuestItems.end() || succeededAOEItems)
        FillQuestLoot(player, succeededAOEItems);

    qmapitr = PlayerFFAItems.find(plguid);
    if (qmapitr == PlayerFFAItems.end() || succeededAOEItems)
        FillFFALoot(player, succeededAOEItems);

    qmapitr = PlayerNonQuestNonFFAConditionalItems.find(plguid);
    if (qmapitr == PlayerNonQuestNonFFAConditionalItems.end() || succeededAOEItems)
        FillNonQuestNonFFAConditionalLoot(player, presentAtLooting, succeededAOEItems);

    // if not auto-processed player will have to come and pick it up manually
    if (!presentAtLooting || !player->IsInMap(lootOwner))
        return;

    // Process currency items
    uint32 max_slot      = GetMaxSlotInLootFor(player);
    LootItem const* item = nullptr;
    uint32 itemsSize     = uint32(items.size());

    qmapitr = PlayerQuestItems.find(plguid);
    for (uint32 i = 0; i < max_slot; ++i)
    {
        if (i < items.size())
            item = &items[i];
        else
            item = &quest_items[qmapitr->second->at(i - itemsSize).index];

        if (!item->is_looted && item->freeforall && item->AllowedForPlayer(player))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid))
                if (proto->IsCurrencyToken() || proto->IsAutoLooted())
                    player->StoreLootItem(i, this);
    }
}

void Loot::FillFFALoot(Player const* player, std::vector<LootItem>* succeededAOEItems)
{
    std::shared_ptr<QuestItemList> ql;
    if (!succeededAOEItems)
        ql = std::make_shared<QuestItemList>();
    else
    {
        auto itr = PlayerFFAItems.find(player->GetGUIDLow());
        if (itr != PlayerFFAItems.end())
            ql = itr->second;

        if (!ql)
            ql = std::make_shared<QuestItemList>();
    }
    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem& item = items[i];
        if (succeededAOEItems && !item.isAOELoot)
            continue;

        if (!item.is_looted && item.freeforall && item.AllowedForPlayer(player))
        {
            ql->push_back(QuestItem(i));
            ++unlootedCount;

            if (succeededAOEItems && item.isAOELoot)
                succeededAOEItems->push_back(item);
        }

        if (item.isAOELoot && item.conditions.empty())
            item.ResetAOE();
    }

    if (ql->empty())
        return;

    PlayerFFAItems[player->GetGUIDLow()] = std::move(ql);
}

void Loot::FillQuestLoot(Player* player, std::vector<LootItem>* succeededAOEItems)
{
    if (items.size() == MAX_NR_LOOT_ITEMS)
        return;

    std::shared_ptr<QuestItemList> ql;
    if (!succeededAOEItems)
        ql = std::make_shared<QuestItemList>();
    else
    {
        auto itr = PlayerQuestItems.find(player->GetGUIDLow());
        if (itr != PlayerQuestItems.end())
            ql = itr->second;

        if (!ql)
            ql = std::make_shared<QuestItemList>();
    }

    for (uint8 i = 0; i < quest_items.size(); ++i)
    {
        LootItem& item = quest_items[i];
        if (succeededAOEItems && !item.isAOELoot)
            continue;

        if (!item.is_looted && (item.AllowedForPlayer(player) || (item.follow_loot_rules && player->GetGroup() && ((player->GetGroup()->GetLootMethod() == MASTER_LOOT && player->GetGroup()->GetMasterLooterGuid() == player->GetGUID()) || player->GetGroup()->GetLootMethod() != MASTER_LOOT))))
        {
            ql->push_back(QuestItem(i));

            // quest items get blocked when they first appear in a
            // player's quest vector
            //
            // increase once if one looter only, looter-times if free for all
            if (item.freeforall || !item.is_blocked)
                ++unlootedCount;
            if (!player->GetGroup() || (player->GetGroup()->GetLootMethod() != GROUP_LOOT && player->GetGroup()->GetLootMethod() != ROUND_ROBIN))
                item.is_blocked = true;

            if (succeededAOEItems && item.isAOELoot)
                succeededAOEItems->push_back(item);

            item.ResetAOE();

            if (items.size() + ql->size() == MAX_NR_LOOT_ITEMS)
                break;
        }
        else
            item.ResetAOE();
    }

    if (ql->empty())
        return;

    PlayerQuestItems[player->GetGUIDLow()] = std::move(ql);
}

void Loot::FillNonQuestNonFFAConditionalLoot(Player* player, bool presentAtLooting, std::vector<LootItem>* succeededAOEItems)
{
    std::shared_ptr<QuestItemList> ql;
    if (!succeededAOEItems)
        ql = std::make_shared<QuestItemList>();
    else
    {
        auto itr = PlayerNonQuestNonFFAConditionalItems.find(player->GetGUIDLow());
        if (itr != PlayerNonQuestNonFFAConditionalItems.end())
            ql = itr->second;

        if (!ql)
            ql = std::make_shared<QuestItemList>();
    }

    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem& item = items[i];
        if (succeededAOEItems && !item.isAOELoot)
            continue;

        if (!item.is_looted && !item.freeforall && (item.AllowedForPlayer(player) || (item.follow_loot_rules && player->GetGroup() && ((player->GetGroup()->GetLootMethod() == MASTER_LOOT && player->GetGroup()->GetMasterLooterGuid() == player->GetGUID()) || player->GetGroup()->GetLootMethod() != MASTER_LOOT))))
        {
            /*if (presentAtLooting)
                item.AddAllowedLooter(player);*/
            if (!item.conditions.empty())
            {
                ql->push_back(QuestItem(i));
                if (succeededAOEItems && item.isAOELoot)
                {
                    succeededAOEItems->push_back(item);
                    ++unlootedCount;
                    item.is_counted = true;
                }
                else if (!item.is_counted)
                {
                    ++unlootedCount;
                    item.is_counted = true;
                }
            }
        }

        item.ResetAOE();
    }

    if (ql->empty())
        return;

    PlayerNonQuestNonFFAConditionalItems[player->GetGUIDLow()] = std::move(ql);
}

static bool IsItemValidForLog(LootItem* item)
{
    if (IsBadgeOrEmblem(item->itemid))
        return false;

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid);
    if (!proto || proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT)
        return false;

    if (proto->Quality < ITEM_QUALITY_EPIC)
        return false;

    return true;
}

void Loot::LogLootToDB()
{
    Map* map = sMapMgr->FindMap(m_mapID.nMapId, m_mapID.nInstanceId);
    if (map == nullptr)
        return;

    WorldObject* object = nullptr;
    if (IS_CRE_OR_VEH_GUID(m_lootedGUID))
        object = map->GetCreature(m_lootedGUID);
    else if (IS_GAMEOBJECT_GUID(m_lootedGUID))
        object = map->GetGameObject(m_lootedGUID);

    if (!object || items.empty())
        return;

    auto trans = CharacterDatabase.BeginTransaction();
    //(map_id, instance_id, owner_entry, item_entry )
    for (LootItem& item : items)
    {
        if (!IsItemValidForLog(&item))
            continue;

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOOT_LOG_LOOT_CREATED);
        stmt->setUInt32(0, m_mapID.nMapId);
        stmt->setUInt32(1, m_mapID.nInstanceId);
        stmt->setUInt32(2, object->GetEntry());
        stmt->setUInt32(3, item.itemid);
        trans->Append(stmt);
    }

    for (LootItem& qItem : quest_items)
    {
        if (!IsItemValidForLog(&qItem))
            continue;

        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOOT_LOG_LOOT_CREATED);
        stmt->setUInt32(0, m_mapID.nMapId);
        stmt->setUInt32(1, m_mapID.nInstanceId);
        stmt->setUInt32(2, object->GetEntry());
        stmt->setUInt32(3, qItem.itemid);
        trans->Append(stmt);
    }

    CharacterDatabase.CommitTransaction(trans);
}

//===================================================

void Loot::NotifyItemRemoved(uint8 lootIndex)
{
    // notify all players that are looting this that the item was removed
    // convert the index to the slot the player sees
    PlayersLootingSet::iterator i_next;
    for (PlayersLootingSet::iterator i = PlayersLooting.begin(); i != PlayersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* player = ObjectAccessor::FindPlayer(*i))
            player->SendNotifyLootItemRemoved(lootIndex);
        else
            PlayersLooting.erase(i);
    }
}

void Loot::NotifyMoneyRemoved()
{
    // notify all players that are looting this that the money was removed
    PlayersLootingSet::iterator i_next;
    for (PlayersLootingSet::iterator i = PlayersLooting.begin(); i != PlayersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* player = ObjectAccessor::FindPlayer(*i))
            player->SendNotifyLootMoneyRemoved();
        else
            PlayersLooting.erase(i);
    }
}

void Loot::NotifyQuestItemRemoved(uint8 questIndex)
{
    // when a free for all questitem is looted
    // all players will get notified of it being removed
    // (other questitems can be looted by each group member)
    // bit inefficient but isn't called often

    PlayersLootingSet::iterator i_next;
    for (PlayersLootingSet::iterator i = PlayersLooting.begin(); i != PlayersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* player = ObjectAccessor::FindPlayer(*i))
        {
            QuestItemMap::const_iterator pq = PlayerQuestItems.find(player->GetGUIDLow());
            if (pq != PlayerQuestItems.end() && pq->second)
            {
                // find where/if the player has the given item in it's vector
                QuestItemList& pql = *pq->second;

                uint8 j;
                for (j = 0; j < pql.size(); ++j)
                    if (pql[j].index == questIndex)
                        break;

                if (j < pql.size())
                    player->SendNotifyLootItemRemoved(items.size() + j);
            }
        }
        else
            PlayersLooting.erase(i);
    }
}

void Loot::generateMoneyLoot(uint32 minAmount, uint32 maxAmount)
{
    if (maxAmount > 0)
    {
        if (maxAmount <= minAmount)
            gold = uint32(maxAmount * sWorld->getRate(RATE_DROP_MONEY));
        else if ((maxAmount - minAmount) < 32700)
            gold = uint32(urand(minAmount, maxAmount) * sWorld->getRate(RATE_DROP_MONEY));
        else
            gold = uint32(urand(minAmount >> 8, maxAmount >> 8) * sWorld->getRate(RATE_DROP_MONEY)) << 8;
    }
}

void Loot::SetItemLooted(LootItem* pLootItem, Player* looter)
{
    pLootItem->is_looted = true;

    if (pLootItem->freeforall) // those are not saved, trying to remove them will cause log spam
        return;

    RemoveItemFromSavedLoot(pLootItem);

    if (looter && m_lootedGUID)
    {
        if (!IsItemValidForLog(pLootItem))
            return;

        sLog->outLoot("Loot::SetItemLooted: Map Id: [%u] Instance Id: [%u] Item Id: [%u] looted by: %s [%u]", m_mapID.nMapId, m_mapID.nInstanceId, pLootItem->itemid, looter->GetName().c_str(), looter->GetGUIDLow());

        Map* map = sMapMgr->FindMap(m_mapID.nMapId, m_mapID.nInstanceId);
        if (map == nullptr)
            return;

         WorldObject* object = nullptr;
        if (IS_CRE_OR_VEH_GUID(m_lootedGUID))
            object = map->GetCreature(m_lootedGUID);
        else if (IS_GAMEOBJECT_GUID(m_lootedGUID))
            object = map->GetGameObject(m_lootedGUID);

        if (!object)
            return;

        //(map_id, instance_id, owner_entry, item_entry, looter_guid)
        auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_LOOT_LOG_ITEM_LOOTED);
        stmt->setUInt32(0, m_mapID.nMapId);
        stmt->setUInt32(1, m_mapID.nInstanceId);
        stmt->setUInt32(2, object->GetEntry());
        stmt->setUInt32(3, pLootItem->itemid);
        stmt->setUInt32(4, looter->GetGUIDLow());
        CharacterDatabase.Execute(stmt);
    }
}

LootItem* Loot::LootItemInSlot(uint32 lootSlot, Player* player, QuestItem** qitem, QuestItem** ffaitem, QuestItem** conditem)
{
    LootItem* item = nullptr;
    bool is_looted = true;
    if (lootSlot >= items.size())
    {
        uint32 questSlot                 = lootSlot - items.size();
        QuestItemMap::const_iterator itr = PlayerQuestItems.find(player->GetGUIDLow());
        if (itr != PlayerQuestItems.end() && questSlot < itr->second->size())
        {
            QuestItem* qitem2 = &itr->second->at(questSlot);
            if (qitem)
                *qitem = qitem2;
            item = &quest_items[qitem2->index];
            if (item->follow_loot_rules && !item->AllowedForPlayer(player)) // pussywizard: such items (follow_loot_rules) are added to every player, but not everyone is allowed, check it here
                return nullptr;
            is_looted = qitem2->is_looted;
        }
    }
    else
    {
        item      = &items[lootSlot];
        is_looted = item->is_looted;
        if (item->freeforall)
        {
            QuestItemMap::const_iterator itr = PlayerFFAItems.find(player->GetGUIDLow());
            if (itr != PlayerFFAItems.end())
            {
                for (QuestItemList::const_iterator iter = itr->second->begin(); iter != itr->second->end(); ++iter)
                    if (iter->index == lootSlot)
                    {
                        QuestItem* ffaitem2 = (QuestItem*)&(*iter);
                        if (ffaitem)
                            *ffaitem = ffaitem2;
                        is_looted = ffaitem2->is_looted;
                        break;
                    }
            }
        }
        else if (!item->conditions.empty())
        {
            QuestItemMap::const_iterator itr = PlayerNonQuestNonFFAConditionalItems.find(player->GetGUIDLow());
            if (itr != PlayerNonQuestNonFFAConditionalItems.end())
            {
                for (QuestItemList::const_iterator iter = itr->second->begin(); iter != itr->second->end(); ++iter)
                {
                    if (iter->index == lootSlot)
                    {
                        QuestItem* conditem2 = (QuestItem*)&(*iter);
                        if (conditem)
                            *conditem = conditem2;
                        is_looted = conditem2->is_looted;
                        break;
                    }
                }
            }
        }
    }

    if (is_looted)
        return nullptr;

    return item;
}

LootItem* Loot::LootItemInSlot(uint32 lootSlot)
{
    if (lootSlot >= items.size())
        return nullptr;
    return &items[lootSlot];
}

uint32 Loot::GetMaxSlotInLootFor(Player* player) const
{
    QuestItemMap::const_iterator itr = PlayerQuestItems.find(player->GetGUIDLow());
    return items.size() + (itr != PlayerQuestItems.end() ? itr->second->size() : 0);
}

// return true if there is any FFA, quest or conditional item for the player.
bool Loot::hasItemFor(Player* player) const
{
    QuestItemMap const& lootPlayerQuestItems = GetPlayerQuestItems();
    QuestItemMap::const_iterator q_itr       = lootPlayerQuestItems.find(player->GetGUIDLow());
    if (q_itr != lootPlayerQuestItems.end())
    {
        QuestItemList* q_list = q_itr->second.get();
        for (QuestItemList::const_iterator qi = q_list->begin(); qi != q_list->end(); ++qi)
        {
            const LootItem& item = quest_items[qi->index];
            if (!qi->is_looted && !item.is_looted)
                return true;
        }
    }

    QuestItemMap const& lootPlayerFFAItems = GetPlayerFFAItems();
    QuestItemMap::const_iterator ffa_itr   = lootPlayerFFAItems.find(player->GetGUIDLow());
    if (ffa_itr != lootPlayerFFAItems.end())
    {
        QuestItemList* ffa_list = ffa_itr->second.get();
        for (QuestItemList::const_iterator fi = ffa_list->begin(); fi != ffa_list->end(); ++fi)
        {
            const LootItem& item = items[fi->index];
            if (!fi->is_looted && !item.is_looted)
                return true;
        }
    }

    QuestItemMap const& lootPlayerNonQuestNonFFAConditionalItems = GetPlayerNonQuestNonFFAConditionalItems();
    QuestItemMap::const_iterator nn_itr                          = lootPlayerNonQuestNonFFAConditionalItems.find(player->GetGUIDLow());
    if (nn_itr != lootPlayerNonQuestNonFFAConditionalItems.end())
    {
        QuestItemList* conditional_list = nn_itr->second.get();
        for (QuestItemList::const_iterator ci = conditional_list->begin(); ci != conditional_list->end(); ++ci)
        {
            const LootItem& item = items[ci->index];
            if (!ci->is_looted && !item.is_looted)
                return true;
        }
    }

    return false;
}

// return true if there is any item over the group threshold (i.e. not underthreshold).
bool Loot::hasOverThresholdItem() const
{
    for (uint8 i = 0; i < items.size(); ++i)
    {
        if (!items[i].is_looted && !items[i].is_underthreshold && !items[i].freeforall)
            return true;
    }

    return false;
}

ByteBuffer& operator<<(ByteBuffer& b, LootItem const& li)
{
    b << uint32(li.itemid);
    b << uint32(li.count); // nr of items of this type
    b << uint32(sObjectMgr->GetItemTemplate(li.itemid)->DisplayInfoID);
    b << uint32(li.randomSuffix);
    b << uint32(li.randomPropertyId);
    //b << uint8(0);                                        // slot type - will send after this function call
    return b;
}

ByteBuffer& operator<<(ByteBuffer& b, LootView const& lv)
{
    if (lv.permission == NONE_PERMISSION)
    {
        b << uint32(0); //gold
        b << uint8(0);  // item count
        return b;       // nothing output more
    }

    Loot& l = lv.loot;

    uint8 itemsShown = 0;

    b << uint32(l.gold); //gold

    size_t count_pos = b.wpos(); // pos of item count byte
    b << uint8(0);               // item count placeholder

    switch (lv.permission)
    {
        case GROUP_PERMISSION:
        case MASTER_PERMISSION:
        case RESTRICTED_PERMISSION: {
            // if you are not the round-robin group looter, you can only see
            // blocked rolled items and quest items, and !ffa items
            for (uint8 i = 0; i < l.items.size(); ++i)
            {
                if (!l.items[i].is_looted && !l.items[i].freeforall && l.items[i].conditions.empty() && l.items[i].AllowedForPlayer(lv.viewer))
                {
                    uint8 slot_type;

                    if (l.items[i].is_blocked) // for ML & restricted is_blocked = !is_underthreshold
                    {
                        switch (lv.permission)
                        {
                            case GROUP_PERMISSION:
                                slot_type = LOOT_SLOT_TYPE_ROLL_ONGOING;
                                break;
                            case MASTER_PERMISSION: {
                                if (lv.viewer->GetGroup())
                                {
                                    if (lv.viewer->GetGroup()->GetMasterLooterGuid() == lv.viewer->GetGUID())
                                        slot_type = LOOT_SLOT_TYPE_MASTER;
                                    else
                                        slot_type = LOOT_SLOT_TYPE_LOCKED;
                                }
                                break;
                            }
                            case RESTRICTED_PERMISSION:
                                slot_type = LOOT_SLOT_TYPE_LOCKED;
                                break;
                            default:
                                continue;
                        }
                    }
                    else if (l.items[i].rollWinnerGUID)
                    {
                        if (l.items[i].rollWinnerGUID == lv.viewer->GetGUID())
                            slot_type = LOOT_SLOT_TYPE_OWNER;
                        else
                            continue;
                    }
                    else if (l.roundRobinPlayer == 0 || lv.viewer->GetGUID() == l.roundRobinPlayer || !l.items[i].is_underthreshold)
                    {
                        // no round robin owner or he has released the loot
                        // or it IS the round robin group owner
                        // => item is lootable
                        slot_type = LOOT_SLOT_TYPE_ALLOW_LOOT;
                    }
                    else
                        // item shall not be displayed.
                        continue;

                    b << uint8(i) << l.items[i];
                    b << uint8(slot_type);
                    ++itemsShown;
                }
            }
            break;
        }
        case ROUND_ROBIN_PERMISSION: {
            for (uint8 i = 0; i < l.items.size(); ++i)
            {
                if (!l.items[i].is_looted && !l.items[i].freeforall && l.items[i].conditions.empty() && l.items[i].AllowedForPlayer(lv.viewer))
                {
                    if (l.roundRobinPlayer != 0 && lv.viewer->GetGUID() != l.roundRobinPlayer)
                        // item shall not be displayed.
                        continue;

                    b << uint8(i) << l.items[i];
                    b << uint8(LOOT_SLOT_TYPE_ALLOW_LOOT);
                    ++itemsShown;
                }
            }
            break;
        }
        case ALL_PERMISSION:
        case OWNER_PERMISSION: {
            uint8 slot_type = lv.permission == OWNER_PERMISSION ? LOOT_SLOT_TYPE_OWNER : LOOT_SLOT_TYPE_ALLOW_LOOT;
            for (uint8 i = 0; i < l.items.size(); ++i)
            {
                if (!l.items[i].is_looted && !l.items[i].freeforall && l.items[i].conditions.empty() && l.items[i].AllowedForPlayer(lv.viewer))
                {
                    b << uint8(i) << l.items[i];
                    b << uint8(slot_type);
                    ++itemsShown;
                }
            }
            break;
        }
        default:
            return b;
    }

    LootSlotType slotType = lv.permission == OWNER_PERMISSION ? LOOT_SLOT_TYPE_OWNER : LOOT_SLOT_TYPE_ALLOW_LOOT;

    // Xinef: items that do not follow loot rules need this
    LootSlotType partySlotType = lv.permission == MASTER_PERMISSION ? LOOT_SLOT_TYPE_MASTER : slotType;

    QuestItemMap const& lootPlayerQuestItems = l.GetPlayerQuestItems();
    QuestItemMap::const_iterator q_itr       = lootPlayerQuestItems.find(lv.viewer->GetGUIDLow());
    if (q_itr != lootPlayerQuestItems.end())
    {
        QuestItemList* q_list = q_itr->second.get();
        for (QuestItemList::const_iterator qi = q_list->begin(); qi != q_list->end(); ++qi)
        {
            LootItem& item = l.quest_items[qi->index];
            if (!qi->is_looted && !item.is_looted)
            {
                b << uint8(l.items.size() + (qi - q_list->begin()));
                b << item;
                if (item.follow_loot_rules)
                {
                    switch (lv.permission)
                    {
                        case MASTER_PERMISSION:
                            b << uint8(LOOT_SLOT_TYPE_MASTER);
                            break;
                        case RESTRICTED_PERMISSION:
                            b << (item.is_blocked ? uint8(LOOT_SLOT_TYPE_LOCKED) : uint8(slotType));
                            break;
                        case GROUP_PERMISSION:
                        case ROUND_ROBIN_PERMISSION:
                            if (!item.is_blocked)
                                b << uint8(LOOT_SLOT_TYPE_ALLOW_LOOT);
                            else
                                b << uint8(LOOT_SLOT_TYPE_ROLL_ONGOING);
                            break;
                        default:
                            b << uint8(slotType);
                            break;
                    }
                }
                else if (!item.freeforall)
                    b << uint8(partySlotType);
                else
                    b << uint8(slotType);
                ++itemsShown;
            }
        }
    }

    QuestItemMap const& lootPlayerFFAItems = l.GetPlayerFFAItems();
    QuestItemMap::const_iterator ffa_itr   = lootPlayerFFAItems.find(lv.viewer->GetGUIDLow());
    if (ffa_itr != lootPlayerFFAItems.end())
    {
        QuestItemList* ffa_list = ffa_itr->second.get();
        for (QuestItemList::const_iterator fi = ffa_list->begin(); fi != ffa_list->end(); ++fi)
        {
            LootItem& item = l.items[fi->index];
            if (!fi->is_looted && !item.is_looted)
            {
                b << uint8(fi->index);
                b << item;
                // Xinef: Here are FFA items, so dont use owner permision
                b << uint8(LOOT_SLOT_TYPE_ALLOW_LOOT /*slotType*/);
                ++itemsShown;
            }
        }
    }

    QuestItemMap const& lootPlayerNonQuestNonFFAConditionalItems = l.GetPlayerNonQuestNonFFAConditionalItems();
    QuestItemMap::const_iterator nn_itr                          = lootPlayerNonQuestNonFFAConditionalItems.find(lv.viewer->GetGUIDLow());
    if (nn_itr != lootPlayerNonQuestNonFFAConditionalItems.end())
    {
        QuestItemList* conditional_list = nn_itr->second.get();
        for (QuestItemList::const_iterator ci = conditional_list->begin(); ci != conditional_list->end(); ++ci)
        {
            LootItem& item = l.items[ci->index];
            if (!ci->is_looted && !item.is_looted)
            {
                b << uint8(ci->index);
                b << item;
                if (item.follow_loot_rules)
                {
                    switch (lv.permission)
                    {
                        case MASTER_PERMISSION:
                            b << uint8(LOOT_SLOT_TYPE_MASTER);
                            break;
                        case RESTRICTED_PERMISSION:
                            b << (item.is_blocked ? uint8(LOOT_SLOT_TYPE_LOCKED) : uint8(slotType));
                            break;
                        case GROUP_PERMISSION:
                        case ROUND_ROBIN_PERMISSION:
                            if (!item.is_blocked)
                                b << uint8(LOOT_SLOT_TYPE_ALLOW_LOOT);
                            else
                                b << uint8(LOOT_SLOT_TYPE_ROLL_ONGOING);
                            break;
                        default:
                            b << uint8(slotType);
                            break;
                    }
                }
                else if (!item.freeforall)
                    b << uint8(partySlotType);
                else
                    b << uint8(slotType);
                ++itemsShown;
            }
        }
    }

    //update number of items shown
    b.put<uint8>(count_pos, itemsShown);

    return b;
}

//
// --------- LootTemplate::LootGroup ---------
//

LootTemplate::LootGroup::~LootGroup()
{
    while (!ExplicitlyChanced.empty())
    {
        delete ExplicitlyChanced.back();
        ExplicitlyChanced.pop_back();
    }

    while (!EqualChanced.empty())
    {
        delete EqualChanced.back();
        EqualChanced.pop_back();
    }
}

// Adds an entry to the group (at loading stage)
void LootTemplate::LootGroup::AddEntry(LootStoreItem* item)
{
    if (item->chance != 0)
        ExplicitlyChanced.push_back(item);
    else
        EqualChanced.push_back(item);
}

// Rolls an item from the group, returns NULL if all miss their chances
LootStoreItem const* LootTemplate::LootGroup::Roll(Loot& loot, uint16 lootMode, std::optional<AdditionalLootSettings> settings) const
{
    LootStoreItemList possibleLoot = ExplicitlyChanced;
    possibleLoot.remove_if(LootGroupInvalidSelector(loot, lootMode));

    if (!possibleLoot.empty()) // First explicitly chanced entries are checked
    {
        float roll = (float)rand_chance();
        float totalChance         = RawTotalChance();
        bool applyChanceModifiers = totalChance < 100.f && roll > totalChance;

        for (LootStoreItemList::const_iterator itr = possibleLoot.begin(); itr != possibleLoot.end(); ++itr) // check each explicitly chanced entry in the template and modify its chance based on quality.
        {
            LootStoreItem* item = *itr;
            float finalChance   = item->chance;
            bool const applyBonusChance = [settings, applyChanceModifiers, finalChance, item]()
            {
                if (!applyChanceModifiers)
                    return false;

                if (!settings)
                    return false;

                if (settings->IsInInstanceOrNorthrend)
                    return false;

                return true;
            }();

            if (applyBonusChance)
            {
                finalChance *= 2.0f;
                if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid))
                    if (sObjectMgr->IsQuestItem(proto))
                        if (auto const questItemDropChance = sCustomMgr->GetQuestItemDropChance())
                            finalChance = questItemDropChance.value();
            }

            finalChance = std::min(100.0f, finalChance);
            if (finalChance >= 100.0f)
                return item;

            roll -= finalChance;
            if (roll < 0)
                return item;
        }
    }

    possibleLoot = EqualChanced;
    possibleLoot.remove_if(LootGroupInvalidSelector(loot, lootMode));
    if (!possibleLoot.empty()) // If nothing selected yet - an item is taken from equal-chanced part
        return Trinity::Containers::SelectRandomContainerElement(possibleLoot);

    return nullptr; // Empty drop from the group
}

// True if group includes at least 1 quest drop entry
bool LootTemplate::LootGroup::HasQuestDrop() const
{
    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if ((*i)->needs_quest)
            return true;

    for (LootStoreItemList::const_iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        if ((*i)->needs_quest)
            return true;

    return false;
}

// True if group includes at least 1 quest drop entry for active quests of the player
bool LootTemplate::LootGroup::HasQuestDropForPlayer(Player const* player) const
{
    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if (player->HasQuestForItem((*i)->itemid))
            return true;

    for (LootStoreItemList::const_iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        if (player->HasQuestForItem((*i)->itemid))
            return true;

    return false;
}

void LootTemplate::LootGroup::CopyConditions(ConditionList /*conditions*/)
{
    for (LootStoreItemList::iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        (*i)->conditions.clear();

    for (LootStoreItemList::iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        (*i)->conditions.clear();
}

// Rolls an item from the group (if any takes its chance) and adds the item to the loot
void LootTemplate::LootGroup::Process(Loot& loot, uint16 lootMode, std::optional<AdditionalLootSettings> settings) const
{
    if (LootStoreItem const* item = Roll(loot, lootMode, settings))
        loot.AddItem(*item);
}

// Overall chance for the group without equal chanced items
float LootTemplate::LootGroup::RawTotalChance() const
{
    float result = 0;

    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if (!(*i)->needs_quest)
            result += (*i)->chance;

    return result;
}

// Overall chance for the group
float LootTemplate::LootGroup::TotalChance() const
{
    float result = RawTotalChance();

    if (!EqualChanced.empty() && result < 100.0f)
        return 100.0f;

    return result;
}

void LootTemplate::LootGroup::Verify(LootStore const& lootstore, uint32 id, uint8 group_id) const
{
    float chance = RawTotalChance();
    if (chance > 101.0f) // TODO: replace with 100% when DBs will be ready
    {
        sLog->outErrorDb("Table '%s' entry %u group %d has total chance > 100%% (%f)", lootstore.GetName(), id, group_id, chance);
    }

    if (chance >= 100.0f && !EqualChanced.empty())
    {
        sLog->outErrorDb("Table '%s' entry %u group %d has items with chance=0%% but group total chance >= 100%% (%f)", lootstore.GetName(), id, group_id, chance);
    }
}

void LootTemplate::LootGroup::CheckLootRefs(LootTemplateMap const& /*store*/, LootIdSet* ref_set) const
{
    for (LootStoreItemList::const_iterator ieItr = ExplicitlyChanced.begin(); ieItr != ExplicitlyChanced.end(); ++ieItr)
    {
        LootStoreItem* item = *ieItr;
        if (item->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-item->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-item->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-item->mincountOrRef);
        }
    }

    for (LootStoreItemList::const_iterator ieItr = EqualChanced.begin(); ieItr != EqualChanced.end(); ++ieItr)
    {
        LootStoreItem* item = *ieItr;
        if (item->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-item->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-item->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-item->mincountOrRef);
        }
    }
}

//
// --------- LootTemplate ---------
//

LootTemplate::~LootTemplate()
{
    while (!Entries.empty())
    {
        delete Entries.back();
        Entries.pop_back();
    }

    for (size_t i = 0; i < Groups.size(); ++i)
        delete Groups[i];
    Groups.clear();
}

// Adds an entry to the group (at loading stage)
void LootTemplate::AddEntry(LootStoreItem* item)
{
    if (item->group > 0 && item->mincountOrRef > 0) // Group
    {
        if (item->group >= Groups.size())
            Groups.resize(item->group, nullptr); // Adds new group the the loot template if needed
        if (!Groups[item->group - 1])
            Groups[item->group - 1] = new LootGroup();

        Groups[item->group - 1]->AddEntry(item); // Adds new entry to the group
    }
    else // Non-grouped entries and references are stored together
        Entries.push_back(item);
}

void LootTemplate::CopyConditions(ConditionList conditions)
{
    for (LootStoreItemList::iterator i = Entries.begin(); i != Entries.end(); ++i)
        (*i)->conditions.clear();

    for (LootGroups::iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            group->CopyConditions(conditions);
}

void LootTemplate::CopyConditions(LootItem* li) const
{
    // Copies the conditions list from a template item to a LootItem
    for (LootStoreItemList::const_iterator _iter = Entries.begin(); _iter != Entries.end(); ++_iter)
    {
        LootStoreItem* item = *_iter;
        if (item->itemid != li->itemid)
            continue;

        li->conditions = item->conditions;
        break;
    }
}

// Rolls for every item in the template and adds the rolled items the the loot
void LootTemplate::Process(Loot& loot, bool rate, uint16 lootMode, uint8 groupId, std::optional<AdditionalLootSettings> settings) const
{
    if (groupId) // Group reference uses own processing of the group
    {
        if (groupId > Groups.size())
            return; // Error message already printed at loading stage

        if (!Groups[groupId - 1])
            return;

        Groups[groupId - 1]->Process(loot, lootMode, settings);
        return;
    }

    // Rolling non-grouped items
    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        LootStoreItem* item = *i;
        if (!(item->lootmode & lootMode)) // Do not add if mode mismatch
            continue;

        if (!item->Roll(rate, settings))
            continue; // Bad luck for the entry

        if (item->mincountOrRef < 0) // References processing
        {
            LootTemplate const* Referenced = LootTemplates_Reference.GetLootFor(-item->mincountOrRef);
            if (!Referenced)
                continue; // Error message already printed at loading stage

            uint32 maxcount = uint32(float(item->maxcount) * sWorld->getRate(RATE_DROP_ITEM_REFERENCED_AMOUNT));
            for (uint32 loop = 0; loop < maxcount; ++loop) // Ref multiplicator
                Referenced->Process(loot, rate, lootMode, item->group, settings);
        }
        else                     // Plain entries (not a reference, not grouped)
            loot.AddItem(*item); // Chance is already checked, just add
    }

    // Now processing groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            group->Process(loot, lootMode, settings);
}

// True if template includes at least 1 quest drop entry
bool LootTemplate::HasQuestDrop(LootTemplateMap const& store, uint8 groupId) const
{
    if (groupId) // Group reference
    {
        if (groupId > Groups.size())
            return false; // Error message [should be] already printed at loading stage

        if (!Groups[groupId - 1])
            return false;

        return Groups[groupId - 1]->HasQuestDrop();
    }

    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        LootStoreItem* item = *i;
        if (item->mincountOrRef < 0) // References
        {
            LootTemplateMap::const_iterator Referenced = store.find(-item->mincountOrRef);
            if (Referenced == store.end())
                continue; // Error message [should be] already printed at loading stage
            if (Referenced->second->HasQuestDrop(store, item->group))
                return true;
        }
        else if (item->needs_quest)
            return true; // quest drop found
    }

    // Now processing groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            if (group->HasQuestDrop())
                return true;

    return false;
}

// True if template includes at least 1 quest drop for an active quest of the player
bool LootTemplate::HasQuestDropForPlayer(LootTemplateMap const& store, Player const* player, uint8 groupId) const
{
    if (groupId) // Group reference
    {
        if (groupId > Groups.size())
            return false; // Error message already printed at loading stage

        if (!Groups[groupId - 1])
            return false;

        return Groups[groupId - 1]->HasQuestDropForPlayer(player);
    }

    // Checking non-grouped entries
    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        LootStoreItem* item = *i;
        if (item->mincountOrRef < 0) // References processing
        {
            LootTemplateMap::const_iterator Referenced = store.find(-item->mincountOrRef);
            if (Referenced == store.end())
                continue; // Error message already printed at loading stage
            if (Referenced->second->HasQuestDropForPlayer(store, player, item->group))
                return true;
        }
        else if (player->HasQuestForItem(item->itemid))
            return true; // active quest drop found
    }

    // Now checking groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            if (group->HasQuestDropForPlayer(player))
                return true;

    return false;
}

// Checks integrity of the template
void LootTemplate::Verify(LootStore const& lootstore, uint32 id) const
{
    // Checking group chances
    for (uint32 i = 0; i < Groups.size(); ++i)
        if (Groups[i])
            Groups[i]->Verify(lootstore, id, i + 1);

    // TODO: References validity checks
}

void LootTemplate::CheckLootRefs(LootTemplateMap const& store, LootIdSet* ref_set) const
{
    for (LootStoreItemList::const_iterator ieItr = Entries.begin(); ieItr != Entries.end(); ++ieItr)
    {
        LootStoreItem* item = *ieItr;
        if (item->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-item->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-item->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-item->mincountOrRef);
        }
    }

    for (LootGroups::const_iterator grItr = Groups.begin(); grItr != Groups.end(); ++grItr)
        if (LootGroup* group = *grItr)
            group->CheckLootRefs(store, ref_set);
}

bool LootTemplate::addConditionItem(Condition* cond)
{
    if (!cond || !cond->isLoaded()) //should never happen, checked at loading
    {
        sLog->outError("LootTemplate::addConditionItem: condition is null");
        return false;
    }

    if (!Entries.empty())
    {
        for (LootStoreItemList::iterator i = Entries.begin(); i != Entries.end(); ++i)
        {
            if ((*i)->itemid == uint32(cond->SourceEntry))
            {
                (*i)->conditions.push_back(cond);
                return true;
            }
        }
    }

    if (!Groups.empty())
    {
        for (LootGroups::iterator groupItr = Groups.begin(); groupItr != Groups.end(); ++groupItr)
        {
            LootGroup* group = *groupItr;
            if (!group)
                continue;

            LootStoreItemList* itemList = group->GetExplicitlyChancedItemList();
            if (!itemList->empty())
            {
                for (LootStoreItemList::iterator i = itemList->begin(); i != itemList->end(); ++i)
                {
                    if ((*i)->itemid == uint32(cond->SourceEntry))
                    {
                        (*i)->conditions.push_back(cond);
                        return true;
                    }
                }
            }

            itemList = group->GetEqualChancedItemList();
            if (!itemList->empty())
            {
                for (LootStoreItemList::iterator i = itemList->begin(); i != itemList->end(); ++i)
                {
                    if ((*i)->itemid == uint32(cond->SourceEntry))
                    {
                        (*i)->conditions.push_back(cond);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool LootTemplate::isReference(uint32 id) const
{
    for (LootStoreItemList::const_iterator ieItr = Entries.begin(); ieItr != Entries.end(); ++ieItr)
        if ((*ieItr)->itemid == id && (*ieItr)->mincountOrRef < 0)
            return true;

    return false; //not found or not reference
}

void LoadLootTemplates_Creature()
{
    sLog->outString("Loading creature loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Creature.LoadAndCollectLootIds(lootIdSet);

    // Remove real entries and check loot existence
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.lootid)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Creature.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Creature.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u creature loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 creature loot templates. DB table `creature_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Disenchant()
{
    sLog->outString("Loading disenchanting loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Disenchant.LoadAndCollectLootIds(lootIdSet);

    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        if (uint32 lootid = itr->second.DisenchantID)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Disenchant.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Disenchant.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u disenchanting loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 disenchanting loot templates. DB table `disenchant_loot_template` is empty");
    sLog->outString();
}

void LoadLootTemplates_Fishing()
{
    sLog->outString("Loading fishing loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Fishing.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sAreaTableStore.GetNumRows(); ++i)
        if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(i))
            if (lootIdSet.find(areaEntry->ID) != lootIdSet.end())
                lootIdSet.erase(areaEntry->ID);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Fishing.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u fishing loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 fishing loot templates. DB table `fishing_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Gameobject()
{
    sLog->outString("Loading gameobject loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Gameobject.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    GameObjectTemplateContainer const* gotc = sObjectMgr->GetGameObjectTemplates();
    for (GameObjectTemplateContainer::const_iterator itr = gotc->begin(); itr != gotc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.GetLootId())
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Gameobject.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Gameobject.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u gameobject loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 gameobject loot templates. DB table `gameobject_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Item()
{
    sLog->outString("Loading item loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Item.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
        if (lootIdSet.find(itr->second.ItemId) != lootIdSet.end() && itr->second.Flags & ITEM_PROTO_FLAG_OPENABLE)
            lootIdSet.erase(itr->second.ItemId);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Item.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u item loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 item loot templates. DB table `item_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Milling()
{
    sLog->outString("Loading milling loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Milling.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        if (!(itr->second.Flags & ITEM_PROTO_FLAG_MILLABLE))
            continue;

        if (lootIdSet.find(itr->second.ItemId) != lootIdSet.end())
            lootIdSet.erase(itr->second.ItemId);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Milling.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u milling loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 milling loot templates. DB table `milling_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Pickpocketing()
{
    sLog->outString("Loading pickpocketing loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Pickpocketing.LoadAndCollectLootIds(lootIdSet);

    // Remove real entries and check loot existence
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.pickpocketLootId)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Pickpocketing.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Pickpocketing.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u pickpocketing loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 pickpocketing loot templates. DB table `pickpocketing_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Prospecting()
{
    sLog->outString("Loading prospecting loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Prospecting.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        if (!(itr->second.Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
            continue;

        if (lootIdSet.find(itr->second.ItemId) != lootIdSet.end())
            lootIdSet.erase(itr->second.ItemId);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Prospecting.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u prospecting loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 prospecting loot templates. DB table `prospecting_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Mail()
{
    sLog->outString("Loading mail loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Mail.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sMailTemplateStore.GetNumRows(); ++i)
        if (sMailTemplateStore.LookupEntry(i))
            if (lootIdSet.find(i) != lootIdSet.end())
                lootIdSet.erase(i);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Mail.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u mail loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 mail loot templates. DB table `mail_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Skinning()
{
    sLog->outString("Loading skinning loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Skinning.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.SkinLootId)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Skinning.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Skinning.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u skinning loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 skinning loot templates. DB table `skinning_loot_template` is empty");

    sLog->outString();
}

void LoadLootTemplates_Spell()
{
    sLog->outString("Loading spell loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Spell.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    for (uint32 spell_id = 1; spell_id < sSpellMgr->GetSpellInfoStoreSize(); ++spell_id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
        if (!spellInfo)
            continue;

        // possible cases
        if (!spellInfo->IsLootCrafting())
            continue;

        if (lootIdSet.find(spell_id) == lootIdSet.end())
        {
            // not report about not trainable spells (optionally supported by DB)
            // ignore 61756 (Northrend Inscription Research (FAST QA VERSION) for example
            if (!spellInfo->HasAttribute(SPELL_ATTR0_NOT_SHAPESHIFT) || spellInfo->HasAttribute(SPELL_ATTR0_TRADESPELL))
            {
                LootTemplates_Spell.ReportNotExistedId(spell_id);
            }
        }
        else
            lootIdSet.erase(spell_id);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Spell.ReportUnusedIds(lootIdSet);

    if (count)
        sLog->outString(">> Loaded %u spell loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outErrorDb(">> Loaded 0 spell loot templates. DB table `spell_loot_template` is empty");
    sLog->outString();
}

void LoadLootTemplates_Reference()
{
    sLog->outString("Loading reference loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    LootTemplates_Reference.LoadAndCollectLootIds(lootIdSet);

    // check references and remove used
    LootTemplates_Creature.CheckLootRefs(&lootIdSet);
    LootTemplates_Fishing.CheckLootRefs(&lootIdSet);
    LootTemplates_Gameobject.CheckLootRefs(&lootIdSet);
    LootTemplates_Item.CheckLootRefs(&lootIdSet);
    LootTemplates_Milling.CheckLootRefs(&lootIdSet);
    LootTemplates_Pickpocketing.CheckLootRefs(&lootIdSet);
    LootTemplates_Skinning.CheckLootRefs(&lootIdSet);
    LootTemplates_Disenchant.CheckLootRefs(&lootIdSet);
    LootTemplates_Prospecting.CheckLootRefs(&lootIdSet);
    LootTemplates_Mail.CheckLootRefs(&lootIdSet);
    LootTemplates_Reference.CheckLootRefs(&lootIdSet);

    // output error for any still listed ids (not referenced from any loot table)
    LootTemplates_Reference.ReportUnusedIds(lootIdSet);

    sLog->outString(">> Loaded refence loot templates in %u ms", GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();
}
