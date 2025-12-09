-- Delicate Touch (12820)
DELETE FROM `creature` WHERE `id` IN (29619, 29618, 29475);
DELETE FROM `creature` WHERE `guid`=117311;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(117311, 40617, 571, 1, 1, 0, 1, 6332.98, -1381.66, 425.86, 0, 300, 0, 0, 0, 0, 0, 0, 0, 0);
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_garm_invasion_bunny" WHERE `entry`=40617;
SET @ENTRY := 29618;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_snowblind_follower" WHERE `entry`=@ENTRY;
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_garm_invader" WHERE `entry`=29619;
SET @ENTRY := 29475;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_improved_land_mine" WHERE `entry`=@ENTRY;
