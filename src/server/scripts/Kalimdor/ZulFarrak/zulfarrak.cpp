#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "MythicAI.h"
#include "GameObjectAI.h"
#include "zulfarrak.h"
#include "MoveSplineInit.h"
#include "Vehicle.h"

/** Mythic stuff **/
//! screw aligning, we will eventually align it automatically.. somehow
enum ThekaMythicMisc
{
    NPC_SCARAB_ZULFARRAK = 7269,
    SPELL_THEKA_FEVERED_PLAGUE = 8600,
    SPELL_THEKA_TRANSFORM = 11089,
    SPELL_LEECHING_SWARM_THEKA = 53467,
    SPELL_THEKA_POISON_SHOCK_MYTHIC = 28741,

    /** Mythic stuff **/
    NPC_THEKA_ANUB_TRIGGER_MYTHIC = 91502,
    NPC_THEKA_POISON_TRIGGER_MYTHIC = 91503,
    ACTION_THEKA_MOVE_SCARAB = 100,
    DATA_THEKA_SCARAB_DONE  = 101,
    DATA_THEKA_SCARAB_DEATH = 102
};

struct ZulfarrakAI : public MythicAI
{
    ZulfarrakAI(Creature* creature) : MythicAI(creature)
    {
        IsPartOfStairsEvent = false;
        task.ClearValidator();
        task.SetValidator([this]()
        {
            return !me->IsCasting() && !me->IsUnderCrowdControl();
        });
    }

    void TagBlyGroup()
    {
        for (uint32 entry : { NPC_SERGEANT_BLY, NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
        {
            uint64 const guid = instance->GetData64(entry);
            Creature* creature = ObjectAccessor::GetCreature(*me, guid);
            if (creature && me->GetDistance(creature) < 25.f)
            {
                creature->AddThreat(me, 0.1f);
                me->AddThreat(creature, 0.1f);
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_MARK_AS_STAIRS_ADD)
            IsPartOfStairsEvent = true;
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        if (IsPartOfStairsEvent)
            TagBlyGroup();
    }

public:
    bool IsPartOfStairsEvent;
};

constexpr uint32 SPELL_SANDFURY_BLOOD_LEECH{ 11898 };
struct npc_sandfury_blood_drinker : public ZulfarrakAI
{
    npc_sandfury_blood_drinker(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(3000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(5000, 10000));
            if (!IsMythic())
            {
                DoCastAOE(SPELL_SANDFURY_BLOOD_LEECH, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_BLOOD_LEECH, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                me->CastCustomSpell(SPELL_SANDFURY_BLOOD_LEECH, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(urand(10000, 14000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

enum SandfuryShadowcaster
{
    SPELL_SHADOWFURY_SC_SW_PAIN = 14032,
    SPELL_SANDFURY_SC_SHADOW_BOLT = 12471,
    SPELL_SANDFURY_DISMOUNTING_BLAST = 18396,
    SPELL_SANDFURY_DEMON_SKIN = 20798
};

struct npc_sandfury_shadowcaster : public ZulfarrakAI
{
    npc_sandfury_shadowcaster(Creature* creature) : ZulfarrakAI(creature) { }

    void Reset() override
    {
        MythicAI::Reset();

        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(120000);
            if (!IsMythic())
            {
                DoCastSelf(SPELL_SANDFURY_DEMON_SKIN, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_DEMON_SKIN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                me->CastCustomSpell(SPELL_SANDFURY_DEMON_SKIN, val, me);
                repeatTime = Milliseconds(1200000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(urand(4000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(13000, 24000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_SHADOWFURY_SC_SW_PAIN))
                    DoCast(target, SPELL_SHADOWFURY_SC_SW_PAIN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SHADOWFURY_SC_SW_PAIN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_SHADOWFURY_SC_SW_PAIN))
                    me->CastCustomSpell(SPELL_SHADOWFURY_SC_SW_PAIN, val, target);
                repeatTime = Milliseconds(13000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 3000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3000, 4000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_SC_SHADOW_BOLT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_SC_SHADOW_BOLT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1750);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_SC_SHADOW_BOLT, val, target);
                repeatTime = 2s;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(0), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(0);
            if (!IsMythic())
            {
                if (Unit* target = me->GetVictim())
                    if (target->IsMounted())
                        DoCastVictim(SPELL_SANDFURY_DISMOUNTING_BLAST, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_DISMOUNTING_BLAST, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);

                repeatTime = 0ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_SANDFURY_FLASH_HEAL{ 17843 };
constexpr uint32 SPELL_SANDFURY_LAVA_SPOUT_TOTEM{ 8264 };
constexpr uint32 SPELL_SANDFURY_HEALING_TOTEM{ 11899 };
//! ToDo: issue with totems, one replaces the other
struct npc_sandfury_witchdoctor : public ZulfarrakAI
{
    npc_sandfury_witchdoctor(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(1500), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 11000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 40, true))
                    DoCast(target, SPELL_SANDFURY_FLASH_HEAL);
                else
                    repeatTime = 1500ms;
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_FLASH_HEAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                Unit* target = DoSelectLowestHpFriendly(40.f, 10, true);
                if (target)
                    me->CastCustomSpell(SPELL_SANDFURY_FLASH_HEAL, val, target);

                repeatTime = target ? Milliseconds(6500) : 1500ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
        task.Schedule(Milliseconds(15000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(30000, 30000));
            if (!IsMythic())
            {
                DoCastSelf(SPELL_SANDFURY_LAVA_SPOUT_TOTEM, true);
            }
            else
            {
                DoCastSelf(SPELL_SANDFURY_LAVA_SPOUT_TOTEM, true);
                repeatTime = Milliseconds(25000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 2000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(30000);
            if (!IsMythic())
            {
                DoCastSelf(SPELL_SANDFURY_HEALING_TOTEM, true);
            }
            else
            {
                DoCastSelf(SPELL_SANDFURY_HEALING_TOTEM, true);
                repeatTime = Milliseconds(urand(30000, 40000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_ZULFARRAK_TOTEM_HEALING_WARD{ 11901 };
constexpr uint32 SPELL_ZULFARRAK_LAVA_TOTEM{ 8266 };
constexpr uint32 NPC_ZULFARRAK_LAVA_SPOUT_TOTEM{ 6017 };
struct npc_sandfury_zulfarak_totem : public ZulfarrakAI
{
    npc_sandfury_zulfarak_totem(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        me->RemoveAllAuras();
        if (me->GetEntry() == NPC_ZULFARRAK_LAVA_SPOUT_TOTEM)
            DoCastSelf(SPELL_ZULFARRAK_LAVA_TOTEM);
        else
            DoCastSelf(SPELL_ZULFARRAK_TOTEM_HEALING_WARD);
    }

    void Reset() override
    {
        MythicDoZoneInCombat(me, 75.f);
    }

    void MoveInLineOfSight(Unit* /*who*/) override { }
    void AttackStart(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

class spell_zulfarrak_totem_mythic_scaling : public AuraScript
{
    PrepareAuraScript(spell_zulfarrak_totem_mythic_scaling);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        InstanceScript* instance = caster->GetInstanceScript();
        if (instance && instance->IsMythic())
        {
            PreventDefaultAction();
            uint32 triggeredSpellId = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(triggeredSpellId, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 20000.f);
            if (triggeredSpellId == 11900)
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 100000.f);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            caster->CastCustomSpell(triggeredSpellId, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_zulfarrak_totem_mythic_scaling::HandlePeriodic, EFFECT_0,
            SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};


constexpr uint32 SPELL_SULLITHUZ_PETRIFY{ 11020 };
struct npc_sullithuz_sandcrawler : public ZulfarrakAI
{
    npc_sullithuz_sandcrawler(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(5000, 9000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(15000, 22000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SULLITHUZ_PETRIFY, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SULLITHUZ_PETRIFY, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SULLITHUZ_PETRIFY, val, target, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(20000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_SCARAB_LORD_PLAGUE_CLOUD{ 3256 };
constexpr uint32 SPELL_SCARAB_LORD_MYTHIC_GROW{ 16711 };
struct npc_scarab_lord_zulfarrak : public ZulfarrakAI
{
    npc_scarab_lord_zulfarrak(Creature* creature) : ZulfarrakAI(creature) { }

    void Reset() override
    {
        ZulfarrakAI::Reset();
        _startUpdatingScheduler = false;
        _canAttack = true;
    }

    void SetGUID(uint64 guid, int32 data) override
    {
        if (data == ACTION_THEKA_MOVE_SCARAB)
        {
            Creature* anub = ObjectAccessor::GetCreature(*me, guid);
            if (!anub)
                return;

            _canAttack = false;
            me->GetMotionMaster()->Clear();
            me->DisableChainPullFor(1800000ms);
            Movement::MoveSplineInit init(me);
            Position const leftSide{ 1816.77f, 891.724f, 8.87809f, 4.14601f };

            float const _angle = me->GetAngle(anub->GetPositionX(), anub->GetPositionY());
            bool _clockwise = _angle < 5.7 && _angle > 2.67f ? false : true;
            FillRoundPathToAnub({ 1803.01f, 870.174f, 9.89885f, 4.09666f }, 20.f, me->GetPositionZ(), init.Path(), _clockwise);

            init.SetWalk(false);
            init.SetSmooth();
            auto scheduleMs = Milliseconds(init.Launch());
            _startUpdatingScheduler = true;
            task.Schedule(scheduleMs, [&](TaskContext func)
            {
                _canAttack = true;
                me->SetImmuneToAll(false);
                me->SetAggressive();
                me->SetSelectable(true);
                MythicDoZoneInCombat(me, 100.f);
                ScheduleCombatTasks();
                if (Creature* theka = instance->GetCreature(BOSS_THEKA_MARTYR))
                    theka->AI()->SetData(DATA_THEKA_SCARAB_DONE, DATA_THEKA_SCARAB_DONE);
            });
        }
    }

    void FillRoundPathToAnub(Position const& centerPos, float radius, float z, Movement::PointsArray& path, bool clockwise)
    {
        float step = clockwise ? -M_PI / 8.0f : M_PI / 8.0f;
        float angle = centerPos.GetAngle(me->GetPositionX(), me->GetPositionY());

        Position const EndPosition{ 1789.35f, 881.139f, 8.87716f, 5.82128f };
        for (; angle < 2 * M_PI; angle += step)
        {
            G3D::Vector3 point;
            point.x = centerPos.GetPositionX() + radius * cosf(angle);
            point.y = centerPos.GetPositionY() + radius * sinf(angle);
            point.z = z;
            path.push_back(point);

            //! we're very close to our final point
            //! stop constructing path, we will add last points manually
            Position const curPos{ point.x, point.y, point.z };
            if (curPos.GetExactDist2d(EndPosition.m_positionX, EndPosition.m_positionY) < 8.f)
                break;
        }

        Position const finalPoint = me->GetNearPositionFromPos(EndPosition, 3.f);
        path.push_back(G3D::Vector3(finalPoint.GetPositionX(), finalPoint.GetPositionY(), finalPoint.GetPositionZ()));
        path.push_back(G3D::Vector3(1800.690f, 875.188f, 8.87885f));
    }

    void ScheduleCombatTasks()
    {
        task.Schedule(Milliseconds(urand(1000, 11000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(15000, 22000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SCARAB_LORD_PLAGUE_CLOUD, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SCARAB_LORD_PLAGUE_CLOUD, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT2, Mythic::CalculateMythicValue(data.SpellEffectValue2, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
                {
                    return obj->IsPlayer() && obj->IsWithinDist(me, 30.f);
                }))
                    me->CastCustomSpell(SPELL_SCARAB_LORD_PLAGUE_CLOUD, val, target, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (IsMythic())
        {
            task.Schedule(1250ms, [&](TaskContext /*func*/)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 100);
                me->CastCustomSpell(SPELL_SCARAB_LORD_MYTHIC_GROW, val, me, TRIGGERED_FULL_MASK);
            });
        }
    }

    void AttackStart(Unit* who) override
    {
        if (!_canAttack)
            return;

        MythicAI::AttackStart(who);
    }

    void JustDied(Unit* killer) override
    {
        ZulfarrakAI::JustDied(killer);
        if (IsMythic())
        {
            Creature* theka = instance->GetCreature(BOSS_THEKA_MARTYR);
            CreatureAI* ai = theka ? theka->AI() : nullptr;

            if (theka && ai)
                theka->AI()->SetData(DATA_THEKA_SCARAB_DEATH, DATA_THEKA_SCARAB_DEATH);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        bool const _updateSuccess = UpdateVictim();
        if (_updateSuccess || _startUpdatingScheduler)
            task.Update(diff);

        if (!_updateSuccess)
            return;

        DoMeleeAttackIfReady();
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        if (!IsMythic())
            ScheduleCombatTasks();
    }
private:
    bool _canAttack;
    bool _startUpdatingScheduler;
};

constexpr uint32 SPELL_SANDFURY_DARK_OFFERING{ 7154 };
constexpr uint32 SPELL_SANDFURY_SOUL_BITE{ 11016 };
struct npc_sandfury_souleater : public ZulfarrakAI
{
    npc_sandfury_souleater(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(urand(8000, 11000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 11000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 20, true))
                    DoCast(target, SPELL_SANDFURY_DARK_OFFERING);
                else
                    repeatTime = 1500ms;
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_DARK_OFFERING, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                Unit* target = DoSelectLowestHpFriendly(40.f, 10, true);
                if (target)
                    me->CastCustomSpell(SPELL_SANDFURY_DARK_OFFERING, val, target);

                repeatTime = target ? Milliseconds(urand(6000, 8000)) : 1500ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 7000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(7000, 11000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                    DoCast(target, SPELL_SANDFURY_SOUL_BITE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_SOUL_BITE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
                {
                    return obj && obj->getPowerType() == POWER_MANA;
                });

                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM);

                if (target)
                    me->CastCustomSpell(SPELL_SANDFURY_SOUL_BITE, val, target);
                repeatTime = Milliseconds(7500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_NAME_SHADOWHUNTER_HEX{ 11641 };
constexpr uint32 SPELL_SANDFURY_SHADOWHUNTER_SHOOT{ 15547 };
struct npc_sandfury_shadowhunter : public ZulfarrakAI
{
    npc_sandfury_shadowhunter(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(urand(4000, 10000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(13000, 24000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 20.f, true))
                    DoCast(target, SPELL_NAME_SHADOWHUNTER_HEX);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_NAME_SHADOWHUNTER_HEX, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0U))
                    me->CastCustomSpell(SPELL_NAME_SHADOWHUNTER_HEX, val, target);
                repeatTime = Milliseconds(14000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(1), [&](TaskContext func)
        {
            if (!IsMythic())
            {
                //! ToDo: filter targets that are not mounted
                DoCastAOE(SPELL_SANDFURY_DISMOUNTING_BLAST, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_DISMOUNTING_BLAST, data));
                CustomSpellValues val;
                me->CastCustomSpell(SPELL_SANDFURY_DISMOUNTING_BLAST, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            }
        });

        task.Schedule(Milliseconds(2000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(2000);
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_SHADOWHUNTER_SHOOT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_SHADOWHUNTER_SHOOT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_SHADOWHUNTER_SHOOT, val, target);
                repeatTime = Milliseconds(2000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_SERVANT_ANTUSUL_PETRIFY{ 11020 };
struct npc_servant_of_antsul : public ZulfarrakAI
{
    npc_servant_of_antsul(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(5000, 7500)), [&](TaskContext func)
        {
            auto repeatTime = randseconds(15, 22);
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SERVANT_ANTUSUL_PETRIFY, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SERVANT_ANTUSUL_PETRIFY, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = me->GetVictim())
                {
                    if (target->IsWithinMeleeRange(me))
                    {
                        me->CastCustomSpell(SPELL_SERVANT_ANTUSUL_PETRIFY, val, target);
                        repeatTime = 15s;
                    }
                    else
                        repeatTime = 7s;
                }
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_ANTUSUL_EARTHGRAB_TRIGGERED{ 8377 };
constexpr uint32 SPELL_ANTUSUL_EARTHGRAB_TRIGGERED_MYTHIC{ 20699 };
struct npc_antusul_earthgrab : public PassiveAI
{
    npc_antusul_earthgrab(Creature* creature) : PassiveAI(creature)
    {
        me->SetCanMissSpells(false);
        instance = me->GetInstanceScript();
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void UpdateAI(uint32 diff) override { scheduler.Update(diff); }

    void EnterCombat(Unit* who) override
    {
        scheduler.Schedule(randseconds(4, 6), [&](TaskContext func)
        {
            auto repeatTime = 15s;
            bool const IsMythic = instance ? instance->GetMythicKeyLevel() != 0 : false;
            if (!IsMythic)
            {
                DoCastAOE(SPELL_ANTUSUL_EARTHGRAB_TRIGGERED, true);
            }
            else
            {
                float variance = float(instance->GetMythicKeyLevel()) * 0.06f;
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 6100);
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, 5);
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 20000);
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(3500, variance));
                me->CastCustomSpell(SPELL_ANTUSUL_EARTHGRAB_TRIGGERED_MYTHIC, val, (Unit*)nullptr);
                repeatTime = randseconds(10, 14);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
private:
    TaskScheduler scheduler;
    InstanceScript* instance;
};

constexpr uint32 SPELL_WARD_OF_ZUMRAH_PASSIVE{ 11087 };
constexpr uint32 NPC_SKELETON_OF_ZUMRAH{ 7786 };
struct npc_zumrah_ward : public ZulfarrakAI
{
    npc_zumrah_ward(Creature* creature) : ZulfarrakAI(creature)
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->IsMythic())
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, DEFAULT_COMBAT_REACH);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void UpdateAI(uint32 diff) override { task.Update(diff); }

    void Reset() override
    {
        MythicAI::Reset();
        if (!IsMythic())
        {
            me->RemoveAurasDueToSpell(SPELL_WARD_OF_ZUMRAH_PASSIVE);
            DoCastSelf(SPELL_WARD_OF_ZUMRAH_PASSIVE);
        }
        else
        {
            task.Schedule(5s, [&](TaskContext func)
            {
                uint32 count = 3;
                float step = float(2.f * M_PI) / count;
                float angle = 0.f;
                for (uint32 i = 0; i < count; ++i, angle += step)
                {
                    Position pos;
                    me->GetNearPosition(pos, 1.5f, angle);
                    pos.m_positionZ += 1.f;
                    me->SummonCreature(NPC_SKELETON_OF_ZUMRAH, pos, TEMPSUMMON_CORPSE_DESPAWN);
                }

                func.Repeat(randseconds(35, 45));
            });
        }
    }
};

struct npc_zumrah_skeleton : public ZulfarrakAI
{
    npc_zumrah_skeleton(Creature* creature) : ZulfarrakAI(creature) { }

    void Reset() override
    {
        if (!instance)
            return;

        if (Creature* zumrah = instance->GetCreature(BOSS_WITCH_DOCTOR_ZUMRAH))
            if (zumrah->IsAIEnabled)
                zumrah->AI()->JustSummoned(me);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        MythicDoZoneInCombat(me, 100.f);
    }
};

struct npc_zulfarrak_zombie : public ZulfarrakAI
{
    npc_zulfarrak_zombie(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        MythicDoZoneInCombat(me, 75.f);
    }
};

constexpr uint32 SPELL_DEAD_HERO_INFECTED_WOUND{ 3427 };
struct npc_zulfarrak_dead_hero : public ZulfarrakAI
{
    npc_zulfarrak_dead_hero(Creature* creature) : ZulfarrakAI(creature) { }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        MythicDoZoneInCombat(me, 75.f);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 5000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(14000, 21000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_DEAD_HERO_INFECTED_WOUND);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_DEAD_HERO_INFECTED_WOUND, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_DEAD_HERO_INFECTED_WOUND, val, target);
                repeatTime = Milliseconds(60000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

struct npc_sandfury_slave : public ZulfarrakAI
{
    npc_sandfury_slave(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        task.CancelAll();
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

enum SandfuryAcolyte
{
    SPELL_SANDFURY_ACOLYTE_CURSE_OF_WEAKNESS = 11980,
    SPELL_SANDFURY_ACOLYTE_SW_PAIN = 11639,
    SPELL_SANDFURY_ACOLYTE_MANA_BURN = 11981,
    SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT = 9613
};
struct npc_sandfury_acolyte : public ZulfarrakAI
{
    npc_sandfury_acolyte(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        task.CancelAll();

        task.Schedule(Milliseconds(urand(5000, 15000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(22000, 32000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    DoCast(target, SPELL_SANDFURY_ACOLYTE_CURSE_OF_WEAKNESS);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_ACOLYTE_CURSE_OF_WEAKNESS, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U))
                    me->CastCustomSpell(SPELL_SANDFURY_ACOLYTE_CURSE_OF_WEAKNESS, val, target);
                repeatTime = Milliseconds(20000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 8000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(12000, 20000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_SANDFURY_ACOLYTE_SW_PAIN))
                    DoCast(target, SPELL_SANDFURY_ACOLYTE_SW_PAIN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_ACOLYTE_SW_PAIN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 0.0f, true, -SPELL_SANDFURY_ACOLYTE_SW_PAIN))
                    me->CastCustomSpell(SPELL_SANDFURY_ACOLYTE_SW_PAIN, val, target);
                repeatTime = Milliseconds(12000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 4000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(11000, 13000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                }))
                    DoCast(target, SPELL_SANDFURY_ACOLYTE_MANA_BURN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_ACOLYTE_MANA_BURN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
                {
                    return obj->getPowerType() == POWER_MANA;
                });

                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM);

                if (target)
                    me->CastCustomSpell(SPELL_SANDFURY_ACOLYTE_MANA_BURN, val, target);
                repeatTime = Milliseconds(8000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 3000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3000, 4000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

constexpr uint32 SPELL_SANDFURY_ZELOT_ENRAGE{ 8599 };
constexpr uint32 TALK_SANDFURY_ZELOT_ENRAGE{ 0 };
struct npc_sandfury_zelot : public ZulfarrakAI
{
    npc_sandfury_zelot(Creature* creature) : ZulfarrakAI(creature) { }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_enraged && me->HealthBelowPctDamaged(30, damage))
        {
            _enraged = true;
            Talk(TALK_SANDFURY_ZELOT_ENRAGE);

            if (!IsMythic())
                DoCastSelf(SPELL_SANDFURY_ZELOT_ENRAGE, true);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_ZELOT_ENRAGE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_SANDFURY_ZELOT_ENRAGE, val, me, TRIGGERED_FULL_MASK);
            }
        }
    }

    void Reset() override
    {
        task.CancelAll();
        _enraged = false;
    }

    void EnterCombat(Unit* who) override
    {
        task.CancelAll();
        MythicDoZoneInCombat(me, 75.f);
    }
private:
    bool _enraged;
};

enum SandfuryCretin
{
    SPELL_SANDFURY_CRETIN_SW_PAIN = 14032,
    SPELL_SANDFURY_CRETIN_DEMON_SKIN = 20798
};

struct npc_sandfury_cretin : public ZulfarrakAI
{
    npc_sandfury_cretin(Creature* creature) : ZulfarrakAI(creature) { }

    void Reset() override
    {
        task.CancelAll();
        me->GetMotionMaster()->MoveRandom(10.f);

        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(60000);
            if (!IsMythic())
            {
                DoCastSelf(SPELL_SANDFURY_CRETIN_DEMON_SKIN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_CRETIN_DEMON_SKIN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                me->CastCustomSpell(SPELL_SANDFURY_CRETIN_DEMON_SKIN, val, me, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(60000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 5000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(12000, 20000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    DoCast(target, SPELL_SANDFURY_CRETIN_SW_PAIN);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_CRETIN_SW_PAIN, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_SANDFURY_CRETIN_SW_PAIN))
                    me->CastCustomSpell(SPELL_SANDFURY_CRETIN_SW_PAIN, val, target);
                repeatTime = Milliseconds(10000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

struct npc_sullithuz_abomination : public ZulfarrakAI
{
    npc_sullithuz_abomination(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(2000, 9000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(15000, 22000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SULLITHUZ_PETRIFY);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SULLITHUZ_PETRIFY, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SULLITHUZ_PETRIFY, val, target);
                repeatTime = Milliseconds(20000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

class spell_zulfarrak_summon_zulfarrak_zombies : public SpellScript
{
    PrepareSpellScript(spell_zulfarrak_summon_zulfarrak_zombies);

    void HandleSummon(SpellEffIndex effIndex)
    {
        if (effIndex == EFFECT_0)
        {
            if (roll_chance_i(30))
            {
                PreventHitDefaultEffect(effIndex);
                return;
            }
        }
        else if (roll_chance_i(40))
        {
            PreventHitDefaultEffect(effIndex);
            return;
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_zulfarrak_summon_zulfarrak_zombies::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        OnEffectHit += SpellEffectFn(spell_zulfarrak_summon_zulfarrak_zombies::HandleSummon, EFFECT_1, SPELL_EFFECT_SUMMON);
    }
};

constexpr uint32 SPELL_ZERILLIS_NET_ROOT{ 6533 };
constexpr uint32 SPELL_ZERILLIS_FROST_SHOOT{ 12551 };
struct npc_zerillis : public ZulfarrakAI
{
    npc_zerillis(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(12000, 25000));
            if (!IsMythic())
            {
                DoCastAOE(SPELL_ZERILLIS_NET_ROOT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ZERILLIS_NET_ROOT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_ZERILLIS_NET_ROOT, val, target);
                repeatTime = Milliseconds(urand(6000, 8500));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(6000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(6000, 15000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_ZERILLIS_FROST_SHOOT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(0, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_ZERILLIS_FROST_SHOOT, val, target);
                repeatTime = Milliseconds(8000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(2000, 2000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(2000, 2000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_SHADOWHUNTER_SHOOT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_SHADOWHUNTER_SHOOT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_SHADOWHUNTER_SHOOT, val, target);
                repeatTime = Milliseconds(urand(2000, 2000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

enum ThekaMartyrEventIds
{
    EVENT_THEKA_MOVE_SCARABS    = 1,
    SCHEDULER_GROUP_LOCUST_SWARM = 1
};

struct npc_theka_the_martyr : public BossAI
{
    npc_theka_the_martyr(Creature* creature) : BossAI(creature, BOSS_THEKA_MARTYR)
    {
        transformPct = instance->IsMythic() ? 60U : 30U;
        if (instance && instance->IsMythic())
        {
            me->SetObjectScale(2.f);
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

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        Position const pos { 1803.01f, 870.174f, 9.89885f, 4.09666f };
        return me->GetDistance(pos) > 45.f;
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 1787.67f, 878.952f, 8.87736f, 5.73737f };
        if (instance && instance->IsMythic())
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void EnterEvadeMode() override
    {
        summons.DespawnAll();
        _DespawnAtEvade(10, me);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (me->GetVictim())
            return;

        if (me->GetDistance(who) < 7.f && me->CanStartAttack(who))
            AttackStart(who);
    }

    void Reset() override
    {
        BossAI::Reset();
        _transformed = false;
        _anubGUID = 0;
        _scarabsAlive = 0;
        if (instance->IsMythic())
        {
            SpawnAnubTrigger();
            HandleScarabs();
        }
    }

    void SpawnAnubTrigger()
    {
        // cant spawn at non mythic map
        if (!instance->IsMythic())
            return;

        Creature* anub = me->SummonCreature(NPC_THEKA_ANUB_TRIGGER_MYTHIC, { 1803.01f, 870.174f, 9.89885f, 4.09666f });
        if (!anub)
            return;

        _anubGUID = anub->GetGUID();
        anub->SetPassive();
    }

    void SetData(uint32 data, uint32 /*value*/) override
    {
        if (!me->IsInCombat())
            return;

        if (data == DATA_THEKA_SCARAB_DONE)
        {
            ++_scarabsAlive;
            UpdateAnubsSwarm();
            ScheduleAnotherScarab(false);
        }
        else if (data == DATA_THEKA_SCARAB_DEATH)
        {
            --_scarabsAlive;
            UpdateAnubsSwarm();
        }
    }

    uint32 GetData(uint32 data) const override
    {
        return static_cast<uint32>(_scarabsAlive);
    }

    Seconds const GetEventTimer(uint32 id)
    {
        //! scarabs
        if (id == EVENT_THEKA_MOVE_SCARABS)
            return Seconds(20 - instance->GetMythicKeyLevel());

        //! This cannot happen ever
        ASSERT(false);
        return 0s;
    }

    void ScheduleAnotherScarab(bool first)
    {
        scheduler.Schedule(first ? 5s : GetEventTimer(EVENT_THEKA_MOVE_SCARABS), [&](TaskContext func)
        {
            uint64 const scarabGUID = Trinity::Containers::SelectRandomContainerElement(_nearbyScarabsGUIDs);
            Creature* scarab = ObjectAccessor::GetCreature(*me, scarabGUID);
            if (scarab && scarab->IsAlive() && scarab->IsAIEnabled)
                scarab->AI()->SetGUID(_anubGUID, ACTION_THEKA_MOVE_SCARAB);

            _nearbyScarabsGUIDs.erase(std::remove(_nearbyScarabsGUIDs.begin(), _nearbyScarabsGUIDs.end(),
                scarabGUID), _nearbyScarabsGUIDs.end());
        });
    }

    void UpdateAnubsSwarm()
    {
        if (_scarabsAlive < 0)
            _scarabsAlive = 0;

        Creature* anub = ObjectAccessor::GetCreature(*me, _anubGUID);
        if (!anub)
            return;

        if (!_scarabsAlive)
        {
            if (anub->HasAura(SPELL_LEECHING_SWARM_THEKA))
                anub->RemoveAurasDueToSpell(SPELL_LEECHING_SWARM_THEKA);
        }
        else
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
            anub->CastCustomSpell(SPELL_LEECHING_SWARM_THEKA, val, anub, TRIGGERED_FULL_MASK);
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (attacker && attacker->IsPlayer() && attacker->GetPositionZ() > 17.f)
        {
            damage = 0;
            Unit::Kill(me, attacker);
        }

        if (!_transformed && me->HealthBelowPctDamaged(transformPct, damage))
        {
            _transformed = true;
            scheduler.Schedule(1ms, [&](TaskContext func)
            {
                if (instance->IsMythic())
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
                    me->CastCustomSpell(SPELL_THEKA_TRANSFORM, val, me, TRIGGERED_FULL_MASK);
                    func.Repeat(30s);
                }
                else
                    DoCastSelf(SPELL_THEKA_TRANSFORM, true);
            });
        }
    }

    void HandleScarabs()
    {
        _nearbyScarabsGUIDs.clear();
        std::list<Creature*> _temp;
        me->GetCreatureListWithEntryInGrid(_temp, NPC_SCARAB_ZULFARRAK, 100.f);
        for (Creature* creature : _temp)
        {
            if (creature->GetPositionZ() >= 10.5f)
                continue;

            if (instance->IsMythic())
            {
                creature->SetPassive();
                if (!creature->IsAlive())
                    creature->Respawn();

                if (creature->IsSelectable())
                    creature->SetSelectable(false);

                creature->SetImmuneToAll(true);


                if (creature->IsAIEnabled)
                    creature->AI()->EnterEvadeMode();

                if (creature->m_mythicScaling)
                    creature->m_mythicScaling->m_affixesAllowed = false;

                _nearbyScarabsGUIDs.push_back(creature->GetGUID());
            }
            else
                _nearbyScarabsGUIDs.push_back( creature->GetGUID() );
        }
    }

    void EnterCombat(Unit* who) override
    {
        HandleScarabs();
        if ( !_nearbyScarabsGUIDs.empty() )
            Trinity::Containers::RandomShuffle( _nearbyScarabsGUIDs );

        BossAI::EnterCombat(who);
        scheduler.Schedule(Milliseconds(urand(1000, 5000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 14000));
            if (!instance->IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    DoCast(target, SPELL_THEKA_FEVERED_PLAGUE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_THEKA_FEVERED_PLAGUE, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1000);
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
                {
                    return !obj->HasAura(SPELL_THEKA_FEVERED_PLAGUE);
                });

                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM);

                if (target)
                    me->CastCustomSpell(SPELL_THEKA_FEVERED_PLAGUE, val, target);
                repeatTime = Milliseconds(urand(10500, 13000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (!instance->IsMythic())
        {
            scheduler.Schedule(2s, [&](TaskContext func)
            {
                uint64 scarabGUID = Trinity::Containers::SelectRandomContainerElement(_nearbyScarabsGUIDs);
                Creature* scarab = ObjectAccessor::GetCreature(*me, scarabGUID);
                if (scarab && scarab->IsAlive() && !scarab->IsInCombat() && scarab->IsAIEnabled)
                {
                    scarab->DisableChainPullFor(120000ms);
                    scarab->AI()->DoZoneInCombat(scarab, 100.f);
                }

                func.Repeat(6s);
            });
        }
        else
            ScheduleAnotherScarab(true);

        if (instance->IsMythic())
        {
            scheduler.Schedule(5s, [&](TaskContext func)
            {
                DoCastVictim(SPELL_THEKA_POISON_SHOCK_MYTHIC);
                func.Repeat(7s);
            });

            scheduler.Schedule(10s, [&](TaskContext func)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [](Unit* obj)
                {
                    return obj->IsPlayer();
                }))
                    ///! validate Z position
                    me->SummonCreature(NPC_THEKA_POISON_TRIGGER_MYTHIC, target->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN,
                        60000);

                func.Repeat(20s);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff, std::bind(&BossAI::DoMeleeAttackIfReady, this));
        EnterEvadeIfOutOfCombatArea();
    }
private:
    uint32 transformPct;
    bool _transformed;
    std::vector<uint64> _nearbyScarabsGUIDs;
    uint64 _anubGUID;
    int32 _scarabsAlive;
};

struct npc_theka_poison_trigger : public ZulfarrakAI
{
    npc_theka_poison_trigger(Creature* creature) : ZulfarrakAI(creature) { }

    void Reset() override
    {
        me->SetSelectable(false);
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 2.0f);
        me->setFaction(21);
        sizeTimer = 0;
        task.Schedule(1ms, [&](TaskContext /*func*/)
        {
            me->SetImmuneToAll(true);
            DoCastSelf(28158);
        });

        task.Schedule(2s, [&](TaskContext /*func*/)
        {
            me->SetImmuneToAll(false);
            MythicDoZoneInCombat(me, 100.f);
        });
    }

    void AttackStart(Unit* /*who*/) override { }
    void EnterCombat(Unit* /*who*/) override { }
    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);

        //! Grobbulus CnP
        sizeTimer += diff;
        // increase size to 15yd in 60 seconds, 0.00025 is the growth of size in 1ms
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 2.0f + (0.00025f * sizeTimer));
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*type*/, SpellSchoolMask /*mask*/) override
    {
        damage = 0;
    }
private:
    uint32 sizeTimer;
};

constexpr uint32 SPELL_LEECHING_SWARM_THEKA_TRIGGERED{ 53468 };
struct spell_theka_leeching_swarm_aura : public AuraScript
{
    PrepareAuraScript(spell_theka_leeching_swarm_aura);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetCaster();
        if (!target)
            return;

        InstanceScript* instance = target->GetInstanceScript();
        if (!instance || !instance->IsMythic())
            return;

        Creature* theka = instance->GetCreature(BOSS_THEKA_MARTYR);
        if (!theka)
            return;

        PreventDefaultAction();
        uint32 scarabCount = theka->AI()->GetData(0);
        if (!scarabCount)
            return;

        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT0, 3500U * scarabCount);
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3000);
        val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 20000.f);
        target->CastCustomSpell(SPELL_LEECHING_SWARM_THEKA_TRIGGERED, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_theka_leeching_swarm_aura::HandlePeriodic, EFFECT_0,
            SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

struct npc_theka_anub_scarab : public ZulfarrakAI
{
    npc_theka_anub_scarab(Creature* creature) : ZulfarrakAI(creature)
    {
        me->SetSelectable(false);
    }

    void UpdateAI(uint32 /*diff*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*type*/, SpellSchoolMask /*mask*/) override
    {
        damage = 0;
    }
};

class at_antu_sul_zulfarrak : public AreaTriggerScript
{
public:
    at_antu_sul_zulfarrak() : AreaTriggerScript("at_antu_sul_zulfarrak") { }

    bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
    {
        if (player->IsGameMaster())
            return false;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance)
            return false;

        Creature* antusul = instance->GetCreature(BOSS_ANTUSUL);
        if (!antusul || antusul->isDead() || !antusul->IsAIEnabled)
            return false;

        antusul->AI()->DoZoneInCombat(antusul, 200.f);
        return false;
    }
};

enum Antusulmisc
{
    SPELL_ANTUSUL_EARTHSHOCK = 15501,
    SPELL_ANTUSUL_CHAIN_LIGHTNING = 16006,
    SPELL_ANTUSUL_EARTH_GRAB = 8376,
    SPELL_ANTUSUL_HEALING_WARD = 11899,
    SPELL_ANTUSUL_MINION = 11894,
    SPELL_ANTUSUL_HEALING_WAVE = 11895,
    SPELL_ANTUSUL_MYTHIC_DIVINE_SHIELD = 11890,
    SPELL_ANTUSUL_MYTHIC_FLAME_SHOCK = 59684,
    SPELL_ANTUSUL_SHAMANISTIC_RAGE = 30823,
    NPC_ANTUSUL_MINION = 8156,
    NPC_ANTUSUL_LIGHTNING_WOLF = 91504,
    NPC_ANTUSUL_EARTHGRAB_TOTEM = 6066,
    NPC_ANTUSUL_HEALINGWARD_TOTEM = 8179
};

constexpr uint32 ELECTRIC_SPAWN_POS_SIZE{ 2 };
Position const ElectricWolfPositions[ELECTRIC_SPAWN_POS_SIZE] =
{
    { 1821.22f, 717.152f, 14.7863f, 1.74596f },
    { 1804.00f, 717.658f, 14.7031f, 1.74596f }
};

struct npc_antusul_zulfarrak : public BossAI
{
    npc_antusul_zulfarrak(Creature* creature) : BossAI(creature, BOSS_ANTUSUL)
    {
        if (instance && instance->IsMythic())
        {
            me->SetObjectScale(2.f);
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

    bool CanAIAttack(Unit const* who) const override
    {
        if (instance && instance->IsMythic() && who && who->IsPlayer() && who->GetPositionZ() > 22.5f)
        {
            me->getThreatManager().resetAggro([who](Unit* obj)
            {
                return obj->GetGUID() == who->GetGUID();
            });

            return false;
        }

        return true;
    }

    void SummonMinion(bool reset, float const angle)
    {
        if (!instance->IsMythic())
            DoCastAOE(SPELL_ANTUSUL_MINION, true);
        else
        {
            Position pos = me->GetPosition();
            me->MovePositionToFirstCollision(pos, 2.5f, 0.f);
            Creature* minion = me->SummonCreature(NPC_ANTUSUL_MINION, pos, TEMPSUMMON_CORPSE_DESPAWN);
            if (!reset)
            {
                DoCastSelf(SPELL_ANTUSUL_MYTHIC_DIVINE_SHIELD, true);
                ++_minionCount;
            }
            else if (minion)
                _firstMinionGUID = minion->GetGUID();

            if (minion && minion->IsAIEnabled)
                minion->AI()->DoZoneInCombat(minion, 100.f);
        }
    }

    void Reset() override
    {
        BossAI::Reset();
        _minionCount = 0;
        _firstMinionGUID = 0;
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        if ((summon->GetEntry() == NPC_ANTUSUL_MINION || summon->GetEntry() == NPC_ANTUSUL_LIGHTNING_WOLF) && me->IsInCombat())
        {
            if (summon->IsAIEnabled)
                summon->AI()->DoZoneInCombat(summon, 150.f);
        }
        else
            summon->SetInCombatWithZone();
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        Position const caveEnterance{ 1811.92f, 724.211f, 14.3445f, 4.56849f };
        return me->GetDistance(caveEnterance) > 70.f;
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        BossAI::SummonedCreatureDies(summon, killer);
        if (!instance->IsMythic())
            return;

        if (summon->GetEntry() != NPC_ANTUSUL_MINION || summon->GetGUID() == _firstMinionGUID)
            return;

        --_minionCount;
        if (!_minionCount && me->HasAura(SPELL_ANTUSUL_MYTHIC_DIVINE_SHIELD))
            me->RemoveAurasDueToSpell(SPELL_ANTUSUL_MYTHIC_DIVINE_SHIELD);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_75Health && me->HealthBelowPctDamaged(75, damage))
        {
            _75Health = true;
            SummonMinion(false, static_cast<float>(M_PI / 2.f));
            if (instance->IsMythic())
            {
                scheduler.Schedule(1s, [&](TaskContext func)
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ANTUSUL_MYTHIC_FLAME_SHOCK, data));
                    float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));

                    auto tList = me->getThreatManager().getThreatList();
                    for (auto const& actor : tList)
                    {
                        Unit* unit = ObjectAccessor::GetUnit(*me, actor->getUnitGuid());
                        if (!unit || !unit->IsPlayer())
                            continue;

                        me->CastCustomSpell(SPELL_ANTUSUL_MYTHIC_FLAME_SHOCK, val, unit);
                    }

                    func.Repeat(randseconds(15, 20));
                });
            }
        }

        if (!_50Health && me->HealthBelowPctDamaged(50, damage))
        {
            _50Health = true;
            SummonMinion(false, static_cast<float>(M_PI / 2.f));
            if (instance->IsMythic())
            {
                scheduler.Schedule(1s, [&](TaskContext func)
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 15000);
                    me->CastCustomSpell(SPELL_ANTUSUL_SHAMANISTIC_RAGE, val, me, TRIGGERED_FULL_MASK);
                    func.Repeat(1min);
                });


                for (uint32 i = 0; i < 2; ++i)
                {
                    Creature* wolf = me->SummonCreature(NPC_ANTUSUL_LIGHTNING_WOLF, ElectricWolfPositions[i], TEMPSUMMON_CORPSE_DESPAWN);
                    if (wolf && wolf->IsAIEnabled)
                    {
                        wolf->AI()->DoZoneInCombat(wolf, 150.f);
                    }
                }
            }
        }

        if (!_25Health && me->HealthBelowPctDamaged(25, damage))
        {
            _25Health = true;
            SummonMinion(false, static_cast<float>(M_PI / 2.f));

            if (instance->IsMythic())
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
                me->CastCustomSpell(2825, val, me);
                for (uint64 summ : summons)
                {
                    Creature* summoned = ObjectAccessor::GetCreature(*me, summ);
                    if (!summoned)
                        continue;

                    summoned->CastCustomSpell(2825, val, summoned);
                }
            }
        }

        if (!instance->IsMythic() && !_healScheduled && me->HealthBelowPctDamaged(20, damage))
        {
            _healScheduled = true;
            DoCastSelf(SPELL_ANTUSUL_HEALING_WAVE);
        }
    }

    void EnterEvadeMode() override
    {
        summons.DespawnAll();
        _DespawnAtEvade(10, me);
    }

    void EnterCombat(Unit* who) override
    {
        BossAI::EnterCombat(who);
        _75Health = false;
        _50Health = false;
        _25Health = false;
        _healScheduled = false;
        _firstMinionGUID = 0;
        SummonMinion(true, static_cast<float>(M_PI / 2.f));
        summons.DoZoneInCombat();

        scheduler.Schedule(5s, [&](TaskContext func)
        {
            std::vector<uint64> _playerStore;
            auto const& tList = me->getThreatManager().getThreatList();
            for (auto const& ref : tList)
            {
                Unit* target = ref->getTarget();
                if (target && target->ToPlayer() && target->GetPositionZ() > 22.5f)
                    _playerStore.push_back(target->GetGUID());
            }

            for (uint64 guid : _playerStore)
            {
                Player* player = ObjectAccessor::GetPlayer(*me, guid);
                if (player && player->IsInMap(me))
                {
                    me->MonsterWhisper("You cannot stand there!", player, false);
                    player->NearTeleportTo(me->GetPosition());
                }
            }

            func.Repeat(randseconds(3, 6));
        });

        scheduler.Schedule(3s, [&](TaskContext func)
        {
            auto repeatTime = randseconds(9, 11);
            if (!instance->IsMythic())
            {
                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
                {
                    return obj->IsPlayer() && obj->IsWithinDist(me, 20.f) && obj->IsCasting();
                });

                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM);

                DoCast(target, SPELL_ANTUSUL_EARTHSHOCK, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ANTUSUL_EARTHSHOCK, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);

                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
                {
                    return obj->IsPlayer() && obj->IsWithinDist(me, 20.f) && obj->IsCasting();
                });

                if (!target)
                    target = SelectTarget(SELECT_TARGET_RANDOM);

                me->CastCustomSpell(SPELL_ANTUSUL_EARTHSHOCK, val, target, TRIGGERED_FULL_MASK);
                repeatTime = 9s;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        scheduler.Schedule(Milliseconds(5000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(12000, 14000));
            if (!instance->IsMythic())
            {
                DoCastVictim(SPELL_ANTUSUL_CHAIN_LIGHTNING);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ANTUSUL_CHAIN_LIGHTNING, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1000);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.f, true))
                    me->CastCustomSpell(SPELL_ANTUSUL_CHAIN_LIGHTNING, val, target);

                repeatTime = Milliseconds(urand(9000, 13000));
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        scheduler.Schedule(randseconds(5, 8), [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            if (target && target->IsWithinCombatRange(me, 5.f))
            {
                summons.DespawnEntry(NPC_ANTUSUL_EARTHGRAB_TOTEM);
                if (!instance->IsMythic())
                    DoCastAOE(SPELL_ANTUSUL_EARTH_GRAB);
                else
                {
                    Position pos = me->GetPosition();
                    me->MovePositionToFirstCollision(pos, 2.5f, 0.f);
                    Creature* totem = me->SummonCreature(NPC_ANTUSUL_EARTHGRAB_TOTEM, pos, TEMPSUMMON_CORPSE_DESPAWN);
                    if (totem)
                        totem->SetPassive();
                }
            }

            if (instance->IsMythic())
                func.Repeat(randseconds(25, 35));
            else
                func.Repeat(20s);
        });

        scheduler.Schedule(13s, [&](TaskContext func)
        {
            auto repeatTime = 17s;
            if (!instance->IsMythic())
                DoCastAOE(SPELL_ANTUSUL_HEALING_WARD, true);
            else
            {
                summons.DespawnEntry(NPC_ANTUSUL_HEALINGWARD_TOTEM);

                Position pos = me->GetPosition();
                me->MovePositionToFirstCollision(pos, 2.5f, 0.f);
                Creature* totem = me->SummonCreature(NPC_ANTUSUL_HEALINGWARD_TOTEM, pos, TEMPSUMMON_CORPSE_DESPAWN);
                if (totem)
                {
                    totem->SetPassive();
                    totem->setFaction(me->getFaction());
                }
                repeatTime = randseconds(29, 37);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 1769.676147f, 730.951660f, 16.511854f, 0.361025f };
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

        scheduler.Update(diff, std::bind(&BossAI::DoMeleeAttackIfReady, this));
        EnterEvadeIfOutOfCombatArea();
    }
private:
    bool _75Health, _50Health, _25Health;
    bool _healScheduled;
    uint32 _minionCount;
    uint64 _firstMinionGUID;
};

enum ElectricWolfMisc
{
    SPELL_WOLF_ELECTRIC_LASH = 62939,
    SPELL_WOLF_ELECTRIC_OVERLOAD = 59038,
    SPELL_WOLF_PERM_FEIGN_DEATH = 29266
};

struct npc_electric_wolf_antusul : public ZulfarrakAI
{
    npc_electric_wolf_antusul(Creature* creature) : ZulfarrakAI(creature)
    {
        _died = false;

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

    void EnterCombat(Unit* who) override
    {
        ZulfarrakAI::EnterCombat(who);

        task.Schedule(Seconds(urand(3, 7)), [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_WOLF_ELECTRIC_LASH, data));
            float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            if (Unit* target = me->GetVictim())
                me->CastCustomSpell(SPELL_WOLF_ELECTRIC_LASH, val, target, TRIGGERED_FULL_MASK);

            func.Repeat(randseconds(4, 7));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (damage > me->GetHealth() - 1)
        {
            damage = 0;
            if (!_died)
            {
                _died = true;
                me->AttackStop();
                me->GetMotionMaster()->Clear();

                DoCastSelf(SPELL_WOLF_PERM_FEIGN_DEATH, true);

                task.Schedule(250ms, [&](TaskContext func)
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 5000);
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, 25000);
                    me->CastCustomSpell(SPELL_WOLF_ELECTRIC_OVERLOAD, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                    me->DespawnOrUnsummon(1000ms);
                });

                //! fallback in case we do not trigger task
                me->DespawnOrUnsummon(5s);
            }
        }
    }

private:
    bool _died;
};

Position const ZumrahMythicHomePosition{ 1868.89f, 1030.75f, 8.87773f, 5.82201f };
Position const ZumrahCauldronSpawnPosition{ 1862.84f, 1033.76f, 8.87685f, 5.82201f };
constexpr uint32 ZUMRAH_WARD_POSITION_SIZE{ 8 };
Position const ZumrahWardPositions[ZUMRAH_WARD_POSITION_SIZE] =
{
    { 1890.620605f, 1000.725525f, 8.876894f },
    { 1874.883301f, 998.921021f, 8.878757f },
    { 1901.033691f, 1015.220337f, 12.204275f },
    { 1870.288208f, 1014.454163f, 8.877494f },
    { 1875.905396f, 1039.189819f, 8.876985f },
    { 1863.027100f, 1033.844971f, 8.876811f },
    { 1847.228027f, 1029.911621f, 9.111856f },
    { 1851.835693f, 1053.405518f, 8.877021f }
};

enum ZumrahMisc
{
    GO_ZUMRAH_CAULDRON = 176361,
    NPC_ZUMRAH_CAULDRON_TRIGGER = 91505,
    SPELL_ZUMRAH_MYTHIC_FIREBALL_VOLLEY = 43240,
    SPELL_ZUMRAH_MYTHIC_POISON_VOLLEY = 38655,
    SPELL_ZUMRAH_AOE_CURSE_OF_AGONY = 39672,
    SPELL_ZUMRAH_IMMOLATE = 75383,
    SPELL_ZUMRAH_HEALING_WAVE    = 12491,
    SPELL_ZUMRAH_WARD_OF_ZUMRAH = 11086,
    SPELL_ZUMRAH_BIG_BANG = 64443,
    NPC_ZUMRAH_WARD_OF_ZUMRAH = 7785,
    SPELL_ZUMRAH_SHADOW_BOLT = 12739,
    SPELL_ZUMRAH_SHADOW_BOLT_VALLEY = 15245,
    SPELL_ZUMRAH_MYTHIC_METEOR = 28884,
    NPC_ZUMRAH_GRAVE_ZOMBIE = 7286,
    NPC_ZUMRAH_GRAVE_DEAD_HERO = 7276,
    GO_ZUMRAH_SHALLOW_GRAVE_1 = 128403,
    GO_ZUMRAH_SHALLOW_GRAVE_2 = 128308,

    SAY_ZUMRAH_AGGRO    = 0,
    SAY_ZUMRAH_DEATH    = 1,

    ZUMRAH_SCHEDULER_HARD_WIPE   = 1
};

struct npc_witchdoctor_zumrah : public BossAI
{
    npc_witchdoctor_zumrah(Creature* creature) : BossAI(creature, BOSS_WITCH_DOCTOR_ZUMRAH)
    {
        _finishedSettingMythic = false;
        _cauldronTriggerGUID   = 0;
        if (instance && instance->IsMythic())
        {
            me->SetObjectScale(2.5f);
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
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        }
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (!who)
            return false;

        float const posY = who->GetPositionY();
        return posY > 974.5f && posY < 1063.f;
    }

    void PrepareMythic()
    {
        if (!_finishedSettingMythic)
        {
            me->SetHomePosition(ZumrahMythicHomePosition);
            me->Relocate(ZumrahMythicHomePosition);
            me->setFaction(14);

            GameObject* cauldron = me->SummonGameObject(GO_ZUMRAH_CAULDRON, ZumrahCauldronSpawnPosition, 0.f, 0.f, 0.f, 0.f, 0);
            if (cauldron)
            {
                cauldron->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                cauldron->EnableCollision(false);
            }

            _finishedSettingMythic = true;
        }

        Position triggerPosition = ZumrahCauldronSpawnPosition;
        triggerPosition.m_positionZ += 5.f;

        Creature* previousTrigger = ObjectAccessor::GetCreature(*me, _cauldronTriggerGUID);
        if (previousTrigger)
            previousTrigger->DespawnOrUnsummon();

        Creature* trigger = me->SummonCreature(NPC_ZUMRAH_CAULDRON_TRIGGER, triggerPosition);
        if (trigger)
            _cauldronTriggerGUID = trigger->GetGUID();
    }

    void Reset() override
    {
        BossAI::Reset();
        _cauldronScheduler.CancelAll();

        if (_graveGUIDs.empty())
        {
            std::list<GameObject*> _temps;
            me->GetGameObjectListWithEntryInGrid(_temps, GO_ZUMRAH_SHALLOW_GRAVE_1, 100.f);
            me->GetGameObjectListWithEntryInGrid(_temps, GO_ZUMRAH_SHALLOW_GRAVE_2, 100.f);

            for (GameObject* go : _temps)
                _graveGUIDs.push_back(go->GetGUID());
        }

        if (instance->IsMythic())
        {
            PrepareMythic();
            _volleySpellId = SPELL_ZUMRAH_MYTHIC_FIREBALL_VOLLEY;
        }
    }

    void AttackStart(Unit* who) override
    {
        if (!who)
            return;

        if (instance->IsMythic())
            me->Attack(who, false);
        else
        {
            auto const _currentMana = me->GetPower(POWER_MANA);
            auto const _maxMana = me->GetMaxPower(POWER_MANA);
            if (_currentMana > (_maxMana * 0.1f))
                AttackStartCaster(who, 100.f);
            else
                ScriptedAI::AttackStart(who);
        }
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        Talk(SAY_ZUMRAH_DEATH);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 1881.421753f, 985.902649f, 8.876889f, 1.032854f };
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

        scheduler.Update(diff);
        _cauldronScheduler.Update(diff);
        //DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (attacker && attacker->IsPlayer() && attacker->GetPositionZ() > 15.f)
        {
            damage = 0;
            Unit::Kill(me, attacker);
        }

        if (!_healScheduled && me->HealthBelowPctDamaged(70, damage))
        {
            _healScheduled = true;
            scheduler.Schedule(Milliseconds(1000), [&](TaskContext func)
            {
                auto repeatTime = 0ms;
                if (!instance->IsMythic())
                    DoCastSelf(SPELL_ZUMRAH_HEALING_WAVE);
            });
        }
    }

    void EnterCombat(Unit* who) override
    {
        BossAI::EnterCombat(who);
        _healScheduled = false;
        Talk(SAY_ZUMRAH_AGGRO);

        _cauldronScheduler.Schedule(5s, [&](TaskContext func)
        {
            std::vector<uint64> _playerStore;
            auto const& tList = me->getThreatManager().getThreatList();
            for (auto const& ref : tList)
            {
                Unit* target = ref->getTarget();
                if (target && target->ToPlayer() && target->GetPositionZ() > 20.5f)
                    _playerStore.push_back(target->GetGUID());
            }

            for (uint64 guid : _playerStore)
            {
                Player* player = ObjectAccessor::GetPlayer(*me, guid);
                if (player && player->IsInMap(me))
                {
                    me->MonsterWhisper("You cannot stand there!", player, false);
                    player->NearTeleportTo(me->GetPosition());
                }
            }

            func.Repeat(randseconds(3, 6));
        });

        scheduler.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3000, 4000));
            if (!instance->IsMythic())
            {
                DoCastVictim(SPELL_ZUMRAH_SHADOW_BOLT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ZUMRAH_SHADOW_BOLT, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1500);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_ZUMRAH_SHADOW_BOLT, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        scheduler.Schedule( instance->IsMythic() ? 7s : randseconds( 5, 6 ), [&]( TaskContext func )
        {
            auto repeatTime = randseconds( 30, 36 );
            if ( !instance->IsMythic() )
            {
                DoCastSelf( SPELL_ZUMRAH_WARD_OF_ZUMRAH, true );
            }
            else
            {
                Position pos = Trinity::Containers::SelectRandomContainerElement( ZumrahWardPositions );
                Creature* ward = me->SummonCreature( NPC_ZUMRAH_WARD_OF_ZUMRAH, pos );
                if ( ward )
                {
                    ward->SetObjectScale( 3.f );
                    me->MonsterTextEmote( "Ward of Zumrah appears!", nullptr, true );
                    ward->SetPassive();
                }

                repeatTime = 40s;
            }

            func.Repeat( repeatTime );
        } );

        if (instance->IsMythic())
        {
            _cauldronScheduler.Schedule(GetMythicTimerForEvent(ZUMRAH_SCHEDULER_HARD_WIPE), [&](TaskContext func)
            {
                scheduler.CancelAll();
                DoCastAOE(SPELL_ZUMRAH_BIG_BANG, true);
            });

            /*_cauldronScheduler.Schedule(Milliseconds(urand(5000, 8000)), [&](TaskContext func)
            {
            //! if you choose to use it, fix caster, it should be cauldron
                auto repeatTime = Milliseconds(urand(12000, 16000));
                if (!IsMythic)
                {
                    DoCastAOE(SPELL_ZUMRAH_SHADOW_BOLT_VALLEY);
                }
                else
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ZUMRAH_SHADOW_BOLT_VALLEY, data));
                    float variance = float(MythicKeyLevel) * data.SpellValueVariance;

                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                    me->CastCustomSpell(SPELL_ZUMRAH_SHADOW_BOLT_VALLEY, val, (Unit*)nullptr);
                    repeatTime = Milliseconds(8000);
                }

                if (repeatTime.count())
                    func.Repeat(repeatTime);
            });*/

            _cauldronScheduler.Schedule(10s, [&](TaskContext func)
            {
                Creature* trigger = ObjectAccessor::GetCreature(*me, _cauldronTriggerGUID);
                if (trigger)
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(_volleySpellId, data));
                    float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    if (_volleySpellId == SPELL_ZUMRAH_MYTHIC_POISON_VOLLEY)
                        val.AddSpellMod(SPELLVALUE_BASE_POINT1, 500);
                    val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
                    val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 35000);

                    trigger->CastCustomSpell(_volleySpellId, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                }

                func.Repeat(3s);
            });

            _cauldronScheduler.Schedule(3s, [&](TaskContext func)
            {
                Creature* trigger = ObjectAccessor::GetCreature(*me, _cauldronTriggerGUID);
                if (trigger)
                {
                    Mythic::MythicSpells data;
                    ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ZUMRAH_IMMOLATE, data));
                    float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                    val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, true, -SPELL_ZUMRAH_IMMOLATE);
                    if (target)
                        trigger->CastCustomSpell(SPELL_ZUMRAH_IMMOLATE, val, target, TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_CAST_DIRECTLY));
                }

                func.Repeat(Seconds(urand(9, 14)));
            });

            scheduler.Schedule(12s, [&](TaskContext func)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, std::min(4500U, 650U * instance->GetMythicKeyLevel()));
                me->CastCustomSpell(SPELL_ZUMRAH_AOE_CURSE_OF_AGONY, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                func.Repeat(1min);
            });

            _cauldronScheduler.Schedule(20s, [&](TaskContext func)
            {
                _volleySpellId = _volleySpellId == SPELL_ZUMRAH_MYTHIC_POISON_VOLLEY ? SPELL_ZUMRAH_MYTHIC_FIREBALL_VOLLEY :
                    SPELL_ZUMRAH_MYTHIC_POISON_VOLLEY;
                func.Repeat(30s);
            });

            _cauldronScheduler.Schedule(37s, [&](TaskContext func)
            {
                uint64 const goGUID = Trinity::Containers::SelectRandomContainerElement(_graveGUIDs);
                GameObject* go = ObjectAccessor::GetGameObject(*me, goGUID);
                if (go)
                {
                    Position const summonPos = me->GetNearPositionFromPos(go->GetPosition(), 2.5f);
                    Creature* graveFighter = me->SummonCreature(NPC_ZUMRAH_GRAVE_ZOMBIE, summonPos, TEMPSUMMON_CORPSE_DESPAWN);
                    if (graveFighter && graveFighter->AI())
                        graveFighter->AI()->DoZoneInCombat(me, 100.f);
                }

                func.Repeat(55s);
            });
        }
    }

    Seconds const GetMythicTimerForEvent(uint32 id)
    {
        if (id == ZUMRAH_SCHEDULER_HARD_WIPE)
        {
            return 360s;
            //return Seconds(std::max(3U, 6U - MythicKeyLevel));
        }

        return 0s;
    }
private:
    bool _finishedSettingMythic;
    bool _healScheduled;
    uint64 _cauldronTriggerGUID;
    uint32 _volleySpellId;
    std::vector<uint64> _graveGUIDs;
    TaskScheduler _cauldronScheduler;
};

struct npc_zumrah_cauldron_triggerAI : public ScriptedAI
{
    npc_zumrah_cauldron_triggerAI(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 diff) override { }
    void AttackStart(Unit* who) override { me->Attack(who, false); }
    void EnterCombat(Unit* who) override { }
    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        damage = 0;
    }
};

enum VelrathaMisc
{
    SPELL_HYDROMANCER_WATER_SPOUT = 35735,
    SPELL_HYDROMANCER_HEALING_WAVE = 12491,
    SPELL_HYDROMANCER_WATER_BOLT = 32011,
    SPELL_HYDROMANCER_ICEBLOCK = 45776,
    SPELL_HYDROMANCER_ORB_FROSTNOVA = 31250,
    SPELL_HYDROMANCER_FROSTBOLT_VOLLEY = 62604,
    SPELL_HYDROMANCER_FROZEN_EMBRACE = 63136,
    SPELL_GAHZRILLA_MYTHIC_FROST_BREATH = 69649,
    SPELL_HYDROMANCER_ELEMENTAL_FREEZING_CIRCLE = 69574,
    SPELL_HYDROMANCER_ELEMENTAL_WATERY_GRAVE = 38049,
    SPELL_HYDROMANCER_ELEMENTAL_FROSTBOLT = 50378,
    SPELL_HYDROMANCER_WATER_TOMB_HIT = 38235,
    NPC_HYDROMANCER_WATER_TOMB = 91704,
    DISPLAY_ID_WATER_GLOBULE = 20782,
    NPC_GAHZRILLA_MYTHIC = 91506,
    NPC_VELRATHA_FROZENORB_TRIGGER = 91507,
    POINT_VELRATHA_FROZEN_ORB_MOVE = 100,
    DATA_VELRATHA_GET_MOVE_TARGET = 101
};

struct npc_hydromancer_velratha : public BossAI
{
    npc_hydromancer_velratha(Creature* creature) : BossAI(creature, BOSS_HYDROMANCER_VELRATHA)
    {
        if (instance && instance->IsMythic())
        {
            me->SetObjectScale(2.f);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
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
    }

    void AttackStart(Unit* who) override
    {
        if (!who)
            return;

        auto const _currentMana = me->GetPower(POWER_MANA);
        auto const _maxMana = me->GetMaxPower(POWER_MANA);
        if (_currentMana > (_maxMana * 0.1f))
            AttackStartCaster(who, 10.f);
        else
            ScriptedAI::AttackStart(who);
    }

    uint64 GetData64(uint32 data) const override
    {
        if (data == DATA_VELRATHA_GET_MOVE_TARGET)
        {
            Unit* target = me->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0U, 100.f, true, -SPELL_HYDROMANCER_WATER_TOMB_HIT);
            return target ? target->GetGUID() : 0;
        }

        return 0;
    }

    void JustSummoned(Creature* summon) override
    {
        //base class calls do zone in combat which breaks gahzrilla
        summons.Summon(summon);
    }

    void UnleashTheGahzrilla()
    {
        Position const GahzRillaSpawnPosition{ 1659.52f, 1180.41f, -2.88258f, 0.734786f };
        me->SummonCreature(NPC_GAHZRILLA_MYTHIC, GahzRillaSpawnPosition);
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
        summons.DespawnEntry(NPC_VELRATHA_FROZENORB_TRIGGER);
        val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
        me->CastCustomSpell(SPELL_HYDROMANCER_ICEBLOCK, val, me, TRIGGERED_FULL_MASK);
        me->SetControlled(true, UNIT_STATE_STUNNED);
        scheduler.CancelAll();
        me->SetSelectable(false);
    }

    void ScheduleCheckPeopleInWater()
    {
        return;

        _secondScheduler.Schedule( 5s, [&]( TaskContext func )
        {
            auto const& tlist = me->getThreatManager().getThreatList();
            for ( auto ref : tlist )
            {
                if (!ref)
                    continue;

                Unit* unit = ObjectAccessor::GetUnit( *me, ref->getUnitGuid() );
                if ( unit && unit->IsPlayer() && unit->IsInWater( true ) )
                {
                    CustomSpellValues val;
                    val.AddSpellMod( SPELLVALUE_BASE_POINT0, 5000 );
                    val.AddSpellMod( SPELLVALUE_SPELL_RANGE, 120 );
                    me->CastCustomSpell( SPELL_HYDROMANCER_WATER_SPOUT, val, unit, TRIGGERED_FULL_MASK );
                }
            }

            func.Repeat( 5s );
        } );
    }

    void Reset() override
    {
        me->SetControlled(false, UNIT_STATE_STUNNED);
        BossAI::Reset();
        me->SetCanMissSpells(false);
        _secondScheduler.CancelAll();
        if (!me->IsSelectable())
            me->SetSelectable(true);

        //! automatically set to true if not mythic or keylevel not enough, will never happen
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
        me->SetDisplayId(me->GetNativeDisplayId());
        _halfHealthAction = false;
    }

    bool const IsValidOrbTarget(Unit* obj) const
    {
        return obj->IsPlayer();
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        BossAI::SummonedCreatureDies(summon, killer);

        if (!instance->IsMythic())
            return;

        if (summon->GetEntry() != NPC_GAHZRILLA_MYTHIC)
            return;

        _halfHealthAction = true;
        me->SetHealth(me->GetMaxHealth() * 0.5f);
        me->SetControlled(false, UNIT_STATE_STUNNED);
        me->RemoveAurasDueToSpell(SPELL_HYDROMANCER_ICEBLOCK);
        me->SetSelectable(true);
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_BASE_POINT1, 10);
        val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 20000);
        me->CastCustomSpell(64626, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        me->SetDisplayId(20162);
        _secondScheduler.CancelAll();
        scheduler.CancelAll();
        summons.DespawnAll();

        Creature* orb = me->SummonCreature(NPC_VELRATHA_FROZENORB_TRIGGER, { 1681.25f, 1202.54f, 11.6777f, 0.805314f });
        Creature* orb2 = me->SummonCreature(NPC_VELRATHA_FROZENORB_TRIGGER, { 1656.46f, 1178.52f, 12.5526f, 0.805314f });
        if (orb && orb2)
        {
            orb->setFaction(me->getFaction());
            orb2->setFaction(me->getFaction());
        }

        ScheduleCheckPeopleInWater();

        scheduler.Schedule(3s, [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, true, -SPELL_HYDROMANCER_WATER_TOMB_HIT);
            if (target)
            {
                CustomSpellValues circleVal;
                circleVal.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 750);
                me->CastCustomSpell(SPELL_HYDROMANCER_ELEMENTAL_FREEZING_CIRCLE, circleVal, target);

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 7500);
                me->CastCustomSpell(SPELL_HYDROMANCER_ELEMENTAL_FROSTBOLT, val, target, TRIGGERED_FULL_MASK);
            }

            func.Repeat(7s);
        });

        scheduler.Schedule(8s, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1500);
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 100000);
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3000);
            val.AddSpellMod(SPELLVALUE_TARGET_PLAYERS_ONLY, 1);
            val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
            me->CastCustomSpell(SPELL_HYDROMANCER_ELEMENTAL_WATERY_GRAVE, val, (Unit*)nullptr);
            ScheduleWaterTombs(7s);
            func.Repeat(25s);
        });

        scheduler.Schedule(2s, [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_HYDROMANCER_ELEMENTAL_FROSTBOLT, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 2000);
            val.AddSpellMod(SPELLVALUE_BASE_POINT1, -25);
            Unit* target = me->GetVictim();
            if (target)
                me->CastCustomSpell(SPELL_HYDROMANCER_ELEMENTAL_FROSTBOLT, val, target);

            func.Repeat(2s);
        });
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HYDROMANCER_WATER_TOMB_HIT);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 1693.369263f, 1214.453979f, 8.877023f, 0.783132f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void EnterEvadeMode() override
    {
        summons.DespawnAll();
        _DespawnAtEvade(10, me);
        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HYDROMANCER_WATER_TOMB_HIT);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_halfHealthAction && me->HealthBelowPctDamaged(50, damage))
        {
            _halfHealthAction = true;
            if (!instance->IsMythic())
            {
                scheduler.Schedule(Milliseconds(1000), [&](TaskContext func)
                {
                    DoCastSelf(SPELL_HYDROMANCER_HEALING_WAVE);
                    func.Repeat(8s);
                });
            }
            else
                UnleashTheGahzrilla();
        }
    }

    void SpellHitTarget(Unit* victim, SpellInfo const* spellInfo) override
    {
        if (victim && victim->IsPlayer() && spellInfo->Id == SPELL_HYDROMANCER_WATER_TOMB_HIT)
        {
            Position pos = victim->GetPosition();
            pos.m_positionZ = victim->GetMap()->GetHeight(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            Creature* tomb = me->SummonCreature(NPC_HYDROMANCER_WATER_TOMB, pos, TEMPSUMMON_CORPSE_DESPAWN);
            if (tomb && instance)
            {
                tomb->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
                tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS, true);
                tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS_DEST, true);

                tomb->SetObjectScale(victim->GetFloatValue(OBJECT_FIELD_SCALE_X));
                tomb->SetPassive();
                float variance = static_cast<float>(instance->GetMythicKeyLevel()) * 0.1f;
                uint32 val = Mythic::CalculateMythicValue(20000, variance);
                tomb->SetMaxHealth(val);
                tomb->SetFullHealth();
                if (tomb->IsAIEnabled)
                    tomb->AI()->SetGUID(victim->GetGUID(), 0);
            }
        }
    }

    void ScheduleWaterTombs(Seconds secs)
    {
        scheduler.Schedule(secs, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
            val.AddSpellMod(SPELLVALUE_MAX_TARGETS, 1);
            val.AddSpellMod(SPELLVALUE_BASE_POINT1,
                Mythic::CalculateMythicValue(2500, (0.04f * instance->GetMythicKeyLevel())));
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
            {
                return obj->IsPlayer() && me->GetDistance2d(obj) < 10.f;
            }))
                me->CastCustomSpell(SPELL_HYDROMANCER_WATER_TOMB_HIT, val, target, TRIGGERED_FULL_MASK);
        });
    }

    void EnterCombat(Unit* who) override
    {
        BossAI::EnterCombat(who);

        if (instance->IsMythic())
        {
            ScheduleCheckPeopleInWater();

            _secondScheduler.Schedule(2s, [&](TaskContext /*func*/)
            {
                Creature* orb = me->SummonCreature(NPC_VELRATHA_FROZENORB_TRIGGER, { 1669.88f, 1191.12f, 12.6122f, 3.95607f });
                if (orb)
                    orb->setFaction(me->getFaction());
            });

            scheduler.Schedule(10s, [&](TaskContext func)
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_HYDROMANCER_FROZEN_EMBRACE, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                me->CastCustomSpell(SPELL_HYDROMANCER_FROZEN_EMBRACE, val, me, TRIGGERED_FULL_MASK);

                CustomSpellValues val2;
                val2.AddSpellMod(SPELLVALUE_BASE_POINT0, 150000);
                val2.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 10000);
                val2.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                me->CastCustomSpell(SPELL_HYDROMANCER_FROSTBOLT_VOLLEY, val2, (Unit*)nullptr);
                func.Repeat(40s);

                //! hook it up to orb scheduler, so it expires after 10s
                //! normal scheduler is not updated during casts
                _secondScheduler.Schedule(10s, [&](TaskContext func)
                {
                    ScheduleWaterTombs(2s);
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                });
            });

            scheduler.Schedule(8s, [&](TaskContext func)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, [](Unit* obj)
                {
                    return obj->IsPlayer();
                }))
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3000);
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(3000, 0.03f));
                    me->CastCustomSpell(SPELL_HYDROMANCER_ORB_FROSTNOVA, val, target, TRIGGERED_FULL_MASK);
                }

                func.Repeat(22s);
            });
        }

        scheduler.Schedule(Milliseconds(urand(1000, 1100)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3400, 4700));
            if (!instance->IsMythic())
                DoCastVictim(SPELL_HYDROMANCER_WATER_BOLT);
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_HYDROMANCER_WATER_BOLT, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1250);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 45.f, true))
                    me->CastCustomSpell(SPELL_HYDROMANCER_WATER_BOLT, val, target);
                repeatTime = 2500ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _secondScheduler.Update(diff);
        scheduler.Update(diff);

        if (me->IsCasting())
            return;

        DoMeleeAttackIfReady();
    }
private:
    bool _halfHealthAction;
    TaskScheduler _secondScheduler;
};

constexpr uint32 SPELL_FROZEN_ORB_VISUAL_MYTHIC{ 72067 };
constexpr uint32 SPELL_FROZEN_ORB_MYTHIC_DAMAGE{ 72082 };
struct npc_velratha_frozen_orb_mythic : public ScriptedAI
{
    npc_velratha_frozen_orb_mythic(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
        if (!instance)
            me->DespawnOrUnsummon();

        me->SetWalk(true);
        me->SetDisableGravity(true);
        me->SetHover(true);
        me->SetCanMissSpells(false);
    }

    void Reset() override
    {
        me->SetSelectable(false);
        DoCastSelf(SPELL_FROZEN_ORB_VISUAL_MYTHIC, true);
        scheduler.CancelAll();
        ScheduleNextMove();
        scheduler.Schedule(1s, [&](TaskContext func)
        {
            CustomSpellValues val;
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FROZEN_ORB_MYTHIC_DAMAGE, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            me->CastCustomSpell(SPELL_FROZEN_ORB_MYTHIC_DAMAGE, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            func.Repeat(1s);
        });
    }

    void AttackStart(Unit* who) override { }
    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        damage = 0;
    }

    void ScheduleNextMove()
    {
        scheduler.Schedule(1s, [&](TaskContext func)
        {
            Unit* target = RequestVelrathaTarget();
            if (target)
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
                Position movePosition = target->GetPosition();
                movePosition.m_positionZ = me->GetMap()->GetWaterOrGroundLevel(me->GetPhaseMask(),
                    movePosition.m_positionX, movePosition.m_positionY, movePosition.m_positionZ);

                me->GetMotionMaster()->MovePoint(POINT_VELRATHA_FROZEN_ORB_MOVE, movePosition);
            }
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == POINT_VELRATHA_FROZEN_ORB_MOVE)
            ScheduleNextMove();
    }

    Unit* RequestVelrathaTarget()
    {
        Creature* velratha = me->GetSummoner();
        CreatureAI* ai = velratha ? velratha->AI() : nullptr;
        if (velratha && ai)
        {
            uint64 const targetGUID = ai->GetData64(DATA_VELRATHA_GET_MOVE_TARGET);
            Unit* followObject = ObjectAccessor::GetUnit(*me, targetGUID);
            return followObject;
        }
        return nullptr;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

private:
    TaskScheduler scheduler;
    InstanceScript* instance;
};

enum GahzrillaMisc
{
    SPELL_GAHZRILLA_SLAM = 11902,
    SPELL_GAHZRILLA_FREEZE_SOLID = 11836,
    SPELL_GAHZRILLA_ICICLE = 11131,
    SPELL_GAHZRILLA_MYTHIC_COLDFLAME = 69146,
    SPELL_GAHZRRILLA_MYTHIC_MASSIVE_GEYSER = 22421,
    GAHZRILLA_MOVE_POINT = 1
};

struct npc_velratha_water_tomb : public PassiveAI
{
    npc_velratha_water_tomb(Creature* creature) : PassiveAI(creature)
    {
        _playerGUID = 0;
        me->SetSelectable(false);
        me->SetDisplayId(11686);
    }

    void SetGUID(uint64 guid, int32 data) override
    {
        _playerGUID = guid;
        task.Schedule(1s, [&](TaskContext /*func*/)
        {
            Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
            if (player)
            {
                me->Relocate(player->GetPosition());
                me->SetDisplayId(DISPLAY_ID_WATER_GLOBULE);
                me->SetSelectable(true);
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
        if (!player)
            return;

        player->RemoveAurasDueToSpell(SPELL_HYDROMANCER_WATER_TOMB_HIT);
    }
private:
    TaskScheduler task;
    uint64 _playerGUID;
};

Position const GahzrillaMovePosition{ 1696.91f, 1216.6f, 8.88f, 0.0f };
struct npc_gahzrilla_mythic : public MythicAI
{
    npc_gahzrilla_mythic(Creature* creature) : MythicAI(creature)
    {
        me->SetImmuneToAll(true);
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

            me->SetCanDualWield(true);
            me->UpdateDamagePhysical(BASE_ATTACK);
            me->UpdateDamagePhysical(OFF_ATTACK);
        }
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        constexpr uint32 GAHZRILLA_PATH_POINTS_SIZE{ 5 };
        G3D::Vector3 const GahzrillaPositions[GAHZRILLA_PATH_POINTS_SIZE] =
        {
            { 1658.45f, 1179.45f, -2.1428f  },
            { 1675.01f, 1197.42f, 3.34366f  },
            { 1683.54f, 1204.61f, 6.47827f  },
            { 1686.00f, 1206.77f, 9.29035f  },
            { 1696.27f, 1216.57f, 8.87681f  }
        };

        Movement::MoveSplineInit init(me);
        for (G3D::Vector3 const& point : GahzrillaPositions)
            init.Path().push_back(point);

        init.SetWalk(true); // enables swimming client-side
        init.SetSmooth();
        uint32 splineDur = init.Launch();
        task.Schedule(Milliseconds(splineDur), [&](TaskContext func)
        {
            MovementInform(POINT_MOTION_TYPE, GAHZRILLA_MOVE_POINT);
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == GAHZRILLA_MOVE_POINT)
        {
            me->SetImmuneToAll(false);
            MythicDoZoneInCombat(me, 120.f);
        }
    }

    void EnterCombat(Unit* who) override
    {
        me->SetCanMissSpells(false);
        task.Schedule(Milliseconds(urand(1000, 10000)), [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_GAHZRILLA_SLAM, data));
            float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            val.AddSpellMod(SPELLVALUE_BASE_POINT1, 30); // just a little knockback, slightly annoying
            me->CastCustomSpell(SPELL_GAHZRILLA_SLAM, val, (Unit*)nullptr);
            func.Repeat(Milliseconds(urand(16000, 25000)));
        });

        task.Schedule(Milliseconds(urand(1000, 16000)), [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_GAHZRILLA_FREEZE_SOLID, data));
            float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3500);
            val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1000);

            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 30.f, true))
                me->CastCustomSpell(SPELL_GAHZRILLA_FREEZE_SOLID, val, target);
            func.Repeat(randseconds(9, 13));
        });

        task.Schedule(Milliseconds(urand(3000, 7000)), [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_GAHZRILLA_ICICLE, data));
            float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, -10);
            val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 100);
            val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 500);
            if (Unit* target = me->GetVictim())
                me->CastCustomSpell(SPELL_GAHZRILLA_ICICLE, val, target);
            func.Repeat(Milliseconds(urand(5000, 7000)));
        });

        task.Schedule(3s, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT2, 0);
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 5000);
            me->CastCustomSpell(SPELL_GAHZRILLA_MYTHIC_FROST_BREATH, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);

            float const singleStep = static_cast<float>(M_PI * 2.f) / 14.f;
            float const wholeCircle = static_cast<float>(M_PI * 2.f);
            float baseAngle = me->GetOrientation();
            float leftAngle = Position::NormalizeOrientation(baseAngle - singleStep);
            float rightAngle = Position::NormalizeOrientation(baseAngle + singleStep);

            for (float _angle : { baseAngle, leftAngle, rightAngle })
            {
                for (uint32 i = 0; i < 11; ++i)
                {
                    Position pos = me->GetPosition();

                    float const _dist = !i ? 0.f : static_cast<float>(i * 3.f);
                    float const x = pos.m_positionX;
                    float const y = pos.m_positionY;
                    pos.m_positionX = x + _dist * cos(_angle);
                    pos.m_positionY = y + _dist * sin(_angle);
                    pos.m_positionZ = me->GetMap()->GetHeight(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                    Trinity::NormalizeMapCoord(pos.m_positionX);
                    Trinity::NormalizeMapCoord(pos.m_positionY);

                    Creature* summoner = me->GetSummoner();
                    CreatureAI* summonerAI = summoner ? summoner->AI() : nullptr;
                    Creature* trigger = me->SummonCreature(WORLD_TRIGGER, pos);
                    if (trigger)
                    {
                        trigger->setFaction(me->getFaction());
                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_AURA_DURATION, 90000);
                        trigger->CastCustomSpell(SPELL_GAHZRILLA_MYTHIC_COLDFLAME, val, (Unit*)nullptr, TRIGGERED_FULL_MASK,
                            NullItemRef, (const AuraEffect*)nullptr, me->GetGUID());
                        summonerAI->JustSummoned(trigger);
                        trigger->DespawnOrUnsummon(90s);
                    }
                }
            }

            func.Repeat(randseconds(10, 18));
        });

    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct npc_gahzrilla : public BossAI
{
    npc_gahzrilla(Creature* creature) : BossAI(creature, BOSS_GAHZRILLA)
    {
        me->SetImmuneToAll(true);
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        me->GetMotionMaster()->MovePoint(GAHZRILLA_MOVE_POINT, GahzrillaMovePosition, false, true);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == GAHZRILLA_MOVE_POINT)
        {
            me->SetImmuneToAll(false);
            DoZoneInCombat();
        }
    }

    void EnterCombat(Unit* who) override
    {
        BossAI::EnterCombat(who);
        scheduler.Schedule(randseconds(1, 4), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(16000, 25000));
            DoCastAOE(SPELL_GAHZRILLA_SLAM);

            func.Repeat(randseconds(16, 25));
        });

        scheduler.Schedule(15s, [&](TaskContext func)
        {
            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 40.f, true);
            if (target)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 2000);
                me->CastCustomSpell(SPELL_GAHZRRILLA_MYTHIC_MASSIVE_GEYSER, val, target);
            }

            func.Repeat(15s);
        });

        scheduler.Schedule(randseconds(1, 16), [&](TaskContext func)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, true, -SPELL_HYDROMANCER_WATER_TOMB_HIT))
                DoCast(target, SPELL_GAHZRILLA_FREEZE_SOLID);

            func.Repeat(randseconds(15, 25));
        });

        scheduler.Schedule(Milliseconds(urand(3000, 7000)), [&](TaskContext func)
        {
            DoCastVictim(SPELL_GAHZRILLA_ICICLE);
            func.Repeat(randseconds(7, 13));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff, std::bind(&BossAI::DoMeleeAttackIfReady, this));
    }
};

class spell_frozen_orb_damage_mythic : public SpellScript
{
    PrepareSpellScript(spell_frozen_orb_damage_mythic);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->IsMythic())
        targets.remove_if([](WorldObject* obj)
        {
            return obj->IsPlayer() && obj->ToPlayer()->HasAura(SPELL_HYDROMANCER_WATER_TOMB_HIT);
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_frozen_orb_damage_mythic::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

constexpr uint32 SPELL_MASSIVE_GEYSER_WATER{ 22422 };
struct npc_massive_geyser_trigger : public PassiveAI
{
    npc_massive_geyser_trigger(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        task.Schedule(1s, [&](TaskContext /*func*/)
        {
            InstanceScript* instance = me->GetInstanceScript();
            if (instance && instance->IsMythic())
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 6000);
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 6000);
                me->CastCustomSpell(SPELL_MASSIVE_GEYSER_WATER, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            }
            else
                DoCastAOE(SPELL_MASSIVE_GEYSER_WATER, true);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }
private:
    TaskScheduler task;
};

class spell_frozen_embrace_remove : public AuraScript
{
    PrepareAuraScript(spell_frozen_embrace_remove);

    void OnAurRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
            return;

        Unit* target = GetTarget();
        if (!target)
            return;

        InstanceScript* instance = target->GetInstanceScript();
        if (!instance)
            return;

        if (!instance->IsMythic())
            return;

        target->InterruptNonMeleeSpells(false);
        target->MonsterTextEmote("The frozen embrace has been shattered!", nullptr, true);
        if (target->IsCreature())
            target->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_frozen_embrace_remove::OnAurRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB,
            AURA_EFFECT_HANDLE_REAL);
    }
};

constexpr uint32 SPELL_EXECTUIONER_CLEAVE{ 15496 };
constexpr uint32 SPELL_EXECUTIONER_BERSERK_STANCE{ 7366 };
constexpr uint32 TALK_SANDFURY_EXECTUIONER_AGGRO{ 0 };
constexpr uint32 TALK_SANDFURY_EXECTUIONER_KILLUNIT{ 1 };
struct npc_sandfury_executioner : public BossAI
{
    npc_sandfury_executioner(Creature* creature) : BossAI(creature, BOSS_SANDFURY_EXECUTIONER) { }

    void KilledUnit(Unit* /*who*/) override
    {
        Talk(TALK_SANDFURY_EXECTUIONER_KILLUNIT);
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        if (instance && instance->IsMythic())
        {
            me->MonsterTextEmote("Cages do not require a key. Click on them to begin the event.", nullptr, true);
        }
    }

    void EnterCombat(Unit* who) override
    {
        if ( !_EnterCombat() )
            return;

        BossAI::EnterCombat(who);
        Talk(TALK_SANDFURY_EXECTUIONER_AGGRO);

        scheduler.Schedule(Milliseconds(urand(3000, 5000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 11000));
            if (!instance->IsMythic())
            {
                DoCastVictim(SPELL_EXECTUIONER_CLEAVE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_EXECTUIONER_CLEAVE, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_EXECTUIONER_CLEAVE, val, target);
                repeatTime = Milliseconds(5000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        scheduler.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(0);
            if (!instance->IsMythic())
            {
                DoCastSelf(SPELL_EXECUTIONER_BERSERK_STANCE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_EXECUTIONER_BERSERK_STANCE, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                me->CastCustomSpell(SPELL_EXECUTIONER_BERSERK_STANCE, val, me, TRIGGERED_FULL_MASK);
                repeatTime = Milliseconds(0);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff, std::bind(&BossAI::DoMeleeAttackIfReady, this));
    }
};

enum RuuzluMisc
{
    SPELL_RUUZLU_HITTIN_YA = 71971,
    SPELL_RUUZLU_HITTIN_YA_TRIGGERED = 71972,
    SPELL_RUUZLU_EXECUTE = 38959,
    SPELL_RUUZLU_BERSERKER_STANCE = 7366,
    SPELL_RUUZLU_WEAPON_THROW = 53240,
    SPELL_RUUZLU_HURRICANE_DMG = 63272,
    SPELL_RUUZLU_INVISIBILITY = 50013,
    SPELL_RUUZLU_ENRAGE = 78722,
    SPELL_RUUZLU_TORNADO_TRIGGERED = 25160,
    SPELL_RUUZLU_FINGER_OF_DEATH = 31984,
    VISUAL_RUUZLU_WEAPON_ID = 5288,
    POINT_RUUZLU_GONG = 100,
    NPC_IMPALING_AXE_TRIGGER = 91511,
    ACTION_UKORZ_REACTIVATE,
    SPELL_RUUZLU_WHIRLWIND_HIT = 63805,
    SPELL_RUUZLU_DEEP_WOUND_HIT = 59826
};

enum TornadoData
{
    DATA_TORNADO_MOVE_LEFT = 1,
    DATA_TORNADO_MOVE_RIGHT = 2,
    NPC_TORNADO_MYTHIC_RUUZLU = 19922
};

Position const RuuzluRightTornado{ 1725.69f, 1060.21f, 54.1366f, 4.16971f };
Position const RuuzluLeftTornado{ 1768.67f, 1024.54f, 53.8435f, 3.93802f };
Position const RuuzluGongMovePosition{ 1728.30f, 1032.14f, 53.2916f, 3.19031f };

constexpr uint32 RIGHT_TORNADO_PATH_SIZE{ 11 };
G3D::Vector3 RightTornadoPath[RIGHT_TORNADO_PATH_SIZE] =
{
    { 1732.92f, 1068.75f, 53.5735f },
    { 1740.80f, 1069.67f, 52.6638f },
    { 1759.22f, 1053.98f, 47.7380f },
    { 1774.08f, 1039.79f, 52.3377f },
    { 1775.99f, 1031.53f, 53.4920f },
    { 1769.49f, 1025.76f, 54.0010f },
    { 1756.66f, 1013.22f, 53.4864f },
    { 1737.69f, 1026.48f, 52.8229f },
    { 1731.13f, 1031.54f, 53.0127f },
    { 1716.57f, 1042.85f, 53.4882f },
    { 1724.42f, 1059.02f, 53.9507f }
};

constexpr uint32 LEFT_TORNADO_PATH_SIZE{ 13 };
G3D::Vector3 LeftTornadoPath[LEFT_TORNADO_PATH_SIZE] =
{
    { 1757.67f, 1013.30f, 53.4867f },
    { 1756.34f, 1013.75f, 53.4867f },
    { 1751.28f, 1017.46f, 53.4318f },
    { 1740.51f, 1025.84f, 52.7323f },
    { 1730.58f, 1033.57f, 52.9512f },
    { 1719.79f, 1041.96f, 53.5900f },
    { 1715.84f, 1050.50f, 53.4883f },
    { 1727.32f, 1061.25f, 54.4757f },
    { 1736.70f, 1069.65f, 53.2516f },
    { 1753.70f, 1057.20f, 48.0307f },
    { 1771.39f, 1041.85f, 51.2421f },
    { 1775.12f, 1031.50f, 53.2452f },
    { 1771.67f, 1027.99f, 54.0032f }
};

struct npc_zulfarrak_ruuzlu : public BossAI
{
    npc_zulfarrak_ruuzlu(Creature* creature) : BossAI(creature, BOSS_RUUZLU)
    {
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
        }
    }

    void Reset() override
    {
        if (instance->IsMythic())
        {
            if (AmIVisible())
                DoCastSelf(SPELL_RUUZLU_INVISIBILITY);

            me->SetPassive();
        }

        me->SetRegeneratingHealth(true);
        _chargePhase = false;
        _mythicIntermission = false;
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, VISUAL_RUUZLU_WEAPON_ID);
        me->SetCanMissSpells(false);
        BossAI::Reset();
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        Position const pos{ 1742.30f, 1037.83f, 50.4377f, 4.01251f };
        return me->GetDistance(pos) > 45.f;
    }

    void EnterEvadeMode() override
    {
        instance->SetBossState(BOSS_RUUZLU, FAIL);
        BossAI::EnterEvadeMode();
    }

    void JustDied(Unit* killer) override
    {
        if (instance->IsMythic())
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        summons.DespawnAll();
        if (instance)
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RUUZLU_KNEEL);

        BossAI::JustDied(killer);
    }

    bool const AmIVisible() const
    {
        return !me->HasAura(SPELL_RUUZLU_INVISIBILITY);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!AmIVisible())
            return;

        BossAI::MoveInLineOfSight(who);
        if (who && who->GetEntry() == NPC_IMPALING_AXE_TRIGGER && who->GetDistance(me) <= 2.f)
        {
            me->RemoveAurasDueToSpell(SPELL_RUUZLU_ENRAGE);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RUUZLU_KNEEL);
            who->ToCreature()->DespawnOrUnsummon();
            summons.DespawnEntry(NPC_IMPALING_AXE_TRIGGER);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, VISUAL_RUUZLU_WEAPON_ID);
        }
    }

    void DoAction(int32 id) override
    {
        if (id == ACTION_DESPAWN_SUMMONS)
            summons.DespawnAll();
    }

    void AttackStart(Unit* who) override
    {
        if (!AmIVisible())
            return;

        BossAI::AttackStart(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!AmIVisible() || me->isDead())
            return;

        scheduler.Update(diff);
        BossAI::UpdateAI(diff);
        EnterEvadeIfOutOfCombatArea();
    }

    void EnterCombat(Unit* who) override
    {
        if (!me->IsVisible())
            return;

        BossAI::EnterCombat(who);

        if (!instance->IsMythic())
        {
            DoCastVictim(SPELL_RUUZLU_BERSERKER_STANCE);

            scheduler.Schedule(Milliseconds(1000), [&](TaskContext func)
            {
                Unit* target = me->GetVictim();
                if (target && target->HealthBelowPct(20))
                    DoCastVictim(SPELL_RUUZLU_EXECUTE);

                func.Repeat(Seconds(urand(5, 13)));
            });

            scheduler.Schedule(Milliseconds(3000), [&](TaskContext func)
            {
                DoCastVictim(SPELL_EXECTUIONER_CLEAVE);

                func.Repeat(Seconds(urand(7, 11)));
            });
        }
        else
        {
            DoCastSelf(SPELL_RUUZLU_HITTIN_YA, true);
            scheduler.Schedule(5s, [&](TaskContext func)
            {
                if (_chargePhase)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 60.f, true, -SPELL_RUUZLU_KNEEL))
                    {
                        float const height = me->GetMap()->GetHeight(target->GetPositionX(), target->GetPositionY(),
                            target->GetPositionZ());
                        me->GetMotionMaster()->MoveCharge(target->GetPositionX(),
                            target->GetPositionY(), height);

                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_AURA_DURATION, 5000);
                        me->CastCustomSpell(SPELL_RUUZLU_TORNADO, val, target, TRIGGERED_FULL_MASK);

                        CustomSpellValues val2;
                        val2.AddSpellMod(SPELLVALUE_BASE_POINT0, 10000);
                        val2.AddSpellMod(SPELLVALUE_AURA_DURATION, 2 * MINUTE * IN_MILLISECONDS);
                        me->CastCustomSpell(SPELL_RUUZLU_HURRICANE_DMG, val2, target, TRIGGERED_FULL_MASK);
                    }
                }
                else
                    ThrowWeaponAndDisarmSelf();

                _chargePhase = !_chargePhase;

                func.Repeat(20s);
            });

            scheduler.Schedule(10s, [&](TaskContext func)
            {
                Unit* target = me->GetVictim();
                if (target && me->IsWithinCombatRange(target, 0.f) && me->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) != 0)
                {
                    constexpr uint32 SPELL_RUUZLU_WHIRLWIND_AURA{ 63806 };
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_MODIFY_CAST_TIME, 1000);
                    me->CastCustomSpell(SPELL_RUUZLU_WHIRLWIND_AURA, val, me);
                    func.Repeat(randseconds(11, 16) + 4s /*auraduration*/);
                }
                else
                    func.Repeat(10s);
            });
        }
    }

    void SpellHitTarget(Unit* victim, SpellInfo const* spell) override
    {
        if (!victim || !victim->IsPlayer())
            return;

        if (spell->Id == SPELL_RUUZLU_WEAPON_THROW)
        {
            me->CastSpell(victim, SPELL_RUUZLU_KNEEL, true);
            me->MonsterTextEmote("Ruuzlu loses his weapon!", nullptr, true);
        }
        else if (spell->Id == SPELL_RUUZLU_WHIRLWIND_HIT)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
            val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 100.f);
            me->CastCustomSpell(SPELL_RUUZLU_DEEP_WOUND_HIT, val, victim, TRIGGERED_FULL_MASK);
        }
    }

    void JustSummoned(Creature* summon) override { summons.Summon(summon); }

    void ThrowWeaponAndDisarmSelf()
    {
        summons.DespawnEntry(NPC_IMPALING_AXE_TRIGGER);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, VISUAL_RUUZLU_WEAPON_ID);

        //! slightly delay so we do not have weapon throw displayed as cube
        scheduler.Schedule(1s, [&](TaskContext /*func*/)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_MAX_TARGETS, 5);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 15000);
            val.AddSpellMod(SPELLVALUE_TARGET_PLAYERS_ONLY, 1);

            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RUUZLU_KNEEL);
            me->CastCustomSpell(SPELL_RUUZLU_WEAPON_THROW, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!attacker)
            return;

        if (attacker && attacker->IsPlayer() && attacker->GetPositionZ() > 59.f)
        {
            damage = 0;
            Unit::Kill(me, attacker);
        }

        if (instance->IsMythic() && !_mythicIntermission && me->HealthBelowPctDamaged(20, damage))
        {
            _mythicIntermission = true;
            me->AttackStop();
            me->GetMotionMaster()->Clear();
            Creature* ukorz = instance->GetCreature(BOSS_UKORZ_SANDSCALP);
            if (ukorz)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, me->GetMaxHealth());
                ukorz->CastCustomSpell(SPELL_RUUZLU_FINGER_OF_DEATH, val, me, TRIGGERED_FULL_MASK);
                summons.DespawnEntry(NPC_IMPALING_AXE_TRIGGER);
                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RUUZLU_KNEEL);
                ukorz->AI()->DoAction(ACTION_UKORZ_REACTIVATE);
            }
        }
    }
private:
    bool _mythicIntermission;
    bool _chargePhase;
};

class spell_ruuzlu_hurricane_fiter : public AuraScript
{
    PrepareAuraScript(spell_ruuzlu_hurricane_fiter);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (GetTarget()->HasAura(SPELL_RUUZLU_TORNADO))
            PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_ruuzlu_hurricane_fiter::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

constexpr uint32 SPELL_RUUZLU_TORNADO_AURA{ 34683 };
struct npc_tornado_ruuzlu_mythic : public PassiveAI
{
    npc_tornado_ruuzlu_mythic(Creature* creature) : PassiveAI(creature)
    {
        float const currentSpeed = me->GetSpeedRate(MOVE_WALK);
        me->SetSpeedRate(MOVE_WALK, currentSpeed / 2.f);
    }

    void Reset() override
    {
        DoCastSelf(SPELL_RUUZLU_TORNADO_AURA, true);
        me->SetWalk(true);
        task.CancelAll();
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == DATA_TORNADO_MOVE_LEFT)
        {
            for (G3D::Vector3 const& point : RightTornadoPath)
                path.push_back(point);
        }
        else if (type == DATA_TORNADO_MOVE_RIGHT)
        {
            for (G3D::Vector3 const& point : LeftTornadoPath)
                path.push_back(point);
        }

        task.Schedule(1s, [&](TaskContext func)
        {
            Movement::MoveSplineInit init(me);
            init.MovebyPath(path);
            init.SetSmooth();
            func.Repeat(Milliseconds(init.Launch()));
        });

    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }
private:
    TaskScheduler task;
    Movement::PointsArray path;
};

enum ChiefUkorz
{
    SPELL_URKOZ_FRENZY = 8269,
    SPELL_URKOZ_BLADE_FLURRY = 51211,
    SPELL_URKOZ_FELFIRE_FISSION = 45779,
    SPELL_URKOZ_FIRE_BREATH = 59197,

    GO_UKORZ_SECOND_GONG = 187359,
    SPELL_UKORZ_BANG_THE_GONG = 45226,
    SPELL_UKORZ_BANG_THE_GONG_EFFECT = 45226,
    SPELL_UKORZ_BLACK_BEAM = 46015,
    NPC_UKORZ_BLACK_BEAM_TRIGGER = 91508,
    NPC_UKORZ_FIRE_TRIGGER = 91509,
    NPC_UKORZ_BLACK_BEAM_CASTER = 91510,
    SPELL_UKORZ_FIRE_SPIT = 66796,
    SPELL_UKORZ_PLASMA_BLAST = 62997,
    NPC_ZULFARRAK_ZOMBIE_FIXATE = 91707,

    SPELL_UKORZ_FLAME_CRASH = 40836,
    SPELL_UKORZ_AGONIZING_FLAMES = 40932,

    TALK_URKOZ_FRENZY = 1,
    TALK_URKOZ_AGGRO = 2,
    TALK_URKOZ_KILL_UNIT = 3,

    DATA_UKORZ_SET_EXPLOSION_TIME,
    DATA_UKORZ_SET_EXPLOSION_DESPAWN_TIME
};

Position const ChiefUkorzGongPosition{ 1725.29f, 1030.48f, 53.4266f, 1.3352f };
Position const ChiefUkorzRightSnakeSpawnPosition{ 1741.40f, 1010.26f, 53.4882f, 1.52705f };
Position const ChiefUkorzRightSnakeEndPosition{ 1736.40f, 1092.91f, 69.3054f, 4.69771f };
Position const ChiefUkorzLeftSnakeSpawnPosition{ 1714.54f, 1031.99f, 53.4882f, 0.158894f };
Position const ChiefUkorzLeftSnakeEndPosition{ 1794.50f, 1041.32f, 71.0904f, 0.0434401f };
constexpr uint32 UKORZ_SIDE_STATUES_SIZE{ 4 };
Position const _statuePositions[UKORZ_SIDE_STATUES_SIZE] =
{
    { 1760.99f, 1004.88f, 55.4953f, 2.17164f },
    { 1773.60f, 1020.91f, 55.7682f, 2.40019f },
    { 1721.41f, 1062.74f, 54.8785f, 5.59126f },
    { 1708.95f, 1046.91f, 54.7672f, 5.62739f }
};

std::vector<Position> _zombiePositions
{
    { 1755.909f, 1033.094f, 48.924f, 1.939f },
    { 1758.939f, 1036.549f, 48.932f, 2.561f },
    { 1735.022f, 1051.166f, 49.045f, 5.524f }
};

Position const RightBlackBeamTriggerSpawnPosition{ 1741.52f, 1005.04f, 85.7388f, 0.0f };
Position const LeftBlackBeamTriggerSpawnPosition{ 1708.094f, 1029.62f, 85.7388f, 0.0f };
constexpr uint32 ITEM_FISTS_OF_MOLTEN_FURY_MH{ 32945 };
constexpr uint32 ITEM_CLAW_OF_MOLTEN_FURY_OH{ 32946 };
struct npc_chief_ukorz_sandscalp : public BossAI
{
    npc_chief_ukorz_sandscalp(Creature* creature) : BossAI(creature, BOSS_UKORZ_SANDSCALP)
    {
        _entranceGateGUID = 0;
        _rightSnakeGUID = 0;
        _leftSnakeGUID = 0;
        _gongGUID = 0;
        _successfulMeleeHitCount = 0;
        _enableAgonizingFlames = false;
        _statues.clear();
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

            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, ITEM_FISTS_OF_MOLTEN_FURY_MH);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, ITEM_CLAW_OF_MOLTEN_FURY_OH);
        }
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        Position const pos{ 1742.30f, 1037.83f, 50.4377f, 4.01251f };
        return me->GetDistance(pos) > 45.f;
    }

    void DoAction(int32 id) override
    {
        if (id == ACTION_DESPAWN_SUMMONS)
            summons.DespawnAll();
        else if (id == ACTION_UKORZ_REACTIVATE)
        {
            me->SetHealth(me->GetMaxHealth() * 0.2f);
            me->SetAggressive();
            me->SetSelectable(true);
            DoZoneInCombat(me, 150.f);
            ScheduleEvents();
            if (instance->GetMythicKeyLevel() >= Mythic::MYTHIC_SECRET_PHASE_KEY_LEVEL)
            {
                _enableAgonizingFlames = true;
                _successfulMeleeHitCount = 0;
            }
        }
    }

    void JustDied(Unit* killer) override
    {
        if (instance->IsMythic())
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        BossAI::JustDied(killer);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 1732.513306f, 1025.945435f, 53.224598f, 0.904855f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void EnterEvadeMode() override
    {
        instance->SetBossState(BOSS_UKORZ_SANDSCALP, FAIL);
        instance->SetBossState(BOSS_RUUZLU, FAIL);
        BossAI::EnterEvadeMode();
    }

    void OnMeleeAttack(VictimState state, WeaponAttackType type, Unit* victim, uint32 /*procAttacker*/) override
    {
        if ( !instance->IsMythic() )
            return;

        bool const validAttacck = state == VICTIMSTATE_HIT || state == VICTIMSTATE_DODGE || state == VICTIMSTATE_PARRY || state == VICTIMSTATE_BLOCKS;
        if (_enableAgonizingFlames && validAttacck && type == BASE_ATTACK && victim)
        {
            if (++_successfulMeleeHitCount >= 2)
            {
                _successfulMeleeHitCount = 0;
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 100);
                float const variance = 0.04 * instance->GetMythicKeyLevel();
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(7500, variance));
                me->CastCustomSpell(SPELL_UKORZ_AGONIZING_FLAMES, val, victim, TRIGGERED_FULL_MASK);

                Position pos = victim->GetPosition();
                pos.m_positionZ = me->GetMap()->GetHeight(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                Creature* flame = me->SummonCreature(WORLD_TRIGGER, pos);
                if (flame)
                {
                    flame->CastSpell(flame, SPELL_UKORZ_FLAME_CRASH);
                    flame->setFaction(me->getFaction());
                    flame->DespawnOrUnsummon(300s);
                }

            }
        }
    }

    void Reset() override
    {
        BossAI::Reset();

        if ( GameObject* gate = ObjectAccessor::GetGameObject( *me, _entranceGateGUID ) )
            gate->RemoveFromWorld();

        _entranceGateGUID = 0;
        me->SetCanDualWield(true);
        me->UpdateDamagePhysical(BASE_ATTACK);
        me->UpdateDamagePhysical(OFF_ATTACK);
        me->SetMeleeDamageSchool(SpellSchools::SPELL_SCHOOL_FIRE);
        //! automatically set to true if map is not mythic, so it doesnt trigger
        _mythicFirstIntermission = instance->IsMythic() ? false : true;
        _successfulMeleeHitCount = 0;
        _statues.clear();
        me->SetSelectable(true);
        _enableAgonizingFlames = false;
        if (instance->IsMythic())
        {
            if (!_gongGUID)
            {
                GameObject* gong = me->SummonGameObject(GO_UKORZ_SECOND_GONG, ChiefUkorzGongPosition, 0.f,
                    0.f, 0.f, 0.f, 0);

                if (gong)
                    _gongGUID = gong->GetGUID();
            }

            //! move below to IsMythic check, moved out during tes
            Creature* snake1 = me->SummonCreature(NPC_UKORZ_BLACK_BEAM_CASTER, RightBlackBeamTriggerSpawnPosition);
            if (snake1)
                _rightSnakeGUID = snake1->GetGUID();

            Creature* snake2 = me->SummonCreature(NPC_UKORZ_BLACK_BEAM_CASTER, LeftBlackBeamTriggerSpawnPosition);
            if (snake2)
                _leftSnakeGUID = snake2->GetGUID();

            for (Position const& pos : _statuePositions)
            {
                Creature* statueTrigger = me->SummonCreature(NPC_UKORZ_BLACK_BEAM_CASTER, pos);
                if (statueTrigger)
                    _statues.push_back(statueTrigger->GetGUID());
            }
        }
    }

    void HandleZombies(bool first)
    {
        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 100.f, true))
        {
            Creature* zombie = me->SummonCreature(NPC_ZULFARRAK_ZOMBIE_FIXATE, _zombiePositions[urand(0, 2)], TEMPSUMMON_CORPSE_DESPAWN);
            if (zombie && zombie->IsAIEnabled)
            {
                zombie->setFaction(me->getFaction());
                zombie->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                zombie->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                zombie->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                zombie->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                zombie->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                zombie->AI()->DoZoneInCombat(zombie, 100.f);
                zombie->AddThreat(target, 1000000.f);
            }
        }
    }

    void ScheduleZombies()
    {
        scheduler.Schedule(10s, [&](TaskContext func)
        {
            bool const _firstTime = func.GetRepeatCounter() == 0;
            HandleZombies(_firstTime);
            func.Repeat(20s);
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (attacker && attacker->IsPlayer() && attacker->GetPositionZ() > 59.f)
        {
            damage = 0;
            Unit::Kill(me, attacker);
        }

        if (!instance->IsMythic() && !_frenzy && me->HealthBelowPctDamaged(30, damage))
        {
            _frenzy = true;
            DoCastVictim(SPELL_URKOZ_FRENZY);
        }

        if (instance->IsMythic() && !_mythicFirstIntermission && me->HealthBelowPctDamaged(50, damage))
        {
            _mythicFirstIntermission = true;
            Creature* ruuzlu = instance->GetCreature(BOSS_RUUZLU);
            if (ruuzlu)
            {
                me->GetMotionMaster()->Clear();
                me->SetPassive();
                me->AttackStop();
                scheduler.CancelAll();
                ScheduleZombies();

                float x, y, z;
                ruuzlu->GetClosePoint(x, y, z, ruuzlu->GetObjectSize() / 6, 1.5f);
                Position const pos{ x,y,z };
                me->SetSelectable(false);
                me->GetMotionMaster()->MovePoint(POINT_RUUZLU_GONG, pos);
            }
        }
    }

    void KilledUnit(Unit* who) override
    {
        if (roll_chance_i(33))
            Talk(TALK_URKOZ_KILL_UNIT);

        BossAI::KilledUnit(who);
    }

    void MoveRuuzluToGong()
    {
        Creature* ruz = instance->GetCreature(BOSS_RUUZLU);
        if (!ruz)
            return;

        ruz->SetImmuneToAll(true);
        ruz->SetPassive();
        ruz->SetSelectable(false);
        ruz->GetMotionMaster()->MovePoint(POINT_RUUZLU_GONG, RuuzluGongMovePosition);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == POINT_RUUZLU_GONG)
        {
            DoCastAOE(SPELL_UKORZ_BANG_THE_GONG);
            DoCastAOE(SPELL_UKORZ_BANG_THE_GONG_EFFECT);

            scheduler.Schedule(2s, [&](TaskContext /*func*/)
            {
                Creature* ruuzlu = instance->GetCreature(BOSS_RUUZLU);
                if (ruuzlu)
                {
                    ruuzlu->RemoveAurasDueToSpell(SPELL_RUUZLU_INVISIBILITY);
                    ruuzlu->SetVisible(true);
                    ruuzlu->SetSelectable(true);
                    ruuzlu->SetAggressive();
                    ruuzlu->SetImmuneToAll(false);
                    ruuzlu->SetRegeneratingHealth(false);
                    ruuzlu->SetHealth(ruuzlu->GetMaxHealth() / 2);
                    ruuzlu->AI()->DoZoneInCombat(me, 150.f);
                    if (Unit* target = me->SelectNearestPlayer(100.f))
                        ruuzlu->AI()->AttackStart(target);
                    else
                    {
                        ruuzlu->AI()->EnterEvadeMode();
                        EnterEvadeMode();
                    }
                }
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);
        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea();
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_UKORZ_BLACK_BEAM_TRIGGER)
            summons.Summon(summon);
        else
            BossAI::JustSummoned(summon);
    }

    void HandleFireBeams()
    {
        Unit* target = me->GetVictim();
        if (!target)
            return;

        Creature* beamCaster = ObjectAccessor::GetCreature(*me, _rightSnakeGUID);
        Creature* secondBeamCaster = ObjectAccessor::GetCreature(*me, _leftSnakeGUID);

        Creature* beamTrigger = target->SummonCreature(NPC_UKORZ_BLACK_BEAM_TRIGGER,
            ChiefUkorzRightSnakeSpawnPosition);
        Creature* secondBeamTrigger = target->SummonCreature(NPC_UKORZ_BLACK_BEAM_TRIGGER,
            ChiefUkorzLeftSnakeSpawnPosition);

        if (beamTrigger && beamCaster && secondBeamCaster && secondBeamTrigger)
        {
            if (_rightSidePath.empty())
                FillPath(false);

            if (_leftSidePath.empty())
                FillPath(true);

            beamTrigger->SetPassive();
            beamTrigger->SetCanFly(true);
            secondBeamTrigger->SetPassive();
            secondBeamTrigger->SetCanFly(true);
            beamTrigger->setFaction(target->getFaction());
            secondBeamTrigger->setFaction(target->getFaction());

            beamTrigger->SetMaxHealth(1000000);
            secondBeamTrigger->SetMaxHealth(1000000);
            beamTrigger->SetFullHealth();
            secondBeamTrigger->SetFullHealth();

            beamCaster->SetCanMissSpells(false);
            secondBeamCaster->SetCanMissSpells(false);
            beamCaster->CastSpell(beamTrigger, SPELL_UKORZ_PLASMA_BLAST, true);
            secondBeamCaster->CastSpell(secondBeamTrigger, SPELL_UKORZ_PLASMA_BLAST, true);

            bool _handlingLeftSide = false;
            for (uint32 i = 0; i < 2; ++i)
            {
                Movement::MoveSplineInit init(_handlingLeftSide ? secondBeamTrigger : beamTrigger);
                std::vector<Position> const& ref = _handlingLeftSide ? _leftSidePath : _rightSidePath;
                for (Position const& pos : ref)
                    init.Path().push_back(G3D::Vector3{ pos.m_positionX, pos.m_positionY, pos.m_positionZ });

                init.SetVelocity(20.f);
                int32 totalDuration = init.Launch();
                size_t steps = init.Path().size();

                for (size_t curStep = 0; curStep < steps; ++curStep)
                {
                    int32 stepTime = (totalDuration / steps);
                    int32 executionTime = (totalDuration / (steps - curStep));
                    Position pos = _handlingLeftSide ? _leftSidePath[curStep] : _rightSidePath[curStep];
                    if (Creature* trigger = me->SummonCreature(NPC_UKORZ_FIRE_TRIGGER, pos))
                    {
                        trigger->AI()->SetData(DATA_UKORZ_SET_EXPLOSION_TIME, stepTime * curStep);
                        trigger->AI()->SetData(DATA_UKORZ_SET_EXPLOSION_DESPAWN_TIME, 7500);
                    }
                }

                if (_handlingLeftSide)
                    secondBeamTrigger->DespawnOrUnsummon(totalDuration + int32(1500));
                else
                    beamTrigger->DespawnOrUnsummon(totalDuration + int32(1500));

                _handlingLeftSide = true;
            }
        }
    }

    void HandleStatueBeam()
    {
        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 100.f, true);
        if (target)
        {
            Position triggerPos = target->GetPosition();
            triggerPos.m_positionZ = me->GetMap()->GetHeight(triggerPos.m_positionX, triggerPos.m_positionY, triggerPos.m_positionZ);
            Creature* trigger = me->SummonCreature(NPC_UKORZ_FIRE_TRIGGER, triggerPos);
            if (!trigger)
                return;

            trigger->SetCanMissSpells(false);
            trigger->SetMaxHealth(1000000);
            trigger->SetFullHealth();

            uint64 const guid = Trinity::Containers::SelectRandomContainerElement(_statues);
            Creature* caster = ObjectAccessor::GetCreature(*me, guid);
            if (!caster)
                return;

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3500);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(2500,
                static_cast<float>(0.10f * instance->GetMythicKeyLevel())));
            caster->CastCustomSpell(SPELL_UKORZ_PLASMA_BLAST, val, target, TRIGGERED_FULL_MASK);
            if (trigger->IsAIEnabled)
            {
                trigger->AI()->SetGUID(target->GetGUID(), 0);
                trigger->AI()->SetData(DATA_UKORZ_SET_EXPLOSION_TIME, 3500);
                trigger->AI()->SetData(DATA_UKORZ_SET_EXPLOSION_DESPAWN_TIME, 2 * MINUTE * IN_MILLISECONDS);
            }
        }
    }

    void FillPath(bool leftSide)
    {
        // build trigger path
        Position const EndPosition = leftSide ? ChiefUkorzLeftSnakeEndPosition :
            ChiefUkorzRightSnakeEndPosition;
        Position const StartPosition = leftSide ? ChiefUkorzLeftSnakeSpawnPosition :
            ChiefUkorzRightSnakeSpawnPosition;

        float const _angle = StartPosition.GetAngle(&EndPosition);
        float const _step = 6.5f;
        float const _distToTrigger = StartPosition.GetExactDist2d(&EndPosition);

        for (float dist = 0.f; dist < _distToTrigger; dist += _step)
        {
            G3D::Vector3 point;
            point.x = StartPosition.GetPositionX() + dist * cosf(_angle);
            point.y = StartPosition.GetPositionY() + dist * sinf(_angle);
            point.z = EndPosition.m_positionZ + 3.f;
            float const heightPos = me->GetMap()->GetHeight(point.x, point.y, EndPosition.m_positionZ, false,
                100.f);
            point.z = heightPos;

            if (leftSide)
                _leftSidePath.push_back({ point.x, point.y, point.z });
            else
                _rightSidePath.push_back({ point.x, point.y, point.z });
        }
    }

    void ScheduleEvents()
    {
        if (_mythicFirstIntermission)
        {
            scheduler.Schedule(3s, [&](TaskContext func)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 3000);
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 100.f, true);
                if (target)
                    me->CastCustomSpell(SPELL_UKORZ_FIRE_SPIT, val, target, TRIGGERED_FULL_MASK);

                func.Repeat(randseconds(3, 7));
            });
        }

        //scheduler.Schedule(15s, [&](TaskContext func)
        //{
        //    CustomSpellValues val;
        //    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 7000);
        //    me->CastCustomSpell(SPELL_URKOZ_BLADE_FLURRY, val, me);

        //    func.Repeat(35s);
        //});

        scheduler.Schedule(6s, [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_URKOZ_FELFIRE_FISSION, data));
            float variance = float(instance->GetMythicKeyLevel() * data.SpellValueVariance);

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
            me->CastCustomSpell(SPELL_URKOZ_FELFIRE_FISSION, val, (Unit*)nullptr);
            func.Repeat(21s);
        });

        scheduler.Schedule(11s, [&](TaskContext func)
        {
            Mythic::MythicSpells data;
            ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_URKOZ_FIRE_BREATH, data));
            float variance = float(instance->GetMythicKeyLevel() * data.SpellValueVariance);

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 15000);
            Unit* target = me->GetVictim();
            if (target)
                me->CastCustomSpell(SPELL_URKOZ_FIRE_BREATH, val, target);
            func.Repeat(15s);
        });

        scheduler.Schedule(11s, [&](TaskContext func)
        {
            HandleFireBeams();
            func.Repeat(30s);
        });

        scheduler.Schedule(4s, [&](TaskContext func)
        {
            HandleStatueBeam();
            func.Repeat(6s);
        });
    }

    void HandleRequiredBossFail() override
    {
        me->SetCanMissSpells( false );
        CustomSpellValues val;
        val.AddSpellMod( SPELLVALUE_RADIUS_MOD, 1000000 );
        val.AddSpellMod( SPELLVALUE_BASE_POINT0, 1000000 );
        me->CastCustomSpell( SPELL_URKOZ_FELFIRE_FISSION, val, ( Unit* )nullptr );

        me->SetCanMissSpells( true );
    }

    void EnterCombat(Unit* who) override
    {
        if ( !_EnterCombat() )
            return;

        _frenzy = false;
        if (instance->IsMythic() && !_mythicFirstIntermission)
            MoveRuuzluToGong();

        if (!instance->IsMythic())
        {
            DoCastSelf(SPELL_RUUZLU_BERSERKER_STANCE);

            scheduler.Schedule(Milliseconds(urand(3000, 6000)), [&](TaskContext func)
            {
                auto repeatTime = Milliseconds(urand(7000, 11000));
                DoCastVictim(SPELL_EXECTUIONER_CLEAVE);

                func.Repeat(Seconds(urand(7, 11)));
            });
        }
        else
        {
            if ( GameObject* go = me->SummonGameObject( 180322, { 1762.29f, 1062.46f, 47.0348f, 3.87911f }, 0.f, 0.f, 0.f, 0.f, 0 ) )
                _entranceGateGUID = go->GetGUID();
            ScheduleZombies();
            ScheduleEvents();
        }
    }
private:
    bool _frenzy;
    bool _mythicFirstIntermission;
    bool _enableAgonizingFlames;
    uint64 _gongGUID;
    uint64 _rightSnakeGUID;
    uint64 _leftSnakeGUID;
    uint32 _successfulMeleeHitCount;
    uint64 _entranceGateGUID;
    std::vector<Position> _leftSidePath;
    std::vector<Position> _rightSidePath;
    std::vector<uint64> _statues;
};

struct go_troll_cage : public GameObjectAI
{
    go_troll_cage(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player, bool /*reportUse*/) override
    {
        InstanceScript* instance = player->GetInstanceScript();
        if (instance && instance->IsMythic())
        {
            auto eState = instance->GetBossState( BOSS_SANDFURY_EXECUTIONER );
            auto state = instance->GetBossState(BOSS_STAIRS_EVENT);
            //! if stairs done or currently in progress or executioner is not yet killed, do not start the event
            if (state == DONE || state == IN_PROGRESS || eState != DONE)
                return false;

            instance->SetBossState(BOSS_STAIRS_EVENT, IN_PROGRESS);
            return true;
        }

        return false;
    }

    void OnUnlockAttempt(SpellCastResult res, Player* player) override
    {
        if (res != SPELL_CAST_OK)
            return;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance)
            return;

        auto eState = instance->GetBossState( BOSS_SANDFURY_EXECUTIONER );
        auto sState = instance->GetBossState(BOSS_STAIRS_EVENT);
        if ( sState == DONE || sState == IN_PROGRESS || eState != DONE )
            return;

        instance->SetBossState(BOSS_STAIRS_EVENT, IN_PROGRESS);
    }
};

struct npc_zulfarrak_zombie_ukorz : public ScriptedAI
{
    npc_zulfarrak_zombie_ukorz(Creature* creature) : ScriptedAI(creature)
    {
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        //me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
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

    bool CanAIAttack(Unit const* who) const override
    {
        return who->IsPlayer() && !who->HasAura(SPELL_RUUZLU_TORNADO);
    }

    void Reset() override
    {
        task.CancelAll();
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, DEFAULT_COMBAT_REACH);

        task.Schedule(1s, [&](TaskContext func)
        {
            if (!me->IsInCombat())
            {
                DoZoneInCombat();
                func.Repeat(2s);
            }
        });

        task.Schedule(2500ms, [&](TaskContext func)
        {
            if (me->FindNearestCreature(NPC_UKORZ_FIRE_TRIGGER, 0.5f, true))
                me->DealDamage(me, me, me->GetMaxHealth() * 0.25f, (const CleanDamage*)nullptr, DIRECT_DAMAGE);

            func.Repeat(1s);
        });

        //! call right away and task will repeat unless in combat
        DoZoneInCombat(me, 150.f);
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (attacker != me)
            damage = 0;
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
        ScriptedAI::EnterCombat(who);
    }

    void OnMeleeAttack(VictimState state, WeaponAttackType type, Unit* victim, uint32 /*procAttacker*/) override
    {
        if (victim && state >= VICTIMSTATE_HIT && state <= VICTIMSTATE_PARRY)
        {
            InstanceScript* instance = me->GetInstanceScript();
            if (!instance || !instance->IsMythic())
                return;

            Creature* ruuzlu = instance->GetCreature(BOSS_RUUZLU);
            if (!ruuzlu)
                return;

            Creature* ukorz = instance->GetCreature(BOSS_UKORZ_SANDSCALP);
            if (!ukorz)
                return;

            if (Aura const* aura = victim->GetAura(SPELL_ZOMBIE_UKORZ_BRAIN_HACKER))
            {
                if (aura->GetStackAmount() >= 5)
                {
                    //CustomSpellValues val;
                    //val.AddSpellMod(SPELLVALUE_BASE_POINT0, 80);
                    //val.AddSpellMod(SPELLVALUE_AURA_DURATION, 240000);
                    //if (ruuzlu)
                    //    ruuzlu->CastCustomSpell(SPELL_CREATURE_OF_NIGHTMARE_ZF, val, victim, TRIGGERED_FULL_MASK);
                    //else if (ukorz)
                    //    ukorz->CastCustomSpell(SPELL_CREATURE_OF_NIGHTMARE_ZF, val, victim, TRIGGERED_FULL_MASK);

                    Unit::Kill(me, victim);
                    return;
                    victim->RemoveAurasDueToSpell(SPELL_ZOMBIE_UKORZ_BRAIN_HACKER);
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 100.f, true))
                    {
                        DoResetThreat();
                        me->SetInCombatWith(target);
                        target->SetInCombatWith(me);
                        me->AddThreat(target, 100000.f);
                    }
                    else
                        me->DespawnOrUnsummon();
                }
                else
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, urand(1000, 1500));
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 180000);
                    me->CastCustomSpell(SPELL_ZOMBIE_UKORZ_BRAIN_HACKER, val, victim);
                }
            }
            else
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, urand(1000, 1500));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 180000);
                me->CastCustomSpell(SPELL_ZOMBIE_UKORZ_BRAIN_HACKER, val, victim);
            }
        }
    }

private:
    TaskScheduler task;
};

enum StairEncounterPaths
{
    INITIAL_PATH_MURTA          = 3037300,
    INITIAL_PATH_EYEGOUGE       = 3037280,
    INITIAL_PATH_RAVEN          = 3037310,
    INITIAL_PATH_BLY            = 3037290,
    INITIAL_PATH_FUSE           = 3037270,

    SECOND_PATH_MURTA           = 3341030,
    SECOND_PATH_EYEGOUGE        = 3341008,
    SECOND_PATH_RAVEN           = 3341041,
    SECOND_PATH_BLY             = 3341019,
    SECOND_PATH_FUSE            = 3340997,

    PATH_FUSE_GATE              = 3644724
};

enum PyramidEventState
{
    PYRAMID_STATE_NONE          = 0,
    PYRAMID_STATE_UPSTAIRS,
    PYRAMID_STATE_FORWARD,
    PYRAMID_STATE_MOVING_FORWARD,
    PYRAMID_STATE_DONE
};

enum SandfuryRavenMisc
{
    SPELL_SANDFURY_RAVEN_BACKSTAB= 7159,
    SPELL_SANDFURY_RAVEN_GOUGE= 12540,
    SPELL_SANDFURY_RAVEN_SINISTER_STRIKE= 57640,
    SPELL_SANDFURY_RAVEN_SHADOWSTEP= 41176,
    SPELL_SANDFURY_RAVEN_CHEAPSHOT= 34243
};

struct npc_sandfury_raven : public ZulfarrakAI
{
    npc_sandfury_raven(Creature* creature) : ZulfarrakAI(creature)
    {
        me->SetCanDualWield(true);
        EventState = PYRAMID_STATE_NONE;
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (EventState == PYRAMID_STATE_UPSTAIRS)
        {
            if (who->GetPositionZ() < 38.40)
                return false;
        }

        return MythicAI::CanAIAttack(who);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LEAVE_CAGE)
        {
            me->GetMotionMaster()->MovePath(INITIAL_PATH_RAVEN, false);
            me->SetHomePosition({ 1891.15f, 1263.73f, 41.3927f, 4.77211f });
        }
        else if (action == ACTION_MOVE_DOWNSTAIRS)
            me->GetMotionMaster()->MovePath(SECOND_PATH_RAVEN, false);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(6000, 6000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(10000);
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_RAVEN_BACKSTAB);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_RAVEN_BACKSTAB, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_RAVEN_BACKSTAB, val, target);
                repeatTime = Milliseconds(6000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 6000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(11000, 23000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_RAVEN_GOUGE, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_RAVEN_GOUGE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_RAVEN_GOUGE, val, target);
                DoResetThreat();
                repeatTime = Milliseconds(20000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (IsMythic() && EventState == PYRAMID_STATE_DONE)
        {
            task.Schedule(3s, [&](TaskContext func)
            {
                CustomSpellValues val;

                Unit* target = me->GetVictim();
                if (target)
                    me->CastCustomSpell(SPELL_SANDFURY_RAVEN_SINISTER_STRIKE, val, target);

                func.Repeat(6s);
            });

            task.Schedule(25s, [&](TaskContext func)
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 35.f);
                if (target)
                {
                    DoCast(target, SPELL_SANDFURY_RAVEN_SHADOWSTEP, true);
                    DoResetThreat();
                    target->AddThreat(me, 2000.f);
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 5500);
                    me->CastCustomSpell(SPELL_SANDFURY_RAVEN_CHEAPSHOT, val, target);
                }
                func.Repeat(30s);
            });
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != WAYPOINT_MOTION_TYPE)
            return;

        //! if moving downstairs then set homeposition at each point
        if (EventState == PYRAMID_STATE_MOVING_FORWARD && pointId)
            me->SetHomePosition(me->GetPosition());
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == PYRAMID_EVENT_SET_STATE)
            EventState = static_cast<PyramidEventState>(value);
    }
private:
    PyramidEventState EventState;
};

constexpr uint32 SPELL_ORC_EYEGOUGE_CURSE_OF_WEAKNESS{ 12741 };
constexpr uint32 SPELL_ORC_EYEGOUGE_IMMOLATE{ 11962 };
constexpr uint32 SPELL_ORC_EYEGOUGE_RAIN_OF_FIRE{ 11990 };
constexpr uint32 SPELL_ORC_EYEGOUGE_CHAOS_BOLT{ 69576 };
constexpr uint32 SPELL_ORC_EYEGOUGE_CONFLAGRATION{ 16805 };
struct npc_orc_eyegouge : public ZulfarrakAI
{
    npc_orc_eyegouge(Creature* creature) : ZulfarrakAI(creature)
    {
        EventState = PYRAMID_STATE_NONE;
    }

    void AttackStart(Unit* who) override
    {
        auto const _currentMana = me->GetPower(POWER_MANA);
        auto const _maxMana = me->GetMaxPower(POWER_MANA);
        if (_currentMana > (_maxMana * 0.1f))
            AttackStartCaster(who, 11.f);
        else
            ZulfarrakAI::AttackStart(who);
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (EventState == PYRAMID_STATE_UPSTAIRS)
        {
            if (who->GetPositionZ() < 36.40)
                return false;
        }

        return MythicAI::CanAIAttack(who);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LEAVE_CAGE)
        {
            me->GetMotionMaster()->MovePath(INITIAL_PATH_EYEGOUGE, false);
            me->SetHomePosition({ 1882.79f, 1271.62f, 41.8513f, 4.67395f });
        }
        else if (action == ACTION_MOVE_DOWNSTAIRS)
            me->GetMotionMaster()->MovePath(SECOND_PATH_EYEGOUGE, false);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        task.Schedule(Milliseconds(urand(1000, 15000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(60000);
            if (!IsMythic())
            {
                DoCastAOE(SPELL_ORC_EYEGOUGE_CURSE_OF_WEAKNESS);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ORC_EYEGOUGE_CURSE_OF_WEAKNESS, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                me->CastCustomSpell(SPELL_ORC_EYEGOUGE_CURSE_OF_WEAKNESS, val, (Unit*)nullptr);
                repeatTime = Milliseconds(25000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(13000, 15000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(23000, 35000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_ORC_EYEGOUGE_IMMOLATE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ORC_EYEGOUGE_IMMOLATE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);

                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_ORC_EYEGOUGE_IMMOLATE, val, target);
                repeatTime = Milliseconds(11000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(6000, 11000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(23000, 35000));
            if (!IsMythic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f))
                    DoCast(target, SPELL_ORC_EYEGOUGE_RAIN_OF_FIRE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_ORC_EYEGOUGE_RAIN_OF_FIRE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_MAX_TARGETS, data.MaxEffectedTargets);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f))
                    me->CastCustomSpell(SPELL_ORC_EYEGOUGE_RAIN_OF_FIRE, val, target);
                repeatTime = Milliseconds(20000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 3000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3000, 5000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SANDFURY_ACOLYTE_SHADOW_BOLT, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (IsMythic() && EventState == PYRAMID_STATE_DONE)
        {
            task.Schedule(5s, [&](TaskContext func)
            {
                Unit* target = me->GetVictim();
                DoCast(target, SPELL_ORC_EYEGOUGE_CHAOS_BOLT);

                func.Repeat(13s);
            });

            task.Schedule(25s, [&](TaskContext func)
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, 30.f, true);
                if (target)
                {
                    CustomSpellValues val;
                    me->CastCustomSpell(SPELL_ORC_EYEGOUGE_CONFLAGRATION, val, target);
                }

                func.Repeat(25s);
            });
        }
    }

    void JustDied(Unit* killer) override
    {
        // ToDo: stair boss handling
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != WAYPOINT_MOTION_TYPE)
            return;

        //! if moving downstairs then set homeposition at each point
        if (EventState == PYRAMID_STATE_MOVING_FORWARD && pointId)
            me->SetHomePosition(me->GetPosition());
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == PYRAMID_EVENT_SET_STATE)
            EventState = static_cast<PyramidEventState>(value);
    }
private:
    PyramidEventState EventState;
};

enum MurtaGrumgut
{
    SPELL_MURMUT_RENEW = 11640,
    SPELL_MURMUT_HOLY_SMITE = 9734,
    SPELL_MURMUT_HEAL = 11642,
    SPELL_MURMUT_POWER_WORD_SHIELD = 11974,
    SPELL_MURMUT_PSYCHIC_SCREAM = 43432,
    SPELL_MURMUT_MASS_DISPEL = 32375
};

struct npc_murta_grimgut : public ZulfarrakAI
{
    npc_murta_grimgut(Creature* creature) : ZulfarrakAI(creature)
    {
        EventState = PYRAMID_STATE_NONE;
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (EventState == PYRAMID_STATE_UPSTAIRS)
        {
            if (who->GetPositionZ() < 36.40)
                return false;
        }

        return MythicAI::CanAIAttack(who);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LEAVE_CAGE)
        {
            me->GetMotionMaster()->MovePath(INITIAL_PATH_MURTA, false);
            me->SetHomePosition({ 1886.78f, 1271.53f, 41.7064f, 4.69751f });
        }
        else if (action == ACTION_MOVE_DOWNSTAIRS)
            me->GetMotionMaster()->MovePath(SECOND_PATH_MURTA, false);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);
        task.Schedule(Milliseconds(urand(1000, 3000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 11000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10, true))
                    DoCast(target, SPELL_MURMUT_RENEW);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MURMUT_RENEW, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10, true))
                    me->CastCustomSpell(SPELL_MURMUT_RENEW, val, target);
                repeatTime = Milliseconds(8000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (!IsMythic())
        {
            task.Schedule(Milliseconds(urand(1000, 1000)), [&](TaskContext func)
            {
                DoCastVictim(SPELL_MURMUT_HOLY_SMITE);
                func.Repeat(Seconds(urand(10, 13)));
            });
        }

        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 11000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10, true))
                    DoCast(target, SPELL_MURMUT_HEAL);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MURMUT_HEAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 30, true))
                    me->CastCustomSpell(SPELL_MURMUT_HEAL, val, target);

                repeatTime = Milliseconds(4500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 1000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(8000, 11000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10, true))
                    DoCast(target, SPELL_MURMUT_POWER_WORD_SHIELD);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_MURMUT_POWER_WORD_SHIELD, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 20, true))
                    me->CastCustomSpell(SPELL_MURMUT_POWER_WORD_SHIELD, val, target);
                repeatTime = Milliseconds(6500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (IsMythic() && EventState == PYRAMID_STATE_DONE)
        {
            task.Schedule(10s, [&](TaskContext func)
            {
                if (me->SelectNearestPlayer(4.f))
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, 6000);
                    me->CastCustomSpell(SPELL_MURMUT_PSYCHIC_SCREAM, val, (Unit*)nullptr);
                    func.Repeat(45s);
                }
                else
                    func.Repeat(5s);
            });

            task.Schedule(10s, [&](TaskContext func)
            {
                Unit* target = DoSelectLowestHpFriendly(30.f, 1U, true);
                if (target)
                    DoCast(target, SPELL_MURMUT_MASS_DISPEL, true);

                func.Repeat(1min);
            });
        }
    }

    void JustDied(Unit* killer) override
    {
        // to do stair boss handling
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == PYRAMID_EVENT_SET_STATE)
            EventState = static_cast<PyramidEventState>(value);
    }
private:
    PyramidEventState EventState;
};

enum BlyMisc
{
    SPELL_SERGEANT_BLY_SUNDER_ARMOR = 64978,
    SPELL_SERGEANT_BLY_SHOCKWAVE = 72149,
    SPELL_SERGEANT_BLY_SHIELD_BASH = 11972,
    SPELL_SERGEANT_BLY_BLOCKING = 3637,
    SPELL_SERGEANT_BLY_REVENGE = 12170,
    SPELL_BLYS_BAND_ESCAPE = 11365,
    GO_BLY_ESCAPE_PORTAL = 142074,
    SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP = 41409,
    SPELL_MYTHIC_AGGRO_NEARBY_BLY = 42864
};

enum blyAndCrewFactions
{
    FACTION_HOSTILE         = 14,
    FACTION_FRIENDLY        = 35,  //while in cages (so the trolls won't attack them while they're caged)
    FACTION_FREED           = 250  //after release (so they'll be hostile towards trolls)
};

constexpr uint32 NPC_ENTRY_SANDFURY_CRETIN{ 7789 };
constexpr uint32 NPC_SANDFURY_SLAVE{ 7787 };
constexpr uint32 NPC_SANDFURY_ACOLYTE{ 8876 };
constexpr uint32 NPC_SANDFURY_DRUDGE{ 7788 };
//! when approaching final boss
constexpr uint32 NPC_SANDFURY_ZEALOT{ 8877 };

constexpr uint32 STAIR_EVENT_SPAWN_POSITIONS_SIZE{ 15 };
Position const StairEventSpawnPositions[STAIR_EVENT_SPAWN_POSITIONS_SIZE] =
{
    { 1876.14f, 1220.36f, 9.11633f, 4.63467f },
    { 1885.22f, 1219.66f, 8.90074f, 4.63467f },
    { 1897.57f, 1220.31f, 8.87713f, 4.56791f },
    { 1896.67f, 1214.07f, 8.87713f, 4.56791f },
    { 1888.40f, 1212.52f, 8.87713f, 4.56791f },
    { 1879.55f, 1210.55f, 8.87713f, 4.56791f },
    { 1870.57f, 1205.38f, 8.87713f, 4.56791f },
    { 1882.98f, 1204.95f, 8.87713f, 4.98574f },
    { 1895.42f, 1205.50f, 8.87713f, 4.63467f },
    { 1900.63f, 1200.55f, 8.87713f, 4.63467f },
    { 1868.76f, 1200.34f, 8.87713f, 4.63467f },
    { 1865.10f, 1201.65f, 8.87713f, 4.63467f },
    { 1891.21f, 1211.08f, 8.87713f, 5.31796f },
    { 1877.19f, 1204.86f, 8.87713f, 4.79253f },
    { 1893.65f, 1202.56f, 8.87713f, 4.43910f }
};

Position const NekrumSpawnPosition{ 1881.04f, 1201.24f, 8.87731f, 6.25103f };
Position const SezzizSpawnPosition{ 1884.62f, 1201.68f, 8.87731f, 4.70223f };
constexpr uint32 BLY_SCHEDULER_GROUP_COMBAT{ 1 };
constexpr uint32 BLY_SCHEDULER_BLY_ESCAPE{ 2 };
constexpr uint32 TALK_BLY_HOW_DARE{ 0 };
constexpr uint32 TALK_BLY_AFTER_ALL_WE{ 1 };
constexpr uint32 TALK_BLY_MOVE_FORWARD{ 2 };
constexpr uint32 POINT_ID_PYRAMID_UPSTAIRS{ 2000 };

std::string const DivinomaticGossipString = "Hey Bly! Bilgewizzle wants his divino-matic rod back!";
std::string const TiredOfHelpingGossipString = "That's it! I'm tired of help you out. It's time we settled things on the battlefield!";
constexpr uint32 QUEST_DIVINO_MATIC_ROD{ 2768 };
//! Do not call bossAI functions that set boss states, we do it manually
struct npc_sergeant_bly : public BossAI
{
    npc_sergeant_bly(Creature* creature) : BossAI(creature, BOSS_STAIRS_EVENT)
    {
        ResetEvent();
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (EventState == PYRAMID_STATE_UPSTAIRS)
        {
            if (who->GetPositionZ() < 37.90)
                return false;
        }

        return BossAI::CanAIAttack(who);
    }

    void EnterEvadeMode() override
    {
        //! Do not call bossAI enter evade mode
        ScriptedAI::EnterEvadeMode();
    }

    void ResetEvent()
    {
        CretinSummonGUIDs.clear();
        StairSummonGUIDs.clear();
        SetEventState(PYRAMID_STATE_NONE);
        SummonsSpawnedCount = 0;
        SummonsDiedCount = 0;
        MoveSummonsCount = 2;
        SpawnBosses = false;
        ScheduleAdditionalWaves = false;
        summons.DespawnAll();
        scheduler.CancelAll();
        if (CreatureData const* data = me->GetCreatureData())
            me->SetHomePosition({ data->posX, data->posY, data->posZ, data->orientation });
        Reseting = false;
        TurningHostile = false;
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        if (instance)
            instance->SetData(INSTANCE_DATA_RESET_CAGES, INSTANCE_DATA_RESET_CAGES);
    }

    void PutEveryoneInCombatAndReduceDamage()
    {
        if (instance && instance->IsMythic() && EventState != PYRAMID_STATE_DONE &&
            !me->HasAura(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP))
        {
            if (EventState != PYRAMID_STATE_DONE)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, -80);
                val.AddSpellMod(SPELLVALUE_BASE_POINT2, -80);
                me->CastCustomSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP, val, me, TRIGGERED_FULL_MASK);
            }
            else
                me->RemoveAurasDueToSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP);

            for (uint32 entry : { NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
            {
                uint64 const guid = instance->GetData64(entry);
                Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                if (!creature)
                    continue;

                if (EventState == PYRAMID_STATE_DONE)
                    creature->RemoveAurasDueToSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP);
                else
                {
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                    val.AddSpellMod(SPELLVALUE_BASE_POINT1, -70);
                    val.AddSpellMod(SPELLVALUE_BASE_POINT2, -70);
                    creature->CastCustomSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP, val, creature, TRIGGERED_FULL_MASK);
                }
            }
        }
    }

    void SetEventState(PyramidEventState state)
    {
        EventState = state;
        if (state == PYRAMID_STATE_DONE)
        {
            instance->SetBossState(BOSS_STAIRS_EVENT, DONE);
            me->SetRegeneratingHealth(true);
            me->SetFullHealth();
            me->RemoveAurasDueToSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP);
        }

        for (uint32 entry : { NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
        {
            uint64 const guid = instance->GetData64(entry);
            Creature* creature = ObjectAccessor::GetCreature(*me, guid);

            if (creature && creature->IsAIEnabled)
            {
                creature->AI()->SetData(PYRAMID_EVENT_SET_STATE, state);
                if (state != PYRAMID_STATE_DONE && !creature->HasAura(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP))
                {
                    if (instance && instance->IsMythic())
                    {
                        CustomSpellValues val;
                        val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                        val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                        val.AddSpellMod(SPELLVALUE_BASE_POINT1, -80);
                        val.AddSpellMod(SPELLVALUE_BASE_POINT2, -80);
                        creature->CastCustomSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP, val, creature, TRIGGERED_FULL_MASK);
                    }
                }

                if (state == PYRAMID_STATE_DONE)
                {
                    creature->SetRegeneratingHealth(true);
                    creature->SetFullHealth();
                    creature->RemoveAurasDueToSpell(SPELL_MYTHIC_DAMAGE_REDUCTION_ON_BLY_GROUP);
                }
            }
        }
    }

    void MoveDownstairsIfPossible()
    {
        if (EventState == PYRAMID_STATE_FORWARD)
        {
            for (uint32 entry : { NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
            {
                uint64 const guid = instance->GetData64(entry);
                Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                if (creature && creature->IsAIEnabled)
                {
                    creature->StopMoving();
                    creature->GetMotionMaster()->Clear();
                    creature->SetSpeedRate(MOVE_WALK, creature->GetSpeedRate(MOVE_WALK) * 0.70f);
                    creature->AI()->DoAction(ACTION_MOVE_DOWNSTAIRS);
                }
            }

            Talk(TALK_BLY_MOVE_FORWARD);
            SetEventState(PYRAMID_STATE_MOVING_FORWARD);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->GetMotionMaster()->Clear();
            me->StopMoving();
            me->SetSpeedRate(MOVE_WALK, me->GetSpeedRate(MOVE_WALK) * 0.70f);
            me->GetMotionMaster()->MovePath(SECOND_PATH_BLY, false);
        }
    }

    void sGossipHello(Player* player) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (EventState)
        {
            case PYRAMID_STATE_NONE:
                player->SEND_GOSSIP_MENU(1515, me->GetGUID());
                break;
            case PYRAMID_STATE_UPSTAIRS:
            case PYRAMID_STATE_FORWARD:
                player->SEND_GOSSIP_MENU(1516, me->GetGUID());
                break;
            case PYRAMID_STATE_DONE:
                if (player->GetQuestStatus(QUEST_DIVINO_MATIC_ROD) == QUEST_STATUS_INCOMPLETE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, DivinomaticGossipString, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                else
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TiredOfHelpingGossipString, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

                player->SEND_GOSSIP_MENU(1517, me->GetGUID());
                break;
            default:
                break;
        }
    }

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        if (!TurningHostile)
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            TurningHostile = true;
            uint64 weegliGUID = instance->GetData64(NPC_WEEGLI);
            Creature* weegli = ObjectAccessor::GetCreature(*me, weegliGUID);
            if (weegli)
            {
                weegli->SetImmuneToAll(true);
                weegli->AI()->DoAction(ACTION_WEEGLI_DESTROY_GATE);
            }

            scheduler.Schedule(1s, [&](TaskContext func)
            {
                auto repeatCounter = func.GetRepeatCounter();
                switch (repeatCounter)
                {
                    case 0:
                    {
                        Player* player = me->SelectNearestPlayer(15.f);
                        if (player)
                            me->SetFacingTo(me->GetAngle(player->GetPositionX(), player->GetPositionY()));
                        Talk(TALK_BLY_HOW_DARE);
                        func.Repeat(2500ms);
                        break;
                    }
                    case 1:
                        Talk(TALK_BLY_AFTER_ALL_WE);
                        func.Repeat(3500ms);
                        break;
                    case 2:
                    {
                        me->setFaction(FACTION_HOSTILE);
                        DoZoneInCombat();

                        for (uint32 entry : { NPC_RAVEN, NPC_ORO, NPC_MURTA })
                        {
                            uint64 const guid = instance->GetData64(entry);
                            Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                            if (creature)
                            {
                                creature->setFaction(FACTION_HOSTILE);
                                creature->SetAggressive();
                                creature->AI()->DoZoneInCombat();
                            }
                        }
                        break;
                    }
                }
            });
        }
    }

    void Reset() override
    {
        if (EventState != PYRAMID_STATE_MOVING_FORWARD)
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        scheduler.CancelGroup(BLY_SCHEDULER_GROUP_COMBAT);
        //! Do not reset entire AI when pyramid event is in progress
        auto state = instance->GetBossState(BOSS_STAIRS_EVENT);
        if (state == IN_PROGRESS)
        {
            MoveDownstairsIfPossible();
            return;
        }

        if (EventState >= PYRAMID_STATE_UPSTAIRS)
            return;

        ResetEvent();
        BossAI::Reset();
    }

    void MoveEliteSummons()
    {
        CretinSummonGUIDs.erase(std::remove_if(CretinSummonGUIDs.begin(), CretinSummonGUIDs.end(), [&](uint64 guid) -> bool
        {
            Creature* summ = ObjectAccessor::GetCreature(*me, guid);
            if (!summ)
                return true;

            //! already in combat, skip
            if (summ->isDead() || summ->IsInCombat())
                return true;
            return false;
        }), CretinSummonGUIDs.end());

        if (CretinSummonGUIDs.empty())
            return;

        uint64 const guid = Trinity::Containers::SelectRandomContainerElement(CretinSummonGUIDs);
        Creature* summon = ObjectAccessor::GetCreature(*me, guid);
        if (!summon)
            return;

        summon->DisableChainPullFor(8000ms);
        summon->GetMotionMaster()->Clear();
        summon->GetMotionMaster()->MovePoint(1, me->GetPosition());
        CretinSummonGUIDs.erase(std::remove(CretinSummonGUIDs.begin(), CretinSummonGUIDs.end(),
            guid), CretinSummonGUIDs.end());
    }

    void MoveNoneliteSummons(uint32 count)
    {
        StairSummonGUIDs.erase(std::remove_if(StairSummonGUIDs.begin(), StairSummonGUIDs.end(), [&](uint64 guid) -> bool
        {
            Creature* summ = ObjectAccessor::GetCreature(*me, guid);
            if (!summ)
                return true;

            //! already in combat, skip
            if (summ->isDead() || summ->IsInCombat())
                return true;
            return false;
        }), StairSummonGUIDs.end());

        for (uint32 i = 0; i < count; ++i)
        {
            if (StairSummonGUIDs.empty())
                return;

            uint64 const summGUID = Trinity::Containers::SelectRandomContainerElement(StairSummonGUIDs);
            Creature* summon = ObjectAccessor::GetCreature(*me, summGUID);
            if (!summon)
                continue;

            summon->DisableChainPullFor(8000ms);
            summon->GetMotionMaster()->Clear();
            summon->GetMotionMaster()->MovePoint(1, me->GetPosition());
            StairSummonGUIDs.erase(std::remove(StairSummonGUIDs.begin(), StairSummonGUIDs.end(),
                summGUID), StairSummonGUIDs.end());
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LEAVE_CAGE)
        {
            me->setFaction(FACTION_FREED);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            for (uint32 entry : { NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
            {
                uint64 const guid = instance->GetData64(entry);
                Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                if (creature)
                {
                    creature->setFaction(FACTION_FREED);
                    creature->SetAggressive();
                }
            }

            SetEventState(PYRAMID_STATE_UPSTAIRS);
            me->SetAggressive();
            me->GetMotionMaster()->MovePath(INITIAL_PATH_BLY, false);
            me->SetHomePosition({ 1886.91f, 1263.28f, 41.4831f, 4.73363f });
        }

        if (action == ACTION_BEGIN_STAIR_EVENT)
            ScheduleSpawning();
    }

    Position GetTrollSpawnPosition()
    {
        return me->GetNearPositionFromPos(Trinity::Containers::SelectRandomContainerElement(StairEventSpawnPositions), 3.5f);
    }

    void SpawnWave(uint32 waveCount)
    {
        TempSummonType sumType = TEMPSUMMON_CORPSE_TIMED_DESPAWN;
        for (uint32 i = 0; i < waveCount; ++i)
        {
            //! each wave contains 1 sandfury cretin, 3 acolytes, 3 grudges and 3 slaves
            me->SummonCreature(NPC_ENTRY_SANDFURY_CRETIN, GetTrollSpawnPosition(), sumType, 6000);
            for (uint32 j = 0; j < 3; ++j)
            {
                me->SummonCreature(NPC_SANDFURY_SLAVE, GetTrollSpawnPosition(), sumType, 6000);
                me->SummonCreature(NPC_SANDFURY_ACOLYTE, GetTrollSpawnPosition(), sumType, 6000);
                me->SummonCreature(NPC_SANDFURY_DRUDGE, GetTrollSpawnPosition(), sumType, 6000);
            }
        }
    }

    void ScheduleSpawning()
    {
        SpawnWave(4U);
        scheduler.Schedule(11s, [&](TaskContext func)
        {
            MoveNoneliteSummons(MoveSummonsCount);
            func.Repeat(8s);
        });

        scheduler.Schedule(11s, [&](TaskContext func)
        {
            MoveEliteSummons();
            func.Repeat(30s);
        });
    }

    void JustSummoned(Creature* summon) override
    {
        if (!summon)
            return;

        if (summon->GetEntry() == NPC_NEKRUM || summon->GetEntry() == NPC_SEZZIZ)
        {
            summon->SetDefensive();
            return;
        }

        BossAI::JustSummoned(summon);
        summon->GetMotionMaster()->MoveRandom(10.f);
        summon->DisableChainPullFor(200000ms);

        if (summon->GetEntry() != NPC_ENTRY_SANDFURY_CRETIN)
            StairSummonGUIDs.push_back(summon->GetGUID());
        else
            CretinSummonGUIDs.push_back(summon->GetGUID());

        if (summon->IsAIEnabled)
            summon->AI()->DoAction(ACTION_MARK_AS_STAIRS_ADD);

        //! Bosses spawn after 100 trolls have been spawned
        if (++SummonsSpawnedCount > 100U && !SpawnBosses)
        {
            MoveSummonsCount = 3;
            SpawnBosses = true;
            Creature* nekrum = me->SummonCreature(NPC_NEKRUM, NekrumSpawnPosition, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000);
            Creature* sezziz = me->SummonCreature(NPC_SEZZIZ, SezzizSpawnPosition, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000);
            if (instance && instance->IsMythic())
            {
                JustSummoned(nekrum);
                JustSummoned(sezziz);
            }

            SetEventState(PYRAMID_STATE_FORWARD);
            //! call reset, it will schedule moving forward
            if (!me->IsInCombat())
                MoveDownstairsIfPossible();
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        if (++SummonsDiedCount > 35U && !ScheduleAdditionalWaves)
        {
            MoveSummonsCount = 2;
            ScheduleAdditionalWaves = true;
            scheduler.Schedule(15s, [&](TaskContext func)
            {
                SpawnWave(2U);
                if (func.GetRepeatCounter() < 3U)
                    func.Repeat(50s);
            });
        }
    }

    void EnterCombat(Unit* who) override
    {
        PutEveryoneInCombatAndReduceDamage();
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 100.f);
        me->CastCustomSpell(SPELL_MYTHIC_AGGRO_NEARBY_BLY, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);

        if (EventState == PYRAMID_STATE_DONE)
            scheduler.CancelGroup(BLY_SCHEDULER_BLY_ESCAPE);

        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        for (uint32 entry : { NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
        {
            //! otherwise weegli will attack players when bly event has been started by player
            //! while weegli is moving to blast the gate
            if (entry == NPC_WEEGLI && EventState == PYRAMID_STATE_DONE)
                continue;

            uint64 const guid = instance->GetData64(entry);
            Creature* creature = ObjectAccessor::GetCreature(*me, guid);
            if (creature)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 100.f);
                creature->CastCustomSpell(SPELL_MYTHIC_AGGRO_NEARBY_BLY, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);

                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                if (creature->IsAIEnabled)
                    creature->AI()->AttackStart(who);
            }
        }

        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        //BossAI::EnterCombat(who);
        scheduler.Schedule(Milliseconds(1000), BLY_SCHEDULER_GROUP_COMBAT, [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(16000, 26000));
            if (!instance->IsMythic())
            {
                DoCastVictim(SPELL_SERGEANT_BLY_SHIELD_BASH);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SERGEANT_BLY_SHIELD_BASH, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0U, [&](Unit* obj)
                {
                    return obj->IsPlayer() && obj->IsCasting() && obj->IsWithinDist(me, 5.f);
                });

                if (!target)
                    target = me->GetVictim();

                if (target)
                    me->CastCustomSpell(SPELL_SERGEANT_BLY_SHIELD_BASH, val, target);
                repeatTime = Milliseconds(5000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        scheduler.Schedule(Milliseconds(urand(1000, 3500)), BLY_SCHEDULER_GROUP_COMBAT, [&](TaskContext func)
        {
            DoCastSelf(SPELL_SERGEANT_BLY_BLOCKING);
        });

        scheduler.Schedule(Milliseconds(urand(3000, 6000)), BLY_SCHEDULER_GROUP_COMBAT, [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(9000, 15000));
            if (!instance->IsMythic())
            {
                DoCastVictim(SPELL_SERGEANT_BLY_REVENGE);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SERGEANT_BLY_REVENGE, data));
                float variance = float(instance->GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SERGEANT_BLY_REVENGE, val, target);
                repeatTime = Milliseconds(8000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        if (instance->IsMythic() && EventState == PYRAMID_STATE_DONE)
        {
            scheduler.Schedule(5s, BLY_SCHEDULER_GROUP_COMBAT, [&](TaskContext func)
            {
                DoCastVictim(SPELL_SERGEANT_BLY_SUNDER_ARMOR);
                func.Repeat(10s);
            });

            scheduler.Schedule(Seconds(urand(6, 11)), BLY_SCHEDULER_GROUP_COMBAT, [&](TaskContext func)
            {
                CustomSpellValues val;
                me->CastCustomSpell(SPELL_SERGEANT_BLY_SHOCKWAVE, val, (Unit*)nullptr);

                func.Repeat(20s);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (EventState == PYRAMID_STATE_NONE)
            return;

        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != WAYPOINT_MOTION_TYPE)
            return;

        if (EventState == PYRAMID_STATE_UPSTAIRS && pointId == 4)
        {
            for (uint32 entry : { NPC_SERGEANT_BLY, NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
            {
                uint64 const guid = instance->GetData64(entry);
                Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                if (creature)
                    creature->GetMotionMaster()->Clear();
            }
        }

        //! if moving downstairs then set homeposition at each point
        if (EventState == PYRAMID_STATE_MOVING_FORWARD && pointId)
        {
            me->SetHomePosition(me->GetPosition());
            if (pointId == 2)
            {
                SetEventState(PYRAMID_STATE_DONE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                for (uint32 entry : { NPC_SERGEANT_BLY, NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
                {
                    uint64 const guid = instance->GetData64(entry);
                    Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                    if (creature)
                        creature->SetFullHealth();
                }

                Creature* weegli = ObjectAccessor::GetCreature(*me, instance->GetData64(NPC_WEEGLI));
                if (weegli)
                    weegli->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                scheduler.Schedule(3min, BLY_SCHEDULER_BLY_ESCAPE, [&](TaskContext func)
                {
                    me->SummonGameObject(142074, { 1883.723022f, 1199.080444f, 8.877083f, 1.430030f }, 0.f, 0.f,
                        0.f, 0.f, 0);
                    DoCastSelf(SPELL_BLYS_BAND_ESCAPE);
                    //! Weegli should never escape
                    for (uint32 entry : { NPC_SERGEANT_BLY, NPC_RAVEN, NPC_ORO, /*NPC_WEEGLI*/ NPC_MURTA })
                    {
                        uint64 const guid = instance->GetData64(entry);
                        Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                        if (creature)
                        {
                            creature->CastSpell(creature, SPELL_BLYS_BAND_ESCAPE, false);
                        }
                    }
                });

                if (!me->IsInCombat())
                    Reset();
            }
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (EventState != PYRAMID_STATE_DONE && damage >= me->GetHealth())
        {
            damage = 0;
            if (!Reseting)
            {
                Reseting = true;
                ResetEvent();
                _DespawnAtEvade(5, me);
                instance->SetBossState(BOSS_STAIRS_EVENT, NOT_STARTED);

                for (uint32 entry : { NPC_SERGEANT_BLY, NPC_RAVEN, NPC_ORO, NPC_WEEGLI, NPC_MURTA })
                {
                    uint64 const guid = instance->GetData64(entry);
                    Creature* creature = ObjectAccessor::GetCreature(*me, guid);
                    if (creature)
                    {
                        if (creature->isDead())
                            creature->Respawn();
                        if (CreatureData const* data = creature->GetCreatureData())
                            creature->SetHomePosition({ data->posX, data->posY, data->posZ, data->orientation });
                        creature->AI()->Reset();
                        creature->DespawnOrUnsummon(1, 30);
                    }
                }
            }
        }
    }

    void JustDied(Unit* killer) override
    {
        if (EventState == PYRAMID_STATE_DONE)
        {
            if (instance && instance->IsMythic())
                instance->SetData(INSTANCE_DATA_BLY_DEATH, INSTANCE_DATA_BLY_DEATH);

            BossAI::JustDied(killer);
        }
    }
private:
    bool SpawnBosses;
    PyramidEventState EventState;
    std::vector<uint64> StairSummonGUIDs;
    std::vector<uint64> CretinSummonGUIDs;
    uint32 SummonsSpawnedCount;
    uint32 SummonsDiedCount;
    uint32 MoveSummonsCount;
    bool ScheduleAdditionalWaves;
    bool Reseting;
    bool TurningHostile;
};

constexpr uint32 POINT_BLASTFUSE_TALK_STAIRS{ 5 };
constexpr uint32 POINT_BLASTFUSE_AT_GATE{ 2 };
constexpr uint32 TALK_BLASTFUSE_HERE{ 0 };
constexpr uint32 TALK_BLASTFUSE_HERE_I_GO{ 1 };
constexpr uint32 TALK_BLASTFUSE_OUT_OF_HERE{ 2 };

enum BlastFuseSpells
{
    SPELL_BLASTFUSE_BOMB              = 8858,
    SPELL_BLASTFUSE_GOBLIN_LAND_MINE  = 10837,
    SPELL_BLASTFUSE_SHOOT             = 6660,
    SPELL_BLASTFUSE_WEEGLIS_BARREL    = 10772
};

std::string BlastFuseDoorGossip = "Will you blow up that door now?";
struct npc_weegli_blastfuse : public ZulfarrakAI
{
    npc_weegli_blastfuse(Creature* creature) : ZulfarrakAI(creature)
    {
        _destroyGate = false;
    }

    void sGossipHello(Player* player) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (EventState)
        {
            case PYRAMID_STATE_NONE:
                player->SEND_GOSSIP_MENU(1511, me->GetGUID());
                break;
            case PYRAMID_STATE_DONE:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, BlastFuseDoorGossip, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(1514, me->GetGUID());  //if event can proceed to end
                break;
            default:
                player->SEND_GOSSIP_MENU(1513, me->GetGUID());
                break;
        }
    }

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 /*actionId*/) override
    {
        player->PlayerTalkClass->SendCloseGossip();
        TalkAndDestroyGate(TALK_BLASTFUSE_HERE_I_GO);
    }

    void TalkAndDestroyGate(uint32 talkId)
    {
        if (!_destroyGate)
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->SetPassive();
            me->GetMotionMaster()->MovePath(PATH_FUSE_GATE, false);
            Talk(talkId);
            _destroyGate = true;
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LEAVE_CAGE)
        {
            me->GetMotionMaster()->MovePath(INITIAL_PATH_FUSE, false);
            me->SetHomePosition({ 1882.94f, 1262.89f, 41.5678f, 4.41477f });
        }
        else if (action == ACTION_MOVE_DOWNSTAIRS)
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->GetMotionMaster()->MovePath(SECOND_PATH_FUSE, false);
        }
        else if (action == ACTION_WEEGLI_DESTROY_GATE)
            TalkAndDestroyGate(TALK_BLASTFUSE_OUT_OF_HERE);
    }

    void EnterCombat(Unit* who) override
    {
        MythicDoZoneInCombat(me, 75.f);

        task.Schedule(10s, [&](TaskContext func)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [&](Unit* obj)
            {
                return !IS_PLAYER_GUID(obj->GetGUID());
            }))
                target->CastSpell(target, SPELL_BLASTFUSE_GOBLIN_LAND_MINE, true, NullItemRef, (AuraEffect*)nullptr, me->GetGUID());

            func.Repeat(10s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        task.Update(diff);

        if (me->isAttackReady() && !me->IsWithinMeleeRange(me->GetVictim()))
        {
            DoCastVictim(SPELL_BLASTFUSE_SHOOT);
            if (me->GetSheath() != SHEATH_STATE_RANGED)
                me->SetSheath(SHEATH_STATE_RANGED);
        }
        else
        {
            if (me->GetSheath() != SHEATH_STATE_MELEE)
                me->SetSheath(SHEATH_STATE_MELEE);
            DoMeleeAttackIfReady();
        }
    }

    bool CanAIAttack(Unit const* who) const override
    {
        if (EventState == PYRAMID_STATE_UPSTAIRS)
        {
            if (who->GetPositionZ() < 36.40)
                return false;
        }

        return MythicAI::CanAIAttack(who);
    }

    void AttackStart(Unit* who) override
    {
        AttackStartCaster(who, 7.f);
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != WAYPOINT_MOTION_TYPE)
            return;

        if (pointId == POINT_BLASTFUSE_TALK_STAIRS && EventState == PYRAMID_STATE_UPSTAIRS)
        {
            Talk(TALK_BLASTFUSE_HERE);
            uint64 const blyGUID = instance->GetData64(NPC_SERGEANT_BLY);
            if (Creature* bly = ObjectAccessor::GetCreature(*me, blyGUID))
                bly->AI()->DoAction(ACTION_BEGIN_STAIR_EVENT);
        }

        //! if moving downstairs then set homeposition at each point
        if (EventState == PYRAMID_STATE_MOVING_FORWARD && pointId)
        {
            me->SetHomePosition(me->GetPosition());
            if (pointId == 3)
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        if (_destroyGate && pointId == POINT_BLASTFUSE_AT_GATE)
        {
            if (InstanceScript* instance = me->GetInstanceScript())
                instance->SetBossState(BOSS_GOBLIN_STAIRS, DONE);

            DoCastAOE(SPELL_BLASTFUSE_WEEGLIS_BARREL, true);
            me->DespawnOrUnsummon(20s);
        }
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == PYRAMID_EVENT_SET_STATE)
            EventState = static_cast<PyramidEventState>(value);
    }
private:
    PyramidEventState EventState;
    bool _destroyGate;
};

constexpr uint32 SPELL_SEZZIZ_HEAL{ 12039 };
constexpr uint32 SPELL_SEZZIZ_PSYCHIC_SCREAM{ 13704 };
constexpr uint32 SPELL_SEZZIZ_RENEW{ 8362 };
constexpr uint32 SPELL_SEZZIZ_SHADOW_BOLT{ 15537 };

constexpr uint32 SEZZIZ_ADDS_SPAWN_SIZE{ 12 };
Position const SezzizAddsSpawnPosition[SEZZIZ_ADDS_SPAWN_SIZE] =
{
    { 1870.44f, 1202.50f, 8.8767f, 1.42009f },
    { 1872.51f, 1202.14f, 8.8767f, 1.39653f },
    { 1875.68f, 1202.25f, 8.8767f, 1.39653f },
    { 1878.20f, 1201.81f, 8.8767f, 1.39653f },
    { 1878.20f, 1201.81f, 8.8767f, 1.39653f },
    { 1880.51f, 1201.40f, 8.8767f, 1.39653f },
    { 1889.55f, 1201.19f, 8.8767f, 1.47507f },
    { 1892.11f, 1200.97f, 8.8767f, 1.64865f },
    { 1894.43f, 1201.15f, 8.8767f, 1.64865f },
    { 1896.99f, 1201.35f, 8.8767f, 1.64865f },
    { 1898.74f, 1201.49f, 8.8767f, 1.64865f },
    { 1893.04f, 1201.04f, 8.8767f, 1.64865f }
};

struct npc_shadowpriest_sezzziz : public ZulfarrakAI
{
    npc_shadowpriest_sezzziz(Creature* creature) : ZulfarrakAI(creature), summons(me)
    {
        IsFirstSpawn = false;
    }

    void Reset() override
    {
        summons.DespawnAll();
        task.CancelAll();
        if (!IsFirstSpawn)
        {
            IsFirstSpawn = true;
            task.Schedule(15s, [&](TaskContext func)
            {
                for (uint32 i = 0; i < 2; ++i)
                {
                    if (summons.size() > 6)
                        continue;

                    Position pos = Trinity::Containers::SelectRandomContainerElement(SezzizAddsSpawnPosition);
                    uint32 entry = Trinity::Containers::SelectRandomContainerElement(
                        std::vector<uint32>{ NPC_SANDFURY_ZEALOT, NPC_SANDFURY_SLAVE,
                        NPC_SANDFURY_ACOLYTE, NPC_SANDFURY_DRUDGE });
                    me->SummonCreature(entry, pos);
                }

                func.Repeat(5s);
            });
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        summons.Despawn(summon);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        if (Creature* nekrum = me->FindNearestCreature(NPC_NEKRUM, 30.f, true))
            if (nekrum->IsAIEnabled)
                nekrum->AI()->DoZoneInCombat(nekrum, 100.f);

        task.Schedule(Milliseconds(urand(3500, 6500)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(7000, 10000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 20, true))
                    DoCast(target, SPELL_SEZZIZ_HEAL);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SEZZIZ_HEAL, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                repeatTime = Milliseconds(8000);
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 30))
                    me->CastCustomSpell(SPELL_SEZZIZ_HEAL, val, target);
                else
                    repeatTime = 1000ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 16000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(22000, 30000));
            if (!IsMythic())
            {
                DoCastAOE(SPELL_SEZZIZ_PSYCHIC_SCREAM, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SEZZIZ_PSYCHIC_SCREAM, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 20000.0f);
                me->CastCustomSpell(SPELL_SEZZIZ_PSYCHIC_SCREAM, val, (Unit*)nullptr);
                repeatTime = Milliseconds(22000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 6000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(10000, 12000));
            if (!IsMythic())
            {
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 20))
                    DoCast(target, SPELL_SEZZIZ_RENEW);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SEZZIZ_RENEW, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                repeatTime = Milliseconds(5000);
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = DoSelectLowestHpFriendly(40.f, 10))
                    me->CastCustomSpell(SPELL_SEZZIZ_RENEW, val, target);
                else
                    repeatTime = 1500ms;
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(1000), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(3000, 4000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_SEZZIZ_SHADOW_BOLT);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SEZZIZ_SHADOW_BOLT, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;

                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_SEZZIZ_SHADOW_BOLT, val, target);
                repeatTime = Milliseconds(3000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
private:
    bool IsFirstSpawn;
    SummonList summons;
};

constexpr uint32 SPELL_NERKUM_HARMSTRING{ 26141 };
constexpr uint32 SPELL_NEKRUM_BERSERKER_CHARGE{ 19471 };

struct npc_nekrum_gutchewer : public ZulfarrakAI
{
    npc_nekrum_gutchewer(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        MythicAI::EnterCombat(who);

        if (Creature* sezziz = me->FindNearestCreature(NPC_SEZZIZ, 30.f, true))
            if (sezziz->IsAIEnabled)
                sezziz->AI()->DoZoneInCombat(sezziz, 100.f);

        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, DoGetSpellMaxRange(SPELL_NEKRUM_BERSERKER_CHARGE), true))
            DoCast(target, SPELL_NEKRUM_BERSERKER_CHARGE, true);

        task.Schedule(Milliseconds(urand(1000, 16000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(12000, 20000));
            if (!IsMythic())
            {
                DoCastVictim(SPELL_NERKUM_HARMSTRING);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_NERKUM_HARMSTRING, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_NERKUM_HARMSTRING, val, target);
                repeatTime = Milliseconds(11500);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });

        task.Schedule(Milliseconds(urand(1000, 20000)), [&](TaskContext func)
        {
            auto repeatTime = Milliseconds(urand(30000, 60000));
            if (!IsMythic())
            {
                DoCastAOE(SPELL_THEKA_FEVERED_PLAGUE, true);
            }
            else
            {
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_THEKA_FEVERED_PLAGUE, data));
                float variance = float(GetMythicKeyLevel()) * data.SpellValueVariance;

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, Mythic::CalculateMythicValue(data.SpellEffectValue1, variance));
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, data.AuraDuration);
                if (Unit* target = me->GetVictim())
                    me->CastCustomSpell(SPELL_THEKA_FEVERED_PLAGUE, val, target);
                repeatTime = Milliseconds(15000);
            }

            if (repeatTime.count())
                func.Repeat(repeatTime);
        });
    }
};

class spell_blys_band_escape : public SpellScript
{
    PrepareSpellScript(spell_blys_band_escape);

    void HandleSpellHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* creature = GetHitCreature())
            creature->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_blys_band_escape::HandleSpellHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

constexpr uint32 SPELL_WEEGLI_BOMB_LAND_MINE_ARMING{ 11816 };
constexpr uint32 SPELL_WEEGLI_BOMB_DETONATE{ 4043 };
enum WeegliBombTalks
{
    TALK_BOMB_10S           = 0,
    TALK_BOMB_5S            = 1,
    TALK_BOMB_EXPLODE       = 2
};

struct npc_goblin_weegli_land_mine : public PassiveAI
{
    npc_goblin_weegli_land_mine(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        task.CancelAll();
        Talk(TALK_BOMB_10S);
        DoCastSelf(SPELL_WEEGLI_BOMB_LAND_MINE_ARMING, true);

        task.Schedule(5s, [&](TaskContext /*func*/)
        {
            Talk(TALK_BOMB_5S);
        });

        task.Schedule(10s, [&](TaskContext /*func*/)
        {
            Talk(TALK_BOMB_EXPLODE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            DoCastAOE(SPELL_WEEGLI_BOMB_DETONATE, true);
            me->RemoveAurasDueToSpell(SPELL_WEEGLI_BOMB_LAND_MINE_ARMING);
            me->DespawnOrUnsummon(1000);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

private:
    TaskScheduler task;
};

constexpr uint32 SPELL_FIRE_PATH_BURNING_PITCH{ 69660 };
constexpr uint32 SPELL_FIRE_PATH_EXPLOSION_VISUAL{ 34602 };
struct npc_urkoz_fire_path_trigger : public PassiveAI
{
    npc_urkoz_fire_path_trigger(Creature* creature) : PassiveAI(creature)
    {
        instance = me->GetInstanceScript();
        if (!instance)
            me->DespawnOrUnsummon();
    }

    void Reset() override
    {
        spawnedAtTargetGUID = 0;
        task.CancelAll();
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type == DATA_UKORZ_SET_EXPLOSION_TIME)
        {
            task.Schedule(Milliseconds(value), [&](TaskContext /*func*/)
            {
                Unit* target = ObjectAccessor::GetUnit(*me, spawnedAtTargetGUID);
                if (target && target->IsAlive() && target->IsInMap(me))
                    me->Relocate(target->GetPosition());

                DoCastAOE(SPELL_FIRE_PATH_EXPLOSION_VISUAL);

                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_FIRE_PATH_BURNING_PITCH, data));
                float variance = float(instance->GetMythicKeyLevel() * data.SpellValueVariance);

                CustomSpellValues val;
                //! 2000 evaluates to 0.2, base radius of flame spell is 5y and random direction.
                //! this mod ensures we spawn on top of caster
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 5000);
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
                me->CastCustomSpell(SPELL_FIRE_PATH_BURNING_PITCH, val, me, TRIGGERED_FULL_MASK);
            });
        }
        else if (type == DATA_UKORZ_SET_EXPLOSION_DESPAWN_TIME)
            me->DespawnOrUnsummon(value);
    }

    void SetGUID(uint64 guid, int32 action) override
    {
        spawnedAtTargetGUID = guid;
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

private:
    TaskScheduler task;
    InstanceScript* instance;
    uint64 spawnedAtTargetGUID;
};

struct go_gong_of_zulfarrak : public GameObjectAI
{
    go_gong_of_zulfarrak(GameObject* go) : GameObjectAI(go) { }

    void OnUnlockAttempt(SpellCastResult res, Player* /*unlocker*/) override
    {
        InstanceScript* instance = go->GetInstanceScript();
        if (!instance || instance->IsMythic())
            return;

        //! Gahzrilla already present
        if (instance->GetCreature(BOSS_GAHZRILLA) || instance->GetBossState(BOSS_GAHZRILLA) == DONE)
            return;

        //! poprawic move position
        Position const GahzRillaSpawnPosition{ 1659.52f, 1180.41f, -2.88258f, 0.734786f };
        go->GetMap()->SummonCreature(NPC_GAHZRILLA, GahzRillaSpawnPosition);
    }
};

class spell_mythic_burning_pitch_mod_dest : public SpellScript
{
    PrepareSpellScript(spell_mythic_burning_pitch_mod_dest);

    void ModDestHeight(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();
        InstanceScript* instance = caster ? caster->GetInstanceScript() : nullptr;

        if (caster && instance && instance->IsMythic())
        {
            Position pos = caster->GetPosition();
            WorldLocation dest = WorldLocation{ caster->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() };

            SetExplTargetDest(dest);
            GetHitDest()->Relocate(dest);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mythic_burning_pitch_mod_dest::ModDestHeight, EFFECT_0, SPELL_EFFECT_PERSISTENT_AREA_AURA);
    }
};

constexpr uint32 SPELL_KNEEL_MYTHIC_GROW{ 74996 };
class spell_kneel_mythic : public AuraScript
{
    PrepareAuraScript(spell_kneel_mythic);

    void OnEffApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* victim = GetTarget();
        if (!victim)
            return;

        InstanceScript* instance = victim->GetInstanceScript();
        if (!instance || !instance->IsMythic())
            return;

        if (!victim->CreateVehicleKit(363, 0))
            return;

        victim->CastSpell(victim, SPELL_KNEEL_MYTHIC_GROW, true);
        WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, victim->GetPackGUID().size() + 4);
        data.appendPackGUID(victim->GetGUID());
        data << uint32(363);
        victim->SendMessageToSet(&data, true);
        data.Initialize(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, 0);
        victim->ToPlayer()->GetSession()->SendPacket(&data);

        Creature* trigger = victim->SummonCreature(NPC_IMPALING_AXE_TRIGGER, victim->GetPosition());
        if (trigger)
        {
            trigger->SetSelectable(false);
            trigger->SetPassive();
            trigger->LoadEquipment(1, true);
            trigger->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 0, victim, false);
            InstanceScript* instance = victim->GetInstanceScript();
            if (instance)
            {
                Creature* ruuzlu = instance->GetCreature(BOSS_RUUZLU);
                if (ruuzlu && ruuzlu->IsAIEnabled)
                    ruuzlu->AI()->JustSummoned(trigger);
            }
        }
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* victim = GetTarget();
        if (!victim)
            return;

        if (!victim->GetVehicleKit())
            return;

        victim->RemoveAurasDueToSpell(SPELL_KNEEL_MYTHIC_GROW);

        victim->RemoveVehicleKit();
        WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, victim->GetPackGUID().size() + 4);
        data.appendPackGUID(victim->GetGUID());
        data << uint32(0);
        victim->SendMessageToSet(&data, true);
        data.Initialize(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, 0);
        victim->ToPlayer()->GetSession()->SendPacket(&data);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_kneel_mythic::OnEffApply, EFFECT_0,
            SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);

        OnEffectRemove += AuraEffectRemoveFn(spell_kneel_mythic::OnRemove, EFFECT_0,
            SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_weapon_throw_ruuzlu : public SpellScript
{
    PrepareSpellScript(spell_weapon_throw_ruuzlu);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (instance && instance->IsMythic())
        {
            targets.remove_if([&](WorldObject* obj) -> bool
            {
                Unit* target = obj->ToUnit();
                return target && (target->HasAura(SPELL_RUUZLU_TORNADO) || target->HasAura(SPELL_RUUZLU_HITTIN_YA_TRIGGERED));
            });

            if (targets.empty())
                return;

            WorldObject* targetObj = Trinity::Containers::SelectRandomContainerElement(targets);
            targets.clear();
            targets.push_back(targetObj);
        }
    }

    void HandleHit(SpellEffIndex /*eff*/)
    {
        Unit* me = GetCaster();
        if (!me)
            return;

        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);

        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
        val.AddSpellMod(SPELLVALUE_BASE_POINT0, 35); // haste
        me->CastCustomSpell(SPELL_RUUZLU_ENRAGE, val, me, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_weapon_throw_ruuzlu::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_weapon_throw_ruuzlu::FilterTargets,
            EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

constexpr uint32 SPELL_FLAME_BUFFET_RUUZLU{ 46279 };
class spell_ruuzlu_enrage_firenova : public SpellScript
{
    PrepareSpellScript(spell_ruuzlu_enrage_firenova);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (instance && instance->IsMythic() && GetHitUnit())
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 300000);
            GetCaster()->CastCustomSpell(SPELL_FLAME_BUFFET_RUUZLU, val, GetHitUnit(), TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ruuzlu_enrage_firenova::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

enum SandfuryGuardian
{
    SPELL_SANDFURY_GUARDIAN_THRASH = 3391,
    SPELL_SANDFURY_GUARDIAN_POISON = 744
};

struct npc_sandfury_guardian_zulfarrak : public ZulfarrakAI
{
    npc_sandfury_guardian_zulfarrak(Creature* creature) : ZulfarrakAI(creature) { }

    void EnterCombat(Unit* who) override
    {
        ZulfarrakAI::EnterCombat(who);

        task.Schedule(randseconds(2, 4), [&](TaskContext func)
        {
            DoCastSelf(SPELL_SANDFURY_GUARDIAN_THRASH);
            func.Repeat(randseconds(20, 36));
        });

        task.Schedule(5s, [&](TaskContext func)
        {
            if (!IsMythic())
                DoCastVictim(SPELL_SANDFURY_GUARDIAN_POISON);
            else
            {
                Unit* target = me->GetVictim();
                Mythic::MythicSpells data;
                ASSERT(sMythicMgr.GetSpellValuesFor(SPELL_SANDFURY_GUARDIAN_POISON, data));
                float variance = float(instance->GetMythicKeyLevel() * data.SpellValueVariance);

                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, Mythic::CalculateMythicValue(data.SpellEffectValue0, variance));
                if (target)
                    me->CastCustomSpell(SPELL_SANDFURY_GUARDIAN_POISON, val, target);
            }

            func.Repeat(randseconds(8, 11));
        });
    }
};

class spell_ruuzlu_enrage_proc : public SpellScript
{
    PrepareSpellScript(spell_ruuzlu_enrage_proc);

    void HandleHit(SpellEffIndex /*eff*/)
    {
        Unit* target = GetHitUnit();
        InstanceScript* instance = target ? target->GetInstanceScript() : nullptr;
        if (instance && target && instance->IsMythic())
        {
            int32 val = Mythic::CalculateMythicValue(4500, (0.06 * instance->GetMythicKeyLevel()));
            SetEffectValue(val);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_ruuzlu_enrage_proc::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class spell_zombie_brain_hacker : public SpellScript
{
    PrepareSpellScript(spell_zombie_brain_hacker);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        InstanceScript* instance = target ? target->GetInstanceScript() : nullptr;
        if (!instance || !instance->IsMythic())
        {
            if (Aura* aura = GetHitUnit()->GetAura(GetSpellInfo()->Id))
                aura->SetStackAmount(1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_zombie_brain_hacker::HandleHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

void AddSC_zulfarrak()
{
    RegisterCreatureAI(npc_sandfury_blood_drinker);
    RegisterCreatureAI(npc_sandfury_shadowcaster);
    RegisterCreatureAI(npc_sandfury_witchdoctor);
    RegisterCreatureAI(npc_sandfury_zulfarak_totem);
    RegisterCreatureAI(npc_sandfury_shadowhunter);
    RegisterCreatureAI(npc_scarab_lord_zulfarrak);
    RegisterCreatureAI(npc_sandfury_souleater);
    RegisterCreatureAI(npc_servant_of_antsul);
    RegisterCreatureAI(npc_antusul_earthgrab);
    RegisterCreatureAI(npc_zumrah_ward);
    RegisterCreatureAI(npc_zumrah_skeleton);
    RegisterCreatureAI(npc_zulfarrak_zombie);
    RegisterCreatureAI(npc_zulfarrak_dead_hero);
    RegisterCreatureAI(npc_hydromancer_velratha);
    RegisterCreatureAI(npc_gahzrilla);
    RegisterCreatureAI(npc_sandfury_slave);
    RegisterCreatureAI(npc_sandfury_acolyte);
    RegisterCreatureAI(npc_sandfury_zelot);
    RegisterCreatureAI(npc_sandfury_cretin);
    RegisterCreatureAI(npc_sullithuz_abomination);
    RegisterCreatureAI(npc_goblin_weegli_land_mine);
    RegisterCreatureAI(npc_theka_anub_scarab);
    RegisterCreatureAI(npc_theka_poison_trigger);
    RegisterCreatureAI(npc_velratha_frozen_orb_mythic);
    RegisterCreatureAI(npc_urkoz_fire_path_trigger);

    //! Bosses
    RegisterCreatureAI(npc_sullithuz_sandcrawler);
    RegisterCreatureAI(npc_zerillis);
    RegisterCreatureAI(npc_theka_the_martyr);
    RegisterCreatureAI(npc_antusul_zulfarrak);
    RegisterCreatureAI(npc_witchdoctor_zumrah);
    RegisterCreatureAI(npc_sandfury_executioner);
    RegisterCreatureAI(npc_shadowpriest_sezzziz);
    RegisterCreatureAI(npc_nekrum_gutchewer);
    RegisterCreatureAI(npc_zulfarrak_ruuzlu);
    RegisterCreatureAI(npc_chief_ukorz_sandscalp);
    RegisterCreatureAI(npc_zumrah_cauldron_triggerAI);
    RegisterCreatureAI(npc_gahzrilla_mythic);
    RegisterCreatureAI(npc_tornado_ruuzlu_mythic);
    RegisterCreatureAI(npc_electric_wolf_antusul);
    RegisterCreatureAI(npc_sandfury_guardian_zulfarrak);
    RegisterCreatureAI(npc_velratha_water_tomb);
    RegisterCreatureAI(npc_zulfarrak_zombie_ukorz);

    //! stair event
    RegisterCreatureAI(npc_sandfury_raven);
    RegisterCreatureAI(npc_orc_eyegouge);
    RegisterCreatureAI(npc_murta_grimgut);
    RegisterCreatureAI(npc_sergeant_bly);
    RegisterCreatureAI(npc_weegli_blastfuse);

    //! areatriggers
    new at_antu_sul_zulfarrak();

    //! spells
    RegisterSpellScript(spell_zulfarrak_summon_zulfarrak_zombies);
    RegisterSpellScript(spell_blys_band_escape);
    RegisterAuraScript(spell_kneel_mythic);
    RegisterSpellScript(spell_weapon_throw_ruuzlu);
    RegisterAuraScript(spell_theka_leeching_swarm_aura);
    RegisterAuraScript(spell_zulfarrak_totem_mythic_scaling);
    RegisterAuraScript(spell_ruuzlu_hurricane_fiter);
    RegisterAuraScript(spell_frozen_embrace_remove);
    RegisterSpellScript(spell_ruuzlu_enrage_firenova);
    RegisterSpellScript(spell_mythic_burning_pitch_mod_dest);
    RegisterSpellScript(spell_ruuzlu_enrage_proc);
    RegisterSpellScript(spell_zombie_brain_hacker);
    RegisterSpellScript(spell_frozen_orb_damage_mythic);

    //! GameObjects
    RegisterGameObjectAI(go_troll_cage);
    RegisterGameObjectAI(go_gong_of_zulfarrak);
}
