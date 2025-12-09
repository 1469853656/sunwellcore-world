#include "blood_furnace.h"

#include "ScriptedCreature.h"
#include "MythicAI.h"

struct BloodFuranceAI : public MythicAI
{
    BloodFuranceAI(Creature* creature) : MythicAI(creature) { }

    //! override a function if needed, mythicAI derives from scriptedAI
    //! check MythicAI class to see which functions are overriden
};

constexpr uint32 SPELL_NAME_ENFORCER_STRIKE{ 14516 };
constexpr uint32 SPELL_NAME_ENFORCER_SHIELD_SLAM{ 15655 };
constexpr uint32 BF_ENFORCER_AGGRO_TEXT{ 0 };
struct npc_laughing_skull_enforcer : public BloodFuranceAI
{
    npc_laughing_skull_enforcer(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        Talk(BF_ENFORCER_AGGRO_TEXT);
        BloodFuranceAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(1900, 8800)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(9300, 14700));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_NAME_ENFORCER_STRIKE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_NAME_ENFORCER_STRIKE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_NAME_ENFORCER_STRIKE, val, target);
                repeatTime = Milliseconds(7000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(4200, 9500)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(9500, 15300));
            if (!IsMythic())
                DoCastVictim(SPELL_NAME_ENFORCER_SHIELD_SLAM);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_NAME_ENFORCER_SHIELD_SLAM, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 0);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_NAME_ENFORCER_SHIELD_SLAM, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(9500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};


constexpr uint32 SPELL_SHADOWMOON_ADEPT_KICK{ 11978 };
constexpr uint32 SPELL_SHADOWMOON_ADEPT_THRASH{ 8876 };
constexpr uint32 SPELL_SHADOWMOON_ADEPT_HELLFIRE_CHANNELING{ 31059 };
constexpr uint32 BF_SHADOWMOON_ADEPT_AGGRO_TEXT{ 0 };

struct npc_shadowmoon_adept : public BloodFuranceAI
{
    npc_shadowmoon_adept(Creature* creature) : BloodFuranceAI(creature) { }

    void Reset() override
    {
        BloodFuranceAI::Reset();
        DoCastSelf(SPELL_SHADOWMOON_ADEPT_THRASH, true);
        task.Schedule(3s, [&](TaskContext func)
        {
            if (!me->IsCasting())
                DoCastSelf(SPELL_SHADOWMOON_ADEPT_HELLFIRE_CHANNELING);

            func.Repeat(5s);
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
        task.CancelAll();
        Talk(BF_SHADOWMOON_ADEPT_AGGRO_TEXT);
        BloodFuranceAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(5600, 9600)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(9600, 11400));
            if (!IsMythic())
                DoCastVictim(SPELL_SHADOWMOON_ADEPT_KICK, true);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOWMOON_ADEPT_KICK, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
                {
                    return obj->IsCasting() && obj->IsWithinDist(me, 5.f);
                });

                if (!target)
                    target = me->GetVictim();

                if (target)
                    me->CastCustomSpell(SPELL_SHADOWMOON_ADEPT_KICK, val, target);
                repeatTime = Milliseconds(8000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_HELLFIRE_IMP_FIREBALL_NORMAL{ 15242 };
constexpr uint32 SPELL_HELLFIRE_IMP_FIREBALL_HEROIC{ 17290 };
constexpr uint32 SPELL_HELLFIRE_IMP_FIREBLAST{ 16144 };
constexpr uint32 SPELL_HELLFIRE_IMP_SUMMON_VISUAL{ 30540 };

struct npc_hellfire_imp_blood_furnace : public BloodFuranceAI
{
    npc_hellfire_imp_blood_furnace(Creature* creature) : BloodFuranceAI(creature) { }

    void Reset() override
    {
        BloodFuranceAI::Reset();
        task.Schedule(3s, [&](TaskContext func)
        {
            if (!me->IsInCombat() && !me->IsCasting())
                DoCastSelf(SPELL_HELLFIRE_IMP_SUMMON_VISUAL, true);
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
        std::list<Creature*> _nearbyImps;
        me->GetCreatureListWithEntryInGrid(_nearbyImps, me->GetEntry(), 15.f);
        for (Creature* creature : _nearbyImps)
        {
            if (creature->IsAIEnabled && who)
                creature->AI()->AttackStart(who);
        }

        task.CancelAll();
        me->RemoveAurasDueToSpell(SPELL_HELLFIRE_IMP_SUMMON_VISUAL);
        BloodFuranceAI::EnterCombat(who);

        task.Schedule(Milliseconds(urand(1900, 5800)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(5300, 7700));
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_HELLFIRE_IMP_FIREBALL_HEROIC : SPELL_HELLFIRE_IMP_FIREBALL_NORMAL, true);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_HELLFIRE_IMP_FIREBALL_NORMAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_HELLFIRE_IMP_FIREBALL_NORMAL, val, target);
                repeatTime = Milliseconds(3500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(4200, 9500)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(9500, 12400));
            if (!IsMythic())
                DoCastVictim(SPELL_HELLFIRE_IMP_FIREBLAST);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_HELLFIRE_IMP_FIREBLAST, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, [](Unit* obj) { return IS_PLAYER_GUID(obj->GetGUID()); }))
                    me->CastCustomSpell(SPELL_HELLFIRE_IMP_FIREBLAST, val, target);
                repeatTime = Milliseconds(9500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_GANARG_TINKERER_POWER_BURN{ 32003 };
struct npc_ganarg_tinkerer : public BloodFuranceAI
{
    npc_ganarg_tinkerer(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 4500)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(4500, 12000));
            if (!IsMythic())
                DoCastAOE(SPELL_GANARG_TINKERER_POWER_BURN, true);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_GANARG_TINKERER_POWER_BURN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT2, Mythic::CalculateMythicValue(data.SpellEffectValue2, variance));
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                me->CastCustomSpell(SPELL_GANARG_TINKERER_POWER_BURN, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(6000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_BLOOD_FURNACE_SHADOWMOON_FEL_POWER{ 33111 };
constexpr uint32 SPELL_BLOOD_FURNACE_SHADOWMOON_COT{ 13338 };
constexpr uint32 SPELL_BLOOD_FURNACE_SHADOWMOON_SHADOWBOLT{ 12739 };
constexpr uint32 SPELL_BLOOD_FURNACE_SHADOWMOON_SHADOWBOLT_HEROIC{ 15472 };
constexpr uint32 SPELL_BLOOD_FURNACE_SHADOWMOON_CORRUPTION{ 32197 };
constexpr uint32 SPELL_BLOOD_FURNACE_SHADOWMOON_CORRUPTION_HEROIC{ 37113 };
struct npc_shadowmoon_warlock_blood_furnace : public BloodFuranceAI
{
    npc_shadowmoon_warlock_blood_furnace(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        task.Schedule(Milliseconds(12000), [&](TaskContext func)
        {
            auto repeatTime = IsMythic() ? Milliseconds(15000) : Milliseconds(urand(21900, 25000));

            DoCastAOE(SPELL_BLOOD_FURNACE_SHADOWMOON_FEL_POWER);
            func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(6600, 10700)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(15000);
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                    DoCast(target, SPELL_BLOOD_FURNACE_SHADOWMOON_COT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BLOOD_FURNACE_SHADOWMOON_COT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return !obj->HasAura(SPELL_BLOOD_FURNACE_SHADOWMOON_COT);
                }))
                    me->CastCustomSpell(SPELL_BLOOD_FURNACE_SHADOWMOON_COT, val, target);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(3000, 4000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3000, 4000));
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_BLOOD_FURNACE_SHADOWMOON_SHADOWBOLT_HEROIC : SPELL_BLOOD_FURNACE_SHADOWMOON_SHADOWBOLT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BLOOD_FURNACE_SHADOWMOON_SHADOWBOLT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_BLOOD_FURNACE_SHADOWMOON_SHADOWBOLT, val, target);
                repeatTime = Milliseconds(urand(3000, 4000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1100, 7800)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(14800, 30100));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                    DoCast(target, IsHeroic() ? SPELL_BLOOD_FURNACE_SHADOWMOON_CORRUPTION_HEROIC : SPELL_BLOOD_FURNACE_SHADOWMOON_CORRUPTION);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_BLOOD_FURNACE_SHADOWMOON_CORRUPTION, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, true))
                    me->CastCustomSpell(SPELL_BLOOD_FURNACE_SHADOWMOON_CORRUPTION, val, target);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_LEGIONNAIRE_UPPERCUT{ 10966 };
constexpr uint32 SPELL_LEGIONAAIRE_ENRAGE{ 8599 };
constexpr uint32 TALK_LEGIONNAIRE_AGGRO{ 0 };
struct npc_laughing_skull_legionnaire : public BloodFuranceAI
{
    npc_laughing_skull_legionnaire(Creature* creature) : BloodFuranceAI(creature) { }

    void Reset() override
    {
        _frenzied = false;
        BloodFuranceAI::Reset();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_frenzied && me->HealthBelowPctDamaged(30, damage))
        {
            _frenzied = true;
            DoCastSelf(SPELL_LEGIONAAIRE_ENRAGE);
        }
    }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        Talk(TALK_LEGIONNAIRE_AGGRO);

        task.Schedule(Milliseconds(urand(2600, 7800)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(7200, 13300));
            if (!IsMythic())
                DoCastVictim(SPELL_LEGIONNAIRE_UPPERCUT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_LEGIONNAIRE_UPPERCUT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_LEGIONNAIRE_UPPERCUT, val, target);
                repeatTime = Milliseconds(urand(7500, 9000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
private:
    bool _frenzied;
};

constexpr uint32 SPELL_SHADOWMOON_SUMMONER_SEDUCTRESS{ 30853 };
constexpr uint32 SPELL_SHADOWMOON_SUMMONER_FEL_MANASTALKER{ 30851 };
constexpr uint32 SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE{ 18399 };
constexpr uint32 SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE_HEROIC{ 16102 };
constexpr uint32 SPELL_SHADOWMOON_SUMMONER_FIREBALL{ 15242 };
constexpr uint32 SPELL_SHADOWMOON_SUMMONER_FIREBALL_HEROIC{ 17290 };
struct npc_shadowmoon_summoner_blood_furnace : public BloodFuranceAI
{
    npc_shadowmoon_summoner_blood_furnace(Creature* creature) : BloodFuranceAI(creature), summons(me) { }

    void Reset() override
    {
        summons.DespawnAll();
        BloodFuranceAI::Reset();
    }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        if (IsMythic())
        {
            task.Schedule(10s, [&](TaskContext func)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 2000 - (100 * GetMythicKeyLevel()));
                if (func.GetRepeatCounter())
                {
                    if (func.GetRepeatCounter() % 2 == 0)
                        me->CastCustomSpell(SPELL_SHADOWMOON_SUMMONER_FEL_MANASTALKER, val, (Unit*)nullptr);
                    else
                        me->CastCustomSpell(SPELL_SHADOWMOON_SUMMONER_SEDUCTRESS, val, (Unit*)nullptr);
                }
                else
                {
                    uint32 spellId = Trinity::Containers::SelectRandomContainerElement(
                        std::vector<uint32>{ SPELL_SHADOWMOON_SUMMONER_FEL_MANASTALKER, SPELL_SHADOWMOON_SUMMONER_SEDUCTRESS });
                    me->CastCustomSpell(spellId, val, (Unit*)nullptr);
                }

                func.Repeat(30s);
            });
        }
        else
        {
            task.Schedule(Milliseconds(urand(1000, 5000)), [&](TaskContext func)
            {
                if (!func.GetRepeatCounter())
                {
                    DoCastAOE(SPELL_SHADOWMOON_SUMMONER_SEDUCTRESS);
                    func.Repeat(Milliseconds(urand(15000, 28000)));
                }
                else
                    DoCastAOE(SPELL_SHADOWMOON_SUMMONER_FEL_MANASTALKER);
            });
        }

        task.Schedule(Milliseconds(urand(12000, 14000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(14000);
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                    DoCast(target, IsHeroic() ? SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE_HEROIC : SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return obj->HasAura(SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE);
                }))
                    me->CastCustomSpell(SPELL_SHADOWMOON_SUMMONER_FLAMESTRIKE, val, target);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2400, 3800)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(3000);
            if (!IsMythic())
                DoCastVictim(IsHeroic() ? SPELL_SHADOWMOON_SUMMONER_FIREBALL_HEROIC : SPELL_SHADOWMOON_SUMMONER_FIREBALL);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOWMOON_SUMMONER_FIREBALL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SHADOWMOON_SUMMONER_FIREBALL, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        if (me->IsInCombat() && summon->AI())
            summon->AI()->DoZoneInCombat();
    }
private:
    SummonList summons;
};

enum FelhunterBloodFurance
{
    SPELL_FELHUNTER_MANA_BURN_BF = 13321,
    SPELL_FELHUNTER_SPELL_LOCK_BF = 30849
};

struct npc_felhunter_blood_furnace_summoned : public BloodFuranceAI
{
    npc_felhunter_blood_furnace_summoned(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        task.Schedule(randseconds(1, 3), [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
            {
                return obj->IsPlayer() && obj->getPowerType() == POWER_MANA;
            });

            Seconds repeat = 2s;
            if (target)
            {
                if (IsMythic())
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FELHUNTER_MANA_BURN_BF, data));
                    float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    me->CastCustomSpell(SPELL_FELHUNTER_MANA_BURN_BF, val, target);
                }
                else
                    DoCast(target, SPELL_FELHUNTER_MANA_BURN_BF);

                repeat = randseconds(12, 14);
            }

            func.Repeat(repeat);
        });

        task.Schedule(randseconds(2, 6), [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [](Unit* obj)
            {
                return obj->IsPlayer() && obj->IsCasting();
            });

            if (target)
            {
                DoCast(target, SPELL_FELHUNTER_SPELL_LOCK_BF);
            }

            func.Repeat(randseconds(13, 15));
        });
    }
};

enum SeductressBF
{
    SPELL_SEDUCTRESS_LASH_OF_PAIN = 32202,
    SPELL_SEDUCTRESS_SEDUCE = 31865
};

struct npc_seductress_blood_furnace : public BloodFuranceAI
{
    npc_seductress_blood_furnace(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);

        task.Schedule(randseconds(1, 3), [&](TaskContext func)
        {
            if (IsMythic())
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SEDUCTRESS_LASH_OF_PAIN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SEDUCTRESS_LASH_OF_PAIN, val, target);
            }
            else
                DoCastVictim(SPELL_SEDUCTRESS_LASH_OF_PAIN);

            func.Repeat(IsMythic() ? randseconds(4, 6) : 13s);
        });

        task.Schedule(randseconds(3, 8), [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, [](Unit* obj)
            {
                return obj->IsPlayer();
            });

            if (target)
                DoCast(target, SPELL_SEDUCTRESS_SEDUCE);

            func.Repeat(IsMythic() ? 11s : 14s);
        });
    }
};

constexpr uint32 TALK_SHADOWMOON_TECHNICAN_AGGRO{ 0 };
constexpr uint32 SPELL_SHADOWMOON_TECHNICAN_SILENCE{ 6726 };
constexpr uint32 SPELL_SHADOWMOON_TECHNICAN_THROW_PROXIMITY_BOMB{ 30846 };
constexpr uint32 SPELL_SHADOWMOON_TECHNICAN_THROW_PROXIMITY_BOMB_HEROIC{ 32784 };
constexpr uint32 SPELL_SHADOWMOON_TECHNICAN_THROW_DYNAMITE{ 40062 };
constexpr uint32 SPELL_SHADOWMOON_TECHNICAN_THROW_DYNAMITE_HEROIC{ 40064 };
struct npc_shadowmoon_technican_blood_furnace : public BloodFuranceAI
{
    npc_shadowmoon_technican_blood_furnace(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        Talk(TALK_SHADOWMOON_TECHNICAN_AGGRO);

        task.Schedule(Milliseconds(urand(5400, 11600)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(23000);
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, [](Unit* obj)
                {
                    return obj->IsPlayer();
                }))
                    DoCast(target, SPELL_SHADOWMOON_TECHNICAN_SILENCE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOWMOON_TECHNICAN_SILENCE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, [](Unit* obj)
                {
                    return obj->IsPlayer();
                }))
                    me->CastCustomSpell(SPELL_SHADOWMOON_TECHNICAN_SILENCE, val, target);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2400, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(17200, 23000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, true))
                    DoCast(target, IsHeroic () ? SPELL_SHADOWMOON_TECHNICAN_THROW_PROXIMITY_BOMB_HEROIC :
                        SPELL_SHADOWMOON_TECHNICAN_THROW_PROXIMITY_BOMB);
            }
            else
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, true))
                    DoCast(target, SPELL_SHADOWMOON_TECHNICAN_THROW_PROXIMITY_BOMB, true);

                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(4400, 10600)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(7200, 11300));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                    DoCast(target, IsHeroic() ? SPELL_SHADOWMOON_TECHNICAN_THROW_DYNAMITE_HEROIC :
                        SPELL_SHADOWMOON_TECHNICAN_THROW_DYNAMITE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOWMOON_TECHNICAN_THROW_DYNAMITE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 30.f, true))
                    me->CastCustomSpell(SPELL_SHADOWMOON_TECHNICAN_THROW_DYNAMITE, val, target, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(10000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_PROXIMITY_BOMB_EXPLOSION_DMG{ 30844 };
class spell_proximity_bomb_explosion : public SpellScript
{
    PrepareSpellScript(spell_proximity_bomb_explosion);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* hit = GetHitUnit();
        if (!hit)
            return;

        InstanceScript* instance = hit->GetInstanceScript();
        if (!instance || !instance->IsMythic())
            return;

        Mythic::MythicSpells data;
        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_PROXIMITY_BOMB_EXPLOSION_DMG, data));
        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;
        SetEffectValue(Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_proximity_bomb_explosion::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

constexpr uint32 SPELL_LAUGHING_SKULL_STEALTH_DETECTION{ 38551 };
constexpr uint32 SPELL_LAUGHING_SKULL_BATTLE_SHOUT{ 9128 };
constexpr uint32 TALK_LAUGHING_SKULL_AGGRO{ 0 };
struct npc_laughing_skull_warden : public BloodFuranceAI
{
    npc_laughing_skull_warden(Creature* creature) : BloodFuranceAI(creature) { }

    void Reset() override
    {
        BloodFuranceAI::Reset();
        //! this shit break visibility around the NPC, why? Oo
        //DoCastSelf(SPELL_LAUGHING_SKULL_STEALTH_DETECTION);
    }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        Talk(TALK_LAUGHING_SKULL_AGGRO);

        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(0);
            if (!IsMythic())
                DoCastAOE(SPELL_LAUGHING_SKULL_BATTLE_SHOUT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_LAUGHING_SKULL_BATTLE_SHOUT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_LAUGHING_SKULL_BATTLE_SHOUT, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            }
        });
    }
};


constexpr uint32 SPELL_FELGUARD_BRUTE_PUMMEL{ 15615 };
constexpr uint32 SPELL_FELGUARD_BRUTE_UPPERCUT{ 18072 };

struct npc_felguard_brute_blood_furnace : public BloodFuranceAI
{
    npc_felguard_brute_blood_furnace(Creature* creature) : BloodFuranceAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 15000));
            if (!IsMythic())
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
                {
                    return obj && obj->IsCasting() && obj->IsWithinDist(me, 5.f);
                });

                if (!target)
                    target = me->GetVictim();

                DoCast(target, SPELL_FELGUARD_BRUTE_PUMMEL);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FELGUARD_BRUTE_PUMMEL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));

                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
                {
                    return obj && obj->IsCasting() && obj->IsWithinDist(me, 5.f);
                });

                if (!target)
                    target = me->GetVictim();

                if (target)
                    me->CastCustomSpell(SPELL_FELGUARD_BRUTE_PUMMEL, val, target);
                repeatTime = Milliseconds(9000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 5000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(13700, 15700));
            if (!IsMythic())
                DoCastVictim(SPELL_FELGUARD_BRUTE_UPPERCUT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FELGUARD_BRUTE_UPPERCUT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_FELGUARD_BRUTE_UPPERCUT, val, target);
                repeatTime = Milliseconds(urand(8500, 12500));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

enum LaughingSkull
{
    SPELL_LAUGHING_SKULL_STEALTH = 30991,
    SPELL_LAUGHING_SKULL_POISON = 34969,
    SPELL_LAUGHING_SKULL_SND = 6434, // slice and dice
    SPELL_LAUGHING_SKULL_KIDNEY_SHOT = 30832,

    TALK_LAUGHING_SKULL_ROGUE_AGGRO = 0
};

struct npc_laughing_skull_rogue : public BloodFuranceAI
{
    npc_laughing_skull_rogue(Creature* creature) : BloodFuranceAI(creature) { }

    void Reset() override
    {
        BloodFuranceAI::Reset();
        DoCastSelf(SPELL_LAUGHING_SKULL_STEALTH, true);
    }

    void EnterCombat(Unit* who) override
    {
        BloodFuranceAI::EnterCombat(who);
        me->RemoveAurasDueToSpell(SPELL_LAUGHING_SKULL_STEALTH);
        Talk(TALK_LAUGHING_SKULL_ROGUE_AGGRO);

        ScheduleAbilities();
    }

    void ScheduleAbilities()
    {
        task.Schedule(2s, [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            if (me->IsWithinMeleeRange(target))
            {
                if (IsMythic())
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_LAUGHING_SKULL_POISON, data));
                    float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    if (Unit* target = me->GetVictim())
                        me->CastCustomSpell(SPELL_LAUGHING_SKULL_POISON, val, target);
                }
                else
                    DoCastVictim(SPELL_LAUGHING_SKULL_POISON);

                func.Repeat(randseconds(2, 4));
            }
            else
                func.Repeat(2s);
        });

        task.Schedule(8s, [&](TaskContext func)
        {
            DoCastSelf(SPELL_LAUGHING_SKULL_SND);
            func.Repeat(randseconds(15, 20));
        });

        task.Schedule(randseconds(5, 10), [&](TaskContext func)
        {
            DoCastVictim(SPELL_LAUGHING_SKULL_KIDNEY_SHOT);
            func.Repeat(randseconds(12, 15));
        });
    }
};

void AddSC_blood_furnace()
{
    //! Trash
    RegisterCreatureAI(npc_laughing_skull_enforcer);
    RegisterCreatureAI(npc_shadowmoon_adept);
    RegisterCreatureAI(npc_hellfire_imp_blood_furnace);
    RegisterCreatureAI(npc_ganarg_tinkerer);
    RegisterCreatureAI(npc_shadowmoon_warlock_blood_furnace);
    RegisterCreatureAI(npc_laughing_skull_legionnaire);
    RegisterCreatureAI(npc_shadowmoon_summoner_blood_furnace);
    RegisterCreatureAI(npc_shadowmoon_technican_blood_furnace);
    RegisterCreatureAI(npc_laughing_skull_warden);
    RegisterCreatureAI(npc_felguard_brute_blood_furnace);
    RegisterCreatureAI(npc_laughing_skull_rogue);
    RegisterCreatureAI(npc_felhunter_blood_furnace_summoned);
    RegisterCreatureAI(npc_seductress_blood_furnace);

    RegisterSpellScript(spell_proximity_bomb_explosion);
}

