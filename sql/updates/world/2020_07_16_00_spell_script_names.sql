-- Taste for Blood
SET @SPELL_ENTRY := -56636;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_warr_taste_for_blood");
