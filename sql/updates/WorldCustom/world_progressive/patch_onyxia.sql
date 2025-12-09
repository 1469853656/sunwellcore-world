-- Draconic for Dummies Chapter should drop only with The Only Prescription
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 1 AND `SourceGroup` = 36538 AND `SourceEntry` = 21108;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(1, 36538, 21108, 0, 0, 9, 0, 8620, 0, 0, 0, 0, 0, '', 'Draconic for Dummies Chapter VI will drop only when a player have The Only Prescription (8620) in their quest log');
-- Set Triumphs for Onyxia 10 & 25
UPDATE `creature_loot_template` SET `item` = 47241 WHERE `entry` IN (36538, 10184) AND `item` IN  (40752,40753,45624);

DELETE FROM `gameobject` WHERE `id` = 180717 AND `guid` IN (201570,201571);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(201570, 180717, 1, 1, 1, -8069.05, 1641.72, 27.03, -1.53589, 0, 0, 0.694658, -0.71934, 900, 100, 1, 0),
(201571, 180717, 1, 1, 1, -8069.05, 1641.72, 27.03, -1.53589, 0, 0, 0.694658, -0.71934, 900, 100, 1, 0);
DELETE FROM `gameobject` WHERE `guid` = 49451 AND `id` = 180718;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(49451, 180718, 1, 1, 1, -8069.05, 1641.72, 27.03, -1.53589, 0, 0, 0.694658, -0.71934, 900, 100, 1, 0);


DELETE FROM `creature_queststarter` WHERE `id` = 15180 AND `quest` = 8288;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(15180, 8288);

UPDATE `item_template` SET `Flags` = `Flags` | 2048 WHERE `entry` = 21108;
