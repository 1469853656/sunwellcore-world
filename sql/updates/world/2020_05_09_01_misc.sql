-- DB/Quest: Blessing of Zim'Rhuk should be repeatable https://github.com/SunwellTracker/issues/issues/3165
-- Core/Quest: Escape from Coilskar Cistern - fix issue with "%N" instead of player's name in texts https://github.com/SunwellTracker/issues/issues/3132
-- DB/Quest: A Soldier in Need - reduce first aid supplies object time https://github.com/SunwellWoW/Sunwell-WOTLK/issues/135
-- Core/NPC: Thysta - horde flymaster - fix AI should not summon both gryphon & wyvern https://github.com/SunwellWoW/Sunwell-WOTLK/issues/134
-- DB/Go: Large Mithril Bound Chest - restore spawns in Blackrock Spire https://github.com/SunwellWoW/Sunwell-WOTLK/issues/160
-- DB/NPC: Coruu Control Console - should not attack players https://github.com/SunwellWoW/Sunwell-WOTLK/issues/63
-- DB/Quest: Shutting Down Manaforge Duro & Shutting Down Manaforge Coruu - fix issue with Sunfury Technician running to wrong manaforge through textures https://github.com/SunwellWoW/Sunwell-WOTLK/issues/64
UPDATE `quest_template` SET `SpecialFlags` = 1 WHERE `id` = 12656;
UPDATE `gameobject` SET `spawntimesec` = 20 WHERE `guid` = 57806;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_thysta" WHERE `entry` = 1387;

DELETE FROM `gameobject` WHERE `guid` IN (87909,87907,87908) AND `id` = 153469;
INSERT INTO `gameobject` VALUES 
(87907, 153469, 229, 1, 1, -73.9985, -453.335, -18.935, -3.08918, 0, 0, 0, 0, 86400, 0, 1, 0),
(87908, 153469, 229, 1, 1, -84.2677, -350.104, 70.9524, 1.91986, 0, 0, 0, 0, 86400, 0, 1, 0),
(87909, 153469, 229, 1, 1, 86.8551, -587.501, 30.6085, 2.63544, 0, 0, 0, 0, 86400, 0, 1, 0);

UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 2 WHERE `entry` = 20417;

UPDATE `smart_scripts` SET `action_param1` = 2021804, `action_param2` = 2, `action_type` = 80 WHERE `entryorguid` = 20218 AND `id` = 14;
UPDATE `smart_scripts` SET `action_param1` = 2021803 WHERE `entryorguid` = 20218 AND `id` = 12;
