SET @SPELL_ENTRY := 60063;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_item_sundial_of_the_exiled");
UPDATE `spell_proc_event` SET `procFlags`=0 WHERE `entry`=@SPELL_ENTRY;
