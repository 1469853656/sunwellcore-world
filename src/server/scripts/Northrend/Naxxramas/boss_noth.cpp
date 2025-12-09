/*
REWRITTEN FROM SCRATCH BY XINEF, IT OWNS NOW!
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "naxxramas.h"

enum Says
{
    SAY_AGGRO  = 0,
    SAY_SUMMON = 1,
    SAY_SLAY   = 2,
    SAY_DEATH  = 3
};

enum Spells
{
    SPELL_CURSE_OF_THE_PLAGUEBRINGER_10 = 29213,
    SPELL_CURSE_OF_THE_PLAGUEBRINGER_25 = 54835,
    SPELL_CRIPPLE_10                    = 29212,
    SPELL_CRIPPLE_25                    = 54814,
    SPELL_SUMMON_PLAGUED_WARRIORS       = 29237,
    SPELL_TELEPORT                      = 29216,
    SPELL_BLINK                         = 29208,
    SPELL_BERSERK                       = 47008 // Not sure that ID is correct.
};

enum Events
{
    EVENT_SPELL_CURSE                     = 1,
    EVENT_SPELL_CRIPPLE                   = 2,
    EVENT_SUMMON_PLAGUED_WARRIOR_ANNOUNCE = 3,
    EVENT_MOVE_TO_BALCONY                 = 4,
    EVENT_SPELL_BLINK                     = 5,
    EVENT_MOVE_TO_GROUND                  = 6,
    EVENT_SUMMON_PLAGUED_WARRIOR_REAL     = 7,
    EVENT_BALCONY_SUMMON_ANNOUNCE         = 8,
    EVENT_BALCONY_SUMMON_REAL             = 9
};

enum Misc
{
    NPC_PLAGUED_WARRIOR  = 16984,
    NPC_PLAGUED_CHAMPION = 16983,
    NPC_PLAGUED_GUARDIAN = 16981
};

const Position summoningPosition[5] = {
    { 2728.12f, -3544.43f, 261.91f, 6.04f },
    { 2729.05f, -3544.47f, 261.91f, 5.58f },
    { 2728.24f, -3465.08f, 264.20f, 3.56f },
    { 2704.11f, -3456.81f, 265.53f, 4.51f },
    { 2663.56f, -3464.43f, 262.66f, 5.20f },
};

const Position nothPosition = { 2684.94f, -3502.53f, 261.31f, 4.7f };

class boss_noth : public CreatureScript
{
public:
    boss_noth()
        : CreatureScript("boss_noth")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new boss_nothAI(pCreature);
    }

    struct boss_nothAI : BossAI
    {
        boss_nothAI(Creature* c) : BossAI(c, EVENT_NOTH) { }

        uint8 totalPhase;

        void StartGroundPhase()
        {
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetControlled(false, UNIT_STATE_ROOT);

            events.Reset();
            events.ScheduleEvent(EVENT_MOVE_TO_BALCONY, (totalPhase < 2 ? 110000 : (110000 / totalPhase)));
            events.ScheduleEvent(EVENT_SPELL_CURSE, 15000);
            events.ScheduleEvent(EVENT_SUMMON_PLAGUED_WARRIOR_ANNOUNCE, 25000);

            if (Is25ManRaid())
                events.ScheduleEvent(EVENT_SPELL_BLINK, 26000);
        }

        void StartBalconyPhase()
        {
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetControlled(true, UNIT_STATE_ROOT);

            events.Reset();
            events.ScheduleEvent(EVENT_BALCONY_SUMMON_ANNOUNCE, 4000);
            events.ScheduleEvent(EVENT_MOVE_TO_GROUND, 70000);
        }

        void SummonHelper(uint32 entry, uint32 count)
        {
            for (uint8 i = 0; i < count; ++i)
                me->SummonCreature(entry, summoningPosition[urand(0, 4)]);
        }

        bool IsInRoom()
        {
            if (me->GetPositionX() > 2730.0f || me->GetPositionX() < 2614.0f || me->GetPositionY() > -3455.0f || me->GetPositionY() < -3553.0f)
            {
                EnterEvadeMode();
                return false;
            }

            return true;
        }

        void Reset() override
        {
            _Reset();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetControlled(false, UNIT_STATE_ROOT);
            me->SetReactState(REACT_AGGRESSIVE);
            totalPhase = 0;

            if (GameObject* go = me->GetMap()->GetGameObject(instance->GetData64(DATA_NOTH_ENTRANCE_GATE)))
                go->SetGoState(GO_STATE_ACTIVE);
        }

        void EnterEvadeMode() override
        {
            me->SetControlled(false, UNIT_STATE_ROOT);
            BossAI::EnterEvadeMode();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterCombat(Unit* who) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            _EnterCombat();
            me->SetHealth(me->GetMaxHealth());
            Talk(SAY_AGGRO);

            if (!Is25ManRaid())
                me->SetLootMode(GetLootModeFor10Man(instance));

            if (GameObject* go = me->GetMap()->GetGameObject(instance->GetData64(DATA_NOTH_ENTRANCE_GATE)))
                go->SetGoState(GO_STATE_READY);

            StartGroundPhase();
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            summon->SetInCombatWithZone();
        }

        void JustDied(Unit* killer) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            _JustDied();
            Talk(SAY_DEATH);

            if (me->GetPositionZ() > 270.0f)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetControlled(false, UNIT_STATE_ROOT);
                me->NearTeleportTo(nothPosition.GetPositionX(), nothPosition.GetPositionY(), nothPosition.GetPositionZ(), nothPosition.GetOrientation(), true);
            }

            if (GameObject* go = me->GetMap()->GetGameObject(instance->GetData64(DATA_NOTH_ENTRANCE_GATE)))
                go->SetGoState(GO_STATE_ACTIVE);
        }

        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            if (!urand(0, 3))
                Talk(SAY_SLAY);

            instance->SetData(DATA_IMMORTAL_FAIL, 0);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!IsInRoom())
                return;

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventID = events.ExecuteEvent())
            {
                switch (eventID)
                {
                    // GROUND
                    case EVENT_SPELL_CURSE:
                        me->CastCustomSpell(RAID_MODE(SPELL_CURSE_OF_THE_PLAGUEBRINGER_10, SPELL_CURSE_OF_THE_PLAGUEBRINGER_25), SPELLVALUE_MAX_TARGETS, RAID_MODE(3, sWorld->getBoolConfig(CONFIG_BOOST_NAXXRAMAS) ? 15 : 10), me, false);
                        events.Repeat(sWorld->getBoolConfig(CONFIG_BOOST_NAXXRAMAS) ? RAID_MODE(25000, 30000) : 25000);
                        break;
                    case EVENT_SUMMON_PLAGUED_WARRIOR_ANNOUNCE:
                        me->MonsterTextEmote("Noth the Plaguebringer summons forth Skeletal Warriors!", nullptr, true);
                        Talk(SAY_SUMMON);
                        events.Repeat(25000);
                        events.ScheduleEvent(EVENT_SUMMON_PLAGUED_WARRIOR_REAL, 4000);
                        break;
                    case EVENT_SUMMON_PLAGUED_WARRIOR_REAL:
                        me->CastSpell(me, SPELL_SUMMON_PLAGUED_WARRIORS, true);
                        SummonHelper(NPC_PLAGUED_WARRIOR, RAID_MODE(2, 3));
                        break;
                    case EVENT_MOVE_TO_BALCONY:
                        me->MonsterTextEmote("%s teleports to the balcony above!", nullptr, true);
                        StartBalconyPhase();
                        me->CastSpell(me, SPELL_TELEPORT, true);
                        break;
                    case EVENT_SPELL_BLINK:
                        DoResetThreat();
                        me->MonsterTextEmote("%s blinks away!", nullptr, true);
                        me->CastSpell(me, RAID_MODE(SPELL_CRIPPLE_10, SPELL_CRIPPLE_25), false);
                        me->CastSpell(me, SPELL_BLINK, true);
                        events.Repeat(30000);
                        break;
                    // BALCONY
                    case EVENT_BALCONY_SUMMON_ANNOUNCE:
                        me->MonsterTextEmote("%s raises more skeletons!", nullptr, true);
                        events.Repeat(25000);
                        events.ScheduleEvent(EVENT_BALCONY_SUMMON_REAL, 4000);
                        break;
                    case EVENT_BALCONY_SUMMON_REAL:
                        me->CastSpell(me, SPELL_SUMMON_PLAGUED_WARRIORS, true); // visual only
                        if (totalPhase == 0 || totalPhase == 1)
                        {
                            SummonHelper(NPC_PLAGUED_CHAMPION, RAID_MODE(1, 2));
                            SummonHelper(NPC_PLAGUED_GUARDIAN, RAID_MODE(1, 2));
                        }
                        else
                            SummonHelper(NPC_PLAGUED_GUARDIAN, RAID_MODE(2, 4));
                        break;
                    case EVENT_MOVE_TO_GROUND:
                        me->MonsterTextEmote("%s teleports back into the battle!", nullptr, true);
                        totalPhase++;

                        if (totalPhase >= 3)
                            me->CastSpell(me, SPELL_BERSERK, true);

                        StartGroundPhase();
                        me->NearTeleportTo(nothPosition.GetPositionX(), nothPosition.GetPositionY(), nothPosition.GetPositionZ(), nothPosition.GetOrientation(), true);
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }

            if (me->HasReactState(REACT_AGGRESSIVE))
                DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_noth()
{
    new boss_noth();
}
