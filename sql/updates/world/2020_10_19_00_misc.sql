-- DB/Creature: Fix mobs before FOS/POS/HOR https://github.com/SunwellTracker/issues/issues/3399
DELETE FROM `creature` WHERE `guid` IN (
88801, 88787, 88779,
88805, 88781, 88791,
88780, 88788, 88792, 88786, 88790,
88785, 88783, 88789);

DELETE FROM `vehicle_accessory` WHERE `guid` IN (88793,88795,88802,88806,88800,88804,88803,88797);
INSERT INTO `vehicle_accessory` VALUES
(88793, 37965, 0, 1, '', 6, 300),
(88795, 37965, 0, 1, '', 6, 300),
(88802, 37965, 0, 1, '', 6, 300),
(88806, 37965, 0, 1, '', 6, 300),
(88800, 37965, 0, 1, '', 6, 300),
(88804, 37965, 0, 1, '', 6, 300),
(88803, 37965, 0, 1, '', 6, 300),
(88797, 37965, 0, 1, '', 6, 300);