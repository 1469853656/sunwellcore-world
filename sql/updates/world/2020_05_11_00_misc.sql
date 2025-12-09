-- DB/Quest: The God Hakkar - Yeh'kinya should offer Scroll after completion of quest https://github.com/SunwellTracker/issues/issues/3178
DELETE FROM `smart_scripts` WHERE `entryorguid` = 8579 AND `id` = 3;
INSERT INTO `smart_scripts` VALUES
(8579, 0, 3, 0, 62, 0, 100, 0, 1470, 0, 0, 0, 85, 12998, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Yeh\'kinya - on gossip select - cast create Yehkinya Scroll')