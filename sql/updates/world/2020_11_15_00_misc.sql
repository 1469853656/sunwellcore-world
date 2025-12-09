-- DB/NPC: Dire Maul add missin Tsu'zee spawn https://github.com/SunwellTracker/issues/issues/3422
DELETE FROM `creature` WHERE `guid` = 56961 AND `id` = 11467;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(56961, 11467, 429, 1, 1, 0, 1, 130.3, 562.59, -4.22, 3.26, 1000000, 0, 0, 17766, 0, 0, 0, 0, 0);
