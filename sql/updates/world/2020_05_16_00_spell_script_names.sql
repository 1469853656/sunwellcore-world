-- Grand Champions, Fan of Knives
SET @SPELL_ENTRY := 67706;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_grand_champions_fan_of_knives");

-- Grand Champions, Intercept
SET @SPELL_ENTRY := 67573;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_grand_champions_intercept_trigger");

-- Grand Champions, Hex of Mending
SET @SPELL_ENTRY := 67534;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_grand_champions_hex_of_mending");

-- Grand Champions, Rolling Throw
SET @SPELL_ENTRY := 67560;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_grand_champions_rolling_throw");
