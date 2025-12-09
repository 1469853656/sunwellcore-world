
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "nexus.h"
#include "Player.h"
#include "PassiveAI.h"
#include "MythicAI.h"

enum OrmorokEnums
{
    SPELL_CRYSTAL_SPIKES                    = 47958,
    SPELL_CRYSTAL_SPIKE_DAMAGE              = 47944,
    SPELL_CRYSTAL_SPIKE_PREVISUAL           = 50442,
    SPELL_SPELL_REFLECTION                  = 47981,
    SPELL_TRAMPLE                           = 48016,
    SPELL_FRENZY                            = 48017,
    SPELL_SUMMON_CRYSTALLINE_TANGLER        = 61564,
    SPELL_CRYSTAL_CHAINS                    = 47698,
    SPELL_TRAMPLE_MYTHIC                    = 48783
};

enum OrmorokYells
{
    SAY_AGGRO                               = 1,
    SAY_DEATH                               = 2,
    SAY_REFLECT                             = 3,
    SAY_CRYSTAL_SPIKES                      = 4,
    SAY_KILL                                = 5,
    EMOTE_FRENZY                            = 6
};

enum OrmorokEvents
{
    EVENT_ORMOROK_CRYSTAL_SPIKES        = 1,
    EVENT_ORMOROK_TRAMPLE,
    EVENT_ORMOROK_SPELL_REFLECTION,
    EVENT_ORMOROK_SUMMON,
    EVENT_ORMOROK_SUMMON_SPIKES,
    EVENT_KILL_TALK
};

enum OrmorokMisc
{
    NPC_CRYSTAL_SPIKE                        = 27099,
    NPC_CRYSTALLINE_TANGLER                  = 32665,
    GO_CRYSTAL_SPIKE                         = 188537,
    NPC_ORMOROK_MINION_MYTHIC                = 91706
};

constexpr uint32 SPELL_EARTHQUAKE_ORMOROK_MYTHIC{ 33919 };
constexpr uint32 SPELL_ENRAGE_ORMOROK_MYTHIC{ 54356 };
struct boss_ormorok : public BossAI
{
    boss_ormorok(Creature* creature) : BossAI(creature, DATA_ORMOROK_EVENT)
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
        std::list<GameObject*> _spikes;
        me->GetGameObjectListWithEntryInGrid(_spikes, GO_CRYSTAL_SPIKE, 100.f);
        for (GameObject* go : _spikes)
            go->RemoveFromWorld();

        _frenzied = false;
        _spikesRepeatCounter = 0;
        _spikesCount = 0;
        if (instance && instance->IsMythic())
            me->SetCanMissSpells(false);

        BossAI::Reset();
    }

    void EnterCombat(Unit* who) override
    {
        Talk(SAY_AGGRO);
        BossAI::EnterCombat(who);

        events.ScheduleEvent(EVENT_ORMOROK_CRYSTAL_SPIKES, instance->IsMythic() ? 5s : 12s);

        if (!instance->IsMythic())
        {
            events.ScheduleEvent(EVENT_ORMOROK_SPELL_REFLECTION, 30s);
            events.ScheduleEvent(EVENT_ORMOROK_TRAMPLE, 10s);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_ORMOROK_SUMMON, 17s);
        }
        else
        {
            scheduler.Schedule(10s, [&](TaskContext func)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 5000);
                val.AddSpellMod(SPELLVALUE_DISABLE_SPELL_STEAL_DISPEL, 1);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 20);
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, 40);
                me->CastCustomSpell(SPELL_ENRAGE_ORMOROK_MYTHIC, val, me, TRIGGERED_FULL_MASK);
                me->MonsterYell("UNSTOPPABBLLLLEE!!!", 0, nullptr);
                func.Repeat(20s);
            });

            scheduler.Schedule(8s, [&](TaskContext func)
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_AURA_DURATION, 2000);
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 6500);
                me->CastCustomSpell(SPELL_EARTHQUAKE_ORMOROK_MYTHIC, val, (Unit*)nullptr, TRIGGERED_FULL_MASK);
                float const dist = me->GetObjectSize() + 4.5f;

                for (uint32 i = 0; i < 2; ++i)
                {
                    float const _angle = frand(0.f, float(2.f * M_PI));
                    Position spawnPos = me->GetPosition();
                    spawnPos.m_positionX = spawnPos.GetPositionX() + dist * std::cos(_angle);
                    spawnPos.m_positionY = spawnPos.GetPositionY() + dist * std::sin(_angle);
                    spawnPos.m_positionZ = me->GetMap()->GetHeight(spawnPos.m_positionX, spawnPos.m_positionY, spawnPos.m_positionZ);
                    Creature* minion = me->SummonCreature(NPC_ORMOROK_MINION_MYTHIC, spawnPos, TEMPSUMMON_CORPSE_DESPAWN);
                    if (minion && minion->IsAIEnabled)
                    {
                        minion->AI()->DoZoneInCombat();
                        minion->setFaction(me->getFaction());
                    }
                }

                func.Repeat(20s);
            });
        }
    }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_DEATH);
        BossAI::JustDied(killer);

        if (!killer)
            return;

        Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;

        Position const _chestSpawnPos{ 284.297302f, -232.669556f, -8.254621f, 2.792508f };
        if ( instance && instance->IsMythic() )
        {
            uint32 _go = instance->GetTeamIdInInstance() == TEAM_ALLIANCE ?
                Mythic::GO_MYTHIC_CONSUMABLE_CHEST_ALLIANCE : Mythic::GO_MYTHIC_CONSUMABLE_CHEST_HORDE;

            player->SummonGameObject(_go, _chestSpawnPos, 0.f, 0.f, 0.f, 0.f, 0);
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (events.GetNextEventTime(EVENT_KILL_TALK) == 0)
        {
            Talk(SAY_KILL);
            events.ScheduleEvent(EVENT_KILL_TALK, 6s);
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (!_frenzied && me->HealthBelowPctDamaged(25, damage))
        {
            _frenzied = true;
            if (instance->IsMythic())
            {
                //! Decrease the values a little bit, otherwise he obliterates the tank
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT0, 50);
                me->CastSpell(me, SPELL_FRENZY, true);
            }
            else
                me->CastSpell(me, SPELL_FRENZY, true);

            Talk(EMOTE_FRENZY);
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
                case EVENT_ORMOROK_TRAMPLE:
                    if (instance->IsMythic())
                        me->CastSpell(me, SPELL_TRAMPLE_MYTHIC, false);
                    else
                        me->CastSpell(me, SPELL_TRAMPLE, false);
                    events.ScheduleEvent(EVENT_ORMOROK_TRAMPLE, 10s);
                    break;
                case EVENT_ORMOROK_SPELL_REFLECTION:
                    Talk(SAY_REFLECT);
                    me->CastSpell(me, SPELL_SPELL_REFLECTION, false);
                    events.ScheduleEvent(EVENT_ORMOROK_SPELL_REFLECTION, 30s);
                    break;
                case EVENT_ORMOROK_SUMMON:
                    if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 50.0f, true))
                        me->CastSpell(target, SPELL_SUMMON_CRYSTALLINE_TANGLER, true);
                    events.ScheduleEvent(EVENT_ORMOROK_SUMMON, 17s);
                    break;
                case EVENT_ORMOROK_CRYSTAL_SPIKES:
                    Talk(SAY_CRYSTAL_SPIKES);
                    me->CastSpell(me, SPELL_CRYSTAL_SPIKES, false);
                    _spikesCount = 0;
                    if (instance->IsMythic())
                    {
                        ++_spikesRepeatCounter;
                        if (_spikesRepeatCounter % 2 == 0)
                        {
                            DoCastAOE(38540, true);
                            scheduler.Schedule(1500ms, [&](TaskContext /*func*/)
                            {
                                if (GameObject* gameobject = me->SummonGameObject(GO_CRYSTAL_SPIKE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3500))
                                    gameobject->SetGoState(GO_STATE_ACTIVE);

                                me->CastSpell(me, SPELL_CRYSTAL_SPIKE_DAMAGE, false);
                            });
                        }
                    }

                    events.ScheduleEvent(EVENT_ORMOROK_SUMMON_SPIKES, 300ms);
                    events.ScheduleEvent(EVENT_ORMOROK_CRYSTAL_SPIKES, 20s);
                    break;
                case EVENT_ORMOROK_SUMMON_SPIKES:
                    if (++_spikesCount > 9)
                        break;
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        float o = rand_norm() * 2.0f * M_PI;
                        float x = me->GetPositionX() + 5.0f * _spikesCount * cos(o);
                        float y = me->GetPositionY() + 5.0f * _spikesCount * sin(o);
                        me->SummonCreature(NPC_CRYSTAL_SPIKE, x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ() + 5.0f), 0, TEMPSUMMON_TIMED_DESPAWN, 7000);
                    }
                    events.ScheduleEvent(EVENT_ORMOROK_SUMMON_SPIKES, 200ms);
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea();
    }

    bool CheckEvadeIfOutOfCombatArea() const override
    {
        return me->GetHomePosition().GetExactDist2d(me) > 40.f;
    }

private:
    uint32 _spikesRepeatCounter;
    uint8 _spikesCount;
    bool _frenzied;
};

constexpr uint32 SPELL_ORMOROK_MINION_ROCK_RUMBLE{ 38777 };
struct npc_ormorok_minion_mythic : public MythicAI
{
    npc_ormorok_minion_mythic(Creature* creature) : MythicAI(creature) { }

    void Reset() override
    {
        me->SetCanMissSpells(false);
        me->SetControlled(true, UNIT_STATE_STUNNED);
        task.Schedule(1500ms, [&](TaskContext /*func*/)
        {
            me->SetControlled(false, UNIT_STATE_STUNNED);
            MythicDoZoneInCombat(me, 100.f);
        });

        task.Schedule(randseconds(5, 13), [&](TaskContext func)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1U, [&](Unit* obj)
            {
                return obj->IsPlayer() && !obj->IsFalling();
            }))
            {
                CustomSpellValues val;
                val.AddSpellMod(SPELLVALUE_BASE_POINT1, 800);
                val.AddSpellMod(SPELLVALUE_SPELL_RANGE_MIN, 0);
                val.AddSpellMod(SPELLVALUE_SPELL_RANGE, 40.f);
                me->CastCustomSpell(SPELL_ORMOROK_MINION_ROCK_RUMBLE, val, target, TRIGGERED_FULL_MASK);
            }

            func.Repeat(randseconds(8, 20));
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

struct npc_crystal_spike : public NullCreatureAI
{
    npc_crystal_spike(Creature* creature) : NullCreatureAI(creature), _gameObjectGUID(0) { }

    void Reset() override
    {
        scheduler.CancelAll();
        if (GameObject* gameobject = me->SummonGameObject(GO_CRYSTAL_SPIKE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3500))
            _gameObjectGUID = gameobject->GetGUID();

        scheduler.Schedule(2s, [&](TaskContext /*func*/)
        {
            if (GameObject* gameobject = ObjectAccessor::GetGameObject(*me, _gameObjectGUID))
                gameobject->SetGoState(GO_STATE_ACTIVE);

            me->CastSpell(me, SPELL_CRYSTAL_SPIKE_DAMAGE, false);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
private:
    TaskScheduler scheduler;
    uint64 _gameObjectGUID;
};

void AddSC_boss_ormorok()
{
    RegisterCreatureAI(boss_ormorok);
    RegisterCreatureAI(npc_crystal_spike);
    RegisterCreatureAI(npc_ormorok_minion_mythic);
}
