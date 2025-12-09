
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "nexus.h"
#include "MapReference.h"
#include "Player.h"
#include "SpellAuras.h"
#include "MoveSplineInit.h"
#include "GameObjectAI.h"

struct GateData
{
    Position const pos{ };
    float objectSize = 2.f;
};

static const std::vector<GateData> _gatePositions =
{
    { { 302.047f, -67.3530f, -16.6367f, 1.57813f }, 1.5f },
    { { 364.965f, -5.84205f, -16.6367f, 3.07981f }, 1.5f },
    { { 301.821f, 57.64900f, -16.6367f, 4.66710f }, 1.5f },
    { { 238.224f, -5.07367f, -16.6367f, 6.24497f }, 1.5f }
};

enum KeristraszaSpells
{
    SPELL_FROZEN_PRISON             = 47854,
    SPELL_TAIL_SWEEP                = 50155,
    SPELL_CRYSTAL_CHAINS            = 50997,
    SPELL_ENRAGE                    = 8599,
    SPELL_ENRAGE_MYTHIC             = 28371,
    SPELL_CRYSTALFIRE_BREATH        = 48096,
    SPELL_CRYSTALIZE                = 48179,
    SPELL_INTENSE_COLD              = 48094,
    SPELL_INTENSE_COLD_TRIGGER      = 48095
};

enum KeristraszaYells
{
    SAY_AGGRO                       = 0,
    SAY_SLAY,
    SAY_ENRAGE,
    SAY_DEATH,
    SAY_CRYSTAL_NOVA,
    EMOTE_FRENZY
};

enum KeristraszaEvents
{
    EVENT_CRYSTALFIRE_BREATH        = 1,
    EVENT_CRYSTAL_CHAINS,
    EVENT_TAIL_SWEEP,
    EVENT_ACHIEVEMENT_CHECK,
    EVENT_KILL_TALK
};

enum SchedulerGroups
{
    SCHEDULER_GROUP_GROUND = 1,
    SCHEDULER_GROUP_AIR = 2
};

enum KeristraszaMisc
{
    SPELL_KERI_WATER_BUBBLE_HEAD = 35923,
    SPELL_KERI_FROST_CLEAVE = 51857,
    SPELL_KERI_ICE_BEAM = 49549,
    SPELL_KERI_FROST_BOMB_VISUAL = 70022,
    SPELL_KERI_ICEBLAST_ALLY = 69099,
    SPELL_KERI_COLD_FLAMES = 69146,
    NPC_KERI_AIR_PHASE_TRIGGER = 91527,
    NPC_KERI_CRYSTALFIRE_BREATH_TRIGGER = 91528,
    NPC_KERI_WHELP = 91529,
    NPC_KERI_WATER_BUBBLE_TRIGGER = 91530,
    NPC_KERI_FROZEN_ORB_MYTHIC = 91705,
    SPELL_KERI_FROSTBOMB_EXPLOSION = 69845,
    SPELL_KERI_WATERY_BOMB_SELECTOR = 49705,
    GAMEOBJECT_KERI_PORTAL_MYTHIC = 302104,
    NPC_KERI_EGG_MYTHIC = 91708,
    SPELL_KERI_TWILIGHT_REFLECTION = 21064,
    SPELL_KERI_ENRAGED_DEFENSE = 70725,
};

constexpr uint32 ENTRANCE_POSITION_MAX_SIZE{ 4 };
Position const KeristraszaRoomEntrancePositions[ENTRANCE_POSITION_MAX_SIZE] =
{
    { 302.301f,  57.8063f, -16.6359f, 4.70896f },
    { 239.816f, -5.35119f, -16.6367f, 6.27975f },
    { 368.330f, -6.00742f, -16.6367f, 3.07297f },
    { 302.313f, -69.6532f, -16.6361f, 1.57129f }
};

Position const KeristraszaCenterPosition{ 302.01f, -5.42593f, -15.5641f, 6.12022f };
struct boss_keristrasza : public BossAI
{
    boss_keristrasza(Creature* creature) : BossAI(creature, DATA_KERISTRASZA_EVENT)
    {
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 9.f);
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
        for (uint64 guid : _gates)
        {
            GameObject* gate = ObjectAccessor::GetGameObject(*me, guid);
            if (gate)
                gate->RemoveFromWorld();
        }

        _healHandled = false;
        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
        me->SetVisible(true);
        me->SetSelectable(true);
        _gates.clear();
        BossAI::Reset();
        _whelpScheduler.CancelAll();
        RemovePrison(CanRemovePrison());
        aGuids.clear();
        _orbAngle = false;
        _frenzied = false;
        _airPhase = false;
        _whelpsScheduled = false;
        _mythicPhases.fill(false);

        if (instance && instance->IsMythic())
            me->SetCanMissSpells(false);

        me->SetDisableGravity(false);
        me->SetCanFly(false);
        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
        me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
        me->SetAggressive();
    }

    void EnterCombat(Unit* who) override
    {
        RemovePrison(true);
        Talk(SAY_AGGRO);
        BossAI::EnterCombat(who);
        _eggGUID = 0;
        if (instance && instance->IsMythic() && instance->GetMythicKeyLevel() >= Mythic::MYTHIC_SECRET_PHASE_KEY_LEVEL)
        {
            Creature* egg = me->SummonCreature(NPC_KERI_EGG_MYTHIC, me->GetHomePosition(), TEMPSUMMON_CORPSE_DESPAWN);
            if (!egg)
            {
                EnterEvadeMode();
                return;
            }

            _eggGUID = egg->GetGUID();
            egg->SetPhaseMask(256, true);
            egg->SetObjectScale(2.5f);
            egg->SetPassive();
            egg->SetRegeneratingHealth(false);
        }

        me->CastSpell(me, SPELL_INTENSE_COLD, true);
        if (!instance->IsMythic())
        {
            events.ScheduleEvent(EVENT_CRYSTALFIRE_BREATH, 14s);
            events.ScheduleEvent(EVENT_CRYSTAL_CHAINS, DUNGEON_MODE(20s, 11s));
            events.ScheduleEvent(EVENT_TAIL_SWEEP, 5s);
            events.ScheduleEvent(EVENT_ACHIEVEMENT_CHECK, 1s);
        }
        else
        {
            ScheduleMythic();

            for (GateData const& data : _gatePositions)
            {
                GameObject* go = me->SummonGameObject(180322, data.pos, 0.f, 0.f, 0.f, 0.f, 0);
                if (go)
                {
                    _gates.push_back(go->GetGUID());
                    go->SetObjectScale(data.objectSize);
                    go->EnableCollision(true);
                }
            }
        }
    }

    void SummonedCreatureDies(Creature* who, Unit* killer) override
    {
        if (who && who->GetEntry() == NPC_KERI_EGG_MYTHIC)
        {
            me->MonsterTextEmote("Keristrasza goes on the defensive.", nullptr, false);
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, -20);
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, -1);
            me->CastCustomSpell(SPELL_KERI_ENRAGED_DEFENSE, val, me, TRIGGERED_FULL_MASK);
        }
    }

    void FillCirclePath(Position const& centerPos, float radius, float z, Movement::PointsArray& path)
    {
        float step = -M_PI / 8.0f;
        float angle = centerPos.GetAngle(me->GetPositionX(), me->GetPositionY());

        for (uint8 i = 0; i < 16; angle += step, ++i)
        {
            G3D::Vector3 point;
            point.x = centerPos.GetPositionX() + radius * cosf(angle);
            point.y = centerPos.GetPositionY() + radius * sinf(angle);
            point.z = z;
            path.push_back(point);
        }
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        if (target && target->IsPlayer() && spell->Id == SPELL_KERI_WATERY_BOMB_SELECTOR)
        {
            std::string name = target->GetName();
            name += " is about to explode!";
            me->MonsterTextEmote(name.c_str(), nullptr, true);

            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_AURA_DURATION, 3500);
            target->CastCustomSpell(SPELL_KERI_WATER_BUBBLE_HEAD, val, target, TRIGGERED_FULL_MASK);
        }
    }

    void EnterEvadeMode() override
    {
        summons.DespawnAll();
        _DespawnAtEvade(10, me);
    }

    void ScheduleMythic()
    {
        scheduler.Schedule(12s, [&](TaskContext func)
        {
            Creature* egg = ObjectAccessor::GetCreature(*me, _eggGUID);
            if (egg && egg->IsAlive())
            {
                me->MonsterTextEmote("Portal appears..", nullptr, false);
                Position const spawnPos = me->GetHomePosition();
                GameObject* portal = me->SummonGameObject(GAMEOBJECT_KERI_PORTAL_MYTHIC, spawnPos, 0.f, 0.f, 0.f, 0.f, 6000);
                if (portal)
                    portal->RemoveFromOwner();

                func.Repeat(25s);
            }
        });

        scheduler.Schedule(14s, SCHEDULER_GROUP_GROUND, [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            if (target)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 6000);
                val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 4000);
                val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
                val.AddSpellMod(SPELLVALUE_TARGET_PLAYERS_ONLY, 1);
                me->CastCustomSpell(SPELL_CRYSTALFIRE_BREATH, val, target);

                func.Repeat(randseconds(10, 12));
            }
        });

        scheduler.Schedule(5s, SCHEDULER_GROUP_GROUND, [&](TaskContext func)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, 100.f, true))
                DoCast(target, SPELL_KERI_WATERY_BOMB_SELECTOR, true);
            func.Repeat(randseconds(11, 14));
        });

        scheduler.Schedule(2200ms, [&](TaskContext func)
        {
            Unit* target = me->GetVictim();
            CustomSpellValues val;
            if (target)
            {
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 125);
                me->CastCustomSpell(SPELL_KERI_FROST_CLEAVE, val, target, TRIGGERED_FULL_MASK);
            }

            func.Repeat(randseconds(5, 7));
        });
    }

    void HandleAirEvent()
    {
        scheduler.Schedule(4500ms, SCHEDULER_GROUP_AIR, [&](TaskContext func)
        {
            if (func.GetRepeatCounter() < 7)
            {
                uint32 count = 0;
                auto tList = me->getThreatManager().getThreatList();
                if (tList.empty())
                    EnterEvadeMode();
                else
                {
                    for (auto const& actor : tList)
                    {
                        Unit* unit = ObjectAccessor::GetUnit(*me, actor->getUnitGuid());
                        if (!unit || !unit->IsPlayer() || unit->GetDistance(me) > 200.f)
                            continue;

                        Position triggerSpawnPos = unit->GetPosition();
                        triggerSpawnPos.m_positionZ = unit->GetMap()->GetHeight(triggerSpawnPos.m_positionX,
                            triggerSpawnPos.m_positionY, triggerSpawnPos.m_positionZ);

                        Creature* trigger = unit->SummonCreature(NPC_KERI_AIR_PHASE_TRIGGER, triggerSpawnPos);
                        if (trigger)
                        {
                            trigger->SetMaxHealth(10000000);
                            trigger->SetFullHealth();
                            trigger->setFaction(unit->getFaction());
                            trigger->SetPassive();
                            trigger->SetSelectable(false);
                            trigger->CastSpell(trigger, SPELL_KERI_FROST_BOMB_VISUAL, true);
                            JustSummoned(trigger);
                        }

                        scheduler.Schedule(2s, [&](TaskContext /*func*/)
                        {
                            for (uint64 guid : summons)
                            {
                                Creature* trigger = summons.GetSummon(guid);
                                if (!trigger || trigger->GetEntry() != NPC_KERI_AIR_PHASE_TRIGGER)
                                    continue;

                                CustomSpellValues val;
                                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                                val.AddSpellMod(SPELLVALUE_BASE_POINT1, 0);
                                val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 300);
                                me->CastCustomSpell(SPELL_KERI_ICE_BEAM, val, trigger, TRIGGERED_FULL_MASK);
                                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 35000);
                                val.AddSpellMod(SPELLVALUE_DISABLE_REFLECT, 1);
                                trigger->CastCustomSpell(SPELL_KERI_ICEBLAST_ALLY, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                            }
                        });
                    }

                    func.Repeat();
                }
            }
            else
                LandOnGround();
        });
    }

    void TriggerAirPhase()
    {
        scheduler.CancelGroup(SCHEDULER_GROUP_GROUND);
        me->AttackStop();
        me->SetPassive();
        me->SetSelectable(false);
        me->GetMotionMaster()->Clear();

        me->SetDisableGravity(true);
        me->SetCanFly(true);
        me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);

        Movement::MoveSplineInit init(me);
        init.SetAnimation(Movement::AnimType::ToFly);
        init.MoveTo({ KeristraszaCenterPosition.GetPositionX(), KeristraszaCenterPosition.GetPositionY(),
            KeristraszaCenterPosition.GetPositionZ() + 20.f });
        int32 duration = init.Launch();

        scheduler.Schedule(Milliseconds(duration + 2000), SCHEDULER_GROUP_AIR, [&](TaskContext func)
        {
            HandleAirEvent();
        });
    }

    void LandOnGround()
    {
        scheduler.CancelAll();
        Movement::MoveSplineInit init(me);
        init.MoveTo({ KeristraszaCenterPosition.GetPositionX(), KeristraszaCenterPosition.GetPositionY(),
            KeristraszaCenterPosition.GetPositionZ() });
        init.SetAnimation(Movement::AnimType::FlyToGround);
        int32 duration = init.Launch();
        summons.DespawnEntry(NPC_KERI_AIR_PHASE_TRIGGER);

        scheduler.Schedule(Milliseconds(duration + 1500), [&](TaskContext /*func*/)
        {
            ScheduleMythic();
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
            _airPhase = false;
            me->SetSelectable(true);
            me->SetAggressive();
            DoZoneInCombat(me, 100.f);
        });
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        Talk(SAY_DEATH);
        for (uint64 guid : _gates)
        {
            GameObject* gate = ObjectAccessor::GetGameObject(*me, guid);
            if (gate)
                gate->RemoveFromWorld();
        }

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 302.214996f, -5.313606f, -15.564038f, 3.012442f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void KilledUnit(Unit* /*who*/) override
    {
        if (events.GetNextEventTime(EVENT_KILL_TALK) == 0)
        {
            Talk(SAY_SLAY);
            events.ScheduleEvent(EVENT_KILL_TALK, 6000);
        }
    }

    void SetData(uint32 type, uint32) override
    {
        if (type == me->GetEntry() && CanRemovePrison())
            RemovePrison(true);
    }

    bool CanRemovePrison()
    {
        for (uint8 i = DATA_TELESTRA_ORB; i <= DATA_ORMOROK_ORB; ++i)
            if (instance->GetBossState(i) != DONE)
                return false;
        return true;
    }

    void RemovePrison(bool remove)
    {
        if (remove)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
        }
        else
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->CastSpell(me, SPELL_FROZEN_PRISON, true);
        }
    }

    uint32 GetData(uint32 guid) const override
    {
        return aGuids.find(guid) == aGuids.end();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_frenzied && me->HealthBelowPctDamaged(instance->IsMythic() ? 15 : 25, damage))
        {
            _frenzied = true;
            Talk(SAY_ENRAGE);
            Talk(EMOTE_FRENZY);
            me->CastSpell(me, instance->IsMythic() ? SPELL_ENRAGE_MYTHIC : SPELL_ENRAGE, true);
        }

        if (instance->IsMythic())
        {
            if (me->HealthBelowPctDamaged(75, damage) && !_mythicPhases[0])
            {
                _mythicPhases[0] = true;
                TriggerAirPhase();
            }

            if (me->HealthBelowPctDamaged(50, damage) && !_mythicPhases[1])
            {
                _mythicPhases[1] = true;
                TriggerAirPhase();
            }

            if (me->HealthBelowPctDamaged(25, damage) && !_mythicPhases[2])
            {
                _mythicPhases[2] = true;
                TriggerAirPhase();
            }

            if (me->HealthBelowPctDamaged(20, damage) && !_whelpsScheduled)
            {
                _whelpsScheduled = true;
                _whelpScheduler.Schedule(1s, [&](TaskContext func)
                {
                    Position const& pos = Trinity::Containers::SelectRandomContainerElement(KeristraszaRoomEntrancePositions);
                    Creature* whelp = me->SummonCreature(NPC_KERI_WHELP, pos, TEMPSUMMON_CORPSE_DESPAWN);
                    if (whelp)
                    {
                        whelp->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        whelp->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        whelp->SetDisableGravity(true);
                        whelp->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM);
                        if (whelp->IsAIEnabled)
                            whelp->AI()->DoZoneInCombat(whelp, 100.f);
                    }

                    func.Repeat(7s);
                });
            }
        }

        if (damage >= me->GetHealth())
        {
            if (instance && instance->IsMythic() && instance->GetMythicKeyLevel() >= Mythic::MYTHIC_SECRET_PHASE_KEY_LEVEL)
            {
                Creature* egg = ObjectAccessor::GetCreature(*me, _eggGUID);
                if (!_healHandled && egg && egg->IsAlive())
                {
                    me->MonsterTextEmote("Keristrasza siphons remaining life out of the egg...", nullptr, false);
                    uint32 _eggHealth = egg->GetMaxHealth() * float(egg->GetHealthPct() / 100.f);
                    _healHandled = true;
                    damage = 0;
                    CustomSpellValues val;
                    val.AddSpellMod(SPELLVALUE_BASE_POINT0, _eggHealth * 3);
                    me->CastCustomSpell(SPELL_KERI_TWILIGHT_REFLECTION, val, me, TRIGGERED_FULL_MASK);
                    egg->DespawnOrUnsummon();
                    _eggGUID = 0;
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        EnterCombat(target);
                    else
                        EnterEvadeMode();
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() && !_airPhase)
            return;

        _whelpScheduler.Update(diff);
        scheduler.Update(diff);
        events.Update(diff);
        if (me->IsCasting())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ACHIEVEMENT_CHECK:
                {
                    Map::PlayerList const& pList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                        if (Aura* aur = itr->GetSource()->GetAura(SPELL_INTENSE_COLD_TRIGGER))
                            if (aur->GetStackAmount() > 2)
                                aGuids.insert(itr->GetSource()->GetGUIDLow());
                    events.ScheduleEvent(EVENT_ACHIEVEMENT_CHECK, 500ms);
                    break;
                }
                case EVENT_CRYSTALFIRE_BREATH:
                    me->CastSpell(me->GetVictim(), SPELL_CRYSTALFIRE_BREATH, false);
                    events.ScheduleEvent(EVENT_CRYSTALFIRE_BREATH, 14s);
                    break;
                case EVENT_TAIL_SWEEP:
                    me->CastSpell(me, SPELL_TAIL_SWEEP, false);
                    events.ScheduleEvent(EVENT_TAIL_SWEEP, 5s);
                    break;
                case EVENT_CRYSTAL_CHAINS:
                    Talk(SAY_CRYSTAL_NOVA);
                    if (IsHeroic())
                        me->CastSpell(me, SPELL_CRYSTALIZE, false);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                        me->CastSpell(target, SPELL_CRYSTAL_CHAINS, false);
                    events.ScheduleEvent(EVENT_CRYSTAL_CHAINS, DUNGEON_MODE(20s, 11s));
                    break;
            }

            if (me->IsCasting())
                return;
        }

        DoMeleeAttackIfReady();
    }
private:
    bool _healHandled;
    uint64 _eggGUID;
    std::set<uint32> aGuids;
    bool _frenzied;
    bool _airPhase;
    bool _whelpsScheduled;
    std::array<bool, 3> _mythicPhases;
    float _orbAngle;
    std::vector<uint64> _gates;
    std::vector<uint64> _waves;

    TaskScheduler _whelpScheduler;
};

class achievement_intense_cold : public AchievementCriteriaScript
{
    public:
        achievement_intense_cold() : AchievementCriteriaScript("achievement_intense_cold") { }

        bool OnCheck(Player* player, Unit* target) override
        {
            if (!target)
                return false;

            return target->GetAI()->GetData(player->GetGUIDLow());
        }
};

class spell_keristrasza_crystalfire_breath : public AuraScript
{
    PrepareAuraScript(spell_keristrasza_crystalfire_breath);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        float const mapHeight = -16.6072f;
        Unit* target = GetTarget();
        if (!target)
            return;

        InstanceScript* instance = target->GetInstanceScript();
        if (!instance || !instance->IsMythic())
            return;

        Position spawnPosition = target->GetPosition();
        spawnPosition.m_positionZ = mapHeight;
        Creature* keri = instance->GetCreature(DATA_KERISTRASZA_EVENT);
        if (!keri)
            return;

        Creature* trigger = keri->SummonCreature(NPC_KERI_CRYSTALFIRE_BREATH_TRIGGER, spawnPosition);
        if (!trigger)
            return;

        trigger->DespawnOrUnsummon(120s);
        trigger->setFaction(keri->getFaction());
        trigger->SetPassive();

        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, 120000);
        trigger->CastCustomSpell(SPELL_KERI_COLD_FLAMES, val, (Unit*)nullptr, TRIGGERED_FULL_MASK, NullItemRef, (const AuraEffect*)nullptr, keri->GetGUID());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_keristrasza_crystalfire_breath::HandlePeriodic, EFFECT_2,
            SPELL_AURA_PERIODIC_DAMAGE);
    }
};

class spell_water_bubble_head : public AuraScript
{
    PrepareAuraScript(spell_water_bubble_head);

    void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        InstanceScript* instance = target ? target->GetInstanceScript() : nullptr;

        if (target && instance && instance->IsMythic())
        {
            Position spawn = target->GetPosition();
            spawn.m_positionZ = -16.636f;
            Creature* keri = instance->GetCreature(DATA_KERISTRASZA_EVENT);
            if (!keri)
                return;

            Creature* bubbleTrigger = keri->SummonCreature(NPC_KERI_WATER_BUBBLE_TRIGGER, spawn);
            if (!bubbleTrigger)
                return;

            bubbleTrigger->DespawnOrUnsummon(5s);
            bubbleTrigger->setFaction(keri->getFaction());
            bubbleTrigger->SetPassive();
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 15000);
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 200);
            bubbleTrigger->CastCustomSpell(SPELL_KERI_FROSTBOMB_EXPLOSION, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_water_bubble_head::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_keristasza_intense_cold : public AuraScript
{
    PrepareAuraScript(spell_keristasza_intense_cold);

    void HandlePeriodic(AuraEffect const* eff)
    {
        if (!GetTarget())
            return;

        InstanceScript* instance = GetTarget()->GetInstanceScript();
        if (instance && instance->IsMythic() && GetTarget()->IsPlayer())
        {
            PreventDefaultAction();
            CustomSpellValues val;
            uint32 damage = Mythic::CalculateMythicValue(700, (0.04f * instance->GetMythicKeyLevel()));
            val.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
            uint32 spellId = eff->GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
            GetTarget()->CastCustomSpell(spellId, val, GetTarget(), TRIGGERED_FULL_MASK);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_keristasza_intense_cold::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

struct npc_frost_sphere_keristrasza_mythic : public ScriptedAI
{
    npc_frost_sphere_keristrasza_mythic(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        DoZoneInCombat(me, 100.f);
        task.Schedule(500ms, [&](TaskContext func)
        {
            CustomSpellValues val;
            val.AddSpellMod(SPELLVALUE_RADIUS_MOD, 100);
            val.AddSpellMod(SPELLVALUE_MAX_TARGETS, 5);
            val.AddSpellMod(SPELLVALUE_TARGET_PLAYERS_ONLY, 1);
            val.AddSpellMod(SPELLVALUE_BASE_POINT0, 11000);
            me->CastCustomSpell(23187, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
            func.Repeat();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }
private:
    TaskScheduler task;
};

struct go_portal_keristrasza_mythic : public GameObjectAI
{
    go_portal_keristrasza_mythic(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player, bool /*reportUse*/) override
    {
        CustomSpellValues val;
        val.AddSpellMod(SPELLVALUE_AURA_DURATION, 12000);
        player->CastCustomSpell(74310, val, player, TRIGGERED_FULL_MASK);
        go->RemoveFromWorld();
        return true;
    }

    void Reset() override
    {
        task.CancelAll();
        task.Schedule(8s, [&](TaskContext func)
        {
            go->RemoveFromWorld();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        task.Update(diff);
    }

private:
    TaskScheduler task;
};

void AddSC_boss_keristrasza()
{
    RegisterCreatureAI(boss_keristrasza);
    RegisterAuraScript(spell_keristrasza_crystalfire_breath);
    RegisterAuraScript(spell_water_bubble_head);
    RegisterAuraScript(spell_keristasza_intense_cold);
    RegisterCreatureAI(npc_frost_sphere_keristrasza_mythic);
    RegisterGameObjectAI(go_portal_keristrasza_mythic);

    new achievement_intense_cold();
}
