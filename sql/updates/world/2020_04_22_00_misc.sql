-- DB/Quest: Valduran the Stormborn - followup
DELETE FROM `smart_scripts` WHERE `entryorguid` = 2936800 AND `id` = 10;
INSERT INTO `smart_scripts` VALUES 
(2936800, 9, 10, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 19, 256, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Remove Immune To NPC');
UPDATE `smart_scripts` SET `target_param1` = 30, `target_type` = 21 WHERE `entryorguid` = 2936800 AND `id` = 9;
