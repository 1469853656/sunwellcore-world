/*
REWRITTEN FROM SCRATCH BY XINEF, IT OWNS NOW!
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "naxxramas.h"
#include "PassiveAI.h"
#include "SpellScript.h"
#include "SpellAuras.h"

enum Spells
{
    SPELL_POISON_CLOUD                = 28240,
    SPELL_MUTATING_INJECTION          = 28169,
    SPELL_SLIME_SPRAY_10              = 28157,
    SPELL_SLIME_SPRAY_25              = 54364,
    SPELL_POISON_CLOUD_DAMAGE_AURA_10 = 28158,
    SPELL_POISON_CLOUD_DAMAGE_AURA_25 = 54362,
    SPELL_BERSERK                     = 26662,
    SPELL_MUTAGEN_EXPLOSION           = 28206,

    SPELL_DISEASE_CLOUD_10 = 28156,
    SPELL_DISEASE_CLOUD_25 = 54367,

    SPELL_BOMBARD_SLIME = 28280 // Spawn slime when hit by slime spray
};

enum Events
{
    EVENT_SPELL_BERSERK            = 1,
    EVENT_SPELL_POISON_CLOUD       = 2,
    EVENT_SPELL_SLIME_SPRAY        = 3,
    EVENT_SPELL_MUTATING_INJECTION = 4
};

enum Misc
{
    NPC_FALLOUT_SLIME = 16290,
    NPC_SEWAGE_SLIME  = 16375
};

class boss_grobbulus : public CreatureScript
{
public:
    boss_grobbulus()
        : CreatureScript("boss_grobbulus")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new boss_grobbulusAI(pCreature);
    }

    struct boss_grobbulusAI : public BossAI
    {
        boss_grobbulusAI(Creature* c) : BossAI(c, EVENT_GROBBULUS) { }

        uint32 dropSludgeTimer;

        void Reset() override
        {
            _Reset();
            dropSludgeTimer = 0;

            if (GameObject* go = me->GetMap()->GetGameObject(instance->GetData64(DATA_PATCHWERK_GATE)))
                go->SetGoState(GO_STATE_ACTIVE);
        }

        void EnterCombat(Unit* who) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            _EnterCombat();
            me->SetHealth(me->GetMaxHealth());
            events.ScheduleEvent(EVENT_SPELL_POISON_CLOUD, 15000);
            events.ScheduleEvent(EVENT_SPELL_MUTATING_INJECTION, 20000);
            events.ScheduleEvent(EVENT_SPELL_SLIME_SPRAY, 10000);
            events.ScheduleEvent(EVENT_SPELL_BERSERK, sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED) && me->GetMap()->Is25ManRaid() ? 8min : RAID_MODE(12min, 10min));

            if (!Is25ManRaid())
                me->SetLootMode(GetLootModeFor10Man(instance));

            if (GameObject* go = me->GetMap()->GetGameObject(instance->GetData64(DATA_PATCHWERK_GATE)))
                go->SetGoState(GO_STATE_READY);
        }

        bool CheckEvadeIfOutOfCombatArea() const override
        {
            return !SelectTargetFromPlayerList(50.0f);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spellInfo) override
        {
            if (spellInfo->Id == RAID_MODE(SPELL_SLIME_SPRAY_10, SPELL_SLIME_SPRAY_25) && target->GetTypeId() == TYPEID_PLAYER)
                me->SummonCreature(NPC_FALLOUT_SLIME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
        }

        void JustSummoned(Creature* cr) override
        {
            if (cr->GetEntry() == NPC_FALLOUT_SLIME)
            {
                cr->SetInCombatWithZone();
                cr->CastSpell(cr, RAID_MODE(SPELL_DISEASE_CLOUD_10, SPELL_DISEASE_CLOUD_25));
            }

            summons.Summon(cr);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterEvadeMode() override
        {
            BossAI::EnterEvadeMode();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void SummonedCreatureDespawn(Creature* summon) override { summons.Despawn(summon); }

        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                instance->SetData(DATA_IMMORTAL_FAIL, 0);
        }

        void UpdateAI(uint32 diff) override
        {
            // Some nice visuals
            dropSludgeTimer += diff;
            if (!me->IsInCombat() && dropSludgeTimer >= 5000)
            {
                if (me->IsWithinDist3d(3178, -3305, 319, 5.0f) && !summons.HasEntry(NPC_SEWAGE_SLIME))
                    me->CastSpell(3128.96f + irand(-20, 20), -3312.96f + irand(-20, 20), 293.25f, SPELL_BOMBARD_SLIME, false);

                dropSludgeTimer = 0;
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (events.GetEvent())
            {
                case EVENT_SPELL_POISON_CLOUD:
                    me->CastSpell(me, SPELL_POISON_CLOUD, true);
                    events.RepeatEvent(15000);
                    break;
                case EVENT_SPELL_BERSERK:
                    me->CastSpell(me, SPELL_BERSERK, true);
                    events.PopEvent();
                    break;
                case EVENT_SPELL_SLIME_SPRAY:
                    me->MonsterTextEmote("Grobbulus sprays slime across the room!", nullptr, true);
                    me->CastSpell(me->GetVictim(), RAID_MODE(SPELL_SLIME_SPRAY_10, SPELL_SLIME_SPRAY_25), true);

                    events.RepeatEvent(sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED) && me->GetMap()->Is25ManRaid() ? 15000 : 20000);
                    break;
                case EVENT_SPELL_MUTATING_INJECTION:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true, -SPELL_MUTATING_INJECTION))
                        me->CastSpell(target, SPELL_MUTATING_INJECTION, false);

                    events.RepeatEvent(sCustomMgr->GetBoolConfig(CONFIG_NAXXRAMAS_25_BOOST_ENABLED) && me->GetMap()->Is25ManRaid() ? (RAID_MODE(8000 + uint32(120 * me->GetHealthPct()), (8000 + uint32(120 * me->GetHealthPct()) / 2))) : (8000 + uint32(120 * me->GetHealthPct())));
                    break;
            }

            EnterEvadeIfOutOfCombatArea();
            DoMeleeAttackIfReady();
        }
    };
};

class boss_grobbulus_poison_cloud : public CreatureScript
{
public:
    boss_grobbulus_poison_cloud()
        : CreatureScript("boss_grobbulus_poison_cloud")
    {
    }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return new boss_grobbulus_poison_cloudAI(pCreature);
    }

    struct boss_grobbulus_poison_cloudAI : public NullCreatureAI
    {
        boss_grobbulus_poison_cloudAI(Creature* pCreature)
            : NullCreatureAI(pCreature)
        {
        }

        uint32 sizeTimer;
        uint32 auraVisualTimer;

        void Reset() override
        {
            sizeTimer       = 0;
            auraVisualTimer = 1;
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 2.0f);
            me->setFaction(21); // Grobbulus one
        }

        void KilledUnit(Unit* who) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER && me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_IMMORTAL_FAIL, 0);
        }

        void UpdateAI(uint32 diff) override
        {
            // this has to be delayed to be visible :/
            if (auraVisualTimer)
            {
                auraVisualTimer += diff;
                if (auraVisualTimer >= 1000)
                {
                    me->CastSpell(me, (me->GetMap()->Is25ManRaid() ? SPELL_POISON_CLOUD_DAMAGE_AURA_25 : SPELL_POISON_CLOUD_DAMAGE_AURA_10), true);

                    auraVisualTimer = 0;
                }
            }

            sizeTimer += diff;
            // increase size to 15yd in 60 seconds, 0.00025 is the growth of size in 1ms
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 2.0f + (0.00025f * sizeTimer));
        }
    };
};

class spell_grobbulus_poison : public SpellScriptLoader
{
public:
    spell_grobbulus_poison()
        : SpellScriptLoader("spell_grobbulus_poison")
    {
    }

    class spell_grobbulus_poison_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_grobbulus_poison_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            std::list<WorldObject*> tmplist;
            for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                if (GetCaster()->IsWithinDist3d((*itr), 0.0f))
                    tmplist.push_back(*itr);

            targets.clear();
            for (std::list<WorldObject*>::iterator itr = tmplist.begin(); itr != tmplist.end(); ++itr)
                targets.push_back(*itr);
        }

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* target                    = GetHitUnit();
            InstanceScript* instance        = target ? target->GetInstanceScript() : nullptr;
            bool const IsValidSpellOverride = target && instance && instance->IsMythic();
            if (!IsValidSpellOverride)
                return;

            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(GetSpellInfo()->Id, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;
            int32 val      = static_cast<int32>(Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            SetEffectValue(val);
        }

        void Register() override
        {
            OnEffectLaunchTarget += SpellEffectFn(spell_grobbulus_poison_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grobbulus_poison_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_grobbulus_poison_SpellScript();
    }
};

class spell_grobbulus_mutating_injection : public SpellScriptLoader
{
public:
    spell_grobbulus_mutating_injection()
        : SpellScriptLoader("spell_grobbulus_mutating_injection")
    {
    }

    class spell_grobbulus_mutating_injection_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_grobbulus_mutating_injection_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return ValidateSpellInfo({ SPELL_MUTAGEN_EXPLOSION, SPELL_POISON_CLOUD });
        }

        void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode != AURA_REMOVE_BY_ENEMY_SPELL && removeMode != AURA_REMOVE_BY_EXPIRE)
                return;

            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(GetTarget(), SPELL_MUTAGEN_EXPLOSION, true);
                GetTarget()->CastSpell(GetTarget(), SPELL_POISON_CLOUD, true, nullptr, aurEff, GetCasterGUID());
            }
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_grobbulus_mutating_injection_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_grobbulus_mutating_injection_AuraScript();
    }
};

void AddSC_boss_grobbulus()
{
    new boss_grobbulus();
    new boss_grobbulus_poison_cloud();
    new spell_grobbulus_poison();
    new spell_grobbulus_mutating_injection();
}