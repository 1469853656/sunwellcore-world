
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "blood_furnace.h"
#include "Player.h"

#include "MoveSplineInit.h"
#include "MythicAI.h"

#include <numeric>

enum KelidanMisc
{
    SAY_WAKE = 0,
    SAY_ADD_AGGRO,
    SAY_KILL,
    SAY_NOVA,
    SAY_DIE,

    // Keldian spells
    SPELL_CORRUPTION = 30938,
    SPELL_CORRUPTION_MYTHIC = 71937,
    SPELL_EVOCATION = 30935,
    SPELL_FIRE_NOVA = 33132,
    SPELL_SHADOW_BOLT_VOLLEY = 28599,
    SPELL_BURNING_NOVA = 30940,
    SPELL_VORTEX = 37370,
    SPELL_KELIDAN_UNQUENCHABLE_FLAMES = 64705,
    SPELL_KELIDAN_UNQUENCHABLE_FLAMES_ALLY = 64706,

    // Channelers spells
    SPELL_SHADOW_BOLT = 12739, // uses the same values in mythic as shadowmoon warlocks shadowbolt
    SPELL_SHADOW_BOLT_H = 15472,
    SPELL_MARK_OF_SHADOW = 30937,
    SPELL_CHANNELING = 39123,
    // mythic
    SPELL_KELIDAN_METEOR = 57467,
    SPELL_KELIDAN_FIERY_AURA_METEOR = 38841,
    SPELL_KELIDAN_SHADOW_WHIP = 30638, //unsued
    SPELL_KELIDAN_FLAMES = 64561,
    SPELL_KELIDAN_FLAMES_DMG = 64566,
    NPC_KELIDAN_FLAMES_INITIAL = 91518,
    NPC_KELIDAN_FLAMES_SECONDARY = 91519,
    NPC_KELIDAN_FLAME_TSUNAMI = 30616,
    SPELL_KELIDAN_INVISIBLITY = 50013,
    NPC_KELIDAN_SAFE_ZONE_MARKER = 91550,
    NPC_KELIDAN_METEOR_TARGET = 91551,
    NPC_KELIDAN_ELEMENTAL = 91553,

    // Events
    EVENT_SPELL_VOLLEY = 1,
    EVENT_SPELL_CORRUPTION,
    EVENT_SPELL_BURNING_NOVA,
    EVENT_SPELL_FIRE_NOVA,
    EVENT_SPELL_SHADOW_BOLT,
    EVENT_SPELL_MARK,

    // Actions
    ACTION_CHANNELER_ENGAGED = 1,
    ACTION_CHANNELER_DIED,

    SCHEDULER_DELAY_GROUP_NORMAL = 100,

    SPELL_KELIDAN_ELEM_IMMUNE_ARCANE = 34304,
    SPELL_KELIDAN_ELEM_IMMUNE_SHADOW = 34309
};

constexpr uint32 SHADOWMOON_CHANNELER_POSITIONS_SIZE{ 5 };
Position const ShadowmoonChannelers[SHADOWMOON_CHANNELER_POSITIONS_SIZE] =
{
    { 302.0f, -87.00f, -24.4f, 0.157f },
    { 321.0f, -63.50f, -24.6f, 4.887f },
    { 346.0f, -74.50f, -24.6f, 3.595f },
    { 344.0f, -103.5f, -24.5f, 2.356f },
    { 316.0f, -109.0f, -24.6f, 1.257f }
};

enum SafetyZoneMisc
{
    SPELL_KELIDAN_ARCANE_OVERLOAD_AURA = 56432,
    SPELL_KELIDAN_EMPOWERED_SHOCK_VORTEX = 72038,
    SPELL_KELIDAN_COSMIC_SMASH_VIS_STATE = 62300,
    SPELL_KELIDAN_SAFE_ZONE_VISUAL = 45537,
    SPELL_KELIDAN_SAFE_ZONE_CONSE = 59998,
    SPELL_KELIDAN_TRIGGER_SPELL_IMMUNITY = 6356,
    SPELL_KELIDAN_COSMIC_SMASH = 62304,
    SPELL_KELIDAN_COSMIC_SMASH_TRIG = 62311,
    SPELL_KELIDAN_FIRE_CHANNELING = 45845,
    SPELL_KELIDAN_MYTHIC_EARTHQUAKE = 37764,
    DATA_METEOR_START_SMASH,
    DATA_SECRET_PHASE_ENABLE_MAGTHERIDON
};

Position const CenterOfTheRoom{ 325.943f, -87.0352f, -24.6524f, 4.59608f };

constexpr uint32 SafeSpotLocationSize{ 15 };
Position const SafeSpotLocations[SafeSpotLocationSize] =
{
    { 307.638f, -126.595f, -25.242f },
    { 282.535f, -87.0260f, -25.003f },
    { 317.311f, -44.7480f, -25.269f },

    { 282.535f, -87.026f, -25.003f },
    { 317.311f, -44.748f, -25.269f },
    { 362.954f, -64.725f, -25.125f },

    { 317.311f, -44.7489f, -25.269f },
    { 362.954f, -64.7259f, -25.125f },
    { 358.110f, -116.373f, -25.167f },

    { 362.954f, -64.7250f, -25.125f },
    { 358.110f, -116.373f, -25.167f },
    { 307.638f, -126.595f, -25.242f },

    { 358.110f, -116.373f, -25.167f },
    { 307.638f, -126.595f, -25.242f },
    { 282.535f, -87.0260f, -25.003f }
};

struct boss_kelidan_the_breaker : public BossAI
{
    boss_kelidan_the_breaker(Creature* creature)
        : BossAI(creature, DATA_KELIDAN)
        , _gateGUID( 0 )
    {
        scheduler.SetValidator([&]
        {
            return !me->IsCasting() && !me->isStuned();
        });

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

            me->SetObjectScale( 2.f );
        }
    }

    void EnterEvadeMode() override
    {
        if (Creature* magda = instance->GetCreature(DATA_MAGTHERIDON_BF))
            if (magda->isDead())
                magda->Respawn();

        if (GameObject* gate = ObjectAccessor::GetGameObject(*me, _gateGUID))
            gate->RemoveFromWorld();

        _DespawnAtEvade(10U, me);
    }

    void Reset() override
    {
        if (GameObject* gate = ObjectAccessor::GetGameObject(*me, _gateGUID))
            gate->RemoveFromWorld();
        _gateGUID = 0;
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
        _endingScheduled = false;
        _secretPhaseScheduled = false;
        _magtheridonDead = false;
        _previousSafeZoneSet = 0;
        _secondScheduler.CancelAll();
        BossAI::Reset();

        _didAggroYell = false;
        ApplyImmunities(true);
        SummonChannelersAndDrawPentagram();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
        DoCastSelf(SPELL_EVOCATION);
        me->RemoveAurasDueToSpell(SPELL_KELIDAN_FIRE_CHANNELING);
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        Position const& center = me->GetHomePosition();
        return me->GetDistance(center) > 70.f;
    }

    void JustRespawned() override
    {
        BossAI::JustRespawned();
        DoCastSelf(SPELL_EVOCATION);
    }

    void EnterCombat(Unit* who) override
    {
        if ( !_EnterCombat() )
            return;

        GameObject* _gate = me->SummonGameObject(180322, { 327.212f, -157.206f, -25.5164f, 1.55582f }, 0.f, 0.f, 0.f, 0.f, 0);
        if (_gate)
            _gateGUID = _gate->GetGUID();
        _secretPhaseAllowed = instance->GetMythicKeyLevel() >= Mythic::MYTHIC_SECRET_PHASE_KEY_LEVEL;

        me->InterruptNonMeleeSpells(false);
        Talk(SAY_WAKE);
        scheduler.Schedule(2s, [&](TaskContext func)
        {
            if (!me->GetVictim())
            {
                if (!me->SelectNearestPlayer(200.f))
                    EnterEvadeMode();
            }

            func.Repeat(3s);
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == POINT_MOTION_TYPE && pointId == 1 && !_endingScheduled)
        {
            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            _endingScheduled = true;
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(10000, (instance->GetMythicKeyLevel() * 0.03f)));
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 30000);
            me->CastCustomSpell(SPELL_KELIDAN_MYTHIC_EARTHQUAKE, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        }
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == DATA_SECRET_PHASE_ENABLE_MAGTHERIDON && value == DONE)
        {
            me->SetAggressive();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
            DoZoneInCombat(me, 100.f);
            _magtheridonDead = true;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (_secretPhaseAllowed && damage >= me->GetHealth())
        {
            if (!_magtheridonDead)
                damage = 0;

            if (!_secretPhaseScheduled)
            {
                _secretPhaseScheduled = true;
                scheduler.CancelAll();
                _secondScheduler.CancelAll();
                summons.DespawnAll();
                me->SetPassive();
                me->AttackStop();
                me->GetMotionMaster()->Clear();
                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAuras();
                me->SetHealth(1);

                if (Creature* magtheridon = instance->GetCreature(DATA_MAGTHERIDON_BF))
                    if (magtheridon->IsAIEnabled)
                        magtheridon->AI()->SetData(DATA_SECRET_PHASE_ENABLE_MAGTHERIDON, DATA_SECRET_PHASE_ENABLE_MAGTHERIDON);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                me->GetMotionMaster()->MovePoint(1, CenterOfTheRoom);
            }
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void KilledUnit(Unit* victim) override
    {
        BossAI::KilledUnit(victim);
        if (victim && victim->IsPlayer() && roll_chance_i(33))
            Talk(SAY_KILL);
    }

    void DoAction(int32 param) override
    {
        if (param == ACTION_CHANNELER_ENGAGED)
        {
            if (!_didAggroYell)
            {
                _didAggroYell = true;
                Talk(SAY_ADD_AGGRO);

                for (size_t i = 0; i < _channelerGUIDs.size(); ++i)
                {
                    Creature* channeler = ObjectAccessor::GetCreature(*me, _channelerGUIDs[i]);
                    if (channeler && !channeler->IsInCombat())
                        channeler->SetInCombatWithZone();
                }
            }
        }
        else if (param == ACTION_CHANNELER_DIED)
        {
            for (size_t i = 0; i < _channelerGUIDs.size(); ++i)
            {
                Creature* channeler = ObjectAccessor::GetCreature(*me, _channelerGUIDs[i]);
                if (channeler && channeler->IsAlive())
                    return;
            }

            ScheduleAbilities(true);
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
            if (Player* target = me->SelectNearestPlayer(100.0f))
                AttackStart(target);
        }
    }

    void SummonChannelersAndDrawPentagram()
    {
        _channelerGUIDs.clear();
        summons.DespawnAll();

        for (uint16 i = 0; i < 5; ++i)
            if (Creature* channeler = me->SummonCreature(NPC_CHANNELER, ShadowmoonChannelers[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000))
                _channelerGUIDs.push_back(channeler->GetGUID());

        uint16 summonSize = _channelerGUIDs.size();
        for (uint16 j = 0; j < summonSize; ++j)
        {
            Creature* caster = me->GetMap()->GetCreature(_channelerGUIDs[j]);
            Creature* target = me->GetMap()->GetCreature(_channelerGUIDs[(j + 2) % summonSize]);
            if (caster && target)
                caster->CastSpell(target, SPELL_CHANNELING, false);
        }
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        // load grid with start doors
        me->GetMap()->LoadGrid(0, -111.0f);
        BossAI::JustDied(killer);
        Talk(SAY_DIE);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_KELIDAN_UNQUENCHABLE_FLAMES);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_KELIDAN_UNQUENCHABLE_FLAMES_ALLY);

        if (GameObject* go = ObjectAccessor::GetGameObject(*me, _gateGUID))
            go->RemoveFromWorld();

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 326.357300f, -87.161369f, -24.652618f, 4.668618f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void ApplyImmunities(bool apply)
    {
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_KNOCKOUT, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHACKLE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, apply);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, apply);
    }

    void HandleVortex()
    {
        ApplyImmunities(false);
        me->AddAura(SPELL_BURNING_NOVA, me);
        ApplyImmunities(true);
        if (IsHeroic() || instance->IsMythic())
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
            me->CastCustomSpell(SPELL_VORTEX, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            DoCastAOE(SPELL_VORTEX, true);
        }
    }

    void ScheduleMeteorShower()
    {
        //! Handle smashes
        auto GenerateMeteorPosition = [](float angle, float dist)
        {
            float x, y, z;
            x = CenterOfTheRoom.GetPositionX() + dist * std::cos(angle);
            y = CenterOfTheRoom.GetPositionY() + dist * std::sin(angle);
            z = CenterOfTheRoom.GetPositionZ() + 0.5f;
            return Position{ x, y, z };
        };

        _secondScheduler.Schedule(9s, [&](TaskContext func)
        {
            if (func.GetRepeatCounter() < 3)
            {
                for (uint32 _smashCount = 0; _smashCount < 9; ++_smashCount)
                {
                    float _smashAngle = frand(0.f, static_cast<float>(2 * M_PI));
                    float _smashDistance = frand(0.f, 65.f);

                    float x, y, z;
                    x = CenterOfTheRoom.m_positionX + _smashDistance * std::cos(_smashAngle);
                    y = CenterOfTheRoom.m_positionY + _smashDistance * std::sin(_smashAngle);
                    z = me->GetMap()->GetHeight(x, y, CenterOfTheRoom.m_positionZ);
                    Creature* smash = me->SummonCreature(NPC_KELIDAN_METEOR_TARGET, { x, y, z });
                    if (smash && smash->IsAIEnabled)
                    {
                        smash->SetSelectable(false);
                        smash->AI()->SetData(DATA_METEOR_START_SMASH, DATA_METEOR_START_SMASH);
                        smash->CastSpell(smash, SPELL_KELIDAN_COSMIC_SMASH_VIS_STATE, true);
                    }
                }

                func.Repeat(2250ms);
            }
        });
    }

    void CreateSafeZones(uint32 _count)
    {
        summons.DespawnEntry(NPC_KELIDAN_ELEMENTAL);
        summons.DespawnEntry(NPC_KELIDAN_SAFE_ZONE_MARKER);
        //std::vector<uint32> _posSets(SafeSpotLocationSize);

        //for (uint32 i = 0; i < SafeSpotLocationSize; ++i)
        //{
        //    if (i % 3 == 0)
        //    {
        //        _posSets.push_back(i);
        //        stream << " " << std::to_string(i) << ", ";
        //    }
        //}
        //_posSets.erase(std::remove(_posSets.begin(), _posSets.end(), _previousSafeZoneSet), _posSets.end());

        //Trinity::Containers::RandomResize(_posSets, 1U);
        //uint32 idx = _posSets[0];
        //_previousSafeZoneSet = idx;
        //if (idx > SafeSpotLocationSize)
        //    idx = 0;

        //stream << "Spawnie zones zaczynajac od point: " << idx << " i konczac na: " << idx + 2 << std::endl;
        //me->MonsterYell(stream.str().c_str(), 0, nullptr);
        //for (uint32 i = 0; i < 3; ++i, ++idx)
        //{
        // styling is weird because i want to keep the old handling somewhere nearby
        // previously you spawned 3 safe spots instead of 1
            Position const& triggerPosition = Trinity::Containers::SelectRandomContainerElement(SafeSpotLocations);
            Creature* mainTrigger = me->SummonCreature(NPC_KELIDAN_SAFE_ZONE_MARKER, triggerPosition);
            Creature* elemental = me->SummonCreature(NPC_KELIDAN_ELEMENTAL, triggerPosition);
            if (mainTrigger && elemental)
            {
                mainTrigger->setFaction(me->getFaction());
                mainTrigger->SetMaxHealth(10000000);
                mainTrigger->SetFullHealth();
                mainTrigger->SetSelectable(false);
                mainTrigger->SetPassive();

                mainTrigger->CastSpell(mainTrigger, 70571, true);
                mainTrigger->DespawnOrUnsummon(20s);

                elemental->DespawnOrUnsummon(20s);
                elemental->SetRegeneratingHealth(false);
                elemental->SetControlled(true, UNIT_STATE_ROOT);
                for (uint32 i = SPELL_KELIDAN_ELEM_IMMUNE_ARCANE; i < SPELL_KELIDAN_ELEM_IMMUNE_SHADOW + 1; ++i)
                {
                    // holy immunity
                    if (i == 34307)
                        continue;

                    elemental->CastSpell(elemental, i, true);
                }

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 20000);
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 4000);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 3000);
                elemental->CastCustomSpell(SPELL_KELIDAN_SAFE_ZONE_CONSE, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            }
        //}
    }

    void ScheduleSafeZone(Seconds secs)
    {
        _secondScheduler.Schedule(secs, [&](TaskContext func)
        {
            scheduler.CancelAll();
            if (!me->IsCasting())
                DoCastSelf(SPELL_KELIDAN_FIRE_CHANNELING);

            if (func.GetRepeatCounter() < 3)
            {
                CreateSafeZones(3U);
                ScheduleMeteorShower();
                func.Repeat(19s);
            }
            else
            {
                DoResetThreat();
                me->InterruptNonMeleeSpells(false);
                ScheduleAbilities(false);
            }
        });
    }

    void ScheduleAbilities(bool first)
    {
        if ( instance->IsMythic() )
            ScheduleSafeZone( first ? 30s : 60s );

        scheduler.Schedule(100ms, SCHEDULER_DELAY_GROUP_NORMAL, [&](TaskContext func)
        {
            auto repeatTime = randseconds(8, 13);
            if (!instance->IsMythic())
                DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOW_BOLT_VOLLEY, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_SHADOW_BOLT_VOLLEY, val, (Unit*)nullptr);
                repeatTime = randseconds(15, 17);
            }

            func.Repeat(repeatTime);
        });

        scheduler.Schedule(first ? randseconds(5, 6) : randseconds(3, 5), SCHEDULER_DELAY_GROUP_NORMAL, [&](TaskContext func)
        {
            auto repeat = randseconds(30, 50);
            if (!instance->IsMythic())
                DoCastAOE(SPELL_CORRUPTION);
            else
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 2500 + (600 * instance->GetMythicKeyLevel()));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 15000);
                std::list<Unit*> playerList;
                SelectTargetList(playerList, [](Unit* obj)
                {
                    return obj->IsPlayer();
                }, urand(2, 3), SELECT_TARGET_RANDOM);

                for (Unit* unit : playerList)
                {
                    val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                    me->CastCustomSpell(SPELL_CORRUPTION_MYTHIC, val, unit, TRIGGERED_FULL_MASK);
                }

                repeat = 22s;
            }

            func.Repeat(repeat);
        });

        if (!instance->IsMythic())
        {
            scheduler.Schedule(15s, [&](TaskContext func)
            {
                HandleVortex();
                func.Repeat(randseconds(25, 32));
            });
        }

        if (instance->IsMythic())
        {
            scheduler.Schedule(first ? 10s : 15s, SCHEDULER_DELAY_GROUP_NORMAL, [&](TaskContext func)
            {
                HandleVortex();
                func.Repeat(30s);
            });

            scheduler.Schedule(15s, [&](TaskContext func)
            {
                _flamesScheduled = true;
                scheduler.Schedule(1s, [&](TaskContext func)
                {
                    summons.DespawnEntry(NPC_KELIDAN_FLAMES_INITIAL);
                    me->SummonCreature(NPC_KELIDAN_FLAMES_INITIAL, CenterOfTheRoom);
                    func.Repeat(30s);
                });
            });

            scheduler.Schedule(first ? 12s : randseconds(5, 8), [&](TaskContext func)
            {
                bool needToCast = true;
                ThreatContainer::StorageType const& threatlist = me->getThreatManager().getThreatList();
                if (!threatlist.empty())
                {
                    for (auto const ref : threatlist)
                    {
                        Unit* target = ref->getTarget();
                        if (target && target->HasAura(SPELL_KELIDAN_UNQUENCHABLE_FLAMES_ALLY) ||
                            target->HasAura(SPELL_KELIDAN_UNQUENCHABLE_FLAMES))
                        {
                            needToCast = false;
                            break;
                        }
                    }
                }

                if (needToCast)
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 200.f, true))
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 200);
                    me->CastCustomSpell(SPELL_KELIDAN_UNQUENCHABLE_FLAMES, val, target, TRIGGERED_FULL_MASK);
                }

                func.Repeat(15s);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _secondScheduler.Update(diff);
        scheduler.Update(diff);
        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea();
    }
private:
    std::vector<uint64> _channelerGUIDs;
    uint64 _gateGUID;
    bool _didAggroYell;
    bool _flamesScheduled;
    bool _secretPhaseAllowed;
    bool _secretPhaseScheduled;
    bool _endingScheduled;
    bool _magtheridonDead;
    uint32 _previousSafeZoneSet;
    TaskScheduler _secondScheduler;
};

struct npc_shadowmoon_channeler : public ScriptedAI
{
    npc_shadowmoon_channeler(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
        _IsMythic = instance ? instance->IsMythic() : false;
        _MythicKeyLevel = instance && _IsMythic ? instance->GetMythicKeyLevel() : 0;
    }

    Creature* GetKelidan()
    {
        if (instance)
            return instance->GetCreature(DATA_KELIDAN);
        return nullptr;
    }

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* who) override
    {
        if (Creature* kelidan = GetKelidan())
            kelidan->AI()->DoAction(ACTION_CHANNELER_ENGAGED);

        if (instance)
            instance->SetBossState(DATA_KELIDAN, IN_PROGRESS);

        me->InterruptNonMeleeSpells(false);
        events.ScheduleEvent(EVENT_SPELL_SHADOW_BOLT,Milliseconds(urand(1500, 3500)));
        events.ScheduleEvent(EVENT_SPELL_MARK, Milliseconds(urand(5000, 6500)));
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* kelidan = GetKelidan())
            kelidan->AI()->DoAction(ACTION_CHANNELER_DIED);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->IsCasting())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPELL_SHADOW_BOLT:
                {
                    auto repeatTime = Milliseconds(urand(6000, 7500));
                    if (!IsMythic())
                        DoCastVictim(IsHeroic() ? SPELL_SHADOW_BOLT_H :
                            SPELL_SHADOW_BOLT);
                    else
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOW_BOLT, data));
                        float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        if (Unit* target = me->GetVictim())
                            me->CastCustomSpell(SPELL_SHADOW_BOLT, val, target);

                        repeatTime = 4s;
                    }

                    events.Repeat(repeatTime);
                    break;
                }
                case EVENT_SPELL_MARK:
                {
                    auto repeatTime = Milliseconds(urand(16000, 17500));

                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                    {
                        return !obj->HasAura(SPELL_MARK_OF_SHADOW);
                    });

                    if (!target)
                        target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                    if (target)
                    {
                        if (!IsMythic())
                            DoCast(target, SPELL_MARK_OF_SHADOW);
                        else
                        {
                            Mythic::MythicSpells data;
                            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MARK_OF_SHADOW, data));
                            float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                            CustomSpellValues val;
                            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                            if (Unit* target = me->GetVictim())
                                me->CastCustomSpell(SPELL_MARK_OF_SHADOW, val, target);

                            repeatTime = randseconds(4, 7);
                        }
                    }

                    events.Repeat(repeatTime);
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

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        return me->GetHomePosition().GetExactDist2d(me) > 68.f;
    }

    void EnterEvadeMode() override
    {
        Creature* kelidan = instance->GetCreature(DATA_KELIDAN);
        CreatureAI* ai = kelidan ? kelidan->AI() : nullptr;
        if (kelidan && ai)
            ai->EnterEvadeMode();
        ScriptedAI::EnterEvadeMode();
    }

private:
    EventMap events;
    InstanceScript* instance;
    bool _IsMythic;
    inline bool IsMythic() const { return _IsMythic; }
    uint32 _MythicKeyLevel;
    inline uint32 GetMythicKeyLevel() const { return _MythicKeyLevel; }
};

constexpr uint32 SPELL_RAGE_OF_MAGTHERIDON{ 30939 };
struct npc_kelidan_buffer_trigger : public PassiveAI
{
    npc_kelidan_buffer_trigger(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        me->setFaction(35);
        task.CancelAll();
        task.Schedule(2s, [&](TaskContext func)
        {
            if (func.GetRepeatCounter() >= 5)
                me->DespawnOrUnsummon();
            else
                DoCastAOE(SPELL_RAGE_OF_MAGTHERIDON, true);
        });
    }
private:
    TaskScheduler task;
};

class spell_kelidan_burning_nova : public AuraScript
{
    PrepareAuraScript(spell_kelidan_burning_nova);

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetTarget())
            return;

        Creature* target = GetTarget()->ToCreature();
        if (!target)
            return;

        InstanceScript* instance = target->GetInstanceScript();
        bool const IsMythic = !instance ? false : instance->IsMythic();
        if (!IsMythic)
            target->CastSpell(target, SPELL_FIRE_NOVA, TRIGGERED_FULL_MASK);
        else
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FIRE_NOVA, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
            target->CastCustomSpell(SPELL_FIRE_NOVA, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_kelidan_burning_nova::HandleRemove, EFFECT_0, SPELL_AURA_MOD_STUN,
            AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_rage_of_magtheridon_targets : public SpellScript
{
    PrepareSpellScript(spell_rage_of_magtheridon_targets);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        bool const IsMythic = caster->GetInstanceScript() ? caster->GetInstanceScript()->IsMythic() : false;
        if (IsMythic)
        {
            targets.remove_if([](WorldObject* obj)
            {
                return !obj->IsPlayer();
            });
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rage_of_magtheridon_targets::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class spell_kelidan_fiery_marker : public AuraScript
{
    PrepareAuraScript(spell_kelidan_fiery_marker);

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetTarget())
            return;

        InstanceScript* instance = GetTarget()->GetInstanceScript();
        bool const IsMythic = !instance ? false : instance->IsMythic();
        if (IsMythic && instance)
        {
            Creature* kelidan = instance->GetCreature(DATA_KELIDAN);
            if (!kelidan)
                return;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 27500);
            kelidan->CastCustomSpell(SPELL_KELIDAN_METEOR, val, GetTarget(), TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_kelidan_fiery_marker::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY,
            AURA_EFFECT_HANDLE_REAL);
    }
};

struct npc_kelidan_flames_initial : public PassiveAI
{
    npc_kelidan_flames_initial(Creature* creature) : PassiveAI(creature)
    {
        flameCount = 8;
        distFromCenter = 0.5f;
        firstFlames = true;
    }

    void Reset() override
    {
        scheduler.CancelAll();
        scheduler.Schedule(1s, [&](TaskContext func)
        {
            SpawnNextWave();
            func.Repeat(750ms);
        });
    }

    void SpawnNextWave()
    {
        if (distFromCenter >= 70.f)
            return;

        float const _maxAngle = float(2 * M_PI);
        float const _step = _maxAngle / float(flameCount);
        float angle = 0.f;

        for (uint32 i = 0; i < (firstFlames ? 2 : flameCount); angle += _step, ++i)
        {
            Position spawn;
            spawn.m_positionX = CenterOfTheRoom.GetPositionX() + distFromCenter * cosf(angle);
            spawn.m_positionY = CenterOfTheRoom.GetPositionY() + distFromCenter * sinf(angle);
            spawn.m_positionZ = CenterOfTheRoom.m_positionZ;
            Creature* flame = me->SummonCreature(NPC_KELIDAN_FLAMES_SECONDARY, spawn);
            if (flame)
            {
                flame->CastSpell(flame, SPELL_KELIDAN_FLAMES, true);
            }
        }

        flameCount += 2;
        distFromCenter += 4.5f;
        if (firstFlames)
            firstFlames = false;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
private:
    TaskScheduler scheduler;
    float distFromCenter;
    uint32 flameCount;
    bool firstFlames;
};

struct npc_kelidan_flame_secondary : public PassiveAI
{
    npc_kelidan_flame_secondary(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        scheduler.CancelAll();
        scheduler.Schedule(1250ms, [&](TaskContext func)
        {
            me->RemoveAllAuras();
            me->DespawnOrUnsummon(250ms);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
private:
    TaskScheduler scheduler;
};

struct npc_kelidan_meteor_target_mythic : public PassiveAI
{
    npc_kelidan_meteor_target_mythic(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        scheduler.CancelAll();
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == DATA_METEOR_START_SMASH)
        {
            scheduler.Schedule(2s, [&](TaskContext func)
            {
                DoCastSelf(SPELL_KELIDAN_COSMIC_SMASH, true);
                me->RemoveAllAuras();
                me->DespawnOrUnsummon(3s);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

private:
    TaskScheduler scheduler;
};

class spell_flames_kelidan_mythic : public SpellScript
{
    PrepareSpellScript(spell_flames_kelidan_mythic);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (!instance || !instance->IsMythic())
            return;

        float variance = float(instance->GetMythicKeyLevel()) * 0.05f;
        uint32 damageValue = Mythic::CalculateMythicValue(12500, variance);
        if (GetHitUnit() && GetHitUnit()->IsImmunedToDamage(GetSpellInfo()))
            SetEffectValue(0);
        else
            SetEffectValue(damageValue);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_flames_kelidan_mythic::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class spell_kelidan_conse_immunity : public AuraScript
{
    PrepareAuraScript(spell_kelidan_conse_immunity);

    void HandleApplyAura(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (!GetTarget())
            return;

        GetTarget()->ApplySpellImmune(SPELL_VORTEX, IMMUNITY_ID, SPELL_VORTEX, true);
        GetTarget()->ApplySpellImmune(SPELL_KELIDAN_FLAMES_DMG, IMMUNITY_ID, SPELL_KELIDAN_FLAMES_DMG, true);
        GetTarget()->ApplySpellImmune(SPELL_KELIDAN_COSMIC_SMASH, IMMUNITY_ID, SPELL_KELIDAN_COSMIC_SMASH, true);
        GetTarget()->ApplySpellImmune(SPELL_KELIDAN_COSMIC_SMASH_TRIG, IMMUNITY_ID, SPELL_KELIDAN_COSMIC_SMASH_TRIG, true);
    }

    void HandleRemoveAura(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (!GetTarget())
            return;

        GetTarget()->ApplySpellImmune(SPELL_KELIDAN_FLAMES_DMG, IMMUNITY_ID, SPELL_KELIDAN_FLAMES_DMG, false);
        GetTarget()->ApplySpellImmune(SPELL_VORTEX, IMMUNITY_ID, SPELL_VORTEX, false);
        GetTarget()->ApplySpellImmune(SPELL_KELIDAN_COSMIC_SMASH, IMMUNITY_ID, SPELL_KELIDAN_COSMIC_SMASH, false);
        GetTarget()->ApplySpellImmune(SPELL_KELIDAN_COSMIC_SMASH_TRIG, IMMUNITY_ID, SPELL_KELIDAN_COSMIC_SMASH_TRIG, false);
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (!GetCaster() || GetCaster()->isDead())
            amount = 0;
    }

    void Update(AuraEffect const* eff)
    {
        if (!eff)
            return;

        if (AuraEffect* effect = GetAura()->GetEffect(eff->GetEffIndex()))
            effect->RecalculateAmount();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_kelidan_conse_immunity::HandleApplyAura, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_kelidan_conse_immunity::HandleRemoveAura, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_kelidan_conse_immunity::Update, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_kelidan_conse_immunity::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

constexpr uint32 MAGTHERIDON_TELEPORTS_SIZE{ 5 };
Position const MagtheridonTeleportPositions[MAGTHERIDON_TELEPORTS_SIZE] =
{
    { 270.177f, -87.2306f, -137.451f, 0.00309944f },
    { 306.800f, -144.693f, -137.451f, 1.22518000f },
    { 371.915f, -121.385f, -137.451f, 2.35144000f },
    { 372.376f, -53.5092f, -137.451f, 3.70233000f },
    { 307.127f, -29.2736f, -137.451f, 4.88042000f }
};

enum MagtheridonBFSpells
{
    SPELL_MAGTHERIDON_CLEAVE = 15496,
    SPELL_MAGTHERIDON_BURNING_ABYSSMAL = 30511,
    NPC_MAGTHERIDON_METEOR_TARGET = 91552,
    NPC_MAGTHERIDON_ABYSSAL = 17454,
    SPELL_MAGTHERIDON_METEOR_STRIKE_AOE_DAMAGE = 74648,
    SPELL_MAGTHERIDON_METEOR_STRIKE_FIRE_AURA_1 = 74713,
    SPELL_MAGTHERIDON_FRENZY_MYTHIC = 47774
};

struct npc_magtheridon_furnace_mythic : public MythicAI
{
    npc_magtheridon_furnace_mythic(Creature* creature) : MythicAI(creature), summons(me)
    {
        Position homePos = me->GetHomePosition();
        homePos.m_positionZ = me->GetMap()->GetHeight(homePos.m_positionX, homePos.m_positionY, homePos.m_positionZ);
        me->SetHomePosition(homePos);
        me->Relocate(homePos);

        if (IsMythic())
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

            float size = me->GetFloatValue(OBJECT_FIELD_SCALE_X);
            me->SetObjectScale(size * 0.55f);
        }
    }

    void Reset() override
    {
        summons.DespawnAll();
        me->SetImmuneToAll(true);
        _preFight = false;
        if (IsMythic())
        {
            me->setActive(true);
            me->SetSelectable(true);
            me->SetImmuneToAll(true);
        }

        MythicAI::Reset();
        talkScheduler.Schedule(1min, [&](TaskContext func)
        {
            Talk(0);
            func.Repeat();
        });
    }

    void JustSummoned(Creature* creature) override
    {
        summons.Summon(creature);
        if (creature->GetEntry() == NPC_MAGTHERIDON_ABYSSAL)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 3500);
            creature->CastCustomSpell(SPELL_MAGTHERIDON_METEOR_STRIKE_AOE_DAMAGE, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            creature->AddAura(SPELL_MAGTHERIDON_METEOR_STRIKE_FIRE_AURA_1, creature);
            creature->SetPassive();
            creature->SetControlled(true, UNIT_STATE_ROOT);
            creature->SetSelectable(false);
        }
    }

    void SetGUID(uint64 guid, int32 val) override
    {
        if (!val)
        {
            Unit* target = ObjectAccessor::GetCreature(*me, guid);
            if (target)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 300);
                float variance = GetMythicKeyLevel() * 0.02f;
                val.AddSpellMod(SPELLVALUE_BASE_POINT2, Mythic::CalculateMythicValue(12000, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                me->CastCustomSpell(SPELL_MAGTHERIDON_BURNING_ABYSSMAL, val, target, TRIGGERED_FULL_MASK);
            }
        }
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        talkScheduler.CancelAll();

        task.Schedule(randseconds(5, 7), [&](TaskContext func)
        {
            DoCastSelf(SPELL_MAGTHERIDON_FRENZY_MYTHIC, true);
            func.Repeat(13s);
        });

        task.Schedule(randseconds(4, 6), [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 150.f, true);
            if (target)
            {
                Creature* trigger = me->SummonCreature(NPC_MAGTHERIDON_METEOR_TARGET, target->GetPosition());
                if (trigger)
                {
                    trigger->DespawnOrUnsummon(10s);
                    trigger->setFaction(target->getFaction());
                    trigger->SetPassive();
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3500);
                    trigger->CastCustomSpell(74641, val, trigger, TRIGGERED_FULL_MASK);
                }
            }

            func.Repeat(3500ms);
        });

        task.Schedule(randseconds(2, 3), [&](TaskContext func)
        {
            DoCastVictim(SPELL_MAGTHERIDON_CLEAVE);
            func.Repeat(randseconds(4, 6));
        });
    }

    void EnterEvadeMode() override
    {
        if (!_EnterEvadeMode())
            return;

        MythicAI::EnterEvadeMode();

        auto const& mapRef = me->GetMap()->GetPlayers();
        for (auto const& ref : mapRef)
        {
            Player* player = ref.GetSource();
            if (player)
            {
                if (player->IsGameMaster() || player->GetDistance(me) > 65.f)
                    continue;

                Position pos = me->GetNearPositionFromPos(CenterOfTheRoom, 10.f);
                player->NearTeleportTo(pos, true, false, true);
                if (player->IsAlive())
                    player->CastSpell(player, 7077, true);
            }
        }
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (!who)
            return false;

        return MythicAI::CanAIAttack(who) && who->GetPositionZ() < me->GetHomePosition().GetPositionZ() + 15.f;
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == DATA_SECRET_PHASE_ENABLE_MAGTHERIDON && value == DATA_SECRET_PHASE_ENABLE_MAGTHERIDON)
        {
            _preFight = true;

            task.Schedule(3s, [&](TaskContext /*func*/)
            {
                auto const& mapRef = me->GetMap()->GetPlayers();
                for (auto const& ref : mapRef)
                {
                    Player* player = ref.GetSource();
                    if (player)
                    {
                        if (player->IsGameMaster() || !player->IsAlive())
                            continue;

                        Position pos = me->GetNearPositionFromPos(me->GetHomePosition(), 10.f);
                        player->NearTeleportTo(pos, true, false, true);
                        player->CastSpell(player, 7077, true);
                    }
                }
            });

            task.Schedule(12s, [&](TaskContext func)
            {
                me->SetSelectable(true);
                me->SetImmuneToAll(false);
                DoZoneInCombat(me, 100.f);
                _preFight = false;
            });
        }
    }

    void JustDied(Unit* killer) override
    {
        auto const& mapRef = me->GetMap()->GetPlayers();
        for (auto const& ref : mapRef)
        {
            Player* player = ref.GetSource();
            if (player)
            {
                if (player->IsGameMaster() || !player->IsAlive())
                    continue;

                Position pos = me->GetNearPositionFromPos(CenterOfTheRoom, 10.f);
                player->CastSpell(player, 7077, true);
                player->NearTeleportTo(pos, true, false, true);
                if (Creature* kelidan = instance->GetCreature(DATA_KELIDAN))
                    if (kelidan->IsAIEnabled)
                        kelidan->AI()->SetData(DATA_SECRET_PHASE_ENABLE_MAGTHERIDON, DONE);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        talkScheduler.Update(diff);

        if (_preFight)
            task.Update(diff);

        if (!UpdateVictim())
            return;

        task.Update(diff);
        DoMeleeAttackIfReady();
    }
private:
    bool _preFight;
    TaskScheduler talkScheduler;
    SummonList summons;
};

struct npc_burning_abyssmal_mythic : public ScriptedAI
{
    npc_burning_abyssmal_mythic(Creature* creature) : ScriptedAI(creature)
    {
        InstanceScript* instance = me->GetInstanceScript();
        mythicCreature = instance ? instance->IsMythic() : false;
    }

    void AttackStart(Unit* who) override
    {
        if (mythicCreature)
            return;

        ScriptedAI::AttackStart(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (mythicCreature)
            return;

        ScriptedAI::UpdateAI(diff);
    }
private:
    bool mythicCreature;
};

struct npc_kelidan_mythic_elemental : public PassiveAI
{
    npc_kelidan_mythic_elemental(Creature* creature) : PassiveAI(creature) { }
};

void AddSC_boss_kelidan_the_breaker()
{
    RegisterCreatureAI(boss_kelidan_the_breaker);
    RegisterCreatureAI(npc_shadowmoon_channeler);
    RegisterCreatureAI(npc_kelidan_flames_initial);
    RegisterCreatureAI(npc_kelidan_flame_secondary);
    RegisterCreatureAI(npc_kelidan_meteor_target_mythic);
    RegisterCreatureAI(npc_magtheridon_furnace_mythic);
    RegisterCreatureAI(npc_burning_abyssmal_mythic);
    RegisterCreatureAI(npc_kelidan_mythic_elemental);

    RegisterAuraScript(spell_kelidan_burning_nova);
    RegisterAuraScript(spell_kelidan_fiery_marker);
    RegisterSpellScript(spell_rage_of_magtheridon_targets);
    RegisterSpellScript(spell_flames_kelidan_mythic);
    RegisterAuraScript(spell_kelidan_conse_immunity);
}

