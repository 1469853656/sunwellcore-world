
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "blood_furnace.h"
#include "MoveSplineInit.h"
#include "SpellAuraEffects.h"

enum eEnums
{
    SAY_AGGRO                           = 0,
    SAY_KILL                            = 1,
    SAY_DIE                             = 2,

    SPELL_ACID_SPRAY                    = 38153,
    SPELL_EXPLODING_BREAKER             = 30925,
    SPELL_KNOCKDOWN                     = 20276,
    SPELL_DOMINATION                    = 25772,
    //! Mythic
    SPELL_MAKER_BLACK_CRYSTAL_STATE     = 32563,
    SPELL_MAKER_FELFIRE_BALL            = 66532,
    SPELL_MAKER_GOO                     = 70852,
    SPELL_MAKER_GREEN_BEAM              = 58847,
    SPELL_MAKER_ACID_BURST              = 70744,
    SPELL_MAKER_DESPERATE_RAGE          = 33898,

    NPC_MAKER_GOO_TRIGGER               = 91512,
    NPC_MAKER_PRISON_SACRIFICE          = 91513,
    NPC_MAKER_FELFIRE_TRIGGER           = 91514,
    EVENT_SPELL_ACID                    = 1,
    EVENT_SPELL_EXPLODING,
    EVENT_SPELL_DOMINATION,
    EVENT_SPELL_KNOCKDOWN
};

Position const CenterOfMakerRoom{ 330.527f, 86.3622f, 9.61512f, 4.63167f };
constexpr uint32 MAKER_PRISON_POSITION_SIZE{ 4 };
Position const SacrificePositions[MAKER_PRISON_POSITION_SIZE] =
{
    { 282.029f, 100.515f, 9.65654f, 6.26452f },
    { 282.553f, 69.2044f, 9.65437f, 0.0198205f },
    { 373.909f, 68.9496f, 9.65729f, 3.17791f },
    { 373.599f, 99.6030f, 9.65750f, 3.17555f }
};

constexpr uint32 MAKER_FELFIRE_TRIGGER_SIZE{ 4 };
Position const FelfireballTriggerPositions[]
{
    { 359.042f, 76.3816f, 23.6446f, 6.14906f },
    { 359.182f, 107.142f, 23.6446f, 0.23501f },
    { 295.351f, 107.914f, 23.6446f, 3.75045f },
    { 295.655f, 76.5325f, 23.9596f, 3.22267f }
};

struct boss_the_maker : public BossAI
{
    boss_the_maker(Creature* creature) : BossAI(creature, DATA_THE_MAKER)
    {
        if (instance->IsMythic())
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

            if (!me->GetVehicleKit())
                me->CreateVehicleKit(371, 0);
        }
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        return me->GetDistance(CenterOfMakerRoom) > 65.f;
    }

    void Reset() override
    {
        _currentSacrfice = 0;
        _duringCageEvent = false;
        _gooTriggerGUIDs.clear();
        _seatId = 1;
        _prisonCellEntries = { GO_MAKER_PRISON_CELL_1, GO_MAKER_PRISON_CELL_2,
            GO_MAKER_PRISON_CELL_3, GO_MAKER_PRISON_CELL_4 };

        for (uint32 entry : _prisonCellEntries)
        {
            GameObject* go = instance->GetGameObject(entry);
            if (go)
                go->SetGoState(GO_STATE_READY);
        }

        //! Before summoning sacrifices, base class calls summons.DespawnAll();
        BossAI::Reset();
        me->SetAggressive();

        if (instance->IsMythic())
        {
            for (Position const& pos : SacrificePositions)
            {
                Creature* trigger = me->SummonCreature(NPC_MAKER_PRISON_SACRIFICE, pos);
                if (trigger)
                {
                    trigger->SetImmuneToPC(true);
                    trigger->SetObjectScale(2.f);
                    trigger->CastSpell(trigger, SPELL_MAKER_BLACK_CRYSTAL_STATE, true);
                }
            }

            for (Position const& pos : FelfireballTriggerPositions)
            {
                Creature* trigger = me->SummonCreature(NPC_MAKER_FELFIRE_TRIGGER, pos);
                if (trigger)
                {
                    trigger->SetCanFly(true);
                    trigger->SetPassive();
                    _felfireTriggers.push_back(trigger->GetGUID());
                }
            }
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->setFaction(me->getFaction());
        summon->SetSelectable(false);
        summon->SetPassive();

        if (summon->GetEntry() == NPC_MAKER_GOO_TRIGGER)
        {
            summon->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, _seatId, me, false);
            ++_seatId;
        }

        if (_seatId > 2 && instance->IsMythic())
        {
            scheduler.Schedule(5s, [&](TaskContext func)
            {
                uint64 const guid = Trinity::Containers::SelectRandomContainerElement(_gooTriggerGUIDs);
                Creature* trigger = ObjectAccessor::GetCreature(*me, guid);
                if (trigger)
                {
                    std::list<Unit*> _targets;
                    uint64 victimGUID = me->GetVictim() ? me->GetVictim()->GetGUID() : 0;

                    SelectTargetList(_targets, [&](Unit* obj)
                    {
                        return obj->IsPlayer() && obj->GetGUID() != victimGUID && obj->GetDistance(me) > 9.f;
                    }, 5, SELECT_TARGET_RANDOM);

                    if (!_targets.empty())
                    {
                        Unit* target = Trinity::Containers::SelectRandomContainerElement(_targets);
                        if (target)
                            trigger->CastSpell(target, SPELL_MAKER_GOO, true);
                    }
                    else
                    {
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM);
                        if (target)
                            trigger->CastSpell(target, SPELL_MAKER_GOO, true);
                    }
                }

                func.Repeat(10s);
            });
        }
    }

    void EnterCombat(Unit* who) override
    {
        Talk(SAY_AGGRO);
        BossAI::EnterCombat(who);
        ScheduleNormalAbilities();

        if (instance->IsMythic())
        {
            for (auto i = 0; i < 2; ++i)
                if (Creature* goo = me->SummonCreature(NPC_MAKER_GOO_TRIGGER, me->GetPosition()))
                    _gooTriggerGUIDs.push_back(goo->GetGUID());

            scheduler.Schedule(Seconds(40 - instance->GetMythicKeyLevel()), [&](TaskContext func)
            {
                uint32 _entry = Trinity::Containers::SelectRandomContainerElement(_prisonCellEntries);
                _prisonCellEntries.erase(std::remove(_prisonCellEntries.begin(), _prisonCellEntries.end(), _entry),
                    _prisonCellEntries.end());

                GameObject* go = instance->GetGameObject(_entry);
                if (go)
                {
                    _duringCageEvent = true;
                    _currentGateTarget = _entry;
                    Movement::MoveSplineInit init(me);
                    Movement::PointsArray path;

                    float x, y, z;
                    go->GetClosePoint(x, y, z, go->GetObjectSize() / 3, 2.f);
                    me->GetMotionMaster()->Clear();
                    me->AttackStop();
                    me->SetPassive();
                    path.push_back(G3D::Vector3{ me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() });
                    path.push_back(G3D::Vector3{ x,y,z });
                    init.MovebyPath(path);
                    init.SetSmooth();
                    int32 duration = init.Launch();
                    scheduler.DelayAll(Milliseconds(duration + 6500));
                    events.DelayEvents(Milliseconds(duration + 6500));
                    scheduler.Schedule(Milliseconds(duration), [&](TaskContext func)
                    {
                        GameObject* gate = instance->GetGameObject(_currentGateTarget);
                        if (gate)
                            gate->SetGoState(GO_STATE_ACTIVE);

                        Creature* trigger = me->FindNearestCreature(NPC_MAKER_PRISON_SACRIFICE, 30.f, true);
                        if (trigger)
                        {
                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 5000);
                            me->CastCustomSpell(SPELL_MAKER_GREEN_BEAM, val, trigger);
                            _currentSacrfice = trigger->GetGUID();
                        }

                        scheduler.Schedule(5s, [&](TaskContext func)
                        {
                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 15000);
                            val.AddSpellMod(SPELLVALUE_BASE_POINT1, 15000);
                            me->CastCustomSpell(SPELL_MAKER_ACID_BURST, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                            Creature* triggerToKill = ObjectAccessor::GetCreature(*me, _currentSacrfice);
                            if (triggerToKill)
                                triggerToKill->DespawnOrUnsummon();
                            DoCastSelf(SPELL_MAKER_DESPERATE_RAGE, true);
                            me->SetAggressive();
                            DoZoneInCombat(me, 100.f);
                            _duringCageEvent = false;
                        });
                    });
                }

                if (!_prisonCellEntries.empty())
                    func.Repeat(Seconds(40 - instance->GetMythicKeyLevel()));
            });

            scheduler.Schedule(10s, [&](TaskContext func)
            {
                uint64 const guid = Trinity::Containers::SelectRandomContainerElement(_felfireTriggers);
                Creature* trigger = ObjectAccessor::GetCreature(*me, guid);
                if (trigger)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 0.f, true);
                    if (target)
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MAKER_FELFIRE_BALL, data));
                        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                        val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1000);
                        trigger->CastCustomSpell(SPELL_MAKER_FELFIRE_BALL, val, target,
                            TRIGGERED_NONE, NullItemRef, (const AuraEffect*)nullptr, me->GetGUID());
                    }
                }

                func.Repeat(randseconds(2, 3));
            });
        }
    }

    void ScheduleNormalAbilities()
    {
        events.ScheduleEvent(EVENT_SPELL_ACID, 15s);
        if (!instance->IsMythic())
        {
            events.ScheduleEvent(EVENT_SPELL_EXPLODING, 6s);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_SPELL_DOMINATION, 2min);
        }

        events.ScheduleEvent(EVENT_SPELL_KNOCKDOWN, 10s);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER && roll_chance_i(33))
            Talk(SAY_KILL);
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        Talk(SAY_DIE);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 325.413483f, 86.541061f, 9.615262f, 1.546639f };
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
        {
            if (_duringCageEvent)
                scheduler.Update(diff);

            return;
        }

        scheduler.Update(diff);
        events.Update(diff);
        if (me->IsCasting())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPELL_ACID:
                    if (instance->IsMythic())
                    {
                        int32 stackCount = 0;
                        if (Aura* aura = me->GetAura(SPELL_MAKER_DESPERATE_RAGE))
                            stackCount = aura->GetStackAmount();
                        CustomSpellValues val;
                        int32 auraDuration = 8000 - (stackCount * 500);
                        val.AddSpellMod(SPELLVALUE_AURA_DURATION, auraDuration);
                        me->CastCustomSpell(SPELL_ACID_SPRAY, val, me->GetVictim(), TRIGGERED_NONE);
                    }
                    else
                    me->CastSpell(me->GetVictim(), SPELL_ACID_SPRAY, false);
                    events.Repeat(Milliseconds(urand(15000, 23000)));
                    break;
                case EVENT_SPELL_EXPLODING:
                {
                    auto repeatTime = Milliseconds(urand(7000, 11000));
                    if (!instance->IsMythic())
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U))
                            me->CastSpell(target, SPELL_EXPLODING_BREAKER, false);
                    }
                    else
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_EXPLODING_BREAKER, data));
                        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U))
                            me->CastCustomSpell(SPELL_EXPLODING_BREAKER, val, target);
                        repeatTime = 10000ms;
                    }
                    events.Repeat(repeatTime);
                    break;
                }
                case EVENT_SPELL_DOMINATION:
                {
                    if (!instance->IsMythic())
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            me->CastSpell(target, SPELL_DOMINATION, false);

                        events.Repeat(2min);
                    }
                    else
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                        {
                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 12000);
                            me->CastCustomSpell(SPELL_DOMINATION, val, target);
                        }

                        events.Repeat(12s);
                    }
                    break;
                }
                case EVENT_SPELL_KNOCKDOWN:
                {
                    if (!instance->IsMythic())
                        me->CastSpell(me->GetVictim(), SPELL_KNOCKDOWN, false);
                    else
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_KNOCKDOWN, data));
                        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                        if (Unit* target = me->GetVictim())
                            me->CastCustomSpell(SPELL_KNOCKDOWN, val, target);
                    }

                    events.Repeat(Milliseconds(urand(4000, 12000)));
                    break;
                }
                default:
                    break;
            }

            if (me->IsCasting())
                return;
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea();
    }
private:
    uint32 _seatId;
    std::vector<uint64> _gooTriggerGUIDs;
    std::vector<uint32> _prisonCellEntries;
    bool _duringCageEvent;
    uint32 _currentGateTarget;
    uint64 _currentSacrfice;
    std::vector<uint64> _felfireTriggers;
};

class spell_acid_spray_the_maker : public AuraScript
{
    PrepareAuraScript(spell_acid_spray_the_maker);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (!GetCaster() || !GetCaster()->IsCreature())
            return;

        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (!instance)
            return;

        bool isMythic = instance->IsMythic();
        if (!isMythic)
            return;

        PreventDefaultAction();
        uint32 triggeredSpellId = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
        Mythic::MythicSpells data;
        ASSERT(sMythicMgr.GetSpellValuesFor(triggeredSpellId, data));
        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
        val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
        if (Unit* target = GetCaster()->GetVictim())
            GetCaster()->CastCustomSpell(triggeredSpellId, val, target, TRIGGERED_FULL_MASK);

        if (Aura* aura = GetCaster()->GetAura(SPELL_MAKER_DESPERATE_RAGE))
        {
            uint32 rageCount = aura->GetStackAmount();
            int32 periodicTimer = std::max(static_cast<int32>(50), static_cast<int32>(rageCount * 50));
            if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_1))
                effect->SetPeriodicTimer(800 - periodicTimer);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_acid_spray_the_maker::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

class spell_felfire_ball_the_maker : public SpellScript
{
    PrepareSpellScript(spell_felfire_ball_the_maker);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (GetHitPlayer() && GetHitPlayer()->IsCasting() && GetHitPlayer()->GetInstanceScript() && GetHitPlayer()->GetInstanceScript()->IsMythic())
            GetHitPlayer()->InterruptNonMeleeSpells(false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_felfire_ball_the_maker::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

void AddSC_boss_the_maker()
{
    RegisterCreatureAI(boss_the_maker);
    RegisterAuraScript(spell_acid_spray_the_maker);
    RegisterSpellScript(spell_felfire_ball_the_maker);
}

