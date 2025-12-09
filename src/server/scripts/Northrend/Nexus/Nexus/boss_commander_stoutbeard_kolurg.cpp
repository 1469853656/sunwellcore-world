
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "nexus.h"

enum KolurgSpells
{
    SPELL_BATTLE_SHOUT              = 31403,
    SPELL_CHARGE                    = 60067,
    SPELL_FRIGHTENING_SHOUT         = 19134,
    SPELL_WHIRLWIND                 = 38618
};

enum KolurgEvents
{
    EVENT_BATTLE_SHOUT              = 1,
    EVENT_FRIGHTENING_SHOUT,
    EVENT_WHIRLWIND,
    EVENT_COMMANDER_CHARGE,
    EVENT_KILL_TALK
};

enum KolurgSays
{
    SAY_AGGRO                        = 0,
    SAY_DEATH,
    SAY_KILL
};

struct boss_commander_stoutbeard : public BossAI
{
    boss_commander_stoutbeard(Creature* creature) : BossAI(creature, DATA_COMMANDER_EVENT) { }

    void Reset() override
    {
        BossAI::Reset();
    }

    void EnterCombat(Unit* who) override
    {
        BossAI::EnterCombat(who);
        Talk(SAY_AGGRO);

        events.ScheduleEvent(EVENT_BATTLE_SHOUT, 0);
        events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, 10s);
        events.ScheduleEvent(EVENT_WHIRLWIND, 15s);
        events.ScheduleEvent(EVENT_COMMANDER_CHARGE, 1s);
        me->RemoveAllAuras();
    }

    void KilledUnit(Unit* /*who*/) override
    {
        if (events.GetNextEventTime(EVENT_KILL_TALK) == 0)
        {
            Talk(SAY_KILL);
            events.ScheduleEvent(EVENT_KILL_TALK, 6s);
        }
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        Talk(SAY_DEATH);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BATTLE_SHOUT:
                    if (!me->HasAura(SPELL_BATTLE_SHOUT))
                    {
                        if (!instance->IsMythic())
                            me->CastSpell(me, SPELL_BATTLE_SHOUT, true);
                        else
                        {
                            Mythic::MythicSpells data;
                            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BATTLE_SHOUT, data));
                            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                            me->CastCustomSpell(SPELL_CHARGE, val, (Unit*)nullptr);
                        }
                    }
                    events.ScheduleEvent(EVENT_BATTLE_SHOUT, 20s);
                    break;
                case EVENT_FRIGHTENING_SHOUT:
                    me->CastSpell(me->GetVictim(), SPELL_FRIGHTENING_SHOUT, false);
                    if (!instance->IsMythic())
                        events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, Milliseconds(urand(15000, 20000)));
                    else
                        events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, 13000ms);
                    break;
                case EVENT_WHIRLWIND:
                    me->CastSpell(me, SPELL_WHIRLWIND, false);
                    events.ScheduleEvent(EVENT_WHIRLWIND, 16s);
                    break;
                case EVENT_COMMANDER_CHARGE:
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, [&](Unit* obj)
                    {
                        return obj && obj->ToPlayer() && obj->IsWithinDist(me, 25.f) && me->IsWithinLOSInMap(obj);
                    }))
                    {
                        if (!instance->IsMythic())
                            me->CastSpell(target, SPELL_CHARGE, false);
                        else
                        {
                            Mythic::MythicSpells data;
                            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_CHARGE, data));
                            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                            me->CastCustomSpell(SPELL_CHARGE, val, target);
                        }
                    }
                    events.ScheduleEvent(EVENT_COMMANDER_CHARGE, 20s);
                    break;
                }
                default:
                    break;
            }

            if (me->IsCasting())
                return;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_commander_stoutbeard()
{
    RegisterCreatureAI(boss_commander_stoutbeard);
}
