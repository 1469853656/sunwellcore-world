-- DB/Quest: Hot and Cold, Sons of hodir daily - fix issue with Essence of Ice sometimes disappearing under ground and not spawning Scrap https://github.com/SunwellTracker/issues/issues/2980
DELETE FROM `conditions` WHERE `SourceEntry` = 56099;
INSERT INTO `conditions` VALUES 
(13, 1, 56099, 0, 0, 31, 0, 3, 30169, 0, 0, 0, 0, '', 'Essence of Ice Should target alive trigger'),
(13, 1, 56099, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, '', 'Essence of Ice Should target alive trigger');


