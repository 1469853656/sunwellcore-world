-- DB/Quest: bury those cockroaches - fix kill credit when player in combat https://github.com/SunwellTracker/issues/issues/3078
DELETE FROM `smart_scripts` WHERE `entryorguid` = 25401 AND `id` IN (6,7,8,9);
INSERT INTO `smart_scripts` VALUES 
(25401, 0, 6, 0, 0, 2, 100, 1, 100, 100, 0, 0, 11, 45506, 2, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Seaforium Depth Charge - IC (Phase 2/No repeat) - Cast Nerub ar Sinkhole Credit (SW) '),
(25401, 0, 7, 0, 0, 2, 100, 1, 100, 100, 0, 0, 11, 45508, 2, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Seaforium Depth Charge - IC (Phase 2/No repeat) - Cast Nerub ar Sinkhole Credit (SE)'),
(25401, 0, 8, 0, 0, 2, 100, 1, 100, 100, 0, 0, 11, 45510, 2, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Seaforium Depth Charge - IC (Phase 2/No repeat) - Cast Nerub ar Sinkhole Credit (NW)'),
(25401, 0, 9, 0, 0, 2, 100, 1, 100, 100, 0, 0, 11, 45512, 2, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Seaforium Depth Charge - IC (Phase 2/No repeat) - Cast Nerub ar Sinkhole Credit (NE)');

DELETE FROM `conditions` WHERE `SourceGroup` IN (7,8,9,10) AND `SourceEntry` = 25401;
INSERT INTO `conditions` VALUES 
(22, 7, 25401, 0, 0, 29, 1, 25402, 20, 0, 0, 0, 0, '', 'Seaforium Depth Charge - Only run SAI if Nerub ar Sinkhole (South) within 20 yards'),
(22, 8, 25401, 0, 0, 29, 1, 25403, 20, 0, 0, 0, 0, '', 'Seaforium Depth Charge - Only run SAI if Nerub ar Sinkhole (East) within 20 yards'),
(22, 9, 25401, 0, 0, 29, 1, 25404, 20, 0, 0, 0, 0, '', 'Seaforium Depth Charge - Only run SAI if Nerub ar Sinkhole (West) within 20 yards'),
(22, 10, 25401, 0, 0, 29, 1, 25405, 20, 0, 0, 0, 0, '', 'Seaforium Depth Charge - Only run SAI if Nerub ar Sinkhole (North) within 20 yards');
