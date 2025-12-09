

#include "Log.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "ObjectMgr.h"
#include "TemporarySummon.h"
#include "Pet.h"
#include "Player.h"

TempSummon::TempSummon(SummonPropertiesEntry const* properties, WorldObject* owner, bool isWorldObject)
    : Creature(isWorldObject)
    , m_Properties(properties)
    , m_type(TEMPSUMMON_MANUAL_DESPAWN)
    , m_timer(0)
    , m_lifetime(0)
{
    if (owner)
        m_summonerGUID = owner->GetGUID();

    m_unitTypeMask |= UNIT_MASK_SUMMON;
}

Unit* TempSummon::GetSummoner() const
{
    return m_summonerGUID ? ObjectAccessor::GetUnit(*this, m_summonerGUID) : nullptr;
}

Creature* TempSummon::GetSummonerCreatureBase() const
{
    return m_summonerGUID ? ObjectAccessor::GetCreature(*this, m_summonerGUID) : nullptr;
}

void TempSummon::Update(uint32 diff)
{
    Creature::Update(diff);

    if (m_deathState == DEAD)
    {
        UnSummon();
        return;
    }

    if (_linkedToQuest > 0 && _linkedToQuestCheckTimer > 0)
    {
        _linkedToQuestCheckTimer -= static_cast<int32>(diff);
        if (_linkedToQuestCheckTimer <= 0)
        {
            _linkedToQuestCheckTimer = 500;
            if (_linkedToQuest)
            {
                if (Player* owner = GetSummoner()->ToPlayer())
                {
                    QuestStatus const status = owner->GetQuestStatus(_linkedToQuest);
                    if (status != QUEST_STATUS_COMPLETE && status != QUEST_STATUS_INCOMPLETE)
                    {
                        UnSummon();
                        return;
                    }
                }
            }
        }
    }

    switch (m_type)
    {
        case TEMPSUMMON_MANUAL_DESPAWN:
        case TEMPSUMMON_DESPAWNED:
            break;
        case TEMPSUMMON_TIMED_DESPAWN:
        {
            if (m_timer <= diff)
            {
                UnSummon();
                return;
            }

            m_timer -= diff;
            break;
        }
        case TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT:
        {
            if (!IsInCombat())
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }

                m_timer -= diff;
            }
            else if (m_timer != m_lifetime)
                m_timer = m_lifetime;

            break;
        }

        case TEMPSUMMON_CORPSE_TIMED_DESPAWN:
        {
            if (m_deathState == CORPSE)
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }

                m_timer -= diff;
            }
            break;
        }
        case TEMPSUMMON_CORPSE_DESPAWN:
        {
            // if m_deathState is DEAD, CORPSE was skipped
            if (m_deathState == CORPSE || m_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            break;
        }
        case TEMPSUMMON_DEAD_DESPAWN:
        {
            if (m_deathState == DEAD)
            {
                UnSummon();
                return;
            }
            break;
        }
        case TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN:
        {
            // if m_deathState is DEAD, CORPSE was skipped
            if (m_deathState == CORPSE || m_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            if (!IsInCombat())
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }
                else
                    m_timer -= diff;
            }
            else if (m_timer != m_lifetime)
                m_timer = m_lifetime;
            break;
        }
        case TEMPSUMMON_TIMED_OR_DEAD_DESPAWN:
        {
            // if m_deathState is DEAD, CORPSE was skipped
            if (m_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            if (!IsInCombat() && IsAlive())
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }
                else
                    m_timer -= diff;
            }
            else if (m_timer != m_lifetime)
                m_timer = m_lifetime;
            break;
        }
        default:
            UnSummon();
            sLog->outError("Temporary summoned creature (entry: %u) have unknown type %u of ", GetEntry(), m_type);
            break;
    }
}

void TempSummon::InitStats(uint32 duration)
{
    ASSERT(!IsPet());

    m_timer    = duration;
    m_lifetime = duration;

    if (m_type == TEMPSUMMON_MANUAL_DESPAWN)
        m_type = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

    Unit* owner = GetSummoner();
    if (owner)
    {
        if (IsTrigger() && m_spells[0])
        {
            setFaction(owner->getFaction());
            SetLevel(owner->getLevel());
            if (owner->GetTypeId() == TYPEID_PLAYER)
                m_ControlledByPlayer = true;
        }

        if (owner->GetTypeId() == TYPEID_PLAYER)
            m_CreatedByPlayer = true;
    }

    if (!m_Properties)
        return;

    if (owner)
    {
        if (uint32 slot = m_Properties->Slot)
        {
            if (owner->m_SummonSlot[slot] && owner->m_SummonSlot[slot] != GetGUID())
            {
                Creature* oldSummon = GetMap()->GetCreature(owner->m_SummonSlot[slot]);
                if (oldSummon && oldSummon->IsSummon())
                    oldSummon->ToTempSummon()->UnSummon();
            }
            owner->m_SummonSlot[slot] = GetGUID();
        }
    }

    if (m_Properties->Faction)
        setFaction(m_Properties->Faction);
    else if (IsVehicle() && owner) // properties should be vehicle
        setFaction(owner->getFaction());

    // TODO: implement it in database
    switch (GetEntry())
    {
        case 28521: // Nass
            _linkedToQuest = 12630; // Kickin' Nass and Takin' Manes
            break;
        default:
            break;
    }
}

void TempSummon::InitSummon()
{
    Unit* owner = GetSummoner();
    if (owner)
    {
        if (owner->GetTypeId() == TYPEID_UNIT && owner->ToCreature()->IsAIEnabled)
            owner->ToCreature()->AI()->JustSummoned(this);
    }

    // Xinef: Allow to call this hook when npc is summoned by gameobject, in this case pass this as summoner to avoid possible null checks
    if (IsAIEnabled)
        AI()->IsSummonedBy(owner);
}

void TempSummon::SetTempSummonType(TempSummonType type)
{
    m_type = type;
}

void TempSummon::UnSummon(uint32 msTime)
{
    if (msTime)
    {
        ForcedUnsummonDelayEvent* pEvent = new ForcedUnsummonDelayEvent(*this);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(msTime));
        return;
    }

    // Dont allow to call this function twice (possible)
    if (m_type == TEMPSUMMON_DESPAWNED)
        return;
    SetTempSummonType(TEMPSUMMON_DESPAWNED);

    //ASSERT(!IsPet());
    if (IsPet())
    {
        ((Pet*)this)->Remove(PET_SAVE_NOT_IN_SLOT);
        ASSERT(!IsInWorld());
        return;
    }
    else if (IsGuardian())
        ((Guardian*)this)->SaveSpells();

    Unit* owner = GetSummoner();
    if (owner && owner->GetTypeId() == TYPEID_UNIT && owner->ToCreature()->IsAIEnabled)
        owner->ToCreature()->AI()->SummonedCreatureDespawn(this);

    AddObjectToRemoveList();
}

bool ForcedUnsummonDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.UnSummon();
    return true;
}

void TempSummon::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    if (m_Properties)
        if (uint32 slot = m_Properties->Slot)
            if (Unit* owner = GetSummoner())
                if (owner->m_SummonSlot[slot] == GetGUID())
                    owner->m_SummonSlot[slot] = 0;

    //if (GetOwnerGUID())
    //    sLog->outError("Unit %u has owner guid when removed from world", GetEntry());

    Creature::RemoveFromWorld();
}

Minion::Minion(SummonPropertiesEntry const* properties, WorldObject* owner, bool isWorldObject)
    : TempSummon(properties, owner, isWorldObject)
    , m_ownerGUID(owner->GetGUID())
    , m_owner(owner)
{
    ASSERT(m_owner);
    m_unitTypeMask |= UNIT_MASK_MINION;
    m_followAngle = PET_FOLLOW_ANGLE;
    if (properties && properties->Type == SUMMON_TYPE_MINIPET)
        m_followAngle = COMPANION_FOLLOW_ANGLE;
}

void Minion::InitStats(uint32 duration)
{
    TempSummon::InitStats(duration);

    SetReactState(REACT_PASSIVE);

    Unit* owner = GetOwner();

    if (!owner)
        owner = GetOwnerForInit();

    if (owner)
    {
        SetCreatorGUID(owner->GetGUID());
        setFaction(owner->getFaction());
        owner->SetMinion(this, true);
    }
    else // Owner not found, despawn
        DespawnOrUnsummon(1);
}

void Minion::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    if (Unit* owner = GetOwner())
        owner->SetMinion(this, false);

    TempSummon::RemoveFromWorld();
}

Unit* Minion::GetOwner() const
{
    return GetMap()->GetUnit(m_ownerGUID);
}

bool Minion::IsGuardianPet() const
{
    return IsPet() || (m_Properties && m_Properties->Category == SUMMON_CATEGORY_PET);
}

void Minion::setDeathState(DeathState s, bool despawn)
{
    Creature::setDeathState(s, despawn);
    if (s == JUST_DIED && IsGuardianPet())
        if (Unit* owner = GetOwner())
            if (owner->GetTypeId() == TYPEID_PLAYER && owner->GetMinionGUID() == GetGUID())
                for (Unit::ControlSet::const_iterator itr = owner->m_Controlled.begin(); itr != owner->m_Controlled.end(); ++itr)
                    if ((*itr)->IsAlive() && (*itr)->GetEntry() == GetEntry())
                    {
                        owner->SetMinionGUID((*itr)->GetGUID());
                        owner->SetPetGUID((*itr)->GetGUID());
                        owner->ToPlayer()->CharmSpellInitialize();
                    }
}

Guardian::Guardian(SummonPropertiesEntry const* properties, WorldObject* owner, bool isWorldObject)
    : Minion(properties, owner, isWorldObject)
{
    m_unitTypeMask |= UNIT_MASK_GUARDIAN;
    if (properties && properties->Type == SUMMON_TYPE_PET)
    {
        m_unitTypeMask |= UNIT_MASK_CONTROLABLE_GUARDIAN;
        InitCharmInfo();
    }
}

void Guardian::InitStats(uint32 duration)
{
    Minion::InitStats(duration);

    Unit* owner = GetOwner();

    if (!owner)
        owner = GetOwnerForInit();

    if (owner)
    {
        InitStatsForLevel(owner->getLevel());

        if (owner->GetTypeId() == TYPEID_PLAYER && HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
            m_charmInfo->InitCharmCreateSpells();
        else
            SetReactState(REACT_AGGRESSIVE);
    }
    else // Owner not found, despawn
        DespawnOrUnsummon(1);

    // TODO: reimplement
    //SetReactAndCommandState();
}

void Guardian::InitSummon()
{
    TempSummon::InitSummon();

    Unit* owner = GetOwner();
    if (!owner)
        owner = GetOwnerForInit();

    if (owner && owner->IsPlayer() && owner->GetMinionGUID() == GetGUID() && !owner->GetCharmGUID())
        owner->ToPlayer()->CharmSpellInitialize();
}

void Guardian::SaveSpells()
{
    if (!GetOwner() || !GetOwner()->ToPlayer() || !GetCharmInfo() || !GetCharmInfo()->GetPetActionBar())
        return;

    Player* owner        = GetOwner()->ToPlayer();
    auto& guardianSpells = owner->GetGuardianSpells();
    auto trans = CharacterDatabase.BeginTransaction();
    for (UnitActionBarEntry const& actionBar : GetCharmInfo()->GetPetActionBar())
        if (actionBar.IsActionBarForSpell() && actionBar.GetAction())
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(actionBar.GetAction());
            if (!spellInfo || !spellInfo->IsAutocastable())
                continue;
            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUARDIAN_SPELLS);
            stmt->setUInt32(0, owner->GetGUIDLow());
            stmt->setUInt32(1, actionBar.GetAction());
            stmt->setUInt32(2, actionBar.GetType());
            stmt->setUInt32(3, GUARDIAN_VALUE_TYPE_SPELL);
            trans->Append(stmt);
            guardianSpells.erase(actionBar.GetAction());
            guardianSpells.emplace(actionBar.GetAction(), static_cast<uint32>(actionBar.GetType()));
        }

    auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUARDIAN_SPELLS);
    stmt->setUInt32(0, owner->GetGUIDLow());
    stmt->setUInt32(1, GetEntry());
    stmt->setUInt32(2, GetReactState());
    stmt->setUInt32(3, GUARDIAN_VALUE_TYPE_REACTSTATE);
    trans->Append(stmt);
    owner->InsertOrUpdateGuardianState(GetEntry(), GetReactState());

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUARDIAN_SPELLS);
    stmt->setUInt32(0, owner->GetGUIDLow());
    stmt->setUInt32(1, GetEntry());
    stmt->setUInt32(2, GetCharmInfo()->GetCommandState());
    stmt->setUInt32(3, GUARDIAN_VALUE_TYPE_COMMANDSTATE);
    trans->Append(stmt);
    owner->InsertOrUpdateGuardianState(GetEntry(), GetCharmInfo()->GetCommandState());

    CharacterDatabase.CommitTransaction(trans);
}

void Guardian::SetReactAndCommandState()
{
    if (!GetOwner() || !GetOwner()->IsInWorld() || !GetOwner()->IsPlayer())
    {
        SetReactState(REACT_AGGRESSIVE);
        return;
    }
    else
    {
        Player* owner = GetOwner()->ToPlayer();
        auto& states  = owner->GetGuardianStates();
        auto it       = std::find_if(std::begin(states), std::end(states), [this](GuardianStates& tempState) { return tempState.GuardianEntry == GetEntry(); });
        if (it != std::end(states))
        {
            SetReactState(it->ReactState);
            owner->GetSession()->HandlePetActionHelper(this, owner->GetTarget(), it->CommandState, ACT_COMMAND, 0);
        }
        else
            SetReactState(REACT_AGGRESSIVE);
    }
}

Puppet::Puppet(SummonPropertiesEntry const* properties, WorldObject* owner)
    : Minion(properties, owner, false)
{
    ASSERT(IS_PLAYER_GUID(m_ownerGUID));
    m_unitTypeMask |= UNIT_MASK_PUPPET;
}

void Puppet::InitStats(uint32 duration)
{
    Minion::InitStats(duration);
    SetLevel(GetOwner()->getLevel());
    SetReactState(REACT_PASSIVE);
}

void Puppet::InitSummon()
{
    Minion::InitSummon();
    if (!SetCharmedBy(GetOwner(), CHARM_TYPE_POSSESS))
    {
        if (Unit* owner = GetOwner(); owner && owner->IsPlayer())
            sLog->outMisc("Puppet::InitSummon (A1) - %u, %u, %u, %u, %u, %u, %p, %u, %u, %u, %u", owner->GetTypeId(), owner->GetEntry(), owner->GetUnitTypeMask(), owner->GetGUIDLow(), owner->GetMapId(), owner->GetInstanceId(), owner->FindMap(), owner->IsInWorld() ? 1 : 0, owner->IsDuringRemoveFromWorld() ? 1 : 0, owner->ToPlayer()->IsBeingTeleported() ? 1 : 0, owner->isBeingLoaded() ? 1 : 0);
        else
        {
            sLog->outMisc("Puppet::InitSummon (B1)");
            //ASSERT(false); // ZOMG!
        }
    }
}

void Puppet::Update(uint32 time)
{
    Minion::Update(time);
    //check if caster is channelling?
    if (IsInWorld())
    {
        if (!IsAlive())
        {
            UnSummon();
            // TODO: why long distance .die does not remove it
        }
    }
}

void Puppet::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    RemoveCharmedBy(nullptr);
    Minion::RemoveFromWorld();
}
