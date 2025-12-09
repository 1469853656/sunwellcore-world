#ifndef BOOSTMGR_H
#define BOOSTMGR_H

#include "SharedDefines.h"
#include "Player.h"

enum class BoostStatType
{
    None,
    Health,
    Mana,
    MainhandDamage,
    OffhandDamage,
    Level,
    HealthModifier,
    ManaModifier,
    MainhandDamageModifier,
    OffhandDamageModifier,
};

struct BoostStatValue
{
    int32 Difficulty = 0;
    BoostStatType Type = BoostStatType::None;
    int32 ValueMin = 0;
    int32 ValueMax = 0;
};

enum class BoostSpellType
{
    None,
    Effect0,
    Effect1,
    Effect2,
    AuraDuration,
    Effect0Modifier,
    Effect1Modifier,
    Effect2Modifier,
    Effect0Radius,
    Effect1Radius,
    Effect2Radius,
};

struct BoostSpellValue
{
    int32 Difficulty = 0;
    BoostSpellType Type = BoostSpellType::None;
    int32 ValueMin = 0;
    int32 ValueMax = 0;
};

using BoostStatValuesContainer = std::map<uint32 /*MapID*/, std::unordered_map<uint32 /*CreatureID*/, std::vector<BoostStatValue>>>;
using BoostSpellValuesContainer = std::map<uint32 /*MapID*/, std::unordered_map<uint32 /*SpellID*/, std::vector<BoostSpellValue>>>;
struct BoostMgr : public Sunwell::Singleton<BoostMgr>
{
    friend struct Sunwell::Singleton<BoostMgr>;

    void LoadFromDB();

    BoostStatValue const* GetBoostStat(BoostStatType statType, Creature const* creature) const;
    BoostSpellValue const* GetBoostSpellValue(BoostSpellType spellValue, SpellInfo const* spellInfo, WorldObject const* source) const;

    static BoostSpellType GetBoostSpellTypeForEffect(SpellEffIndex effIndex);
    static BoostSpellType GetBoostSpellTypeForEffectModifier(SpellEffIndex effIndex);
    static BoostSpellType GetBoostSpellTypeForEffectRadius(SpellEffIndex effIndex);
    static void ApplyBoostsStatOnSpawn(Creature* creature);

    private:
        void LoadStatValuesFromDB();
        void LoadSpellValuesFromDB();

        BoostStatValuesContainer _statValues;
        BoostSpellValuesContainer _spellValues;
};

#define sBoostMgr BoostMgr::instance()

#endif
