
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "nexus.h"
#include "Player.h"
#include "SpellInfo.h"
#include "World.h"

#include <numeric>

enum AnomalusSpells
{
    SPELL_SPARK                          = 47751,
    SPELL_RIFT_SHIELD                    = 47748,
    SPELL_CHARGE_RIFTS                   = 47747,
    SPELL_CREATE_RIFT                    = 47743,
    SPELL_ARCANE_ATTRACTION              = 57063,
    SPELL_CLOSE_RIFTS                    = 47745
};

enum AnomalusYells
{
    SAY_AGGRO                            = 0,
    SAY_DEATH,
    SAY_RIFT,
    EMOTE_RIFT,
    EMOTE_SHIELD
};

enum Events
{
    EVENT_ANOMALUS_SPARK                = 1,
    EVENT_ANOMALUS_HEALTH,
    EVENT_ANOMALUS_ARCANE_ATTRACTION
};

enum AnomalusMythic
{
    NPC_ANOMALUS_AMZ_TRIGGER = 91523,
    NPC_ANOMALUS_CHAOTIC_RIFT_MYTHIC = 91524,
    SPELL_ANOMALUS_AMZ = 52894,
    SPELL_ANOMALUS_LIGHTNING_BLAST = 63491,
    SPELL_ANOMALUS_THUNDERSHOCK = 60029,
    SPELL_ANOMALUS_SIMPLE_TELEPORT = 21649
};

class ChargeRifts : public BasicEvent
{
    public:
        ChargeRifts(Creature* caster) : _caster(caster) { }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
        {
            _caster->AI()->Talk(EMOTE_SHIELD);
            _caster->CastSpell(_caster, SPELL_CHARGE_RIFTS, true);
            return true;
        }

    private:
        Creature* _caster;
};

std::vector<Position> _mythicZonePositions =
{
    { 634.347f, -266.701f, -8.46590f, 4.89887f },
    { 621.519f, -281.773f, -9.03194f, 4.95228f },
    { 652.687f, -273.458f, -8.74342f, 4.79520f },
    { 652.419f, -296.278f, -9.35363f, 4.74572f },
    { 626.893f, -304.269f, -9.43212f, 5.21696f },
    { 639.887f, -312.359f, -9.47181f, 5.21696f }
};

std::vector<Position> _mythicPortalSpawnPositions =
{
    { 615.671f, -264.221f, -8.45662f, 5.75496f },
    { 611.951f, -292.221f, -9.27981f, 0.19983f },
    { 623.055f, -316.542f, -9.57234f, 1.06456f },
    { 638.797f, -327.151f, -9.59207f, 1.63240f },
    { 653.403f, -315.815f, -9.58213f, 2.43979f },
    { 665.106f, -287.416f, -9.17872f, 3.04141f },
    { 658.864f, -262.047f, -8.36969f, 3.99881f },
    { 636.167f, -252.765f, -7.95413f, 4.73394f }
};

//cool spell, lightning charge 50703
//63491
struct boss_anomalus : public BossAI
{
    boss_anomalus(Creature* creature) : BossAI(creature, DATA_ANOMALUS_EVENT)
    {
        //preNerf = sWorld->PatchNotes(PATCH_MIN, PATCH_332);
        preNerf = false;
        _defaultCombatReach = me->GetFloatValue(UNIT_FIELD_COMBATREACH);
    }

    void Reset() override
    {
        if (instance && instance->IsMythic())
        {
            float const _newReach = _defaultCombatReach * 1.15f;
            if (me->GetFloatValue(UNIT_FIELD_COMBATREACH) < _newReach)
                me->SetFloatValue(UNIT_FIELD_COMBATREACH, _newReach);

            me->SetCanMissSpells(false);
        }

        BossAI::Reset();
        uninterruptibleScheduler.CancelAll();
        achievement = true;
        riftCount = 0;
        magicZoneReversing = false;
        magicZoneIdx = 0;
        me->CastSpell(me, SPELL_CLOSE_RIFTS, true);
    }

    uint32 GetData(uint32 data) const override
    {
        if (data == me->GetEntry())
            return achievement;
        return 0;
    }

    void SetData(uint32 type, uint32 /*valule*/) override
    {
        if ( instance->IsMythic() )
            return;

        if ( preNerf )
            events.ScheduleEvent( EVENT_ANOMALUS_HEALTH, 1000 );

        if ( type == me->GetEntry() )
        {
            me->RemoveAura( SPELL_RIFT_SHIELD );
            me->InterruptNonMeleeSpells( false );
            achievement = false;
        }
    }

    void EnterCombat(Unit* who) override
    {
        Talk( SAY_AGGRO );
        BossAI::EnterCombat(who);

        if (!instance->IsMythic())
        {
            events.SetTimer(45000);
            events.ScheduleEvent(EVENT_ANOMALUS_SPARK, 5000);
            events.ScheduleEvent(EVENT_ANOMALUS_HEALTH, 1000);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_ANOMALUS_ARCANE_ATTRACTION, 8000);
        }
        else
        {
            ScheduleMythic();
            events.ScheduleEvent(EVENT_ANOMALUS_ARCANE_ATTRACTION, 8000);
        }
    }

    void SpawnMagicZone()
    {
        uninterruptibleScheduler.Schedule(1ms, [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 50.f, true);
            if (!target)
            {
                EnterEvadeMode();
                return;
            }

            Creature* amz = me->SummonCreature(NPC_ANOMALUS_AMZ_TRIGGER, _mythicZonePositions[magicZoneIdx]);
            if (!amz)
                return;

            amz->SetRegeneratingHealth(false);
            amz->setFaction(target->getFaction());
            amz->SetImmuneToAll(true);
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 11000);
            amz->CastCustomSpell(SPELL_ANOMALUS_AMZ, val, amz, TRIGGERED_FULL_MASK);
            amz->SetMaxHealth(7);
            amz->SetFullHealth();

            int32 newIdx = magicZoneIdx;
            if (!magicZoneReversing)
                newIdx = std::min(5U, magicZoneIdx + (roll_chance_i(50) ? 1U : 2U));
            else
                newIdx = std::max(0, static_cast<int32>(magicZoneIdx - (roll_chance_i(50) ? 1 : 2)));

            if (newIdx >= 5 && !magicZoneReversing)
                magicZoneReversing = true;
            else if (!newIdx && magicZoneReversing)
                magicZoneReversing = false;

            magicZoneIdx = newIdx;

            func.Repeat(30s);
        });
    }

    void ScheduleMythic()
    {
        SpawnMagicZone();
        uninterruptibleScheduler.Schedule(5s, [&](TaskContext func)
        {
            CustomSpellValues val;
            float variance = instance->GetMythicKeyLevel() * 0.01f;
            val.AddSpellMod(SPELLVALUE_TARGET_PLAYERS_ONLY, 1);
            val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(22000, variance));
            me->CastCustomSpell(SPELL_ANOMALUS_LIGHTNING_BLAST, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);

            func.Repeat(5s);
        });

        scheduler.Schedule(4s, [&](TaskContext func)
        {
            for (uint32 i = 0; i < 2; ++i)
            {
                uint32 const idx = urand(0U, _mythicPortalSpawnPositions.size() - 1U);
                Position const pos = _mythicPortalSpawnPositions[idx];
                Creature* rift = me->SummonCreature(NPC_ANOMALUS_CHAOTIC_RIFT_MYTHIC, pos);
                if (rift)
                {
                    rift->SetPassive();
                    if (rift->IsAIEnabled)
                        rift->AI()->SetData(idx, idx);
                }
            }

            ScheduleSingleMythicRift();
        });
    }

    void ScheduleSingleMythicRift()
    {
        scheduler.Schedule(70s, [&](TaskContext func)
        {
            uint32 const idx = urand(0U, _mythicPortalSpawnPositions.size() - 1U);
            Position const pos = _mythicPortalSpawnPositions[idx];
            Creature* rift = me->SummonCreature(NPC_ANOMALUS_CHAOTIC_RIFT_MYTHIC, pos);
            if (rift)
            {
                rift->SetPassive();
                if (rift->IsAIEnabled)
                    rift->AI()->SetData(idx, idx);
            }

            func.Repeat();
        });
    }

    void JustSummoned(Creature* summon) override
    {
        //! do not call base class, it will call dozoneincombat, we dont want that
        summons.Summon(summon);
    }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_DEATH);
        BossAI::JustDied(killer);
        me->CastSpell(me, SPELL_CLOSE_RIFTS, true);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 637.773987f, -289.766937f, -9.226827f, 1.614400f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        uninterruptibleScheduler.Update(diff);
        scheduler.Update(diff);
        events.Update(diff);
        if (me->IsCasting())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ANOMALUS_HEALTH:
                {
                    bool healthCheck = false;

                    if (preNerf)
                    {
                        healthCheck = (me->HealthBelowPct(75) && riftCount == 0) ||
                            (me->HealthBelowPct(50) && riftCount == 1) ||
                            (me->HealthBelowPct(25) && riftCount == 2);
                    }
                    else
                    {
                        healthCheck = (me->HealthBelowPct(50) && riftCount == 0);
                    }

                    if (healthCheck)
                    {
                        riftCount++;
                        Talk(SAY_RIFT);
                        Talk(EMOTE_RIFT);

                        me->CastSpell(me, SPELL_CREATE_RIFT, false);
                        me->CastSpell(me, SPELL_RIFT_SHIELD, true);
                        me->m_Events.AddEvent(new ChargeRifts(me), me->m_Events.CalculateTime(1000));
                        events.DelayEvents(46s);
                        break;
                    }
                    events.ScheduleEvent(EVENT_ANOMALUS_HEALTH, 1s);
                }
                break;
                case EVENT_ANOMALUS_SPARK:
                    me->CastSpell(me->GetVictim(), SPELL_SPARK, false);
                    events.ScheduleEvent(EVENT_ANOMALUS_SPARK, 5s);
                    break;
                case EVENT_ANOMALUS_ARCANE_ATTRACTION:
                {
                    Unit* target = nullptr;
                    if (instance->IsMythic())
                        target = me->GetVictim();
                    else
                        target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true);

                    if (target)
                    {
                        me->CastSpell(target, SPELL_ARCANE_ATTRACTION, false);
                        if (instance->IsMythic())
                        {
                            scheduler.Schedule(3s, [&](TaskContext func)
                            {
                                Unit* target = me->GetVictim();
                                if (target)
                                {
                                    CustomSpellValues val;
                                    val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(15000,
                                        (instance->GetMythicKeyLevel() * 0.07f)));
                                    me->CastCustomSpell(SPELL_SPARK, val, target, TRIGGERED_FULL_MASK);
                                }
                            });
                        }
                    }
                    events.ScheduleEvent(EVENT_ANOMALUS_ARCANE_ATTRACTION, 15s);
                    break;
                }
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea();
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        return me->GetHomePosition().GetExactDist2d(me) > 60.0f;
    }
private:
    bool achievement;
    uint8 riftCount;
    bool preNerf;
    float _defaultCombatReach;

    uint32 magicZoneIdx;
    bool magicZoneReversing;
    TaskScheduler uninterruptibleScheduler;
};

class achievement_chaos_theory : public AchievementCriteriaScript
{
    public:
        achievement_chaos_theory() : AchievementCriteriaScript("achievement_chaos_theory") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            if (!target)
                return false;

            InstanceScript* instance = target->GetInstanceScript();
            if (!instance)
                return false;

            if (instance->IsMythic())
                return false;

            return target->GetAI()->GetData(target->GetEntry());
        }
};

enum MythicRift
{
    SPELL_CHAOTIC_RIFT_MYTHIC = 47687,
    SPELL_CHAOTIC_RIFT_ARCANEFORM = 48019,
    NPC_CHAOTIC_RIFT_WRAITH_MYTHIC = 91525
};

struct npc_chaotic_rift_mythic : public PassiveAI
{
    npc_chaotic_rift_mythic(Creature* creature) : PassiveAI(creature)
    {
        InstanceScript* instance = me->GetInstanceScript();
        IsMythic = instance ? instance->IsMythic() : false;
        //! if ismythic returns true then instance pointer has to be valid
        MythicKeyLevel = IsMythic ? instance->GetMythicKeyLevel() : 0;
    }

    void Reset() override
    {
        currentIdx = 0;
        scheduler.CancelAll();
        DoCastSelf(SPELL_CHAOTIC_RIFT_MYTHIC, true);
        DoCastSelf(SPELL_CHAOTIC_RIFT_ARCANEFORM, true);
        scheduler.Schedule(2s, [&](TaskContext func)
        {
            Creature* wraith = me->SummonCreature(NPC_CHAOTIC_RIFT_WRAITH_MYTHIC, me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN);
            Creature* summoner = me->GetSummoner();
            //! register for anomlus summonlist so it despawns on death/evade
            if (wraith && summoner && summoner->IsAIEnabled)
            {
                wraith->CastSpell(wraith, SPELL_ANOMALUS_SIMPLE_TELEPORT, true);
                summoner->AI()->JustSummoned(wraith);
                if (wraith->IsAIEnabled)
                    wraith->AI()->DoZoneInCombat(wraith, 100.f);
            }

            func.Repeat(22s);
        });

        scheduler.Schedule(6s, [&](TaskContext func)
        {
            std::vector<uint32> idxs(_mythicPortalSpawnPositions.size());
            std::iota(idxs.begin(), idxs.end(), 0);

            //! remove previous spawn or teleport position
            idxs.erase(std::remove(idxs.begin(), idxs.end(), currentIdx), idxs.end());
            currentIdx = Trinity::Containers::SelectRandomContainerElement(idxs);
            DoCastSelf(SPELL_ANOMALUS_SIMPLE_TELEPORT, true);

            scheduler.Schedule(1500ms, [&](TaskContext /*func*/)
            {
                me->NearTeleportTo(_mythicPortalSpawnPositions[currentIdx], false);
            });

            func.Repeat(20s);
        });
    }

    void SetData(uint32 type, uint32 /*value*/) override
    {
        currentIdx = type;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
private:
    TaskScheduler scheduler;
    uint32 MythicKeyLevel;
    bool IsMythic;
    uint32 currentIdx;
};

constexpr uint32 SPELL_WRATH_MYTHIC_ARCANE_MISSILES{ 33832 };
struct npc_crazed_mana_wraith_nexus_mythic : public ScriptedAI
{
    npc_crazed_mana_wraith_nexus_mythic(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _scheduledDeath = false;
        scheduler.CancelAll();
    }

    void EnterCombat(Unit* who) override
    {
        ScriptedAI::EnterCombat(who);
        scheduler.Schedule(5s, [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            if (target)
                //! Spell triggering another spell, damage increase is done on spellscript side
                DoCastVictim(SPELL_WRATH_MYTHIC_ARCANE_MISSILES);

            func.Repeat(15s);
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (damage >= me->GetHealth() - 1)
        {
            damage = 0;
            if (!_scheduledDeath)
            {
                me->AttackStop();
                me->SetPassive();
                me->GetMotionMaster()->Clear();
                me->SetSelectable(false);
                _scheduledDeath = true;

                scheduler.CancelAll();
                scheduler.Schedule(200ms, [&](TaskContext)
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 4000);
                    me->CastCustomSpell(SPELL_ANOMALUS_THUNDERSHOCK, val, me, TRIGGERED_FULL_MASK);
                    if (Creature* amz = me->FindNearestCreature(NPC_ANOMALUS_AMZ_TRIGGER, 4.f, true))
                    {
                        me->MonsterTextEmote("Wraith dies and with his last breath damages anti-magic zone", nullptr, false);
                        amz->DealDamage(amz, amz, 1);
                    }

                    Unit::Kill(me, me);
                });
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff, std::bind(&ScriptedAI::DoMeleeAttackIfReady, this));
    }

private:
    TaskScheduler scheduler;
    bool _scheduledDeath;
};

struct npc_anomalus_mythic_amz : public PassiveAI
{
    npc_anomalus_mythic_amz(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        scheduler.CancelAll();
        scheduler.Schedule(5s, [&](TaskContext func)
        {
            me->DealDamage(me, me, 1);
            if (me->GetHealth() == 1)
                func.Repeat(7500ms);
            else
                func.Repeat(5s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
private:
    TaskScheduler scheduler;
};

class spell_lightning_blast_anomalus : public SpellScript
{
    PrepareSpellScript(spell_lightning_blast_anomalus);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        targets.remove_if([&](WorldObject* obj)
        {
            return obj->ToUnit() && obj->ToUnit()->HasAura(SPELL_ANOMALUS_AMZ);
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lightning_blast_anomalus::FilterTargets,
            EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

constexpr uint32 SPELL_WRAITH_ARCANE_MISILES_TRIGGERED{ 33833 };
class spell_arcane_wraith_arcane_missiles_mythic : public AuraScript
{
    PrepareAuraScript(spell_arcane_wraith_arcane_missiles_mythic);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetCaster())
            return;

        Creature* caster = GetCaster()->ToCreature();
        if (!caster)
            return;

        Unit* victim = caster->GetVictim();
        if (!victim)
            return;

        InstanceScript* instance = caster->GetInstanceScript();
        if (!instance)
            return;

        if (instance->IsMythic())
        {
            PreventDefaultAction();
            CustomSpellValues val;
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_WRAITH_ARCANE_MISILES_TRIGGERED, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            caster->CastCustomSpell(SPELL_WRAITH_ARCANE_MISILES_TRIGGERED, val, victim, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_arcane_wraith_arcane_missiles_mythic::OnPeriodic,
            EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};
void AddSC_boss_anomalus()
{
    RegisterCreatureAI(boss_anomalus);
    RegisterCreatureAI(npc_chaotic_rift_mythic);
    RegisterCreatureAI(npc_crazed_mana_wraith_nexus_mythic);
    RegisterCreatureAI(npc_anomalus_mythic_amz);

    RegisterSpellScript(spell_lightning_blast_anomalus);
    RegisterAuraScript(spell_arcane_wraith_arcane_missiles_mythic);

    new achievement_chaos_theory();
}
