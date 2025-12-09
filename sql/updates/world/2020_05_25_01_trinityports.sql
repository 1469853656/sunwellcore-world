--  Boss/Script: Remove Hex Lord Malacrass wrong equipment from the script and use the one from the DB https://github.com/TrinityCore/TrinityCore/commit/5856f520bbd8dded42f7742277839587b0645dde
-- DB/Creature: Fix some creatures position in Zul'Aman & add missing https://github.com/TrinityCore/TrinityCore/commit/e6db72d58698fb5a7e04792b70ba4bfbd5df6590
UPDATE `creature_template_addon` SET `bytes2`=1 WHERE `entry`=24239;
SET @CGUID = 303351;
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID AND @CGUID+29;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `MovementType`) VALUES
(@CGUID+1, 24223, 568, 1, 1, 0, 227.8884, 1452.763, 26.00041, 3.001966, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+2, 24223, 568, 1, 1, 0, 231.7856, 1477.332, 26.00007, 0.541052, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+3, 24223, 568, 1, 1, 0, 228.4806, 1433.126, 27.38743, 2.583087, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+4, 24223, 568, 1, 1, 0, 227.03, 1417.474, 32.81706, 2.75762, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+5, 24223, 568, 1, 1, 0, 227.6509, 1404.276, 37.13389, 2.321288, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+6, 24223, 568, 1, 1, 0, 227.5511, 1382.419, 44.78124, 0.1745329, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+7, 24223, 568, 1, 1, 0, 245.9202, 1374.849, 49.40488, 3.403392, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+8, 24223, 568, 1, 1, 0, 229.5756, 1361.475, 49.16213, 5.218534, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+9, 24223, 568, 1, 1, 0, 246.9738, 1356.473, 49.40504, 1.204277, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+10, 24223, 568, 1, 1, 0, 253.9278, 1429.991, 49.40504, 2.181662, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+11, 24223, 568, 1, 1, 0, 261.4551, 1402.299, 49.40504, 2.024582, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+12, 24223, 568, 1, 1, 0, 281.3025, 1430.996, 61.60976, 0.08726646, 7200, 0), -- Eagle Trash Aggro Trigger (Area: 0)
(@CGUID+13, 24551, 568, 1, 1, 0, 409.0086, 1417.029, 74.39153, 0.6457718, 7200, 0), -- Eagle Event Deactivation Trigger (Area: 0)
(@CGUID+14, 21252, 568, 1, 1, 0, -34.71929, 1149.694, 19.2254, 6.038839, 7200, 0), -- World Trigger (Not Immune PC) (Area: 0)
(@CGUID+15, 24549, 568, 1, 1, 1, 114.8588, 992.1212, 30.54751, 0.6108652, 7200, 0), -- Amani'shi Tempest (Area: 0) (possible waypoints or random movement)
(@CGUID+16, 23581, 568, 1, 1, 1, 120.3534, 992.8157, 30.74231, 2.373648, 7200, 0), -- Amani'shi Medicine Man (Area: 0) (possible waypoints or random movement)
(@CGUID+17, 23596, 568, 1, 1, 1, 121.9231, 996.9573, 30.77481, 3.420845, 7200, 0), -- Amani'shi Flame Caster (Area: 0) (possible waypoints or random movement)
(@CGUID+18, 23597, 568, 1, 1, 1, 116.2163, 997.4488, 30.57613, 5.393067, 7200, 0), -- Amani'shi Guardian (Area: 0) (possible waypoints or random movement)
(@CGUID+19, 24179, 568, 1, 1, 1, 231.7972, 1393.424, 40.58874, 1.692969, 7200, 0), -- Amani'shi Wind Walker (Area: 0) (possible waypoints or random movement)
(@CGUID+20, 24179, 568, 1, 1, 1, 246.9084, 1375.147, 49.40504, 2.897247, 7200, 0), -- Amani'shi Wind Walker (Area: 0) (possible waypoints or random movement)
(@CGUID+21, 24179, 568, 1, 1, 1, 284.0443, 1372.319, 49.40504, 2.775074, 7200, 0), -- Amani'shi Wind Walker (Area: 0) (possible waypoints or random movement)
(@CGUID+22, 24180, 568, 1, 1, 1, 224.0694, 1394.261, 40.19848, 1.308997, 7200, 0), -- Amani'shi Protector (Area: 0) (possible waypoints or random movement)
(@CGUID+23, 24180, 568, 1, 1, 1, 244.807, 1367.658, 48.94979, 2.617994, 7200, 0), -- Amani'shi Protector (Area: 0) (possible waypoints or random movement)
(@CGUID+24, 24180, 568, 1, 1, 1, 274.3584, 1385.163, 49.40504, 3.752458, 7200, 0), -- Amani'shi Protector (Area: 0) (possible waypoints or random movement)
(@CGUID+25, 23586, 568, 1, 1, 1, -219.3753, 1380.724, 0.03532434, 1.047198, 7200, 0), -- Amani'shi Scout (Area: 0) (possible waypoints or random movement)
(@CGUID+26, 23597, 568, 1, 1, 1, 411.5532, 1022.259, 9.605077, 4.782202, 7200, 0), -- Amani'shi Guardian (Area: 0)
(@CGUID+27, 23597, 568, 1, 1, 1, 400.7974, 1022.438, 9.605576, 4.660029, 7200, 0), -- Amani'shi Guardian (Area: 0)
(@CGUID+28, 23597, 568, 1, 1, 1, 397.7076, 1048.77, 9.604503, 6.108652, 7200, 0), -- Amani'shi Guardian (Area: 0)
(@CGUID+29, 23597, 568, 1, 1, 1, 406.0692, 1044.394, 9.60488, 2.356194, 7200, 0), -- Amani'shi Guardian (Area: 0)
(@CGUID+0, 24065, 568, 1, 1, 1, 406.4329, 1054.867, 9.604886, 4.520403, 7200, 0); -- Amani'shi Handler (Area: 0)


DELETE FROM `linked_respawn` WHERE `guid` BETWEEN @CGUID+15 AND @CGUID+29;
DELETE FROM `linked_respawn` WHERE `guid` = @CGUID;
INSERT INTO `linked_respawn` (`guid`, `linkedGuid`, `linkType`) VALUES
(@CGUID+15,89357,0),
(@CGUID+16,89357,0),
(@CGUID+17,89357,0),
(@CGUID+18,89357,0),
(@CGUID+19,86494,0),
(@CGUID+20,86494,0),
(@CGUID+21,86494,0),
(@CGUID+22,86494,0),
(@CGUID+23,86494,0),
(@CGUID+24,86494,0),
(@CGUID+25,89322,0),
(@CGUID+26,86195,0),
(@CGUID+27,86195,0),
(@CGUID+28,86195,0),
(@CGUID+29,86195,0),
(@CGUID+0,86195,0);