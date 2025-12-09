-- Stormchops
SET @SPELL_ENTRY := 43730;
DELETE FROM `spell_proc_event` WHERE `entry`=@SPELL_ENTRY;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `procPhase`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(@SPELL_ENTRY, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8000);
