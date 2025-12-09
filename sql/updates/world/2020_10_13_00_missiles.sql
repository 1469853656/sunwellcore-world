SET @SPELL_ENTRY := -5143;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_mage_arcane_missiles");
SET @SPELL_ENTRY := 44401;
DELETE FROM `spell_proc_event` WHERE `entry`=@SPELL_ENTRY;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `procPhase`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(@SPELL_ENTRY, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0);
