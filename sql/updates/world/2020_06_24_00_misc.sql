-- DB/Loot: Add Alterac Valley corpse loot
-- DB/AI: Booty Bay bruiser - call for help when attacked
DELETE FROM `creature_loot_template` WHERE `entry` = 1;
INSERT INTO `creature_loot_template` VALUES 
(1, 17306, 33, 1, 0, 2, 5),
(1, 17326, 30, 1, 2, 2, 5),
(1, 17327, 30, 1, 2, 2, 5),
(1, 17328, 30, 1, 2, 2, 5),
(1, 17422, 100, 1, 0, 20, 20),
(1, 17423, 33, 1, 0, 2, 5),
(1, 17502, 30, 1, 1, 2, 5),
(1, 17503, 30, 1, 1, 2, 5),
(1, 17504, 30, 1, 1, 2, 5),
(1, 18228, 1.5, 1, 4, 1, 1),
(1, 18231, 10, 1, 3, 1, 1);

-- Booty Bay Bruiser - call for help on aggro
DELETE FROM `smart_scripts` WHERE `entryorguid` = 4624 AND `id` = 3;
INSERT INTO `smart_scripts` VALUES
(4624, 0, 3, 0, 4, 0, 100, 0, 0, 0, 0, 0, 39, 20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
