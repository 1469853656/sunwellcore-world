#include "MythicAI.h"
#include "MapReference.h"
#include "Player.h"

enum MythicCastFlags : uint32
{
    MYTHIC_CAST_VICTIM = 0x01,
    MYTHIC_CAST_AOE = 0x02,
    MYTHIC_CAST_SELF = 0x04,
    MYTHIC_CAST_TRIGGERED = 0x08
};

MythicAI::MythicAI(Creature* creature) : ScriptedAI(creature)
{
    instance = me->GetInstanceScript();
    _mythicInstance = instance ? instance->IsMythic() : false;
    _mythicKeyLevel = instance ? instance->GetMythicKeyLevel() : 0;

    task.SetValidator([this]
    {
        return !me->HasUnitState(UNIT_STATE_CASTING);
    });
}

void MythicAI::Reset()
{
    task.CancelAll();
}

void MythicAI::EnterCombat(Unit* who)
{
    MythicDoZoneInCombat(me, 50.f);

    if (IsMythic())
    {
        std::vector<Mythic::MythicSpells> const& spells = me->m_mythicScaling->GetAdditionalSpells();
        for (auto const& data : spells)
        {
            task.Schedule(data.ScheduleTime, [this, data](TaskContext func)
            {
                uint32 spellId = data.SpellId;
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.f, true))
                    DoCast(target, spellId, true);

                func.Repeat(data.RepeatTime);
            });
        }
    }
}

void MythicAI::MythicDoZoneInCombat(Creature* creature /*= NULL*/, float maxRangeToNearestTarget /* = 50.0f*/)
{
    if (!creature)
        creature = me;

    if (!creature->CanHaveThreatList())
        return;

    Map* map = creature->GetMap();
    if (!map->IsDungeon())                                  //use IsDungeon instead of Instanceable, in case battlegrounds will be instantiated
    {
        sLog->outDebug(LOG_FILTER_UNITS, "DoZoneInCombat call for map that isn't an instance (creature entry = %d)", creature->GetTypeId() == TYPEID_UNIT ? creature->ToCreature()->GetEntry() : 0);
        return;
    }

    // Skip creatures in evade mode
    if (!creature->HasReactState(REACT_PASSIVE) && !creature->GetVictim() && !creature->IsInEvadeMode())
    {
        if (Unit* nearTarget = creature->SelectNearestTarget(maxRangeToNearestTarget))
            creature->AI()->AttackStart(nearTarget);
        else if (creature->IsSummon())
        {
            if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
            {
                Unit* target = summoner->getAttackerForHelper();
                if (!target && summoner->CanHaveThreatList() && !summoner->getThreatManager().isThreatListEmpty())
                    target = summoner->getThreatManager().getHostilTarget();
                if (target && (creature->IsFriendlyTo(summoner) || creature->IsHostileTo(target)))
                    creature->AI()->AttackStart(target);
            }
        }
    }

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->GetVictim())
    {
        sLog->outDebug(LOG_FILTER_UNITS, "DoZoneInCombatMythic called for creature that has empty threat list (creature entry = %u)", creature->GetEntry());
        return;
    }

    Map::PlayerList const& playerList = map->GetPlayers();

    if (playerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
    {
        if (Player* player = itr->GetSource())
        {
            if (player->IsGameMaster() || creature->GetDistance(player) > maxRangeToNearestTarget)
                continue;

            if (player->IsAlive())
            {
                creature->SetInCombatWith(player);
                player->SetInCombatWith(creature);
                creature->AddThreat(player, 0.0f);
            }
        }
    }
}

void MythicAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    task.Update(diff,
        std::bind(&ScriptedAI::DoMeleeAttackIfReady, this));
}

bool MythicAI::CanAIAttack(Unit const* who) const
{
    return who && who->GetDistance(me) < 90.f;
}

void MythicAI::AttackStart(Unit* who)
{
    if (IsMythic() && me->m_mythicScaling != nullptr)
    {
        //! We can safely manipulate attacking type due to UpdateVictim()
        //! calling AttackStart method each update.
        //! Switch to melee chase victim if mana less than 10% OR
        //! creature is silenced
        //! Note. make sure to call base class method instead of mythic class
        //! or you will be stuck in infinite loop and crash
        auto type = me->m_mythicScaling->GetMythicType();
        switch (type)
        {
            case Mythic::CREATURE_TYPE_CASTER:
            case Mythic::CREATURE_TYPE_HEALER:
            {
                uint32 const _maxMana = me->GetMaxPower(POWER_MANA);
                uint32 const _currentMana = me->GetPower(POWER_MANA);
                bool const _isSilenced = me->HasAuraType(SPELL_AURA_MOD_SILENCE) || me->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE);
                bool const _hasEnoughMana = _currentMana > (_maxMana * 0.1f);
                if (_hasEnoughMana && !_isSilenced)
                    AttackStartCaster(who, 8.5f);
                else
                    ScriptedAI::AttackStart(who);
                break;
            }
            case Mythic::CREATURE_TYPE_RANGED:
                AttackStartCaster(who, 8.5f);
                break;
            default:
                ScriptedAI::AttackStart(who);
                break;
        }
    }
    else
        ScriptedAI::AttackStart(who);
}

void MythicAI::JustDied(Unit* killer)
{
    ScriptedAI::JustDied(killer);
    me->DeleteThreatList();
    me->CombatStop(true);
}
