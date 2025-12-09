#include "nexus.h"

#include "ScriptedCreature.h"
#include "MythicAI.h"
#include "MoveSplineInit.h"
#include "CreatureGroups.h"

struct NexusAI : public MythicAI
{
    NexusAI(Creature* creature) : MythicAI(creature) { }

    //! override a function if needed, mythicAI derives from scriptedAI
    //! check MythicAI class to see which functions are overriden
};

enum NexusFrayer
{
    SPELL_SUMMON_SEED_POD           = 52796,
    SPELL_SEED_POD                  = 48082,
    SPELL_AURA_OF_REGENERATION      = 52067,
    SPELL_CRYSTAL_BLOOM             = 48058,
    SPELL_ENSNARE                   = 48053
};

struct npc_crystalline_frayer : public ScriptedAI
{
    npc_crystalline_frayer(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        scheduler.CancelAll();
        me->RemoveFlag( UNIT_FIELD_FLAGS, ( UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC ) );
    }

    void EnterCombat(Unit* /*who*/) override
    {
        if ( !me->GetInstanceScript() )
        {
            me->DespawnOrUnsummon();
            return;
        }
        InstanceScript* instance = me->GetInstanceScript();
        _allowDeath = instance->GetBossState(DATA_ORMOROK_EVENT) == DONE;

        if ( instance->IsMythic() && me->m_mythicScaling )
            me->m_mythicScaling->m_affixesAllowed = false;

        scheduler.Schedule(5s, [&](TaskContext func)
        {
            DoCastVictim(SPELL_ENSNARE, false);
            func.Repeat(5s);
        });

        scheduler.Schedule(30s, [&](TaskContext func)
        {
            DoCastVictim(SPELL_CRYSTAL_BLOOM, false);
            func.Repeat(30s);
        });
    }

    void EnterEvadeMode() override
    {
        if (me->isRegeneratingHealth())
            ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (damage >= me->GetHealth())
        {
            if (!_allowDeath)
            {
                me->SetPassive();
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
                damage = 0;

                me->SetFlag( UNIT_FIELD_FLAGS, ( UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC ) );
                me->SetRegeneratingHealth(false);
                me->CastSpell(me, SPELL_SUMMON_SEED_POD, true);
                me->CastSpell(me, SPELL_SEED_POD, true);
                me->CastSpell(me, SPELL_AURA_OF_REGENERATION, false);

                scheduler.CancelAll();
                scheduler.Schedule(90s, [&](TaskContext func)
                {
                    Talk(0);
                    me->SetRegeneratingHealth(true);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, ( UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC ) );
                });
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
private:
    bool _allowDeath;

    TaskScheduler scheduler;
};


constexpr uint32 SPELL_AZURE_WARDER_SILENCE{ 6726 };
constexpr uint32 SPELL_AZURE_WARDER_SILENCE_HEROIC{ 56777 };
constexpr uint32 SPELL_AZURE_WARDER_MANA_SHIELD{ 17741 };
constexpr uint32 SPELL_AZURE_WARDER_MANA_SHIELD_HEROIC{ 56778 };

struct npc_azure_warder_nexus : public NexusAI
{
    npc_azure_warder_nexus(Creature* creature) : NexusAI(creature) { }

    void Reset() override
    {
        NexusAI::Reset();
        _manaShieldScheduled = false;
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(IsMythic() ? 10000 : 15000);
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
            {
                return obj->IsCasting();
            });

            if (!target)
                target = SelectTarget(SELECT_TARGET_RANDOM);

            if (target)
            {
                if (IsMythic())
                    DoCast(target, SPELL_AZURE_WARDER_SILENCE, true);
                else
                    DoCast(target, IsHeroic() ? SPELL_AZURE_WARDER_SILENCE_HEROIC : SPELL_AZURE_WARDER_SILENCE, true);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_manaShieldScheduled && me->HealthBelowPctDamaged(IsMythic() ? 60 : 40, damage))
        {
            _manaShieldScheduled = true;
            task.Schedule(1s, [&](TaskContext func)
            {
                if (IsMythic())
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_AZURE_WARDER_MANA_SHIELD, data));
                    float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    me->CastCustomSpell(SPELL_AZURE_WARDER_MANA_SHIELD, val, me);
                }
                else
                    DoCastSelf(IsHeroic() ? SPELL_AZURE_WARDER_MANA_SHIELD_HEROIC : SPELL_AZURE_WARDER_MANA_SHIELD);
                func.Repeat(30s);
            });
        }
    }
private:
    bool _manaShieldScheduled;
};

enum AzureMagus
{
    SPELL_AZURE_MAGUS_FROSTBOLT_NORMAL = 15530,
    SPELL_AZURE_MAGUS_FROSTBOLT_HEROIC = 56775,
    SPELL_AZURE_MAGUS_ARCANE_SHOCK_NORMAL = 37132,
    SPELL_AZURE_MAGUS_ARCANE_SHOCK_HEROIC = 56776
};

struct npc_azure_magus_nexus : public NexusAI
{
    npc_azure_magus_nexus(Creature* creature) : NexusAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);

        task.Schedule(2s, [&](TaskContext func)
        {
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_AZURE_MAGUS_FROSTBOLT_HEROIC : SPELL_AZURE_MAGUS_FROSTBOLT_NORMAL);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_AZURE_MAGUS_FROSTBOLT_NORMAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_AZURE_MAGUS_FROSTBOLT_NORMAL, val, target);
            }
            func.Repeat(2s);
        });

        task.Schedule(IsMythic() ? 3s : randseconds(10, 15), [&](TaskContext func)
        {
            if (!IsMythic())
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 20.f, true);
                //! DoCast does its own null checks
                DoCast(target, IsHeroic() ? SPELL_AZURE_MAGUS_ARCANE_SHOCK_HEROIC :
                    SPELL_AZURE_MAGUS_ARCANE_SHOCK_NORMAL);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_AZURE_MAGUS_ARCANE_SHOCK_NORMAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 20.f, true);
                if (target)
                    me->CastCustomSpell(SPELL_AZURE_MAGUS_ARCANE_SHOCK_NORMAL, val, target);
            }

            func.Repeat(IsMythic() ? randseconds(7, 10) : 15s);
        });
    }
};

constexpr uint32 MAGE_HUNTER_ASCENDANT_SPELL_SET_SIZE{ 3 };
enum AscendantNexusSpells
{
    // arcane spell set
    SPELL_ASCENDANT_ARCANE_HASTE                = 50182,
    SPELL_ASCENDANT_ARCANE_HASTE_HEROIC         = 56827,
    SPELL_ASCENDANT_ARCANE_EXPLOSION_PROC_AURA  = 35191,
    SPELL_ASCENDANT_ARCANE_TRAP                 = 47789,
    SPELL_ASCENDANT_POLYMORPH                   = 13323,

    // frost spell set
    SPELL_ASCENDANT_FROSTBOLT                   = 12737,
    SPELL_ASCENDANT_FROSTBOLT_HEROIC            = 56837,
    SPELL_ASCENDANT_CONE_OF_COLD                = 15244,
    SPELL_ASCENDANT_CONE_OF_COLD_HEROIC         = 38384,
    SPELL_ASCENDANT_FREEZING_TRAP               = 55040,

    // fire spell set
    SPELL_ASCENDANT_FIREBALL                    = 12466,
    SPELL_ASCENDANT_FIREBALL_HEROIC             = 17290,
    SPELL_ASCENDANT_RAIN_OF_FIRE                = 36808,
    SPELL_ASCENDANT_RAIN_OF_FIRE_HEROIC         = 39376,
    SPELL_ASCENDANT_IMMOLATION_TRAP             = 47784
};

enum AscendantSpellSetIDs
{
    ASCENDANT_SPELL_SET_ARCANE                  = 0,
    ASCENDANT_SPELL_SET_FROST,
    ASCENDANT_SPELL_SET_FIRE
};

struct npc_mage_hunter_ascendant_nexus : public NexusAI
{
    npc_mage_hunter_ascendant_nexus(Creature* creature) : NexusAI(creature)
    {
        _mySpellSetId = urand(0, MAGE_HUNTER_ASCENDANT_SPELL_SET_SIZE - 1);
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        ScheduleTasksForMySpellSet();
    }

    void AttackStart(Unit* who) override
    {
        if (_mySpellSetId == ASCENDANT_SPELL_SET_ARCANE)
            ScriptedAI::AttackStart(who);
        else
            MythicAI::AttackStart(who);
    }

    void ScheduleTasksForMySpellSet()
    {
        switch (_mySpellSetId)
        {
            case ASCENDANT_SPELL_SET_ARCANE:
            {
                DoCastSelf(SPELL_ASCENDANT_ARCANE_EXPLOSION_PROC_AURA);

                task.Schedule(Milliseconds(urand(5000, 10000)), [&](TaskContext func)
                {
                    auto repeatTime = 60000ms;
                    if (IsMythic())
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ASCENDANT_ARCANE_HASTE, data));
                        float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                        me->CastCustomSpell(SPELL_ASCENDANT_ARCANE_HASTE, val, me);
                        repeatTime = 45000ms;
                    }
                    else
                        DoCastSelf(IsHeroic() ? SPELL_ASCENDANT_ARCANE_HASTE_HEROIC : SPELL_ASCENDANT_ARCANE_HASTE);

                    func.Repeat(repeatTime);
                });

                task.Schedule(1ms, [&](TaskContext func)
                {
                    DoCastAOE(SPELL_ASCENDANT_ARCANE_TRAP);

                    if (IsMythic())
                        func.Repeat(30000ms);
                    else
                        func.Repeat(30000ms, 40000ms);
                });

                task.Schedule(Milliseconds(urand(3000, 9000)), [&](TaskContext func)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 30.f, true, -SPELL_ASCENDANT_POLYMORPH);
                    if (target)
                        DoCast(target, SPELL_ASCENDANT_POLYMORPH);

                    if (!IsMythic())
                        func.Repeat(30s);
                    else
                        func.Repeat(randseconds(15, 25));
                });

                break;
            }
            case ASCENDANT_SPELL_SET_FROST:
            {
                task.Schedule(3s, [&](TaskContext func)
                {
                    if (IsMythic())
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ASCENDANT_FROSTBOLT, data));
                        float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        if (Unit* target = me->GetVictim())
                            me->CastCustomSpell(SPELL_ASCENDANT_FROSTBOLT, val, target);
                    }
                    else
                        DoCastVictim(IsHeroic() ? SPELL_ASCENDANT_FROSTBOLT_HEROIC : SPELL_ASCENDANT_FROSTBOLT);

                    func.Repeat(3500ms);
                });

                task.Schedule(Milliseconds(urand(4000, 9000)), [&](TaskContext func)
                {
                    auto repeatTime = Milliseconds(urand(12000, 125000));
                    if (IsMythic())
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ASCENDANT_CONE_OF_COLD, data));
                        float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                        if (Unit* target = me->GetVictim())
                            me->CastCustomSpell(SPELL_ASCENDANT_CONE_OF_COLD, val, target);

                        repeatTime = 8000ms;
                    }
                    else
                        DoCastVictim(IsHeroic() ? SPELL_ASCENDANT_CONE_OF_COLD_HEROIC : SPELL_ASCENDANT_CONE_OF_COLD);

                    func.Repeat(repeatTime);
                });

                task.Schedule(1ms, [&](TaskContext func)
                {
                    DoCastAOE(SPELL_ASCENDANT_FREEZING_TRAP);

                    if (IsMythic())
                        func.Repeat(30000ms);
                    else
                        func.Repeat(30000ms, 40000ms);
                });
                break;
            }
            case ASCENDANT_SPELL_SET_FIRE:
            {

                task.Schedule(randseconds(3, 5), [&](TaskContext func)
                {
                    auto repeat = randseconds(5, 7);
                    if (IsMythic())
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ASCENDANT_FIREBALL, data));
                        float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        if (Unit* target = me->GetVictim())
                            me->CastCustomSpell(SPELL_ASCENDANT_FIREBALL, val, target);

                        repeat = randseconds(3, 5);
                    }
                    else
                        DoCastVictim(IsHeroic() ? SPELL_ASCENDANT_FIREBALL_HEROIC : SPELL_ASCENDANT_FIREBALL);

                    func.Repeat(repeat);
                });

                task.Schedule(Milliseconds(urand(4000, 9000)), [&](TaskContext func)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 30.f, true);
                    if (target)
                    {
                        if (IsMythic())
                        {
                            Mythic::MythicSpells data;
                            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ASCENDANT_RAIN_OF_FIRE, data));
                            float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                            if (Unit* target = me->GetVictim())
                                me->CastCustomSpell(SPELL_ASCENDANT_RAIN_OF_FIRE, val, target);
                        }
                        else
                            DoCastVictim(IsHeroic() ? SPELL_ASCENDANT_RAIN_OF_FIRE_HEROIC : SPELL_ASCENDANT_RAIN_OF_FIRE);
                    }
                });

                task.Schedule(1ms, [&](TaskContext func)
                {
                    DoCastAOE(SPELL_ASCENDANT_IMMOLATION_TRAP);

                    if (IsMythic())
                        func.Repeat(30000ms);
                    else
                        func.Repeat(30000ms, 40000ms);
                });
                break;
            }
        }
    }
private:
    uint32 _mySpellSetId;
};

class spell_arcane_explosion_nexus : public AuraScript
{
    PrepareAuraScript(spell_arcane_explosion_nexus);

    void HandlePeriodic(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        uint32 spellId = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (!instance)
            return;

        if (!instance->IsMythic())
            return;

        PreventDefaultAction();
        Mythic::MythicSpells data;
        ASSERT(sMythicMgr.GetSpellValuesFor(spellId, data));
        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
        GetCaster()->CastCustomSpell(spellId, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_arcane_explosion_nexus::HandlePeriodic, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};


constexpr uint32 SPELL_MAGE_SLAYER_SPELL_LOCK{ 30849 };
constexpr uint32 SPELL_MAGE_SLAYER_DRAW_MAGIC{ 50131 };

struct npc_mage_slayer_nexus : public MythicAI
{
    npc_mage_slayer_nexus(Creature* creature) : MythicAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        me->GetMotionMaster()->MoveIdle();
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 6000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(IsMythic() ? 9000 : 12000);
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
            {
                return obj->IsCasting();
            });

            if (target)
                DoCast(target, SPELL_MAGE_SLAYER_SPELL_LOCK);
            else
            {
                if (IsMythic())
                    repeatTime = 10000ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(15000, 25000));
            if (!IsMythic())
                DoCastSelf(SPELL_MAGE_SLAYER_DRAW_MAGIC);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MAGE_SLAYER_DRAW_MAGIC, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_MAGE_SLAYER_DRAW_MAGIC, val, me);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_RANGER_NEXUS_RAPID_SHOT{ 48191 };
constexpr uint32 SPELL_RANGER_NEXUS_SHOOT{ 15620 };
constexpr uint32 SPELL_RANGER_NEXUS_SHOOT_HEROIC{ 22907 };

constexpr uint32 SPELL_RANGER_NEXUS_INCENDIARY_SHOT{ 47777 };
constexpr uint32 SPELL_RANGER_NEXUS_INCENDIARY_SHOT_HEROIC{ 56933 };

constexpr uint32 SPELL_RANGER_NEXUS_FROZEN{ 47543 };

struct npc_alliance_ranger_nexus : public NexusAI
{
    npc_alliance_ranger_nexus(Creature* creature) : NexusAI(creature) { }

    void Reset() override
    {
        if (!me->GetDBTableGUIDLow() && IsMythic())
            DoCastSelf(SPELL_RANGER_NEXUS_FROZEN, true);
        NexusAI::Reset();
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        me->RemoveAurasDueToSpell(SPELL_RANGER_NEXUS_FROZEN);

        task.Schedule(Milliseconds(urand(7000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(20000, 30000));
            if (!IsMythic())
                DoCastSelf(SPELL_RANGER_NEXUS_RAPID_SHOT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_RANGER_NEXUS_RAPID_SHOT, data));
                CustomSpellValues val;
                if (Unit* target = me->GetVictim())
                {
                    if (target->GetDistance(me) < 8.5f)
                    {
                        me->CastCustomSpell(SPELL_RANGER_NEXUS_RAPID_SHOT, val, me);
                        repeatTime = Milliseconds(10000);
                    }
                    else
                        repeatTime = Milliseconds(6000);
                }
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(3000, 15000)), [&](TaskContext func)
        {
            auto repeatTime = 15000ms;
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_RANGER_NEXUS_INCENDIARY_SHOT_HEROIC : SPELL_RANGER_NEXUS_INCENDIARY_SHOT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_RANGER_NEXUS_INCENDIARY_SHOT, data));
                CustomSpellValues val;
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                me->CastCustomSpell(SPELL_RANGER_NEXUS_INCENDIARY_SHOT, val, me);
                repeatTime = Milliseconds(10000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(2000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(2000);
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_RANGER_NEXUS_SHOOT_HEROIC : SPELL_RANGER_NEXUS_SHOOT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_RANGER_NEXUS_SHOOT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_RANGER_NEXUS_SHOOT, val, target);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

class spell_nexus_rapid_shot_periodic : public AuraScript
{
    PrepareAuraScript(spell_nexus_rapid_shot_periodic);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (!GetOwner())
            return;

        Creature* creature = GetOwner()->ToCreature();
        InstanceScript* instance = GetOwner()->GetInstanceScript();
        if (instance && instance->IsMythic() && creature)
        {
            PreventDefaultAction();
            CustomSpellValues val;
            auto damage = Mythic::CalculateMythicValue(5000, 0.04f);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
            uint32 spellId = aurEff->GetSpellInfo()->Effects[0].TriggerSpell;
            if (Unit* victim = creature->GetVictim())
                creature->CastCustomSpell(spellId, val, victim, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_nexus_rapid_shot_periodic::HandlePeriodic, EFFECT_0, SPELL_AURA_ANY);
    }
};

enum AllianceClericSpells
{
    SPELL_ALLIANCE_CLERIC_FLASH_HEAL = 17843,
    SPELL_ALLIANCE_CLERIC_FLASH_HEAL_HEROIC = 56919,
    SPELL_ALLIANCE_CLERIC_PWD_SHIELD = 17139,
    SPELL_ALLIANCE_CLERIC_PWD_SHIELD_HEROIC = 35944,
    SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH = 47697,
    SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH_HEROIC = 56920
};

struct npc_alliance_cleric_nexus : public NexusAI
{
    npc_alliance_cleric_nexus(Creature* creature) : NexusAI(creature) { }

    void Reset() override
    {
        if (!me->GetDBTableGUIDLow() && IsMythic())
            DoCastSelf(SPELL_RANGER_NEXUS_FROZEN, true);
        NexusAI::Reset();
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        me->RemoveAurasDueToSpell(SPELL_RANGER_NEXUS_FROZEN);

        task.Schedule(Milliseconds(3000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(3000);
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(30.f, 1U))
                    DoCast(target, IsHeroic() ? SPELL_ALLIANCE_CLERIC_FLASH_HEAL_HEROIC : SPELL_ALLIANCE_CLERIC_FLASH_HEAL);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ALLIANCE_CLERIC_FLASH_HEAL, data));
                float variance = 0.05f;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(2000, 0.05f));
                if (Unit* target = DoSelectLowestHpFriendly(30.f, 1U))
                    me->CastCustomSpell(SPELL_ALLIANCE_CLERIC_FLASH_HEAL, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(2000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(6000);
            if (!IsMythic())
            {
                std::list<Creature*> _creatureList = DoFindFriendlyMissingBuff(40.f, SPELL_ALLIANCE_CLERIC_PWD_SHIELD);
                if (!_creatureList.empty())
                {
                    Creature* target = Trinity::Containers::SelectRandomContainerElement(_creatureList);
                    if (target)
                        DoCast(IsHeroic() ? SPELL_ALLIANCE_CLERIC_PWD_SHIELD_HEROIC : SPELL_ALLIANCE_CLERIC_PWD_SHIELD);
                    repeatTime = Milliseconds(6000);
                }
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ALLIANCE_CLERIC_PWD_SHIELD, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                std::list<Creature*> _creatureList = DoFindFriendlyMissingBuff(40.f, SPELL_ALLIANCE_CLERIC_PWD_SHIELD);
                if (!_creatureList.empty())
                {
                    Creature* target = Trinity::Containers::SelectRandomContainerElement(_creatureList);
                    if (target)
                        me->CastCustomSpell(SPELL_ALLIANCE_CLERIC_PWD_SHIELD, val, target);
                    repeatTime = Milliseconds(6000);
                }
                else
                    repeatTime = 1500ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(3000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(15000);
            if (!IsMythic())
            {
                int32 spellId = static_cast<int32>(IsHeroic() ? SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH_HEROIC : SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, false, -spellId))
                    DoCast(target, spellId);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, false, -SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH))
                    me->CastCustomSpell(SPELL_ALLIANCE_CLERIC_SHADOW_WORD_DEATH, val, target);
                repeatTime = Milliseconds(11000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_STEWARD_SPELLBREAKER{ 47780 };
constexpr uint32 SPELL_STEWARD_ARCANE_TORRENT{ 47779 };
struct npc_steward_nexus : public NexusAI
{
    npc_steward_nexus(Creature* creature) : NexusAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        me->RemoveAurasDueToSpell(SPELL_RANGER_NEXUS_FROZEN);

        task.Schedule(Milliseconds(urand(1000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 12000));
            if (!IsMythic())
                DoCastVictim(SPELL_STEWARD_SPELLBREAKER);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_STEWARD_SPELLBREAKER, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_STEWARD_SPELLBREAKER, val, target);
                repeatTime = Milliseconds(6500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 15000));
            DoCastAOE(SPELL_STEWARD_ARCANE_TORRENT);
            if (IsMythic())
                repeatTime = Milliseconds(urand(10000, 12000));

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_MAGE_HUNTER_INITIATE_RENEW{ 25058 };
constexpr uint32 SPELL_MAGE_INITIATE_MANA_BURN{ 50198 };
constexpr uint32 SPELL_MAGE_INITIATE_DRAIN_MANA{ 17682 };

struct npc_mage_hunter_initiate_nexus : public NexusAI
{
    npc_mage_hunter_initiate_nexus(Creature* creature) : NexusAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(3000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(9000);
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(30.f, 10U))
                    DoCast(target, SPELL_MAGE_HUNTER_INITIATE_RENEW);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MAGE_HUNTER_INITIATE_RENEW, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = DoSelectLowestHpFriendly(30.f, 10U))
                {
                    me->CastCustomSpell(SPELL_MAGE_HUNTER_INITIATE_RENEW, val, target);
                    repeatTime = Milliseconds(urand(6000, 9000));
                }
                else
                    repeatTime = 1000ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(5000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(7000, 9000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    DoCast(target, SPELL_MAGE_INITIATE_MANA_BURN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MAGE_INITIATE_MANA_BURN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    me->CastCustomSpell(SPELL_MAGE_INITIATE_MANA_BURN, val, target);
                repeatTime = Milliseconds(urand(10000, 12500));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(15000, 25000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    DoCast(target, SPELL_MAGE_INITIATE_DRAIN_MANA);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MAGE_INITIATE_DRAIN_MANA, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;

                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    me->CastCustomSpell(SPELL_MAGE_INITIATE_DRAIN_MANA, val, target);
                repeatTime = Milliseconds(urand(9000, 15000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};


constexpr uint32 SPELL_TELESTRA_SCORCH{ 47723 };
constexpr uint32 SPELL_TELESTRA_FIREBLAST{ 47721 };
constexpr uint32 SPELL_TELESTRA_CLONE_DIES_FIRE{ 47711 };

struct npc_grand_magus_telestra_fire : public NexusAI
{
    npc_grand_magus_telestra_fire(Creature* creature) : NexusAI(creature) { }

    void JustDied(Unit* killer) override
    {
        NexusAI::JustDied(killer);
        DoCastSelf(SPELL_TELESTRA_CLONE_DIES_FIRE, true);
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(1000, 3000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(3000);
            if (!IsMythic())
            {
                DoCastVictim(SPELL_TELESTRA_SCORCH);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_TELESTRA_SCORCH, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;

                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, true))
                    me->CastCustomSpell(SPELL_TELESTRA_SCORCH, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 4000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 10000));
            if (!IsMythic())
                DoCastVictim(SPELL_TELESTRA_FIREBLAST);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_TELESTRA_FIREBLAST, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_TELESTRA_FIREBLAST, val, target);
                repeatTime = Milliseconds(urand(8000, 9000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_TELESTRA_TIMESTOP{ 47736 };
constexpr uint32 SPELL_TELESTRA_CRITTER{ 47731 };
constexpr uint32 SPELL_TELESTRA_DEATH_TIMESTOPPER{ 47713 };
struct npc_grand_magus_telestra_time : public NexusAI
{
    npc_grand_magus_telestra_time(Creature* creature) : NexusAI(creature) { }

    void JustDied(Unit* killer) override
    {
        NexusAI::JustDied(killer);
        DoCastSelf(SPELL_TELESTRA_DEATH_TIMESTOPPER, true);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(2s, [&](TaskContext func)
        {
            DoCastAOE(SPELL_TELESTRA_TIMESTOP);
            task.Schedule(2s, [&](TaskContext /*func*/)
            {
                DoCastAOE(SPELL_TELESTRA_CRITTER);
            });

            func.Repeat(12s);
        });
    }
};

constexpr uint32 SPELL_TELESTRA_ICEBARB{ 47729 };
constexpr uint32 SPELL_TELESTRA_ICEBARB_HEROIC{ 56937 };
constexpr uint32 SPELL_TELESTRA_BLIZZARD{ 47727 };
constexpr uint32 SPELL_TELESTRA_BLIZZARD_HEROIC{ 56936 };
constexpr uint32 SPELL_TELESTRA_FROST_DEATH{ 47712 };
struct npc_grand_telestra_frost : public NexusAI
{
    npc_grand_telestra_frost(Creature* creature) : NexusAI(creature) { }

    void JustDied(Unit* killer) override
    {
        NexusAI::JustDied(killer);
        DoCastSelf(SPELL_TELESTRA_FROST_DEATH, true);
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(3000);
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_TELESTRA_ICEBARB_HEROIC : SPELL_TELESTRA_ICEBARB);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_TELESTRA_ICEBARB, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_TELESTRA_ICEBARB, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(2000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 12000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                    DoCast(target, IsHeroic() ? SPELL_TELESTRA_BLIZZARD_HEROIC : SPELL_TELESTRA_BLIZZARD);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_TELESTRA_BLIZZARD, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.f, true))
                    me->CastCustomSpell(SPELL_TELESTRA_BLIZZARD, val, target);
                repeatTime = Milliseconds(6000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};


constexpr uint32 SPELL_ENFORCER_CLEAVE{ 15284 };
constexpr uint32 SPELL_ENFORCER_BRUTAL_STRIKE{ 58460 };

struct npc_azure_enforcer_nexus : public MythicAI
{
    npc_azure_enforcer_nexus(Creature* creature) : MythicAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(1000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 12000));
            if (!IsMythic())
                DoCastVictim(SPELL_ENFORCER_CLEAVE);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ENFORCER_CLEAVE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_ENFORCER_CLEAVE, val, target);
                repeatTime = Milliseconds(urand(9000, 10500));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 4000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 15000));
            if (!IsMythic())
                DoCastVictim(SPELL_ENFORCER_BRUTAL_STRIKE);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ENFORCER_BRUTAL_STRIKE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_ENFORCER_BRUTAL_STRIKE, val, target);
                repeatTime = Milliseconds(urand(4000, 10000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};


constexpr uint32 SPELL_BINDER_HEAL{ 15586 };
constexpr uint32 SPELL_BINDER_HEAL_HEROIC{ 61326 };
constexpr uint32 SPELL_BINDER_ARCANE_BLAST{ 38881 };
constexpr uint32 SPELL_BINDER_ARCANE_BLAST_HEROIC{ 56969 };

struct npc_azure_scale_binder : public NexusAI
{
    npc_azure_scale_binder(Creature* creature) : NexusAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(6000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(6000);
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10U))
                    DoCast(target, IsHeroic() ? SPELL_BINDER_HEAL_HEROIC : SPELL_BINDER_HEAL);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BINDER_HEAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10U))
                {
                    me->CastCustomSpell(SPELL_BINDER_HEAL, val, target);
                    repeatTime = Milliseconds(8000);
                }
                else
                    repeatTime = 1000ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 3000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(3000);
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_BINDER_ARCANE_BLAST_HEROIC : SPELL_BINDER_ARCANE_BLAST);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BINDER_ARCANE_BLAST, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_BINDER_ARCANE_BLAST, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_CRAZED_MANA_WRAITH_ARCANE_MISSILES{ 33832 };
constexpr uint32 SPELL_CRAZED_MANA_WRAITH_TELEPORT_VISUAL{ 51347 };
struct npc_crazed_mana_wraith_nexus : public NexusAI
{
    npc_crazed_mana_wraith_nexus(Creature* creature) : NexusAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        NexusAI::IsSummonedBy(summoner);
        DoCastSelf(SPELL_CRAZED_MANA_WRAITH_TELEPORT_VISUAL);
    }

    void Reset() override
    {
        NexusAI::Reset();
        me->GetMotionMaster()->MoveRandom(10.f);
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(randseconds(5, 13), [&](TaskContext func)
        {
            DoCastVictim(SPELL_CRAZED_MANA_WRAITH_ARCANE_MISSILES);
            func.Repeat(randseconds(9, 16));
        });
    }
};

constexpr uint32 SPELL_RIFT_SUMMON_WRAITH{ 47692 };
constexpr uint32 SPELL_RIFT_CHAOTIC_RIFT{ 47687 };
constexpr uint32 SPELL_RIFT_CHARGED_CHAOTIC_RIFT{ 47733 };
constexpr uint32 SPELL_RIFT_CHARGE_RIFT{ 47747 };
constexpr uint32 SPELL_RIFT_CHAOTIC_RIFT_AURA_TRIGGERED{ 47688 };
constexpr uint32 NPC_CRAZED_MANA_WRAITH{ 26746 };
struct npc_chaotic_rift_nexus : public NexusAI
{
    npc_chaotic_rift_nexus(Creature* creature) : NexusAI(creature), summons(me)
    {
        instance = me->GetInstanceScript();
        me->SetPassive();
    }

    void Reset() override
    {
        summons.DespawnAll();
        _wraithRateIncreased = false;
        task.CancelAll();

        task.Schedule(1s, [&](TaskContext func)
        {
            bool shouldSpawn = me->IsInCombat();
            if (!shouldSpawn)
            {
                std::list<Creature*> dragonkinsNearby;
                me->GetCreatureListInGrid(dragonkinsNearby, 10.0f);
                for (Creature const* dragonkin : dragonkinsNearby)
                {
                    switch (dragonkin->GetEntry())
                    {
                        case 26734:
                        case 26735:
                        case 26736:
                        case 26737:
                            shouldSpawn = true;
                            break;
                        default:
                            break;
                    }

                    if (shouldSpawn)
                        break;
                }
            }

            if (shouldSpawn)
                DoCastSelf(SPELL_RIFT_SUMMON_WRAITH);

            func.Repeat(11s);
        });
    }

    void JustSummoned(Creature* creature) override
    {
        summons.Summon(creature);
        if (IsMythic() && summons.GetEntryCount(NPC_CRAZED_MANA_WRAITH) >= 3)
            creature->DespawnOrUnsummon();
    }

    void SpellHit(Unit* caster, SpellInfo const* spellInfo) override
    {
        if (!spellInfo)
            return;

        if (spellInfo->Id == SPELL_RIFT_CHARGE_RIFT && !_wraithRateIncreased)
        {
            _wraithRateIncreased = true;
            me->RemoveAurasDueToSpell(SPELL_RIFT_CHARGE_RIFT);
            DoCastSelf(SPELL_RIFT_CHARGED_CHAOTIC_RIFT);
            task.CancelAll();
            task.Schedule(5s, [&](TaskContext func)
            {
                DoCastSelf(SPELL_RIFT_SUMMON_WRAITH);
                DoCastSelf(SPELL_RIFT_SUMMON_WRAITH);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

    void JustDied( Unit* killer ) override
    {
        NexusAI::JustDied( killer );

        if ( Creature* creature = instance->GetCreature( DATA_ANOMALUS_EVENT ) )
        {
            if ( creature && creature->AI() )
                creature->AI()->SetData( creature->GetEntry(), creature->GetEntry() );
        }
    }

private:
    bool _wraithRateIncreased;
    InstanceScript* instance;
    SummonList summons;
};

class spell_chaotic_rift_aura_nexus : public AuraScript
{
    PrepareAuraScript(spell_chaotic_rift_aura_nexus);

    void HandlePeriodic(AuraEffect const* eff)
    {
        if (!GetCaster())
            return;

        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (instance && instance->IsMythic())
        {
            PreventDefaultAction();
            CustomSpellValues val;
            uint32 damage = Mythic::CalculateMythicValue(3000, 0.04f);
            GetCaster()->CastCustomSpell(SPELL_RIFT_CHAOTIC_RIFT_AURA_TRIGGERED, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_chaotic_rift_aura_nexus::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

constexpr uint32 SPELL_CRAZED_LOOSE_MANA{ 29882 };
constexpr uint32 SPELL_CRAZED_MANA_BURN{ 48054 };
constexpr uint32 SPELL_CRAZED_MANA_BURN_HEROIC{ 57047 };
constexpr uint32 SPELL_CRAZED_ARCANE_NOVA{ 47696 };
constexpr uint32 SPELL_CRAZED_ARCANE_NOVA_HEROIC{ 57046 };

struct npc_crazed_mana_surge_nexus : public NexusAI
{
    npc_crazed_mana_surge_nexus(Creature* creature) : NexusAI(creature) { }

    void JustDied(Unit* killer) override
    {
        NexusAI::JustDied(killer);
        if (!IsMythic())
            DoCastAOE(SPELL_CRAZED_LOOSE_MANA, true);
        else
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_CRAZED_LOOSE_MANA, data));
            float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            me->CastCustomSpell(SPELL_CRAZED_LOOSE_MANA, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        }
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(urand(5000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(6000, 12000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    DoCast(target, IsHeroic() ? SPELL_CRAZED_MANA_BURN_HEROIC : SPELL_CRAZED_MANA_BURN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_CRAZED_MANA_BURN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    me->CastCustomSpell(SPELL_CRAZED_MANA_BURN, val, target);
                repeatTime = Milliseconds(urand(10000, 15500));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(6000, 12000));
            if (!IsMythic())
                DoCastAOE(IsHeroic() ? SPELL_CRAZED_ARCANE_NOVA_HEROIC : SPELL_CRAZED_ARCANE_NOVA);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_CRAZED_ARCANE_NOVA, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, 10);
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                me->CastCustomSpell(SPELL_CRAZED_ARCANE_NOVA, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(urand(6000, 17000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};


constexpr uint32 SPELL_CRYSTALLINE_CRYSTAL_STRIKE{ 33688 };
constexpr uint32 SPELL_CRYSTALLINE_CRYSTAL_STRIKE_HEROIC{ 57052 };
//! scaling
//! dmg base 3000
//! scaling 0.05
constexpr uint32 SPELL_CRYSTALLINE_CRYSTAL_BRAK{ 47699 };
struct npc_crystalline_keeper_nexus : public NexusAI
{
    npc_crystalline_keeper_nexus(Creature* creature) : NexusAI(creature) { }

    void Reset() override
    {
        NexusAI::Reset();
        task.Schedule(2s, [&](TaskContext func)
        {
            if (!me->HasAura(SPELL_CRYSTALLINE_CRYSTAL_BRAK))
                DoCastSelf(SPELL_CRYSTALLINE_CRYSTAL_BRAK);

            func.Repeat(10s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 4000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(6000, 8000));
            if (!IsMythic())
            {
                DoCastVictim(IsHeroic() ? SPELL_CRYSTALLINE_CRYSTAL_STRIKE_HEROIC : SPELL_CRYSTALLINE_CRYSTAL_STRIKE);
            }
            else
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_CRYSTALLINE_CRYSTAL_STRIKE);
                repeatTime = Milliseconds(4500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};


constexpr uint32 SPELL_CRYSTALLINE_TRANQUILITY{ 51972 };
constexpr uint32 SPELL_CRYSTALLINE_TRANQUILITY_HEROIC{ 57054 };
constexpr uint32 SPELL_CRYSTALLINE_TOUGHEN_SKIN{ 50994 };
struct npc_crystalline_tender_nexus : public NexusAI
{
    npc_crystalline_tender_nexus(Creature* creature) : NexusAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(7000, 12000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(15000);
            if (!IsMythic())
                DoCastAOE(IsHeroic() ? SPELL_CRYSTALLINE_TRANQUILITY_HEROIC : SPELL_CRYSTALLINE_TRANQUILITY);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_CRYSTALLINE_TRANQUILITY, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_CRYSTALLINE_TRANQUILITY, val, (Unit*)nullptr);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(2s, [&](TaskContext func)
        {
            DoCastSelf(SPELL_CRYSTALLINE_TOUGHEN_SKIN, true);
            func.Repeat(20s);
        });
    }
};


constexpr uint32 SPELL_PROTECTOR_SHARD_SPRAY{ 50302 };
constexpr uint32 SPELL_PROTECTOR_SHARD_SPRAY_HEROIC{ 57051 };
constexpr uint32 SPELL_PROTECTOR_CRYSTAL_CHAINS{ 47698 };
constexpr uint32 SPELL_PROTECTOR_CRYSTAL_CHAINS_HEROIC{ 57050 };
constexpr uint32 SPELL_PROTECTOR_THUNDERCLAP{ 30633 };

struct npc_crystalline_protector : public NexusAI
{
    npc_crystalline_protector(Creature* creature) : NexusAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(1000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(13000, 18000));
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_PROTECTOR_SHARD_SPRAY_HEROIC : SPELL_PROTECTOR_SHARD_SPRAY);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_PROTECTOR_SHARD_SPRAY, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_PROTECTOR_SHARD_SPRAY, val, target);
                repeatTime = Milliseconds(14500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(13000, 18000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, true))
                    DoCast(target, IsHeroic() ? SPELL_PROTECTOR_CRYSTAL_CHAINS_HEROIC : SPELL_PROTECTOR_CRYSTAL_CHAINS);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_PROTECTOR_CRYSTAL_CHAINS, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, true))
                    me->CastCustomSpell(SPELL_PROTECTOR_CRYSTAL_CHAINS, val, target);
                repeatTime = Milliseconds(13000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 15000));
            if (!IsMythic())
                DoCastAOE(SPELL_PROTECTOR_THUNDERCLAP);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_PROTECTOR_THUNDERCLAP, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_PROTECTOR_THUNDERCLAP, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(10000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_GAUNTLET_MYTHIC_BLIZZARD{ 41482 };
constexpr uint32 SPELL_GAUNTLET_MYTHIC_ICICLE{ 62234 };
//! Creature from hodirs encounter, it doesnt do anything besides casting two spells
//! so its safe to use
constexpr uint32 NPC_HODIR_ICICLE_NEXUS{ 33169 };
constexpr uint32 MYTHIC_BLIZZARD_PATH_SIZE{ 14 };
G3D::Vector3 MythicBlizzardPath[MYTHIC_BLIZZARD_PATH_SIZE] =
{
    { 369.960f, 150.318f, -34.6224f },
    { 383.436f, 153.753f, -35.0195f },
    { 400.855f, 175.064f, -35.0195f },
    { 415.085f, 183.896f, -35.0195f },
    { 434.299f, 183.065f, -35.0195f },
    { 454.754f, 167.981f, -35.0195f },
    { 472.675f, 152.310f, -34.6230f },

    // back
    { 472.675f, 152.310f, -34.6230f },
    { 454.754f, 167.981f, -35.0195f },
    { 434.299f, 183.065f, -35.0195f },
    { 415.085f, 183.896f, -35.0195f },
    { 400.855f, 175.064f, -35.0195f },
    { 383.436f, 153.753f, -35.0195f },
    { 369.960f, 150.318f, -34.6224f }
};

struct npc_blizzard_trigger_mythic : public ScriptedAI
{
    npc_blizzard_trigger_mythic(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->setFaction(16);
        task.CancelAll();
        me->SetSelectable(false);
        me->SetImmuneToNPC(true);
        me->SetImmuneToPC(false);
        task.Schedule(5s, [&](TaskContext func)
        {
            Movement::MoveSplineInit init(me);
            for (auto const& point : MythicBlizzardPath)
                init.Path().push_back({ point.x, point.y, point.z });
            init.SetSmooth();
            init.SetWalk(true);
            func.Repeat(Milliseconds(init.Launch()));
        });

        task.Schedule(2s, [&](TaskContext func)
        {
            for (uint32 i = 0; i < 5; ++i)
            {
                Position pos = Trinity::Containers::SelectRandomContainerElement(MythicGauntletIciclePosition);
                Creature* icicle = me->SummonCreature(91522, pos);
                if (icicle)
                {
                    me->CastSpell(icicle, SPELL_GAUNTLET_MYTHIC_ICICLE, true);
                    icicle->DespawnOrUnsummon(10s);
                }
            }
            func.Repeat(10s);
        });

        task.Schedule(2s, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 6500);
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 7500);
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 6000);
            me->CastCustomSpell(SPELL_GAUNTLET_MYTHIC_BLIZZARD, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            func.Repeat(2s);
        });
    }

    void AttackStart(Unit* /*who*/) override { }
    void EnterCombat(Unit* /*who*/) override { }
    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

private:
    bool _gauntletTriggered;
    TaskScheduler task;
    Movement::PointsArray path;
};

enum AllianceBerserker
{
    SPELL_ALLIANCE_BERSERK_LUST = 21049,
    SPELL_ALLIANCE_BERSERK_FRENZY = 47774,
    SPELL_ALLIANCE_BERSERK_WARSTOMP = 38682
};

struct npc_alliance_berserker : public NexusAI
{
    npc_alliance_berserker(Creature* creature) : NexusAI(creature) { }

    void Reset() override
    {
        DoCastSelf(SPELL_RANGER_NEXUS_FROZEN, true);
        NexusAI::Reset();
    }

    void EnterCombat(Unit* who) override
    {
        NexusAI::EnterCombat(who);
        me->RemoveAurasDueToSpell(SPELL_RANGER_NEXUS_FROZEN);

        task.Schedule(randseconds(5, 30), [&](TaskContext func)
        {
            std::list<Creature*> _missing = DoFindFriendlyMissingBuff(30.f, SPELL_ALLIANCE_BERSERK_LUST);
            if (!_missing.empty())
            {
                Unit* target = Trinity::Containers::SelectRandomContainerElement(_missing);
                if (target)
                    DoCast(target, SPELL_ALLIANCE_BERSERK_LUST);
            }
            func.Repeat(20s);
        });

        task.Schedule(10s, [&](TaskContext func)
        {
            DoCastSelf(SPELL_ALLIANCE_BERSERK_FRENZY);
            func.Repeat(randseconds(10, 12));
        });

        task.Schedule(randseconds(5, 12), [&](TaskContext func)
        {
            if (IsMythic())
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ALLIANCE_BERSERK_WARSTOMP, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                me->CastCustomSpell(SPELL_ALLIANCE_BERSERK_WARSTOMP, val, (Unit*)nullptr);
            }
            else
                DoCastAOE(SPELL_ALLIANCE_BERSERK_WARSTOMP);

            func.Repeat(randseconds(15, 20));
        });
    }
};

class spell_nexus_immolation_trap : public AuraScript
{
    PrepareAuraScript(spell_nexus_immolation_trap);

    void CalculateStatAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* owner = GetUnitOwner())
        {
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->IsMythic())
            {
                auto keyLevel = owner->GetInstanceScript()->GetMythicKeyLevel();
                amount = Mythic::CalculateMythicValue(3000, 0.04f);
            }
        }
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        GetEffect(aurEff->GetEffIndex())->RecalculateAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_nexus_immolation_trap::CalculateStatAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_nexus_immolation_trap::HandlePeriodic, EFFECT_0, SPELL_AURA_ANY);
    }
};

void AddSC_nexus()
{
    RegisterCreatureAI(npc_crystalline_frayer);
    RegisterCreatureAI(npc_azure_warder_nexus);
    RegisterCreatureAI(npc_mage_hunter_ascendant_nexus);
    RegisterCreatureAI(npc_mage_slayer_nexus);
    RegisterCreatureAI(npc_alliance_ranger_nexus);
    RegisterCreatureAI(npc_alliance_cleric_nexus);
    RegisterCreatureAI(npc_steward_nexus);
    RegisterCreatureAI(npc_mage_hunter_initiate_nexus);
    RegisterCreatureAI(npc_grand_magus_telestra_fire);
    RegisterCreatureAI(npc_grand_magus_telestra_time);
    RegisterCreatureAI(npc_grand_telestra_frost);
    RegisterCreatureAI(npc_azure_enforcer_nexus);
    RegisterCreatureAI(npc_azure_scale_binder);
    RegisterCreatureAI(npc_crazed_mana_wraith_nexus);
    RegisterCreatureAI(npc_chaotic_rift_nexus);
    RegisterCreatureAI(npc_crazed_mana_surge_nexus);
    RegisterCreatureAI(npc_crystalline_keeper_nexus);
    RegisterCreatureAI(npc_crystalline_tender_nexus);
    RegisterCreatureAI(npc_crystalline_protector);
    RegisterCreatureAI(npc_blizzard_trigger_mythic);
    RegisterCreatureAI(npc_alliance_berserker);
    RegisterCreatureAI(npc_azure_magus_nexus);

    RegisterAuraScript(spell_arcane_explosion_nexus);
    RegisterAuraScript(spell_nexus_immolation_trap);
    RegisterAuraScript(spell_nexus_rapid_shot_periodic);
    RegisterAuraScript(spell_chaotic_rift_aura_nexus);
}