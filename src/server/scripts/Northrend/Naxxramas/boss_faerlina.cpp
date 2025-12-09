/*
REWRITTEN FROM SCRATCH BY XINEF, IT OWNS NOW!
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "naxxramas.h"
#include "SpellInfo.h"

enum Yells
{
    SAY_GREET,
    SAY_AGGRO,
    SAY_SLAY,
    SAY_DEATH
};

enum Spells
{
    SPELL_POISON_BOLT_VOLLEY_10 = 28796,
    SPELL_POISON_BOLT_VOLLEY_25 = 54098,
    SPELL_RAIN_OF_FIRE_10       = 28794,
    SPELL_RAIN_OF_FIRE_25       = 54099,
    SPELL_FRENZY_10             = 28798,
    SPELL_FRENZY_25             = 54100,
    SPELL_WIDOWS_EMBRACE        = 28732
};

enum Events
{
    EVENT_SPELL_POISON_BOLT = 1,
    EVENT_SPELL_RAIN_OF_FIRE,
    EVENT_SPELL_FRENZY
};

enum Misc
{
    NPC_NAXXRAMAS_WORSHIPPER = 16506,
    NPC_NAXXRAMAS_FOLLOWER   = 16505
};

enum SummonGroups
{
    SUMMON_GROUP_WORSHIPPERS = 1,
    SUMMON_GROUP_FOLLOWERS
};

struct boss_faerlinaAI : BossAI
{
    boss_faerlinaAI(Creature* creature) : BossAI(creature, EVENT_FAERLINA)
    {
        _greet   = false;
        _init    = false;
    }

    void SummonHelpers()
    {
        me->SummonCreatureGroup(SUMMON_GROUP_WORSHIPPERS);
        if (Is25ManRaid())
            me->SummonCreatureGroup(SUMMON_GROUP_FOLLOWERS);
    }

    void JustSummoned(Creature* creature) override
    {
        if (creature)
            creature->setFaction(me->getFaction());

        summons.Summon(creature);
    }

    void Reset() override
    {
        _Reset();
        if (!_init)
        {
            _init = true;
            summons.DespawnAll();
            SummonHelpers();
        }
    }

    void JustReachedHome() override
    {
        _JustReachedHome();
        summons.DespawnAll();
        SummonHelpers();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        _EnterCombat();
        me->SetHealth(me->GetMaxHealth());
        summons.DoZoneInCombat();

        Talk(SAY_AGGRO);
        events.ScheduleEvent(EVENT_SPELL_POISON_BOLT, urand(12000, 15000));
        events.ScheduleEvent(EVENT_SPELL_RAIN_OF_FIRE, urand(6000, 18000));
        events.ScheduleEvent(EVENT_SPELL_FRENZY, urand(60000, 80000), 1);
        events.SetPhase(1);

        if (!Is25ManRaid())
            me->SetLootMode(GetLootModeFor10Man(instance));
    }

    void EnterEvadeMode() override
    {
        BossAI::EnterEvadeMode();
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!_greet && who->GetTypeId() == TYPEID_PLAYER)
        {
            _greet = true;
            Talk(SAY_GREET);
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->IsPlayer() && !urand(0, 3))
            Talk(SAY_SLAY);

        if (instance)
            instance->SetData(DATA_IMMORTAL_FAIL, 0);
    }

    void JustDied(Unit* killer) override
    {
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        _JustDied();
        Talk(SAY_DEATH);
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_WIDOWS_EMBRACE)
        {
            if (Is25ManRaid() && caster)
                Unit::Kill(caster, caster, false);
            me->MonsterTextEmote("%s is affected by Widow's Embrace!", nullptr, true);
            if (me->HasAura(RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25)))
            {
                me->RemoveAurasDueToSpell(RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25));
                events.RescheduleEvent(EVENT_SPELL_FRENZY, urand(60000, 80000), 1);
            }
            else
                events.RescheduleEvent(EVENT_SPELL_FRENZY, std::max(urand(30000, 40000), events.GetTimeUntilEvent(EVENT_SPELL_FRENZY)), 1);

            if (instance)
                instance->SetData(DATA_FRENZY_REMOVED, 0);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->IsInCombat() && !summons.empty() && me->SelectNearestPlayer(40.0f))
            for (std::list<uint64>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                if (Creature* creature = ObjectAccessor::GetCreature(*me, *itr))
                    if (creature->IsInCombat())
                        DoZoneInCombat();

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPELL_POISON_BOLT:
                    if (!me->HasAura(SPELL_WIDOWS_EMBRACE))
                        me->CastCustomSpell(RAID_MODE(SPELL_POISON_BOLT_VOLLEY_10, SPELL_POISON_BOLT_VOLLEY_25), SPELLVALUE_MAX_TARGETS, sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED) && me->GetMap()->Is25ManRaid() ? 25 : 3, me, false);

                    events.Repeat(14000);
                    break;
                case EVENT_SPELL_RAIN_OF_FIRE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        me->CastSpell(target, RAID_MODE(SPELL_RAIN_OF_FIRE_10, SPELL_RAIN_OF_FIRE_25), false);
                    events.Repeat(12000);
                    break;
                case EVENT_SPELL_FRENZY:
                    me->MonsterTextEmote("%s goes into a frenzy!", nullptr, true);
                    me->CastSpell(me, RAID_MODE(SPELL_FRENZY_10, SPELL_FRENZY_25), true);
                    events.Repeat(70000);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        EnterEvadeIfOutOfCombatArea();
        DoMeleeAttackIfReady();
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        return me->GetHomePosition().GetExactDist2d(me) > 65.0f;
    }

    private:
        bool _greet;
        bool _init;
};

void AddSC_boss_faerlina()
{
    new CreatureAILoader<boss_faerlinaAI>("boss_faerlina");
}
