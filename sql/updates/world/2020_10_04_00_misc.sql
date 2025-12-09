-- DB/Profession: Pattern: Fur Lining - should drop only for characters with leatherworking & increase drop chance https://github.com/SunwellTracker/issues/issues/3330
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 10 WHERE `item` IN (44563,44559,44560,44562);
DELETE FROM `conditions` WHERE `sourcetypeorreferenceid` = 1 AND `SourceGroup` = 30921 AND `SourceEntry` = 44559;
DELETE FROM `conditions` WHERE `sourcetypeorreferenceid` = 1 AND `SourceGroup` = 31702 AND `SourceEntry` = 44563;
DELETE FROM `conditions` WHERE `sourcetypeorreferenceid` = 1 AND `SourceGroup` = 32297 AND `SourceEntry` = 44563;
DELETE FROM `conditions` WHERE `sourcetypeorreferenceid` = 1 AND `SourceGroup` = 32289 AND `SourceEntry` = 44560;
DELETE FROM `conditions` WHERE `sourcetypeorreferenceid` = 1 AND `SourceGroup` = 32290 AND `SourceEntry` = 44562;
INSERT INTO `conditions` VALUES
(1, 30921, 44559, 0, 0, 7, 0, 165, 1, 0, 0, 0, 0, '', ''),
(1, 31702, 44563, 0, 0, 7, 0, 165, 1, 0, 0, 0, 0, '', ''),
(1, 32297, 44563, 0, 0, 7, 0, 165, 1, 0, 0, 0, 0, '', ''),
(1, 32289, 44560, 0, 0, 7, 0, 165, 1, 0, 0, 0, 0, '', ''),
(1, 32290, 44562, 0, 0, 7, 0, 165, 1, 0, 0, 0, 0, '', '');