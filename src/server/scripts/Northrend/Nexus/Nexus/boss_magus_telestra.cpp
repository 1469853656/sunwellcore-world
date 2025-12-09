
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "nexus.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "GridNotifiers.h"
#include "GameEventMgr.h"
#include "MoveSplineInit.h"

enum TelestraSpells
{
    // Main
    SPELL_ICE_NOVA                      = 47772,
    SPELL_FIREBOMB                      = 47773,

    SPELL_GRAVITY_WELL                  = 47756,
    SPELL_TELESTRA_BACK                 = 47714,
    SPELL_BURNING_WINDS                 = 46308,
    SPELL_START_SUMMON_CLONES           = 47710,

    SPELL_FIRE_MAGUS_SUMMON             = 47707,
    SPELL_FROST_MAGUS_SUMMON            = 47709,
    SPELL_ARCANE_MAGUS_SUMMON           = 47708,

    SPELL_FIRE_MAGUS_DEATH              = 47711,
    SPELL_ARCANE_MAGUS_DEATH            = 47713,

    SPELL_WEAR_CHRISTMAS_HAT            = 61400
};

enum TelestraYells
{
    SAY_AGGRO                        = 0,
    SAY_KILL,
    SAY_DEATH,
    SAY_MERGE,
    SAY_SPLIT
};

enum TelestraMisc
{
    NPC_FIRE_MAGUS                      = 26928,
    NPC_FROST_MAGUS                     = 26930,
    NPC_ARCANE_MAGUS                    = 26929,

    ACHIEVEMENT_SPLIT_PERSONALITY       = 2150,

    GAME_EVENT_WINTER_VEIL              = 2
};

enum Events
{
    EVENT_MAGUS_ICE_NOVA            = 1,
    EVENT_MAGUS_FIREBOMB,
    EVENT_MAGUS_GRAVITY_WELL,
    EVENT_MAGUS_HEALTH1,
    EVENT_MAGUS_HEALTH2,
    EVENT_MAGUS_FAIL_ACHIEVEMENT,
    EVENT_MAGUS_MERGED,
    EVENT_MAGUS_RELOCATE,
    EVENT_KILL_TALK
};

enum SchedulerGroups
{
    SCHEDULER_MAGUS_TIME_STOP = 1,
    SCHEDULER_MAGUS_NORMAL = 2
};

enum MagusMythicMisc
{
    SPELL_MAGUS_MYTHIC_PYROBLAST = 38535,
    SPELL_MAGUS_MYTHIC_FIRE_SHIELD = 38933,
    SPELL_MAGUS_MYTHIC_IGNITE = 52210,
    SPELL_MAGUS_MYTHIC_INCIENERATE_FLESH = 66237,
    SPELL_MAGUS_MYTHIC_FREEZING_CLOUD = 60020,
    SPELL_MAGUS_MYTHIC_SLOW = 32921,
    SPELL_MAGUS_MYTHIC_FROSTFIRE_BOLT = 69869,

    NPC_GRAND_MAGUS_FIREBALL_ORB = 91526,
    SPELL_FLAMEPSHERE_VISUAL_MYTHIC = 55928
};

enum OrbData
{
    ORB_POINT_TO_CENTER = 1000,
    ORB_POINT_TO_OUTER  = 1001,
    ORB_SPAWN_NEW_ORB   = 1002
};

Position const CenterOfTelestraRoom{ 505.211f, 88.8473f, -16.1246f, 2.54051f };
struct boss_magus_telestra : public BossAI
{
    boss_magus_telestra(Creature* creature)
        : BossAI(creature, DATA_MAGUS_TELESTRA_EVENT)
        , _gateGUID(0)
    {
        if (instance && instance->IsMythic())
        {
            Position const newHomePos{ 486.386536f, 88.885620f, -16.124645f, 0.116366f };
            me->Relocate(newHomePos);
            me->SetHomePosition(newHomePos);

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
        }
    }

    void Reset() override
    {
        BossAI::Reset();
        copiesDied = 0;
        achievement = true;

        if (GameObject* gate = ObjectAccessor::GetGameObject(*me, _gateGUID))
            gate->RemoveFromWorld();

        _gateGUID = 0;
        if (instance->IsMythic())
            me->SetCanMissSpells(false);

        if ((IsHeroic() || instance->IsMythic()) && sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL) && !me->HasAura(SPELL_WEAR_CHRISTMAS_HAT))
            me->AddAura(SPELL_WEAR_CHRISTMAS_HAT, me);
    }

    uint32 GetData(uint32 data) const override
    {
        if (data == me->GetEntry())
            return achievement;
        return 0;
    }

    void SetGUID(uint64 guid, int32 data) override
    {
        if (data == ORB_SPAWN_NEW_ORB)
        {
            Creature* orb = ObjectAccessor::GetCreature(*me, guid);
            if (orb)
            {
                for (uint32 i = 0; i < 4; ++i)
                {
                    Position pos = me->GetNearPositionFromPos(orb->GetPosition(), 4.f);
                    Creature* newOrb = me->SummonCreature(NPC_GRAND_MAGUS_FIREBALL_ORB, pos);
                    if (newOrb)
                    {
                        newOrb->SetPassive();
                        newOrb->SetSelectable(false);
                        newOrb->CastSpell(orb, SPELL_FLAMEPSHERE_VISUAL_MYTHIC, true);
                        newOrb->GetMotionMaster()->MovePoint(ORB_POINT_TO_CENTER, CenterOfTelestraRoom);
                    }
                }
            }
        }
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        return me->GetDistance(CenterOfTelestraRoom) > 35.f;
    }

    void EnterCombat(Unit* who) override
    {
        BossAI::EnterCombat(who);
        Talk(SAY_AGGRO);

        if (!instance->IsMythic())
        {
            events.ScheduleEvent(EVENT_MAGUS_ICE_NOVA, 10s);
            events.ScheduleEvent(EVENT_MAGUS_FIREBOMB, 0);
            events.ScheduleEvent(EVENT_MAGUS_GRAVITY_WELL, 20s);
            events.ScheduleEvent(EVENT_MAGUS_HEALTH1, 1s);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_MAGUS_HEALTH2, 1s);
        }
        else
        {
            Position gateSpawnPos{ 534.262f, 88.4465f, -16.1246f, 3.18205f };
            GameObject* gate = me->SummonGameObject(180322, gateSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
            if (gate)
                _gateGUID = gate->GetGUID();
            ScheduleMythic();
        }

        _splitPctStates.fill(false);
    }

    void JustSummoned(Creature* summon) override
    {
        if (summons.size() > 50)
        {
            summon->DespawnOrUnsummon();
            return;
        }

        summons.Summon(summon);
    }

    void ScheduleMythic()
    {
        scheduler.Schedule(1s, [&](TaskContext func)
        {
            if (!me->HasAura(SPELL_MAGUS_MYTHIC_FIRE_SHIELD))
            {
                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                values.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
                me->CastCustomSpell(SPELL_MAGUS_MYTHIC_FIRE_SHIELD, values, me, TRIGGERED_FULL_MASK);
            }

            func.Repeat(8s);
        });

        scheduler.Schedule(3s, [&](TaskContext func)
        {
            uint32 count = !func.GetRepeatCounter() ? 4 : 1;
            for (uint32 i = 0; i < count; ++i)
            {
                Position pos = me->GetNearPositionFromPos(me->GetPosition(), 4.f);
                Creature* orb = me->SummonCreature(NPC_GRAND_MAGUS_FIREBALL_ORB, pos);
                if (orb)
                {
                    orb->SetPassive();
                    orb->SetSelectable(false);
                    orb->CastSpell(orb, SPELL_FLAMEPSHERE_VISUAL_MYTHIC, true);
                    orb->GetMotionMaster()->MovePoint(ORB_POINT_TO_CENTER, CenterOfTelestraRoom);
                }
            }

            func.Repeat(11s);
        });

        scheduler.Schedule(randseconds(5, 10), [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 100.f, true);
            if (target)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 300);
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 2500);
                target->CastCustomSpell(SPELL_MAGUS_MYTHIC_SLOW, val, target, TRIGGERED_FULL_MASK);

                CustomSpellValues val2;
                val2.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 750);
                int32 damage = 4500 + Mythic::CalculateMythicValue(4500,
                    static_cast<float>(0.06f * instance->GetMythicKeyLevel()));

                val2.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
                val2.AddSpellMod(SPELLVALUE_BASE_POINT2, 2500);
                me->CastCustomSpell(SPELL_MAGUS_MYTHIC_FROSTFIRE_BOLT, val2, target);
            }

            func.Repeat(randseconds(8, 12));
        });

        scheduler.Schedule(100ms, SCHEDULER_MAGUS_NORMAL, [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            if (target)
            {
                if ((func.GetRepeatCounter() % 3) == 0)
                {
                    Unit* farTarget = SelectTarget(SELECT_TARGET_RANDOM, 1U, 40.f, true);
                    if (farTarget)
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FIREBOMB, data));
                        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        me->CastCustomSpell(SPELL_FIREBOMB, val, farTarget, TRIGGERED_FULL_MASK);
                    }
                }

                Mythic::MythicSpells data2;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MAGUS_MYTHIC_PYROBLAST, data2));
                float variance2 = float(instance->GetMythicKeyLevel()) * data2.SpellValueVariance;

                CustomSpellValues val2;
                val2.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                val2.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data2.SpellEffectValue0, variance2));
                val2.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data2.SpellEffectValue1, variance2));
                val2.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1100);
                me->CastCustomSpell(SPELL_MAGUS_MYTHIC_PYROBLAST, val2, target);
            }

            func.Repeat(randseconds(3, 4));
        });
    }

    void TriggerPersonalitySplit()
    {
        me->CastSpell(me, SPELL_START_SUMMON_CLONES, false);
        events.ScheduleEvent(EVENT_MAGUS_RELOCATE, 3500ms);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!instance->IsMythic())
        {
            if (_splitPctStates[0] == false && me->HealthBelowPctDamaged(50, damage))
            {
                _splitPctStates[0] = true;
                TriggerPersonalitySplit();
            }

            if (IsHeroic() && _splitPctStates[1] == false && me->HealthBelowPctDamaged(10, damage))
            {
                _splitPctStates[1] = true;
                TriggerPersonalitySplit();
            }
        }
    }

    void AttackStart(Unit* who) override
    {
        AttackStartCaster(who, 20.f);
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        summons.DespawnAll();
        Talk(SAY_DEATH);

        if (GameObject* go = ObjectAccessor::GetGameObject(*me, _gateGUID))
            go->RemoveFromWorld();

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 504.808807f, 89.122765f, -16.124521f, 6.236450f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        if (events.GetNextEventTime(EVENT_KILL_TALK) == 0)
        {
            Talk(SAY_KILL);
            events.ScheduleEvent(EVENT_KILL_TALK, 6s);
        }
    }

    void SpellHit(Unit* caster, const SpellInfo* spellInfo) override
    {
        if (spellInfo->Id >= SPELL_FIRE_MAGUS_DEATH && spellInfo->Id <= SPELL_ARCANE_MAGUS_DEATH && caster->ToCreature())
        {
            events.ScheduleEvent(EVENT_MAGUS_FAIL_ACHIEVEMENT, 5s);
            caster->ToCreature()->DespawnOrUnsummon(1000);

            if (++copiesDied >= 3)
            {
                copiesDied = 0;
                Talk(SAY_MERGE);
                events.CancelEvent(EVENT_MAGUS_FAIL_ACHIEVEMENT);
                events.ScheduleEvent(EVENT_MAGUS_MERGED, 5s);
                me->CastSpell(me, SPELL_BURNING_WINDS, true);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);
        events.Update(diff);
        if (me->IsCasting())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MAGUS_FIREBOMB:
                    if (instance->IsMythic())
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FIREBOMB, data));
                        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastCustomSpell(SPELL_FIREBOMB, val, target);
                    }
                    else
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            me->CastSpell(target, SPELL_FIREBOMB, false);
                    }
                    events.ScheduleEvent(EVENT_MAGUS_FIREBOMB, 3000);
                    break;
                case EVENT_MAGUS_ICE_NOVA:
                    if (instance->IsMythic())
                    {
                        Mythic::MythicSpells data;
                        ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ICE_NOVA, data));
                        float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                        me->CastCustomSpell(SPELL_ICE_NOVA, val, (Unit*)nullptr);
                    }
                    else
                        me->CastSpell(me, SPELL_ICE_NOVA, false);
                    events.ScheduleEvent(EVENT_MAGUS_ICE_NOVA, 15000);
                    break;
                case EVENT_MAGUS_GRAVITY_WELL:
                    me->CastSpell(me, SPELL_GRAVITY_WELL, false);
                    events.ScheduleEvent(EVENT_MAGUS_GRAVITY_WELL, 15000);
                    break;
                case EVENT_MAGUS_FAIL_ACHIEVEMENT:
                    achievement = false;
                    break;
                case EVENT_MAGUS_RELOCATE:
                    me->NearTeleportTo({ 505.04f, 88.915f, -16.13f, 2.98f });
                    break;
                case EVENT_MAGUS_MERGED:
                    me->CastSpell(me, SPELL_TELESTRA_BACK, true);
                    me->RemoveAllAuras();
                    break;
            }

            if (me->IsCasting())
                return;
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea();
    }
    private:
        uint8 copiesDied;
        bool achievement;
        std::array<bool, 2> _splitPctStates;
        uint64 _gateGUID;
};

class spell_boss_magus_telestra_summon_telestra_clones : public AuraScript
{
    PrepareAuraScript(spell_boss_magus_telestra_summon_telestra_clones);

    bool Load() override
    {
        return GetUnitOwner()->GetTypeId() == TYPEID_UNIT;
    }

    void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetUnitOwner()->ToCreature()->AI()->Talk(SAY_SPLIT);
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_FIRE_MAGUS_SUMMON, true);
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_FROST_MAGUS_SUMMON, true);
        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_ARCANE_MAGUS_SUMMON, true);

        GetUnitOwner()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        GetUnitOwner()->SetControlled(true, UNIT_STATE_STUNNED);
        GetUnitOwner()->ToCreature()->LoadEquipment(0, true);

    }

    void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetUnitOwner()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        GetUnitOwner()->SetControlled(false, UNIT_STATE_STUNNED);
        GetUnitOwner()->ToCreature()->LoadEquipment(1, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_boss_magus_telestra_summon_telestra_clones::HandleApply, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_boss_magus_telestra_summon_telestra_clones::HandleRemove, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_boss_magus_telestra_gravity_well : public SpellScript
{
    PrepareSpellScript(spell_boss_magus_telestra_gravity_well);

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::RandomCheck(50));
    }

    void HandlePull(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        Unit* target = GetHitUnit();
        if (!target)
            return;

        Position pos;
        if (target->GetDistance(GetCaster()) < 5.0f)
        {
            pos.Relocate(GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ() + 1.0f);
            float o = frand(0, 2 * M_PI);
            target->MovePositionToFirstCollision(pos, 20.0f, frand(0, 2 * M_PI));
            pos.m_positionZ += frand(5.0f, 15.0f);
        }
        else
            pos.Relocate(GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ() + 1.0f);

        float speedXY = float(GetSpellInfo()->Effects[effIndex].MiscValue) * 0.1f;
        float speedZ = target->GetDistance(pos) / speedXY * 0.5f * Movement::gravity;

        target->GetMotionMaster()->MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), speedXY, speedZ);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_boss_magus_telestra_gravity_well::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_boss_magus_telestra_gravity_well::HandlePull, EFFECT_0, SPELL_EFFECT_PULL_TOWARDS_DEST);
    }
};

class achievement_split_personality : public AchievementCriteriaScript
{
    public:
        achievement_split_personality() : AchievementCriteriaScript("achievement_split_personality") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            if (!target)
                return false;

            return target->GetAI()->GetData(target->GetEntry());
        }
};

class spell_magus_mythic_polymorph_explosion : public AuraScript
{
    PrepareAuraScript(spell_magus_mythic_polymorph_explosion);

    void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        InstanceScript* instance = GetTarget()->GetInstanceScript();
        if (GetTarget() && instance && instance->IsMythic() && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            GetTarget()->CastSpell(GetTarget(), 64864, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_magus_mythic_polymorph_explosion::HandleRemove, EFFECT_0,
            SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

constexpr uint32 SPELL_TELESTRA_FIRE_ORB_EXPLOSION{ 52146 };
struct npc_telestra_fire_orb_mythic : public PassiveAI
{
    npc_telestra_fire_orb_mythic(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        task.Schedule(1500ms, [&](TaskContext func)
        {
            //! it includes object bounds
            if (Player* player = me->SelectNearestPlayer(0.2f))
            {
                std::string _hitString = "Fire orb hits " + player->GetName() + " and splits into four!";
                me->MonsterTextEmote(_hitString.c_str(), nullptr, false);
                if (Creature* telestra = me->GetInstanceScript()->GetCreature(DATA_MAGUS_TELESTRA_EVENT))
                    if (telestra->AI())
                        telestra->AI()->SetGUID(me->GetGUID(), ORB_SPAWN_NEW_ORB);

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 10000);
                val.AddSpellMod(SPELLVALUE_TARGET_PLAYERS_ONLY, 1);
                me->CastCustomSpell(SPELL_TELESTRA_FIRE_ORB_EXPLOSION, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);

                func.Repeat(2s);
            }
            else
                func.Repeat(500ms);
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (pointId == ORB_POINT_TO_OUTER)
            {
                task.Schedule(200ms, [&](TaskContext /*func*/)
                {
                    float const _baseOuter = 1.32f;
                    float const _addition = 0.11f * (me->GetInstanceScript() ?
                        static_cast<float>(me->GetInstanceScript()->GetMythicKeyLevel()) : 0.f);
                    me->SetSpeedRate(MOVE_RUN, _baseOuter + _addition);
                    me->GetMotionMaster()->MovePoint(ORB_POINT_TO_CENTER, CenterOfTelestraRoom);
                });
            }
            else if (pointId == ORB_POINT_TO_CENTER)
            {
                task.Schedule(200ms, [&](TaskContext /*func*/)
                {
                    float const _baseInner = 1.10f;
                    float const _addition = 0.11f * (me->GetInstanceScript() ?
                        static_cast<float>(me->GetInstanceScript()->GetMythicKeyLevel()) : 0.f);
                    me->SetSpeedRate(MOVE_RUN, _baseInner + _addition);
                    float const _radius = 27.5f;
                    float const _fullCircle = static_cast<float>(2.f * M_PI);
                    float const _myAngle = frand(0.f, _fullCircle);
                    Position movePos = CenterOfTelestraRoom;
                    movePos.m_positionX = CenterOfTelestraRoom.GetPositionX() + _radius * std::cos(_myAngle);
                    movePos.m_positionY = CenterOfTelestraRoom.GetPositionY() + _radius * std::sin(_myAngle);
                    me->GetMotionMaster()->MovePoint(ORB_POINT_TO_OUTER, movePos);
                });
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

private:
    TaskScheduler task;
};

/** Adds code in nexus.cpp file **/
void AddSC_boss_magus_telestra()
{
    RegisterCreatureAI(boss_magus_telestra);
    RegisterAuraScript(spell_boss_magus_telestra_summon_telestra_clones);
    RegisterSpellScript(spell_boss_magus_telestra_gravity_well);
    RegisterCreatureAI(npc_telestra_fire_orb_mythic);

    new achievement_split_personality();
}
