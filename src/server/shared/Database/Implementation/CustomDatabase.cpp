#include "CustomDatabase.h"
#include "MySQLPreparedStatement.h"

void CustomDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_CUSTOMDATABASE_STATEMENTS);

    // Custom Data
    PrepareStatement(CUSTOM_INS_CHARACTER_DATA, "INSERT INTO characters_data (CharacterID, CurrentRacial, CustomCharacterFlags, MultipleSpecsFlag, PremiumBoostFlag, TokenUsePhase, ProfessionTokenUsePhase, XPRate) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(CUSTOM_UPD_CHARACTER_DATA_RACIAL, "UPDATE characters_data SET CustomCharacterFlags = CustomCharacterFlags &~ ?, CurrentRacial = ? WHERE CharacterID = ?", CONNECTION_ASYNC);
    PrepareStatement(CUSTOM_SEL_CHARACTER_DATA, "SELECT CurrentRacial, CustomCharacterFlags, MultipleSpecsFlag, PremiumBoostFlag, TokenUsePhase, ProfessionTokenUsePhase, XPRate FROM characters_data WHERE CharacterID = ?", CONNECTION_ASYNC);
    PrepareStatement(CUSTOM_DEL_CHARACTER_DATA, "DELETE FROM characters_data WHERE CharacterID = ?", CONNECTION_ASYNC);
    PrepareStatement(CUSTOM_REP_CHARACTER_DATA, "REPLACE INTO characters_data (CharacterID, CurrentRacial, CustomCharacterFlags, MultipleSpecsFlag, PremiumBoostFlag, TokenUsePhase, ProfessionTokenUsePhase, XPRate) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PrepareStatement(CUSTOM_SET_CHARACTER_DATA_FLAG, "UPDATE characters_data SET CustomCharacterFlags = CustomCharacterFlags | ? WHERE CharacterID = ?", CONNECTION_ASYNC);
    PrepareStatement(CUSTOM_UNSET_CHARACTER_DATA_FLAG, "UPDATE characters_data SET CustomCharacterFlags = CustomCharacterFlags &~ ? WHERE CharacterID = ?", CONNECTION_ASYNC);

    // Custom String
    PrepareStatement(CUSTOM_SEL_CUSTOM_STRING, "SELECT entry, content FROM custom_string", CONNECTION_SYNCH);

    // Boost Values
    PrepareStatement(CUSTOM_SEL_INSTANCE_BOOST_VALUES, "SELECT InstanceID, BossID, BoostID, Value1, Value2 FROM instance_boost_values", CONNECTION_SYNCH);

    // XP Rates
    PrepareStatement(CUSTOM_SEL_XP_RATE_BRACKETS, "SELECT `Level`, `ExperienceRate` FROM `xp_rate_brackets` ORDER BY `Level` DESC", CONNECTION_SYNCH);

    // Premade Sets
    PrepareStatement(CUSTOM_SEL_PREMADE_SETS, "SELECT `SetName`, `SetLevel`, `SetClass`, `SetSpec`, `SetMask` FROM `premade_sets`", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_PREMADE_SET_EQUIPMENT_ITEMS, "SELECT `SetName`, `EquipmentSlot`, `ItemID`, `EnchantmentID`, `GemID_0`, `GemID_1`, `GemID_2`, `PrismaticGemID` FROM `premade_set_equipment_items`", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_PREMADE_SET_BAG_ITEMS, "SELECT `SetName`, `ItemID`, `ItemCount`, `EnchantmentID`, `GemID_0`, `GemID_1`, `GemID_2`, `PrismaticGemID` FROM `premade_set_bag_items`", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_PREMADE_SET_TALENTS, "SELECT `SetName`, `TalentID`, `TalentRank` FROM `premade_set_talents`", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_PREMADE_SET_GLYPHS, "SELECT `SetName`, `GlyphID`, `GlyphIndex` FROM `premade_set_glyphs`", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_PRESET_GROUPS, "SELECT `GroupID`, `GroupName` FROM `preset_groups`", CONNECTION_SYNCH);

    // BoostMgr
    PrepareStatement(CUSTOM_SEL_BOOST_STAT_VALUES, "SELECT CAST(`StatType` AS UNSIGNED), `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax` FROM `boost_stat_values`", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_BOOST_SPELL_VALUES, "SELECT CAST(`SpellType` AS UNSIGNED), `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax` FROM `boost_spell_values`", CONNECTION_SYNCH);

    // Transmogification
    PrepareStatement(CUSTOM_SEL_TRANSMOG_ENCHANT_NAMES, "SELECT `EnchantmentID`, `Name` FROM `transmog_enchant_names` ORDER BY `EnchantmentID` ASC", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_TRANSMOG_SETS, "SELECT `SetID`, `SetName` FROM `transmog_sets` ORDER BY `SetID` ASC", CONNECTION_SYNCH);
    PrepareStatement(CUSTOM_SEL_TRANSMOG_SET_ITEMS, "SELECT `SetID`, `ItemID`, `SlotID` FROM `transmog_set_items`", CONNECTION_SYNCH);
}
