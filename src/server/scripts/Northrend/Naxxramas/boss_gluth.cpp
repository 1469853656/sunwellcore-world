/*
REWRITTEN FROM SCRATCH BY XINEF, IT OWNS NOW!
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "naxxramas.h"
#include "SpellScript.h"
#include "Player.h"

enum Spells
{
    SPELL_MORTAL_WOUND   = 25646,
    SPELL_ENRAGE_10      = 28371,
    SPELL_ENRAGE_25      = 54427,
    SPELL_DECIMATE_10    = 28374,
    SPELL_DECIMATE_25    = 54426,
    SPELL_BERSERK        = 26662,
    SPELL_INFECTED_WOUND = 29306,
    SPELL_CHOW_SEARCHER  = 28404
};

enum Events
{
    EVENT_SPELL_MORTAL_WOUND = 1,
    EVENT_SPELL_ENRAGE       = 2,
    EVENT_SPELL_DECIMATE     = 3,
    EVENT_SPELL_BERSERK      = 4,
    EVENT_SUMMON_ZOMBIE      = 5,
    EVENT_CAN_EAT_ZOMBIE     = 6
};

enum Misc
{
    NPC_ZOMBIE_CHOW = 16360
};

const Position zombiePos[3] = {
    { 3267.9f, -3172.1f, 297.42f, 0.94f },
    { 3253.2f, -3132.3f, 297.42f, 0.0f },
    { 3308.3f, -3185.8f, 297.42f, 1.58f },
};

class boss_gluth : public CreatureScript
{
public:
    boss_gluth()
        : CreatureScript("boss_gluth")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new boss_gluthAI(pCreature);
    }

    struct boss_gluthAI : BossAI
    {
        boss_gluthAI(Creature* c) : BossAI(c, EVENT_GLUTH) { }
        uint64 gazeTarget;

        void Reset() override
        {
            _Reset();
            me->ApplySpellImmune(29306, IMMUNITY_ID, 29306, true);
            gazeTarget = 0;
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if ((!me->GetVictim() || me->GetVictim()->GetEntry() != NPC_ZOMBIE_CHOW) && who->GetEntry() == NPC_ZOMBIE_CHOW && me->IsWithinDistInMap(who, 1))
            {
                SetGazeOn(who);
                me->MonsterTextEmote("%s spots a nearby zombie to devour!", nullptr, false);
            }
            else
                BossAI::MoveInLineOfSight(who);
        }

        void EnterCombat(Unit* who) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            _EnterCombat();
            me->SetHealth(me->GetMaxHealth());
            me->SetInCombatWithZone();
            events.ScheduleEvent(EVENT_SPELL_MORTAL_WOUND, 10000);
            events.ScheduleEvent(EVENT_SPELL_ENRAGE, 30000);
            bool const is25ManBoostEnabled = sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED) && me->GetMap()->Is25ManRaid();
            uint32 const decimateTimer = is25ManBoostEnabled ? 90000 : 105000;
            events.ScheduleEvent(EVENT_SPELL_DECIMATE, decimateTimer);
            events.ScheduleEvent(EVENT_SPELL_BERSERK, is25ManBoostEnabled ? 4 * decimateTimer : 8 * 60000);
            events.ScheduleEvent(EVENT_SUMMON_ZOMBIE, 10000);
            events.ScheduleEvent(EVENT_CAN_EAT_ZOMBIE, 1000);

            if (!Is25ManRaid())
                me->SetLootMode(GetLootModeFor10Man(instance));
        }

        void JustSummoned(Creature* summon) override
        {
            if (summon->GetEntry() == NPC_ZOMBIE_CHOW)
                summon->AI()->AttackStart(me);

            summons.Summon(summon);
        }

        void SummonedCreatureDies(Creature* cr, Unit*) override { summons.Despawn(cr); }

        void KilledUnit(Unit* who) override
        {
            if (me->IsAlive() && who->GetEntry() == NPC_ZOMBIE_CHOW)
                me->ModifyHealth(int32(me->GetMaxHealth() * 0.05f));

            if (who->GetTypeId() == TYPEID_PLAYER)
                instance->SetData(DATA_IMMORTAL_FAIL, 0);
        }

        void EnterEvadeMode() override
        {
            BossAI::EnterEvadeMode();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void JustDied(Unit* killer) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            _JustDied();
            summons.DespawnAll();

            if (Map* map = me->GetMap())
            {
                //! Load grids around marauding geists so they start pathing, love sunwellcore
                map->LoadGrid(3073.399f, -2985.840f);
                map->LoadGrid(3457.850f, -3365.479f);
            }
        }

        bool IsInRoom()
        {
            if (me->GetPositionX() > 3355.0f || me->GetPositionX() < 3240.0f || me->GetPositionY() > -3070.0f || me->GetPositionY() < -3210.0f)
            {
                EnterEvadeMode();
                return false;
            }

            return true;
        }

        bool SelectPlayerInRoom()
        {
            if (me->IsInCombat())
                return false;

            Map::PlayerList const& pList = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
            {
                Player* player = itr->GetSource();

                if (!player || !player->IsAlive())
                    continue;

                if (player->GetPositionZ() > 300.0f || me->GetExactDist(player) > 50.0f)
                    continue;

                AttackStart(player);
                return true;
            }

            return false;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!IsInRoom())
                return;

            if (!UpdateVictimWithGaze() && !SelectPlayerInRoom())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (events.GetEvent())
            {
                case EVENT_SPELL_BERSERK:
                    me->CastSpell(me, SPELL_BERSERK, true);
                    events.PopEvent();
                    break;
                case EVENT_SPELL_ENRAGE:
                    me->MonsterTextEmote("Gluth becomes enraged!", nullptr, true);
                    me->CastSpell(me, RAID_MODE(SPELL_ENRAGE_10, SPELL_ENRAGE_25), true);
                    events.RepeatEvent(30000);
                    break;
                case EVENT_SPELL_MORTAL_WOUND:
                    me->CastSpell(me->GetVictim(), SPELL_MORTAL_WOUND, false);
                    events.RepeatEvent(RAID_MODE(10000, 8000));
                    break;
                case EVENT_SPELL_DECIMATE:
                    me->MonsterTextEmote("Gluth decimates all nearby flesh!", nullptr, true);
                    me->CastSpell(me, RAID_MODE(SPELL_DECIMATE_10, SPELL_DECIMATE_25), false);
                    events.RepeatEvent(sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED) && me->GetMap()->Is25ManRaid() ? 90000 : 105000);
                    break;
                case EVENT_SUMMON_ZOMBIE:
                {
                    uint8 rand = me->GetMap()->Is25ManRaid() ? urand(0, 2) : 0;
                    for (int32 i = 0; i < RAID_MODE(1, 2); ++i)
                    {
                        me->SummonCreature(NPC_ZOMBIE_CHOW, zombiePos[rand]);
                        (rand == 2 ? rand = 0 : rand++);
                    }

                    events.RepeatEvent(10000);
                    break;
                }
                case EVENT_CAN_EAT_ZOMBIE:
                    events.RepeatEvent(1000);
                    if (me->GetVictim()->GetEntry() == NPC_ZOMBIE_CHOW && me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        me->CastCustomSpell(SPELL_CHOW_SEARCHER, SPELLVALUE_RADIUS_MOD, 20000, me, true);
                        me->MonsterTextEmote("%s devour all nearby zombies!", nullptr, false);
                        return; // leave it to skip DoMeleeAttackIfReady
                    }
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };
};

class spell_gluth_decimate : public SpellScriptLoader
{
public:
    spell_gluth_decimate()
        : SpellScriptLoader("spell_gluth_decimate")
    {
    }

    class spell_gluth_decimate_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gluth_decimate_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (Unit* unitTarget = GetHitUnit())
            {
                int32 damage = int32(unitTarget->GetHealth()) - int32(unitTarget->CountPctFromMaxHealth(5));
                if (damage <= 0)
                    return;

                if (Creature* cTarget = unitTarget->ToCreature())
                {
                    cTarget->SetWalk(true);
                    cTarget->GetMotionMaster()->MoveFollow(GetCaster(), 0.0f, 0.0f, MOTION_SLOT_CONTROLLED);
                    cTarget->SetReactState(REACT_PASSIVE);
                    Unit::DealDamage(GetCaster(), cTarget, damage);
                    return;
                }

                GetCaster()->CastCustomSpell(28375, SPELLVALUE_BASE_POINT0, damage, unitTarget);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gluth_decimate_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gluth_decimate_SpellScript();
    }
};

void AddSC_boss_gluth()
{
    new boss_gluth();
    new spell_gluth_decimate();
}
