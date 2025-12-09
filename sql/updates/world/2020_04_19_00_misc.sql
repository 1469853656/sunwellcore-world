-- DB/Go: Rich Thorium Veins in Dire Maul East https://github.com/SunwellTracker/issues/issues/3084
DELETE FROM `pool_template` WHERE `entry` = 50004;
INSERT INTO `pool_template` VALUES
(50004, 2, 'Dire Maul Vein');
