-- DB/Creature: Remove incorrectly spawned unused creature Yor (10237) https://github.com/TrinityCore/TrinityCore/commit/0bec5baa7a33047adf439accda93fe8a4c042657 
-- DB/Creature: Remove static spawn of Farahlon Crumbler https://github.com/TrinityCore/TrinityCore/commit/a183ca5bf5c424c4ac0940542a5b0942f242fb2e  
-- DB/Creature: Add two missing Steam Cloud https://github.com/TrinityCore/TrinityCore/commit/ed3d9b69dda0c58b1079b74db6759934a905f9e0
-- DB/Creature: Update Arcane Vortex's auras & stats & add two missing spawns https://github.com/TrinityCore/TrinityCore/commit/d47fdeb78283b0fb7c982c2803d4709f7b4a8991
UPDATE `creature_template` SET `skinloot` = 0 WHERE `entry` = 10237;
DELETE FROM `creature` WHERE `id` = 21077;

DELETE FROM `creature` WHERE `guid` IN (310009, 310010) AND `id` = 32544;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES
(310009, 32544, 571, 1, 1, 0, 3393.9279, 5087.1079, 13.07991, 5.077986, 300, 15, 1),
(310010, 32544, 571, 1, 1, 0, 3862.7878, 5240.6787, 13.16521, 2.596991, 300, 15, 1);
DELETE FROM `pool_creature` WHERE `guid` IN (310009, 310010);
INSERT INTO `pool_creature` (`guid`, `pool_entry`, `chance`, `description`) VALUES
(310009, 4998, 0, 'Steam Cloud - Borean Tundra'),
(310010, 4998, 0, 'Steam Cloud - Borean Tundra');

UPDATE `creature_template` SET `speed_walk` = 0.6, `speed_run` = 0.21428, `BaseAttackTime` = 2000, `RangeAttackTime` = 2000, `unit_flags` = 33555200 WHERE `entry` = 17408;
DELETE FROM `creature_template_addon` WHERE `entry` = 17408;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(17408,0,0,0,1,0,"30627");-- DB/Creature: Remove incorrectly spawned unused creature Yor (10237) https://github.com/TrinityCore/TrinityCore/commit/0bec5baa7a33047adf439accda93fe8a4c042657 
-- DB/Creature: Remove static spawn of Farahlon Crumbler https://github.com/TrinityCore/TrinityCore/commit/a183ca5bf5c424c4ac0940542a5b0942f242fb2e  
-- DB/Creature: Add two missing Steam Cloud https://github.com/TrinityCore/TrinityCore/commit/ed3d9b69dda0c58b1079b74db6759934a905f9e0
--  DB/Creature: Update Arcane Vortex's auras & stats & add two missing spawns https://github.com/TrinityCore/TrinityCore/commit/d47fdeb78283b0fb7c982c2803d4709f7b4a8991
UPDATE `creature_template` SET `skinloot` = 0 WHERE `entry` = 10237;
DELETE FROM `creature` WHERE `id` = 21077;

DELETE FROM `creature` WHERE `guid` IN (310009, 310010) AND `id` = 32544;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES
(310009, 32544, 571, 1, 1, 0, 3393.9279, 5087.1079, 13.07991, 5.077986, 300, 15, 1),
(310010, 32544, 571, 1, 1, 0, 3862.7878, 5240.6787, 13.16521, 2.596991, 300, 15, 1);
DELETE FROM `pool_creature` WHERE `guid` IN (310009, 310010);
INSERT INTO `pool_creature` (`guid`, `pool_entry`, `chance`, `description`) VALUES
(310009, 4998, 0, 'Steam Cloud - Borean Tundra'),
(310010, 4998, 0, 'Steam Cloud - Borean Tundra');

UPDATE `creature_template` SET `speed_walk` = 0.6, `speed_run` = 0.21428, `BaseAttackTime` = 2000, `RangeAttackTime` = 2000, `unit_flags` = 33555200 WHERE `entry` = 17408;
DELETE FROM `creature_template_addon` WHERE `entry` = 17408;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(17408,0,0,0,1,0,"30627");

DELETE FROM `creature` WHERE `guid` IN (310011,310012) AND `id` = 17408;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES
(310011, 17408, 530, 1, 1, 0, 2887.78, 3226.63, 174.500, 2.43434, 3600, 15, 1),
(310012, 17408, 530, 1, 1, 0, 2796.53, 4342.95, 143.658, 5.30526, 3600, 15, 1);
