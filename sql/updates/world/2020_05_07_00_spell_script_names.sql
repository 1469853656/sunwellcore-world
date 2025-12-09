-- Mount Speed
SET @SPELL_ENTRY := 48776;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_riding_crop");
