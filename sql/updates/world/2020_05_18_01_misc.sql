-- DB/Quest: Free Knot! - Knot Thimblejack's Cache should spawn after quest completion https://github.com/SunwellTracker/issues/issues/3198
UPDATE `gameobject` SET `spawntimesecs` = 86400 WHERE `guid` = 269097;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 14338 AND `id` = 7;
INSERT INTO `smart_scripts` VALUES 
(14338, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 50, 179501, 180, 1, 0, 0, 0, 8, 0, 0, 0, 585.21, 523.77, -25.4, 2.77244, 'On Quest Reward - Respawn GO');

DELETE FROM `gameobject` WHERE `guid` = 269097;
