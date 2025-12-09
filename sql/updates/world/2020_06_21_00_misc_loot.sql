-- DB/Loot: Schematic Jeeves should drop from more mobs
DELETE FROM `reference_loot_template` WHERE `entry` IN (50017,50013,50014,50016,50015) AND `item` = 49050;
INSERT INTO `reference_loot_template` VALUES
(50013, 49050, 0.8, 1, 0, 1, 1),
(50017, 49050, 0.8, 1, 0, 1, 1),
(50014, 49050, 0.8, 1, 0, 1, 1),
(50016, 49050, 0.8, 1, 0, 1, 1),
(50015, 49050, 0.8, 1, 0, 1, 1);
