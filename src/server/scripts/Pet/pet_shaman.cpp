

/*
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "npc_pet_sha_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"

enum ShamanSpells
{
    SPELL_SHAMAN_ANGEREDEARTH = 36213,
    SPELL_SHAMAN_FIREBLAST    = 57984,
    SPELL_SHAMAN_FIRENOVA     = 12470,
    SPELL_SHAMAN_FIRESHIELD   = 13377
};

enum ShamanEvents
{
    // Earth Elemental
    EVENT_SHAMAN_ANGEREDEARTH = 1,
    // Fire Elemental
    EVENT_SHAMAN_FIRENOVA   = 1,
    EVENT_SHAMAN_FIRESHIELD = 2,
    EVENT_SHAMAN_FIREBLAST  = 3,

    EVENT_SHAMAN_CHECK_LEASH_RANGE = 4
};

class npc_pet_shaman_earth_elemental : public CreatureScript
{
public:
    npc_pet_shaman_earth_elemental()
        : CreatureScript("npc_pet_shaman_earth_elemental")
    {
    }

    struct npc_pet_shaman_earth_elementalAI : public ScriptedAI
    {
        npc_pet_shaman_earth_elementalAI(Creature* creature)
            : ScriptedAI(creature)
            , _initAttack(true)
        {
        }


        void EnterCombat(Unit*) override
        {
            _events.Reset();
            _events.ScheduleEvent(EVENT_SHAMAN_ANGEREDEARTH, 0);
            _events.ScheduleEvent(EVENT_SHAMAN_CHECK_LEASH_RANGE, 2s);
        }

        void InitializeAI() override
        {
            me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true, SPELL_BLOCK_TYPE_NEGATIVE);
        }

        void UpdateAI(uint32 diff) override
        {
            if (_initAttack)
            {
                if (!me->IsInCombat())
                    if (Player* owner = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                        if (Unit* target = owner->GetSelectedUnit())
                            if (me->_CanDetectFeignDeathOf(target) && me->CanCreatureAttack(target))
                                AttackStart(target);
                _initAttack = false;
            }

            if (!UpdateVictim())
                return;

            _events.Update(diff);

            switch (_events.ExecuteEvent())
            {
                case EVENT_SHAMAN_ANGEREDEARTH:
                    DoCastVictim(SPELL_SHAMAN_ANGEREDEARTH);
                    _events.ScheduleEvent(EVENT_SHAMAN_ANGEREDEARTH, urand(5000, 20000));
                    break;
                case EVENT_SHAMAN_CHECK_LEASH_RANGE:
                    if (!me->IsInDist(&me->GetHomePosition(), 50.0f))
                    {
                        EnterEvadeMode();
                        break;
                    }
                    _events.Repeat(2s);
                    break;
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        bool _initAttack;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_pet_shaman_earth_elementalAI(creature);
    }
};

struct npc_pet_shaman_fire_elemental final : ScriptedAI
{
    explicit npc_pet_shaman_fire_elemental(Creature* creature) : ScriptedAI(creature) { }

    void InitializeAI() override
    {
        // The elemental should be immune to all kind of fire spells.
        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
    }

    void EnterCombat(Unit*) override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_SHAMAN_FIRENOVA, 5s, 10s);
        _events.ScheduleEvent(EVENT_SHAMAN_FIREBLAST, 5s, 10s);

        me->RemoveAurasDueToSpell(SPELL_SHAMAN_FIRESHIELD);
        DoCastSelf(SPELL_SHAMAN_FIRESHIELD, true);
    }

    bool IsViableTarget(Unit const* target) const
    {
        return target->IsAlive() && me->_CanDetectFeignDeathOf(target) && me->CanCreatureAttack(target);
    }

    void UpdateTargetGUIDAndAttack()
    {
        if (!me->IsInWorld() || !me->FindMap())
            return;

        Player const* owner = me->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner || !owner->IsInCombat())
            return;

        Unit* target = nullptr;
        if (_currentTarget != 0)
            target = ObjectAccessor::GetCreature(*me, _currentTarget);

        if (!target || !IsViableTarget(target))
        {
            if (Unit* newTarget = SelectNewTargetForAttack())
            {
                AttackStart(newTarget);
                SetGazeOn(newTarget);
                _currentTarget = newTarget->GetGUID();
            }
        }
    }

    Unit* SelectNewTargetForAttack() const
    {
        Player const* owner = me->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner)
            return nullptr;

        // if owner targets any hostile target then attack them as well
        if (Unit* selectedUnit = owner->GetSelectedUnit())
            if (IsViableTarget(selectedUnit))
                return selectedUnit;

        // ...otherwise, select nearest viable target
        Unit* closestViableTarget = nullptr;
        Trinity::NearestUnfriendlyInCombatWithOwnerNoTotemUnitInObjectRangeCheck u_check(me, me, owner, 30.0f);
        Trinity::UnitLastSearcher checker(me, closestViableTarget, u_check);
        me->VisitNearbyObject(30.0f, checker);
        if (closestViableTarget)
            return closestViableTarget;

        return nullptr;
    }

    void UpdateAI(uint32 const diff) override
    {
        if (!UpdateVictimWithGaze())
        {
            UpdateTargetGUIDAndAttack();
            return;
        }

        _events.Update(diff);

        if (me->IsCasting())
            return;

        while (uint32 const eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHAMAN_FIRENOVA:
                    DoCastSelf(SPELL_SHAMAN_FIRENOVA);
                    _events.Repeat(5s + 2s /* casting time */);
                    break;
                case EVENT_SHAMAN_FIREBLAST:
                    DoCastVictim(SPELL_SHAMAN_FIREBLAST);
                    _events.Repeat(6s);
                    break;
                default:
                    break;
            }

            if (me->IsCasting())
                return;
        }

        DoMeleeAttackIfReady();
    }

    private:
        EventMap _events;
        uint64 _currentTarget = 0;
};

void AddSC_shaman_pet_scripts()
{
    new npc_pet_shaman_earth_elemental();
    RegisterCreatureAI(npc_pet_shaman_fire_elemental);
}
