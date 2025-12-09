/*
REWRITTEN FROM SCRATCH BY PUSSYWIZARD, IT OWNS NOW!
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "trial_of_the_crusader.h"
#include "SpellScript.h"
#include "Player.h"
#include "MoveSplineInit.h"

enum eAIs
{
    AI_MELEE  = 0,
    AI_RANGED = 1,
    AI_HEALER = 2,
    AI_PET    = 3,
};

enum eSharedSpells
{
    SPELL_ANTI_AOE    = 68595,
    SPELL_PVP_TRINKET = 65547,
};

enum Boost
{
    SPELL_WILD_GROWTH        = 48438, // Druid
    SPELL_CHAIN_HEAL         = 1064,  // Shaman
    SPELL_BEACON_OF_LIGHT    = 53563, // Paladin
    SPELL_GUARDIAN_SPIRIT    = 47788, // Priest
    SPELL_MIND_SEAR          = 48045, // Priest
    SPELL_SEED_OF_CORRUPTION = 47836, // Warlock
    SPELL_MIRROR_IMAGE       = 58832, // Mage
    SPELL_MULTI_SHOT         = 49048, // Hunter
    SPELL_STARFALL           = 53201, // Druid
    SPELL_SHOCKWAVE          = 46968, // Warrior
    SPELL_DEATH_AND_DECAY    = 43265, // Death Knight
    SPELL_KILLING_SPREE      = 51690, // Rogue
    SPELL_CONSECRATION       = 69930, // Paladin
};

template<typename Type>
Unit* GetHighestAttackerCount(Unit* src, std::list<Type*> targetList, bool excludeSelf = false)
{
    if (excludeSelf)
        Trinity::Containers::EraseIf(targetList, [src](Type const* unit) { return unit->GetGUID() == src->GetGUID(); });
    auto it = std::max_element(std::begin(targetList), std::end(targetList), [](Type const* lhs, Type const* rhs) { return lhs->getAttackers().size() < rhs->getAttackers().size(); });
    if (it == std::end(targetList))
        return nullptr;

    return *it;
}

struct boss_faction_championsAI : public ScriptedAI
{
    boss_faction_championsAI(Creature* pCreature, uint32 aitype)
        : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
        mAIType     = aitype;
        threatTimer = 2000;
        powerTimer  = 1000;
        checkTimer  = 5000;

        me->SetReactState(REACT_AGGRESSIVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            me->SetMaxHealth(me->GetMaxHealth() * 1.35f);
            me->SetFullHealth();
        }
    }

    InstanceScript* pInstance;
    uint32 mAIType;
    uint32 threatTimer;
    uint32 powerTimer;
    uint32 checkTimer;

    void EnterCombat(Unit* /*who*/) override
    {
        me->SetInCombatWithZone();
        RecalculateThreat();
        if (pInstance)
            pInstance->SetData(TYPE_FACTION_CHAMPIONS_START, 0);
    }

    void AttackStart(Unit* who) override
    {
        if (!who)
            return;

        if (mAIType == AI_MELEE || mAIType == AI_PET)
            UnitAI::AttackStart(who);
        else
            UnitAI::AttackStartCaster(who, 18.5f);
    }

    float GetThreatMod(float dist, float armor, uint32 health, uint32 /*maxhealth*/, Unit* target)
    {
        float mod_dist   = 1.0f;                                                // 0.2 .. 1.0
        float mod_health = health > 40000 ? 2.0f : (60000 - health) / 10000.0f; // 2.0 .. 6.0
        float mod_armor  = (mAIType == AI_MELEE || mAIType == AI_PET) ? Unit::CalcArmorReducedDamage(me, target, 10000, nullptr) / 10000.0f : 1.0f;
        return mod_dist * mod_health * mod_armor;
    }

    void RecalculateThreat(Unit* overrideTarget = nullptr)
    {
        if (overrideTarget)
        {
            float threatMod = GetThreatMod(me->GetDistance2d(overrideTarget), (float)overrideTarget->GetArmor(), overrideTarget->GetHealth(), overrideTarget->GetMaxHealth(), overrideTarget);
            me->getThreatManager().modifyThreatPercent(overrideTarget, -100);
            if (HostileReference* ref = me->getThreatManager().getOnlineContainer().getReferenceByTarget(overrideTarget))
                ref->addThreat(10000000.0f * threatMod);
            return;
        }

        std::vector<Unit*> targets;
        ThreatContainer::StorageType const& threatList = me->getThreatManager().getThreatList();
        for (auto itr = threatList.begin(); itr != threatList.end(); ++itr)
        {
            Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
            if (unit && unit->GetTypeId() == TYPEID_PLAYER && me->getThreatManager().getThreat(unit) && !IsNonViableTarget(unit))
                if (targets.size() <= 2)
                    targets.push_back(unit);
        }

        if (!targets.empty())
        {
            if (Unit* target = Trinity::Containers::SelectRandomContainerElement(targets))
            {
                float threatMod = GetThreatMod(me->GetDistance2d(target), (float)target->GetArmor(), target->GetHealth(), target->GetMaxHealth(), target);
                me->getThreatManager().modifyThreatPercent(target, -100);
                if (HostileReference* ref = me->getThreatManager().getOnlineContainer().getReferenceByTarget(target))
                    ref->addThreat(10000000.0f * threatMod);
            }
        }
    }

    void EventMapGCD(EventMap& e, uint32 delay, uint32 gcd = 0)
    {
        e.DelayEventsToMax(delay, gcd);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (pInstance && mAIType != AI_PET && !IsPet(me))
            pInstance->SetData(TYPE_FACTION_CHAMPIONS, DONE);
    }

    void KilledUnit(Unit* who) override
    {
        if (pInstance)
            pInstance->SetData(TYPE_FACTION_CHAMPIONS_PLAYER_DIED, 1);
    }

    void EnterEvadeMode() override
    {
        if (pInstance)
            pInstance->SetData(TYPE_FAILED, 0);
    }

    bool IsCCed()
    {
        // check for stun, fear, etc.
        // for casting, silence, disarm check individually in the ai
        if (me->isFeared() || me->isFrozen() || me->HasUnitState(UNIT_STATE_STUNNED) || me->HasUnitState(UNIT_STATE_CONFUSED))
        {
            if (!IsHeroic())
                return true;
            if (me->HasSpellCooldown(SPELL_PVP_TRINKET))
                return true;
            else
            {
                me->CastSpell(me, SPELL_PVP_TRINKET, false);
                me->AddSpellCooldown(SPELL_PVP_TRINKET, 0, 0);
            }
        }
        return false;
    }

    bool IsPet(Unit* unit) const
    {
        return unit->GetEntry() == 35465 || unit->GetEntry() == 35610;
    }

    Creature* SelectTarget_MostHPLostFriendlyMissingBuff(uint32 spell, float range)
    {
        std::list<Creature*> list = DoFindFriendlyMissingBuff(range, spell);

        if (list.empty())
            return nullptr;

        // Select target based on amount of health (for example when target has 25% health the chance for selection is 100%-25%=75%).
        for (auto itr : list)
            if (!IsPet(itr) && !itr->HasAura(27827))
                if (roll_chance_f(100.0f - itr->GetHealthPct()))
                    return itr;

        // Selection based on amount of health failed - try to choose random target that has lower hp than 100%.
        for (auto itr : list)
            if (!IsPet(itr))
                if (itr->GetHealth() < itr->GetMaxHealth())
                    return itr;

        // All non-pet targets are fully healed - get random pet
        for (auto itr : list)
            if (IsPet(itr))
                if (itr->GetHealth() < itr->GetMaxHealth())
                    return itr;

        return Trinity::Containers::SelectRandomContainerElement(list);
    }

    template<typename Predicate>
    std::vector<Unit*> GetEnemiesWithPredicate(Predicate&& pred)
    {
        std::vector<Unit*> targets;
        ThreatContainer::StorageType const& tList = me->getThreatManager().getThreatList();
        for (ThreatContainer::StorageType::const_iterator iter = tList.begin(); iter != tList.end(); ++iter)
        {
            Unit* target = ObjectAccessor::GetUnit((*me), (*iter)->getUnitGuid());
            if (!target)
                continue;
            if (IsNonViableTarget(target))
                continue;
            if (pred(target))
                targets.emplace_back(target);
        }

        return targets;
    }

    template<typename Predicate>
    std::vector<Unit*> GetAlliesWithPredicate(Predicate&& pred, uint32 spellId = 0, float range = 100.0f)
    {
        std::vector<Unit*> newTargets;
        for (Creature* target : DoFindFriendlyMissingBuff(range, spellId))
            if (pred(target))
                newTargets.emplace_back(target);

        return newTargets;
    }

    uint32 EnemiesInRange(float distance)
    {
        return GetEnemiesWithPredicate([this, distance](Unit const* target) { return me->IsInRange(target, 0.0f, distance); }).size();
    }

    Unit* SelectEnemyCaster(bool casting, float range) const
    {
        auto const& players = me->GetMap()->GetPlayers();
        for (auto iter = players.begin(); iter != players.end(); ++iter)
        {
            auto player = iter->GetSource();
            if (!player)
                continue;
            if (IsNonViableTarget(player))
                continue;
            if (player->IsGameMaster())
                continue;
            if (player->getPowerType() == POWER_MANA && (!casting || player->HasUnitState(UNIT_STATE_CASTING)) && me->IsInRange(player, 0.0f, range))
                return player;
        }
        return nullptr;
    }

    bool IsNonViableTarget(Unit* target) const
    {
        if (!target)
            return false;

        if (target->HasUnitState(UNIT_STATE_ISOLATED) || target->IsPolymorphed() || target->isFeared())
            return true;

        for (uint32 spellId : { 66008, 65878, 65877, 66054, 47585, 1022, 5599, 10278, 45438, 71264, 71265 })
            if (target->HasAura(spellId))
                return true;

        return false;
    }

    //! Riztazz's refactor, could be done a lot better but i just dont have time rn
    Creature* GetPet()
    {
        std::vector<uint32> entries { 35465, 35610 };
        if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            entries.emplace_back(NPC_BOOST_FELGUARD);
            entries.emplace_back(NPC_BOOST_KING_KRUSH);
            entries.emplace_back(NPC_BOOST_LOQUENAHAK);
        }

        std::vector<Creature*> pets;
        pets.reserve(entries.size());
        for (uint32 entry : entries)
            if (Creature* pet = me->FindNearestCreature(entry, 80.0f))
                pets.emplace_back(pet);
        
        if (pets.empty())
            return nullptr;

        auto minHealthPet = std::min_element(pets.begin(), pets.end(), [] (Creature const* lhs, Creature const* rhs) { return lhs->GetHealthPct() < rhs->GetHealthPct(); });
        if (minHealthPet != pets.end() && (*minHealthPet)->GetHealthPct() != 100.0f)
            return *minHealthPet;

        return nullptr;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->IsInCombat())
            return;

        if (checkTimer <= diff)
        {
            uint8 aliveCount          = 0;
            Map::PlayerList const& pl = pInstance->instance->GetPlayers();
            for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
                if (Player* plr = itr->GetSource())
                    if (plr->IsAlive() && !plr->IsGameMaster())
                        ++aliveCount;

            if (aliveCount == 0)
                EnterEvadeMode();

            checkTimer = 5000;
        }
        else
            checkTimer -= diff;

        if (Unit* victim = me->GetVictim())
        {
            if (IsNonViableTarget(victim))
            {
                RecalculateThreat();
                threatTimer = urand(8750, 9250);
            }
        }

        if (threatTimer <= diff)
        {
            RecalculateThreat();
            threatTimer = urand(8750, 9250);
        }
        else
            threatTimer -= diff;

        if (me->getPowerType() == POWER_MANA)
        {
            if (powerTimer <= diff)
            {
                me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                powerTimer = 4000;
            }
            else
                powerTimer -= diff;
        }
        else if (me->getPowerType() == POWER_ENERGY)
        {
            if (powerTimer <= diff)
            {
                me->ModifyPower(POWER_ENERGY, me->GetMaxPower(POWER_ENERGY) / 3);
                powerTimer = 1000;
            }
            else
                powerTimer -= diff;
        }
    }
};

struct npc_toc_pet_baseAI : public boss_faction_championsAI
{
    npc_toc_pet_baseAI(Creature* creature)
        : boss_faction_championsAI(creature, AI_PET)
    {
        me->SetHealth(me->CountPctFromMaxHealth(75));
        me->SetRegeneratingHealth(false);
        events.Reset();
        if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        }
    }

    EventMap events;

    virtual void DoEvent(uint32 eventId) = 0;
    virtual void ScheduleCombatEvents(){};

    void EnterCombat(Unit* attacker) override
    {
        boss_faction_championsAI::EnterCombat(attacker);
        ScheduleCombatEvents();
    }

    virtual bool CanCast()
    {
        return !(me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->SelectNearestPlayer(200.0f))
            EnterEvadeMode();

        boss_faction_championsAI::UpdateAI(diff);
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (!CanCast())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            DoEvent(eventId);
            if (!CanCast())
                return;
        }

        DoMeleeAttackIfReady();
    }

    void EnterEvadeMode() override
    {
        me->DespawnOrUnsummon();
    }
};


enum eDruidSpells
{
    SPELL_LIFEBLOOM    = 66093,
    SPELL_NOURISH      = 66066,
    SPELL_REGROWTH     = 66067,
    SPELL_REJUVENATION = 66065,
    SPELL_THORNS       = 66068,
    SPELL_TRANQUILITY  = 66086,
    SPELL_BARKSKIN     = 65860,
    SPELL_NATURE_GRASP = 66071,
};

enum eDruidEvents
{
    EVENT_SPELL_LIFEBLOOM = 1,
    EVENT_SPELL_NOURISH,
    EVENT_SPELL_REGROWTH,
    EVENT_SPELL_REJUVENATION,
    EVENT_SPELL_THORNS,
    EVENT_SPELL_TRANQUILITY,
    EVENT_SPELL_NATURE_GRASP,
    EVENT_SPELL_BARKSKIN = 101,
    // Boost
    EVENT_SPELL_WILD_GROWTH
};

class npc_toc_druid : public CreatureScript
{
public:
    npc_toc_druid()
        : CreatureScript("npc_toc_druid")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_druidAI(pCreature);
    }

    struct npc_toc_druidAI : public boss_faction_championsAI
    {
        npc_toc_druidAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_HEALER)
        {
            SetEquipmentSlots(false, 51799, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_LIFEBLOOM, urand(5000, 15000));
            events.RescheduleEvent(EVENT_SPELL_NOURISH, urand(5000, 15000));
            events.RescheduleEvent(EVENT_SPELL_REGROWTH, urand(5000, 15000));
            events.RescheduleEvent(EVENT_SPELL_REJUVENATION, urand(5000, 15000));
            // Boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_WILD_GROWTH, urand(5000, 15000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        void EnterCombat(Unit* who) override
        {
            boss_faction_championsAI::EnterCombat(who);
            events.RescheduleEvent(EVENT_SPELL_TRANQUILITY, urand(25000, 40000));
            events.RescheduleEvent(EVENT_SPELL_BARKSKIN, 10000);
            events.RescheduleEvent(EVENT_SPELL_THORNS, urand(5000, 15000));
            events.RescheduleEvent(EVENT_SPELL_NATURE_GRASP, urand(5000, 15000));
            // Boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_WILD_GROWTH, urand(5000, 15000));
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!me->IsInCombat())
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPELL_LIFEBLOOM:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_LIFEBLOOM, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_NOURISH:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_NOURISH, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_REGROWTH:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_REGROWTH, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_REJUVENATION:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_REJUVENATION, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        // Boost
                        case EVENT_SPELL_WILD_GROWTH:
                            if (Creature* target = GetPet())
                                me->CastCustomSpell(SPELL_WILD_GROWTH, SPELLVALUE_BASE_POINT0, urand(3000, 4000), target);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        default:
                            break;
                    }
                }
            }
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim() && !me->IsInCombat())
                return;

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_LIFEBLOOM:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_LIFEBLOOM, 40.0f))
                        me->CastSpell(target, SPELL_LIFEBLOOM, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_NOURISH:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_NOURISH, 40.0f))
                        me->CastSpell(target, SPELL_NOURISH, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_REGROWTH:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_REGROWTH, 40.0f))
                        me->CastSpell(target, SPELL_REGROWTH, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_REJUVENATION:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_REJUVENATION, 40.0f))
                        me->CastSpell(target, SPELL_REJUVENATION, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_THORNS:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_THORNS, 30.0f))
                        me->CastSpell(target, SPELL_THORNS, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_TRANQUILITY:
                    me->CastSpell(me, SPELL_TRANQUILITY, false);
                    events.RepeatEvent(urand(120000, 180000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_BARKSKIN:
                    if (HealthBelowPct(50))
                    {
                        me->CastSpell(me, SPELL_BARKSKIN, false);
                        events.RepeatEvent(60000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_NATURE_GRASP:
                    me->CastSpell(me, SPELL_NATURE_GRASP, false);
                    events.RepeatEvent(60000);
                    EventMapGCD(events, 1500);
                    break;
                // Boost
                case EVENT_SPELL_WILD_GROWTH:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_WILD_GROWTH, 40.0f))
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, urand(3000, 4000));
                        values.AddSpellMod(SPELLVALUE_MAX_TARGETS, 5);
                        me->CastCustomSpell(SPELL_WILD_GROWTH, values, target);
                    }
                    events.RepeatEvent(6000);
                    EventMapGCD(events, 1500);
                    break;
                default:
                    break;
            }
        }
    };
};

enum eShamanSpells
{
    SPELL_GROUNDING_TOTEM      = 65989,
    SPELL_WINDFURY_TOTEM       = 65990,
    SPELL_TREMOR_TOTEM         = 65992,
    SPELL_STR_OF_EARTH_TOTEM   = 65991,
    SPELL_HEALING_STREAM_TOTEM = 65995,
    SPELL_SEARING_TOTEM        = 65997,

    SPELL_HEALING_WAVE   = 66055,
    SPELL_RIPTIDE        = 66053,
    SPELL_SPIRIT_CLEANSE = 66056,
    SPELL_HEROISM        = 65983,
    SPELL_BLOODLUST      = 65980,
    SPELL_HEX            = 66054,
    SPELL_EARTH_SHIELD   = 66063,
    SPELL_EARTH_SHOCK    = 65973,
    AURA_EXHAUSTION      = 57723,
    AURA_SATED           = 57724,

    SPELL_TOTEM_OF_WRATH     = 57722,
    SPELL_MANA_SPRING_TOTEM  = 58774,
    SPELL_WRATH_OF_AIR_TOTEM = 3738
};

enum eShamanEvents
{
    EVENT_SPELL_HEALING_WAVE = 1,
    EVENT_SPELL_RIPTIDE,
    EVENT_SPELL_SPIRIT_CLEANSE,
    EVENT_SPELL_HEROISM_OR_BLOODLUST,
    EVENT_SPELL_HEX,
    EVENT_SPELL_EARTH_SHIELD,
    EVENT_SPELL_EARTH_SHOCK,
    EVENT_SPELL_CHAIN_HEAL
};

enum ShamanType
{
    SHAMAN_RESTORATION = 0,
    SHAMAN_ELEMENTAL,
    SHAMAN_ENHANCEMENT,
};

eAIs GetShamanAIType(ShamanType type)
{
    switch (type)
    {
        case SHAMAN_RESTORATION:
            return AI_HEALER;
        case SHAMAN_ELEMENTAL:
            return AI_RANGED;
        case SHAMAN_ENHANCEMENT:
        default:
            return AI_MELEE;
    }
}

struct npc_toc_shaman_baseAI : boss_faction_championsAI
{
    npc_toc_shaman_baseAI(Creature* creature, ShamanType type)
        : boss_faction_championsAI(creature, GetShamanAIType(type))
        , _type(type)
    {
    }

    void HandleTotems()
    {
        if (_type == SHAMAN_ELEMENTAL)
        {
            DoCastAOE(SPELL_WRATH_OF_AIR_TOTEM, true);                           // air
            DoCastAOE(RAND(SPELL_STR_OF_EARTH_TOTEM, SPELL_TREMOR_TOTEM), true); // earth
            DoCastAOE(SPELL_MANA_SPRING_TOTEM, true);                            // water
            DoCastAOE(SPELL_TOTEM_OF_WRATH, true);                               // fire
        }
        else
        {
            DoCastAOE(RAND(SPELL_GROUNDING_TOTEM, SPELL_WINDFURY_TOTEM), true);  // air
            DoCastAOE(RAND(SPELL_STR_OF_EARTH_TOTEM, SPELL_TREMOR_TOTEM), true); // earth
            DoCastAOE(SPELL_HEALING_STREAM_TOTEM, true);                         // water
            DoCastAOE(SPELL_SEARING_TOTEM, true);                                // fire
        }
    }

protected:
    ShamanType _type;
};

class npc_toc_shaman : public CreatureScript
{
public:
    npc_toc_shaman()
        : CreatureScript("npc_toc_shaman")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_shamanAI(pCreature);
    }

    struct npc_toc_shamanAI : npc_toc_shaman_baseAI
    {
        npc_toc_shamanAI(Creature* pCreature)
            : npc_toc_shaman_baseAI(pCreature, SHAMAN_RESTORATION)
        {
            SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_HEALING_WAVE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_RIPTIDE, urand(3000, 10000));
            // Boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_CHAIN_HEAL, urand(5000, 15000));
        }

        EventMap events;

        void EnterCombat(Unit* who) override
        {
            boss_faction_championsAI::EnterCombat(who);
            events.RescheduleEvent(EVENT_SPELL_SPIRIT_CLEANSE, urand(10000, 15000));
            events.RescheduleEvent(EVENT_SPELL_HEROISM_OR_BLOODLUST, urand(25000, 40000));
            events.RescheduleEvent(EVENT_SPELL_HEX, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_EARTH_SHIELD, urand(15000, 25000));
            events.RescheduleEvent(EVENT_SPELL_EARTH_SHOCK, urand(3000, 10000));
            events.RescheduleEvent(151, urand(1000, 3000));
            // Boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_CHAIN_HEAL, urand(5000, 15000));
        }

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!me->IsInCombat())
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPELL_HEALING_WAVE:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_HEALING_WAVE, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_RIPTIDE:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_RIPTIDE, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        // Boost
                        case EVENT_SPELL_CHAIN_HEAL:
                            if (Creature* target = GetPet())
                                me->CastCustomSpell(SPELL_CHAIN_HEAL, SPELLVALUE_BASE_POINT0, 15000, target);
                            events.RepeatEvent(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        default:
                            break;
                    }
                }
            }

            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim() && !me->IsInCombat())
                return;

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_HEALING_WAVE:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_HEALING_WAVE, 40.0f))
                        me->CastSpell(target, SPELL_HEALING_WAVE, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_RIPTIDE:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_RIPTIDE, 40.0f))
                        me->CastSpell(target, SPELL_RIPTIDE, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_SPIRIT_CLEANSE:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_SPIRIT_CLEANSE, 40.0f))
                        me->CastSpell(target, SPELL_SPIRIT_CLEANSE, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_HEROISM_OR_BLOODLUST:
                    if (me->GetEntry() == NPC_ALLIANCE_SHAMAN_RESTORATION)
                        me->CastSpell((Unit*)nullptr, SPELL_HEROISM, true);
                    else
                        me->CastSpell((Unit*)nullptr, SPELL_BLOODLUST, true);
                    events.RepeatEvent(300000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_HEX:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                        me->CastSpell(target, SPELL_HEX, false);
                    events.RepeatEvent(45000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_EARTH_SHIELD:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_EARTH_SHIELD, 40.0f))
                        me->CastSpell(target, SPELL_EARTH_SHIELD, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_EARTH_SHOCK:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        DoCastVictim(SPELL_EARTH_SHOCK);
                    events.RepeatEvent(urand(5000, 10000));
                    EventMapGCD(events, 1500);
                    break;
                case 151:
                    HandleTotems();
                    EventMapGCD(events, 1500);
                    events.RepeatEvent(urand(40000, 60000));
                    break;
                // Boost
                case EVENT_SPELL_CHAIN_HEAL:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_CHAIN_HEAL, 40.0f))
                        me->CastCustomSpell(SPELL_CHAIN_HEAL, SPELLVALUE_BASE_POINT0, 15000, target);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
            }
        }
    };
};

enum ePaladinSpells
{
    SPELL_HAND_OF_FREEDOM    = 68757,
    SPELL_BUBBLE             = 66010,
    SPELL_CLEANSE            = 66116,
    SPELL_FLASH_OF_LIGHT     = 66113,
    SPELL_HOLY_LIGHT         = 66112,
    SPELL_HOLY_SHOCK         = 66114,
    SPELL_HAND_OF_PROTECTION = 66009,
    SPELL_HAMMER_OF_JUSTICE  = 66613,
};

enum ePaladinEvents
{
    EVENT_SPELL_HAND_OF_FREEDOM = 1,
    EVENT_SPELL_BUBBLE,
    EVENT_SPELL_CLEANSE,
    EVENT_SPELL_FLASH_OF_LIGHT,
    EVENT_SPELL_HOLY_LIGHT,
    EVENT_SPELL_HOLY_SHOCK,
    EVENT_SPELL_HAND_OF_PROTECTION,
    EVENT_SPELL_HAMMER_OF_JUSTICE,
    // Boost
    EVENT_SPELL_BEACON_OF_LIGHT
};

class npc_toc_paladin : public CreatureScript
{
public:
    npc_toc_paladin()
        : CreatureScript("npc_toc_paladin")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_paladinAI(pCreature);
    }

    struct npc_toc_paladinAI : public boss_faction_championsAI
    {
        npc_toc_paladinAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_HEALER)
        {
            SetEquipmentSlots(false, 50771, 47079, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_FLASH_OF_LIGHT, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_HOLY_LIGHT, urand(5000, 15000));
            events.RescheduleEvent(EVENT_SPELL_HOLY_SHOCK, urand(3000, 10000));
        }

        EventMap events;
        uint64 CurrentBeaconTarget;

        void EnterCombat(Unit* who) override
        {
            CurrentBeaconTarget = 0;
            boss_faction_championsAI::EnterCombat(who);
            events.RescheduleEvent(EVENT_SPELL_HAND_OF_PROTECTION, urand(20000, 35000));
            events.RescheduleEvent(EVENT_SPELL_HAMMER_OF_JUSTICE, urand(10000, 20000));
            events.RescheduleEvent(EVENT_SPELL_HAND_OF_FREEDOM, urand(10000, 15000));
            events.RescheduleEvent(EVENT_SPELL_BUBBLE, 10000);
            events.RescheduleEvent(EVENT_SPELL_CLEANSE, urand(10000, 15000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_BEACON_OF_LIGHT, 0s, 1s);
        }

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        bool IsViableFreedomTarget(Unit* target) const
        {
            if (!target->IsFriendlyTo(me))
                return false;

            if (IsPet(target))
                return false;

            static uint32 const MechanicImmunityList =
                (1 << MECHANIC_SNARE)
                | (1 << MECHANIC_ROOT);

            if (target->HasAuraWithMechanic(MechanicImmunityList))
                return true;

            static std::list<AuraType> const AuraImmunityList = {
                SPELL_AURA_MOD_DECREASE_SPEED,
                SPELL_AURA_MOD_ROOT
            };

            for (AuraType type : AuraImmunityList)
                if (target->HasAuraType(type))
                    return true;

            return false;
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!me->IsInCombat())
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPELL_FLASH_OF_LIGHT:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_FLASH_OF_LIGHT, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_HOLY_LIGHT:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_HOLY_LIGHT, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_HOLY_SHOCK:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_HOLY_SHOCK, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        default:
                            break;
                    }
                }
            }
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim() && !me->IsInCombat())
                return;

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_HAND_OF_FREEDOM:
                {
                    std::list<Creature*> list = DoFindFriendlyMissingBuff(30.0f, SPELL_HAND_OF_FREEDOM);
                    std::list<Creature*> viableTargets;
                    if (!list.empty())
                        for (auto itr : list)
                            if (IsViableFreedomTarget(itr))
                                viableTargets.push_back(itr);

                    if (!viableTargets.empty())
                    {
                        Unit* target = Trinity::Containers::SelectRandomContainerElement(viableTargets);
                        DoCast(target, SPELL_HAND_OF_FREEDOM);
                        events.RepeatEvent(25000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(2500);
                    break;
                }
                case EVENT_SPELL_BUBBLE:
                    if (HealthBelowPct(25))
                    {
                        me->CastSpell(me, SPELL_BUBBLE, false);
                        events.RepeatEvent(300000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(10000);
                    break;
                case EVENT_SPELL_CLEANSE:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_CLEANSE, 40.0f))
                        me->CastSpell(target, SPELL_CLEANSE, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FLASH_OF_LIGHT:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_FLASH_OF_LIGHT, 40.0f))
                        me->CastSpell(target, SPELL_FLASH_OF_LIGHT, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_HOLY_LIGHT:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_HOLY_LIGHT, 40.0f))
                        me->CastSpell(target, SPELL_HOLY_LIGHT, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_HOLY_SHOCK:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_HOLY_SHOCK, 40.0f))
                        me->CastSpell(target, SPELL_HOLY_SHOCK, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_HAND_OF_PROTECTION:
                    if (Unit* target = GetHighestAttackerCount(me, DoFindFriendlyMissingBuff(30.0f, SPELL_HAND_OF_PROTECTION)))
                    {
                        EventMapGCD(events, 1500);
                        DoCast(target, SPELL_HAND_OF_PROTECTION);
                        events.RepeatEvent(300000);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_HAMMER_OF_JUSTICE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 15.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_HAMMER_OF_JUSTICE, false);
                        events.RepeatEvent(40000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(10000);
                    break;
                case EVENT_SPELL_BEACON_OF_LIGHT:
                {
                    bool findNewTarget = false;
                    Unit* target       = ObjectAccessor::GetUnit(*me, CurrentBeaconTarget);
                    if (target && target->HasAura(SPELL_BEACON_OF_LIGHT, me->GetGUID()))
                    {
                        events.RepeatEvent(2000);
                        break;
                    }

                    if (!target)
                        target = GetHighestAttackerCount(me, DoFindFriendlyMissingBuff(60.0f, 0), true);

                    if (target)
                    {
                        DoCast(target, SPELL_BEACON_OF_LIGHT);
                        CurrentBeaconTarget = target->GetGUID();
                    }

                    events.RepeatEvent(4000);
                    break;
                }
            }
        }
    };
};

enum ePriestSpells
{
    SPELL_RENEW          = 66177,
    SPELL_SHIELD         = 66099,
    SPELL_FLASH_HEAL     = 66104,
    SPELL_DISPEL         = 65546,
    SPELL_MANA_BURN      = 66100,
    SPELL_PSYCHIC_SCREAM = 65543,
    SPELL_PENANCE        = 66097,
};

enum ePriestEvents
{
    EVENT_SPELL_RENEW = 1,
    EVENT_SPELL_SHIELD,
    EVENT_SPELL_FLASH_HEAL,
    EVENT_SPELL_MANA_BURN,
    EVENT_SPELL_PENANCE,
    EVENT_SPELL_DISPEL         = 100,
    EVENT_SPELL_PSYCHIC_SCREAM = 101
};

class npc_toc_priest : public CreatureScript
{
public:
    npc_toc_priest()
        : CreatureScript("npc_toc_priest")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_priestAI(pCreature);
    }

    struct npc_toc_priestAI : public boss_faction_championsAI
    {
        npc_toc_priestAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_HEALER)
        {
            SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_RENEW, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_FLASH_HEAL, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_PENANCE, urand(3000, 10000));
            _guardianSpirit = false;
        }

        EventMap events;
        bool _guardianSpirit;

        void EnterCombat(Unit* who) override
        {
            boss_faction_championsAI::EnterCombat(who);
            events.RescheduleEvent(EVENT_SPELL_SHIELD, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DISPEL, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_MANA_BURN, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_PSYCHIC_SCREAM, 10000);
            events.RescheduleEvent(EVENT_SPELL_PENANCE, urand(3000, 10000));
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageEffectType*/, SpellSchoolMask /*spellSchoolMask*/) override
        {
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
            {
                if (damage > me->GetHealth())
                {
                    damage = me->GetHealth() - 1;
                    if (!_guardianSpirit)
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->AddAura(27827, me);
                        _guardianSpirit = true;
                    }
                }
            }
        }

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!me->IsInCombat())
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPELL_RENEW:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_RENEW, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_PENANCE:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_PENANCE, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        case EVENT_SPELL_FLASH_HEAL:
                            if (Creature* target = GetPet())
                                me->CastSpell(target, SPELL_FLASH_HEAL, false);
                            events.Repeat(urand(10000, 15000));
                            EventMapGCD(events, 1500);
                            break;
                        default:
                            break;
                    }
                }
            }

            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim() && !me->IsInCombat())
                return;

            // Boost
            if (_guardianSpirit && !me->HasAura(27827))
                me->Kill(me, me);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_RENEW:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_RENEW, 40.0f))
                        me->CastSpell(target, SPELL_RENEW, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_PENANCE:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_PENANCE, 40.0f))
                        me->CastSpell(target, SPELL_PENANCE, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_SHIELD:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_SHIELD, 40.0f))
                        me->CastSpell(target, SPELL_SHIELD, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FLASH_HEAL:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_FLASH_HEAL, 40.0f))
                        me->CastSpell(target, SPELL_FLASH_HEAL, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_DISPEL:
                    if (Unit* target = (urand(0, 1) ? SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 30.0f, true) : SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_DISPEL, 40.0f)))
                        me->CastSpell(target, SPELL_DISPEL, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_MANA_BURN:
                    if (Unit* target = SelectEnemyCaster(false, 30.0f))
                    {
                        me->CastSpell(target, SPELL_MANA_BURN, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_PSYCHIC_SCREAM:
                    if (HealthBelowPct(50) && EnemiesInRange(8.0f) >= 3)
                    {
                        me->CastSpell((Unit*)nullptr, SPELL_PSYCHIC_SCREAM, false);
                        events.RepeatEvent(30000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
            }
        }
    };
};

enum eShadowPriestSpells
{
    SPELL_SILENCE        = 65542,
    SPELL_VAMPIRIC_TOUCH = 65490,
    SPELL_SW_PAIN        = 65541,
    SPELL_MIND_FLAY      = 65488,
    SPELL_MIND_BLAST     = 65492,
    SPELL_HORROR         = 65545,
    SPELL_DISPERSION     = 65544,
    SPELL_SHADOWFORM     = 16592,
};

enum eShadowPriestEvents
{
    EVENT_SPELL_SILENCE = 1,
    EVENT_SPELL_VAMPIRIC_TOUCH,
    EVENT_SPELL_SW_PAIN,
    EVENT_SPELL_MIND_FLAY,
    EVENT_SPELL_MIND_BLAST,
    EVENT_SPELL_HORROR,
    EVENT_SPELL_DISPERSION,
    // Boost
    EVENT_SPELL_MIND_SEAR
};

class npc_toc_shadow_priest : public CreatureScript
{
public:
    npc_toc_shadow_priest()
        : CreatureScript("npc_toc_shadow_priest")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_shadow_priestAI(pCreature);
    }

    struct npc_toc_shadow_priestAI : public boss_faction_championsAI
    {
        npc_toc_shadow_priestAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_RANGED)
        {
            SetEquipmentSlots(false, 50040, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_SILENCE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_VAMPIRIC_TOUCH, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_SW_PAIN, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_MIND_FLAY, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_MIND_BLAST, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_HORROR, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DISPERSION, 10000);
            events.RescheduleEvent(EVENT_SPELL_DISPEL, urand(5000, 10000));
            events.RescheduleEvent(EVENT_SPELL_PSYCHIC_SCREAM, 10000);
            // boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_MIND_SEAR, 5s, 10s);
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_SILENCE:
                    if (Unit* target = SelectEnemyCaster(false, 30.0f))
                    {
                        me->CastSpell(target, SPELL_SILENCE, false);
                        events.RepeatEvent(45000);
                        EventMapGCD(events, 1500);
                        break;
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_VAMPIRIC_TOUCH:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_VAMPIRIC_TOUCH, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_SW_PAIN:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 40.0f) && !IsNonViableTarget(tar) && !tar->HasAura(SPELL_SW_PAIN); }))
                    {
                        DoCast(target, SPELL_SW_PAIN);
                        EventMapGCD(events, 1500);
                    }
                    events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_MIND_FLAY:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_MIND_FLAY, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_MIND_BLAST:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_MIND_BLAST, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_HORROR:
                    if (me->GetVictim() && me->GetExactDist2d(me->GetVictim()) <= 30.0f && !IsNonViableTarget(me->GetVictim()))
                    {
                        me->CastSpell(me->GetVictim(), SPELL_HORROR, false);
                        events.RepeatEvent(120000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(4000);
                    break;
                case EVENT_SPELL_DISPERSION:
                    if (HealthBelowPct(25))
                    {
                        me->CastSpell(me, SPELL_DISPERSION, false);
                        events.RepeatEvent(180000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_DISPEL:
                    if (auto friendlyTarget = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_DISPEL, 40.0f))
                    {
                        if (auto target = roll_chance_f(friendlyTarget->GetHealthPct()) ? SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 30.0f, true) : friendlyTarget)
                        {
                            DoCast(target, SPELL_DISPEL);
                            EventMapGCD(events, 1500);
                        }
                    }
                    events.RepeatEvent(urand(10000, 15000));
                    break;
                case EVENT_SPELL_PSYCHIC_SCREAM:
                    if (EnemiesInRange(8.0f) >= 3)
                    {
                        DoCastAOE(SPELL_PSYCHIC_SCREAM);
                        events.RepeatEvent(30000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_MIND_SEAR:
                    // TODO: Better target selector
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                    {
                        DoCast(target, SPELL_MIND_SEAR);
                        events.RepeatEvent(15000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
            }
        }
    };
};

enum eWarlockSpells
{
    SPELL_HELLFIRE                   = 65816,
    SPELL_CORRUPTION                 = 65810,
    SPELL_CURSE_OF_AGONY             = 65814,
    SPELL_CURSE_OF_EXHAUSTION        = 65815,
    SPELL_FEAR                       = 65809,
    SPELL_SEARING_PAIN               = 65819,
    SPELL_SHADOW_BOLT                = 65821,
    SPELL_UNSTABLE_AFFLICTION        = 65812,
    SPELL_UNSTABLE_AFFLICTION_DISPEL = 65813,
    SPELL_SUMMON_FELHUNTER           = 67514,
    SPELL_DEATH_COIL_WARL            = 65820,
};

enum eWarlockEvents
{
    EVENT_SPELL_HELLFIRE = 1,
    EVENT_SPELL_CORRUPTION,
    EVENT_SPELL_CURSE_OF_AGONY,
    EVENT_SPELL_CURSE_OF_EXHAUSTION,
    EVENT_SPELL_FEAR,
    EVENT_SPELL_SEARING_PAIN,
    EVENT_SPELL_SHADOW_BOLT,
    EVENT_SPELL_UNSTABLE_AFFLICTION,
    EVENT_SPELL_SUMMON_FELHUNTER,
    EVENT_SPELL_DEATH_COIL_WARL,
    // Boost
    EVENT_SPELL_SEED_OF_CORRUPTION
};

class npc_toc_warlock : public CreatureScript
{
public:
    npc_toc_warlock()
        : CreatureScript("npc_toc_warlock")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_warlockAI(pCreature);
    }

    struct npc_toc_warlockAI : public boss_faction_championsAI
    {
        npc_toc_warlockAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_RANGED)
        {
            SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_HELLFIRE, 10000);
            events.RescheduleEvent(EVENT_SPELL_CORRUPTION, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_CURSE_OF_AGONY, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_CURSE_OF_EXHAUSTION, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_FEAR, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_SEARING_PAIN, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_SHADOW_BOLT, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_UNSTABLE_AFFLICTION, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DEATH_COIL_WARL, urand(15000, 25000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_SEED_OF_CORRUPTION, 10s, 20s);
        }

        EventMap events;

        void DoAction(int32 actionId) override
        {
            if (actionId == 1)
            {
                Position pos = me->GetPosition();
                me->GetNearPosition(pos, 1.0f, float(M_PI / 2));
                DoSummon(35465, pos);
            }
        }

        void JustDied(Unit* killer) override
        {
            boss_faction_championsAI::JustDied(killer);
            if (Creature* felhunter = GetFelhunter())
                felhunter->DespawnOrUnsummon();
        }

        void EnterCombat(Unit* who) override
        {
            boss_faction_championsAI::EnterCombat(who);
            if (Creature* felhunter = GetFelhunter())
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    if (felhunter->IsAIEnabled)
                        felhunter->AI()->AttackStart(target);
        }

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        Creature* GetFelhunter()
        {
            return me->FindNearestCreature(35465, 250.0f);
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_SUMMON_FELHUNTER:
                    DoSummon(35465, *me);
                    events.PopEvent();
                    break;
                case EVENT_SPELL_HELLFIRE:
                    if (EnemiesInRange(9.0f) >= 3)
                    {
                        me->CastSpell((Unit*)nullptr, SPELL_HELLFIRE, false);
                        events.RepeatEvent(30000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_CORRUPTION:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_CORRUPTION, false);
                    events.RepeatEvent(urand(10000, 20000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_CURSE_OF_AGONY:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_CURSE_OF_AGONY, false);
                    events.RepeatEvent(urand(10000, 20000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_CURSE_OF_EXHAUSTION:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_CURSE_OF_EXHAUSTION, false);
                    events.RepeatEvent(urand(10000, 20000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FEAR:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                        me->CastSpell(target, SPELL_FEAR, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_SEARING_PAIN:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_SEARING_PAIN, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_SHADOW_BOLT:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_SHADOW_BOLT, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_UNSTABLE_AFFLICTION:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_UNSTABLE_AFFLICTION, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_DEATH_COIL_WARL:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                        DoCast(target, SPELL_DEATH_COIL_WARL);
                    events.RepeatEvent(120000);
                    break;
                case EVENT_SPELL_SEED_OF_CORRUPTION:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, 3000);
                        values.AddSpellMod(SPELLVALUE_BASE_POINT1, 8000);
                        me->CastCustomSpell(SPELL_SEED_OF_CORRUPTION, values, target);
                    }

                    events.RepeatEvent(urand(10000, 20000));
                    EventMapGCD(events, 1500);
                    break;
            }
        }
    };
};

enum eMageSpells
{
    SPELL_ARCANE_BARRAGE   = 65799,
    SPELL_ARCANE_BLAST     = 65791,
    SPELL_ARCANE_EXPLOSION = 65800,
    SPELL_BLINK            = 65793,
    SPELL_COUNTERSPELL     = 65790,
    SPELL_FROST_NOVA       = 65792,
    SPELL_FROSTBOLT        = 65807,
    SPELL_ICE_BLOCK        = 65802,
    SPELL_POLYMORPH        = 65801,
};

enum eMageEvents
{
    EVENT_SPELL_ARCANE_BARRAGE_BLAST = 1,
    EVENT_SPELL_ARCANE_EXPLOSION,
    EVENT_SPELL_BLINK,
    EVENT_SPELL_COUNTERSPELL,
    EVENT_SPELL_FROSTBOLT,
    EVENT_SPELL_ICE_BLOCK,
    EVENT_SPELL_POLYMORPH,
    // Boost
    EVENT_SPELL_MIRROR_IMAGE
};

class npc_toc_mage : public CreatureScript
{
public:
    npc_toc_mage()
        : CreatureScript("npc_toc_mage")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_mageAI(pCreature);
    }

    struct npc_toc_mageAI : public boss_faction_championsAI
    {
        npc_toc_mageAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_RANGED)
        {
            SetEquipmentSlots(false, 47524, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_ARCANE_BARRAGE_BLAST, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_ARCANE_EXPLOSION, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_BLINK, 10000);
            events.RescheduleEvent(EVENT_SPELL_COUNTERSPELL, urand(10000, 20000));
            events.RescheduleEvent(EVENT_SPELL_FROSTBOLT, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_ICE_BLOCK, 10000);
            events.RescheduleEvent(EVENT_SPELL_POLYMORPH, urand(5000, 10000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_MIRROR_IMAGE, urand(5000, 15000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        void JustSummoned(Creature* summon) override
        {
            DoZoneInCombat(summon, 250.0f);
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_ARCANE_BARRAGE_BLAST:
                    if (auto victim = me->GetVictim())
                    {
                        if (!IsNonViableTarget(victim))
                        {
                            if (auto aura = me->GetAura(36032))
                            {
                                auto amount = std::min(aura->GetStackAmount(), static_cast<uint8>(4));
                                DoCast(victim, roll_chance_i(amount * 25) ? SPELL_ARCANE_BARRAGE : SPELL_ARCANE_BLAST);
                            }
                            else
                                DoCast(victim, SPELL_ARCANE_BLAST);

                            EventMapGCD(events, 1500);
                        }
                    }
                    events.RepeatEvent(urand(3000, 6000));
                    break;
                case EVENT_SPELL_ARCANE_EXPLOSION:
                    if (EnemiesInRange(9.0f) >= 3)
                    {
                        me->CastSpell((Unit*)nullptr, SPELL_ARCANE_EXPLOSION, false);
                        events.RepeatEvent(6000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_BLINK:
                    if (HealthBelowPct(50) && EnemiesInRange(10.0f) >= 3)
                    {
                        me->CastSpell((Unit*)nullptr, SPELL_FROST_NOVA, false);
                        events.RepeatEvent(15000);
                        EventMapGCD(events, 1500);
                        DoCastSelf(SPELL_BLINK);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_COUNTERSPELL:
                    if (Unit* target = SelectEnemyCaster(true, 30.0f))
                    {
                        me->CastSpell(target, SPELL_COUNTERSPELL, false);
                        events.RepeatEvent(24000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_FROSTBOLT:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_FROSTBOLT, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_ICE_BLOCK:
                    if (HealthBelowPct(25))
                    {
                        me->CastSpell(me, SPELL_ICE_BLOCK, false);
                        events.RepeatEvent(300000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_POLYMORPH:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && tar->IsPlayer() && !tar->HasAura(SPELL_POLYMORPH); }))
                        me->CastSpell(target, SPELL_POLYMORPH, false);
                    events.RepeatEvent(15000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_MIRROR_IMAGE:
                    DoCastSelf(SPELL_MIRROR_IMAGE);
                    events.RepeatEvent(urand(180000, 200000));
                    EventMapGCD(events, 1500);
                    break;
            }
        }
    };
};

enum eHunterSpells
{
    SPELL_AIMED_SHOT     = 65883,
    SPELL_DETERRENCE     = 65871,
    SPELL_DISENGAGE      = 65870,
    SPELL_EXPLOSIVE_SHOT = 65866,
    SPELL_FROST_TRAP     = 65880,
    SPELL_SHOOT          = 65868,
    SPELL_STEADY_SHOT    = 65867,
    SPELL_WING_CLIP      = 66207,
    SPELL_WYVERN_STING   = 65877,
    SPELL_CALL_PET       = 67777,
};

enum eHunterEvents
{
    EVENT_SPELL_AIMED_SHOT = 1,
    EVENT_SPELL_DETERRENCE,
    EVENT_SPELL_DISENGAGE,
    EVENT_SPELL_EXPLOSIVE_SHOT,
    EVENT_SPELL_FROST_TRAP,
    EVENT_SPELL_STEADY_SHOT,
    EVENT_SPELL_WING_CLIP,
    EVENT_SPELL_WYVERN_STING,
    EVENT_SPELL_CALL_PET,
    // Boost
    EVENT_SPELL_MULTI_SHOT
};

class npc_toc_hunter : public CreatureScript
{
public:
    npc_toc_hunter()
        : CreatureScript("npc_toc_hunter")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_hunterAI(pCreature);
    }

    struct npc_toc_hunterAI : public boss_faction_championsAI
    {
        npc_toc_hunterAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_RANGED)
        {
            SetEquipmentSlots(false, 47156, EQUIP_NO_CHANGE, 48711);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_AIMED_SHOT, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DETERRENCE, 10000);
            events.RescheduleEvent(EVENT_SPELL_DISENGAGE, 10000);
            events.RescheduleEvent(EVENT_SPELL_EXPLOSIVE_SHOT, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_FROST_TRAP, urand(15000, 20000));
            events.RescheduleEvent(EVENT_SPELL_STEADY_SHOT, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_WING_CLIP, 10000);
            events.RescheduleEvent(EVENT_SPELL_WYVERN_STING, urand(5000, 15000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_MULTI_SHOT, urand(5000, 15000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_RANGED) || IsCCed());
        }

        void JustDied(Unit* killer) override
        {
            boss_faction_championsAI::JustDied(killer);
            if (Creature* cat = GetCat())
                cat->DespawnOrUnsummon();
        }

        void DoAction(int32 actionId) override
        {
            if (actionId == 1)
            {
                Position pos = me->GetPosition();
                me->GetNearPosition(pos, 1.0f, float(M_PI / 2));
                DoSummon(35610, pos);
            }
        }

        Creature* GetCat()
        {
            return me->FindNearestCreature(35610, 250.0f);
        }

        void EnterCombat(Unit* who) override
        {
            boss_faction_championsAI::EnterCombat(who);
            if (Creature* cat = GetCat())
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    if (cat->IsAIEnabled)
                        cat->AI()->AttackStart(target);
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_CALL_PET:
                    DoSummon(35610, *me);
                    events.PopEvent();
                    break;
                case EVENT_SPELL_AIMED_SHOT:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_AIMED_SHOT, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_DETERRENCE:
                    if (HealthBelowPct(25))
                    {
                        me->CastSpell(me, SPELL_DETERRENCE, false);
                        events.RepeatEvent(90000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_DISENGAGE:
                    if (EnemiesInRange(10.0f) >= 3)
                    {
                        me->CastSpell(me, SPELL_DISENGAGE, false);
                        events.RepeatEvent(30000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_EXPLOSIVE_SHOT:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_EXPLOSIVE_SHOT, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FROST_TRAP:
                    me->CastSpell(me, SPELL_FROST_TRAP, false);
                    events.RepeatEvent(30000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_STEADY_SHOT:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_STEADY_SHOT, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_WING_CLIP:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) <= 5.0f)
                        me->CastSpell(me->GetVictim(), SPELL_WING_CLIP, false);
                    events.RepeatEvent(6000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_WYVERN_STING:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 35.0f) && !IsNonViableTarget(tar) && tar->IsPlayer() && !tar->HasAura(SPELL_WYVERN_STING); }))
                    {
                        me->CastSpell(target, SPELL_WYVERN_STING, false);
                        events.RepeatEvent(60000);
                        EventMapGCD(events, 1500);
                        break;
                    }
                    events.RepeatEvent(10000);
                    break;
                case EVENT_SPELL_MULTI_SHOT:
                    // TODO: Better target selector
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 35.0f, true))
                    {
                        DoCast(target, SPELL_MULTI_SHOT);
                        events.RepeatEvent(urand(15000, 30000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
            }

            if (me->GetVictim() && me->isAttackReady() && !me->HasUnitState(UNIT_STATE_CASTING) && !IsNonViableTarget(me->GetVictim()))
            {
                me->CastSpell(me->GetVictim(), SPELL_SHOOT, true);
                me->resetAttackTimer();
            }
        }
    };
};

enum eBoomkinSpells
{
    SPELL_WRATH            = 65862,
    SPELL_MOONFIRE         = 65856,
    SPELL_STARFIRE         = 65854,
    SPELL_INSECT_SWARM     = 65855,
    SPELL_ENTANGLING_ROOTS = 65857,
    SPELL_FAERIE_FIRE      = 65863,
    SPELL_CYCLONE          = 65859,
    SPELL_FORCE_OF_NATURE  = 65861,
};

enum eBoomkinEvents
{
    EVENT_SPELL_WRATH = 1,
    EVENT_SPELL_MOONFIRE,
    EVENT_SPELL_STARFIRE,
    EVENT_SPELL_INSECT_SWARM,
    EVENT_SPELL_ENTANGLING_ROOTS,
    EVENT_SPELL_FAERIE_FIRE,
    EVENT_SPELL_CYCLONE,
    EVENT_SPELL_FORCE_OF_NATURE,
    // Boost
    EVENT_SPELL_STARFALL
};

class npc_toc_boomkin : public CreatureScript
{
public:
    npc_toc_boomkin()
        : CreatureScript("npc_toc_boomkin")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_boomkinAI(pCreature);
    }

    struct npc_toc_boomkinAI : public boss_faction_championsAI
    {
        npc_toc_boomkinAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_RANGED)
        {
            SetEquipmentSlots(false, 50966, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_BARKSKIN, 10000);
            events.RescheduleEvent(EVENT_SPELL_WRATH, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_MOONFIRE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_STARFIRE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_INSECT_SWARM, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_ENTANGLING_ROOTS, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_FAERIE_FIRE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_CYCLONE, urand(10000, 15000));
            events.RescheduleEvent(EVENT_SPELL_FORCE_OF_NATURE, urand(20000, 40000));
            // boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_STARFALL, urand(5000, 30000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED) || IsCCed());
        }

        void JustSummoned(Creature* c) override
        {
            if (Unit* target = c->SelectNearestTarget(200.0f))
                c->AI()->AttackStart(target);
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_BARKSKIN:
                    if (HealthBelowPct(50))
                    {
                        me->CastSpell(me, SPELL_BARKSKIN, false);
                        events.RepeatEvent(60000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_WRATH:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_WRATH, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_MOONFIRE:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_MOONFIRE, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_STARFIRE:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_STARFIRE, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_INSECT_SWARM:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_INSECT_SWARM, false);
                    events.RepeatEvent(urand(5000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_ENTANGLING_ROOTS:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && tar->IsPlayer() && !tar->HasAura(SPELL_ENTANGLING_ROOTS); }))
                        me->CastSpell(target, SPELL_ENTANGLING_ROOTS, false);
                    events.RepeatEvent(urand(10000, 15000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FAERIE_FIRE:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                        me->CastSpell(me->GetVictim(), SPELL_FAERIE_FIRE, false);
                    events.RepeatEvent(urand(15000, 20000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_CYCLONE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !IsNonViableTarget(tar) && tar->IsPlayer() && !tar->HasAura(SPELL_CYCLONE); }))
                        me->CastSpell(target, SPELL_CYCLONE, false);
                    events.RepeatEvent(urand(25000, 40000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FORCE_OF_NATURE:
                    me->CastSpell((Unit*)nullptr, SPELL_FORCE_OF_NATURE, false);
                    events.RepeatEvent(180000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_STARFALL:
                    DoCastSelf(SPELL_STARFALL);
                    events.RepeatEvent(urand(90000, 120000));
                    EventMapGCD(events, 1500);
                    break;
            }
        }
    };
};

enum eWarriorSpells
{
    SPELL_BLADESTORM         = 65947,
    SPELL_INTIMIDATING_SHOUT = 65930,
    SPELL_MORTAL_STRIKE      = 65926,
    SPELL_CHARGE             = 68764,
    SPELL_DISARM             = 65935,
    SPELL_OVERPOWER          = 65924,
    SPELL_SUNDER_ARMOR       = 65936,
    SPELL_SHATTERING_THROW   = 65940,
    SPELL_RETALIATION        = 65932,
};

enum eWarriorEvents
{
    EVENT_SPELL_BLADESTORM = 1,
    EVENT_SPELL_INTIMIDATING_SHOUT,
    EVENT_SPELL_MORTAL_STRIKE,
    EVENT_SPELL_CHARGE,
    EVENT_SPELL_DISARM,
    EVENT_SPELL_OVERPOWER,
    EVENT_SPELL_SUNDER_ARMOR,
    EVENT_SPELL_SHATTERING_THROW,
    EVENT_SPELL_RETALIATION,
    // Boost
    EVENT_SPELL_SHOCKWAVE
};

class npc_toc_warrior : public CreatureScript
{
public:
    npc_toc_warrior()
        : CreatureScript("npc_toc_warrior")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_warriorAI(pCreature);
    }

    struct npc_toc_warriorAI : public boss_faction_championsAI
    {
        npc_toc_warriorAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_MELEE)
        {
            SetEquipmentSlots(false, 47427, 46964, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_BLADESTORM, 20000);
            events.RescheduleEvent(EVENT_SPELL_INTIMIDATING_SHOUT, 14000);
            events.RescheduleEvent(EVENT_SPELL_MORTAL_STRIKE, urand(5000, 10000));
            events.RescheduleEvent(EVENT_SPELL_CHARGE, 3000);
            events.RescheduleEvent(EVENT_SPELL_DISARM, urand(15000, 25000));
            events.RescheduleEvent(EVENT_SPELL_OVERPOWER, urand(5000, 10000));
            events.RescheduleEvent(EVENT_SPELL_SUNDER_ARMOR, urand(5000, 10000));
            events.RescheduleEvent(EVENT_SPELL_SHATTERING_THROW, urand(25000, 40000));
            events.RescheduleEvent(EVENT_SPELL_RETALIATION, urand(25000, 40000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_SHOCKWAVE, 5s, 20s);
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_BLADESTORM:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (EnemiesInRange(8.0f) >= 3)
                    {
                        me->CastSpell(me, SPELL_BLADESTORM, false);
                        events.RepeatEvent(90000);
                        events.DelayEvents(9000);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_INTIMIDATING_SHOUT:
                    if (EnemiesInRange(8.0f) >= 3)
                    {
                        me->CastSpell((Unit*)nullptr, SPELL_INTIMIDATING_SHOUT, false);
                        events.RepeatEvent(120000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_MORTAL_STRIKE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim())
                        me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE, false);
                    events.RepeatEvent(urand(6000, 8000));
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_CHARGE:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) > 8.0f && me->GetDistance2d(me->GetVictim()) < 25.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_CHARGE, false);
                        events.RepeatEvent(10000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_DISARM:
                {
                    std::vector<Player*> targets;
                    auto const& pl = pInstance->instance->GetPlayers();
                    for (auto itr = pl.begin(); itr != pl.end(); ++itr)
                        if (Player* player = itr->GetSource())
                            if (player->getClass() == CLASS_WARRIOR || player->getClass() == CLASS_DEATH_KNIGHT || player->getClass() == CLASS_PALADIN || player->getClass() == CLASS_HUNTER || player->getClass() == CLASS_ROGUE)
                                targets.push_back(player);

                    if (targets.empty())
                    {
                        events.RepeatEvent(5000);
                        break;
                    }

                    if (Player* target = Trinity::Containers::SelectRandomContainerElement(targets))
                    {
                        DoCast(target, SPELL_DISARM);
                        events.RepeatEvent(60000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                }
                case EVENT_SPELL_OVERPOWER:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) < 5.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_OVERPOWER, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_SUNDER_ARMOR:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) < 5.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_SUNDER_ARMOR, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_SHATTERING_THROW:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) < 25.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_SHATTERING_THROW, false);
                        events.RepeatEvent(300000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_RETALIATION:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (EnemiesInRange(8.0f) >= 3)
                    {
                        me->CastSpell(me, SPELL_RETALIATION, false);
                        events.RepeatEvent(300000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_SHOCKWAVE:
                    std::vector<Unit*> targets = GetEnemiesWithPredicate([this](Unit const* target) { return me->isInFront(target, M_PI / 4.0f) && me->IsInRange(target, 0.0f, 10.0f); });
                    if (targets.empty())
                    {
                        events.RepeatEvent(5000);
                        break;
                    }

                    me->SetFacingToObject(targets.front());
                    me->CastCustomSpell(SPELL_SHOCKWAVE, SPELLVALUE_BASE_POINT0, 12000, nullptr, TRIGGERED_FULL_MASK);
                    events.RepeatEvent(25000);
                    EventMapGCD(events, 1500);
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };
};

enum eDeathKnightSpells
{
    SPELL_CHAINS_OF_ICE      = 66020,
    SPELL_DEATH_COIL         = 66019,
    SPELL_DEATH_GRIP         = 66017,
    SPELL_FROST_STRIKE       = 66047,
    SPELL_ICEBOUND_FORTITUDE = 66023,
    SPELL_ICY_TOUCH          = 66021,
    SPELL_STRANGULATE        = 66018,
};

enum eDeathKnightEvents
{
    EVENT_SPELL_CHAINS_OF_ICE = 1,
    EVENT_SPELL_DEATH_COIL,
    EVENT_SPELL_DEATH_GRIP,
    EVENT_SPELL_FROST_STRIKE,
    EVENT_SPELL_ICEBOUND_FORTITUDE,
    EVENT_SPELL_ICY_TOUCH,
    EVENT_SPELL_STRANGULATE,
    EVENT_SPELL_DEATH_AND_DECAY
};

class npc_toc_dk : public CreatureScript
{
public:
    npc_toc_dk()
        : CreatureScript("npc_toc_dk")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_dkAI(pCreature);
    }

    struct npc_toc_dkAI : public boss_faction_championsAI
    {
        npc_toc_dkAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_MELEE)
        {
            SetEquipmentSlots(false, 47518, 51021, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_CHAINS_OF_ICE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DEATH_COIL, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DEATH_GRIP, 0);
            events.RescheduleEvent(EVENT_SPELL_FROST_STRIKE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_ICEBOUND_FORTITUDE, 10000);
            events.RescheduleEvent(EVENT_SPELL_ICY_TOUCH, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_STRANGULATE, urand(20000, 30000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_DEATH_AND_DECAY, 10s, 25s);
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_CHAINS_OF_ICE:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) <= 25.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_CHAINS_OF_ICE, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_DEATH_COIL:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) <= 30.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_DEATH_COIL, false);
                        events.RepeatEvent(urand(5000, 8000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_DEATH_GRIP:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) <= 30.0f && me->GetDistance2d(me->GetVictim()) >= 12.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_DEATH_GRIP, false);
                        Position pos;
                        float x, y, z;
                        me->GetClosePoint(x, y, z, 3.0f);
                        pos.Relocate(x, y, z);
                        me->GetVictim()->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 49575, true);
                        events.RepeatEvent(20000);
                        EventMapGCD(events, 2000);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_FROST_STRIKE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) < 5.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_FROST_STRIKE, false);
                        events.RepeatEvent(urand(6000, 10000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_ICEBOUND_FORTITUDE:
                    if (HealthBelowPct(50))
                    {
                        me->CastSpell(me, SPELL_ICEBOUND_FORTITUDE, false);
                        events.RepeatEvent(60000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_ICY_TOUCH:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) < 20.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_ICY_TOUCH, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_STRANGULATE:
                    if (Unit* target = SelectEnemyCaster(false, 30.0f))
                    {
                        me->CastSpell(target, SPELL_STRANGULATE, false);
                        events.RepeatEvent(120000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_DEATH_AND_DECAY:
                    // TODO: Better target selector
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                    {
                        me->CastCustomSpell(SPELL_DEATH_AND_DECAY, SPELLVALUE_BASE_POINT0, 4000, target);
                        events.RepeatEvent(20000);
                        EventMapGCD(events, 1500);
                    }
                    break;
                default:
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };
};

enum eRogueSpells
{
    SPELL_FAN_OF_KNIVES = 65955,
    SPELL_BLIND         = 65960,
    SPELL_CLOAK         = 65961,
    SPELL_BLADE_FLURRY  = 65956,
    SPELL_SHADOWSTEP    = 66178,
    SPELL_HEMORRHAGE    = 65954,
    SPELL_EVISCERATE    = 65957,
    SPELL_WOUND_POISON  = 65962,
};

enum eRogueEvents
{
    EVENT_SPELL_FAN_OF_KNIVES = 1,
    EVENT_SPELL_BLIND,
    EVENT_SPELL_CLOAK,
    EVENT_SPELL_BLADE_FLURRY,
    EVENT_SPELL_SHADOWSTEP,
    EVENT_SPELL_HEMORRHAGE,
    EVENT_SPELL_EVISCERATE,
    // Boost
    EVENT_SPELL_KILLING_SPREE
};

class npc_toc_rogue : public CreatureScript
{
public:
    npc_toc_rogue()
        : CreatureScript("npc_toc_rogue")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_rogueAI(pCreature);
    }

    struct npc_toc_rogueAI : public boss_faction_championsAI
    {
        npc_toc_rogueAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_MELEE)
        {
            SetEquipmentSlots(false, 47422, 49982, EQUIP_NO_CHANGE);
            me->setPowerType(POWER_ENERGY);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_FAN_OF_KNIVES, 10000);
            events.RescheduleEvent(EVENT_SPELL_BLIND, urand(10000, 15000));
            events.RescheduleEvent(EVENT_SPELL_CLOAK, 10000);
            events.RescheduleEvent(EVENT_SPELL_BLADE_FLURRY, urand(20000, 40000));
            events.RescheduleEvent(EVENT_SPELL_SHADOWSTEP, urand(15000, 25000));
            events.RescheduleEvent(EVENT_SPELL_HEMORRHAGE, urand(3000, 5000));
            events.RescheduleEvent(EVENT_SPELL_EVISCERATE, urand(20000, 25000));
            // boost
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_KILLING_SPREE, urand(10000, 20000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
        }

        void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
        {
            if (damageType == DIRECT_DAMAGE && roll_chance_i(65) && me->GetVictim())
                DoCast(victim, SPELL_WOUND_POISON, true);
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_FAN_OF_KNIVES:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (EnemiesInRange(10.0f) >= 3)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_FAN_OF_KNIVES, false);
                        events.RepeatEvent(urand(6000, 10000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_BLIND:
                    if (Unit* target = SelectTarget(SELECT_TARGET_BOTTOMAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !IsNonViableTarget(tar) && tar->IsPlayer() && !tar->HasAura(SPELL_BLIND); }))
                    {
                        me->CastSpell(target, SPELL_BLIND, false);
                        events.RepeatEvent(120000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_CLOAK:
                    if (HealthBelowPct(50))
                    {
                        me->CastSpell(me, SPELL_CLOAK, false);
                        events.RepeatEvent(90000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(6000);
                    break;
                case EVENT_SPELL_BLADE_FLURRY:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                        events.RepeatEvent(5000);
                    else
                    {
                        me->CastSpell(me, SPELL_BLADE_FLURRY, false);
                        events.RepeatEvent(120000);
                        EventMapGCD(events, 1500);
                    }
                    break;
                case EVENT_SPELL_SHADOWSTEP:
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->IsInRange(me->GetVictim(), 10.0f, 40.0f))
                    {
                        me->CastSpell(me->GetVictim(), SPELL_SHADOWSTEP, false);
                        events.RepeatEvent(20000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_HEMORRHAGE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) <= 5.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_HEMORRHAGE, false);
                        events.RepeatEvent(5000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_EVISCERATE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()) && me->GetDistance2d(me->GetVictim()) <= 5.0f)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_EVISCERATE, false);
                        events.RepeatEvent(urand(15000, 25000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_KILLING_SPREE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                        events.RepeatEvent(5000);
                    else if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                    {
                        DoCastVictim(SPELL_KILLING_SPREE, true);
                        events.RepeatEvent(40000);
                        EventMapGCD(events, 1500);
                    }
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };
};

class spell_fan_of_knives_fc_SpellScript : public SpellScript
{
    PrepareSpellScript(spell_fan_of_knives_fc_SpellScript);

    void HandleOnHit()
    {
        if (Unit* target = GetExplTargetUnit())
            if (roll_chance_i(65))
                GetCaster()->CastSpell(target, SPELL_WOUND_POISON, false);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_fan_of_knives_fc_SpellScript::HandleOnHit);
    }
};

enum eEnhShamanSpells
{
    SPELL_EARTH_SHOCK_ENH  = 65973,
    SPELL_LAVA_LASH        = 65974,
    SPELL_STORMSTRIKE      = 65970,
    SPELL_WINDFURY         = 65975,
    SPELL_MAELSTROM_WEAPON = 65988,
};

enum eEnhShamanEvents
{
    EVENT_SPELL_EARTH_SHOCK_ENH = 201,
    EVENT_SPELL_LAVA_LASH,
    EVENT_SPELL_STORMSTRIKE,
    EVENT_SUMMON_TOTEM,
    EVENT_SUMMON_FERAL_SPIRITS
};

class npc_toc_enh_shaman : public CreatureScript
{
public:
    npc_toc_enh_shaman()
        : CreatureScript("npc_toc_enh_shaman")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_enh_shamanAI(pCreature);
    }

    struct npc_toc_enh_shamanAI : npc_toc_shaman_baseAI
    {
        npc_toc_enh_shamanAI(Creature* pCreature)
            : npc_toc_shaman_baseAI(pCreature, SHAMAN_ENHANCEMENT)
        {
            SetEquipmentSlots(false, 51803, 48013, EQUIP_NO_CHANGE);
            me->SetModifierValue(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, 1.0f);
            me->UpdateDamagePhysical(OFF_ATTACK);
            DoCastSelf(SPELL_WINDFURY);
            DoCastSelf(SPELL_MAELSTROM_WEAPON);

            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_HEROISM_OR_BLOODLUST, urand(25000, 40000));
            events.RescheduleEvent(EVENT_SPELL_EARTH_SHOCK_ENH, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_LAVA_LASH, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_STORMSTRIKE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SUMMON_TOTEM, urand(10000, 20000));
            events.RescheduleEvent(151, urand(1000, 3000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SUMMON_FERAL_SPIRITS, urand(5000, 20000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_EARTH_SHOCK_ENH:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 25.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_EARTH_SHOCK_ENH, false);
                        events.RepeatEvent(urand(6000, 8000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_LAVA_LASH:
                    if (me->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_OFFHAND))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 5.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        int32 dmg = RAID_MODE(urand(6000, 8000), urand(10000, 12000), urand(10000, 12000), urand(11000, 14000));
                        me->CastCustomSpell(target, SPELL_LAVA_LASH, &dmg, nullptr, nullptr, false);
                        events.RepeatEvent(urand(6000, 8000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_STORMSTRIKE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED) && me->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_OFFHAND))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 5.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_STORMSTRIKE, false);
                        events.RepeatEvent(urand(8000, 9000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_HEROISM_OR_BLOODLUST:
                    if (me->GetEntry() == NPC_ALLIANCE_SHAMAN_RESTORATION)
                        me->CastSpell((Unit*)nullptr, SPELL_HEROISM, true);
                    else
                        me->CastSpell((Unit*)nullptr, SPELL_BLOODLUST, true);
                    events.RepeatEvent(600000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SUMMON_TOTEM:
                    me->CastSpell((Unit*)nullptr, RAND(SPELL_GROUNDING_TOTEM, SPELL_WINDFURY_TOTEM, SPELL_TREMOR_TOTEM), false);
                    events.RepeatEvent(30000);
                    EventMapGCD(events, 1500);
                    break;
                case 151:
                    HandleTotems();
                    EventMapGCD(events, 1500);
                    events.RepeatEvent(urand(40000, 60000));
                    break;
                case EVENT_SUMMON_FERAL_SPIRITS:
                {
                    Position pos = me->GetPosition();
                    me->GetNearPosition(pos, 2.0f, float(M_PI / 2));
                    DoSummon(NPC_BOOST_FERAL_SPIRIT, pos, 30000, TEMPSUMMON_TIMED_DESPAWN);
                    me->GetNearPosition(pos, 2.0f, float(-M_PI / 2));
                    DoSummon(NPC_BOOST_FERAL_SPIRIT, pos, 30000, TEMPSUMMON_TIMED_DESPAWN);
                    EventMapGCD(events, 1500);
                    events.RepeatEvent(urand(80000, 85000));
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };
};

enum eRetroPaladinSpells
{
    SPELL_AVENGING_WRATH         = 66011,
    SPELL_CRUSADER_STRIKE        = 66003,
    SPELL_DIVINE_SHIELD          = 66010,
    SPELL_DIVINE_STORM           = 66006,
    SPELL_HAMMER_OF_JUSTICE_RET  = 66007,
    SPELL_HAND_OF_PROTECTION_RET = 66009,
    SPELL_JUDGEMENT_OF_COMMAND   = 66005,
    SPELL_REPENTANCE             = 66008,
    SPELL_SEAL_OF_COMMAND        = 66004,
};

enum eRetroPaladinEvents
{
    EVENT_SPELL_AVENGING_WRATH = 1,
    EVENT_SPELL_CRUSADER_STRIKE,
    EVENT_SPELL_DIVINE_SHIELD,
    EVENT_SPELL_DIVINE_STORM,
    EVENT_SPELL_HAMMER_OF_JUSTICE_RET,
    EVENT_SPELL_HAND_OF_PROTECTION_RET,
    EVENT_SPELL_JUDGEMENT_OF_COMMAND,
    EVENT_SPELL_REPENTANCE,
    // Boost
    EVENT_SPELL_CONSECRATION
};

class npc_toc_retro_paladin : public CreatureScript
{
public:
    npc_toc_retro_paladin()
        : CreatureScript("npc_toc_retro_paladin")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new npc_toc_retro_paladinAI(pCreature);
    }

    struct npc_toc_retro_paladinAI : public boss_faction_championsAI
    {
        npc_toc_retro_paladinAI(Creature* pCreature)
            : boss_faction_championsAI(pCreature, AI_MELEE)
        {
            SetEquipmentSlots(false, 47519, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            events.Reset();
            events.RescheduleEvent(EVENT_SPELL_AVENGING_WRATH, urand(20000, 30000));
            events.RescheduleEvent(EVENT_SPELL_CRUSADER_STRIKE, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_DIVINE_SHIELD, 10000);
            events.RescheduleEvent(EVENT_SPELL_DIVINE_STORM, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_HAMMER_OF_JUSTICE_RET, urand(15000, 25000));
            events.RescheduleEvent(EVENT_SPELL_HAND_OF_PROTECTION_RET, urand(25000, 40000));
            events.RescheduleEvent(EVENT_SPELL_JUDGEMENT_OF_COMMAND, urand(3000, 10000));
            events.RescheduleEvent(EVENT_SPELL_REPENTANCE, urand(10000, 15000));
            if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
                events.RescheduleEvent(EVENT_SPELL_CONSECRATION, urand(2000, 10000));
        }

        EventMap events;

        bool myCanCast()
        {
            return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
        }

        void UpdateAI(uint32 diff) override
        {
            boss_faction_championsAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (!myCanCast())
                return;

            switch (events.GetEvent())
            {
                case 0:
                    break;
                case EVENT_SPELL_AVENGING_WRATH:
                    me->CastSpell(me, SPELL_AVENGING_WRATH, false);
                    events.RepeatEvent(180000);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_CRUSADER_STRIKE:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 5.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_CRUSADER_STRIKE, false);
                        events.RepeatEvent(urand(6000, 8000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_DIVINE_SHIELD:
                    if (HealthBelowPct(25))
                    {
                        me->CastSpell(me, SPELL_DIVINE_SHIELD, false);
                        events.RepeatEvent(300000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_DIVINE_STORM:
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
                    {
                        events.RepeatEvent(5000);
                        break;
                    }
                    if (EnemiesInRange(5.0f) >= 3)
                    {
                        me->CastSpell((Unit*)nullptr, SPELL_DIVINE_STORM, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_HAMMER_OF_JUSTICE_RET:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 15.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_HAMMER_OF_JUSTICE_RET, false);
                        events.RepeatEvent(40000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_HAND_OF_PROTECTION_RET:
                {
                    if (Unit* target = GetHighestAttackerCount(me, DoFindFriendlyMissingBuff(30.0f, SPELL_HAND_OF_PROTECTION_RET)))
                    {
                        EventMapGCD(events, 1500);
                        DoCast(target, SPELL_HAND_OF_PROTECTION_RET);
                        events.RepeatEvent(300000);
                    }
                    else
                        events.RepeatEvent(5000);
                }
                case EVENT_SPELL_JUDGEMENT_OF_COMMAND:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_JUDGEMENT_OF_COMMAND, false);
                        events.RepeatEvent(urand(10000, 15000));
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_REPENTANCE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_BOTTOMAGGRO, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastSpell(target, SPELL_REPENTANCE, false);
                        events.RepeatEvent(60000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
                case EVENT_SPELL_CONSECRATION:
                    if (EnemiesInRange(8.0f) >= 3)
                    {
                        me->CastCustomSpell(SPELL_CONSECRATION, SPELLVALUE_BASE_POINT0, 5000, me);
                        events.RepeatEvent(12000);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.RepeatEvent(5000);
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };
};

enum eWarlockPetSpells
{
    SPELL_DEVOUR_MAGIC = 67518,
    SPELL_SPELL_LOCK   = 67519,
};

enum eWarlockPetEvents
{
    EVENT_SPELL_DEVOUR_MAGIC = 1,
    EVENT_SPELL_SPELL_LOCK,
    EVENT_CHANGE_TARGET_WARL_PET
};

struct npc_toc_pet_warlock : public npc_toc_pet_baseAI
{
    npc_toc_pet_warlock(Creature* creature)
        : npc_toc_pet_baseAI(creature)
    {
    }

    void ScheduleCombatEvents() override
    {
        events.RescheduleEvent(EVENT_SPELL_DEVOUR_MAGIC, 5s, 15s);
        events.RescheduleEvent(EVENT_SPELL_SPELL_LOCK, 5s, 15s);
        events.RescheduleEvent(EVENT_CHANGE_TARGET_WARL_PET, 15s, 25s);
    }

    void DoEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case EVENT_SPELL_DEVOUR_MAGIC:
                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [this](Unit* tar) { return tar->IsPlayer() && tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar); }))
                {
                    DoCast(target, SPELL_DEVOUR_MAGIC);
                    events.Repeat(8s, 15s);
                    EventMapGCD(events, 1500);
                }
                else
                    events.Repeat(5s);
                break;
            case EVENT_SPELL_SPELL_LOCK:
                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [this](Unit* tar) { return tar->IsPlayer() && tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && (tar->IsCasting() || roll_chance_i(20)); }))
                {
                    DoCast(target, SPELL_SPELL_LOCK);
                    events.Repeat(24s);
                    EventMapGCD(events, 1500);
                }
                else
                    events.Repeat(5s);
                break;
            case EVENT_CHANGE_TARGET_WARL_PET:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) { return tar->IsPlayer() && !IsNonViableTarget(tar); }))
                    AttackStart(target);
                events.Repeat(15s, 25s);
                break;
            default:
                break;
        }
    }
};

enum eHunterPetSpells
{
    SPELL_CLAW = 67793,
};

enum eHunterPetEvents
{
    EVENT_SPELL_CLAW = 1,
    EVENT_CHANGE_TARGET_HUNTER_PET
};

struct npc_toc_pet_hunter : public npc_toc_pet_baseAI
{
    npc_toc_pet_hunter(Creature* creature)
        : npc_toc_pet_baseAI(creature)
    {
    }

    void ScheduleCombatEvents() override
    {
        events.RescheduleEvent(EVENT_SPELL_CLAW, 5s, 15s);
        events.RescheduleEvent(EVENT_CHANGE_TARGET_HUNTER_PET, 15s, 25s);
    }

    bool CanCast() override
    {
        return !IsCCed();
    }

    void DoEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case EVENT_SPELL_CLAW:
                if (me->GetVictim() && !IsNonViableTarget(me->GetVictim()))
                {
                    DoCast(me->GetVictim(), SPELL_CLAW);
                    events.Repeat(8s, 15s);
                    EventMapGCD(events, 1500);
                }
                else
                    events.Repeat(2s);
                break;
            case EVENT_CHANGE_TARGET_HUNTER_PET:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) { return tar->IsPlayer() && !IsNonViableTarget(tar); }))
                    AttackStart(target);
                events.Repeat(15s, 25s);
                break;
            default:
                break;
        }
    }
};

class go_toc_champions_cache : public GameObjectScript
{
public:
    go_toc_champions_cache()
        : GameObjectScript("go_toc_champions_cache")
    {
    }

    bool OnGossipHello(Player* plr, GameObject* go) override
    {
        if (plr->IsGameMaster())
            return false;

        if (!go->loot.items.size())
            return false;

        uint8 invalidCount = 0;
        for (std::vector<LootItem>::iterator itr = go->loot.items.begin(); itr != go->loot.items.end(); ++itr)
            if (ItemTemplate const* iProto = sObjectMgr->GetItemTemplate((*itr).itemid))
                if (((iProto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY) && plr->GetTeam() != TEAM_HORDE) || ((iProto->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY) && plr->GetTeam() != TEAM_ALLIANCE))
                    if (!((*itr).is_looted))
                    {
                        (*itr).count     = 0;
                        (*itr).is_looted = true;
                        if (go->loot.unlootedCount)
                            --go->loot.unlootedCount;
                    }

        return false;
    }
};

class spell_faction_champion_warl_unstable_affliction : public SpellScriptLoader
{
public:
    spell_faction_champion_warl_unstable_affliction()
        : SpellScriptLoader("spell_faction_champion_warl_unstable_affliction")
    {
    }

    class spell_faction_champion_warl_unstable_affliction_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_faction_champion_warl_unstable_affliction_AuraScript);

        bool Validate(SpellInfo const* /*spell*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_AFFLICTION_DISPEL))
                return false;
            return true;
        }

        void HandleDispel(DispelInfo* dispelInfo)
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(dispelInfo->GetDispeller(), SPELL_UNSTABLE_AFFLICTION_DISPEL, true, nullptr, GetEffect(EFFECT_0));
        }

        void Register() override
        {
            AfterDispel += AuraDispelFn(spell_faction_champion_warl_unstable_affliction_AuraScript::HandleDispel);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_faction_champion_warl_unstable_affliction_AuraScript();
    }
};

struct npc_searing_totem_factionsAI : public ScriptedAI
{
    npc_searing_totem_factionsAI(Creature* creature)
        : ScriptedAI(creature)
    {
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (Player* target = SelectTargetFromPlayerList(15.0f, 0, true))
            DoCast(target, 65998);
    }

    void EnterEvadeMode() override
    {
        me->DespawnOrUnsummon();
    }
};

struct npc_healing_stream_totem_factionsAI : public ScriptedAI
{
    npc_healing_stream_totem_factionsAI(Creature* creature)
        : ScriptedAI(creature)
    {
        events.Reset();
        events.RescheduleEvent(1, 2000);
    }

    EventMap events;

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case 1:
                    DoCastAOE(65993);
                    events.Repeat(2000);
                    break;
                default:
                    break;
            }
        }
    }

    void EnterEvadeMode() override
    {
        me->DespawnOrUnsummon();
    }
};

enum Lifebloom
{
    SPELL_LIFEBLOOM_TRIGGER = 66094
};

// 66093 - Lifebloom
class spell_faction_champions_lifebloom : public AuraScript
{
    PrepareAuraScript(spell_faction_champions_lifebloom);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_LIFEBLOOM_TRIGGER });
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode == AURA_REMOVE_BY_EXPIRE || removeMode == AURA_REMOVE_BY_ENEMY_SPELL)
        {
            uint8 stackAmount          = GetTargetApplication()->GetBase()->GetStackAmount();
            Unit* caster               = GetCaster();
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_LIFEBLOOM_TRIGGER);
            uint32 value               = spellInfo->Effects[EFFECT_0].CalcValue(caster, nullptr, caster);
            caster->CastCustomSpell(SPELL_LIFEBLOOM_TRIGGER, SPELLVALUE_BASE_POINT0, value * stackAmount, caster, TRIGGERED_FULL_MASK, nullptr, aurEff);
        }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_faction_champions_lifebloom::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }
};

/*

        BOOST SCRIPTS

*/
enum BeastMasterSpells
{
    SPELL_ARCANE_SHOT  = 3044,
    SPELL_DEFEND       = 63896,
    SPELL_INTIMIDATION = 24394,
    SPELL_BEAST_WITHIN = 38373
};

enum BeastMasterEvents
{
    EVENT_SUMMON_PET = 1,
    EVENT_SPELL_BM_FROST_TRAP,
    EVENT_SPELL_BM_ARCANE_SHOT,
    EVENT_SPELL_BM_DETERRENCE,
    EVENT_SPELL_BM_DISENGAGE,
    EVENT_SPELL_BM_WING_CLIP,
    EVENT_SPELL_BM_INTIMIDATION,
    EVENT_SPELL_BM_BEAST_WITHIN
};

struct npc_toc_beast_master_hunter : public boss_faction_championsAI
{
    npc_toc_beast_master_hunter(Creature* creature)
        : boss_faction_championsAI(creature, AI_RANGED)
    {
        SetEquipmentSlots(false, 47156, EQUIP_NO_CHANGE, 48711);
        events.Reset();
        events.RescheduleEvent(EVENT_SPELL_BM_FROST_TRAP, 15s, 20s);
        events.RescheduleEvent(EVENT_SPELL_BM_ARCANE_SHOT, 5s, 10s);
        events.RescheduleEvent(EVENT_SPELL_BM_DETERRENCE, 10s);
        events.RescheduleEvent(EVENT_SPELL_BM_DISENGAGE, 10s);
        events.RescheduleEvent(EVENT_SPELL_BM_WING_CLIP, 10s);
        events.RescheduleEvent(EVENT_SPELL_BM_INTIMIDATION, 5s, 45s);
        events.RescheduleEvent(EVENT_SPELL_BM_BEAST_WITHIN, 10s, 25s);
    }

    EventMap events;

    bool CanCast()
    {
        return !(me->IsCasting() || me->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_RANGED) || IsCCed());
    }

    void JustDied(Unit* killer) override
    {
        boss_faction_championsAI::JustDied(killer);
        for (Creature* pet : GetAllPets())
            pet->DespawnOrUnsummon();
    }
   
    void EnterEvadeMode() override
    {
        boss_faction_championsAI::EnterEvadeMode();
        for (Creature* pet : GetAllPets())
            pet->DespawnOrUnsummon();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == 1)
        {
            Position pos = me->GetPosition();
            me->GetNearPosition(pos, 2.0f, float(M_PI / 2));
            DoSummon(NPC_BOOST_LOQUENAHAK, pos);
            me->GetNearPosition(pos, 2.0f, float(-M_PI / 2));
            DoSummon(NPC_BOOST_KING_KRUSH, pos);
        }
    }
    void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) override
    {
        if (GetAllPets().empty())
            me->RemoveAurasDueToSpell(SPELL_DEFEND);
    }

    std::vector<Creature*> GetAllPets() const
    {
        std::vector<Creature*> pets;
        for (uint32 entry : { NPC_BOOST_LOQUENAHAK, NPC_BOOST_KING_KRUSH })
            if (Creature* pet = me->FindNearestCreature(entry, 250.0f))
                pets.emplace_back(pet);

        return pets;
    }

    void EnterCombat(Unit* who) override
    {
        me->CastCustomSpell(SPELL_DEFEND, SPELLVALUE_AURA_DURATION, -1, me, TRIGGERED_FULL_MASK);
        boss_faction_championsAI::EnterCombat(who);
        for (Creature* pet : GetAllPets())
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                if (pet->IsAIEnabled)
                    pet->AI()->AttackStart(target);
    }

    void UpdateAI(uint32 diff) override
    {
        boss_faction_championsAI::UpdateAI(diff);
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (!CanCast())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPELL_BM_FROST_TRAP:
                    DoCastSelf(SPELL_FROST_TRAP);
                    events.Repeat(30s);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_BM_ARCANE_SHOT:
                    if (Unit* victim = me->GetVictim())
                    {
                        me->CastCustomSpell(SPELL_ARCANE_SHOT, SPELLVALUE_BASE_POINT0, 13000, victim);
                        events.Repeat(6s);
                        EventMapGCD(events, 1500);
                        break;
                    }
                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_BM_DETERRENCE:
                    if (HealthBelowPct(25))
                    {
                        DoCastSelf(SPELL_DETERRENCE);
                        events.Repeat(1.5min);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(6s);
                    break;
                case EVENT_SPELL_BM_DISENGAGE:
                    if (EnemiesInRange(10.0f) >= 3)
                    {
                        DoCastSelf(SPELL_DISENGAGE);
                        events.Repeat(30s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(6s);
                    break;
                case EVENT_SPELL_BM_WING_CLIP:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, [this](Unit* tar) { return !IsNonViableTarget(tar) && me->GetDistance2d(tar) <= 5.0f; }))
                    {
                        DoCast(target, SPELL_WING_CLIP);
                        events.Repeat(6s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_BM_INTIMIDATION:
                {
                    auto pets = GetAllPets();
                    if (!pets.empty())
                    {
                        Creature* pet = Trinity::Containers::SelectRandomContainerElement(pets);
                        if (Unit* victim = pet->GetVictim())
                        {
                            pet->CastSpell(victim, SPELL_INTIMIDATION, true);
                            events.Repeat(1.0min, 1.2min);
                            EventMapGCD(events, 1500);
                            break;
                        }

                    }

                    events.Repeat(5s);
                    break;
                }
                case EVENT_SPELL_BM_BEAST_WITHIN:
                {
                    CustomSpellValues values;
                    values.AddSpellMod(SPELLVALUE_AURA_DURATION, 10000);
                    values.AddSpellMod(SPELLVALUE_BASE_POINT1, 50);

                    bool casted = false;
                    for (Creature* pet : GetAllPets())
                    {
                        casted = true;
                        pet->CastCustomSpell(SPELL_BEAST_WITHIN, values, pet, TRIGGERED_FULL_MASK);
                    }

                    if (casted)
                    {
                        events.Repeat(50s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                }
                default:
                    break;
            }

            if (!CanCast())
                return;
        }

        if (me->GetVictim() && me->isAttackReady() && !me->IsCasting() && !IsNonViableTarget(me->GetVictim()))
        {
            DoCastVictim(SPELL_SHOOT, true);
            me->resetAttackTimer();
        }
    }
};

enum LoquenahakSpells
{
    SPELL_LOQUENAHAK_CLAW          = 52472,
    SPELL_LOQUENAHAK_SPIRIT_STRIKE = 61198
};

enum LoquenahakEvents
{
    EVENT_LOQUENAHAK_CLAW = 1,
    EVENT_LOQUENAHAK_SPIRIT_STRIKE
};

struct npc_toc_beast_master_pet_baseAI : public npc_toc_pet_baseAI
{
    npc_toc_beast_master_pet_baseAI(Creature* creature)
        : npc_toc_pet_baseAI(creature)
    {
        if (pInstance && pInstance->IsBoostEnabled() == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            me->SetMaxHealth(me->GetMaxHealth() * 1.6f);
            me->SetHealth(CalculatePct(me->GetMaxHealth(), 75));
        }
    }

    uint64 GetOtherPetVictimGUID() const
    {
        if (Creature* other = me->FindNearestCreature(me->GetEntry() == NPC_BOOST_KING_KRUSH ? NPC_BOOST_LOQUENAHAK : NPC_BOOST_KING_KRUSH, 250.0f))
            return other->GetTarget();

        return 0;
    }

    bool CanAIAttack(Unit const* victim) const override
    {
        if (me->getThreatManager().getThreatList().size() <= 5)
            return true;

        return victim->GetGUID() != GetOtherPetVictimGUID();
    }
};

struct npc_toc_pet_hunter_loquenahak : public npc_toc_beast_master_pet_baseAI
{
    npc_toc_pet_hunter_loquenahak(Creature* creature)
        : npc_toc_beast_master_pet_baseAI(creature)
    {
    }

    void ScheduleCombatEvents() override
    {
        events.RescheduleEvent(EVENT_LOQUENAHAK_CLAW, 5s, 15s);
        events.RescheduleEvent(EVENT_LOQUENAHAK_SPIRIT_STRIKE, 5s, 10s);
    }

    void DoEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case EVENT_LOQUENAHAK_CLAW:
                if (Unit* victim = me->GetVictim())
                {
                    me->CastCustomSpell(SPELL_LOQUENAHAK_CLAW, SPELLVALUE_BASE_POINT0, 6000, victim, TRIGGERED_FULL_MASK);
                    events.Repeat(5s, 8s);
                    EventMapGCD(events, 1500);
                    break;
                }

                events.Repeat(1s);
                break;
            case EVENT_LOQUENAHAK_SPIRIT_STRIKE:
                if (Unit* victim = me->GetVictim())
                {
                    int32 bp0 = 2000;
                    int32 bp1 = 2000;
                    me->CastCustomSpell(victim, SPELL_LOQUENAHAK_SPIRIT_STRIKE, &bp0, &bp1, nullptr, true);
                    events.Repeat(8s, 10s);
                    EventMapGCD(events, 1500);
                    break;
                }

                events.Repeat(1s);
                break;
            default:
                break;
        }
    }
};

enum KingKrushSpells
{
    SPELL_KING_KRUSH_BITE           = 52473,
    SPELL_KING_KRUSH_MONSTROUS_BITE = 55498
};

enum KingKrushEvents
{
    EVENT_KING_KRUSH_BITE = 1,
    EVENT_KING_KRUSH_MONSTROUS_BITE
};

struct npc_toc_pet_hunter_king_krush : public npc_toc_beast_master_pet_baseAI
{
    npc_toc_pet_hunter_king_krush(Creature* creature)
        : npc_toc_beast_master_pet_baseAI(creature)
    {
        me->SetObjectScale(0.25f);
    }

    void ScheduleCombatEvents() override
    {
        events.RescheduleEvent(EVENT_KING_KRUSH_BITE, 5s, 15s);
        events.RescheduleEvent(EVENT_KING_KRUSH_MONSTROUS_BITE, 5s, 10s);
    }

    void DoEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case EVENT_KING_KRUSH_BITE:
                if (Unit* victim = me->GetVictim())
                {
                    me->CastCustomSpell(SPELL_KING_KRUSH_BITE, SPELLVALUE_BASE_POINT0, 6000, victim, TRIGGERED_FULL_MASK);
                    events.Repeat(5s, 8s);
                    EventMapGCD(events, 1500);
                    break;
                }

                events.Repeat(1s);
                break;
            case EVENT_KING_KRUSH_MONSTROUS_BITE:
                if (Unit* victim = me->GetVictim())
                {
                    me->CastCustomSpell(SPELL_KING_KRUSH_MONSTROUS_BITE, SPELLVALUE_BASE_POINT0, 6000, victim, TRIGGERED_FULL_MASK);
                    events.Repeat(8s, 10s);
                    EventMapGCD(events, 1500);
                    break;
                }

                events.Repeat(1s);
                break;
            default:
                break;
        }
    }
};

enum DemonHunterSpells
{
    SPELL_DH_SINISTER_STRIKE  = 48638,
    SPELL_DH_SARONITE_BARRIER = 63364,
    SPELL_DH_SHADOW_PRISON    = 45922,
    SPELL_DH_EVASION          = 26669,
    SPELL_DH_SWARMING_SHADOWS = 71264,
    SPELL_DH_EYE_BLAST        = 39908,
    SPELL_DH_METAMORPHOSIS    = 47241
};

enum DemonHunterEvents
{
    EVENT_SPELL_DH_SINISTER_STRIKE = 1,
    EVENT_SPELL_DH_SARONITE_BARRIER,
    EVENT_SPELL_DH_SHADOW_PRISON,
    EVENT_SPELL_DH_EVASION,
    EVENT_SPELL_DH_SWARMING_SHADOWS,
    EVENT_SPELL_DH_EYE_BLAST,
    EVENT_SPELL_DH_EYE_BLAST_RETURN,

    PHASE_DH_MAIN = 1,
    PHASE_DH_FLIGHT
};

enum DemonHunterMisc
{
    POINT_DH_AIR = 1,
    POINT_DH_GROUND,

    NPC_DH_EYE_BLAST_TRIGGER = 22515
};

struct npc_toc_demon_hunter : public boss_faction_championsAI
{
    npc_toc_demon_hunter(Creature* creature)
        : boss_faction_championsAI(creature, AI_MELEE)
    {
        ScheduleCombatEvents();
        _flyTargetGUID = 0;
        _casted        = false;
        me->SetCanDualWield(true);
    }

    void ScheduleCombatEvents()
    {
        events.Reset();
        events.SetPhase(PHASE_DH_MAIN);
        events.RescheduleEvent(EVENT_SPELL_DH_SINISTER_STRIKE, 2s, 6s, 0, PHASE_DH_MAIN);
        events.RescheduleEvent(EVENT_SPELL_DH_SARONITE_BARRIER, 10s, 0, PHASE_DH_MAIN);
        events.RescheduleEvent(EVENT_SPELL_DH_SHADOW_PRISON, 5s, 15s, 0, PHASE_DH_MAIN);
        events.RescheduleEvent(EVENT_SPELL_DH_EVASION, 5s, 15s, 0, PHASE_DH_MAIN);
        events.RescheduleEvent(EVENT_SPELL_DH_SWARMING_SHADOWS, 5s, 20s, 0, PHASE_DH_MAIN);
        events.RescheduleEvent(EVENT_SPELL_DH_EYE_BLAST, 25s, 30s, 0, PHASE_DH_MAIN);
    }

    EventMap events;

    bool CanCast()
    {
        return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == EFFECT_MOTION_TYPE)
        {
            switch (id)
            {
                case POINT_DH_GROUND:
                    _casted = false;
                    me->SetCanFly(false);
                    me->SetAggressive();
                    me->SetDisableGravity(false);
                    if (Unit* flyTarget = ObjectAccessor::GetPlayer(*me, _flyTargetGUID))
                        RecalculateThreat(flyTarget);
                    me->RemoveAurasDueToSpell(SPELL_DH_METAMORPHOSIS);
                    me->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    me->SetRegeneratingHealth(true);
                    _flyTargetGUID = 0;
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                    me->RemoveMechanicImmuneMask(MECHANIC_IMMUNE_MASK_BOSS_ALL);
                    
                    ScheduleCombatEvents();
                    break;
                case POINT_DH_AIR:
                    me->SetTarget(_flyTargetGUID);
                    events.ScheduleEvent(EVENT_SPELL_DH_EYE_BLAST_RETURN, 8s, 0, PHASE_DH_FLIGHT);
                    if (Unit* target = ObjectAccessor::GetUnit(*me, _flyTargetGUID))
                        if (Creature* trigger = me->SummonTrigger(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() - 1.0f, 0.0f, 8000))
                        {
                            trigger->CastCustomSpell(47373, SPELLVALUE_BASE_POINT0, -45, me, TRIGGERED_FULL_MASK);
                            trigger->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                            DoCast(trigger, SPELL_DH_EYE_BLAST, true);
                            if (target->IsPlayer())
                                me->MonsterWhisper("Demonic Eyebeam is focusing on you!", target->ToPlayer(), true);
                        }

                    break;
                default:
                    break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        boss_faction_championsAI::UpdateAI(diff);
        if (!UpdateVictim() && !events.IsInPhase(PHASE_DH_FLIGHT))
            return;

        events.Update(diff);

        if (!CanCast() && !events.IsInPhase(PHASE_DH_FLIGHT))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPELL_DH_SINISTER_STRIKE:
                    DoCastVictim(SPELL_DH_SINISTER_STRIKE, true);
                    events.Repeat(3s, 6s);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_DH_SARONITE_BARRIER:
                    if (HealthBelowPct(75))
                    {
                        me->CastCustomSpell(SPELL_DH_SARONITE_BARRIER, SPELLVALUE_AURA_DURATION, 8000, me, TRIGGERED_FULL_MASK);
                        events.Repeat(45s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_DH_SHADOW_PRISON:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 40.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        target->CastCustomSpell(SPELL_DH_SHADOW_PRISON, SPELLVALUE_AURA_DURATION, 8000, target, TRIGGERED_FULL_MASK, nullptr, nullptr, me->GetGUID());
                        events.Repeat(30s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_DH_EVASION:
                    if (HealthAbovePct(75) || me->HasAura(SPELL_DH_SARONITE_BARRIER))
                    {
                        events.Repeat(5s);
                        break;
                    }

                    DoCastSelf(SPELL_DH_EVASION);
                    events.Repeat(50s);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_DH_SWARMING_SHADOWS:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 250.0f) && !tar->HasAura(SPELL_DH_SHADOW_PRISON) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        DoCast(target, SPELL_DH_SWARMING_SHADOWS);
                        events.Repeat(20s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_DH_EYE_BLAST:
                {
                    events.SetPhase(PHASE_DH_FLIGHT);
                    _flyTargetGUID = 0;
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsPlayer() && tar->IsWithinLOSInMap(me); }))
                        _flyTargetGUID = target->GetGUID();
                    else if (Unit* victim = me->GetVictim())
                        _flyTargetGUID = victim->GetGUID();

                    me->SetMechanicImmuneMask(MECHANIC_IMMUNE_MASK_BOSS_ALL);
                    me->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
                    me->SetCanFly(true);
                    me->SetRegeneratingHealth(false);
                    me->SetDisableGravity(true);
                    me->SetPassive();
                    me->AttackStop();
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                    if (Aura* aura = me->AddAura(SPELL_DH_METAMORPHOSIS, me))
                    {
                        aura->SetMaxDuration(-1);
                        aura->SetDuration(-1);
                    }        

                    Position flightPos = me->GetPosition();
                    flightPos.m_positionZ += 15.0f;
                    me->GetMotionMaster()->MoveTakeoff(POINT_DH_AIR, flightPos, me->GetSpeed(MOVE_RUN));

                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    me->SetSpeedRate(MOVE_RUN, 2.5f);
                    break;
                }
                case EVENT_SPELL_DH_EYE_BLAST_RETURN:
                {
                    me->InterruptNonMeleeSpells(false);
                    me->SetSpeedRate(MOVE_RUN, 1.0f);
                    Position groundPos = me->GetPosition();
                    groundPos.m_positionZ -= 15.0f;
                    me->GetMotionMaster()->MoveLand(POINT_DH_GROUND, groundPos, 5.0f);
                }
                default:
                    break;
            }

            if (!CanCast())
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    uint64 _flyTargetGUID;
    bool _casted;
};

enum FeralDruidSpells
{
    SPELL_FD_CAT_FORM   = 768,
    SPELL_FD_MANGLE     = 48566,
    SPELL_FD_SWIPE      = 62078,
    SPELL_FD_BERSERK    = 50334,
    SPELL_FD_PROWL      = 9913,
    SPELL_FD_RAKE       = 48574,
    SPELL_FD_DASH       = 33357,
    SPELL_FD_FERAL_JUMP = 49376,
};

enum FeralDruidEvents
{
    EVENT_SPELL_FD_MANGLE = 1,
    EVENT_SPELL_FD_SWIPE,
    EVENT_SPELL_FD_BERSERK,
    EVENT_SPELL_FD_RAKE,
    EVENT_SPELL_FD_DASH,
    EVENT_SPELL_FD_FERAL_JUMP,
    EVENT_SPELL_FD_BARKSKIN
};

struct npc_toc_feral_druid : public boss_faction_championsAI
{
    npc_toc_feral_druid(Creature* creature)
        : boss_faction_championsAI(creature, AI_MELEE)
    {
        events.Reset();
        DoCastSelf(SPELL_FD_CAT_FORM, true);
        events.RescheduleEvent(EVENT_SPELL_FD_MANGLE, 5s, 10s);
        events.RescheduleEvent(EVENT_SPELL_FD_SWIPE, 5s, 15s);
        events.RescheduleEvent(EVENT_SPELL_FD_BERSERK, 10s, 20s);
        events.RescheduleEvent(EVENT_SPELL_FD_RAKE, 5s, 15s);
        events.RescheduleEvent(EVENT_SPELL_FD_DASH, 1s);
        events.RescheduleEvent(EVENT_SPELL_FD_FERAL_JUMP, 20s, 30s);
        events.RescheduleEvent(EVENT_SPELL_FD_BARKSKIN, 5s);
        if (me->GetEntry() == 110002)
            me->SetDisplayId(8571);
    }

    EventMap events;

    bool CanCast()
    {
        return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
    }

    void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/) override
    {
        if (me->HasAura(SPELL_FD_BERSERK))
            damage *= 1.2f;
    }

    void EnterCombat(Unit* attacker) override
    {
        boss_faction_championsAI::EnterCombat(attacker);
        me->SetAggressive();
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveAurasDueToSpell(SPELL_FD_PROWL);
    }

    void UpdateAI(uint32 diff) override
    {
        boss_faction_championsAI::UpdateAI(diff);
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (!CanCast())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPELL_FD_MANGLE:
                {
                    CustomSpellValues values;
                    values.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                    values.AddSpellMod(SPELLVALUE_BASE_POINT2, 150);
                    if (Unit* victim = me->GetVictim())
                    {
                        me->CastCustomSpell(SPELL_FD_MANGLE, values, me->GetVictim());
                        events.Repeat(10s, 15s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                }
                case EVENT_SPELL_FD_SWIPE:
                {
                    std::vector<Unit*> targets = GetEnemiesWithPredicate([this](Unit const* target) { return me->isInFront(target, M_PI / 2.0f) && me->IsWithinMeleeRange(target, 1.0f, true); });
                    if (targets.size() < 2)
                    {
                        events.Repeat(5s);
                        break;
                    }

                    me->CastCustomSpell(SPELL_FD_SWIPE, SPELLVALUE_BASE_POINT0, 75, targets.front());
                    events.Repeat(15s, 25s);
                    EventMapGCD(events, 1500);
                    break;
                }
                case EVENT_SPELL_FD_BERSERK:
                    DoCastSelf(SPELL_FD_BERSERK);
                    events.Repeat(3.0min, 3.1min);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_SPELL_FD_RAKE:
                {
                    CustomSpellValues values;
                    values.AddSpellMod(SPELLVALUE_AURA_DURATION, 15000);
                    values.AddSpellMod(SPELLVALUE_BASE_POINT0, 2000);
                    values.AddSpellMod(SPELLVALUE_BASE_POINT1, 4000);
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 5.0f, false, -SPELL_FD_RAKE))
                    {
                        me->CastCustomSpell(SPELL_FD_RAKE, values, target);
                        events.Repeat(20s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                }
                case EVENT_SPELL_FD_DASH:
                    if (Unit* victim = me->GetVictim())
                        if (!victim->IsWithinMeleeRange(me, 1.0f, true))
                        {
                            me->CastCustomSpell(SPELL_FD_DASH, SPELLVALUE_BASE_POINT0, 50, me, TRIGGERED_FULL_MASK);
                            events.Repeat(30s, 35s);
                            EventMapGCD(events, 1500);
                            break;
                        }

                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_FD_FERAL_JUMP:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 8.0f, 25.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        DoCast(target, SPELL_FD_FERAL_JUMP);
                        RecalculateThreat(target);
                        events.Repeat(15s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(5s);
                    break;
                case EVENT_SPELL_FD_BARKSKIN:
                    if (HealthBelowPct(50))
                    {
                        me->CastCustomSpell(SPELL_BARKSKIN, SPELLVALUE_BASE_POINT1, -30, me, TRIGGERED_FULL_MASK);
                        events.Repeat(35s, 40s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                default:
                    break;
            }

            if (!CanCast())
                return;
        }

        DoMeleeAttackIfReady();
    }
};

enum ElementalShamanSpells
{
    SPELL_ES_LIGHTNING_BOLT      = 63809,
    SPELL_ES_CHAIN_LIGHTNING     = 49271,
    SPELL_ES_FLAME_SHOCK         = 49233,
    SPELL_ES_LAVA_BURST          = 60043,
    SPELL_ES_EARTHBIND_TOTEM     = 2484,
    SPELL_ES_THUNDERSTORM        = 59159,
    SPELL_ES_LESSER_HEALING_WAVE = 25420
};

enum ElementalShamanEvents
{
    EVENT_ES_LIGHTNING_BOLT = 1,
    EVENT_ES_CHAIN_LIGHTNING,
    EVENT_ES_FLAME_SHOCK,
    EVENT_ES_LAVA_BURST,
    EVENT_ES_EARTHBIND_TOTEM,
    EVENT_ES_THUNDERSTORM,
    EVENT_ES_LESSER_HEALING_WAVE,
    EVENT_ES_HEROISM_OR_BLOODLUST
};

struct npc_toc_elemental_shaman : npc_toc_shaman_baseAI
{
    npc_toc_elemental_shaman(Creature* creature)
        : npc_toc_shaman_baseAI(creature, SHAMAN_ELEMENTAL)
    {
        events.Reset();
        events.RescheduleEvent(EVENT_ES_LIGHTNING_BOLT, 0s, 2s);
        events.RescheduleEvent(EVENT_ES_CHAIN_LIGHTNING, 2s, 5s);
        events.RescheduleEvent(EVENT_ES_FLAME_SHOCK, 0s, 2s);
        events.RescheduleEvent(EVENT_ES_LAVA_BURST, 5s, 10s);
        events.RescheduleEvent(EVENT_ES_EARTHBIND_TOTEM, 10s, 20s);
        events.RescheduleEvent(EVENT_ES_THUNDERSTORM, 20s, 40s);
        events.RescheduleEvent(EVENT_ES_LESSER_HEALING_WAVE, 5s, 15s);
        events.RescheduleEvent(EVENT_ES_HEROISM_OR_BLOODLUST, 25s, 40s);
        events.RescheduleEvent(151, 1s, 3s);
    }

    EventMap events;

    bool CanCast()
    {
        return !(me->HasUnitState(UNIT_STATE_CASTING) || IsCCed());
    }

    void UpdateAI(uint32 diff) override
    {
        boss_faction_championsAI::UpdateAI(diff);
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (!CanCast())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ES_LIGHTNING_BOLT:
                    if (Unit* victim = me->GetVictim())
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, urand(8000, 10000));
                        values.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1800);
                        values.AddSpellMod(SPELLVALUE_SPELL_INTERRUPT_FLAGS, 0);
                        me->CastCustomSpell(SPELL_ES_LIGHTNING_BOLT, values, victim);
                        events.Repeat(3s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(2s);
                    break;
                case EVENT_ES_CHAIN_LIGHTNING:
                    if (Unit* victim = me->GetVictim())
                    {
                        me->CastCustomSpell(SPELL_ES_CHAIN_LIGHTNING, SPELLVALUE_BASE_POINT0, 12000, victim);
                        events.Repeat(6s, 8s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(2s);
                    break;
                case EVENT_ES_FLAME_SHOCK:
                    if (Unit* victim = me->GetVictim())
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, 4000);
                        values.AddSpellMod(SPELLVALUE_BASE_POINT1, 2000);
                        values.AddSpellMod(SPELLVALUE_AURA_DURATION, 15000);
                        me->CastCustomSpell(SPELL_ES_FLAME_SHOCK, values, victim);
                        events.Repeat(8s);
                        events.RescheduleEvent(EVENT_ES_LAVA_BURST, 0s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(2s);
                    break;
                case EVENT_ES_LAVA_BURST:
                    if (Unit* victim = me->GetVictim())
                    {
                        me->CastCustomSpell(SPELL_ES_LAVA_BURST, SPELLVALUE_BASE_POINT0, 18000, victim);
                        EventMapGCD(events, 1500);
                    }
                    break;
                case EVENT_ES_EARTHBIND_TOTEM:
                    if (EnemiesInRange(10.0f) >= 3)
                    {
                        me->CastCustomSpell(SPELL_ES_EARTHBIND_TOTEM, SPELLVALUE_AURA_DURATION, 10000, nullptr);
                        events.Repeat(20s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                case EVENT_ES_THUNDERSTORM:
                    if (EnemiesInRange(10.0f) >= 3)
                    {
                        me->CastCustomSpell(SPELL_ES_THUNDERSTORM, SPELLVALUE_BASE_POINT0, 5000, me);
                        events.Repeat(30s);
                        EventMapGCD(events, 1500);
                        break;
                    }

                    events.Repeat(6s);
                    break;
                case EVENT_ES_LESSER_HEALING_WAVE:
                    if (Creature* target = SelectTarget_MostHPLostFriendlyMissingBuff(SPELL_HEALING_WAVE, 40.0f))
                        DoCast(target, SPELL_HEALING_WAVE);
                    events.Repeat(10s, 15s);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_ES_HEROISM_OR_BLOODLUST:
                    DoCastSelf(me->GetEntry() == NPC_ALLIANCE_ELEMENTAL_SHAMAN ? SPELL_HEROISM : SPELL_BLOODLUST, true);
                    events.Repeat(5min);
                    EventMapGCD(events, 1500);
                    break;
                case 151:
                    HandleTotems();
                    events.Repeat(45s, 60s);
                    EventMapGCD(events, 1500);
                    break;
                default:
                    break;
            }

            if (!CanCast())
                return;
        }

        DoMeleeAttackIfReady();
    }
};

enum DemonologyWarlockSpells
{
    SPELL_DW_HEALTHSTONE         = 23472,
    SPELL_DW_INCINERATE          = 29722,
    SPELL_DW_SHADOW_BOLT         = 1106,
    SPELL_DW_FEAR                = 5782,
    SPELL_DW_SOUL_LINK           = 25228,
    SPELL_DW_IMMOLATE            = 47811,
    SPELL_DW_DEMONIC_EMPOWERMENT = 54508,
    SPELL_DW_METAMORPHOSIS       = 47241,
    SPELL_DW_IMMOLATION_AURA     = 50589,
    SPELL_DW_SHADOW_CLEAVE       = 70670,
    SPELL_DW_CURSE_OF_AGONY      = 68138,
    SPELL_DW_CURSE_OF_TONGUES    = 11719
};

enum DemonologyWarlockEvents
{
    EVENT_DW_INCINERATE = 1,
    EVENT_DW_SHADOW_BOLT,
    EVENT_DW_FEAR,
    EVENT_DW_DEATH_COIL,
    EVENT_DW_IMMOLATE,
    EVENT_DW_DEMONIC_EMPOWERMENT,
    EVENT_DW_METAMORPHOSIS,
    EVENT_DW_IMMOLATION_AURA,
    EVENT_DW_SHADOW_CLEAVE,
    EVENT_DW_CURSE,
    EVENT_DW_METAMORPHOSIS_END
};

struct npc_toc_demonology_warlock : boss_faction_championsAI
{
    npc_toc_demonology_warlock(Creature* creature)
        : boss_faction_championsAI(creature, AI_RANGED)
    {
        HealthstoneCounter = 0;
        CurrentCurseTarget = 0;
        events.Reset();
        events.RescheduleEvent(EVENT_DW_INCINERATE, 2s, 4s);
        events.RescheduleEvent(EVENT_DW_SHADOW_BOLT, 2s, 4s);
        events.RescheduleEvent(EVENT_DW_FEAR, 4s, 12s);
        events.RescheduleEvent(EVENT_DW_DEATH_COIL, 15s, 25s);
        events.RescheduleEvent(EVENT_DW_IMMOLATE, 2s, 12s);
        events.RescheduleEvent(EVENT_DW_DEMONIC_EMPOWERMENT, 10s, 30s);
        events.RescheduleEvent(EVENT_DW_METAMORPHOSIS, 10s, 40s);
        events.RescheduleEvent(EVENT_DW_CURSE, 0s, 2s);
    }

    EventMap events;
    uint32 HealthstoneCounter;
    uint64 CurrentCurseTarget;

    bool CanCast()
    {
        return !(me->IsCasting() || IsCCed());
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == 1)
        {
            Position pos = me->GetPosition();
            me->GetNearPosition(pos, 4.0f, float(M_PI / 2));
            if (Creature* felguard = DoSummon(NPC_BOOST_FELGUARD, pos))
                felguard->SetOwnerGUID(me->GetGUID());
        }
    }

    void EnterCombat(Unit* who) override
    {
        boss_faction_championsAI::EnterCombat(who);
        if (Creature* felguard = GetFelguard())
        {
            felguard->CastSpell(felguard, SPELL_DW_SOUL_LINK, true);
            DoZoneInCombat(felguard, 250.0f);
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageEffectType*/, SpellSchoolMask /*spellSchoolMask*/) override
    {
        if (Creature* felguard = GetFelguard())
        {
            uint32 damageToConvert = CalculatePct(damage, 20);
            damage -= damageToConvert;
            me->DealDamage(me, felguard, damageToConvert);
        }

        if ((me->HealthBelowPctDamaged(50, damage) && HealthstoneCounter == 0) || (me->HealthBelowPctDamaged(25, damage) && HealthstoneCounter == 1))
        {
            ++HealthstoneCounter;
            me->CastCustomSpell(SPELL_DW_HEALTHSTONE, SPELLVALUE_BASE_POINT0, CalculatePct(me->GetHealth(), 15), me, TRIGGERED_FULL_MASK);
            EventMapGCD(events, 1500);
        }
    }

    Creature* GetFelguard() const
    {
        return me->FindNearestCreature(NPC_BOOST_FELGUARD, 250.0f);
    }

    void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType damageType) override
    {
        if (Creature* felguard = GetFelguard())
            felguard->ModifyHealth(CalculatePct(damage, 15));
    }

    void UpdateAI(uint32 diff) override
    {
        boss_faction_championsAI::UpdateAI(diff);
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (!CanCast())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DW_INCINERATE:
                    if (Unit* victim = me->GetVictim())
                    {
                        me->CastCustomSpell(SPELL_DW_INCINERATE, SPELLVALUE_BASE_POINT0, 12000, victim);
                        events.Repeat(5s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_DW_SHADOW_BOLT:
                    if (Unit* victim = me->GetVictim())
                    {
                        me->CastCustomSpell(SPELL_DW_SHADOW_BOLT, SPELLVALUE_BASE_POINT0, 12000, victim);
                        events.Repeat(5s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_DW_FEAR:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 20.0f) && !tar->HasAura(SPELL_DW_FEAR) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        me->CastCustomSpell(SPELL_DW_FEAR, SPELLVALUE_AURA_DURATION, 12000, target);
                        events.Repeat(15s, 20s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                case EVENT_DW_DEATH_COIL:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) -> bool { return tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && tar->IsPlayer(); }))
                    {
                        DoCast(target, SPELL_DEATH_COIL_WARL);
                        events.Repeat(30s, 35s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                case EVENT_DW_IMMOLATE:
                    if (Unit* victim = me->GetVictim())
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, 2000);
                        values.AddSpellMod(SPELLVALUE_BASE_POINT1, 8000);
                        me->CastCustomSpell(SPELL_DW_IMMOLATE, values, victim);
                        events.Repeat(8s, 12s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(3s);
                    break;
                case EVENT_DW_DEMONIC_EMPOWERMENT:
                    if (Creature* felguard = GetFelguard())
                    {
                        felguard->CastSpell(felguard, SPELL_DW_DEMONIC_EMPOWERMENT, false);
                        events.Repeat(45s, 50s);
                        EventMapGCD(events, 1500);
                    }
                    else
                        events.Repeat(5s);
                    break;
                case EVENT_DW_METAMORPHOSIS:
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
                    me->CastCustomSpell(SPELL_DW_METAMORPHOSIS, SPELLVALUE_AURA_DURATION, 20000, me);
                    events.Repeat(90s, 95s);
                    events.RescheduleEvent(EVENT_DW_IMMOLATION_AURA, 1s);
                    events.RescheduleEvent(EVENT_DW_SHADOW_CLEAVE, 2s, 4s);
                    events.RescheduleEvent(EVENT_DW_METAMORPHOSIS_END, 20s);
                    EventMapGCD(events, 1500);
                    break;
                case EVENT_DW_METAMORPHOSIS_END:
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, false);
                    break;
                case EVENT_DW_IMMOLATION_AURA:
                    if (EnemiesInRange(8.0f) >= 3)
                    {
                        me->CastCustomSpell(SPELL_DW_IMMOLATION_AURA, SPELLVALUE_AURA_DURATION, 9000, me);
                        EventMapGCD(events, 1500);
                    }
                    break;
                case EVENT_DW_SHADOW_CLEAVE:
                {
                    if (!me->HasAura(SPELL_DW_METAMORPHOSIS))
                        break;

                    std::vector<Unit*> targets = GetEnemiesWithPredicate([this](Unit const* target) { return me->isInFront(target, M_PI / 2.0f) && me->IsWithinMeleeRange(target, 1.0f, true); });
                    if (targets.size() < 2)
                    {
                        events.Repeat(1s);
                        break;
                    }

                    CustomSpellValues values;
                    values.AddSpellMod(SPELLVALUE_BASE_POINT0, 8000);
                    values.AddSpellMod(SPELLVALUE_MAX_TARGETS, 3);
                    me->CastCustomSpell(SPELL_DW_SHADOW_CLEAVE, values, targets.front());
                    events.Repeat(5s);
                    EventMapGCD(events, 1500);
                    break;
                }
                case EVENT_DW_CURSE:
                {
                    Unit* target = ObjectAccessor::GetCreature(*me, CurrentCurseTarget);
                    if (target && target->IsAlive())
                    {
                        events.Repeat(2s);
                        break;
                    }

                    if (!target || (!target->HasAura(SPELL_DW_CURSE_OF_AGONY) && !target->HasAura(SPELL_DW_CURSE_OF_TONGUES)))
                    {
                        if (Unit* newTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, [this](Unit* tar) { return tar->IsInRange(me, 0.0f, 30.0f) && !IsNonViableTarget(tar) && tar->IsPlayer() && !tar->HasAura(SPELL_DW_CURSE_OF_AGONY) && !tar->HasAura(SPELL_DW_CURSE_OF_TONGUES); }))
                        {
                            DoCast(RAND(SPELL_DW_CURSE_OF_AGONY, SPELL_DW_CURSE_OF_TONGUES));
                            events.Repeat(12s);
                            EventMapGCD(events, 1500);
                            break;
                        }
                    }
                    events.Repeat(2s);
                    break;
                }
                default:
                    break;
            }

            if (!CanCast())
                return;
        }

        DoMeleeAttackIfReady();
    }
};

enum FelguardPetSpells
{
    SPELL_DW_INTERCEPT = 47996,
    SPELL_DW_CLEAVE    = 47994
};

enum FelguardPetEvents
{
    EVENT_DW_INTERCEPT = 1,
    EVENT_DW_CLEAVE,
    EVENT_CHANGE_TARGET_FELGUARD
};

struct npc_toc_pet_warlock_felguard : public npc_toc_pet_baseAI
{
    npc_toc_pet_warlock_felguard(Creature* creature)
        : npc_toc_pet_baseAI(creature)
    {
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
    }

    void ScheduleCombatEvents() override
    {
        events.RescheduleEvent(EVENT_DW_INTERCEPT, 5s, 15s);
        events.RescheduleEvent(EVENT_DW_CLEAVE, 5s, 10s);
        events.RescheduleEvent(EVENT_CHANGE_TARGET_FELGUARD, 15s, 25s);
    }

    void DoEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case EVENT_DW_INTERCEPT:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [this](Unit* tar) { return tar->IsPlayer() && tar->IsInRange(me, 8.0f, 25.0f) && !IsNonViableTarget(tar); }))
                {
                    RecalculateThreat(target);
                    DoCast(target, SPELL_DW_INTERCEPT);
                    events.Repeat(25s, 30s);
                    EventMapGCD(events, 1500);
                }
                else
                    events.Repeat(5s);
                break;
            case EVENT_DW_CLEAVE:
                DoCastVictim(SPELL_DW_CLEAVE);
                events.Repeat(5s, 8s);
                EventMapGCD(events, 1500);
                break;
            case EVENT_CHANGE_TARGET_FELGUARD:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* tar) { return tar->IsPlayer() && !IsNonViableTarget(tar); }))
                    AttackStart(target);
                events.Repeat(15s, 25s);
                break;
            default:
                break;
        }
    }
};

void AddSC_boss_faction_champions()
{
    new npc_toc_druid();
    new npc_toc_shaman();
    new npc_toc_paladin();
    new npc_toc_priest();
    new npc_toc_shadow_priest();
    new npc_toc_mage();
    new npc_toc_warlock();
    new npc_toc_hunter();
    new npc_toc_boomkin();
    new npc_toc_warrior();
    new npc_toc_dk();
    new npc_toc_rogue();
    new SpellScriptLoaderEx<spell_fan_of_knives_fc_SpellScript>("spell_fan_of_knives_fc");
    new npc_toc_enh_shaman();
    new npc_toc_retro_paladin();
    RegisterCreatureAI(npc_toc_pet_warlock);
    RegisterCreatureAI(npc_toc_pet_hunter);
    new go_toc_champions_cache();
    new spell_faction_champion_warl_unstable_affliction();
    new CreatureAILoader<npc_searing_totem_factionsAI>("npc_searing_totem_factions");
    new CreatureAILoader<npc_healing_stream_totem_factionsAI>("npc_healing_stream_totem_factions");
    RegisterAuraScript(spell_faction_champions_lifebloom);
    // Boost
    RegisterCreatureAI(npc_toc_beast_master_hunter);
    RegisterCreatureAI(npc_toc_pet_hunter_loquenahak);
    RegisterCreatureAI(npc_toc_pet_hunter_king_krush);
    RegisterCreatureAI(npc_toc_demon_hunter);
    RegisterCreatureAI(npc_toc_feral_druid);
    RegisterCreatureAI(npc_toc_elemental_shaman);
    RegisterCreatureAI(npc_toc_demonology_warlock);
    RegisterCreatureAI(npc_toc_pet_warlock_felguard);
}
