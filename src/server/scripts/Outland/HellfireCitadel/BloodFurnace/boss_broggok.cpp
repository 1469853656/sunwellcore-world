
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "blood_furnace.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "MythicAI.h"

enum eEnums
{
    SAY_AGGRO = 0,

    SPELL_SLIME_SPRAY = 30913,
    SPELL_POISON_CLOUD = 30916,
    SPELL_POISON_BOLT = 30917,
    SPELL_POISON = 30914,

    EVENT_SPELL_SLIME = 1,
    EVENT_SPELL_POISON = 2,
    EVENT_SPELL_BOLT = 3,

    NPC_POISON_CLOUD_BROGGOK = 17662,

    //! Mythic
    NPC_BROGOKK_PORTAL_TRIGGER = 91515,
    NPC_BROGGOK_BEAM_CASTER = 91516,
    NPC_BROGGOK_BEAM_VICTIM = 91517,
    SPELL_BOGROKK_PORTAL_AURA = 40280,
    SPELL_BROGGOK_SHADOW_BLAST = 60646,
    SPELL_BROGGOK_RED_BEAM = 36295,
    SPELL_BROGGOK_PROFOUND_DARKNESS = 63420,
    SPELL_BROGGOK_NECROTIC_PURPLE_BEAM = 45491,
    NPC_BROGGOK_GAZER = 91520,
    SPELL_BROGGOK_MYTHIC_GLOWY = 22581
};

Position const BrogokkMythicHomePosition{ 456.426f, 49.4463f, 9.61676f, 1.58197f };
Position const BrogokkBeamCasterSpawnPosition{ 457.372f, 94.5087f, 46.4035f, 4.75012f };
constexpr uint32 DEMON_PORTAL_SPAWN_POSITION_SIZE{ 4 };
Position const DemonPortalSpawnPositions[DEMON_PORTAL_SPAWN_POSITION_SIZE] =
{
    { 418.393341f, 84.2856520f, 9.638652f, 6.2611330f },
    { 493.667877f, 84.3142470f, 9.638395f, 3.1784450f },
    { 419.782074f, 115.071228f, 9.635814f, 6.280768f  },
    { 493.069977f, 115.097626f, 9.637634f, 3.123469f  }
};

struct boss_broggok : public BossAI
{
    boss_broggok(Creature* creature) : BossAI(creature, DATA_BROGGOK)
    {
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 2.5f);
        darknessScheduled = true;
        if (instance && instance->IsMythic())
        {
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_KNOCKOUT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHACKLE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);

            darknessScheduled = false;
            me->SetHomePosition(BrogokkMythicHomePosition);
            me->Relocate(me->GetHomePosition());
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 4.5f);
        }
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        Position const center{ 454.589f, 94.8111f, 9.61502f, 6.23939f };
        return me->GetDistance(center) > 70.f;
    }

    void Reset() override
    {
        //_beamCasterGUID = 0;
        BossAI::Reset();
        darknessScheduled = true; // just make sure to set it to true on nonmythic
        if (instance->IsMythic())
        {
            darknessScheduled = false;
            me->SetAggressive();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
            for (uint32 entry : { GO_BROGOKK_PRISON_CELL_1, GO_BROGOKK_PRISON_CELL_2,
                GO_BROGOKK_PRISON_CELL_3, GO_BROGOKK_PRISON_CELL_4 })
            {
                GameObject* gate = instance->GetGameObject(entry);
                if (gate)
                    gate->SetGoState(GO_STATE_ACTIVE);
            }
            GameObject* lever = instance->GetGameObject(GO_BROGOKK_LEVER);
            if (lever && lever->IsVisible())
            {
                lever->SetPhaseMask(8, true);
                lever->EnableCollision(false);
            }

            for (Position const& pos : DemonPortalSpawnPositions)
            {
                Creature* trigger = me->SummonCreature(NPC_BROGOKK_PORTAL_TRIGGER, pos);
                if (trigger)
                {
                    _portalTriggers.push_back(trigger->GetGUID());
                    trigger->SetObjectScale(trigger->GetObjectSize() / 2.25f);
                    trigger->SetPassive();
                    trigger->SetImmuneToAll(true);
                    trigger->SetSelectable(false);
                    trigger->CastSpell(trigger, SPELL_BOGROKK_PORTAL_AURA, true);
                }
            }

            //Creature* beamCaster = me->SummonCreature(NPC_BROGGOK_BEAM_CASTER, BrogokkBeamCasterSpawnPosition);
            //if (beamCaster)
            //{
            //    beamCaster->SetCanFly(true);
            //    beamCaster->SetDisableGravity(true);
            //    _beamCasterGUID = beamCaster->GetGUID();

            //    beamCaster->CastSpell(beamCaster, SPELL_BROGGOK_MYTHIC_GLOWY, true);
            //}
        }
        else
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
        }

        canAttack = (instance && instance->IsMythic()) ? true : false;
        secondScheduler.CancelAll();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!darknessScheduled && me->HealthBelowPctDamaged(30, damage))
        {
            darknessScheduled = true;
            scheduler.Schedule(2s, [&](TaskContext func)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 1000);
                me->CastCustomSpell(SPELL_BROGGOK_PROFOUND_DARKNESS, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                func.Repeat(3s);
            });
        }
    }

    void ScheduleMythicAbilities()
    {
        secondScheduler.Schedule(7s, [&](TaskContext func)
        {
            Position const& pos = Trinity::Containers::SelectRandomContainerElement(DemonPortalSpawnPositions);
            Creature* gazer = me->SummonCreature(NPC_BROGGOK_GAZER, pos);
            if (gazer)
            {
                gazer->SetImmuneToAll(false);
                gazer->SetSelectable(true);
                gazer->AI()->DoZoneInCombat(gazer, 100.f);
            }
            func.Repeat(12500ms);
        });

        scheduler.Schedule(5s, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 900);
            val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1500);
            val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 300.f);
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 300.f, true);
            if (target)
                me->CastCustomSpell(SPELL_BROGGOK_SHADOW_BLAST, val, target);

            func.Repeat(17s);
        });

        //if (GetMythicKeyLevel() >= 7)
        //{
        //    //scheduler.Schedule(15s, [&](TaskContext func)
        //    //{
        //    //    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 0.f, true);
        //    //    if (target)
        //    //    {
        //    //        Position vicPos = target->GetPosition();
        //    //        vicPos.m_positionZ = 9.61f;
        //    //        Creature* victim = me->SummonCreature(NPC_BROGGOK_BEAM_VICTIM, vicPos);
        //    //        Creature* beamCaster = ObjectAccessor::GetCreature(*me, _beamCasterGUID);
        //    //        if (beamCaster && victim)
        //    //        {
        //    //            beamCaster->setFaction(me->getFaction());
        //    //            victim->setFaction(me->getFaction());
        //    //            victim->SetObjectScale(victim->GetObjectSize() / 6);
        //    //            //victim->SetImmuneToAll(true);
        //    //            victim->SetPassive();
        //    //            CustomSpellValues val;
        //    //            val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 150.f);
        //    //            beamCaster->CastCustomSpell(SPELL_BROGGOK_NECROTIC_PURPLE_BEAM, val, victim, TRIGGERED_NONE);
        //    //            victim->SetSpeedRate(MOVE_RUN, 0.9f);
        //    //            victim->AI()->SetGUID(target->GetGUID(), 0);
        //    //        }
        //    //    }
        //    //    func.Repeat(15s);
        //    //});
        //}
    }

    void EnterCombat(Unit* who) override
    {
        if ( !_EnterCombat() )
            return;

        Talk(SAY_AGGRO);
        if (instance->IsMythic())
        {
            ScheduleMythicAbilities();
            ScheduleNormalAbilities();
        }
        // else scheduled in DoAction
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BROGGOK_PROFOUND_DARKNESS);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 455.666443f, 100.607971f, 9.615189f, 4.731461f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() != NPC_POISON_CLOUD_BROGGOK)
        {
            summons.Summon(summoned);
            return;
        }

        BossAI::JustSummoned(summoned);
        summoned->CastSpell(summoned, SPELL_POISON, true);
        summoned->setFaction(16);
        summoned->SetFlag(UNIT_FIELD_FLAGS, (UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE));
        summoned->SetCanMissSpells(false);
    }

    void ScheduleNormalAbilities()
    {
        scheduler.Schedule(10s, [&](TaskContext func)
        {
            auto repeatTime = Seconds(urand(7, 12));
            if (!instance->IsMythic())
                DoCastVictim(SPELL_SLIME_SPRAY);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SLIME_SPRAY, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SLIME_SPRAY, val, target);

                repeatTime = 10s;
            }

            func.Repeat(repeatTime);
        });

        scheduler.Schedule(7s, [&](TaskContext func)
        {
            auto repeatTime = Seconds(urand(8, 12));
            if (!instance->IsMythic())
                DoCastAOE(SPELL_POISON_BOLT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_POISON_BOLT, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_POISON_BOLT, val, (Unit*)nullptr);
            }

            func.Repeat(repeatTime);
        });

        scheduler.Schedule(5s, [&](TaskContext func)
        {
            DoCastAOE(SPELL_POISON_CLOUD, true);
            if (!instance->IsMythic())
                func.Repeat(17s);
            else
                func.Repeat(15s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || !canAttack)
            return;

        secondScheduler.Update(diff);
        scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_PREPARE_BROGGOK:
                me->SetInCombatWithZone();
                break;
            case ACTION_ACTIVATE_BROGGOK:
                ScheduleNormalAbilities();
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                canAttack = true;
                break;
        }
    }
private:
    bool canAttack;
    bool darknessScheduled;

    std::vector<uint64> _portalTriggers;
    //uint64 _beamCasterGUID;
    TaskScheduler secondScheduler;
};

class go_broggok_lever : public GameObjectScript
{
    public:
        go_broggok_lever() : GameObjectScript("go_broggok_lever") {}

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_BROGGOK) != DONE && instance->GetBossState(DATA_BROGGOK) != IN_PROGRESS)
                    if (Creature* broggok = instance->GetCreature(DATA_BROGGOK))
                        broggok->AI()->DoAction(ACTION_PREPARE_BROGGOK);
            }

            go->UseDoorOrButton();
            return false;
        }
};

// 30914, 38462 - Poison (Broggok)
constexpr uint32 SPELL_BROGGOK_POISON_TRIGGERED{ 30915 };
class spell_broggok_poison_cloud : public AuraScript
{
    PrepareAuraScript(spell_broggok_poison_cloud);

    bool Validate(SpellInfo const* spellInfo) override
    {
        if (!sSpellMgr->GetSpellInfo(spellInfo->Effects[EFFECT_0].TriggerSpell))
            return false;
        return true;
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        PreventDefaultAction();

        uint32 triggerSpell = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
        int32 mod = int32(((float(aurEff->GetTickNumber()) / aurEff->GetTotalTicks()) * 0.9f + 0.1f) * 10000 * 2 / 3);
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_RADIUS_MOD, mod);

        InstanceScript* instance = GetTarget()->GetInstanceScript();
        bool const _IsMythic = instance ? instance->IsMythic() : false;
        if (instance && _IsMythic)
        {
            Mythic::MythicSpells data;
            //! SPELL_BROGGOK_POISON_TRIGGERED spells equal to normal mode spellId
            //! heroic should never trigger nor assert, there is no mythic for heroic
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BROGGOK_POISON_TRIGGERED, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
        }

        GetTarget()->CastCustomSpell(triggerSpell, val, (Unit*)nullptr, TRIGGERED_FULL_MASK, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_broggok_poison_cloud::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

constexpr uint32 SPELL_NASCENT_STOMP{ 31900 };
constexpr uint32 SPELL_NASCENT_CONCUSSION_BLOW{ 22427 };
constexpr uint32 TALK_NASCENT_AGGRO{ 0 };
struct npc_nascent_orc_blood_furnace : public MythicAI
{
    npc_nascent_orc_blood_furnace(Creature* creature) : MythicAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        Talk(TALK_NASCENT_AGGRO);

        task.Schedule(Milliseconds(urand(7000, 8200)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(15800, 20800));
            if (!IsMythic())
            {
                if (Unit* target = me->GetVictim())
                {
                    if (target->GetDistance(me) <= 5.f)
                        DoCastAOE(SPELL_NASCENT_STOMP, true);
                }
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_NASCENT_STOMP, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                if (Unit* target = me->GetVictim())
                {
                    if (target->GetDistance(me) <= 5.f)
                        me->CastCustomSpell(SPELL_NASCENT_STOMP, val, (Unit*)nullptr);
                }
                repeatTime = Milliseconds(urand(5000, 11000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1200, 7800)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(10800);
            if (!IsMythic())
                DoCastVictim(SPELL_NASCENT_CONCUSSION_BLOW);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_NASCENT_CONCUSSION_BLOW, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_NASCENT_CONCUSSION_BLOW, val, target);
                repeatTime = Milliseconds(12000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_BROGGOK_VICTIM_ARCANE_TEMPEST{ 35845 };
struct npc_broggok_beam_victim : public PassiveAI
{
    npc_broggok_beam_victim(Creature* creature) : PassiveAI(creature)
    {
        _targetGUID = 0;
        me->SetPassive();
        instance = me->GetInstanceScript();
        if (!instance)
            me->DespawnOrUnsummon();
    }

    void Reset() override
    {
        me->DespawnOrUnsummon(10s);
        _targetGUID = 0;
        scheduler.CancelAll();
        scheduler.Schedule(1s, [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BROGGOK_VICTIM_ARCANE_TEMPEST, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            me->CastCustomSpell(SPELL_BROGGOK_VICTIM_ARCANE_TEMPEST, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            func.Repeat(1500ms);
        });
    }

    void SetGUID(uint64 guid, int32 data) override
    {
        if (!guid)
            return;

        _targetGUID = guid;

        scheduler.Schedule(100ms, [&](TaskContext func)
        {
            Player* target = ObjectAccessor::GetPlayer(*me, _targetGUID);
            if (target)
            {
                me->GetMotionMaster()->MovePoint(1, target->GetPosition());
                func.Repeat(1s);
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
private:
    TaskScheduler scheduler;
    uint64 _targetGUID;
    InstanceScript* instance;
};

enum GazerSpells
{
    SPELL_BROGGOK_GAZER_DRAIN_LIFE = 46291,
    SPELL_BROGGOK_GAZER_TENTACLE_SWEEP = 46290
};

struct npc_broggok_gazer_mythic : public ScriptedAI
{
    npc_broggok_gazer_mythic(Creature* creature) : ScriptedAI(creature)
    {
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 2.5f);
        scheduler.SetValidator([this]
        {
            return !me->IsCasting();
        });

        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_KNOCKOUT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHACKLE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
    }

    void Reset() override
    {
        ScriptedAI::Reset();
        scheduler.CancelAll();
    }

    void EnterCombat(Unit* who) override
    {
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
        ScriptedAI::EnterCombat(who);

        scheduler.Schedule(randseconds(6, 8), [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            if (target)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 6000);
                me->CastCustomSpell(SPELL_BROGGOK_GAZER_DRAIN_LIFE, val, target);
            }

            func.Repeat(randseconds(15, 19));
        });

        scheduler.Schedule(3s, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT1, 100);
            me->CastCustomSpell(SPELL_BROGGOK_GAZER_TENTACLE_SWEEP, val, (Unit*)nullptr);

            func.Repeat(randseconds(3, 8));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff, std::bind(&ScriptedAI::DoMeleeAttackIfReady, this));
    }

private:
    TaskScheduler scheduler;
};

void AddSC_boss_broggok()
{
    new go_broggok_lever();

    RegisterCreatureAI(boss_broggok);
    RegisterAuraScript(spell_broggok_poison_cloud);
    RegisterCreatureAI(npc_nascent_orc_blood_furnace);
    RegisterCreatureAI(npc_broggok_gazer_mythic);

    //! Mythic
    RegisterCreatureAI(npc_broggok_beam_victim);
}
