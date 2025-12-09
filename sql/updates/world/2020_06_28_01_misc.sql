-- DB/NPC: Add missing midsummer vendors
DELETE FROM `game_event_creature` WHERE `guid` IN (301020, 301025);
INSERT INTO `game_event_creature` VALUES
(1, 301020),
(1, 301025);
DELETE FROM `creature` WHERE `guid` = 301020 AND `id` = 26124;
DELETE FROM `creature` WHERE `guid` = 301025 AND `id` = 26123;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(301020, 26124, 530, 1, 1, 0, 0, 9790.85, -7250.58, 26.1002, 5.48896, 300, 0, 0, 3524, 0, 0, 0, 0, 0),
(301025, 26123, 0, 1, 1, 0, 0, -8827.68, 875.783, 98.6689, 4.60823, 300, 0, 0, 3524, 0, 0, 0, 0, 0);
