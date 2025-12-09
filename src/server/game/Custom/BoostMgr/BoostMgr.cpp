#include "BoostMgr.h"

#include "InstanceScript.h"
#include "MapManager.h"

void BoostMgr::LoadFromDB()
{
    LoadStatValuesFromDB();
    LoadSpellValuesFromDB();
}

void BoostMgr::LoadStatValuesFromDB()
{
    uint32 const oldMSTime = getMSTime();
    auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_BOOST_STAT_VALUES);
    _statValues.clear();
    uint32 statValueCount = 0;

    if (auto const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            auto const statType = static_cast<BoostStatType>(fields[0].GetUInt32());
            uint32 const creatureID = fields[1].GetUInt32();
            if (!sObjectMgr->GetCreatureTemplate(creatureID))
            {
                sLog->outErrorDb("%s", fmt::format("Invalid creature with ID {} passed in `boost_stat_values` table, skipping...", creatureID).c_str());
                continue;
            }

            uint32 const mapID = fields[2].GetUInt32();
            if (!sMapMgr->IsValidMAP(mapID, true))
            {
                sLog->outErrorDb("%s", fmt::format("Invalid map ID {} passed in `boost_stat_values` table, skipping...", mapID).c_str());
                continue;
            }

            int32 const valueMin = fields[4].GetInt32();
            int32 const valueMax = fields[5].GetInt32();
            if (valueMin > valueMax)
            {
                sLog->outErrorDb("%s", fmt::format("Invalid values passed in `boost_stat_values` table (`ValueMin` {} is greater than `ValueMax` {}), skipping...", valueMin, valueMax).c_str());
                continue;
            }

            BoostStatValue statValue;
            statValue.Difficulty = fields[3].GetInt32();
            statValue.Type = statType;
            statValue.ValueMin = valueMin;
            statValue.ValueMax = valueMax;
            _statValues[mapID][creatureID].push_back(statValue);
            ++statValueCount;
        } while (result->NextRow());
    }

    sLog->outString("%s", fmt::format(">> Loaded {} boost stat values in {} ms", statValueCount, GetMSTimeDiffToNow(oldMSTime)).c_str());
}

void BoostMgr::LoadSpellValuesFromDB()
{
    uint32 const oldMSTime = getMSTime();
    auto const stmt = CustomDatabase.GetPreparedStatement(CUSTOM_SEL_BOOST_SPELL_VALUES);
    _spellValues.clear();
    uint32 spellValueCount = 0;

    if (auto const result = CustomDatabase.Query(stmt))
    {
        do
        {
            Field const* fields = result->Fetch();
            auto const spellType = static_cast<BoostSpellType>(fields[0].GetUInt32());
            uint32 const spellID = fields[1].GetUInt32();
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
            if (!spellInfo)
            {
                sLog->outErrorDb("%s", fmt::format("Invalid spell with ID {} passed in `boost_spell_values` table, skipping...", spellID).c_str());
                continue;
            }

            if (spellType == BoostSpellType::Effect0 && !spellInfo->Effects[EFFECT_0].IsEffect())
            {
                sLog->outErrorDb("%s", fmt::format("Spell with ID {} has `boost_spell_values` value EFFECT_0, but spell has no EFFECT_0, skipping...", spellID).c_str());
                continue;
            }

            if (spellType == BoostSpellType::Effect1 && !spellInfo->Effects[EFFECT_1].IsEffect())
            {
                sLog->outErrorDb("%s", fmt::format("Spell with ID {} has `boost_spell_values` value EFFECT_1, but spell has no EFFECT_1, skipping...", spellID).c_str());
                continue;
            }

            if (spellType == BoostSpellType::Effect2 && !spellInfo->Effects[EFFECT_2].IsEffect())
            {
                sLog->outErrorDb("%s", fmt::format("Spell with ID {} has `boost_spell_values` value EFFECT_2, but spell has no EFFECT_2, skipping...", spellID).c_str());
                continue;
            }

            uint32 const mapID = fields[2].GetUInt32();
            if (!sMapMgr->IsValidMAP(mapID, true))
            {
                sLog->outErrorDb("%s", fmt::format("Invalid map ID {} passed in `boost_spell_values` table, skipping...", mapID).c_str());
                continue;
            }

            int32 const valueMin = fields[4].GetInt32();
            int32 const valueMax = fields[5].GetInt32();
            if (valueMin > valueMax)
            {
                sLog->outErrorDb("%s", fmt::format("Invalid values passed in `boost_stat_values` table (`ValueMin` {} is greater than `ValueMax` {}), skipping...", valueMin, valueMax).c_str());
                continue;
            }

            BoostSpellValue spellValue;
            spellValue.Difficulty = fields[3].GetInt32();
            spellValue.Type = spellType;
            spellValue.ValueMin = valueMin;
            spellValue.ValueMax = valueMax;
            _spellValues[mapID][spellID].push_back(spellValue);
            ++spellValueCount;
        } while (result->NextRow());
    }

    sLog->outMisc("%s", fmt::format(">> Loaded {} boost spell values in {} ms", spellValueCount, GetMSTimeDiffToNow(oldMSTime)).c_str());
}

BoostStatValue const* BoostMgr::GetBoostStat(BoostStatType statType, Creature const* creature) const
{
    InstanceScript const* instance = creature->GetInstanceScript();
    if (!instance || !instance->IsBoostEnabledForInstance())
        return nullptr;

    auto const mapIt = _statValues.find(creature->GetMapId());
    if (mapIt == _statValues.end())
        return nullptr;

    uint32 const creatureID = creature->GetEntry();
    auto const creatureIt = mapIt->second.find(creatureID);
    if (creatureIt == mapIt->second.end())
        return nullptr;

    int32 const difficulty = instance->instance->GetDifficulty();
    for (auto const& boostValue : creatureIt->second)
    {
        if (boostValue.Type != statType)
            continue;

        if (boostValue.Difficulty != -1 && boostValue.Difficulty != difficulty)
            continue;

        return &boostValue;
    }

    return nullptr;
}

BoostSpellValue const* BoostMgr::GetBoostSpellValue(BoostSpellType spellValue, SpellInfo const* spellInfo, WorldObject const* source) const
{
    InstanceScript const* instance = source->GetInstanceScript();
    if (!instance || !instance->IsBoostEnabledForInstance())
        return nullptr;

    auto const mapIt = _spellValues.find(source->GetMapId());
    if (mapIt == _spellValues.end())
        return nullptr;

    auto const spellIt = mapIt->second.find(spellInfo->Id);
    if (spellIt == mapIt->second.end())
        return nullptr;

    int32 const difficulty = instance->instance->GetDifficulty();
    for (auto const& boostValue : spellIt->second)
    {
        if (boostValue.Type != spellValue)
            continue;
        
        if (boostValue.Difficulty != -1 && boostValue.Difficulty != difficulty)
            continue;

        return &boostValue;
    }

    return nullptr;
}

BoostSpellType BoostMgr::GetBoostSpellTypeForEffect(SpellEffIndex effIndex)
{
    switch (effIndex)
    {
        case EFFECT_0: return BoostSpellType::Effect0;
        case EFFECT_1: return BoostSpellType::Effect1;
        case EFFECT_2: return BoostSpellType::Effect2;
        default: return BoostSpellType::None;
    }
}

BoostSpellType BoostMgr::GetBoostSpellTypeForEffectModifier(SpellEffIndex effIndex)
{
    switch (effIndex)
    {
        case EFFECT_0: return BoostSpellType::Effect0Modifier;
        case EFFECT_1: return BoostSpellType::Effect1Modifier;
        case EFFECT_2: return BoostSpellType::Effect2Modifier;
        default: return BoostSpellType::None;
    }
}

BoostSpellType BoostMgr::GetBoostSpellTypeForEffectRadius(SpellEffIndex effIndex)
{
    switch (effIndex)
    {
        case EFFECT_0: return BoostSpellType::Effect0Radius;
        case EFFECT_1: return BoostSpellType::Effect1Radius;
        case EFFECT_2: return BoostSpellType::Effect2Radius;
        default: return BoostSpellType::None;
    }
}

void BoostMgr::ApplyBoostsStatOnSpawn(Creature* creature)
{
    if (!creature->IsAlive())
        return;

    creature->SelectLevel(false);

    if (BoostStatValue const* stat = sBoostMgr->GetBoostStat(BoostStatType::Health, creature))
    {
        auto const newHealth = static_cast<uint32>(irand(stat->ValueMin, stat->ValueMax));
        creature->SetMaxHealth(newHealth);
        creature->SetFullHealth();
        creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, static_cast<float>(newHealth));
    }

    if (BoostStatValue const* stat = sBoostMgr->GetBoostStat(BoostStatType::HealthModifier, creature))
    {
        auto const healthModifier = static_cast<uint32>(irand(stat->ValueMin, stat->ValueMax));
        uint32 newHealth = creature->GetMaxHealth();
        newHealth = AddPct(newHealth, healthModifier);
        creature->SetMaxHealth(newHealth);
        creature->SetFullHealth();
        creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, static_cast<float>(newHealth));
    }

    if (BoostStatValue const* stat = sBoostMgr->GetBoostStat(BoostStatType::Mana, creature))
    {
        auto const newMana = static_cast<uint32>(irand(stat->ValueMin, stat->ValueMax));
        creature->SetMaxPower(POWER_MANA, newMana);
        creature->SetPower(POWER_MANA, creature->GetMaxPower(POWER_MANA));
        creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, static_cast<float>(newMana));
    }

    if (BoostStatValue const* stat = sBoostMgr->GetBoostStat(BoostStatType::ManaModifier, creature))
    {
        auto const manaModifier = static_cast<uint32>(irand(stat->ValueMin, stat->ValueMax));
        uint32 newMana = creature->GetMaxPower(POWER_MANA);
        newMana = AddPct(newMana, manaModifier);
        creature->SetMaxPower(POWER_MANA, newMana);
        creature->SetPower(POWER_MANA, creature->GetMaxPower(POWER_MANA));
        creature->SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, static_cast<float>(newMana));
    }
}
