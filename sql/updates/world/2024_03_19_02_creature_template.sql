UPDATE `creature_template` SET `AIName`="" WHERE `entry`=30837;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry`=58152 AND `ConditionTypeOrReference`=31 AND `ElseGroup`=20;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 58152, 0, 20, 31, 0, 3, 30695, 0, 0, 0, 0, '', 'Defense System - Arcane Lightning');

DELETE FROM `spell_proc_event` WHERE `entry` IN (48294, 59301);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `procFlags`, `procEx`, `procPhase`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
(48294, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 500),
(59301, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 500);
