-- [Quest] Princess Must Die! - remove one bodyguard #2610 https://github.com/SunwellTracker/issues/issues/2610
-- [Quest] Children of Ursoc correct reputation reward https://github.com/SunwellTracker/issues/issues/2587
-- [Quest] Shadow of Thel'zan the Duskbringer - add script https://github.com/SunwellTracker/issues/issues/2605
UPDATE `creature_template` SET `lootid` = 0, `flags_extra` = `flags_extra` | 64  WHERE `entry` IN (30957, 30958);
UPDATE `creature` SET `MovementType`=0 WHERE `guid`=131180;
DELETE FROM `creature` WHERE `guid` = 80848;

DELETE FROM `creature_text` WHERE `entry` = 27510;
INSERT INTO `creature_text` VALUES
(27510, 0, 0, 'Soldiers of the frozen wastes, rise and hear the call of your master! Do not let $N escape with the tome!', 14, 0, 100, 0, 0, 0, 0, 'Shadow of Thelzan the Duskbringer'),
(27510, 1, 0, 'Impressive. The master said that you and your people would be a nuisance. No matter, your end draws near. Soon the combined might of Naxxramas will come down upon this place and shatter your meager existence.', 12, 0, 100, 0, 0, 0, 0, 'Shadow of Thelzan the Duskbringer'),
(27510, 2, 0, 'Where do you think you\'re going with that, mortal? Put the tome down and I might spare you an eternity of anguish.', 12, 0, 100, 0, 0, 0, 0, 'Shadow of Thelzan the Duskbringer');

UPDATE `quest_template` SET `RewardFactionId1` = 1050 WHERE `id` = 12247;
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 27510;
UPDATE `gameobject_template` SET `AIName` = "SmartGameObjectAI", `ScriptName` = "" WHERE `entry` = 189311;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 189311;
INSERT INTO `smart_scripts` VALUES
(189311, 1, 0, 0, 19, 0, 100, 0, 12312, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 131180, 27510, 0, 0, 0, 0, 0, 'On quest taken - Set Data');


DELETE FROM `smart_scripts` WHERE `entryorguid` = 27510;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 2751000;
INSERT INTO `smart_scripts` VALUES
(27510, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 80, 2751000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On quest taken - start script'),
(2751000, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0 ,0, 0, 0, 0, 0, 'On Script Say Line 0'),
(2751000, 9, 1, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 ,0, 0, 0, 0, 0, 'On Script Say Line 0'),
(2751000, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0 ,0, 0, 0, 0, 0, 'On Script Say Line 0');
UPDATE `npc_vendor` SET `incrtime` = 3600 WHERE `item` = 16217;
UPDATE `creature_template` SET `AIname` = "SmartAI", `ScriptName` = "" WHERE `entry` = 27331;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 27331;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(27331, 0, 0, 1, 8, 0, 100, 0, 48345, 0, 110000, 110000, 33, 27331, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Quest Credit \'Bombard the Ballistae\''),
(27331, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 11, 48351, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast  Bombard the Ballistae: Ballista Bow'),
(27331, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 11, 48352, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Ballista Frame'),
(27331, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 11, 48353, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Ballista Missile'),
(27331, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 11, 48354, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Ballista Wheel'),
(27331, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 11, 48347, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: FX Master'),
(27331, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 11, 52687, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Knockback'),
(27331, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 99, 3, 0, 0, 0, 0, 0, 20, 188673, 0, 0, 0, 0, 0, 0, 'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Despawn GO');

UPDATE `gameobject` SET `spawntimesecs` = 110 WHERE `id` = 188673;
UPDATE `gameobject` SET `spawnMask` = 15 WHERE `guid` IN (150443, 151226);
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 34228;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 32599;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 32600;
DELETE FROM `creature_addon` WHERE `guid` = 34225;
INSERT INTO `creature_addon` VALUES
(34225, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 1820.19, position_y = -2611.46, position_z = 60.7361, orientation = 6.12588 WHERE guid = 34226;
DELETE FROM `creature_addon` WHERE `guid` = 34226;
INSERT INTO `creature_addon` VALUES (34226, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 32649;
UPDATE creature SET position_x = 2204.18, position_y = -1422.82, position_z = 90.7302, orientation = 0.65005 WHERE guid = 33087;
DELETE FROM `creature_addon` WHERE `guid` = 33087;
INSERT INTO `creature_addon` VALUES (33087, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 2314.2, position_y = -1425.68, position_z = 86.7688, orientation = 3.2998 WHERE guid = 33035;
DELETE FROM `creature_addon` WHERE `guid` = 33035;
INSERT INTO `creature_addon` VALUES (33035, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 2370.66, position_y = -1181.04, position_z = 90.8939, orientation = 4.43775 WHERE guid = 33034;
DELETE FROM `creature_addon` WHERE `guid` = 33034;
INSERT INTO `creature_addon` VALUES (33034, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 2379.65, position_y = -1195.1, position_z = 89.3769, orientation = 2.97333 WHERE guid = 33082;
DELETE FROM `creature_addon` WHERE `guid` = 33082;
INSERT INTO `creature_addon` VALUES (33082, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 2365.41, position_y = -1197.17, position_z = 89.0061, orientation = 0.622459 WHERE guid = 33081;
DELETE FROM `creature_addon` WHERE `guid` = 33081;
INSERT INTO `creature_addon` VALUES (33081, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1498.21, position_y = -3812.88, position_z = 24.285, orientation = 4.36949 WHERE guid = 13808;
DELETE FROM `creature_addon` WHERE `guid` = 13808;
INSERT INTO `creature_addon` VALUES (13808, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 1589.26, position_y = -2536.65, position_z = 102.131, orientation = 3.9547 WHERE guid = 34832;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 34832;
UPDATE creature SET position_x = 1711.3, position_y = -3459.53, position_z = 144.087, orientation = 2.09165 WHERE guid = 32896;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 32896;
UPDATE creature SET position_x = 1777.18, position_y = -2006.78, position_z = 111.49, orientation = 4.69821 WHERE guid = 32589;
DELETE FROM creature_addon WHERE guid = 32589;
INSERT INTO creature_addon VALUES (32589,0,0,0,4097,16,'');

UPDATE creature SET position_x = 1570.8, position_y = -2509.23, position_z = 97.9949, orientation = 1.51761 WHERE guid = 33048;
DELETE FROM creature_addon WHERE guid = 33048;
INSERT INTO creature_addon VALUES (33048,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 33028;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 33037;
UPDATE creature SET position_x = 2433.5, position_y = -3424.5, position_z = 99.1643, orientation = 6.04849 WHERE guid = 33061;
DELETE FROM creature_addon WHERE guid = 33061;
INSERT INTO creature_addon VALUES (33061,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1096.84, position_y = -5580.96, position_z = 11.1797, orientation = 2.11735 WHERE guid = 13030;
DELETE FROM creature_addon WHERE guid = 13030;
INSERT INTO creature_addon VALUES (13030,0,0,1,4097,0,'');
UPDATE creature SET position_x = 881.191, position_y = -4198.7, position_z = -14.1639, orientation = 1.68093 WHERE guid = 6432;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6432;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32821);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32821;
INSERT INTO `smart_scripts` VALUES
(-32821, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32821, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 33010);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -33010;
INSERT INTO `smart_scripts` VALUES (-33010, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-33010, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 33026);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -33026;
INSERT INTO `smart_scripts` VALUES (-33026, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-33026, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32810);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32810;
INSERT INTO `smart_scripts` VALUES (-32810, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32810, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 33015);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -33015;
INSERT INTO `smart_scripts` VALUES (-33015, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-33015, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32817);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32817;
INSERT INTO `smart_scripts` VALUES (-32817, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32817, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32813);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32813;
INSERT INTO `smart_scripts` VALUES (-32813, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32813, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32829);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32829;
INSERT INTO `smart_scripts` VALUES (-32829, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32829, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 33022);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -33022;
INSERT INTO `smart_scripts` VALUES (-33022, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-33022, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32814);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32814;
INSERT INTO `smart_scripts` VALUES (-32814, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32814, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32845);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32845;
INSERT INTO `smart_scripts` VALUES (-32845, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32845, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 32841);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -32841;
INSERT INTO `smart_scripts` VALUES (-32841, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-32841, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = 4326.65, position_y = -6129.89, position_z = 127.311, orientation = 0.859247 WHERE guid = 35583;
DELETE FROM `creature_addon` WHERE `guid` = 35583;
INSERT INTO `creature_addon` VALUES (35583, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 4374.39, position_y = -6259.35, position_z = 96.3749, orientation = 0.586623 WHERE guid = 35574;
DELETE FROM `creature_addon` WHERE `guid` = 35574;
INSERT INTO `creature_addon` VALUES (35574, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 3807.07, position_y = -4910.9, position_z = 145.909, orientation = 2.75166 WHERE guid = 35214;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35214;
UPDATE creature SET position_x = 3804.26, position_y = -4898.72, position_z = 146.775, orientation = 3.81282 WHERE guid = 35232;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35232;
UPDATE creature SET position_x = 4580.69, position_y = -5580.5, position_z = 99.6718, orientation = 0.106314 WHERE guid = 35261;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35261;
UPDATE creature SET position_x = 4596.12, position_y = -5570.11, position_z = 97.2836, orientation = 3.76453 WHERE guid = 35262;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35262;
UPDATE creature SET position_x = 4597.88, position_y = -5583.27, position_z = 97.8965, orientation = 2.47647 WHERE guid = 35271;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35271;
UPDATE creature SET position_x = 4455.96, position_y = -5534.67, position_z = 108.689, orientation = 3.15034 WHERE guid = 35248;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35248;
UPDATE creature SET position_x = 4452.51, position_y = -5526.76, position_z = 108.815, orientation = 4.75064 WHERE guid = 35266;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35266;
UPDATE creature SET position_x = 4313.92, position_y = -5739.26, position_z = 102.323, orientation = 1.6224 WHERE guid = 35256;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35256;
UPDATE creature SET position_x = 4305.08, position_y = -5728.58, position_z = 102.045, orientation = 5.6919 WHERE guid = 35235;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35235;
UPDATE creature SET position_x = 4315.57, position_y = -5725.57, position_z = 101.601, orientation = 4.20139 WHERE guid = 35259;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35259;
UPDATE creature SET position_x = 4700.81, position_y = -5902.82, position_z = 109.449, orientation = 5.10268 WHERE guid = 35653;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35653;
UPDATE creature SET position_x = 4527.04, position_y = -6371.55, position_z = 128.686, orientation = 0.607872 WHERE guid = 35794;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35794;
UPDATE creature SET position_x = 4493.4, position_y = -6397.09, position_z = 126.569, orientation = 2.68132 WHERE guid = 35674;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35674;
UPDATE creature SET position_x = 4487.16, position_y = -6396.54, position_z = 126.994, orientation = 0.272771 WHERE guid = 35639;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35639;
UPDATE creature SET position_x = 4780.27, position_y = -6635.03, position_z = 111.696, orientation = 2.73195 WHERE guid = 35722;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35722;
UPDATE creature SET position_x = 4772.35, position_y = -6634.33, position_z = 111.747, orientation = 0.773687 WHERE guid = 35656;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 35656;
UPDATE creature SET position_x = 2314.22, position_y = -5889.56, position_z = 123.342, orientation = 1.48555 WHERE guid = 36000;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 36000;
UPDATE creature SET position_x = 4334.96, position_y = -6128.63, position_z = 126.506, orientation = 1.32699 WHERE guid = 35592;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 35592;
UPDATE creature SET position_x = 4366.38, position_y = -6135.34, position_z = 123.513, orientation = 3.13132 WHERE guid = 35598;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 35598;
UPDATE creature SET position_x = 4381.03, position_y = -6251.73, position_z = 95.3706, orientation = 5.35866 WHERE guid = 35601;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 35601;
UPDATE creature SET position_x = 4385.61, position_y = -6264.79, position_z = 95.6827, orientation = 4.09853 WHERE guid = 35577;
UPDATE creature SET position_x = -4521.8, position_y = -11634.7, position_z = 13.2385, orientation = 0.787583 WHERE guid = 60692;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60692;
UPDATE creature SET position_x = 9706.32, position_y = 1546.42, position_z = 1253.9, orientation = 0.33927 WHERE guid = 49166;
DELETE FROM `creature_addon` WHERE `guid` = 49166;
INSERT INTO `creature_addon` VALUES (49166, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4483.64, position_y = -11619, position_z = 10.9754, orientation = 2.97623 WHERE guid = 60713;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60713;
UPDATE creature SET position_x = -4509.85, position_y = -11595.8, position_z = 12.2825, orientation = 4.8193 WHERE guid = 60716;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60716;
UPDATE creature SET position_x = -4639.77, position_y = -11538, position_z = 16.8412, orientation = 1.1576 WHERE guid = 60709;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60709;
UPDATE creature SET position_x = -4605.94, position_y = -11526.6, position_z = 18.535, orientation = 3.03097 WHERE guid = 60701;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60701;
UPDATE creature SET position_x = -4629.92, position_y = -11499.1, position_z = 20.2262, orientation = 4.30268 WHERE guid = 60703;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60703;
UPDATE creature SET position_x = -5048.92, position_y = -11092.3, position_z = 26.9676, orientation = 2.42408 WHERE guid = 60762;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60762;
UPDATE creature SET position_x = -5066.57, position_y = -11053.7, position_z = 27.8809, orientation = 4.60135 WHERE guid = 60741;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60741;
UPDATE creature SET position_x = -5049.19, position_y = -11081.6, position_z = 28.8076, orientation = 0.0972611 WHERE guid = 60742;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60742;
UPDATE creature SET position_x = -4977.51, position_y = -11194, position_z = 16.8436, orientation = 2.75365 WHERE guid = 60753;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 60753;
UPDATE creature SET position_x = -2496.02, position_y = -12290.1, position_z = 14.1912, orientation = 3.25979 WHERE guid = 62098;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62098;
UPDATE creature SET position_x = -2519.05, position_y = -12316.6, position_z = 14.1699, orientation = 1.45512 WHERE guid = 62094;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62094;
UPDATE creature SET position_x = -2534.39, position_y = -12278.9, position_z = 14.1212, orientation = 5.51127 WHERE guid = 62112;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62112;
UPDATE creature SET position_x = -2398.67, position_y = -12170.2, position_z = 33.0596, orientation = 2.5391 WHERE guid = 62124;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62124;
UPDATE creature SET position_x = -2438.82, position_y = -12159.8, position_z = 32.8202, orientation = 5.89013 WHERE guid = 62105;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62105;
UPDATE creature SET position_x = -2513.35, position_y = -12201.7, position_z = 27.9375, orientation = 3.25298 WHERE guid = 62102;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62102;
UPDATE creature SET position_x = -2534.95, position_y = -12217.6, position_z = 27.8881, orientation = 1.25632 WHERE guid = 62117;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62117;
UPDATE creature SET position_x = -2553.01, position_y = -12183.7, position_z = 28.0573, orientation = 5.50445 WHERE guid = 62118;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62118;
UPDATE creature SET position_x = -2426.44, position_y = -11322.4, position_z = 29.6291, orientation = 1.55556 WHERE guid = 62379;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62379;
UPDATE creature SET position_x = -2454.35, position_y = -11316.4, position_z = 30.7272, orientation = 0.354908 WHERE guid = 62373;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62373;
UPDATE creature SET position_x = -2448.93, position_y = -11322.9, position_z = 30.4449, orientation = 0.947971 WHERE guid = 62395;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62395;
UPDATE creature SET position_x = -2347.56, position_y = -11363.5, position_z = 27.9424, orientation = 2.10295 WHERE guid = 62388;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62388;
UPDATE creature SET position_x = -2425.67, position_y = -11239.4, position_z = 24.2871, orientation = 2.47994 WHERE guid = 62374;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62374;
UPDATE creature SET position_x = -2413.52, position_y = -11217.3, position_z = 24.362, orientation = 3.41233 WHERE guid = 62363;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62363;
UPDATE creature SET position_x = -2345.9, position_y = -11179.1, position_z = 13.9425, orientation = 0.0784045 WHERE guid = 62400;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62400;
UPDATE creature SET position_x = -2314.76, position_y = -11196.8, position_z = 13.9438, orientation = 2.07355 WHERE guid = 62384;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62384;
UPDATE creature SET position_x = -2310.84, position_y = -11164.8, position_z = 13.942, orientation = 3.15915 WHERE guid = 62365;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62365;
UPDATE creature SET position_x = -2320, position_y = -11158.9, position_z = 13.942, orientation = 4.81372 WHERE guid = 62366;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62366;
UPDATE creature SET position_x = -1618.29, position_y = -11914.9, position_z = 9.32866, orientation = 2.21047 WHERE guid = 62129;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62129;
UPDATE creature SET position_x = -1649.07, position_y = -11912.5, position_z = 9.32859, orientation = 0.645812 WHERE guid = 62159;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62159;
UPDATE creature SET position_x = -1631.4, position_y = -11881.4, position_z = 9.34529, orientation = 4.63753 WHERE guid = 62146;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62146;
UPDATE creature SET position_x = -1653.7, position_y = -11816.6, position_z = 21.8957, orientation = 2.16179 WHERE guid = 62162;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62162;
UPDATE creature SET position_x = -1651.46, position_y = -11785.5, position_z = 21.8439, orientation = 3.8757 WHERE guid = 62142;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62142;
UPDATE creature SET position_x = -1683.05, position_y = -11798.1, position_z = 22.233, orientation = 6.27233 WHERE guid = 62163;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62163;
UPDATE creature SET position_x = -1548.12, position_y = -11818.1, position_z = 23.217, orientation = 5.42943 WHERE guid = 62148;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62148;
UPDATE creature SET position_x = -1535.02, position_y = -11849.8, position_z = 23.2177, orientation = 1.4945 WHERE guid = 62165;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62165;
UPDATE creature SET position_x = -1518.77, position_y = -11821.6, position_z = 23.2176, orientation = 3.74841 WHERE guid = 62166;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62166;
UPDATE creature SET position_x = -1515.01, position_y = -11947.8, position_z = 19.0508, orientation = 0.678139 WHERE guid = 62145;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62145;
UPDATE creature SET position_x = -1511.94, position_y = -11918.1, position_z = 19.0508, orientation = 5.33098 WHERE guid = 62160;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62160;
UPDATE creature SET position_x = -1482.88, position_y = -11935.2, position_z = 19.0508, orientation = 3.07601 WHERE guid = 62136;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62136;
UPDATE creature SET position_x = -1535.01, position_y = -12050.8, position_z = 10.7182, orientation = 1.46682 WHERE guid = 62126;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62126;
UPDATE creature SET position_x = -1548.89, position_y = -12017.1, position_z = 10.7723, orientation = 5.43919 WHERE guid = 62155;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62155;
UPDATE creature SET position_x = -1517.96, position_y = -12021.1, position_z = 10.7259, orientation = 3.85443 WHERE guid = 62156;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62156;
UPDATE creature SET position_x = -1417.67, position_y = -11964.8, position_z = 14.8838, orientation = 6.22616 WHERE guid = 62170;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62170;
UPDATE creature SET position_x = -1384.32, position_y = -11951.5, position_z = 14.8848, orientation = 3.86648 WHERE guid = 62153;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62153;
UPDATE creature SET position_x = -1387.37, position_y = -11982.5, position_z = 14.8844, orientation = 2.19096 WHERE guid = 62172;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62172;
UPDATE creature SET position_x = -1415.75, position_y = -11835.2, position_z = 19.0516, orientation = 3.08457 WHERE guid = 62150;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62150;
UPDATE creature SET position_x = -1445.22, position_y = -11818.4, position_z = 19.0508, orientation = 5.39119 WHERE guid = 62137;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62137;
UPDATE creature SET position_x = -1449.01, position_y = -11848.4, position_z = 19.0516, orientation = 0.730986 WHERE guid = 62169;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62169;
UPDATE creature SET position_x = -1467.44, position_y = -11392, position_z = 68.1271, orientation = 3.88166 WHERE guid = 62420;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62420;
UPDATE creature SET position_x = -1565.09, position_y = -11291.6, position_z = 66.0907, orientation = 4.37189 WHERE guid = 62436;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 62436;
UPDATE creature SET position_x = 10265.7, position_y = 967.391, position_z = 1340.82, orientation = 6.13679 WHERE guid = 47289;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 47289;
UPDATE creature SET position_x = 9631.46, position_y = 466.877, position_z = 1312.79, orientation = 1.11374 WHERE guid = 48077;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 48077;
UPDATE creature SET position_x = 9264.37, position_y = 936.077, position_z = 1311.86, orientation = 0.454009 WHERE guid = 48564;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 48564;
UPDATE creature SET position_x = 9107.09, position_y = 1849.67, position_z = 1327.5, orientation = 5.62019 WHERE guid = 49207;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 49207;
UPDATE creature SET position_x = 9153.55, position_y = 1840, position_z = 1327.68, orientation = 3.77364 WHERE guid = 49215;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 49215;
UPDATE creature SET position_x = 9363.36, position_y = 1716.69, position_z = 1308.86, orientation = 6.11238 WHERE guid = 48530;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 48530;
UPDATE creature SET position_x = 9372.2, position_y = 1716.17, position_z = 1308.86, orientation = 4.4962 WHERE guid = 48529;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 48529;
UPDATE creature SET position_x = 9563.39, position_y = 1301.75, position_z = 1300.79, orientation = 5.57482 WHERE guid = 48488;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 48488;
UPDATE creature SET position_x = -4597.53, position_y = -12880, position_z = 6.72633, orientation = 0.0801197 WHERE guid = 62062;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 62062;
DELETE FROM creature_addon WHERE guid = 63424;
INSERT INTO creature_addon VALUES (63424,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 63255;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 63296;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 63308;
DELETE FROM creature_addon WHERE guid = 63295;
INSERT INTO creature_addon VALUES (63295,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4627.84, position_y = -12929.1, position_z = 7.63546, orientation = 4.82043 WHERE guid = 62061;
DELETE FROM creature_addon WHERE guid = 62061;
INSERT INTO creature_addon VALUES (62061,0,0,0,4097,467,'');
UPDATE creature SET position_x = -1531.69, position_y = -3688.41, position_z = 92.0285, orientation = 0.38228 WHERE guid = 13837;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 13837;
UPDATE creature SET position_x = -129.696, position_y = -2034.19, position_z = 91.8452, orientation = 0.946709 WHERE guid = 20473;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 20473;
UPDATE creature SET position_x = -1210.84, position_y = -2723.15, position_z = 106.684, orientation = 4.8011 WHERE guid = 13993;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13993;
UPDATE creature SET position_x = -4059.73, position_y = -2145.07, position_z = 90.2194, orientation = 4.27031 WHERE guid = 13593;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13593;
UPDATE creature SET position_x = -4198.14, position_y = -2220.44, position_z = 53.7819, orientation = 4.22144 WHERE guid = 13591;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13591;
UPDATE creature SET position_x = -4189.71, position_y = -2191.37, position_z = 50.252, orientation = 3.95266 WHERE guid = 13612;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13612;
UPDATE creature SET position_x = -4161.76, position_y = -2220.22, position_z = 51.4337, orientation = 1.10428 WHERE guid = 13613;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13613;
UPDATE creature SET position_x = 106.86, position_y = -365.238, position_z = 3.69841, orientation = 1.70381 WHERE guid = 29387;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 29387;
UPDATE creature SET position_x = 78.3119, position_y = -464.922, position_z = 13.249, orientation = 1.88882 WHERE guid = 29350;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 29350;
UPDATE creature SET position_x = 73.0561, position_y = -457.585, position_z = 13.0959, orientation = 5.64128 WHERE guid = 29353;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 29353;
UPDATE creature SET position_x = 108.334, position_y = -587.977, position_z = -1.20949, orientation = 2.4194 WHERE guid = 29361;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 29361;
UPDATE creature SET position_x = -12.0665, position_y = -783.852, position_z = 1.32145, orientation = 4.03907 WHERE guid = 29357;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 29357;
UPDATE creature SET position_x = -1463.21, position_y = -3638.96, position_z = 92.064, orientation = 0.738326 WHERE guid = 13852;
DELETE FROM creature_addon WHERE guid = 13852;
INSERT INTO creature_addon VALUES (13852,0,0,0,4097,16,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 13838;
UPDATE creature SET position_x = 106.86, position_y = -365.238, position_z = 3.69841, orientation = 1.70381 WHERE guid = 29387;
DELETE FROM creature_addon WHERE guid = 29387;
INSERT INTO creature_addon VALUES (29387,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 29376;
INSERT INTO creature_addon VALUES (29376,0,0,1,4097,0,'');
UPDATE creature SET position_x = 78.3119, position_y = -464.922, position_z = 13.249, orientation = 1.88882 WHERE guid = 29350;
DELETE FROM creature_addon WHERE guid = 29350;
INSERT INTO creature_addon VALUES (29350,0,0,1,4097,0,'');
UPDATE creature SET position_x = 73.0561, position_y = -457.585, position_z = 13.0959, orientation = 5.64128 WHERE guid = 29353;
DELETE FROM creature_addon WHERE guid = 29353;
INSERT INTO creature_addon VALUES (29353,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29318;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29324;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29321;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29298;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29331;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29303;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29344;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29343;
UPDATE creature SET position_x = -4059.73, position_y = -2145.07, position_z = 90.2194, orientation = 4.27031 WHERE guid = 13593;
UPDATE creature_addon SET emote = 467 WHERE guid = 13593;
UPDATE creature SET position_x = -4191.18, position_y = -2158.14, position_z = 67.6418, orientation = 4.23191 WHERE guid = 13607;
UPDATE creature_addon SET emote = 467 WHERE guid = 13607;
UPDATE creature_addon SET emote = 467 WHERE guid = 13583;
UPDATE creature_addon SET emote = 467 WHERE guid = 13610;
UPDATE creature_addon SET emote = 467 WHERE guid = 13594;
UPDATE creature_addon SET emote = 467 WHERE guid = 13611;
UPDATE creature_addon SET emote = 467 WHERE guid = 13615;
UPDATE creature_addon SET emote = 467 WHERE guid = 13600;
UPDATE creature SET position_x = -4198.14, position_y = -2220.44, position_z = 53.7819, orientation = 4.22144 WHERE guid = 13591;
UPDATE creature_addon SET emote = 467 WHERE guid = 13591;
UPDATE creature SET position_x = -4189.71, position_y = -2191.37, position_z = 50.252, orientation = 3.95266 WHERE guid = 13612;
UPDATE creature_addon SET emote = 467 WHERE guid = 13612;
UPDATE creature SET position_x = -4161.76, position_y = -2220.22, position_z = 51.4337, orientation = 1.10428 WHERE guid = 13613;
UPDATE creature_addon SET emote = 467 WHERE guid = 13613;
UPDATE creature_addon SET emote = 467 WHERE guid = 13584;
UPDATE creature_addon SET emote = 467 WHERE guid = 13984;
UPDATE creature SET position_x = 108.334, position_y = -587.977, position_z = -1.20949, orientation = 2.4194 WHERE guid = 29361;
UPDATE creature SET position_x = -12.0665, position_y = -783.852, position_z = 1.32145, orientation = 4.03907 WHERE guid = 29357;
UPDATE creature SET position_x = -9866.1, position_y = -226.149, position_z = 35.6849, orientation = 0.879959 WHERE guid = 80744;

DELETE FROM `creature_addon` WHERE `guid` = 80744;
INSERT INTO `creature_addon` VALUES (80744, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -8916.36, position_y = -2305.28, position_z = 134.597, orientation = 0.910883 WHERE guid = 28335;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 28335;
UPDATE creature SET position_x = -8974, position_y = -2302.16, position_z = 133.21, orientation = 3.21821 WHERE guid = 16355;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16355;
UPDATE creature SET position_x = -8910.08, position_y = -2195.06, position_z = 134.614, orientation = 0.0521787 WHERE guid = 16410;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16410;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16414;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16418;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16419;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16425;
UPDATE creature SET position_x = -8933.35, position_y = -2107.26, position_z = 133.214, orientation = 4.95306 WHERE guid = 16420;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16420;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 31827;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 31826;
UPDATE creature SET position_x = -8174.75, position_y = -1567.72, position_z = 138.986, orientation = 5.89753 WHERE guid = 5543;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5543;
UPDATE creature SET position_x = -8078.74, position_y = -1902.6, position_z = 134.092, orientation = 5.531 WHERE guid = 5554;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5554;
UPDATE creature SET position_x = -8874.2, position_y = -776.441, position_z = 69.1815, orientation = 2.84463 WHERE guid = 81143;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 81143;
UPDATE creature SET position_x = -8880.17, position_y = -767.139, position_z = 68.5773, orientation = 4.20599 WHERE guid = 81142;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 81142;
UPDATE creature SET position_x = -8896, position_y = -768.564, position_z = 69.4018, orientation = 5.15893 WHERE guid = 81141;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 81141;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80534;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80536;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80535;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80521;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80520;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80519;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80518;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80525;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80526;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80523;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80500;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80499;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80501;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80508;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 80506;
UPDATE creature SET position_x = -8141.56, position_y = -1569.31, position_z = 133.52, orientation = 1.151901 WHERE guid = 5542;
DELETE FROM creature_addon WHERE guid = 5542;
INSERT INTO creature_addon VALUES (5542,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 5507;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 5506;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 5513;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 5515;
DELETE FROM creature_addon WHERE guid = 5282;
INSERT INTO creature_addon VALUES (5282,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 5365;
INSERT INTO creature_addon VALUES (5365,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9195.53, position_y = -2878.23, position_z = 113.039, orientation = 4.76098 WHERE guid = 7456;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7456;
DELETE FROM creature_addon WHERE guid = 7454;
INSERT INTO creature_addon VALUES (7454,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9215.4, position_y = -2902.34, position_z = 112.942, orientation = 6.20263 WHERE guid = 7455;
DELETE FROM creature_addon WHERE guid = 7455;
INSERT INTO creature_addon VALUES (7455,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6743;
DELETE FROM creature_addon WHERE guid = 6741;
INSERT INTO creature_addon VALUES (6741,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9537.18, position_y = -3260.01, position_z = 48.9345, orientation = 3.17467 WHERE guid = 6194;
DELETE FROM creature_addon WHERE guid = 6194;
INSERT INTO creature_addon VALUES (6194,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6195;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6193;
UPDATE creature SET position_x = -8885.72, position_y = -780.213, position_z = 69.6421, orientation = 1.63337 WHERE guid = 81144;
DELETE FROM creature_addon WHERE guid = 81144;
INSERT INTO creature_addon VALUES (81144,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9158.6, position_y = 87.9833, position_z = 76.3192, orientation = 3.05055 WHERE guid = 79889;
DELETE FROM creature_addon WHERE guid = 79889;
INSERT INTO creature_addon VALUES (79889,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8953.39, position_y = -432.069, position_z = 64.9321, orientation = 1.85876 WHERE guid = 80229;
DELETE FROM creature_addon WHERE guid = 80229;
INSERT INTO creature_addon VALUES (80229,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8958.41, position_y = -425.765, position_z = 64.9529, orientation = 0.232111 WHERE guid = 80231;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 80231;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 80230;
UPDATE creature SET position_x = -9436.99, position_y = 482.065, position_z = 53.624, orientation = 1.10826 WHERE guid = 80394;
DELETE FROM creature_addon WHERE guid = 80394;
INSERT INTO creature_addon VALUES (80394,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9441.15, position_y = 468.821, position_z = 53.2751, orientation = 4.21146 WHERE guid = 80391;
DELETE FROM creature_addon WHERE guid = 80391;
INSERT INTO creature_addon VALUES (80391,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9863.19, position_y = -217.097, position_z = 36.1244, orientation = 5.0478 WHERE guid = 80747;
DELETE FROM creature_addon WHERE guid = 80747;
INSERT INTO creature_addon VALUES (80747,0,0,1,4097,0,'');
UPDATE creature SET position_x = -9859.02, position_y = -220.764, position_z = 35.2197, orientation = 2.88709 WHERE guid = 80747;
DELETE FROM creature_addon WHERE guid = 80747;
INSERT INTO creature_addon VALUES (80747,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8848.86, position_y = -830.219, position_z = 72.9368, orientation = 2.25471 WHERE guid = 501804;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 501804);
DELETE FROM creature_addon WHERE guid = 5284;
INSERT INTO creature_addon VALUES (5284,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9153.94, position_y = -589.377, position_z = 59.0815, orientation = 1.27033 WHERE guid = 80976;
DELETE FROM creature_addon WHERE guid = 80976;
INSERT INTO creature_addon VALUES (80976,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9139.9, position_y = -602.645, position_z = 57.9601, orientation = 5.4661 WHERE guid = 80974;
DELETE FROM creature_addon WHERE guid = 80974;
INSERT INTO creature_addon VALUES (80974,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9126.95, position_y = -598.915, position_z = 57.3118, orientation = 5.73837 WHERE guid = 80975;
DELETE FROM creature_addon WHERE guid = 80975;
INSERT INTO creature_addon VALUES (80975,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9109.96, position_y = -588.702, position_z = 57.1763, orientation = 0.317376 WHERE guid = 80980;
DELETE FROM creature_addon WHERE guid = 80980;
INSERT INTO creature_addon VALUES (80980,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9099.73, position_y = -604.907, position_z = 57.4027, orientation = 5.11005 WHERE guid = 80984;
DELETE FROM creature_addon WHERE guid = 80984;
INSERT INTO creature_addon VALUES (80984,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9099.59, position_y = -583.103, position_z = 58.2384, orientation = 2.0278 WHERE guid = 80986;
DELETE FROM creature_addon WHERE guid = 80986;
INSERT INTO creature_addon VALUES (80986,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9084.79, position_y = -587.779, position_z = 58.315, orientation = 0.170765 WHERE guid = 80983;
DELETE FROM creature_addon WHERE guid = 80983;
INSERT INTO creature_addon VALUES (80983,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9114.8, position_y = -562.566, position_z = 59.9672, orientation = 1.53561 WHERE guid = 80985;
DELETE FROM creature_addon WHERE guid = 80985;
INSERT INTO creature_addon VALUES (80985,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9082.97, position_y = -556.574, position_z = 60.3386, orientation = 6.06649 WHERE guid = 80982;
DELETE FROM creature_addon WHERE guid = 80982;
INSERT INTO creature_addon VALUES (80982,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9087.46, position_y = -571.504, position_z = 62.5464, orientation = 0.450019 WHERE guid = 80981;
DELETE FROM creature_addon WHERE guid = 80981;
INSERT INTO creature_addon VALUES (80981,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9073.09, position_y = -582.797, position_z = 62.4685, orientation = 6.20611 WHERE guid = 80979;
DELETE FROM creature_addon WHERE guid = 80979;
INSERT INTO creature_addon VALUES (80979,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9814.01, position_y = 107.432, position_z = 6.22107, orientation = 5.12949 WHERE guid = 80610;
DELETE FROM creature_addon WHERE guid = 80610;
INSERT INTO creature_addon VALUES (80610,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9798.92, position_y = 121.049, position_z = 6.01491, orientation = 0.752205 WHERE guid = 80609;
DELETE FROM creature_addon WHERE guid = 80609;
INSERT INTO creature_addon VALUES (80609,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9790.51, position_y = 107.705, position_z = 5.11763, orientation = 4.09626 WHERE guid = 80636;
DELETE FROM creature_addon WHERE guid = 80636;
INSERT INTO creature_addon VALUES (80636,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9791.35, position_y = 86.9973, position_z = 5.66384, orientation = 4.95845 WHERE guid = 80637;
DELETE FROM creature_addon WHERE guid = 80637;
INSERT INTO creature_addon VALUES (80637,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9825.43, position_y = 88.3274, position_z = 0.731221, orientation = 1.90412 WHERE guid = 80630;
DELETE FROM creature_addon WHERE guid = 80630;
INSERT INTO creature_addon VALUES (80630,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9760.73, position_y = 81.6915, position_z = 13.6383, orientation = 4.05336 WHERE guid = 80649;
DELETE FROM creature_addon WHERE guid = 80649;
INSERT INTO creature_addon VALUES (80649,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9738.8, position_y = -93.2015, position_z = 13.098, orientation = 0.270491 WHERE guid = 80645;
DELETE FROM creature_addon WHERE guid = 80645;
INSERT INTO creature_addon VALUES (80645,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9730.67, position_y = 137.862, position_z = 18.6476, orientation = 5.94281 WHERE guid = 80639;
DELETE FROM creature_addon WHERE guid = 80639;
INSERT INTO creature_addon VALUES (80639,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9753.82, position_y = 135.642, position_z = 20.72, orientation = 3.1817 WHERE guid = 80640;
DELETE FROM creature_addon WHERE guid = 80640;
INSERT INTO creature_addon VALUES (80640,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9758.48, position_y = 157.258, position_z = 24.735, orientation = 0.801072 WHERE guid = 80615;
DELETE FROM creature_addon WHERE guid = 80615;
INSERT INTO creature_addon VALUES (80615,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9775.74, position_y = 158.307, position_z = 26.199, orientation = 1.65279 WHERE guid = 80617;
DELETE FROM creature_addon WHERE guid = 80617;
INSERT INTO creature_addon VALUES (80617,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9777.08, position_y = 134.271, position_z = 25.9625, orientation = 5.42271 WHERE guid = 80616;
DELETE FROM creature_addon WHERE guid = 80616;
INSERT INTO creature_addon VALUES (80616,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9785.94, position_y = 159.056, position_z = 26.2062, orientation = 0.986079 WHERE guid = 80620;
DELETE FROM creature_addon WHERE guid = 80620;
INSERT INTO creature_addon VALUES (80620,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9832.84, position_y = 155.847, position_z = 29.4553, orientation = 3.32482 WHERE guid = 80614;
DELETE FROM creature_addon WHERE guid = 80614;
INSERT INTO creature_addon VALUES (80614,0,0,0,4097,467,'');
UPDATE creature SET position_x = -9287.31, position_y = 512.419, position_z = 82.2279, orientation = 2.94784 WHERE guid = 80385;
UPDATE creature SET position_x = -9438.23, position_y = 486.452, position_z = 53.1618, orientation = 3.93221 WHERE guid = 80393;
UPDATE creature SET position_x = 7459.23, position_y = -3122.61, position_z = 438.984, orientation = 5.64032 WHERE guid = 42360;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 42360;
UPDATE creature SET position_x = 6846.47, position_y = -2490.99, position_z = 561.694, orientation = 3.94188 WHERE guid = 41039;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41039;
UPDATE creature SET position_x = 6789.5, position_y = -2661.51, position_z = 544.585, orientation = 3.52649 WHERE guid = 41061;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41061;
UPDATE creature SET position_x = 6790.35, position_y = -2670.82, position_z = 544.377, orientation = 2.52467 WHERE guid = 41059;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41059;
UPDATE creature SET position_x = 6859.78, position_y = -5105.3, position_z = 693.132, orientation = 2.62622 WHERE guid = 41030;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41030;
UPDATE creature SET position_x = 6845.1, position_y = -5115.15, position_z = 694.23, orientation = 1.65931 WHERE guid = 41034;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41034;
UPDATE creature SET position_x = 6833.79, position_y = -5108.67, position_z = 693.505, orientation = 0.269677 WHERE guid = 41048;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41048;
UPDATE creature SET position_x = 6833.53, position_y = -5041.07, position_z = 690.69, orientation = 6.05754 WHERE guid = 41049;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41049;
UPDATE creature SET position_x = 6787.79, position_y = -5150.82, position_z = 731.869, orientation = 4.29406 WHERE guid = 41026;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41026;
UPDATE creature SET position_x = 6779.21, position_y = -5061.91, position_z = 722.837, orientation = 2.34601 WHERE guid = 41025;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41025;
UPDATE creature SET position_x = 6776.26, position_y = -5053.47, position_z = 722.834, orientation = 5.32511 WHERE guid = 41024;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41024;
UPDATE creature SET position_x = 6730.75, position_y = -5134.24, position_z = 733.39, orientation = 0.202659 WHERE guid = 41031;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41031;
UPDATE creature SET position_x = 6724.96, position_y = -5146.41, position_z = 733.477, orientation = 5.00345 WHERE guid = 41028;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41028;
UPDATE creature SET position_x = 6743.6, position_y = -5204.63, position_z = 762.269, orientation = 3.57568 WHERE guid = 41027;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 41027;
UPDATE creature SET position_x = 6725.25, position_y = -5274.67, position_z = 778.199, orientation = 1.81778 WHERE guid = 42315;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 42315;
UPDATE creature SET position_x = 4559.17, position_y = 144.869, position_z = 59.4827, orientation = 1.67817 WHERE guid = 37353;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37353;
UPDATE creature SET position_x = 4560.9, position_y = 152.769, position_z = 59.0622, orientation = 4.18097 WHERE guid = 37326;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37326;
UPDATE creature SET position_x = 4665.18, position_y = 39.5621, position_z = 65.7598, orientation = 3.11767 WHERE guid = 37370;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37370;
UPDATE creature SET position_x = 4657.83, position_y = 35.5051, position_z = 65.7482, orientation = 1.97116 WHERE guid = 37384;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37384;
UPDATE creature SET position_x = 4621.09, position_y = 52.4438, position_z = 68.7987, orientation = 0.51294 WHERE guid = 37383;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37383;
UPDATE creature SET position_x = 6829.17, position_y = -387.82, position_z = 40.2858, orientation = 2.68772 WHERE guid = 37328;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37328;
UPDATE creature SET position_x = 6806.11, position_y = -396.398, position_z = 40.2088, orientation = 0.280883 WHERE guid = 37333;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37333;
UPDATE creature SET position_x = 6811.8, position_y = -409.983, position_z = 40.1818, orientation = 1.4691 WHERE guid = 37357;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37357;
UPDATE creature SET position_x = 6853.65, position_y = -484.541, position_z = 40.1436, orientation = 5.7277 WHERE guid = 37361;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37361;
UPDATE creature SET position_x = 6865.83, position_y = -517.246, position_z = 40.1436, orientation = 1.06111 WHERE guid = 37360;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37360;
UPDATE creature SET position_x = 6889.96, position_y = -492.03, position_z = 40.1436, orientation = 4.17826 WHERE guid = 37358;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 37358;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 36997;
UPDATE creature SET position_x = 7174.65, position_y = -682.451, position_z = 43.3661, orientation = 1.57816 WHERE guid = 37001;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 37001;
UPDATE creature SET position_x = 7138.25, position_y = -759.011, position_z = 52.8807, orientation = 0.220293 WHERE guid = 36987;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 36987;
UPDATE creature SET position_x = 7188.82, position_y = -814.862, position_z = 39.8169, orientation = 1.1523 WHERE guid = 36981;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 36981;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 36984;
UPDATE creature SET position_x = 7206.67, position_y = -845.199, position_z = 36.0919, orientation = 0.352938 WHERE guid = 36983;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 36983;
UPDATE creature SET position_x = -451.258, position_y = 1753.87, position_z = 153.488, orientation = 3.95048 WHERE guid = 29140;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 29140;
UPDATE creature SET position_x = -339.034, position_y = 1650.53, position_z = 132.294, orientation = 1.68504 WHERE guid = 27620;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 27620;
UPDATE creature SET position_x = -1382.51, position_y = 2490.68, position_z = 89.2181, orientation = 4.63159 WHERE guid = 27318;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 27318;
UPDATE creature SET position_x = -219.019, position_y = 1663.6, position_z = 99.3311, orientation = 5.98204 WHERE guid = 27626;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 27626;
UPDATE creature SET position_x = -1036.7, position_y = 901.073, position_z = 92.7503, orientation = 6.12908 WHERE guid = 27039;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 27039;
UPDATE creature SET position_x = -1376.82, position_y = 2828.22, position_z = 112.817, orientation = 5.38906 WHERE guid = 27394;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27394;
UPDATE creature SET position_x = -1378.39, position_y = 2773.32, position_z = 112.449, orientation = 0.893094 WHERE guid = 27389;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27389;
UPDATE creature SET position_x = -1341.05, position_y = 2792.99, position_z = 112.665, orientation = 2.81645 WHERE guid = 27393;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27393;
UPDATE creature SET position_x = -1339.68, position_y = 2767.84, position_z = 113.217, orientation = 2.47087 WHERE guid = 27335;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27335;
UPDATE creature SET position_x = -1365.97, position_y = 2907.07, position_z = 127.295, orientation = 4.24762 WHERE guid = 27337;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27337;
UPDATE creature SET position_x = -1278, position_y = 2893.07, position_z = 114.33, orientation = 3.92997 WHERE guid = 27334;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27334;
UPDATE creature SET position_x = -1295.75, position_y = 2885.69, position_z = 113.621, orientation = 5.69624 WHERE guid = 27333;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27333;
UPDATE creature SET position_x = -1289.32, position_y = 2898.55, position_z = 113.169, orientation = 5.10143 WHERE guid = 27345;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27345;
UPDATE creature SET position_x = -1185.09, position_y = 2861.95, position_z = 139.036, orientation = 0.718563 WHERE guid = 27331;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27331;
UPDATE creature SET position_x = -1195.42, position_y = 2770.25, position_z = 111.144, orientation = 0.561482 WHERE guid = 27327;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27327;
UPDATE creature SET position_x = -1189.56, position_y = 2747.66, position_z = 111.122, orientation = 5.03134 WHERE guid = 27329;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27329;
UPDATE creature SET position_x = -1146.46, position_y = 2696.94, position_z = 111.224, orientation = 2.82343 WHERE guid = 27326;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27326;
UPDATE creature SET position_x = -1177.68, position_y = 2692.46, position_z = 111.683, orientation = 2.13228 WHERE guid = 27315;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27315;
UPDATE creature SET position_x = -1386.33, position_y = 2646.51, position_z = 111.557, orientation = 5.94408 WHERE guid = 27310;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27310;
UPDATE creature SET position_x = -1367.1, position_y = 2580.06, position_z = 116.119, orientation = 1.24217 WHERE guid = 27309;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27309;
UPDATE creature SET position_x = -1110.02, position_y = 2860.01, position_z = 139.737, orientation = 2.96306 WHERE guid = 27344;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27344;
UPDATE creature SET position_x = -1986.45, position_y = 2594.06, position_z = 61.6702, orientation = 2.63514 WHERE guid = 27216;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27216;
UPDATE creature SET position_x = -1991.15, position_y = 2633.75, position_z = 61.0699, orientation = 4.36651 WHERE guid = 27164;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27164;
UPDATE creature SET position_x = -2156.97, position_y = 2679.14, position_z = 61.1318, orientation = 4.30019 WHERE guid = 27218;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27218;
UPDATE creature SET position_x = -2276.69, position_y = 2662.98, position_z = 60.1887, orientation = 5.17285 WHERE guid = 27291;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27291;
UPDATE creature SET position_x = -2266.55, position_y = 2604.21, position_z = 62.0279, orientation = 1.60191 WHERE guid = 27288;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27288;
UPDATE creature SET position_x = -2302.99, position_y = 2632.19, position_z = 60.2993, orientation = 6.13278 WHERE guid = 27292;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27292;
UPDATE creature SET position_x = -2360.73, position_y = 2483.31, position_z = 71.0047, orientation = 0.724005 WHERE guid = 27293;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27293;
UPDATE creature SET position_x = -2365.19, position_y = 2438.09, position_z = 77.0439, orientation = 5.24608 WHERE guid = 27296;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27296;
UPDATE creature SET position_x = -2368.02, position_y = 2427.22, position_z = 77.1752, orientation = 0.561609 WHERE guid = 27282;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27282;
UPDATE creature SET position_x = -2301.49, position_y = 2422.87, position_z = 67.9272, orientation = 0.427877 WHERE guid = 27286;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27286;
UPDATE creature SET position_x = -1964.65, position_y = 1239.73, position_z = 91.1532, orientation = 2.56854 WHERE guid = 27118;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27118;
UPDATE creature SET position_x = -1947.95, position_y = 1248.35, position_z = 91.519, orientation = 5.42765 WHERE guid = 27123;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27123;
UPDATE creature SET position_x = -1963.86, position_y = 1268.89, position_z = 91.58, orientation = 5.34388 WHERE guid = 27117;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27117;
UPDATE creature SET position_x = -1935.26, position_y = 1253.77, position_z = 91.9754, orientation = 4.6981 WHERE guid = 27066;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27066;
UPDATE creature SET position_x = -1919.27, position_y = 1249.25, position_z = 92.5786, orientation = 5.15538 WHERE guid = 27088;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27088;
UPDATE creature SET position_x = -1897.13, position_y = 1100.49, position_z = 92.5808, orientation = 0.546395 WHERE guid = 27125;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27125;
UPDATE creature SET position_x = -1878.2, position_y = 1117.31, position_z = 92.3577, orientation = 3.99167 WHERE guid = 27124;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27124;
UPDATE creature SET position_x = -1882.57, position_y = 1089.35, position_z = 92.0062, orientation = 6.02673 WHERE guid = 27106;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27106;
UPDATE creature SET position_x = -1801.08, position_y = 1060.16, position_z = 91.5964, orientation = 5.33482 WHERE guid = 27120;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27120;
UPDATE creature SET position_x = -1799.48, position_y = 1042.99, position_z = 92.8258, orientation = 0.597997 WHERE guid = 27128;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27128;
UPDATE creature SET position_x = -1689.12, position_y = 1027.64, position_z = 94.18, orientation = 3.06719 WHERE guid = 27119;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27119;
UPDATE creature SET position_x = -1898.7, position_y = 987.44, position_z = 90.6244, orientation = 6.17737 WHERE guid = 27126;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27126;
UPDATE creature SET position_x = -1725.42, position_y = 922.016, position_z = 91.8857, orientation = 1.53257 WHERE guid = 27127;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27127;
UPDATE creature SET position_x = -1748.92, position_y = 933.427, position_z = 92.4926, orientation = 0.942645 WHERE guid = 27121;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27121;
UPDATE creature SET position_x = -842.228, position_y = 1067.38, position_z = 91.202, orientation = 2.52281 WHERE guid = 27036;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27036;
UPDATE creature SET position_x = -853.87, position_y = 1056.78, position_z = 91.2885, orientation = 4.52994 WHERE guid = 27011;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27011;
UPDATE creature SET position_x = -840.306, position_y = 951.375, position_z = 90.5414, orientation = 5.33926 WHERE guid = 27037;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27037;
UPDATE creature SET position_x = -818.954, position_y = 940.535, position_z = 90.2374, orientation = 2.9307 WHERE guid = 27054;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27054;
UPDATE creature SET position_x = -837.002, position_y = 925.413, position_z = 88.9018, orientation = 1.97671 WHERE guid = 27021;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27021;
UPDATE creature SET position_x = -949.344, position_y = 882.374, position_z = 91.3745, orientation = 1.27889 WHERE guid = 27045;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27045;
UPDATE creature SET position_x = -962.186, position_y = 886.858, position_z = 91.3743, orientation = 1.06945 WHERE guid = 27016;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27016;
UPDATE creature SET position_x = -1062.46, position_y = 911.293, position_z = 91.8349, orientation = 6.22166 WHERE guid = 27038;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27038;
UPDATE creature SET position_x = -1012.43, position_y = 876.265, position_z = 92.545, orientation = 1.91768 WHERE guid = 27042;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27042;
UPDATE creature SET position_x = -1046.51, position_y = 1012.36, position_z = 90.4732, orientation = 6.02171 WHERE guid = 27049;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27049;
UPDATE creature SET position_x = -1064.59, position_y = 1009.44, position_z = 90.1695, orientation = 0.964735 WHERE guid = 27035;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27035;
UPDATE creature SET position_x = -1069.65, position_y = 1046.27, position_z = 90.6404, orientation = 0.0117828 WHERE guid = 27018;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 27018;
UPDATE creature_addon SET emote = 16 WHERE guid = 27707;
UPDATE creature SET position_x = -447.442, position_y = 1751.15, position_z = 153.488, orientation = 4.24369 WHERE guid = 27628;
UPDATE creature_addon SET emote = 16 WHERE guid = 27628;
UPDATE creature SET position_x = -241.784, position_y = 1672.81, position_z = 100.813, orientation = 0.327175 WHERE guid = 27615;
UPDATE creature_addon SET emote = 16 WHERE guid = 27615;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27578;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27569;
UPDATE creature SET position_x = -369.481, position_y = 1717.57, position_z = 130.79, orientation = 4.16416 WHERE guid = 27619;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27619;
UPDATE creature SET position_x = -386.943, position_y = 1796.5, position_z = 127.894, orientation = 2.58912 WHERE guid = 27622;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27622;
UPDATE creature SET position_x = -381.587, position_y = 1802.92, position_z = 127.709, orientation = 2.42157 WHERE guid = 27623;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27623;
DELETE FROM creature_addon WHERE guid = 27642;
INSERT INTO creature_addon VALUES (27642,0,0,1,4097,0,'');
UPDATE creature SET position_x = -413.931, position_y = 1859.7, position_z = 127.463, orientation = 4.36587 WHERE guid = 27741;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27741;
UPDATE creature SET position_x = -214.504, position_y = 1711.56, position_z = 105.559, orientation = 5.31185 WHERE guid = 27617;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 27617;
UPDATE creature SET position_x = -7105.53, position_y = -2426.52, position_z = 243.498, orientation = 5.50944 WHERE guid = 7011;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7011;
UPDATE creature SET position_x = -7108.61, position_y = -2372.01, position_z = 240.874, orientation = 5.51991 WHERE guid = 7210;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7210;
UPDATE creature SET position_x = -7099.71, position_y = -2399.17, position_z = 241.542, orientation = 2.92635 WHERE guid = 6975;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 6975;
UPDATE creature SET position_x = -7123.55, position_y = -2391.99, position_z = 241.166, orientation = 4.16553 WHERE guid = 6973;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 6973;
UPDATE creature SET position_x = -7129.96, position_y = -2372.34, position_z = 240.491, orientation = 4.12365 WHERE guid = 6974;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 6974;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7004;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7810;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7044;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7304;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7042;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7402;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7467;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7313;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7470;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7468;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7403;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7401;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7254;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7203;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 7308;
UPDATE creature SET position_x = -6878.05, position_y = -1334.12, position_z = 241.273, orientation = 2.8215 WHERE guid = 6821;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 6821;
UPDATE creature SET position_x = -5855.99, position_y = -1996.61, position_z = 401.525, orientation = 2.71896 WHERE guid = 5171;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5171;
UPDATE creature SET position_x = -5581.85, position_y = 516.679, position_z = 384.64, orientation = 0.320802 WHERE guid = 2912;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 2912;
UPDATE creature SET position_x = -6424.27, position_y = -3416.29, position_z = 241.581, orientation = 2.45543 WHERE guid = 7030;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7030;
UPDATE creature SET position_x = -6334.41, position_y = -3118.39, position_z = 299.724, orientation = 3.90427 WHERE guid = 7822;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7822;
UPDATE creature SET position_x = -6364.5, position_y = -3087.53, position_z = 310.836, orientation = 0.881363 WHERE guid = 7133;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7133;
UPDATE creature SET position_x = -7036.06, position_y = -1283.11, position_z = 258.716, orientation = 1.9038 WHERE guid = 5823;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5823;
UPDATE creature SET position_x = -7032.88, position_y = -1271.48, position_z = 241.957, orientation = 2.54078 WHERE guid = 5822;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5822;
UPDATE creature SET position_x = -7018.53, position_y = -1306.61, position_z = 258.736, orientation = 4.69284 WHERE guid = 5777;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5777;
UPDATE creature SET position_x = -6898.24, position_y = -1070.36, position_z = 256.914, orientation = 4.39929 WHERE guid = 6833;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 6833;
UPDATE creature SET position_x = -6706, position_y = -1105.72, position_z = 185.57, orientation = 0.563049 WHERE guid = 5679;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 5679;
UPDATE creature SET position_x = -6486.05, position_y = 504.214, position_z = 386.047, orientation = 0.000784874 WHERE guid = 974;
DELETE FROM creature_addon WHERE guid = 974;
INSERT INTO creature_addon VALUES (974,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6513.95, position_y = 689.106, position_z = 387.275, orientation = 5.53235 WHERE guid = 936;
DELETE FROM creature_addon WHERE guid = 936;
INSERT INTO creature_addon VALUES (936,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 935;
INSERT INTO creature_addon VALUES (935,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6503.2, position_y = 684.203, position_z = 387.274, orientation = 4.69111 WHERE guid = 933;
DELETE FROM creature_addon WHERE guid = 933;
INSERT INTO creature_addon VALUES (933,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 879;
INSERT INTO creature_addon VALUES (879,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 869;
INSERT INTO creature_addon VALUES (869,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5565.3, position_y = 499.527, position_z = 381.908, orientation = 1.50064 WHERE guid = 2915;
DELETE FROM creature_addon WHERE guid = 2915;
INSERT INTO creature_addon VALUES (2915,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5558.73, position_y = 514.79, position_z = 382.155, orientation = 2.66653 WHERE guid = 2913;
DELETE FROM creature_addon WHERE guid = 2913;
INSERT INTO creature_addon VALUES (2913,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5561.18, position_y = 518.706, position_z = 371.855, orientation = 4.56195 WHERE guid = 2911;
DELETE FROM creature_addon WHERE guid = 2911;
INSERT INTO creature_addon VALUES (2911,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5565.02, position_y = 515.761, position_z = 381.849, orientation = 0.00664091 WHERE guid = 2914;
DELETE FROM creature_addon WHERE guid = 2914;
INSERT INTO creature_addon VALUES (2914,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5309.81, position_y = 599.071, position_z = 390.545, orientation = 5.05064 WHERE guid = 2462;
DELETE FROM creature_addon WHERE guid = 2462;
INSERT INTO creature_addon VALUES (2462,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5078.3, position_y = 489.124, position_z = 401.822, orientation = 4.16052 WHERE guid = 2424;
DELETE FROM creature_addon WHERE guid = 2424;
INSERT INTO creature_addon VALUES (2424,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 3195;
INSERT INTO creature_addon VALUES (3195,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 3194;
INSERT INTO creature_addon VALUES (3194,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 3202;
INSERT INTO creature_addon VALUES (3202,0,0,1,4097,0,'');
UPDATE creature SET position_x = -7141.63, position_y = -2416, position_z = 240.516, orientation = 5.99114 WHERE guid = 7014;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7014;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7012;
UPDATE creature SET position_x = -7245.19, position_y = -2324.14, position_z = 244.448, orientation = 0.592146 WHERE guid = 7037;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7037;
UPDATE creature SET position_x = -7052.18, position_y = 3633.63, position_z = 241.667, orientation = 0.205442 WHERE guid = 7194;
DELETE FROM creature_addon WHERE guid = 7194;
INSERT INTO creature_addon VALUES (7194,0,0,1,4097,0,'');
UPDATE creature SET position_x = -7043.97, position_y = -3641.01, position_z = 241.668, orientation = 2.02757 WHERE guid = 7193;
DELETE FROM creature_addon WHERE guid = 7193;
INSERT INTO creature_addon VALUES (7193,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6274.86, position_y = -3741.61, position_z = 244.193, orientation = 5.14856 WHERE guid = 7195;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7195;
UPDATE creature SET position_x = -6248.66, position_y = -3735.16, position_z = 243.455, orientation = 2.75048 WHERE guid = 7838;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7838;
UPDATE creature SET position_x = -6398.42, position_y = -3412.74, position_z = 241.667, orientation = 1.92485 WHERE guid = 7164;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 7164;
UPDATE creature SET position_x = -6401.87, position_y = 3410.81, position_z = 241.663, orientation = 0.235367 WHERE guid = 7165;
DELETE FROM creature_addon WHERE guid = 7165;
INSERT INTO creature_addon VALUES (7165,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6794.57, position_y = -1084.52, position_z = 243.182, orientation = 4.15145 WHERE guid = 6777;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6777;
UPDATE creature SET position_x = -6798.75, position_y = -1088.92, position_z = 243.702, orientation = 0.744566 WHERE guid = 6779;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6779;
UPDATE creature SET position_x = -6860.67, position_y = -1174.4, position_z = 240.233, orientation = 0.66428 WHERE guid = 6809;
DELETE FROM creature_addon WHERE guid = 6809;
INSERT INTO creature_addon VALUES (6809,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6838.26, position_y = -1167.12, position_z = 241.227, orientation = 3.55804 WHERE guid = 6814;
DELETE FROM creature_addon WHERE guid = 6814;
INSERT INTO creature_addon VALUES (6814,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6866.66, position_y = -1282.95, position_z = 242.919, orientation = 2.51083 WHERE guid = 6820;
DELETE FROM creature_addon WHERE guid = 6820;
INSERT INTO creature_addon VALUES (6820,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6870.79, position_y = -1288.56, position_z = 242.872, orientation = 2.56494 WHERE guid = 6819;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6819;
UPDATE creature SET position_x = -6703.76, position_y = -1279.69, position_z = 240.933, orientation = 4.09211 WHERE guid = 6784;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6784;
UPDATE creature SET position_x = -6701.88, position_y = -1283.79, position_z = 240.889, orientation = 2.60159 WHERE guid = 6782;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6782;
UPDATE creature SET position_x = -6681.88, position_y = -1338.19, position_z = 247.962, orientation = 4.22824 WHERE guid = 6787;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6787;
UPDATE creature SET position_x = -6695.49, position_y = -1418.96, position_z = 242.294, orientation = 6.17602 WHERE guid = 6797;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6797;
UPDATE creature SET position_x = -6693.38, position_y = -1423.42, position_z = 241.63, orientation = 2.55272 WHERE guid = 6794;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 6794;
UPDATE creature SET position_x = -5855.99, position_y = -1996.61, position_z = 401.525, orientation = 2.71896 WHERE guid = 5171;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 5171);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -5171;
INSERT INTO `smart_scripts` VALUES
(-5171, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-5171, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -5538.19, position_y = 556.114, position_z = 394.457, orientation = 1.30866 WHERE guid = 2919;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 2919);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -2919;
INSERT INTO `smart_scripts` VALUES
(-2919, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-2919, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');

DELETE FROM creature_addon WHERE guid = 934;
INSERT INTO creature_addon VALUES (934,0,0,0,4097,467,'');
DELETE FROM creature_addon WHERE guid = 868;
INSERT INTO creature_addon VALUES (868,0,0,0,4097,467,'');
UPDATE creature SET position_x = -6452.74, position_y = -3436.99, position_z = 229.71, orientation = 4.17981 WHERE guid = 7076;
UPDATE creature_addon SET emote = 467 WHERE guid = 7076;
UPDATE creature SET position_x = -6457.14, position_y = -3421.79, position_z = 229.71, orientation = 2.6509 WHERE guid = 7074;
UPDATE creature_addon SET emote = 467 WHERE guid = 7074;
UPDATE creature SET position_x = -6438.32, position_y = -3422.37, position_z = 229.71, orientation = 0.203955 WHERE guid = 7070;
UPDATE creature_addon SET emote = 467 WHERE guid = 7070;
UPDATE creature SET position_x = -6443.12, position_y = -3403.56, position_z = 229.71, orientation = 2.41703 WHERE guid = 7075;
UPDATE creature_addon SET emote = 467 WHERE guid = 7075;
UPDATE creature SET position_x = -6436.72, position_y = -3385.9, position_z = 229.59, orientation = 2.90922 WHERE guid = 7072;
UPDATE creature_addon SET emote = 467 WHERE guid = 7072;
UPDATE creature SET position_x = -6415.71, position_y = -3388.88, position_z = 230.408, orientation = 5.52721 WHERE guid = 7067;
UPDATE creature_addon SET emote = 467 WHERE guid = 7067;
UPDATE creature SET position_x = -6402.66, position_y = -3383.66, position_z = 231.074, orientation = 0.00149775 WHERE guid = 7066;
UPDATE creature_addon SET emote = 467 WHERE guid = 7066;
UPDATE creature SET position_x = -6414.9, position_y = -3369.09, position_z = 229.711, orientation = 1.10106 WHERE guid = 7078;
UPDATE creature_addon SET emote = 467 WHERE guid = 7078;
UPDATE creature_addon SET emote = 467 WHERE guid = 8190;
UPDATE creature_addon SET emote = 467 WHERE guid = 5778;
UPDATE creature SET position_x = -6706, position_y = -1105.72, position_z = 185.57, orientation = 0.563049 WHERE guid = 5679;
UPDATE creature_addon SET emote = 467 WHERE guid = 5679;
UPDATE creature SET position_x = -6701.1, position_y = -1139.18, position_z = 185.565, orientation = 0.856265 WHERE guid = 5685;
UPDATE creature_addon SET emote = 467 WHERE guid = 5685;
UPDATE creature_addon SET emote = 467 WHERE guid = 5684;
UPDATE creature_addon SET emote = 467 WHERE guid = 5686;
UPDATE creature_addon SET emote = 467 WHERE guid = 5683;
UPDATE creature_addon SET emote = 467 WHERE guid = 5641;
UPDATE creature_addon SET emote = 467 WHERE guid = 5583;
UPDATE creature_addon SET emote = 467 WHERE guid = 5581;
UPDATE creature_addon SET emote = 467 WHERE guid = 5671;
UPDATE creature_addon SET emote = 467 WHERE guid = 5807;
UPDATE creature SET position_x = -5554.84, position_y = 521.41, position_z = 382.617, orientation = 0.237024 WHERE guid = 2916;
UPDATE creature SET position_x = -6830.28, position_y = -1223.77, position_z = 240.014, orientation = 2.21413 WHERE guid = 6817;
UPDATE creature SET position_x = -6685.21, position_y = -1347.11, position_z = 247.962, orientation = 4.72042 WHERE guid = 6790;
UPDATE creature SET position_x = -6460.22, position_y = -1199.98, position_z = 181.016, orientation = 2.55273 WHERE guid = 5811;
UPDATE creature SET position_x = -1353.27, position_y = -3913.65, position_z = 8.16514, orientation = 1.71636 WHERE guid = 13760;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13760;
UPDATE creature SET position_x = -1512.23, position_y = -3815.51, position_z = 22.9664, orientation = 5.04606 WHERE guid = 13766;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13766;
UPDATE creature SET position_x = 1498.34, position_y = -3813.21, position_z = 24.2483, orientation = 4.40029 WHERE guid = 13795;

DELETE FROM `creature_addon` WHERE `guid` = 13795;

INSERT INTO `creature_addon` VALUES (13795, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1716.61, position_y = -3834.56, position_z = 11.1503, orientation = 1.38087 WHERE guid = 13816;

DELETE FROM `creature_addon` WHERE `guid` = 13816;

INSERT INTO `creature_addon` VALUES (13816, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1738.45, position_y = -3727.45, position_z = 14.4764, orientation = 3.63932 WHERE guid = 13789;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13789;
UPDATE creature SET position_x = -2123.58, position_y = -3663.12, position_z = 60.9543, orientation = 0.55642 WHERE guid = 13871;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13871;
UPDATE creature SET position_x = -2205.22, position_y = -3687.85, position_z = 92.8067, orientation = 0.0956576 WHERE guid = 13886;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13886;
UPDATE creature SET position_x = -2196.62, position_y = -3674.08, position_z = 92.2828, orientation = 5.71816 WHERE guid = 13905;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13905;
UPDATE creature SET position_x = -2144.38, position_y = -3587.93, position_z = 92.1668, orientation = 2.70223 WHERE guid = 13866;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13866;
UPDATE creature SET position_x = -1683.69, position_y = -3608.92, position_z = 93.2598, orientation = 3.09668 WHERE guid = 13833;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13833;
UPDATE creature SET position_x = -1696.45, position_y = -3598.95, position_z = 91.7737, orientation = 5.02003 WHERE guid = 13851;

DELETE FROM `creature_addon` WHERE `guid` = 13851;

INSERT INTO `creature_addon` VALUES (13851, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1629.22, position_y = -3606.53, position_z = 91.6671, orientation = 4.18228 WHERE guid = 13831;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13831;
UPDATE creature SET position_x = -1497.57, position_y = -3676.7, position_z = 91.7582, orientation = 4.34284 WHERE guid = 13857;

DELETE FROM `creature_addon` WHERE `guid` = 13857;

INSERT INTO `creature_addon` VALUES (13857, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1363.37, position_y = -3664.31, position_z = 92.8751, orientation = 4.26256 WHERE guid = 13855;

DELETE FROM `creature_addon` WHERE `guid` = 13855;

INSERT INTO `creature_addon` VALUES (13855, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1368.37, position_y = -3594.74, position_z = 91.7584, orientation = 3.90652 WHERE guid = 13835;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 13835;
UPDATE creature SET position_x = -391.87, position_y = -2225.26, position_z = 184.308, orientation = 5.87308 WHERE guid = 13744;

DELETE FROM `creature_addon` WHERE `guid` = 13744;

INSERT INTO `creature_addon` VALUES (13744, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4584.6, position_y = -2146.45, position_z = 90.0827, orientation = 2.17178 WHERE guid = 20368;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 20368;
UPDATE creature SET position_x = -4860.96, position_y = -2299.1, position_z = 86.7481, orientation = 4.41627 WHERE guid = 20338;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 20338;
UPDATE creature SET position_x = -1270.57, position_y = -5516.61, position_z = 4.97543, orientation = 4.66621 WHERE guid = 12361;

DELETE FROM `creature_addon` WHERE `guid` = 12361;

INSERT INTO `creature_addon` VALUES (12361, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1254.41, position_y = -5534.96, position_z = 5.24493, orientation = 2.95578 WHERE guid = 12358;

DELETE FROM `creature_addon` WHERE `guid` = 12358;

INSERT INTO `creature_addon` VALUES (12358, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 431.906, position_y = -4251.63, position_z = 25.4746, orientation = 1.92242 WHERE guid = 7348;

DELETE FROM `creature_addon` WHERE `guid` = 7348;

INSERT INTO `creature_addon` VALUES (7348, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -85.9186, position_y = -4043.88, position_z = 64.2063, orientation = 1.09513 WHERE guid = 6445;

DELETE FROM `creature_addon` WHERE `guid` = 6445;

INSERT INTO `creature_addon` VALUES (6445, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 34.3413, position_y = -4290.46, position_z = 69.1948, orientation = 5.79007 WHERE guid = 7326;

DELETE FROM `creature_addon` WHERE `guid` = 7326;

INSERT INTO `creature_addon` VALUES (7326, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -33.6544, position_y = -3380.71, position_z = 91.7854, orientation = 2.73211 WHERE guid = 20343;

DELETE FROM `creature_addon` WHERE `guid` = 20343;

INSERT INTO `creature_addon` VALUES (20343, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -94.8824, position_y = -2866.28, position_z = 91.6672, orientation = 0.152514 WHERE guid = 20396;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 20396;
UPDATE creature SET position_x = -1562.51, position_y = -3877.82, position_z = 17.5446, orientation = 3.17995 WHERE guid = 13768;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 13768;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 13876;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 13894;
UPDATE creature SET position_x = -1132.62, position_y = -5519.68, position_z = 6.42267, orientation = 4.29271 WHERE guid = 12374;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 12374;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 13032;
UPDATE creature SET position_x = 73.9832, position_y = -4536.62, position_z = 61.2027, orientation = 5.41701 WHERE guid = 12315;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 12315;
UPDATE creature SET position_x = -1157.47, position_y = -3127.11, position_z = 93.7628, orientation = 3.0509 WHERE guid = 14007;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 14007;
UPDATE creature SET position_x = -1132.06, position_y = -3170.99, position_z = 93.5824, orientation = 2.17946 WHERE guid = 20588;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 20588;
UPDATE creature SET position_x = 59.3298, position_y = -2076.95, position_z = 91.6668, orientation = 4.78043 WHERE guid = 20479;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 20479;
UPDATE creature SET position_x = -1574.35, position_y = -3893.38, position_z = 14.3381, orientation = 2.042 WHERE guid = 15066;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 15066;
UPDATE creature SET position_x = -1357.47, position_y = -3896.77, position_z = 10.5585, orientation = 4.97764 WHERE guid = 13797;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13797;
UPDATE creature SET position_x = -1358.4, position_y = -3901.17, position_z = 10.1848, orientation = 0.876122 WHERE guid = 13758;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13758;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13808;
UPDATE creature SET position_x = -1502.92, position_y = -3824.4, position_z = 23.4833, orientation = 2.18023 WHERE guid = 13767;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13767;
UPDATE creature SET position_x = -1715.65, position_y = -3823.63, position_z = 12.3789, orientation = 6.2259 WHERE guid = 13790;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13790;
UPDATE creature SET position_x = -1749.33, position_y = -3730.01, position_z = 13.5787, orientation = 0.448218 WHERE guid = 13818;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13818;
UPDATE creature SET position_x = -1695.06, position_y = -3612.79, position_z = 91.8868, orientation = 1.4456 WHERE guid = 13854;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13854;
UPDATE creature SET position_x = -1631.58, position_y = -3617.65, position_z = 91.6671, orientation = 2.57657 WHERE guid = 13849;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13849;
UPDATE creature SET position_x = -1590.11, position_y = -3683.06, position_z = 91.9939, orientation = 4.90484 WHERE guid = 13850;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13850;
UPDATE creature SET position_x = -1503.45, position_y = -3685.14, position_z = 91.8024, orientation = 6.14053 WHERE guid = 13838;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13838;
UPDATE creature SET position_x = -1376.96, position_y = -3599.99, position_z = 92.0034, orientation = 5.94157 WHERE guid = 13826;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13826;
UPDATE creature SET position_x = -1134.18, position_y = -5541.51, position_z = 10.6749, orientation = 1.50717 WHERE guid = 12367;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 12367;
UPDATE creature SET position_x = 436.958, position_y = -4210.28, position_z = 24.612, orientation = 4.49992 WHERE guid = 7351;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7351;
UPDATE creature SET position_x = 428.499, position_y = -4214.34, position_z = 24.4876, orientation = 4.92788 WHERE guid = 12328;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 12328;
UPDATE creature SET position_x = 26.971, position_y = -4339.08, position_z = 73.4466, orientation = 0.983433 WHERE guid = 7331;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7331;
UPDATE creature SET position_x = 32.0841, position_y = -4303.17, position_z = 69.561, orientation = 5.91224 WHERE guid = 8425;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 8425;
UPDATE creature SET position_x = 63.7112, position_y = -4448.18, position_z = 46.3869, orientation = 5.79051 WHERE guid = 6413;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 6413;
UPDATE creature SET position_x = 50.607, position_y = -4456.19, position_z = 48.4505, orientation = 5.83937 WHERE guid = 6414;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 6414;
UPDATE creature SET position_x = 52.6596, position_y = -4478.87, position_z = 50.119, orientation = 1.03274 WHERE guid = 7877;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7877;
UPDATE creature SET position_x = 109.524, position_y = -4551.88, position_z = 57.4286, orientation = 3.95442 WHERE guid = 8427;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 8427;
UPDATE creature SET position_x = 93.7723, position_y = -4564.86, position_z = 54.8645, orientation = 2.8025 WHERE guid = 12309;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 12309;
UPDATE creature SET position_x = 89.5106, position_y = -4559.46, position_z = 54.5843, orientation = 4.82011 WHERE guid = 7332;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 7332;
UPDATE creature SET position_x = 86.0809, position_y = -4565.59, position_z = 54.9529, orientation = 0.523105 WHERE guid = 12314;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 12314;
UPDATE creature SET position_x = 90.3229, position_y = -4548.33, position_z = 56.7675, orientation = 3.91254 WHERE guid = 12313;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 12313;
UPDATE creature SET position_x = -1064.02, position_y = -4597.81, position_z = 25.8221, orientation = 6.16401 WHERE guid = 3449;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 3449;
UPDATE creature SET position_x = -923.65, position_y = -4478.58, position_z = 29.5333, orientation = 4.29651 WHERE guid = 3446;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 3446;
UPDATE creature SET position_x = -971.835, position_y = -4418.43, position_z = 29.2555, orientation = 2.96657 WHERE guid = 6588;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 6588;
UPDATE creature SET position_x = -891.303, position_y = -2925.74, position_z = 91.6731, orientation = 3.53383 WHERE guid = 20530;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20530;
UPDATE creature SET position_x = -905.546, position_y = -2934.63, position_z = 91.6667, orientation = 0.872896 WHERE guid = 20452;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20452;
UPDATE creature SET position_x = -859.867, position_y = -2749.41, position_z = 91.667, orientation = 2.35748 WHERE guid = 20488;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20488;
UPDATE creature SET position_x = -1146.88, position_y = -2797.89, position_z = 92.0033, orientation = 0.0745828 WHERE guid = 13996;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13996;
UPDATE creature SET position_x = -1122.24, position_y = -2796.24, position_z = 91.8851, orientation = 0.0501482 WHERE guid = 13995;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13995;
UPDATE creature SET position_x = -1104.05, position_y = -2783.82, position_z = 91.6672, orientation = 4.14469 WHERE guid = 20512;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20512;
UPDATE creature SET position_x = -1288.95, position_y = -2818.63, position_z = 93.053, orientation = 5.02782 WHERE guid = 20580;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20580;
UPDATE creature SET position_x = -1290.56, position_y = -2845.85, position_z = 93.532, orientation = 1.35216 WHERE guid = 20586;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20586;
UPDATE creature SET position_x = -1392.66, position_y = -2732.26, position_z = 91.6668, orientation = 3.71185 WHERE guid = 13994;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13994;
UPDATE creature SET position_x = -1416.83, position_y = -2739.03, position_z = 91.7485, orientation = 6.12738 WHERE guid = 13997;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 13997;
UPDATE creature SET position_x = -1397.6, position_y = -2752.26, position_z = 91.7001, orientation = 2.03633 WHERE guid = 20576;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20576;
UPDATE creature SET position_x = -1549.62, position_y = -2882, position_z = 91.6673, orientation = 5.46416 WHERE guid = 20585;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20585;
UPDATE creature SET position_x = -1543.68, position_y = -2904.25, position_z = 91.6673, orientation = 1.40452 WHERE guid = 20582;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20582;
UPDATE creature SET position_x = -1492.71, position_y = -3045.11, position_z = 91.7562, orientation = 1.43943 WHERE guid = 14003;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 14003;
UPDATE creature SET position_x = -1483.52, position_y = -3035.17, position_z = 91.68, orientation = 3.34123 WHERE guid = 20583;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20583;
UPDATE creature SET position_x = -1342.47, position_y = -3111.27, position_z = 91.6669, orientation = 5.40831 WHERE guid = 20579;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20579;
UPDATE creature SET position_x = -1335.84, position_y = -3136.7, position_z = 91.6669, orientation = 1.31377 WHERE guid = 14005;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 14005;
UPDATE creature SET position_x = -1325.09, position_y = -3127.12, position_z = 91.6669, orientation = 3.25109 WHERE guid = 14004;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 14004;
UPDATE creature SET position_x = -1172.96, position_y = -3154.76, position_z = 91.6974, orientation = 0.165348 WHERE guid = 20543;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20543;
UPDATE creature SET position_x = -1159.98, position_y = -3167.95, position_z = 91.6667, orientation = 1.99445 WHERE guid = 14002;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 14002;
UPDATE creature SET position_x = -1052.42, position_y = -2323.55, position_z = 91.6809, orientation = 1.5267 WHERE guid = 20445;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20445;
UPDATE creature SET position_x = -1031.83, position_y = -2317.38, position_z = 91.6743, orientation = 2.6856 WHERE guid = 20453;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20453;
UPDATE creature SET position_x = -1010.27, position_y = -2329.23, position_z = 92.0148, orientation = 2.60531 WHERE guid = 20517;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20517;
UPDATE creature SET position_x = -1218.72, position_y = -2176.04, position_z = 91.6251, orientation = 0.353835 WHERE guid = 20439;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20439;
UPDATE creature SET position_x = -1209.57, position_y = -2179.8, position_z = 91.624, orientation = 1.69076 WHERE guid = 20514;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20514;
UPDATE creature SET position_x = -1187.02, position_y = -2001.38, position_z = 91.6685, orientation = 5.25123 WHERE guid = 20481;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20481;
UPDATE creature SET position_x = -1195.3, position_y = -2016.92, position_z = 91.667, orientation = 0.992631 WHERE guid = 20518;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20518;
UPDATE creature SET position_x = -1060.12, position_y = -1910.27, position_z = 91.7393, orientation = 4.30875 WHERE guid = 20483;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20483;
UPDATE creature SET position_x = -1071.63, position_y = -1908.36, position_z = 91.6933, orientation = 5.10462 WHERE guid = 20482;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20482;
UPDATE creature SET position_x = -1078.42, position_y = -1918.74, position_z = 91.6933, orientation = 6.12041 WHERE guid = 20541;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20541;
UPDATE creature SET position_x = -940.945, position_y = -1897.78, position_z = 101.749, orientation = 1.83388 WHERE guid = 20505;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20505;
UPDATE creature SET position_x = -938.674, position_y = -1870.93, position_z = 102.069, orientation = 3.86195 WHERE guid = 20559;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20559;
UPDATE creature SET position_x = -110.292, position_y = -2007.1, position_z = 91.6669, orientation = 4.33668 WHERE guid = 20472;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20472;
UPDATE creature SET position_x = -102.322, position_y = -2027.91, position_z = 91.6669, orientation = 2.22832 WHERE guid = 20475;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20475;
UPDATE creature SET position_x = -125.119, position_y = -2009.97, position_z = 91.6668, orientation = 5.87257 WHERE guid = 20470;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20470;
UPDATE creature SET position_x = 71.0542, position_y = -2097.32, position_z = 91.6662, orientation = 2.82566 WHERE guid = 20538;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20538;
UPDATE creature SET position_x = 51.4488, position_y = -2097.69, position_z = 91.6662, orientation = 0.657964 WHERE guid = 20477;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20477;
UPDATE creature SET position_x = 261.491, position_y = -1950.35, position_z = 91.8744, orientation = 2.1415 WHERE guid = 20461;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20461;
UPDATE creature SET position_x = 262.528, position_y = -1936.15, position_z = 91.7202, orientation = 3.86588 WHERE guid = 20528;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20528;
UPDATE creature SET position_x = 154.723, position_y = -1830.11, position_z = 92.4899, orientation = 0.256537 WHERE guid = 20531;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20531;
UPDATE creature SET position_x = 160.578, position_y = -1810.94, position_z = 92.1511, orientation = 5.5683 WHERE guid = 20457;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20457;
UPDATE creature SET position_x = 191.486, position_y = -1801.97, position_z = 94.6589, orientation = 5.30054 WHERE guid = 20442;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20442;
UPDATE creature SET position_x = 172.621, position_y = -1806.22, position_z = 92.3787, orientation = 4.66524 WHERE guid = 20529;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20529;
UPDATE creature SET position_x = 32.1307, position_y = -1793.54, position_z = 91.6917, orientation = 2.9474 WHERE guid = 20456;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20456;
UPDATE creature SET position_x = 12.7285, position_y = -1783.67, position_z = 91.6784, orientation = 4.965 WHERE guid = 20510;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20510;
UPDATE creature SET position_x = 26.3394, position_y = -1733.85, position_z = 108.738, orientation = 1.57557 WHERE guid = 20511;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20511;
UPDATE creature SET position_x = -48.0991, position_y = -1640.24, position_z = 91.6669, orientation = 3.19524 WHERE guid = 20515;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20515;
UPDATE creature SET position_x = -62.4791, position_y = -1630.3, position_z = 91.6675, orientation = 5.0383 WHERE guid = 20449;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20449;
UPDATE creature SET position_x = -202.079, position_y = -1648.85, position_z = 91.6674, orientation = 4.53914 WHERE guid = 20535;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20535;
UPDATE creature SET position_x = -211.607, position_y = -1662.16, position_z = 91.6833, orientation = 6.22862 WHERE guid = 20465;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20465;
UPDATE creature SET position_x = -234.554, position_y = -1660.02, position_z = 91.7011, orientation = 6.16928 WHERE guid = 20458;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20458;
UPDATE creature SET position_x = -165.174, position_y = -1782.18, position_z = 91.667, orientation = 4.77999 WHERE guid = 20467;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20467;
UPDATE creature SET position_x = -173.414, position_y = -1794.8, position_z = 91.667, orientation = 0.193262 WHERE guid = 20468;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20468;
UPDATE creature SET position_x = -41.4369, position_y = -2213.99, position_z = 91.6672, orientation = 4.38205 WHERE guid = 20448;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20448;
UPDATE creature SET position_x = -269.73, position_y = -5126.6, position_z = 24.3416, orientation = 1.4994 WHERE guid = 13262;
DELETE FROM creature_addon WHERE guid = 13262;
INSERT INTO creature_addon VALUES (13262,0,0,0,4097,16,'');
UPDATE creature SET position_x = -67.865, position_y = -4958.74, position_z = 21.412, orientation = 6.04424 WHERE guid = 8410;
DELETE FROM creature_addon WHERE guid = 8410;
INSERT INTO creature_addon VALUES (8410,0,0,0,4097,16,'');
DELETE FROM creature_addon WHERE guid = 10206;
INSERT INTO creature_addon VALUES (10206,0,0,0,4097,16,'');
UPDATE creature SET position_x = -51.7646, position_y = -4961.44, position_z = 21.4199, orientation = 3.00388 WHERE guid = 7262;
DELETE FROM creature_addon WHERE guid = 7262;
INSERT INTO creature_addon VALUES (7262,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1135.16, position_y = -5589.33, position_z = 10.5109, orientation = 1.55159 WHERE guid = 12380;
DELETE FROM creature_addon WHERE guid = 12380;
INSERT INTO creature_addon VALUES (12380,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1153.95, position_y = -5570.38, position_z = 10.5518, orientation = 6.27445 WHERE guid = 12378;
DELETE FROM creature_addon WHERE guid = 12378;
INSERT INTO creature_addon VALUES (12378,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1116.6, position_y = -5571.63, position_z = 10.5527, orientation = 3.07701 WHERE guid = 12377;
DELETE FROM creature_addon WHERE guid = 12377;
INSERT INTO creature_addon VALUES (12377,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1134.96, position_y = -5571.31, position_z = 9.90866, orientation = 1.51415 WHERE guid = 12382;
DELETE FROM creature_addon WHERE guid = 12382;
INSERT INTO creature_addon VALUES (12382,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1102.69, position_y = -5524.41, position_z = 11.7738, orientation = 3.79355 WHERE guid = 13027;
DELETE FROM creature_addon WHERE guid = 13027;
INSERT INTO creature_addon VALUES (13027,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1284.4, position_y = -5508.64, position_z = 8.46743, orientation = 0.00617907 WHERE guid = 7908;
DELETE FROM creature_addon WHERE guid = 7908;
INSERT INTO creature_addon VALUES (7908,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1240.5, position_y = -5513.12, position_z = 9.53035, orientation = 3.23853 WHERE guid = 12357;
DELETE FROM creature_addon WHERE guid = 12357;
INSERT INTO creature_addon VALUES (12357,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1162.72, position_y = -5586.39, position_z = 12.3751, orientation = 2.23322 WHERE guid = 6457;
DELETE FROM creature_addon WHERE guid = 6457;
INSERT INTO creature_addon VALUES (6457,0,0,0,4097,16,'');
UPDATE creature SET position_x = 438.274, position_y = -4224.32, position_z = 25.398, orientation = 3.04292 WHERE guid = 7905;
DELETE FROM creature_addon WHERE guid = 7905;
INSERT INTO creature_addon VALUES (7905,0,0,0,4097,16,'');
UPDATE creature SET position_x = -1998.27, position_y = -3583.54, position_z = 22.1664, orientation = 0.601802 WHERE guid = 13891;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 13891);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -13891;
INSERT INTO `smart_scripts` VALUES
(-13891, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-13891, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -1975.68, position_y = -3592.48, position_z = 21.7602, orientation = 1.63154 WHERE guid = 13883;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 13883);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -13883;
INSERT INTO `smart_scripts` VALUES
(-13883, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-13883, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -2228.42, position_y = -3710.73, position_z = 91.8894, orientation = 1.84797 WHERE guid = 13899;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 13899);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -13899;
INSERT INTO `smart_scripts` VALUES
(-13899, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-13899, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -2232.24, position_y = -3721.85, position_z = 92.6277, orientation = 2.15514 WHERE guid = 13903;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 13903);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -13903;
INSERT INTO `smart_scripts` VALUES
(-13903, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-13903, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -1465.31, position_y = -3650.3, position_z = 92.1188, orientation = 6.01836 WHERE guid = 13832;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 13832);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -13832;
INSERT INTO `smart_scripts` VALUES
(-13832, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-13832, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -4060.08, position_y = -2170.52, position_z = 52.6094, orientation = 0.328167 WHERE guid = 13610;
UPDATE creature_addon SET emote = 467 WHERE guid = 13610;
UPDATE creature_addon SET emote = 467 WHERE guid = 13583;
UPDATE creature SET position_x = -4101.5, position_y = 2163.69, position_z = 50.6182, orientation = 2.71683 WHERE guid = 13598;
UPDATE creature_addon SET emote = 467 WHERE guid = 13598;
UPDATE creature SET position_x = -4111.89, position_y = 2188.03, position_z = 50.7965, orientation = 5.6804 WHERE guid = 13614;
UPDATE creature_addon SET emote = 467 WHERE guid = 13614;
UPDATE creature SET position_x = -4129.01, position_y = -2154.94, position_z = 50.429, orientation = 1.0448 WHERE guid = 13611;
UPDATE creature_addon SET emote = 467 WHERE guid = 13611;
UPDATE creature SET position_x = -4113.6, position_y = -2168.57, position_z = 50.1833, orientation = 0.9688008 WHERE guid = 13594;
UPDATE creature_addon SET emote = 467 WHERE guid = 13594;
UPDATE creature SET position_x = -4149.44, position_y = -2158.82, position_z = 51.2343, orientation = 2.14436 WHERE guid = 13615;
UPDATE creature_addon SET emote = 467 WHERE guid = 13615;
UPDATE creature_addon SET emote = 467 WHERE guid = 13600;
UPDATE creature SET position_x = -4186.78, position_y = -2192.55, position_z = 50.1852, orientation = 4.21781 WHERE guid = 13612;
UPDATE creature_addon SET emote = 467 WHERE guid = 13612;
UPDATE creature SET position_x = -4197.01, position_y = -2214.41, position_z = 54.4268, orientation = 1.73247 WHERE guid = 13591;
UPDATE creature_addon SET emote = 467 WHERE guid = 13591;
UPDATE creature SET position_x = -4161.21, position_y = -2220.54, position_z = 51.3881, orientation = 1.15651 WHERE guid = 13613;
UPDATE creature_addon SET emote = 467 WHERE guid = 13613;
UPDATE creature SET position_x = -4226.44, position_y = -2283.43, position_z = 55.9873, orientation = 3.78228 WHERE guid = 13584;
UPDATE creature_addon SET emote = 467 WHERE guid = 13584;
UPDATE creature SET position_x = -4212.66, position_y = -2275.04, position_z = 50.19, orientation = 1.79958 WHERE guid = 13984;
UPDATE creature_addon SET emote = 467 WHERE guid = 13984;
UPDATE creature SET position_x = -4059.93, position_y = -2145.39, position_z = 90.2221, orientation = 4.27795 WHERE guid = 13593;
UPDATE creature_addon SET emote = 467 WHERE guid = 13593;
UPDATE creature SET position_x = -3668.69, position_y = -4544.74, position_z = 9.23516, orientation = 2.11214 WHERE guid = 30652;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 30652;
UPDATE creature SET position_x = -3011.67, position_y = -4356.75, position_z = 7.30641, orientation = 1.63323 WHERE guid = 30709;

DELETE FROM `creature_addon` WHERE `guid` = 30709;

INSERT INTO `creature_addon` VALUES (30709, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2670, position_y = -4258.03, position_z = 3.62222, orientation = 3.34026 WHERE guid = 31276;

DELETE FROM `creature_addon` WHERE `guid` = 31276;

INSERT INTO `creature_addon` VALUES (31276, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2635.13, position_y = -5063.93, position_z = 25.6723, orientation = 0.699572 WHERE guid = 30859;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 30859;
UPDATE creature SET position_x = -3642.26, position_y = -3421.8, position_z = 37.1981, orientation = 3.2178 WHERE guid = 31552;

DELETE FROM `creature_addon` WHERE `guid` = 31552;

INSERT INTO `creature_addon` VALUES (31552, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2890.07, position_y = -3425.91, position_z = 39.3609, orientation = 1.32336 WHERE guid = 33895;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 33895;
UPDATE creature SET position_x = -5099.58, position_y = -3951.4, position_z = 41.5024, orientation = 5.38236 WHERE guid = 31041;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 31041;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 30707;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 30702;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 30710;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 31275;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 31568;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 31269;
UPDATE creature SET position_x = -2631.14, position_y = -5062.32, position_z = 26.4101, orientation = 2.3175 WHERE guid = 18610;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 18610;
UPDATE creature SET position_x = -2642.82, position_y = -5054.06, position_z = 22.9889, orientation = 2.46611 WHERE guid = 18617;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 18617;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 18618;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 18611;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 18616;
UPDATE creature SET position_x = -2895.31, position_y = -3338.72, position_z = 32.3193, orientation = 6.05136 WHERE guid = 31274;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 31274;
UPDATE creature SET position_x = -3693.77, position_y = -3459.37, position_z = 37.2807, orientation = 0.885721 WHERE guid = 33913;
DELETE FROM creature_addon WHERE guid = 33913;
INSERT INTO creature_addon VALUES (33913,0,0,1,4097,0,'');
UPDATE creature SET position_x = -3709.99, position_y = -3402.81, position_z = 37.1953, orientation = 5.54575 WHERE guid = 33912;
DELETE FROM creature_addon WHERE guid = 33912;
INSERT INTO creature_addon VALUES (33912,0,0,1,4097,0,'');
UPDATE creature SET position_x = -3885.23, position_y = -2828.36, position_z = 35.6199, orientation = 2.34898 WHERE guid = 31038;
UPDATE creature_addon SET emote = 467 WHERE guid = 31038;
UPDATE creature SET position_x = -3902.8, position_y = -2848.58, position_z = 44.0985, orientation = 3.63005 WHERE guid = 30715;
UPDATE creature_addon SET emote = 467 WHERE guid = 30715;
UPDATE creature SET position_x = -3940.16, position_y = -2847, position_z = 46.4108, orientation = 3.10646 WHERE guid = 33829;
UPDATE creature_addon SET emote = 467 WHERE guid = 33829;
UPDATE creature SET position_x = -3905, position_y = -2856.53, position_z = 46.3355, orientation = 0.373269 WHERE guid = 31594;
UPDATE creature_addon SET emote = 467 WHERE guid = 31594;
UPDATE creature SET position_x = -3902.19, position_y = 2869.89, position_z = 46.4755, orientation = 6.05955 WHERE guid = 30444;
UPDATE creature_addon SET emote = 467 WHERE guid = 30444;
UPDATE creature SET position_x = -3917.34, position_y = -2887.42, position_z = 46.4757, orientation = 5.42774 WHERE guid = 8454;
UPDATE creature_addon SET emote = 467 WHERE guid = 8454;
UPDATE creature SET position_x = -3929.61, position_y = -2888.16, position_z = 46.3368, orientation = 4.44687 WHERE guid = 8455;
UPDATE creature_addon SET emote = 467 WHERE guid = 8455;
UPDATE creature SET position_x = -3939.09, position_y = -2875.62, position_z = 46.4754, orientation = 1.2948 WHERE guid = 23508;
UPDATE creature_addon SET emote = 467 WHERE guid = 23508;
UPDATE creature SET position_x = -3948.75, position_y = -2870.21, position_z = 46.4741, orientation = 3.39269 WHERE guid = 30757;
UPDATE creature_addon SET emote = 467 WHERE guid = 30757;
UPDATE creature SET position_x = -3917.12, position_y = -2872.78, position_z = 46.4798, orientation = 5.49407 WHERE guid = 31601;
UPDATE creature_addon SET emote = 467 WHERE guid = 31601;
UPDATE creature SET position_x = -3935.15, position_y = -2859.72, position_z = 46.4798, orientation = 2.49559 WHERE guid = 31596;
UPDATE creature_addon SET emote = 467 WHERE guid = 31596;
UPDATE creature SET position_x = 1784.27, position_y = 1344.78, position_z = 89.4031, orientation = 5.7286 WHERE guid = 29881;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 29881;
UPDATE creature SET position_x = 7934.32, position_y = -6142.93, position_z = 18.6012, orientation = 1.75231 WHERE guid = 82204;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 82204;
UPDATE creature SET position_x = 7167.57, position_y = -6791.1, position_z = 42.4918, orientation = 5.75269 WHERE guid = 82767;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 82767;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 44957;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 44964;
UPDATE creature SET position_x = 1972.27, position_y = 1366.09, position_z = 64.9518, orientation = 1.41946 WHERE guid = 44670;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 44670;
UPDATE creature SET position_x = 2964.75, position_y = 164.709, position_z = 4.1047, orientation = 1.02354 WHERE guid = 37910;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 37910;
UPDATE creature SET position_x = -3701.81, position_y = -2538.98, position_z = 69.2885, orientation = 1.37438 WHERE guid = 18594;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 18594;
UPDATE creature SET position_x = 8667.07, position_y = -7929.97, position_z = 152.975, orientation = 1.46956 WHERE guid = 55365;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 55365;
UPDATE creature SET position_x = 1763.52, position_y = 1384.63, position_z = 92.6558, orientation = 5.38302 WHERE guid = 44986;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 44986;
UPDATE creature SET position_x = 3043.64, position_y = 646.732, position_z = 59.0602, orientation = 1.53823 WHERE guid = 28140;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 28140;
UPDATE creature SET position_x = 2913.4, position_y = 379.633, position_z = 31.4818, orientation = 3.1547 WHERE guid = 44786;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 44786;
UPDATE creature SET position_x = 3051.82, position_y = -563.776, position_z = 125.885, orientation = 3.09245 WHERE guid = 44763;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 44763;
UPDATE creature SET position_x = 2150.94, position_y = -527.13, position_z = 81.6997, orientation = 4.72023 WHERE guid = 44766;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 44766;
UPDATE creature SET position_x = 1799.72, position_y = 727.12, position_z = 48.9897, orientation = 4.66786 WHERE guid = 44581;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 44581;
UPDATE creature SET position_x = 10150.9, position_y = -6005.03, position_z = 110.154, orientation = 5.80661 WHERE guid = 55165;
DELETE FROM creature_addon WHERE guid = 55165;
INSERT INTO creature_addon VALUES (55165,0,0,0,4097,16,'');
DELETE FROM creature_addon WHERE guid = 55610;
INSERT INTO creature_addon VALUES (55610,0,0,0,4097,16,'');
DELETE FROM creature_addon WHERE guid = 55616;
INSERT INTO creature_addon VALUES (55616,0,0,0,4097,16,'');
UPDATE creature SET position_x = 8761.36, position_y = -6102.72, position_z = 73.0918, orientation = 2.51814 WHERE guid = 56994;
DELETE FROM creature_addon WHERE guid = 56994;
INSERT INTO creature_addon VALUES (56994,0,0,0,4097,16,'');
UPDATE creature SET position_x = 3076.85, position_y = -557.27, position_z = 126.72, orientation = 5.52893 WHERE guid = 44741;
DELETE FROM creature_addon WHERE guid = 44741;
INSERT INTO creature_addon VALUES (44741,0,0,0,4097,16,'');
UPDATE creature SET position_x = 3080.68, position_y = -556.221, position_z = 126.725, orientation = 4.43287 WHERE guid = 44740;
DELETE FROM creature_addon WHERE guid = 44740;
INSERT INTO creature_addon VALUES (44740,0,0,0,4097,16,'');
UPDATE creature SET position_x = 2436.22, position_y = 355.827, position_z = 33.3481, orientation = 6.23866 WHERE guid = 44557;
UPDATE creature_addon SET emote = 16 WHERE guid = 44557;
UPDATE creature SET position_x = 1805.9, position_y = 729.737, position_z = 53.7585, orientation = 5.42883 WHERE guid = 44582;
DELETE FROM creature_addon WHERE guid = 44582;
INSERT INTO creature_addon VALUES (44582,0,0,0,4097,16,'');
DELETE FROM creature_addon WHERE guid = 55630;
INSERT INTO creature_addon VALUES (55630,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55592;
INSERT INTO creature_addon VALUES (55592,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55608;
INSERT INTO creature_addon VALUES (55608,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55594;
INSERT INTO creature_addon VALUES (55594,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55596;
INSERT INTO creature_addon VALUES (55596,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55634;
INSERT INTO creature_addon VALUES (55634,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55617;
INSERT INTO creature_addon VALUES (55617,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 55600;
INSERT INTO creature_addon VALUES (55600,0,0,1,4097,0,'');
UPDATE creature SET position_x = 8425.06, position_y = -7552.38, position_z = 161.539, orientation = 3.53953 WHERE guid = 55622;
DELETE FROM creature_addon WHERE guid = 55622;
INSERT INTO creature_addon VALUES (55622,0,0,1,4097,0,'');
UPDATE creature SET position_x = 8418.47, position_y = -7549.77, position_z = 161.54, orientation = 5.36514 WHERE guid = 55640;
DELETE FROM creature_addon WHERE guid = 55640;
INSERT INTO creature_addon VALUES (55640,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 40168;
UPDATE creature SET position_x = 1795.86, position_y = 1342.77, position_z = 89.0072, orientation = 3.56788 WHERE guid = 38322;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 38322;
DELETE FROM creature_addon WHERE guid = 28450;
INSERT INTO creature_addon VALUES (28450,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 28455;
INSERT INTO creature_addon VALUES (28455,0,0,1,4097,0,'');
UPDATE creature SET position_x = 2154.85, position_y = -527.711, position_z = 81.7015, orientation = 3.86502 WHERE guid = 44767;
DELETE FROM creature_addon WHERE guid = 44767;
INSERT INTO creature_addon VALUES (44767,0,0,1,4097,0,'');
UPDATE creature SET position_x = 8682.3, position_y = -7931.61, position_z = 152.953, orientation = 3.41037 WHERE guid = 55627;
UPDATE creature SET position_x = 7679.4, position_y = -5665.94, position_z = 1.01596, orientation = 2.32687 WHERE guid = 82293;
UPDATE creature SET position_x = 4756.2, position_y = -540.666, position_z = 285.017, orientation = 5.80265 WHERE guid = 39676;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 39676;
UPDATE creature SET position_x = 4824.41, position_y = -553.011, position_z = 273.942, orientation = 3.53373 WHERE guid = 40172;

DELETE FROM `creature_addon` WHERE `guid` = 40172;

INSERT INTO `creature_addon` VALUES (40172, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 4746.89, position_y = -592.018, position_z = 280.958, orientation = 0.807527 WHERE guid = 40166;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 40166;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 40686;
UPDATE creature SET position_x = 4788.14, position_y = -376.949, position_z = 351.558, orientation = 5.15689 WHERE guid = 40652;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40652;
UPDATE creature SET position_x = 5209.93, position_y = -573.914, position_z = 287.327, orientation = 2.85611 WHERE guid = 40659;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40659;
UPDATE creature SET position_x = 5205.98, position_y = -564.018, position_z = 287.327, orientation = 4.3571 WHERE guid = 40665;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40665;
UPDATE creature SET position_x = 6176.8, position_y = -1165.84, position_z = 369.681, orientation = 0.10065 WHERE guid = 22240;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 22240;
UPDATE creature SET position_x = 6577.1, position_y = -797.635, position_z = 474.973, orientation = 4.23281 WHERE guid = 40230;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40230;
UPDATE creature SET position_x = 6581.84, position_y = -804.634, position_z = 474.719, orientation = 2.64534 WHERE guid = 40196;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40196;
UPDATE creature SET position_x = 6548.26, position_y = -845.662, position_z = 474.071, orientation = 1.64701 WHERE guid = 40240;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40240;
UPDATE creature SET position_x = 6584.57, position_y = -848.883, position_z = 474.233, orientation = 3.32951 WHERE guid = 40283;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40283;
UPDATE creature SET position_x = 6752.09, position_y = -1961.9, position_z = 550.919, orientation = 2.86875 WHERE guid = 40409;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40409;
UPDATE creature SET position_x = 6749.53, position_y = -1967.05, position_z = 550.919, orientation = 1.37475 WHERE guid = 40413;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40413;
UPDATE creature SET position_x = 6839.06, position_y = -1958.33, position_z = 551, orientation = 4.35228 WHERE guid = 40414;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40414;
UPDATE creature SET position_x = 6854.05, position_y = -1888.95, position_z = 550.919, orientation = 2.99791 WHERE guid = 40464;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40464;
UPDATE creature SET position_x = 6850.8, position_y = -1881.76, position_z = 550.919, orientation = 5.1773 WHERE guid = 40472;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40472;
UPDATE creature SET position_x = 6912.21, position_y = -1816.11, position_z = 571.49, orientation = 4.02067 WHERE guid = 40610;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40610;
UPDATE creature SET position_x = 6905.4, position_y = -1819.53, position_z = 570.247, orientation = 6.12553 WHERE guid = 40613;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 40613;
DELETE FROM creature_addon WHERE guid = 40367;
INSERT INTO creature_addon VALUES (40367,0,0,0,4097,16,'');
UPDATE creature_addon SET emote = 16 WHERE guid = 39626;
UPDATE creature SET position_x = 4751.15, position_y = -549.37, position_z = 283.659, orientation = 5.87595 WHERE guid = 39675;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 39675;
UPDATE creature SET position_x = 4781.99, position_y = -525.396, position_z = 283.622, orientation = 0.0940275 WHERE guid = 39720;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 39720;
UPDATE creature SET position_x = 4785.39, position_y = -522.812, position_z = 283.798, orientation = 4.68215 WHERE guid = 39727;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 39727;
UPDATE creature SET position_x = 4752.66, position_y = -600.39, position_z = 279.724, orientation = 0.971588 WHERE guid = 40164;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 40164;
UPDATE creature SET position_x = 4730.61, position_y = -454.757, position_z = 349.957, orientation = 0.339778 WHERE guid = 40310;
DELETE FROM creature_addon WHERE guid = 40310;
INSERT INTO creature_addon VALUES (40310,0,0,1,4097,0,'');
UPDATE creature SET position_x = 5109.84, position_y = -531.574, position_z = 296.678, orientation = 1.35975 WHERE guid = 40618;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 40618;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 40589;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 39633;
UPDATE creature SET position_x = -4092.5, position_y = 439.415, position_z = 51.2046, orientation = 5.15032 WHERE guid = 50427;

DELETE FROM `creature_addon` WHERE `guid` = 50427;

INSERT INTO `creature_addon` VALUES (50427, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4096.86, position_y = 445.822, position_z = 51.2423, orientation = 4.64417 WHERE guid = 50428;

DELETE FROM `creature_addon` WHERE `guid` = 50428;

INSERT INTO `creature_addon` VALUES (50428, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4160.74, position_y = 608.77, position_z = 70.8209, orientation = 3.90066 WHERE guid = 50046;

DELETE FROM `creature_addon` WHERE `guid` = 50046;

INSERT INTO `creature_addon` VALUES (50046, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4186.05, position_y = 664.643, position_z = 70.4117, orientation = 5.51192 WHERE guid = 50606;

DELETE FROM `creature_addon` WHERE `guid` = 50606;

INSERT INTO `creature_addon` VALUES (50606, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4171.77, position_y = 656.722, position_z = 70.4604, orientation = 3.21855 WHERE guid = 50041;

DELETE FROM `creature_addon` WHERE `guid` = 50041;

INSERT INTO `creature_addon` VALUES (50041, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4956.85, position_y = 639.231, position_z = 42.7182, orientation = 1.51511 WHERE guid = 50549;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50549;
UPDATE creature SET position_x = -4951.97, position_y = 637.802, position_z = 42.6648, orientation = 0.19215 WHERE guid = 50508;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50508;
UPDATE creature SET position_x = -4853.49, position_y = 888.903, position_z = 136.099, orientation = 5.30247 WHERE guid = 50483;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50483;
UPDATE creature SET position_x = -4804.4, position_y = 873.962, position_z = 142.916, orientation = 3.40355 WHERE guid = 50561;

DELETE FROM `creature_addon` WHERE `guid` = 50561;

INSERT INTO `creature_addon` VALUES (50561, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4806.6, position_y = 915.095, position_z = 134.177, orientation = 1.90257 WHERE guid = 50485;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50485;
UPDATE creature SET position_x = -4800.54, position_y = 916.81, position_z = 133.779, orientation = 2.87646 WHERE guid = 50530;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50530;
UPDATE creature SET position_x = -4760.34, position_y = 860.316, position_z = 142.973, orientation = 3.23949 WHERE guid = 50528;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50528;
UPDATE creature SET position_x = -4753.31, position_y = 848.934, position_z = 143.292, orientation = 3.48035 WHERE guid = 50565;

DELETE FROM `creature_addon` WHERE `guid` = 50565;

INSERT INTO `creature_addon` VALUES (50565, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4981.11, position_y = 363.402, position_z = 24.2349, orientation = 5.51781 WHERE guid = 50553;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 50553;
UPDATE creature SET position_x = -4736.46, position_y = 589.182, position_z = 35.7505, orientation = 1.62921 WHERE guid = 50431;

DELETE FROM `creature_addon` WHERE `guid` = 50431;

INSERT INTO `creature_addon` VALUES (50431, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4505.08, position_y = 823.879, position_z = 63.21, orientation = 3.85913 WHERE guid = 50040;

DELETE FROM `creature_addon` WHERE `guid` = 50040;

INSERT INTO `creature_addon` VALUES (50040, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4056.51, position_y = 427.928, position_z = 51.3335, orientation = 2.90931 WHERE guid = 50384;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50384;
UPDATE creature SET position_x = -4069.11, position_y = 423.698, position_z = 51.0121, orientation = 0.127254 WHERE guid = 50462;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50462;
UPDATE creature SET position_x = -4069.7, position_y = 449.175, position_z = 51.4067, orientation = 4.71398 WHERE guid = 50429;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50429;
UPDATE creature SET position_x = -4108.6, position_y = 441.271, position_z = 51.2732, orientation = 5.42259 WHERE guid = 50426;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50426;
UPDATE creature SET position_x = -4102.82, position_y = 429.851, position_z = 51.2818, orientation = 1.13257 WHERE guid = 50425;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50425;
UPDATE creature SET position_x = -4181.93, position_y = 385.372, position_z = 61.0557, orientation = 2.3892 WHERE guid = 50473;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50473;
UPDATE creature SET position_x = -4189.61, position_y = 388.996, position_z = 61.0647, orientation = 5.92873 WHERE guid = 50447;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50447;
UPDATE creature SET position_x = -4886.06, position_y = 676.29, position_z = 42.2982, orientation = 5.6306 WHERE guid = 50532;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50532;
UPDATE creature SET position_x = -4877.46, position_y = 676.518, position_z = 42.2982, orientation = 4.03536 WHERE guid = 50495;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50495;
UPDATE creature SET position_x = -4808.97, position_y = 878.459, position_z = 143.233, orientation = 5.34436 WHERE guid = 50482;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50482;
UPDATE creature SET position_x = -4774.4, position_y = 912.237, position_z = 143.719, orientation = 4.73 WHERE guid = 50566;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50566;
UPDATE creature SET position_x = -4748.2, position_y = 859.022, position_z = 143.454, orientation = 3.73168 WHERE guid = 50490;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50490;
UPDATE creature SET position_x = -4756.16, position_y = 826.448, position_z = 140.434, orientation = 3.43846 WHERE guid = 50491;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50491;
UPDATE creature SET position_x = -4957.62, position_y = 477.998, position_z = 17.5895, orientation = 3.90621 WHERE guid = 50570;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50570;
UPDATE creature SET position_x = -4955.03, position_y = 467.033, position_z = 17.5937, orientation = 1.95144 WHERE guid = 50550;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50550;
UPDATE creature SET position_x = -4986.45, position_y = 410.131, position_z = 16.9878, orientation = 2.56579 WHERE guid = 50572;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50572;
UPDATE creature SET position_x = -5000.66, position_y = 363.798, position_z = 24.0297, orientation = 5.74819 WHERE guid = 50516;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50516;
UPDATE creature SET position_x = -4970.77, position_y = 349.406, position_z = 23.952, orientation = 2.36225 WHERE guid = 50571;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50571;
UPDATE creature SET position_x = -4876.55, position_y = 465.748, position_z = 19.008, orientation = 2.21215 WHERE guid = 50434;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50434;
UPDATE creature SET position_x = -4833.38, position_y = 537.864, position_z = 20.2332, orientation = 2.33432 WHERE guid = 50464;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50464;
UPDATE creature SET position_x = -4834.98, position_y = 543.858, position_z = 20.5098, orientation = 4.62419 WHERE guid = 50433;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50433;
UPDATE creature SET position_x = -4729.35, position_y = 582.862, position_z = 35.622, orientation = 1.84563 WHERE guid = 50430;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50430;
UPDATE creature SET position_x = -4724.87, position_y = 588.061, position_z = 35.8393, orientation = 3.13717 WHERE guid = 50389;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 50389;
UPDATE creature SET position_x = -4531.89, position_y = 846.642, position_z = 59.882, orientation = 6.26219 WHERE guid = 50027;
UPDATE creature_addon SET emote = 16 WHERE guid = 50027;
UPDATE creature SET position_x = -3743.2, position_y = 201.444, position_z = 121.772, orientation = 1.21285 WHERE guid = 50203;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50203;
UPDATE creature SET position_x = -3739.07, position_y = 213.256, position_z = 121.922, orientation = 4.3684 WHERE guid = 50180;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50180;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50195;
UPDATE creature SET position_x = -3725.53, position_y = 244.344, position_z = 121.146, orientation = 2.64752 WHERE guid = 50252;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50252;
UPDATE creature SET position_x = -3664.54, position_y = 265.484, position_z = 144.166, orientation = 5.29693 WHERE guid = 50205;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50205;
UPDATE creature SET position_x = -4189.16, position_y = 610.936, position_z = 70.6221, orientation = 5.65154 WHERE guid = 50031;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50031;
UPDATE creature SET position_x = -4226.4, position_y = 646.645, position_z = 70.5439, orientation = 2.00573 WHERE guid = 50642;
DELETE FROM creature_addon WHERE guid = 50642;
INSERT INTO creature_addon VALUES (50642,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4229.62, position_y = 651.821, position_z = 70.5841, orientation = 5.50494 WHERE guid = 50035;
DELETE FROM creature_addon WHERE guid = 50035;
INSERT INTO creature_addon VALUES (50035,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4492.87, position_y = 789.928, position_z = 65.6001, orientation = 3.75789 WHERE guid = 50051;
DELETE FROM creature_addon WHERE guid = 50051;
INSERT INTO creature_addon VALUES (50051,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 50039;
INSERT INTO creature_addon VALUES (50039,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4964.65, position_y = 1208.27, position_z = 56.6005, orientation = 2.88393 WHERE guid = 50227;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50227;
UPDATE creature SET position_x = -4970.31, position_y = 1216.22, position_z = 55.4151, orientation = 4.63973 WHERE guid = 50190;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50190;
UPDATE creature SET position_x = -5281.25, position_y = 1279.07, position_z = 50.744, orientation = 3.16152 WHERE guid = 50230;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50230;
UPDATE creature SET position_x = -5352.28, position_y = 1318.19, position_z = 35.6352, orientation = 3.89561 WHERE guid = 50222;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50222;
UPDATE creature SET position_x = -5355.33, position_y = 1307.41, position_z = 35.425, orientation = 1.74885 WHERE guid = 50297;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50297;
UPDATE creature SET position_x = -5397.29, position_y = 1276.35, position_z = 23.4647, orientation = 2.56217 WHERE guid = 50319;
DELETE FROM creature_addon WHERE guid = 50319;
INSERT INTO creature_addon VALUES (50319,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5454.13, position_y = 1220.77, position_z = 33.0337, orientation = 2.70878 WHERE guid = 50317;
DELETE FROM creature_addon WHERE guid = 50317;
INSERT INTO creature_addon VALUES (50317,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5464.67, position_y = 1219.29, position_z = 32.935, orientation = 0.736556 WHERE guid = 50318;
DELETE FROM creature_addon WHERE guid = 50318;
INSERT INTO creature_addon VALUES (50318,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5508.18, position_y = 1202.71, position_z = 18.8538, orientation = 5.68282 WHERE guid = 50240;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50240;
UPDATE creature SET position_x = -5536.18, position_y = 1305.26, position_z = 20.9684, orientation = 4.11202 WHERE guid = 50281;
DELETE FROM creature_addon WHERE guid = 50281;
INSERT INTO creature_addon VALUES (50281,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5542.55, position_y = 1291.38, position_z = 20.5775, orientation = 1.7279 WHERE guid = 50265;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50265;
UPDATE creature SET position_x = -5497.43, position_y = 1383.63, position_z = 20.4386, orientation = 3.03341 WHERE guid = 50249;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50249;
UPDATE creature SET position_x = -5767.55, position_y = 1123.12, position_z = 68.406, orientation = 1.04024 WHERE guid = 50276;
DELETE FROM creature_addon WHERE guid = 50276;
INSERT INTO creature_addon VALUES (50276,0,0,1,4097,0,'');
UPDATE creature SET position_x = -5757.51, position_y = 1129.4, position_z = 68.4748, orientation = 2.94265 WHERE guid = 50237;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 50237;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51097;
UPDATE creature SET position_x = -4416.42, position_y = 1482.33, position_z = 126.011, orientation = 5.0998 WHERE guid = 51084;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51084;
UPDATE creature SET position_x = -4423.68, position_y = 1366.14, position_z = 123.983, orientation = 0.942427 WHERE guid = 51089;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51089;
UPDATE creature SET position_x = -4408.79, position_y = 1370.33, position_z = 124.451, orientation = 1.36829 WHERE guid = 51016;
DELETE FROM creature_addon WHERE guid = 51016;
INSERT INTO creature_addon VALUES (51016,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4521.34, position_y = 1273.87, position_z = 126.309, orientation = 0.450294 WHERE guid = 51009;
DELETE FROM creature_addon WHERE guid = 51009;
INSERT INTO creature_addon VALUES (51009,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51077;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51068;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51075;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 51085;
UPDATE creature SET position_x = -5501.43, position_y = 1205.88, position_z = 18.8538, orientation = 0.834298 WHERE guid = 50266;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 50266);
DELETE FROM `smart_scripts` WHERE `entryorguid` = -50266;
INSERT INTO `smart_scripts` VALUES
(-50266, 0, 0, 0, 1, 0, 100, 0, 2000, 3000, 4000, 5000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ''),
(-50266, 0, 1, 0, 1, 0, 100, 0, 4000, 5000, 6000, 8000, 5, 54, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE creature SET position_x = -54.3924, position_y = 286.24, position_z = 66.7402, orientation = 4.86954 WHERE guid = 17313;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 17313;
UPDATE creature SET position_x = 1111.14, position_y = -724.71, position_z = 71.3815, orientation = 1.50421 WHERE guid = 16935;

DELETE FROM `creature_addon` WHERE `guid` = 16935;

INSERT INTO `creature_addon` VALUES (16935, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 1005.84, position_y = -783.698, position_z = 108.629, orientation = 4.04891 WHERE guid = 16884;

DELETE FROM `creature_addon` WHERE `guid` = 16884;

INSERT INTO `creature_addon` VALUES (16884, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 985.643, position_y = -781.942, position_z = 109.175, orientation = 5.9199 WHERE guid = 16909;

DELETE FROM `creature_addon` WHERE `guid` = 16909;

INSERT INTO `creature_addon` VALUES (16909, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 950.014, position_y = -839.599, position_z = 114.722, orientation = 3.24056 WHERE guid = 16897;

DELETE FROM `creature_addon` WHERE `guid` = 16897;

INSERT INTO `creature_addon` VALUES (16897, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -1334.74, position_y = -1262.84, position_z = 52.607, orientation = 0.677887 WHERE guid = 15865;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 15865;
UPDATE creature SET position_x = -1342.31, position_y = -1254.12, position_z = 52.6066, orientation = 0.670906 WHERE guid = 15864;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 15864;
UPDATE creature SET position_x = -379.451, position_y = 50.4244, position_z = 54.547, orientation = 4.23768 WHERE guid = 16535;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 16535;
UPDATE creature SET position_x = -1661.27, position_y = -1865.81, position_z = 79.1622, orientation = 5.0866 WHERE guid = 14548;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 14548;
UPDATE creature SET position_x = -8849.85, position_y = 668.669, position_z = 97.8207, orientation = 5.21745 WHERE guid = 18158;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 18158;
UPDATE creature SET position_x = -8803.11, position_y = 690.712, position_z = 101.989, orientation = 5.01172 WHERE guid = 20478;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20478;
UPDATE creature SET position_x = -338.526, position_y = 1.82372, position_z = 60.2856, orientation = 2.47773 WHERE guid = 16520;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 16520;
UPDATE creature SET position_x = -353.31, position_y = 20.1172, position_z = 54.6392, orientation = 3.1619 WHERE guid = 16020;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 16020;
UPDATE creature SET position_x = 746.982, position_y = -972.307, position_z = 167.587, orientation = 1.55632 WHERE guid = 17020;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 17020;
UPDATE creature SET position_x = -1313.48, position_y = -1266.83, position_z = 39.3462, orientation = 0.677186 WHERE guid = 16132;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 16132;
UPDATE creature SET position_x = 1316.46, position_y = -1212.24, position_z = 49.9031, orientation = 3.81878 WHERE guid = 15850;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 15850;
UPDATE creature SET position_x = -487.687, position_y = -1477.96, position_z = 88.2005, orientation = 2.84908 WHERE guid = 16100;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 16100;
UPDATE creature SET position_x = -1679.91, position_y = -1890.47, position_z = 79.2933, orientation = 4.3745 WHERE guid = 14538;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 14538;
UPDATE creature SET position_x = -908.655, position_y = -3946.26, position_z = 152.038, orientation = 3.14453 WHERE guid = 11950;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 11950;
UPDATE creature SET position_x = -522.943, position_y = 118.863, position_z = 59.6851, orientation = 0.0303884 WHERE guid = 15986;
DELETE FROM creature_addon WHERE guid = 15986;
INSERT INTO creature_addon VALUES (15986,0,0,1,4097,0,'');
UPDATE creature SET position_x = 1180.3, position_y = -549.715, position_z = 71.4439, orientation = 3.71031 WHERE guid = 16922;
DELETE FROM creature_addon WHERE guid = 16922;
INSERT INTO creature_addon VALUES (16922,0,0,1,4097,0,'');
UPDATE creature SET position_x = 1176.53, position_y = -552.948, position_z = 71.1011, orientation = 0.778155 WHERE guid = 16927;
DELETE FROM creature_addon WHERE guid = 16927;
INSERT INTO creature_addon VALUES (16927,0,0,1,4097,0,'');
UPDATE creature SET position_x = 993.833, position_y = -788.503, position_z = 108.768, orientation = 0.457018 WHERE guid = 16908;
DELETE FROM creature_addon WHERE guid = 16908;
INSERT INTO creature_addon VALUES (16908,0,0,1,4097,0,'');
UPDATE creature SET position_x = 937.744, position_y = -846.624, position_z = 114.643, orientation = 2.37523 WHERE guid = 16708;
DELETE FROM creature_addon WHERE guid = 16708;
INSERT INTO creature_addon VALUES (16708,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1339.48, position_y = -1229.56, position_z = 39.841, orientation = 3.67218 WHERE guid = 16092;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 16092;
UPDATE creature SET position_x = -787.45, position_y = -2082.69, position_z = 34.9712, orientation = 2.19633 WHERE guid = 12020;
DELETE FROM creature_addon WHERE guid = 12020;
INSERT INTO creature_addon VALUES (12020,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 14541;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 14540;
UPDATE creature SET position_x = -1680.31, position_y = -1825.61, position_z = 108.393, orientation = 3.1046 WHERE guid = 11650;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 11650;
UPDATE creature SET position_x = -1698.66, position_y = -3510.23, position_z = 60.3816, orientation = 0.222851 WHERE guid = 12016;
DELETE FROM creature_addon WHERE guid = 12016;
INSERT INTO creature_addon VALUES (12016,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1810.09, position_y = -3417.58, position_z = 45.4481, orientation = 1.11012 WHERE guid = 13345;
DELETE FROM creature_addon WHERE guid = 13345;
INSERT INTO creature_addon VALUES (13345,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1670.08, position_y = -3254.4, position_z = 25.7303, orientation = 3.71416 WHERE guid = 12601;
DELETE FROM creature_addon WHERE guid = 12601;
INSERT INTO creature_addon VALUES (12601,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1904.34, position_y = -3192.34, position_z = 56.5823, orientation = 5.69336 WHERE guid = 13335;
DELETE FROM creature_addon WHERE guid = 13335;
INSERT INTO creature_addon VALUES (13335,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1871.52, position_y = -3069.09, position_z = 57.8812, orientation = 4.76834 WHERE guid = 14728;
DELETE FROM creature_addon WHERE guid = 14728;
INSERT INTO creature_addon VALUES (14728,0,0,1,4097,0,'');
UPDATE creature SET position_x = -1755.66, position_y = -3142.73, position_z = 30.4596, orientation = 0.279348 WHERE guid = 13242;
DELETE FROM creature_addon WHERE guid = 13242;
INSERT INTO creature_addon VALUES (13242,0,0,1,4097,0,'');
UPDATE creature SET position_x = -2022.01, position_y = -3312.59, position_z = 54.6401, orientation = 6.05639 WHERE guid = 13385;
DELETE FROM creature_addon WHERE guid = 13385;
INSERT INTO creature_addon VALUES (13385,0,0,1,4097,0,'');
UPDATE creature SET position_x = -2018.49, position_y = -3316.2, position_z = 54.4936, orientation = 1.64769 WHERE guid = 13380;
DELETE FROM creature_addon WHERE guid = 13380;
INSERT INTO creature_addon VALUES (13380,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 11423;
INSERT INTO creature_addon VALUES (11423,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 11411;
INSERT INTO creature_addon VALUES (11411,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 11705;
INSERT INTO creature_addon VALUES (11705,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 11706;
INSERT INTO creature_addon VALUES (11706,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 13285;
INSERT INTO creature_addon VALUES (13285,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 13280;
INSERT INTO creature_addon VALUES (13280,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 12067;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 12076;
UPDATE creature SET position_x = -1961.77, position_y = -2800.12, position_z = 81.2137, orientation = 0.307272 WHERE guid = 12075;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 12075;
UPDATE creature SET position_x = 102.018, position_y = 330.687, position_z = 43.3329, orientation = 2.38421 WHERE guid = 16876;
DELETE FROM creature_addon WHERE guid = 16876;
INSERT INTO creature_addon VALUES (16876,0,0,0,4097,467,'');
UPDATE creature SET position_x = 43.1224, position_y = 390.444, position_z = 42.1293, orientation = 4.19587 WHERE guid = 16862;
DELETE FROM creature_addon WHERE guid = 16862;
INSERT INTO creature_addon VALUES (16862,0,0,0,4097,467,'');
UPDATE creature SET position_x = 85.385, position_y = 371.768, position_z = 43.4397, orientation = 5.33731 WHERE guid = 16873;
DELETE FROM creature_addon WHERE guid = 16873;
INSERT INTO creature_addon VALUES (16873,0,0,0,4097,467,'');
UPDATE creature SET position_x = 87.9066, position_y = 363.469, position_z = 48.2045, orientation = 3.82935 WHERE guid = 16815;
DELETE FROM creature_addon WHERE guid = 16815;
INSERT INTO creature_addon VALUES (16815,0,0,0,4097,467,'');
UPDATE creature SET position_x = 121.373, position_y = 487.544, position_z = 44.1493, orientation = 5.33382 WHERE guid = 16791;
DELETE FROM creature_addon WHERE guid = 16791;
INSERT INTO creature_addon VALUES (16791,0,0,0,4097,467,'');
UPDATE creature SET position_x = 168.489, position_y = 231.675, position_z = 41.2237, orientation = 0.775021 WHERE guid = 16924;
DELETE FROM creature_addon WHERE guid = 16924;
INSERT INTO creature_addon VALUES (16924,0,0,0,4097,467,'');
UPDATE creature SET position_x = -763.593, position_y = 157.701, position_z = 22.4123, orientation = 5.08241 WHERE guid = 15922;
UPDATE creature_addon SET emote = 467 WHERE guid = 15922;
UPDATE creature_addon SET emote = 467 WHERE guid = 15588;
UPDATE creature_addon SET emote = 467 WHERE guid = 15911;
UPDATE creature_addon SET emote = 467 WHERE guid = 15398;
UPDATE creature_addon SET emote = 467 WHERE guid = 15696;
UPDATE creature SET position_x = -992.263, position_y = -3841.47, position_z = 148.673, orientation = 0.614151 WHERE guid = 11945;
DELETE FROM creature_addon WHERE guid = 11945;
INSERT INTO creature_addon VALUES (11945,0,0,0,4097,467,'');
UPDATE creature SET position_x = -985.684, position_y = -3883.39, position_z = 148.653, orientation = 3.90934 WHERE guid = 13257;
DELETE FROM creature_addon WHERE guid = 13257;
INSERT INTO creature_addon VALUES (13257,0,0,0,4097,467,'');
UPDATE creature SET position_x = -948.869, position_y = -3893.87, position_z = 154.438, orientation = 5.85363 WHERE guid = 11947;
DELETE FROM creature_addon WHERE guid = 11947;
INSERT INTO creature_addon VALUES (11947,0,0,0,4097,467,'');
UPDATE creature SET position_x = -906.563, position_y = -3880.91, position_z = 139.591, orientation = 4.16066 WHERE guid = 11583;
DELETE FROM creature_addon WHERE guid = 11583;
INSERT INTO creature_addon VALUES (11583,0,0,0,4097,467,'');
UPDATE creature SET position_x = -879.673, position_y = -3909.08, position_z = 134.645, orientation = 0.593205 WHERE guid = 14529;
DELETE FROM creature_addon WHERE guid = 14529;
INSERT INTO creature_addon VALUES (14529,0,0,0,4097,467,'');
UPDATE creature SET position_x = -893.289, position_y = -3931.79, position_z = 135.272, orientation = 3.07157 WHERE guid = 11952;
DELETE FROM creature_addon WHERE guid = 11952;
INSERT INTO creature_addon VALUES (11952,0,0,0,4097,467,'');
UPDATE creature SET position_x = -871.18, position_y = -3958.26, position_z = 134.012, orientation = 5.38937 WHERE guid = 11951;
DELETE FROM creature_addon WHERE guid = 11951;
INSERT INTO creature_addon VALUES (11951,0,0,0,4097,467,'');
UPDATE creature SET position_x = -891.708, position_y = -4008.49, position_z = 145.761, orientation = 3.72433 WHERE guid = 13252;
DELETE FROM creature_addon WHERE guid = 13252;
INSERT INTO creature_addon VALUES (13252,0,0,0,4097,467,'');
UPDATE creature SET position_x = -867.716, position_y = 3971.08, position_z = 152.231, orientation = 3.71734 WHERE guid = 11954;
DELETE FROM creature_addon WHERE guid = 11954;
INSERT INTO creature_addon VALUES (11954,0,0,0,4097,467,'');
UPDATE creature SET position_x = -855.904, position_y = -3932.79, position_z = 149.359, orientation = 1.59502 WHERE guid = 11956;
DELETE FROM creature_addon WHERE guid = 11956;
INSERT INTO creature_addon VALUES (11956,0,0,0,4097,467,'');
UPDATE creature SET position_x = -798.038, position_y = -3899.56, position_z = 144.968, orientation = 5.9758 WHERE guid = 13254;
DELETE FROM creature_addon WHERE guid = 13254;
INSERT INTO creature_addon VALUES (13254,0,0,0,4097,467,'');
UPDATE creature SET position_x = -960.63, position_y = -3935.01, position_z = 145.51, orientation = 3.38888 WHERE guid = 11949;
DELETE FROM creature_addon WHERE guid = 11949;
INSERT INTO creature_addon VALUES (11949,0,0,0,4097,467,'');
UPDATE creature SET position_x = -933.494, position_y = -3954.4, position_z = 148.5, orientation = 4.22664 WHERE guid = 11948;
DELETE FROM creature_addon WHERE guid = 11948;
INSERT INTO creature_addon VALUES (11948,0,0,0,4097,467,'');
UPDATE creature SET position_x = -351.95, position_y = 90.05, position_z = 54.0404, orientation = 0.729564 WHERE guid = 16523;
UPDATE creature SET position_x = -1679.91, position_y = -1890.47, position_z = 79.2933, orientation = 4.3745 WHERE guid = 14538;
UPDATE creature SET position_x = -294.704, position_y = -3918.2, position_z = 216.893, orientation = 6.14052 WHERE guid = 93693;

DELETE FROM `creature_addon` WHERE `guid` = 93693;

INSERT INTO `creature_addon` VALUES (93693, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -266.409, position_y = -3900.58, position_z = 216.88, orientation = 2.55911 WHERE guid = 93700;

DELETE FROM `creature_addon` WHERE `guid` = 93700;

INSERT INTO `creature_addon` VALUES (93700, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -267.114, position_y = -3915.62, position_z = 216.885, orientation = 3.90778 WHERE guid = 93703;

DELETE FROM `creature_addon` WHERE `guid` = 93703;

INSERT INTO `creature_addon` VALUES (93703, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -525.779, position_y = -4189.84, position_z = 216.877, orientation = 6.27794 WHERE guid = 93039;

DELETE FROM `creature_addon` WHERE `guid` = 93039;

INSERT INTO `creature_addon` VALUES (93039, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -538.922, position_y = -4182.05, position_z = 216.891, orientation = 4.75602 WHERE guid = 93040;

DELETE FROM `creature_addon` WHERE `guid` = 93040;

INSERT INTO `creature_addon` VALUES (93040, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -400.097, position_y = -4115.26, position_z = 174.124, orientation = 1.09303 WHERE guid = 93654;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 93654;
UPDATE creature SET position_x = -476.48, position_y = -4156.75, position_z = 195.25, orientation = 1.83654 WHERE guid = 93205;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 93205;
UPDATE creature SET position_x = -382.184, position_y = -3904.09, position_z = 215.866, orientation = 3.85542 WHERE guid = 93232;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 93232;
UPDATE creature SET position_x = -369.914, position_y = -3007.65, position_z = 111.792, orientation = 1.46099 WHERE guid = 93374;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 93374;
UPDATE creature SET position_x = -493.543, position_y = -2825.59, position_z = 116.731, orientation = 5.92205 WHERE guid = 93037;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 93037;
UPDATE creature SET position_x = -252.279, position_y = -2806.95, position_z = 115.941, orientation = 3.69501 WHERE guid = 93114;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 93114;
UPDATE creature SET position_x = -504.934, position_y = -3802.32, position_z = 245.584, orientation = 5.41923 WHERE guid = 93575;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 93575;
UPDATE creature SET position_x = -598.892, position_y = -3882.58, position_z = 246.47, orientation = 5.42622 WHERE guid = 93571;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 93571;
UPDATE creature SET position_x = -354.395, position_y = -4075.06, position_z = 177.515, orientation = 2.42646 WHERE guid = 93642;
DELETE FROM creature_addon WHERE guid = 93642;
INSERT INTO creature_addon VALUES (93642,0,0,0,4097,16,'');
UPDATE creature SET position_x = 461.97, position_y = -3623.82, position_z = 119.248, orientation = 0.0263291 WHERE guid = 93633;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93633;
UPDATE creature SET position_x = 468.168, position_y = -3626.97, position_z = 118.949, orientation = 2.11025 WHERE guid = 93637;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93637;
UPDATE creature SET position_x = 327.39, position_y = -4125.15, position_z = 119.536, orientation = 2.65829 WHERE guid = 93668;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93668;
UPDATE creature SET position_x = 320.25, position_y = -4125.1, position_z = 119.496, orientation = 0.511531 WHERE guid = 93671;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93671;
UPDATE creature SET position_x = 223.999, position_y = -4311.37, position_z = 117.768, orientation = 4.78785 WHERE guid = 93660;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93660;
UPDATE creature SET position_x = 224.199, position_y = -4318.57, position_z = 117.701, orientation = 1.46984 WHERE guid = 93661;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93661;
UPDATE creature SET position_x = -239.659, position_y = -4183.75, position_z = 121.737, orientation = 1.01972 WHERE guid = 93603;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93603;
UPDATE creature SET position_x = -301.984, position_y = -4169.69, position_z = 136.438, orientation = 3.69356 WHERE guid = 93753;
DELETE FROM creature_addon WHERE guid = 93753;
INSERT INTO creature_addon VALUES (93753,0,0,1,4097,0,'');
UPDATE creature SET position_x = -349.577, position_y = -4219.07, position_z = 152.01, orientation = 1.75625 WHERE guid = 93754;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93754;
UPDATE creature SET position_x = -217.563, position_y = -4048.92, position_z = 174.25, orientation = 1.36181 WHERE guid = 93638;
DELETE FROM creature_addon WHERE guid = 93638;
INSERT INTO creature_addon VALUES (93638,0,0,1,4097,0,'');
UPDATE creature SET position_x = -374.624, position_y = -4053.24, position_z = 174.004, orientation = 4.70237 WHERE guid = 93650;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93650;
UPDATE creature SET position_x = -406.474, position_y = -4197.84, position_z = 174.073, orientation = 0.848694 WHERE guid = 93555;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93555;
UPDATE creature SET position_x = -463.887, position_y = -4192.56, position_z = 195.065, orientation = 2.17513 WHERE guid = 93678;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93678;
UPDATE creature SET position_x = -415.812, position_y = -4020.75, position_z = 195.088, orientation = 0.894056 WHERE guid = 93675;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93675;
UPDATE creature SET position_x = -414.862, position_y = -4006.68, position_z = 195.069, orientation = 6.04976 WHERE guid = 93672;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93672;
UPDATE creature SET position_x = -207.011, position_y = -3972.42, position_z = 195.451, orientation = 3.40385 WHERE guid = 93141;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93141;
UPDATE creature SET position_x = -221.514, position_y = -3978.38, position_z = 195.228, orientation = 0.880101 WHERE guid = 93727;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93727;
UPDATE creature SET position_x = -278.204, position_y = -3907.87, position_z = 215.862, orientation = 6.28143 WHERE guid = 93704;
DELETE FROM creature_addon WHERE guid = 93704;
INSERT INTO creature_addon VALUES (93704,0,0,1,4097,0,'');
UPDATE creature SET position_x = -394.689, position_y = -3944.4, position_z = 215.862, orientation = 1.90065 WHERE guid = 93599;
DELETE FROM creature_addon WHERE guid = 93599;
INSERT INTO creature_addon VALUES (93599,0,0,1,4097,0,'');
UPDATE creature SET position_x = -396.99, position_y = -3939.04, position_z = 215.862, orientation = 5.31801 WHERE guid = 93231;
DELETE FROM creature_addon WHERE guid = 93231;
INSERT INTO creature_addon VALUES (93231,0,0,1,4097,0,'');
UPDATE creature SET position_x = -432.816, position_y = 3901.77, position_z = 215.861, orientation = 1.59697 WHERE guid = 93698;
DELETE FROM creature_addon WHERE guid = 93698;
INSERT INTO creature_addon VALUES (93698,0,0,1,4097,0,'');
UPDATE creature SET position_x = -593.43, position_y = -4161.96, position_z = 238.366, orientation = 1.85877 WHERE guid = 93215;
DELETE FROM creature_addon WHERE guid = 93215;
INSERT INTO creature_addon VALUES (93215,0,0,1,4097,0,'');
UPDATE creature SET position_x = -655.81, position_y = -4095.83, position_z = 238.351, orientation = 5.59378 WHERE guid = 93184;
DELETE FROM creature_addon WHERE guid = 93184;
INSERT INTO creature_addon VALUES (93184,0,0,1,4097,0,'');
UPDATE creature SET position_x = -648.119, position_y = -4109.23, position_z = 238.397, orientation = 1.55858 WHERE guid = 93042;
DELETE FROM creature_addon WHERE guid = 93042;
INSERT INTO creature_addon VALUES (93042,0,0,1,4097,0,'');
UPDATE creature SET position_x = -649.469, position_y = -4005.21, position_z = 238.351, orientation = 3.64948 WHERE guid = 93573;
DELETE FROM creature_addon WHERE guid = 93573;
INSERT INTO creature_addon VALUES (93573,0,0,1,4097,0,'');
UPDATE creature SET position_x = -675.938, position_y = -4004.59, position_z = 238.351, orientation = 5.60425 WHERE guid = 93572;
DELETE FROM creature_addon WHERE guid = 93572;
INSERT INTO creature_addon VALUES (93572,0,0,1,4097,0,'');
UPDATE creature SET position_x = -630.165, position_y = -3946.98, position_z = 238.242, orientation = 1.89369 WHERE guid = 93586;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93586;
UPDATE creature SET position_x = -630.598, position_y = -3941.19, position_z = 237.813, orientation = 4.42441 WHERE guid = 93588;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 93588;
UPDATE creature SET position_x = -539.23, position_y = -3993.19, position_z = 215.861, orientation = 5.91492 WHERE guid = 93217;
DELETE FROM creature_addon WHERE guid = 93217;
INSERT INTO creature_addon VALUES (93217,0,0,1,4097,0,'');
UPDATE creature SET position_x = -533.196, position_y = -3994.59, position_z = 215.861, orientation = 3.05956 WHERE guid = 93219;
DELETE FROM creature_addon WHERE guid = 93219;
INSERT INTO creature_addon VALUES (93219,0,0,1,4097,0,'');
UPDATE creature SET position_x = -283.6, position_y = -3839.01, position_z = 238.352, orientation = 1.59697 WHERE guid = 93221;
DELETE FROM creature_addon WHERE guid = 93221;
INSERT INTO creature_addon VALUES (93221,0,0,1,4097,0,'');
UPDATE creature SET position_x = -252.928, position_y = -3825.31, position_z = 238.352, orientation = 4.33364 WHERE guid = 93242;
DELETE FROM creature_addon WHERE guid = 93242;
INSERT INTO creature_addon VALUES (93242,0,0,1,4097,0,'');
UPDATE creature SET position_x = -255.415, position_y = -3837.5, position_z = 238.712, orientation = 2.19387 WHERE guid = 93220;
DELETE FROM creature_addon WHERE guid = 93220;
INSERT INTO creature_addon VALUES (93220,0,0,1,4097,0,'');
UPDATE creature SET position_x = -480.608, position_y = -4078.36, position_z = 195.065, orientation = 2.78948 WHERE guid = 93674;
UPDATE creature_addon SET emote = 467 WHERE guid = 93674;
UPDATE creature SET position_x = -296.867, position_y = -4110.99, position_z = 137.771, orientation = 4.71633 WHERE guid = 93757;
UPDATE creature SET position_x = -314.368, position_y = -4232.2, position_z = 137.431, orientation = 3.16997 WHERE guid = 93758;
UPDATE creature SET position_x = -377.536, position_y = -4161.67, position_z = 174.093, orientation = 1.26407 WHERE guid = 93206;
UPDATE creature SET position_x = -378.877, position_y = -4191.74, position_z = 195.065, orientation = 1.66898 WHERE guid = 93207;
UPDATE creature SET position_x = -292.986, position_y = -3987.79, position_z = 195.136, orientation = 0.377442 WHERE guid = 93733;
UPDATE creature SET position_x = -229.005, position_y = -3961.97, position_z = 196.722, orientation = 5.79844 WHERE guid = 93729;
UPDATE creature SET position_x = -511.808, position_y = 4078.03, position_z = 215.86, orientation = 1.40499 WHERE guid = 93211;
UPDATE creature SET position_x = -591.58, position_y = -4104.85, position_z = 238.759, orientation = 1.57952 WHERE guid = 93212;
UPDATE creature SET position_x = -582.511, position_y = -4158.34, position_z = 238.351, orientation = 4.92706 WHERE guid = 93214;
UPDATE creature SET position_x = -621.925, position_y = -4137.69, position_z = 238.41, orientation = 3.0456 WHERE guid = 93187;
UPDATE creature SET position_x = -579.558, position_y = -4010.89, position_z = 238.351, orientation = 3.82402 WHERE guid = 93044;
UPDATE creature SET position_x = -593.158, position_y = -4011.31, position_z = 238.351, orientation = 3.47146 WHERE guid = 93045;
UPDATE creature SET position_x = -644.372, position_y = -4018.4, position_z = 238.35, orientation = 2.5988 WHERE guid = 93585;
UPDATE creature SET position_x = -690.663, position_y = -4021.2, position_z = 238.134, orientation = 0.350807 WHERE guid = 93577;
UPDATE creature SET position_x = -669.033, position_y = -4046.14, position_z = 238.963, orientation = 5.26915 WHERE guid = 93589;
UPDATE creature SET position_x = -2963.71, position_y = -1291.6, position_z = 72.2046, orientation = 2.64035 WHERE guid = 24920;

DELETE FROM `creature_addon` WHERE `guid` = 24920;

INSERT INTO `creature_addon` VALUES (24920, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2916.9, position_y = -1339.62, position_z = 87.9819, orientation = 1.60691 WHERE guid = 24919;

DELETE FROM `creature_addon` WHERE `guid` = 24919;

INSERT INTO `creature_addon` VALUES (24919, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2775.92, position_y = -710.889, position_z = 5.76477, orientation = 0.199744 WHERE guid = 24819;

DELETE FROM `creature_addon` WHERE `guid` = 24819;

INSERT INTO `creature_addon` VALUES (24819, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2742.86, position_y = -433.845, position_z = -4.18548, orientation = 1.45638 WHERE guid = 24828;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 24828;
UPDATE creature SET position_x = -2407.46, position_y = 229.57, position_z = 49.0053, orientation = 0.525686 WHERE guid = 24815;

DELETE FROM `creature_addon` WHERE `guid` = 24815;

INSERT INTO `creature_addon` VALUES (24815, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -2753.46, position_y = -465.292, position_z = 1.11346, orientation = 0.677694 WHERE guid = 24830;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 24830;
UPDATE creature SET position_x = -2937.63, position_y = -1274.33, position_z = 72.2054, orientation = 0.434255 WHERE guid = 24931;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24931;
UPDATE creature SET position_x = -2932.18, position_y = -1270.49, position_z = 72.2054, orientation = 3.8167 WHERE guid = 24912;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24912;
UPDATE creature SET position_x = -2937.14, position_y = -1344.46, position_z = 87.3398, orientation = 1.37304 WHERE guid = 24918;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24918;
UPDATE creature SET position_x = -2998.04, position_y = -1275.21, position_z = 72.2043, orientation = 0.126874 WHERE guid = 24879;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24879;
UPDATE creature SET position_x = -3078.53, position_y = -1212.6, position_z = 86.4532, orientation = 5.67004 WHERE guid = 24927;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24927;
UPDATE creature SET position_x = -3109.93, position_y = -1198.63, position_z = 85.3918, orientation = 5.84108 WHERE guid = 26208;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26208;
UPDATE creature SET position_x = -2864.06, position_y = -1026.59, position_z = 56.3665, orientation = 5.45318 WHERE guid = 24926;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24926;
UPDATE creature SET position_x = -2848.9, position_y = -1025.12, position_z = 55.4461, orientation = 4.97496 WHERE guid = 24925;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24925;
UPDATE creature SET position_x = -2773.72, position_y = -699.403, position_z = 5.94565, orientation = 4.90166 WHERE guid = 24847;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24847;
UPDATE creature SET position_x = -2411.07, position_y = 222.886, position_z = 48.7398, orientation = 5.22411 WHERE guid = 24814;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24814;
UPDATE creature SET position_x = -2402.84, position_y = 227.445, position_z = 48.6749, orientation = 4.07569 WHERE guid = 24806;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 24806;
UPDATE creature SET position_x = -1444.02, position_y = -1043.81, position_z = 142.425, orientation = 4.22806 WHERE guid = 26497;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26497;
UPDATE creature SET position_x = -1464.06, position_y = -1069.43, position_z = 143.201, orientation = 0.416257 WHERE guid = 26502;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26502;
UPDATE creature SET position_x = -1460.96, position_y = -1003.86, position_z = 146.816, orientation = 2.71114 WHERE guid = 26524;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26524;
UPDATE creature SET position_x = -1473.53, position_y = -980.73, position_z = 148.622, orientation = 4.03759 WHERE guid = 26495;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26495;
UPDATE creature SET position_x = -1505.58, position_y = -988.669, position_z = 147.296, orientation = 3.11257 WHERE guid = 26496;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26496;
UPDATE creature SET position_x = -1527.65, position_y = -987.497, position_z = 150.733, orientation = 5.53159 WHERE guid = 26501;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26501;
UPDATE creature SET position_x = -1937.35, position_y = -1055.28, position_z = 43.9628, orientation = 1.36239 WHERE guid = 26438;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 26438;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 26179;
UPDATE creature SET position_x = -808.921, position_y = -6.75549, position_z = -13.2731, orientation = 4.79249 WHERE guid = 26517;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 26517;
UPDATE creature SET position_x = -834.389, position_y = -29.3004, position_z = -13.3249, orientation = 6.2097 WHERE guid = 26518;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 26518;
UPDATE creature SET position_x = -154.805, position_y = -908.076, position_z = 8.40191, orientation = 0.038387 WHERE guid = 79388;

DELETE FROM `creature_addon` WHERE `guid` = 79388;

INSERT INTO `creature_addon` VALUES (79388, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -80.7326, position_y = -788.029, position_z = 38.7565, orientation = 2.19496 WHERE guid = 79327;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 79327;
UPDATE creature SET position_x = -369.302, position_y = 165.476, position_z = -21.3588, orientation = 4.44026 WHERE guid = 48745;
DELETE FROM creature_addon WHERE guid = 48745;
INSERT INTO creature_addon VALUES (48745,0,0,0,4097,16,'');
UPDATE creature SET position_x = -146.392, position_y = 903.748, position_z = 7.49405, orientation = 4.61813 WHERE guid = 79385;
DELETE FROM creature_addon WHERE guid = 79385;
INSERT INTO creature_addon VALUES (79385,0,0,1,4097,0,'');
UPDATE creature SET position_x = -60.6817, position_y = -350.172, position_z = 55.8465, orientation = 6.09732 WHERE guid = 79138;
DELETE FROM creature_addon WHERE guid = 79138;
INSERT INTO creature_addon VALUES (79138,0,0,0,4097,467,'');
UPDATE creature SET position_x = -186.993, position_y = -699.872, position_z = 8.83842, orientation = 3.1107 WHERE guid = 79262;
DELETE FROM creature_addon WHERE guid = 79262;
INSERT INTO creature_addon VALUES (79262,0,0,0,4097,467,'');
DELETE FROM creature_addon WHERE guid = 79286;
INSERT INTO creature_addon VALUES (79286,0,0,0,4097,467,'');
UPDATE creature SET position_x = -122.655, position_y = -704.315, position_z = 8.58482, orientation = 0.774917 WHERE guid = 79287;
DELETE FROM creature_addon WHERE guid = 79287;
INSERT INTO creature_addon VALUES (79287,0,0,0,4097,467,'');
UPDATE creature SET position_x = -42.9714, position_y = -691.529, position_z = 4.16845, orientation = 0.555007 WHERE guid = 79293;
DELETE FROM creature_addon WHERE guid = 79293;
INSERT INTO creature_addon VALUES (79293,0,0,0,4097,467,'');
UPDATE creature SET position_x = -35.725, position_y = -689.823, position_z = 4.91363, orientation = 2.89375 WHERE guid = 79298;
DELETE FROM creature_addon WHERE guid = 79298;
INSERT INTO creature_addon VALUES (79298,0,0,0,4097,467,'');
UPDATE creature SET position_x = -129.46, position_y = -768.761, position_z = 2.85371, orientation = 1.22172 WHERE guid = 79312;
DELETE FROM creature_addon WHERE guid = 79312;
INSERT INTO creature_addon VALUES (79312,0,0,0,4097,467,'');
UPDATE creature SET position_x = -130.686, position_y = -761.457, position_z = 4.51862, orientation = 5.55014 WHERE guid = 79303;
DELETE FROM creature_addon WHERE guid = 79303;
INSERT INTO creature_addon VALUES (79303,0,0,0,4097,467,'');
UPDATE creature SET position_x = -127.141, position_y = -821.803, position_z = 16.8149, orientation = 1.55967 WHERE guid = 79353;
DELETE FROM creature_addon WHERE guid = 79353;
INSERT INTO creature_addon VALUES (79353,0,0,0,4097,467,'');
UPDATE creature SET position_x = -121.5, position_y = -810.372, position_z = 16.9585, orientation = 5.55997 WHERE guid = 79320;
DELETE FROM creature_addon WHERE guid = 79320;
INSERT INTO creature_addon VALUES (79320,0,0,0,4097,467,'');
UPDATE creature SET position_x = -109.181, position_y = -797.775, position_z = 17.0057, orientation = 5.40288 WHERE guid = 79304;
DELETE FROM creature_addon WHERE guid = 79304;
INSERT INTO creature_addon VALUES (79304,0,0,0,4097,467,'');
UPDATE creature SET position_x = -89.0735, position_y = -789.267, position_z = 17.2247, orientation = 5.06778 WHERE guid = 79314;
DELETE FROM creature_addon WHERE guid = 79314;
INSERT INTO creature_addon VALUES (79314,0,0,0,4097,467,'');
UPDATE creature SET position_x = -52.1286, position_y = -787.256, position_z = 18.1872, orientation = 5.05033 WHERE guid = 79325;
DELETE FROM creature_addon WHERE guid = 79325;
INSERT INTO creature_addon VALUES (79325,0,0,0,4097,467,'');
UPDATE creature SET position_x = -46.7813, position_y = -851.425, position_z = 18.4285, orientation = 1.75165 WHERE guid = 79347;
DELETE FROM creature_addon WHERE guid = 79347;
INSERT INTO creature_addon VALUES (79347,0,0,0,4097,467,'');
UPDATE creature SET position_x = -81.5385, position_y = -792.904, position_z = 25.6776, orientation = 5.63675 WHERE guid = 79315;
DELETE FROM creature_addon WHERE guid = 79315;
INSERT INTO creature_addon VALUES (79315,0,0,0,4097,467,'');
UPDATE creature SET position_x = -98.795, position_y = -806.295, position_z = 30.6216, orientation = 4.41153 WHERE guid = 79319;
DELETE FROM creature_addon WHERE guid = 79319;
INSERT INTO creature_addon VALUES (79319,0,0,0,4097,467,'');
UPDATE creature SET position_x = -70.7095, position_y = -797.656, position_z = 38.931, orientation = 5.50062 WHERE guid = 79309;
DELETE FROM creature_addon WHERE guid = 79309;
INSERT INTO creature_addon VALUES (79309,0,0,0,4097,467,'');
UPDATE creature SET position_x = -206.69, position_y = -602.72, position_z = 19.3077, orientation = 3.04385 WHERE guid = 79247;
UPDATE creature SET position_x = -150.83, position_y = -917.048, position_z = 11.5426, orientation = 1.85702 WHERE guid = 79390;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43076;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43070;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43071;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43069;
UPDATE creature SET position_x = -6804.9, position_y = 1651.15, position_z = 6.36944, orientation = 0.993945 WHERE guid = 43025;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43025;
UPDATE creature SET position_x = -6753.92, position_y = 1674.85, position_z = 7.04228, orientation = 5.59463 WHERE guid = 42973;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 42973;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 42993;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 42952;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43075;
UPDATE creature SET position_x = -7872.4, position_y = 1910.9, position_z = 3.66463, orientation = 4.24088 WHERE guid = 43074;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43074;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 43072;
UPDATE creature SET position_x = -6717.08, position_y = 1674.14, position_z = 8.42831, orientation = 1.55245 WHERE guid = 42972;
UPDATE creature SET position_x = -6751.13, position_y = 1648.93, position_z = 7.57685, orientation = 5.48642 WHERE guid = 42971;
UPDATE creature SET position_x = -7895.3, position_y = 1891.9, position_z = 8.44665, orientation = 3.29011 WHERE guid = 42963;
UPDATE creature SET position_x = 1006.69, position_y = 717.892, position_z = 63.3367, orientation = 3.12488 WHERE guid = 17975;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 17975;
UPDATE creature SET position_x = 1019.64, position_y = 671.781, position_z = 63.2213, orientation = 1.54209 WHERE guid = 17962;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 17962;
UPDATE creature SET position_x = 1010.33, position_y = 662.775, position_z = 63.139, orientation = 4.6767 WHERE guid = 17949;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 17949;
UPDATE creature SET position_x = 30.4152, position_y = 798.564, position_z = 65.9653, orientation = 6.21957 WHERE guid = 17751;

DELETE FROM `creature_addon` WHERE `guid` = 17751;

INSERT INTO `creature_addon` VALUES (17751, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 34.3829, position_y = 798.814, position_z = 70.3653, orientation = 4.75524 WHERE guid = 17754;

DELETE FROM `creature_addon` WHERE `guid` = 17754;

INSERT INTO `creature_addon` VALUES (17754, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -4.63847, position_y = 1342.53, position_z = 60.1205, orientation = 3.6164 WHERE guid = 19196;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 19196;
UPDATE creature SET position_x = -414.359, position_y = 1451.94, position_z = 32.3272, orientation = 0.37462 WHERE guid = 17910;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 17910;
UPDATE creature SET position_x = -348.358, position_y = 1608.7, position_z = 16.9814, orientation = 5.10836 WHERE guid = 19222;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 19222;
UPDATE creature SET position_x = 21.2348, position_y = 771.346, position_z = 64.3981, orientation = 4.09725 WHERE guid = 17847;
DELETE FROM creature_addon WHERE guid = 17847;
INSERT INTO creature_addon VALUES (17847,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 17837;
INSERT INTO creature_addon VALUES (17837,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 17840;
INSERT INTO creature_addon VALUES (17840,0,0,1,4097,0,'');
UPDATE creature SET position_x = 18.1618, position_y = 786.781, position_z = 65.4233, orientation = 6.18466 WHERE guid = 17753;
DELETE FROM creature_addon WHERE guid = 17753;
INSERT INTO creature_addon VALUES (17753,0,0,1,4097,0,'');
UPDATE creature SET position_x = -165.336, position_y = 776.388, position_z = 70.1361, orientation = 1.43929 WHERE guid = 18122;
DELETE FROM creature_addon WHERE guid = 18122;
INSERT INTO creature_addon VALUES (18122,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 19198;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 17908;
UPDATE creature SET position_x = -433.696, position_y = 1563.03, position_z = 20.1001, orientation = 5.39499 WHERE guid = 19229;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 19229;
UPDATE creature SET position_x = -443.259, position_y = 1540.7, position_z = 19.6354, orientation = 0.0507915 WHERE guid = 18996;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 18996;
UPDATE creature SET position_x = -435.84, position_y = 1518.54, position_z = 19.5469, orientation = 0.912987 WHERE guid = 19224;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 19224;
DELETE FROM creature_addon WHERE guid = 17738;
INSERT INTO creature_addon VALUES (17738,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 17736;
INSERT INTO creature_addon VALUES (17736,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 17823;
INSERT INTO creature_addon VALUES (17823,0,0,1,4097,0,'');
UPDATE creature SET position_x = -741.652, position_y = 1538.68, position_z = 17.1266, orientation = 4.76279 WHERE guid = 17927;
DELETE FROM creature_addon WHERE guid = 17927;
INSERT INTO creature_addon VALUES (17927,0,0,1,4097,0,'');
UPDATE creature SET position_x = -349.106, position_y = 1505.29, position_z = 18.8091, orientation = 3.37554 WHERE guid = 18307;
UPDATE creature_addon SET emote = 467 WHERE guid = 18307;
UPDATE creature SET position_x = 963.788, position_y = 712.194, position_z = 60.9752, orientation = 3.21563 WHERE guid = 18153;
UPDATE creature SET position_x = 1013.55, position_y = 696.052, position_z = 60.9751, orientation = 0.00270653 WHERE guid = 19036;
UPDATE creature SET position_x = -19.0413, position_y = 1356.67, position_z = 59.9285, orientation = 1.09614 WHERE guid = 19199;
UPDATE creature SET position_x = -411.49, position_y = 1508.96, position_z = 19.7701, orientation = 4.65044 WHERE guid = 18350;
UPDATE creature SET position_x = 17.0561, position_y = -704.188, position_z = -19.13, orientation = 3.12063 WHERE guid = 29385;

DELETE FROM `creature_addon` WHERE `guid` = 29385;

INSERT INTO `creature_addon` VALUES (29385, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 165.983, position_y = -900.339, position_z = 5.66198, orientation = 0.181507 WHERE guid = 29351;

DELETE FROM `creature_addon` WHERE `guid` = 29351;

INSERT INTO `creature_addon` VALUES (29351, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 115.785, position_y = -796.026, position_z = -7.49602, orientation = 4.73682 WHERE guid = 29301;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 29301;
UPDATE creature SET position_x = 111.494, position_y = -800.271, position_z = -7.49602, orientation = 6.22733 WHERE guid = 29316;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 29316;
UPDATE creature SET position_x = 109.777, position_y = -751.067, position_z = -14.5527, orientation = 5.5257 WHERE guid = 29380;

DELETE FROM `creature_addon` WHERE `guid` = 29380;

INSERT INTO `creature_addon` VALUES (29380, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = 32.1569, position_y = -800.11, position_z = -5.86569, orientation = 0.1466 WHERE guid = 29304;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 29304;
UPDATE creature SET position_x = 146.363, position_y = -765.967, position_z = -7.45248, orientation = 1.02626 WHERE guid = 29323;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 29323;
UPDATE creature SET position_x = 2417.31, position_y = 1791.18, position_z = 393.665, orientation = 5.20015 WHERE guid = 32211;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 32211;
UPDATE creature SET position_x = 2341.34, position_y = 1836.62, position_z = 381.679, orientation = 6.05536 WHERE guid = 32209;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 32209;
UPDATE creature_addon SET emote = 16 WHERE guid = 29344;
UPDATE creature_addon SET emote = 16 WHERE guid = 29343;
UPDATE creature_addon SET emote = 16 WHERE guid = 29307;
UPDATE creature SET position_x = -3.13356, position_y = -775.341, position_z = -1.19678, orientation = 1.42257 WHERE guid = 29327;
UPDATE creature_addon SET emote = 16 WHERE guid = 29327;
UPDATE creature SET position_x = -4.01762, position_y = -770.978, position_z = -1.30724, orientation = 5.50127 WHERE guid = 29328;
UPDATE creature_addon SET emote = 16 WHERE guid = 29328;
UPDATE creature_addon SET emote = 16 WHERE guid = 29331;
UPDATE creature_addon SET emote = 16 WHERE guid = 29303;
UPDATE creature SET position_x = 113.032, position_y = -837.949, position_z = -1.97828, orientation = 4.57275 WHERE guid = 29305;
UPDATE creature_addon SET emote = 16 WHERE guid = 29305;
UPDATE creature_addon SET emote = 16 WHERE guid = 29319;
UPDATE creature SET position_x = 167.493, position_y = -895.103, position_z = 5.09558, orientation = 3.41037 WHERE guid = 29321;
UPDATE creature_addon SET emote = 16 WHERE guid = 29321;
UPDATE creature SET position_x = 160.431, position_y = -894.648, position_z = 5.01697, orientation = 5.95157 WHERE guid = 29298;
UPDATE creature_addon SET emote = 16 WHERE guid = 29298;
UPDATE creature SET position_x = 74.1623, position_y = -725.888, position_z = -20.8107, orientation = 2.51676 WHERE guid = 29335;
UPDATE creature_addon SET emote = 16 WHERE guid = 29335;
UPDATE creature_addon SET emote = 16 WHERE guid = 29318;
UPDATE creature_addon SET emote = 16 WHERE guid = 29324;
DELETE FROM creature_addon WHERE guid = 29376;
INSERT INTO creature_addon VALUES (29376,0,0,0,4097,16,'');
UPDATE creature SET position_x = 122.569, position_y = -359.826, position_z = 3.30819, orientation = 5.60949 WHERE guid = 29364;
DELETE FROM creature_addon WHERE guid = 29364;
INSERT INTO creature_addon VALUES (29364,0,0,0,4097,16,'');
UPDATE creature SET position_x = 106.555, position_y = -364.903, position_z = 3.72665, orientation = 1.56381 WHERE guid = 29387;
DELETE FROM creature_addon WHERE guid = 29387;
INSERT INTO creature_addon VALUES (29387,0,0,0,4097,16,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29317;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 29306;
UPDATE creature SET position_x = 77.4862, position_y = -464.647, position_z = 13.1073, orientation = 1.70693 WHERE guid = 29350;
DELETE FROM creature_addon WHERE guid = 29350;
INSERT INTO creature_addon VALUES (29350,0,0,1,4097,0,'');
UPDATE creature SET position_x = 73.4575, position_y = -457.252, position_z = 13.1855, orientation = 5.47335 WHERE guid = 29353;
DELETE FROM creature_addon WHERE guid = 29353;
INSERT INTO creature_addon VALUES (29353,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 29355;
INSERT INTO creature_addon VALUES (29355,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 29379;
INSERT INTO creature_addon VALUES (29379,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 29359;
INSERT INTO creature_addon VALUES (29359,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 29361;
INSERT INTO creature_addon VALUES (29361,0,0,1,4097,0,'');
UPDATE creature SET position_x = 136.496, position_y = -541.474, position_z = 3.00726, orientation = 2.3876 WHERE guid = 29371;
DELETE FROM creature_addon WHERE guid = 29371;
INSERT INTO creature_addon VALUES (29371,0,0,1,4097,0,'');
UPDATE creature SET position_x = 725.032, position_y = 343.68, position_z = 64.4025, orientation = 0.13974 WHERE guid = 32308;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 32308;
UPDATE creature_addon SET emote = 467 WHERE guid = 29602;
UPDATE creature_addon SET emote = 467 WHERE guid = 29601;
UPDATE creature_addon SET emote = 467 WHERE guid = 29630;
UPDATE creature_addon SET emote = 467 WHERE guid = 29631;
UPDATE creature SET position_x = -11252.6, position_y = 1160.49, position_z = 89.0313, orientation = 0.0619915 WHERE guid = 54600;

DELETE FROM `creature_addon` WHERE `guid` = 54600;

INSERT INTO `creature_addon` VALUES (54600, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -11322.1, position_y = 1731.85, position_z = 37.4074, orientation = 4.66967 WHERE guid = 89766;

DELETE FROM `creature_addon` WHERE `guid` = 89766;

INSERT INTO `creature_addon` VALUES (89766, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -11309.7, position_y = 1720.08, position_z = 37.4563, orientation = 3.20708 WHERE guid = 89768;

DELETE FROM `creature_addon` WHERE `guid` = 89768;

INSERT INTO `creature_addon` VALUES (89768, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -11186, position_y = 1702.41, position_z = 40.9175, orientation = 3.39558 WHERE guid = 45555;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 45555;
UPDATE creature SET position_x = -10565.5, position_y = 1992.03, position_z = -8.02915, orientation = 6.05547 WHERE guid = 44295;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 44295;
DELETE FROM creature_addon WHERE guid = 54601;
INSERT INTO creature_addon VALUES (54601,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 54599;
INSERT INTO creature_addon VALUES (54599,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 89767;
INSERT INTO creature_addon VALUES (89767,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 45553;
INSERT INTO creature_addon VALUES (45553,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 45554;
INSERT INTO creature_addon VALUES (45554,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 89588;
INSERT INTO creature_addon VALUES (89588,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 89589;
INSERT INTO creature_addon VALUES (89589,0,0,1,4097,0,'');
UPDATE creature SET position_x = -11933.8, position_y = -573.268, position_z = 16.941, orientation = 3.08294 WHERE guid = 2506;
DELETE FROM creature_addon WHERE guid = 2506;
INSERT INTO creature_addon VALUES (2506,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1586;
INSERT INTO creature_addon VALUES (1586,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1584;
INSERT INTO creature_addon VALUES (1584,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1066;
INSERT INTO creature_addon VALUES (1066,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1063;
INSERT INTO creature_addon VALUES (1063,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1064;
INSERT INTO creature_addon VALUES (1064,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1262;
INSERT INTO creature_addon VALUES (1262,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1263;
INSERT INTO creature_addon VALUES (1263,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2160;
INSERT INTO creature_addon VALUES (2160,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2531;
INSERT INTO creature_addon VALUES (2531,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2166;
INSERT INTO creature_addon VALUES (2166,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2168;
INSERT INTO creature_addon VALUES (2168,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 273;
INSERT INTO creature_addon VALUES (273,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 276;
INSERT INTO creature_addon VALUES (276,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 275;
INSERT INTO creature_addon VALUES (275,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1290;
INSERT INTO creature_addon VALUES (1290,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 1289;
INSERT INTO creature_addon VALUES (1289,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2038;
INSERT INTO creature_addon VALUES (2038,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2068;
INSERT INTO creature_addon VALUES (2068,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 2071;
INSERT INTO creature_addon VALUES (2071,0,0,1,4097,0,'');
UPDATE creature SET position_x = -14683, position_y = 511.206, position_z = 1.07825, orientation = 4.66071 WHERE guid = 1635;
DELETE FROM creature_addon WHERE guid = 1635;
INSERT INTO creature_addon VALUES (1635,0,0,1,4097,0,'');
UPDATE creature SET position_x = -10484.1, position_y = 1938.89, position_z = 7.29564, orientation = 3.12681 WHERE guid = 89857;
DELETE FROM creature_addon WHERE guid = 89857;
INSERT INTO creature_addon VALUES (89857,0,0,0,4097,467,'');
UPDATE creature SET position_x = -10457, position_y = 1953.28, position_z = 8.68041, orientation = 5.73084 WHERE guid = 44290;
DELETE FROM creature_addon WHERE guid = 44290;
INSERT INTO creature_addon VALUES (44290,0,0,0,4097,467,'');
UPDATE creature SET position_x = -10476.2, position_y = 1993.33, position_z = 9.61412, orientation = 2.21923 WHERE guid = 89850;
DELETE FROM creature_addon WHERE guid = 89850;
INSERT INTO creature_addon VALUES (89850,0,0,0,4097,467,'');
UPDATE creature SET position_x = -10560.8, position_y = 1951.87, position_z = -2.56938, orientation = 4.39042 WHERE guid = 89858;
DELETE FROM creature_addon WHERE guid = 89858;
INSERT INTO creature_addon VALUES (89858,0,0,0,4097,467,'');
UPDATE creature SET position_x = -10585.1, position_y = 1978.7, position_z = -3.71766, orientation = 3.55965 WHERE guid = 89855;
DELETE FROM creature_addon WHERE guid = 89855;
INSERT INTO creature_addon VALUES (89855,0,0,0,4097,467,'');
UPDATE creature SET position_x = -10584.8, position_y = 2003.55, position_z = -6.39223, orientation = 2.52292 WHERE guid = 89853;
DELETE FROM creature_addon WHERE guid = 89853;
INSERT INTO creature_addon VALUES (89853,0,0,0,4097,467,'');
UPDATE creature SET position_x = -10532.3, position_y = 2006.97, position_z = -8.96002, orientation = 0.550696 WHERE guid = 44231;
DELETE FROM creature_addon WHERE guid = 44231;
INSERT INTO creature_addon VALUES (44231,0,0,0,4097,467,'');
UPDATE creature SET position_x = -7863.7, position_y = -5115.75, position_z = 6.10189, orientation = 0.312493 WHERE guid = 23332;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 23332;
UPDATE creature SET position_x = -7982.34, position_y = -5129.42, position_z = 6.06346, orientation = 4.75959 WHERE guid = 23314;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 23314;
UPDATE creature SET position_x = -7188.68, position_y = -3005.63, position_z = 31.2325, orientation = 5.88658 WHERE guid = 21812;

DELETE FROM `creature_addon` WHERE `guid` = 21812;

INSERT INTO `creature_addon` VALUES (21812, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -6939.69, position_y = -3139.62, position_z = 30.6904, orientation = 4.10845 WHERE guid = 21777;

DELETE FROM `creature_addon` WHERE `guid` = 21777;

INSERT INTO `creature_addon` VALUES (21777, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -7971.91, position_y = -5451.57, position_z = 0.162684, orientation = 3.75113 WHERE guid = 23325;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 23325;
UPDATE creature SET position_x = -7905.34, position_y = -5204.88, position_z = 0.777644, orientation = 4.1592 WHERE guid = 23330;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 23330;
UPDATE creature SET position_x = -7987.89, position_y = -5137.15, position_z = 6.31491, orientation = 0.0456813 WHERE guid = 23334;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 23334;
UPDATE creature SET position_x = -796.09, position_y = 5240.13, position_z = 1.67749, orientation = 6.22066 WHERE guid = 23337;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 23337;
UPDATE creature SET position_x = -8000.31, position_y = -5375.55, position_z = 1.11969, orientation = 3.53001 WHERE guid = 23379;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 23379;
UPDATE creature SET position_x = -7715.01, position_y = -4631.52, position_z = 8.87731, orientation = 0.220522 WHERE guid = 23606;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 23606;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 23534;
UPDATE creature SET position_x = -7280.11, position_y = -4540.57, position_z = 8.87765, orientation = 4.60492 WHERE guid = 23555;
UPDATE creature_addon SET emote = 16 WHERE guid = 23555;
UPDATE creature SET position_x = -7856.01, position_y = 5109.23, position_z = 6.82288, orientation = 4.72119 WHERE guid = 23302;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23302;
UPDATE creature SET position_x = -7912.33, position_y = -5209.29, position_z = 0.638919, orientation = 0.36485 WHERE guid = 23299;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23299;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23400;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23380;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23290;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23376;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23306;
UPDATE creature SET position_x = -8048.98, position_y = -5281.4, position_z = 0.578146, orientation = 2.50376 WHERE guid = 23423;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23423;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23345;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23370;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23353;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23294;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23304;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23301;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23313;
UPDATE creature SET position_x = -7935.3, position_y = -5427.55, position_z = 7.48121, orientation = 3.62197 WHERE guid = 23393;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23393;
UPDATE creature SET position_x = -7976, position_y = -5482.73, position_z = 7.5286, orientation = 5.2905 WHERE guid = 23333;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23333;
UPDATE creature SET position_x = -7982.61, position_y = -5491.21, position_z = 7.52768, orientation = 0.550192 WHERE guid = 23388;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23388;
UPDATE creature SET position_x = -7383.01, position_y = -4405.57, position_z = 9.23847, orientation = 4.81856 WHERE guid = 23601;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23601;
UPDATE creature SET position_x = -8504.27, position_y = -3035.02, position_z = 8.88833, orientation = 6.11908 WHERE guid = 23196;
DELETE FROM creature_addon WHERE guid = 23196;
INSERT INTO creature_addon VALUES (23196,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8322.4, position_y = -3124.49, position_z = 8.64354, orientation = 2.62615 WHERE guid = 23184;
DELETE FROM creature_addon WHERE guid = 23184;
INSERT INTO creature_addon VALUES (23184,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23153;
UPDATE creature SET position_x = -8358.78, position_y = -3022.3, position_z = 8.76361, orientation = 2.62002 WHERE guid = 23154;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23154;
UPDATE creature SET position_x = -8371.07, position_y = -3022.54, position_z = 8.68303, orientation = 0.595634 WHERE guid = 23176;
DELETE FROM creature_addon WHERE guid = 23176;
INSERT INTO creature_addon VALUES (23176,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8371.09, position_y = 3013.65, position_z = 8.77612, orientation = 5.67454 WHERE guid = 23188;
DELETE FROM creature_addon WHERE guid = 23188;
INSERT INTO creature_addon VALUES (23188,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8379.4, position_y = -2980.37, position_z = 8.60073, orientation = 5.30454 WHERE guid = 23142;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23142;
UPDATE creature SET position_x = -8391.44, position_y = -2875.34, position_z = 8.62768, orientation = 3.84893 WHERE guid = 23150;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23150;
DELETE FROM creature_addon WHERE guid = 23177;
INSERT INTO creature_addon VALUES (23177,0,0,1,4097,0,'');
UPDATE creature SET position_x = -8237.39, position_y = -2987.27, position_z = 9.25953, orientation = 3.78959 WHERE guid = 23148;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 23148;
UPDATE creature SET position_x = -7229.05, position_y = -3021.63, position_z = 8.81706, orientation = 1.59308 WHERE guid = 21811;
DELETE FROM creature_addon WHERE guid = 21811;
INSERT INTO creature_addon VALUES (21811,0,0,1,4097,0,'');
UPDATE creature SET position_x = -7059.26, position_y = -2779.62, position_z = 11.1598, orientation = 0.51446 WHERE guid = 21808;
DELETE FROM creature_addon WHERE guid = 21808;
INSERT INTO creature_addon VALUES (21808,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6907.93, position_y = -3110.12, position_z = 12.4113, orientation = 3.54646 WHERE guid = 21797;
DELETE FROM creature_addon WHERE guid = 21797;
INSERT INTO creature_addon VALUES (21797,0,0,1,4097,0,'');
UPDATE creature SET position_x = -6925.93, position_y = -3119.9, position_z = 8.79564, orientation = 2.76455 WHERE guid = 21817;
DELETE FROM creature_addon WHERE guid = 21817;
INSERT INTO creature_addon VALUES (21817,0,0,1,4097,0,'');
UPDATE creature SET position_x = -7697.83, position_y = -4632.04, position_z = 9.04017, orientation = 4.43075 WHERE guid = 23605;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE entry = (SELECT id FROM creature WHERE guid = 23605);

UPDATE creature SET position_x = -8078.38, position_y = -5338.3, position_z = 7.71045, orientation = 5.33818 WHERE guid = 23372;
UPDATE creature_addon SET emote = 467 WHERE guid = 23372;
UPDATE creature SET position_x = -8084.05, position_y = -5342.24, position_z = 7.65334, orientation = 5.40101 WHERE guid = 23365;
UPDATE creature_addon SET emote = 467 WHERE guid = 23365;
UPDATE creature_addon SET emote = 467 WHERE guid = 23354;
UPDATE creature_addon SET emote = 467 WHERE guid = 23363;
UPDATE creature SET position_x = -8047.39, position_y = -5356.05, position_z = 0.570481, orientation = 3.82323 WHERE guid = 23343;
UPDATE creature_addon SET emote = 467 WHERE guid = 23343;
UPDATE creature SET position_x = -8049.54, position_y = -5365.94, position_z = 6.14308, orientation = 3.0483 WHERE guid = 23368;
UPDATE creature_addon SET emote = 467 WHERE guid = 23368;
UPDATE creature_addon SET emote = 467 WHERE guid = 23349;
UPDATE creature_addon SET emote = 467 WHERE guid = 23358;
UPDATE creature_addon SET emote = 467 WHERE guid = 23369;
UPDATE creature_addon SET emote = 467 WHERE guid = 23362;
UPDATE creature SET position_x = -8066.72, position_y = -5368.36, position_z = 0.569805, orientation = 2.44911 WHERE guid = 23352;
UPDATE creature_addon SET emote = 467 WHERE guid = 23352;
UPDATE creature SET position_x = -7224.94, position_y = -3013.71, position_z = 8.79601, orientation = 3.862 WHERE guid = 21794;
DELETE FROM creature_addon WHERE guid = 21794;
INSERT INTO creature_addon VALUES (21794,0,0,0,4097,467,'');
UPDATE creature SET position_x = -8077.37, position_y = -5251.56, position_z = 0.681158, orientation = 2.50376 WHERE guid = 23351;
UPDATE creature SET position_x = -8079.92, position_y = -5254.8, position_z = 0.680767, orientation = 2.21403 WHERE guid = 23348;
UPDATE creature SET position_x = -8078.95, position_y = -5288.14, position_z = 0.576086, orientation = 4.30494 WHERE guid = 23364;
UPDATE creature SET position_x = -8082.32, position_y = -5290.14, position_z = 1.71479, orientation = 1.86497 WHERE guid = 23371;
UPDATE creature SET position_x = -8090.08, position_y = -5309.02, position_z = 0.85184, orientation = 1.80912 WHERE guid = 23350;
UPDATE creature SET position_x = -8094.27, position_y = -5308.6, position_z = 0.730177, orientation = 1.26108 WHERE guid = 23357;
UPDATE creature SET position_x = -8023.75, position_y = -5382.22, position_z = 0.56924, orientation = 3.86581 WHERE guid = 23386;
UPDATE creature SET position_x = -8016.95, position_y = -5385.12, position_z = 0.570468, orientation = 4.46323 WHERE guid = 23397;
UPDATE creature SET position_x = -7979.08, position_y = -5377.75, position_z = 0.561514, orientation = 5.43014 WHERE guid = 23375;
UPDATE creature SET position_x = -7974.9, position_y = -5377.09, position_z = 0.570447, orientation = 0.236038 WHERE guid = 23377;
UPDATE creature SET position_x = -7378.96, position_y = -4731.5, position_z = 9.14922, orientation = 4.74997 WHERE guid = 23506;
UPDATE creature SET position_x = -7388.73, position_y = -4724.55, position_z = 9.16047, orientation = 3.5387 WHERE guid = 23507;
UPDATE creature SET position_x = -5140.13, position_y = -1949.57, position_z = 89.1881, orientation = 1.406 WHERE guid = 21121;

DELETE FROM `creature_addon` WHERE `guid` = 21121;

INSERT INTO `creature_addon` VALUES (21121, 0, 0, 3, 0, 0, '42648 0');

DELETE FROM `creature_addon` WHERE `guid` = 21123;

INSERT INTO `creature_addon` VALUES (21123, 0, 0, 3, 0, 0, '42648 0');

DELETE FROM `creature_addon` WHERE `guid` = 21117;

INSERT INTO `creature_addon` VALUES (21117, 0, 0, 3, 0, 0, '42648 0');

DELETE FROM `creature_addon` WHERE `guid` = 21124;

INSERT INTO `creature_addon` VALUES (21124, 0, 0, 3, 0, 0, '42648 0');

DELETE FROM `creature_addon` WHERE `guid` = 21588;

INSERT INTO `creature_addon` VALUES (21588, 0, 0, 3, 0, 0, '42648 0');

DELETE FROM `creature_addon` WHERE `guid` = 21590;

INSERT INTO `creature_addon` VALUES (21590, 0, 0, 3, 0, 0, '42648 0');

DELETE FROM `creature_addon` WHERE `guid` = 21589;

INSERT INTO `creature_addon` VALUES (21589, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -5079.35, position_y = -1049.71, position_z = -3.1828, orientation = 0.968743 WHERE guid = 21128;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 21128;
UPDATE creature SET position_x = -5026.56, position_y = -2243.85, position_z = -52.8783, orientation = 3.17533 WHERE guid = 21734;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 21734;
UPDATE creature SET position_x = -4745.12, position_y = -2096.21, position_z = 83.5184, orientation = 3.85925 WHERE guid = 21587;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21587;
UPDATE creature SET position_x = -4993.19, position_y = -2093.1, position_z = 83.568, orientation = 0.476086 WHERE guid = 21074;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21074;
UPDATE creature SET position_x = -4640.42, position_y = -1254.85, position_z = -52.0317, orientation = 0.996206 WHERE guid = 21007;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21007;
UPDATE creature SET position_x = -4629.12, position_y = -1221.37, position_z = -54.7297, orientation = 5.58992 WHERE guid = 21002;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21002;
UPDATE creature SET position_x = -4595.2, position_y = -1246.11, position_z = -53.8572, orientation = 2.84976 WHERE guid = 21003;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21003;
UPDATE creature SET position_x = -4596.93, position_y = -1223.3, position_z = -54.9663, orientation = 4.77381 WHERE guid = 21754;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21754;
UPDATE creature SET position_x = -4911.25, position_y = -2188.88, position_z = -51.1754, orientation = 2.34107 WHERE guid = 20986;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20986;
UPDATE creature SET position_x = -4977.6, position_y = -2212.3, position_z = -53.9937, orientation = 2.92051 WHERE guid = 21738;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21738;
UPDATE creature SET position_x = -5015.18, position_y = -2229.2, position_z = -53.2823, orientation = 4.84736 WHERE guid = 20988;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20988;
UPDATE creature SET position_x = -5017.68, position_y = -2260.62, position_z = -52.065, orientation = 1.83143 WHERE guid = 21742;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21742;
UPDATE creature SET position_x = -5046.46, position_y = -2220.88, position_z = -54.1731, orientation = 4.29583 WHERE guid = 21733;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21733;
UPDATE creature SET position_x = -5048.63, position_y = -2162.85, position_z = -52.1752, orientation = 4.59952 WHERE guid = 21735;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21735;
UPDATE creature SET position_x = -5079.59, position_y = -2212.91, position_z = -54.8988, orientation = 4.7566 WHERE guid = 21713;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21713;
UPDATE creature SET position_x = -5110.83, position_y = -2236.38, position_z = -50.6927, orientation = 5.43585 WHERE guid = 20985;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20985;
UPDATE creature SET position_x = -5092.33, position_y = -2278.66, position_z = -54.8176, orientation = 5.71511 WHERE guid = 20981;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20981;
UPDATE creature SET position_x = -5068.01, position_y = -2297.47, position_z = -53.2807, orientation = 3.35509 WHERE guid = 21719;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21719;
UPDATE creature SET position_x = -5158.2, position_y = -2295.58, position_z = -44.6779, orientation = 1.5944 WHERE guid = 20980;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20980;
UPDATE creature SET position_x = -5112.96, position_y = -2319.22, position_z = -55.3035, orientation = 5.98704 WHERE guid = 21000;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21000;
UPDATE creature SET position_x = -5049.51, position_y = -2330.83, position_z = -52.8077, orientation = 2.93271 WHERE guid = 21744;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21744;
UPDATE creature SET position_x = -5130.7, position_y = -2393.38, position_z = -53.4156, orientation = 3.80188 WHERE guid = 21709;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21709;
UPDATE creature SET position_x = -5092.85, position_y = -2389.39, position_z = -53.6205, orientation = 1.96929 WHERE guid = 20976;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20976;
UPDATE creature SET position_x = -5134.46, position_y = -2431.11, position_z = -52.4345, orientation = 2.29741 WHERE guid = 21743;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21743;
UPDATE creature SET position_x = -5164.39, position_y = -2412.75, position_z = -49.3261, orientation = 5.28891 WHERE guid = 20999;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20999;
UPDATE creature SET position_x = -5146.18, position_y = -2465.62, position_z = -47.9364, orientation = 3.14215 WHERE guid = 21748;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21748;
UPDATE creature SET position_x = -5194.7, position_y = -2462.15, position_z = -51.5098, orientation = 0.726091 WHERE guid = 21718;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21718;
UPDATE creature SET position_x = -5199.92, position_y = -2418.91, position_z = -37.9443, orientation = 3.6832 WHERE guid = 20997;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20997;
UPDATE creature SET position_x = -5179.94, position_y = -2530.19, position_z = -50.967, orientation = 0.897656 WHERE guid = 21746;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 21746;
UPDATE creature SET position_x = -5157.51, position_y = -2556.28, position_z = -50.2229, orientation = 2.99031 WHERE guid = 20991;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20991;
UPDATE creature SET position_x = -5190.09, position_y = -2580.36, position_z = -51.2486, orientation = 0.374058 WHERE guid = 20973;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 20973;
UPDATE creature SET position_x = -4750.48, position_y = -2122.93, position_z = 84.4136, orientation = 2.85185 WHERE guid = 21122;
DELETE FROM creature_addon WHERE guid = 21122;
INSERT INTO creature_addon VALUES (21122,0,0,0,4097,16,'');
UPDATE creature SET position_x = -5014.02, position_y = -2113.03, position_z = 84.0732, orientation = 3.71542 WHERE guid = 21078;
UPDATE creature_addon SET emote = 16 WHERE guid = 21078;
UPDATE creature SET position_x = -4919.32, position_y = -1834.88, position_z = 80.8205, orientation = 3.91788 WHERE guid = 21088;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21088;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21084;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21073;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21092;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21114;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21083;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21072;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21081;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21104;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21079;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 21080;
DELETE FROM creature_addon WHERE guid = 21120;
INSERT INTO creature_addon VALUES (21120,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 21118;
INSERT INTO creature_addon VALUES (21118,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 21119;
INSERT INTO creature_addon VALUES (21119,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 21125;
INSERT INTO creature_addon VALUES (21125,0,0,1,4097,0,'');
UPDATE creature SET position_x = -3324.19, position_y = -2258.55, position_z = 53.245, orientation = 2.42207 WHERE guid = 9752;

DELETE FROM `creature_addon` WHERE `guid` = 9752;

INSERT INTO `creature_addon` VALUES (9752, 0, 0, 3, 0, 0, '42648 0');
UPDATE creature SET position_x = -3414.45, position_y = -2273.14, position_z = 51.8986, orientation = 5.9988 WHERE guid = 9609;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 9609;
UPDATE creature SET position_x = -3436.57, position_y = -2440.39, position_z = 52.9444, orientation = 1.11778 WHERE guid = 9771;
UPDATE creature_addon SET bytes1 = 3, auras = '42648 0' WHERE guid = 9771;
UPDATE creature SET position_x = -3879.41, position_y = -3012.85, position_z = 8.78343, orientation = 4.43758 WHERE guid = 10719;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 10719;
UPDATE creature SET position_x = -3924.77, position_y = -3026.04, position_z = 7.89288, orientation = 6.11309 WHERE guid = 9973;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 9973;
UPDATE creature SET position_x = -3889.22, position_y = -3070.49, position_z = 11.6758, orientation = 3.16697 WHERE guid = 11150;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 11150;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 9317;
UPDATE creature SET MovementType = 1, spawndist = 5 WHERE guid = 9408;
UPDATE creature SET position_x = -4220.55, position_y = -2687.4, position_z = 41.1913, orientation = 4.87023 WHERE guid = 10335;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 10335;
UPDATE creature SET position_x = -4196.64, position_y = -2386.77, position_z = 216.814, orientation = 1.57241 WHERE guid = 10872;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 10872;
UPDATE creature SET position_x = -4778.3, position_y = -2982.01, position_z = 323.481, orientation = 1.20988 WHERE guid = 9303;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 9303;
UPDATE creature SET position_x = -4787.89, position_y = -3028.35, position_z = 301.36, orientation = 2.3618 WHERE guid = 9308;
UPDATE creature SET MovementType = 0, spawndist = 0 WHERE guid = 9308;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9751;
DELETE FROM creature_addon WHERE guid = 9737;
INSERT INTO creature_addon VALUES (9737,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9739;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9738;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9626;
DELETE FROM creature_addon WHERE guid = 11175;
INSERT INTO creature_addon VALUES (11175,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 9610;
INSERT INTO creature_addon VALUES (9610,0,0,1,4097,0,'');
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9627;
DELETE FROM creature_addon WHERE guid = 9773;
INSERT INTO creature_addon VALUES (9773,0,0,1,4097,0,'');
UPDATE creature SET position_x = -3432.52, position_y = -2424.77, position_z = 52.0663, orientation = 4.65382 WHERE guid = 9772;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9772;
UPDATE creature SET position_x = -3428.98, position_y = -2429.46, position_z = 52.0293, orientation = 2.9783 WHERE guid = 9707;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9707;
UPDATE creature SET position_x = -3649.2, position_y = -2586.43, position_z = 51.9033, orientation = 0.733805 WHERE guid = 10580;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 10580;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9774;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9785;
DELETE FROM creature_addon WHERE guid = 9790;
INSERT INTO creature_addon VALUES (9790,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10304;
INSERT INTO creature_addon VALUES (10304,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10306;
INSERT INTO creature_addon VALUES (10306,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10300;
INSERT INTO creature_addon VALUES (10300,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10530;
INSERT INTO creature_addon VALUES (10530,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10490;
INSERT INTO creature_addon VALUES (10490,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10493;
INSERT INTO creature_addon VALUES (10493,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 10717;
INSERT INTO creature_addon VALUES (10717,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 9974;
INSERT INTO creature_addon VALUES (9974,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 9966;
INSERT INTO creature_addon VALUES (9966,0,0,1,4097,0,'');
UPDATE creature SET position_x = -2926.32, position_y = -2968.08, position_z = 30.5612, orientation = 3.79877 WHERE guid = 10045;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 10045;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9746;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9749;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9712;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 9711;
DELETE FROM creature_addon WHERE guid = 9710;
INSERT INTO creature_addon VALUES (9710,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4262.26, position_y = -2357.36, position_z = 207.02, orientation = 5.29277 WHERE guid = 10013;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 10013;
UPDATE creature SET position_x = -4246.49, position_y = -2373.79, position_z = 204.344, orientation = 6.2534 WHERE guid = 11087;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 11087;
UPDATE creature SET position_x = -4240.2, position_y = -2371.25, position_z = 204.084, orientation = 4.05079 WHERE guid = 11089;
UPDATE creature_addon SET bytes1 = 1 WHERE guid = 11089;
UPDATE creature SET position_x = -4229.1, position_y = -2350.14, position_z = 204.313, orientation = 3.72267 WHERE guid = 11088;
DELETE FROM creature_addon WHERE guid = 11088;
INSERT INTO creature_addon VALUES (11088,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4227.11, position_y = -2357.99, position_z = 204.315, orientation = 3.45738 WHERE guid = 11085;
DELETE FROM creature_addon WHERE guid = 11085;
INSERT INTO creature_addon VALUES (11085,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4942.61, position_y = -4073.6, position_z = 300.533, orientation = 3.04596 WHERE guid = 9213;
DELETE FROM creature_addon WHERE guid = 9213;
INSERT INTO creature_addon VALUES (9213,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4949.38, position_y = -4073.3, position_z = 299.463, orientation = 0.138239 WHERE guid = 9212;
DELETE FROM creature_addon WHERE guid = 9212;
INSERT INTO creature_addon VALUES (9212,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4939.73, position_y = -4031.63, position_z = 300.98, orientation = 2.17678 WHERE guid = 9217;
DELETE FROM creature_addon WHERE guid = 9217;
INSERT INTO creature_addon VALUES (9217,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 9368;
INSERT INTO creature_addon VALUES (9368,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 9363;
INSERT INTO creature_addon VALUES (9363,0,0,1,4097,0,'');
DELETE FROM creature_addon WHERE guid = 9366;
INSERT INTO creature_addon VALUES (9366,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4835.35, position_y = -3881.85, position_z = 301.507, orientation = 0.623438 WHERE guid = 9369;
DELETE FROM creature_addon WHERE guid = 9369;
INSERT INTO creature_addon VALUES (9369,0,0,1,4097,0,'');
UPDATE creature SET position_x = -4855.39, position_y = -2972.69, position_z = 321.227, orientation = 2.18029 WHERE guid = 9311;
UPDATE creature_addon SET emote = 467 WHERE guid = 9311;
UPDATE creature SET position_x = -4897.14, position_y = -2988.83, position_z = 317.183, orientation = 4.59233 WHERE guid = 9327;
UPDATE creature_addon SET emote = 467 WHERE guid = 9327;
UPDATE creature SET position_x = -4923.02, position_y = -2958.51, position_z = 317.518, orientation = 1.87311 WHERE guid = 9321;
UPDATE creature_addon SET emote = 467 WHERE guid = 9321;
UPDATE creature SET position_x = -4927.55, position_y = -2976.02, position_z = 317.322, orientation = 4.95885 WHERE guid = 9323;
UPDATE creature_addon SET emote = 467 WHERE guid = 9323;
UPDATE creature SET position_x = -4937.06, position_y = -2965.37, position_z = 317.636, orientation = 3.60796 WHERE guid = 9326;
UPDATE creature_addon SET emote = 467 WHERE guid = 9326;
UPDATE creature SET position_x = -4920.34, position_y = -2998.56, position_z = 320.779, orientation = 4.44921 WHERE guid = 9325;
UPDATE creature_addon SET emote = 467 WHERE guid = 9325;
UPDATE creature SET position_x = -4969.77, position_y = -2994.4, position_z = 317.298, orientation = 1.55196 WHERE guid = 9150;
UPDATE creature_addon SET emote = 467 WHERE guid = 9150;
UPDATE creature SET position_x = -4991.38, position_y = -2990.13, position_z = 315.453, orientation = 3.54862 WHERE guid = 9149;
UPDATE creature_addon SET emote = 467 WHERE guid = 9149;
UPDATE creature SET position_x = -3309.73, position_y = -2265.33, position_z = 51.8987, orientation = 5.65216 WHERE guid = 9721;
UPDATE creature SET position_x = -3442.47, position_y = -2347.44, position_z = 51.921, orientation = 4.42344 WHERE guid = 10544;
UPDATE creature SET position_x = -3448.29, position_y = -2352.86, position_z = 51.8988, orientation = 6.17574 WHERE guid = 10542;
UPDATE creature SET position_x = -3453.68, position_y = -2353.83, position_z = 51.8988, orientation = 4.59447 WHERE guid = 9884;
UPDATE creature SET position_x = -3897.07, position_y = -3034.47, position_z = 11.0337, orientation = 2.77951 WHERE guid = 10720;
UPDATE creature SET position_x = -2880.3, position_y = -2946.31, position_z = 34.4049, orientation = 1.37974 WHERE guid = 9750;
UPDATE creature SET position_x = -2824.74, position_y = -2861.73, position_z = 32.7783, orientation = 4.3119 WHERE guid = 10002;
UPDATE creature SET position_x = -4245.68, position_y = -2329.21, position_z = 206.695, orientation = 5.45055 WHERE guid = 10014;
UPDATE creature SET position_x = -4237.98, position_y = -2382.09, position_z = 204.546, orientation = 1.16402 WHERE guid = 11084;
UPDATE creature SET position_x = -3187.93, position_y = -2386.48, position_z = 209.707, orientation = 5.00373 WHERE guid = 10029;
UPDATE `gameobject` SET `spawntimesecs` = 7200 WHERE `id` IN (153451, 2855, 2857, 4149, 153453, 106318, 2843, 181804);DELETE FROM `smart_scripts` WHERE `entryorguid` = 27331;
INSERT INTO `smart_scripts` VALUES 
(27331,0,0,1,8,0,100,0,48345,0,110000,110000,33,27331,0,0,0,0,0,7,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Quest Credit \'Bombard the Ballistae\''),
(27331,0,1,2,61,0,100,0,0,0,0,0,11,48351,2,0,0,0,0,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast  Bombard the Ballistae: Ballista Bow'),
(27331,0,2,3,61,0,100,0,0,0,0,0,11,48352,2,0,0,0,0,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Ballista Frame'),
(27331,0,3,4,61,0,100,0,0,0,0,0,11,48353,2,0,0,0,0,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Ballista Missile'),
(27331,0,4,5,61,0,100,0,0,0,0,0,11,48354,2,0,0,0,0,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Ballista Wheel'),
(27331,0,5,6,61,0,100,0,0,0,0,0,11,48347,2,0,0,0,0,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: FX Master'),
(27331,0,6,7,61,0,100,0,0,0,0,0,11,52687,2,0,0,0,0,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Cast Bombard the Ballistae: Knockback'),
(27331,0,7,8,61,0,100,0,0,0,0,0,44,2,0,0,0,0,0,20,188673,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Change Phasemask to 2'),
(27331,0,8,0,61,0,100,0,0,0,0,0,67,1,110000,110000,0,0,100,1,0,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Spellhit \'Siegesmith Bombs\' - Create Timed Event'),
(27331,0,9,0,59,0,100,0,1,0,0,0,44,1,0,0,0,0,0,20,188673,0,0,0,0,0,0,'Bombard the Ballistae Kill Credit Bunny - On Timed Event - Change phasemask to 1');

UPDATE `creature` SET `phaseMask` = 3 WHERE `id` = 27331;-- [NPC]Reanimated Crusader - Hammer of Injustice should not bypass cooldown
-- [NPC]Rampaging Ghoul - correct movement speed
-- [NPC]Pustulent Horror - add spells
-- [NPC] Reanimated Abomination from "Thats Abominable" quest should despawn after exploding
-- [Spell] Burst at the Seams - should knockback players
UPDATE `smart_scripts` SET `event_param3` = 65000, `event_param4` = 70000 WHERE `entryorguid` = 30202 AND `id` = 3;
UPDATE `creature_template` SET `speed_run` = 2 WHERE `entry` = 32178;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 31139 AND `id` IN (2,3);
INSERT INTO `smart_scripts` VALUES
(31139, 0, 2, 0, 0, 0, 100, 0, 3000, 5000, 3000, 7000, 11, 28405, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Pustulent Horror - IC - Cast Knock Back'),
(31139, 0, 3, 0, 0, 0, 100, 0, 4000, 6000, 5000, 9000, 11, 63546, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Pustulent Horror - IC - Cast Stomp');
UPDATE `smart_scripts` SET `target_type` = 7 WHERE `entryorguid` = 31692 AND `id` = 4;
UPDATE `smart_scripts` SET `link` = 7 WHERE `entryorguid` = 31692 AND `id` = 6;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 31692 AND `id` = 7;
INSERT INTO `smart_scripts` VALUES 
(31692, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Reanimated Abomination - Linked - Despawn self');

DELETE FROM `conditions` WHERE `SourceEntry` = 59576 AND `ElseGroup` = 3;
INSERT INTO `conditions` VALUES
(13, 2, 59576, 0, 3, 31, 0, 4, 0, 0, 0, 0, 0, '', '');
UPDATE `creature` SET `position_x` = 1378.12, `position_y` = 1976.69, `position_z` = 13.9847 WHERE `guid` = 18158;
UPDATE `creature` SET `position_x` = 120.852, `position_y` = -1992.2, `position_z` = 93.781 WHERE `guid` = 20478;
-- DB/Skinning: Correct number of borean leather dropped by some creatures
-- DB/NPC: Dying Soldier/Dying Berserker - should not follow players in combat
-- DB/NPC: Dying Soldier/Dying Berserker  - correct amount of hp
-- DB/Quest: The Will of the Naaru - add script for Rhydian to summon portal
UPDATE `skinning_loot_template` SET `maxcount` = 3 WHERE `entry` IN (70215,70203,70216);
UPDATE `creature_template_addon` SET `auras` = '65985' WHERE `entry` IN (31304, 31273);
UPDATE `creature` SET `curhealth` = 37 WHERE `id` IN (31304, 31273);
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = '' WHERE `entry` IN (31044, 30656);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (31044,30656, 3065600);
INSERT INTO `smart_scripts` VALUES
(31044, 0, 0, 0, 19, 0, 100, 0, 13081, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 124114, 30656, 0, 0, 0, 0, 0, "Tirion on quest set data on Rhydian"),
(30656, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 80, 3065600, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rhydian on data set - run script"),
(3065600, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 46, 15, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  "Rhydian on script move forward"),
(3065600, 9, 1, 0, 0, 0, 100, 0, 3400, 3400, 0, 0, 11, 57676, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rhydian on script cast portal"),
(3065600, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rhydian on script say line 0"),
(3065600, 9, 3, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6409.120117, 422.381989, 511.347992, 0.628319, "Rhydian on script return to home pos"),
(3065600, 9, 4, 0, 0, 0, 100, 0, 2600, 2600, 0, 0, 66, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.628319, "Rhydian on script set orientation");
DELETE FROM `creature_text` WHERE `entry`=30656;
INSERT INTO `creature_text` VALUES
(30656, 0, 0, "Hail. I could not help but overhear your conversation. Please allow me to lend some assistance.", 12, 0, 100, 0, 0, 0, 0, "Rhydian");

DELETE FROM `trinity_string` WHERE `entry` = 11038;
INSERT INTO `trinity_string`(`entry`, `content_default`) VALUES (11038, 'In order to change your current xp multiplier please use command .xp 1-%u');UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 412 AND `item` =  3360;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1688 AND `item` =  3261;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2177 AND `item` =  6506;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2202 AND `item` =  4690;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2234 AND `item` =  6517;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2478 AND `item` =  5319;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3056 AND `item` =  4854;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3113 AND `item` =  7350;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3117 AND `item` =  2964;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3118 AND `item` =  3213;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3131 AND `item` =  2963;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3274 AND `item` =  2975;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3528 AND `item` =  17187;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 4316 AND `item` =  3643;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 10160 AND `item` =  4690;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 14428 AND `item` =  18611;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 14431 AND `item` =  18612;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1531 AND `item` =  3322;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1533 AND `item` =  3323;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 471 AND `item` =  6148;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 79 AND `item` =  6147;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1866 AND `item` = 1438;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1936 AND `item` = 4261;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2001 AND `item` = 9752;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2018 AND `item` = 7350;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2177 AND `item` = 7351;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2202 AND `item` = 4684;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 2234 AND `item` = 6515;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3197 AND `item` = 9744;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3207 AND `item` = 2960;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3268 AND `item` = 7350;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3379 AND `item` = 7351;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3632 AND `item` = 6549;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 4263 AND `item` = 3213;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 6231 AND `item` = 9444;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 10940 AND `item` = 3419;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 3267 AND `item` = 6517;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1137 AND `item` = 3008;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1132 AND `item` = 3224;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 1260 AND `item` = 2546;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 99 AND `item` = 6201;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 3 WHERE `entry` = 61 AND `item` = 6202;
UPDATE creature SET MovementType = 0 WHERE guid IN (13891,13883);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132667;
UPDATE `creature_addon` SET `path_id` = 1326670 WHERE `guid` = 132667;
UPDATE `creature` SET `position_x` = 3070.668457, `position_y` = 5194.461426, `position_z` = 66.927841 WHERE `guid` = 132667;
DELETE FROM `waypoint_data` WHERE `id` = 1326670;
INSERT INTO `waypoint_data` VALUES
(1326670, 1, 3070.668457, 5194.461426, 66.927841, 0, 0, 1, 0, 100, 0),
(1326670, 2, 3117.395264, 5225.469238, 64.121391, 0, 1000, 1, 0, 100, 0),
(1326670, 3, 3163.513672, 5203.871582, 61.459068, 0, 0, 1, 0, 100, 0),
(1326670, 4, 3210.350342, 5239.753418, 48.873417, 0, 0, 1, 0, 100, 0),
(1326670, 5, 3226.071533, 5243.932617, 46.903416, 0, 0, 1, 0, 100, 0),
(1326670, 6, 3243.238281, 5274.019531, 42.793327, 0, 0, 1, 0, 100, 0),
(1326670, 7, 3229.214600, 5326.244141, 45.194752, 0, 0, 1, 0, 100, 0),
(1326670, 8, 3161.405762, 5356.872070, 58.279976, 0, 8000, 1, 0, 100, 0),
(1326670, 9, 3229.214600, 5326.244141, 45.194752, 0, 0, 1, 0, 100, 0),
(1326670, 10, 3243.238281, 5274.019531, 42.793327, 0, 0, 1, 0, 100, 0),
(1326670, 11, 3226.071533, 5243.932617, 46.903416, 0, 2000, 1, 0, 100, 0),
(1326670, 12, 3210.350342, 5239.753418, 48.873417, 0, 0, 1, 0, 100, 0),
(1326670, 13, 3163.513672, 5203.871582, 61.459068, 0, 0, 1, 0, 100, 0),
(1326670, 14, 3117.395264, 5225.469238, 64.121391, 0, 0, 1, 0, 100, 0),
(1326670, 15, 3070.668457, 5194.461426, 66.927841, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132667;
INSERT INTO `creature_formations` VALUES
(132667,132667, 0, 0, 515, 0, 0),
(132667,133077,5,270,515, 0, 0),
(132667,133059,5,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132658;
UPDATE `creature_addon` SET `path_id` = 1326580 WHERE `guid` = 132658;
DELETE FROM `waypoint_data` WHERE `id` = 1326580;
INSERT INTO `waypoint_data` VALUES
(1326580, 1, 3141.724, 5223.142, 61.55839, 0, 0, 1, 0, 100, 0),
(1326580, 2, 3134.156, 5249.918, 59.50746, 0, 0, 1, 0, 100, 0),
(1326580, 3, 3150.428, 5270.516, 57.02120, 0, 0, 1, 0, 100, 0),
(1326580, 4, 3153.455, 5306.382, 52.14919, 0, 0, 1, 0, 100, 0),
(1326580, 5, 3201.838, 5315.559, 47.25852, 0, 0, 1, 0, 100, 0),
(1326580, 6, 3153.455, 5306.382, 52.14919, 0, 0, 1, 0, 100, 0),
(1326580, 7, 3150.428, 5270.516, 57.02120, 0, 0, 1, 0, 100, 0),
(1326580, 8, 3134.156, 5249.918, 59.50746, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132658;
INSERT INTO `creature_formations` VALUES
(132658,132658, 0, 0, 515, 0, 0),
(132658,133061,6,270,515, 0, 0),
(132658,133052,6,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132656;
UPDATE `creature_addon` SET `path_id` = 1326560 WHERE `guid` = 132656;
DELETE FROM `waypoint_data` WHERE `id` = 1326560;
INSERT INTO `waypoint_data` VALUES
(1326560, 1, 3155.984, 5340.942, 56.07048, 0, 0, 1, 0, 100, 0),
(1326560, 2, 3142.782, 5359.769, 59.15238, 0, 0, 1, 0, 100, 0),
(1326560, 3, 3130.513, 5368.055, 59.20333, 0, 0, 1, 0, 100, 0),
(1326560, 4, 3115.192, 5374.140, 59.23418, 0, 0, 1, 0, 100, 0),
(1326560, 5, 3085.786, 5383.560, 60.26452, 0, 0, 1, 0, 100, 0),
(1326560, 6, 3056.305, 5384.749, 62.75426, 0, 0, 1, 0, 100, 0),
(1326560, 7, 3033.100, 5392.823, 64.76941, 0, 0, 1, 0, 100, 0),
(1326560, 8, 3018.143, 5421.970, 59.80108, 0, 0, 1, 0, 100, 0),
(1326560, 9, 3038.264, 5436.198, 60.53225, 0, 0, 1, 0, 100, 0),
(1326560, 10, 3064.302, 5439.519, 61.37813, 0, 0, 1, 0, 100, 0),
(1326560, 11, 3093.771, 5436.637, 56.72150, 0, 0, 1, 0, 100, 0),
(1326560, 12, 3120.410, 5445.031, 52.55723, 0, 0, 1, 0, 100, 0),
(1326560, 13, 3145.624, 5437.730, 55.47248, 0, 0, 1, 0, 100, 0),
(1326560, 14, 3171.409, 5417.523, 57.32546, 0, 0, 1, 0, 100, 0),
(1326560, 15, 3191.081, 5400.143, 56.18970, 0, 0, 1, 0, 100, 0),
(1326560, 16, 3185.127, 5374.577, 53.50888, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132656;
INSERT INTO `creature_formations` VALUES
(132656,132656, 0, 0, 515, 0, 0),
(132656,133062,5,270,515, 0, 0),
(132656,133079,5,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132669;
UPDATE `creature_addon` SET `path_id` = 1326690 WHERE `guid` = 132669;
DELETE FROM `waypoint_data` WHERE `id` = 1326690;
INSERT INTO `waypoint_data` VALUES
(1326690, 1, 3115.506, 5343.338, 56.12812, 0, 0, 1, 0, 100, 0),
(1326690, 2, 3088.202, 5325.820, 58.87288, 0, 0, 1, 0, 100, 0),
(1326690, 3, 3092.121, 5316.901, 59.31512, 0, 0, 1, 0, 100, 0),
(1326690, 4, 3106.736, 5297.232, 58.68513, 0, 0, 1, 0, 100, 0),
(1326690, 5, 3101.197, 5264.944, 59.70798, 0, 0, 1, 0, 100, 0),
(1326690, 6, 3106.736, 5297.232, 58.68513, 0, 0, 1, 0, 100, 0),
(1326690, 7, 3092.121, 5316.901, 59.31512, 0, 0, 1, 0, 100, 0),
(1326690, 8, 3088.202, 5325.820, 58.87288, 0, 0, 1, 0, 100, 0),
(1326690, 9, 3115.506, 5343.338, 56.12812, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132669;
INSERT INTO `creature_formations` VALUES
(132669,132669, 0, 0, 515, 0, 0),
(132669,133068,5,270,515, 0, 0),
(132669,133070,5,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132660;
UPDATE `creature_addon` SET `path_id` = 1326600 WHERE `guid` = 132660;
DELETE FROM `waypoint_data` WHERE `id` = 1326600;
INSERT INTO `waypoint_data` VALUES
(1326600, 1, 3084.805, 5296.040, 59.82234, 0, 0, 1, 0, 100, 0),
(1326600, 2, 3078.038, 5275.828, 59.00567, 0, 0, 1, 0, 100, 0),
(1326600, 3, 3076.892, 5261.172, 60.56978, 0, 0, 1, 0, 100, 0),
(1326600, 4, 3084.753, 5248.626, 61.92756, 0, 0, 1, 0, 100, 0),
(1326600, 5, 3088.863, 5224.569, 63.84053, 0, 0, 1, 0, 100, 0),
(1326600, 6, 3066.866, 5204.818, 65.68366, 0, 0, 1, 0, 100, 0),
(1326600, 7, 3088.863, 5224.569, 63.84053, 0, 0, 1, 0, 100, 0),
(1326600, 8, 3084.753, 5248.626, 61.92756, 0, 0, 1, 0, 100, 0),
(1326600, 9, 3076.892, 5261.172, 60.56978, 0, 0, 1, 0, 100, 0),
(1326600, 10, 3078.038, 5275.828, 59.00567, 0, 0, 1, 0, 100, 0),
(1326600, 11, 3084.805, 5296.040, 59.82234, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132660;
INSERT INTO `creature_formations` VALUES
(132660,132660, 0, 0, 515, 0, 0),
(132660,133069,6,270,515, 0, 0),
(132660,133087,6,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132663;
UPDATE `creature_addon` SET `path_id` = 1326630 WHERE `guid` = 132663;
DELETE FROM `waypoint_data` WHERE `id` = 1326630;
INSERT INTO `waypoint_data` VALUES
(1326630, 1, 3009.818, 5218.888, 63.96097, 0, 0, 1, 0, 100, 0),
(1326630, 2, 3005.054, 5234.823, 63.54463, 0, 0, 1, 0, 100, 0),
(1326630, 3, 2990.613, 5241.074, 59.59129, 0, 0, 1, 0, 100, 0),
(1326630, 4, 2975.220, 5228.281, 63.77478, 0, 0, 1, 0, 100, 0),
(1326630, 5, 2985.239, 5218.604, 64.67577, 0, 0, 1, 0, 100, 0),
(1326630, 6, 2994.729, 5214.046, 62.03285, 0, 0, 1, 0, 100, 0),
(1326630, 7, 3084.805, 5296.040, 59.82234, 0, 0, 1, 0, 100, 0),
(1326630, 8, 3078.038, 5275.828, 59.00567, 0, 0, 1, 0, 100, 0),
(1326630, 9, 3076.892, 5261.172, 60.56978, 0, 0, 1, 0, 100, 0),
(1326630, 10, 3084.753, 5248.626, 61.92756, 0, 0, 1, 0, 100, 0),
(1326630, 11, 3088.863, 5224.569, 63.84053, 0, 0, 1, 0, 100, 0),
(1326630, 12, 3066.866, 5204.818, 65.68366, 0, 0, 1, 0, 100, 0),
(1326630, 13, 3003.693, 5203.208, 60.32031, 0, 0, 1, 0, 100, 0),
(1326630, 14, 3006.951, 5199.074, 58.58787, 0, 0, 1, 0, 100, 0),
(1326630, 15, 3009.132, 5196.373, 55.91871, 0, 0, 1, 0, 100, 0),
(1326630, 16, 3009.895, 5191.224, 52.18281, 0, 0, 1, 0, 100, 0),
(1326630, 17, 3011.845, 5183.586, 48.79534, 0, 0, 1, 0, 100, 0),
(1326630, 18, 3015.048, 5176.491, 45.69197, 0, 0, 1, 0, 100, 0),
(1326630, 19, 3031.639, 5165.105, 43.96414, 0, 0, 1, 0, 100, 0),
(1326630, 20, 3028.626, 5151.572, 44.19624, 0, 0, 1, 0, 100, 0),
(1326630, 21, 3025.031, 5143.610, 41.98796, 0, 0, 1, 0, 100, 0),
(1326630, 22, 3012.568, 5130.162, 39.90676, 0, 0, 1, 0, 100, 0),
(1326630, 23, 2996.858, 5128.554, 39.97041, 0, 0, 1, 0, 100, 0),
(1326630, 24, 3012.568, 5130.162, 39.90676, 0, 0, 1, 0, 100, 0),
(1326630, 25, 3025.031, 5143.610, 41.98796, 0, 0, 1, 0, 100, 0),
(1326630, 26, 3028.626, 5151.572, 44.19624, 0, 0, 1, 0, 100, 0),
(1326630, 27, 3031.639, 5165.105, 43.96414, 0, 0, 1, 0, 100, 0),
(1326630, 28, 3015.048, 5176.491, 45.69197, 0, 0, 1, 0, 100, 0),
(1326630, 29, 3011.845, 5183.586, 48.79534, 0, 0, 1, 0, 100, 0),
(1326630, 30, 3009.895, 5191.224, 52.18281, 0, 0, 1, 0, 100, 0),
(1326630, 31, 3009.132, 5196.373, 55.91871, 0, 0, 1, 0, 100, 0),
(1326630, 32, 3006.951, 5199.074, 58.58787, 0, 0, 1, 0, 100, 0),
(1326630, 33, 3003.693, 5203.208, 60.32031, 0, 0, 1, 0, 100, 0),
(1326630, 34, 3066.866, 5204.818, 65.68366, 0, 0, 1, 0, 100, 0),
(1326630, 35, 3088.863, 5224.569, 63.84053, 0, 0, 1, 0, 100, 0),
(1326630, 36, 3084.753, 5248.626, 61.92756, 0, 0, 1, 0, 100, 0),
(1326630, 37, 3076.892, 5261.172, 60.56978, 0, 0, 1, 0, 100, 0),
(1326630, 38, 3078.038, 5275.828, 59.00567, 0, 0, 1, 0, 100, 0),
(1326630, 39, 3084.805, 5296.040, 59.82234, 0, 0, 1, 0, 100, 0),
(1326630, 40, 2994.729, 5214.046, 62.03285, 0, 0, 1, 0, 100, 0),
(1326630, 41, 2985.239, 5218.604, 64.67577, 0, 0, 1, 0, 100, 0),
(1326630, 42, 2975.220, 5228.281, 63.77478, 0, 0, 1, 0, 100, 0),
(1326630, 43, 2990.613, 5241.074, 59.59129, 0, 0, 1, 0, 100, 0),
(1326630, 44, 3005.054, 5234.823, 63.54463, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132663;
INSERT INTO `creature_formations` VALUES
(132663,132663, 0, 0, 515, 0, 0),
(132663,133055,5,270,515, 0, 0),
(132663,133054,5,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132671;
UPDATE `creature_addon` SET `path_id` = 1326710 WHERE `guid` = 132671;
DELETE FROM `waypoint_data` WHERE `id` = 1326710;
INSERT INTO `waypoint_data` VALUES
(1326710, 1, 3084.805, 5296.040, 59.82234, 0, 0, 1, 0, 100, 0),
(1326710, 2, 3078.038, 5275.828, 59.00567, 0, 0, 1, 0, 100, 0),
(1326710, 3, 3076.892, 5261.172, 60.56978, 0, 0, 1, 0, 100, 0),
(1326710, 4, 3084.753, 5248.626, 61.92756, 0, 0, 1, 0, 100, 0),
(1326710, 5, 3088.863, 5224.569, 63.84053, 0, 0, 1, 0, 100, 0),
(1326710, 6, 3066.866, 5204.818, 65.68366, 0, 0, 1, 0, 100, 0),
(1326710, 7, 3003.693, 5203.208, 60.32031, 0, 0, 1, 0, 100, 0),
(1326710, 8, 3006.951, 5199.074, 58.58787, 0, 0, 1, 0, 100, 0),
(1326710, 9, 3009.132, 5196.373, 55.91871, 0, 0, 1, 0, 100, 0),
(1326710, 10, 3009.895, 5191.224, 52.18281, 0, 0, 1, 0, 100, 0),
(1326710, 11, 3011.845, 5183.586, 48.79534, 0, 0, 1, 0, 100, 0),
(1326710, 12, 3015.048, 5176.491, 45.69197, 0, 0, 1, 0, 100, 0),
(1326710, 13, 3031.639, 5165.105, 43.96414, 0, 0, 1, 0, 100, 0),
(1326710, 14, 3028.626, 5151.572, 44.19624, 0, 0, 1, 0, 100, 0),
(1326710, 15, 3025.031, 5143.610, 41.98796, 0, 0, 1, 0, 100, 0),
(1326710, 16, 3012.568, 5130.162, 39.90676, 0, 0, 1, 0, 100, 0),
(1326710, 17, 2996.858, 5128.554, 39.97041, 0, 0, 1, 0, 100, 0),
(1326710, 18, 3009.818, 5218.888, 63.96097, 0, 0, 1, 0, 100, 0),
(1326710, 19, 3005.054, 5234.823, 63.54463, 0, 0, 1, 0, 100, 0),
(1326710, 20, 2990.613, 5241.074, 59.59129, 0, 0, 1, 0, 100, 0),
(1326710, 21, 2975.220, 5228.281, 63.77478, 0, 0, 1, 0, 100, 0),
(1326710, 22, 2985.239, 5218.604, 64.67577, 0, 0, 1, 0, 100, 0),
(1326710, 23, 2994.729, 5214.046, 62.03285, 0, 0, 1, 0, 100, 0),
(1326710, 24, 2945.366, 5243.989, 60.69884, 0, 0, 1, 0, 100, 0),
(1326710, 25, 2958.108, 5238.834, 62.23391, 0, 0, 1, 0, 100, 0),
(1326710, 26, 2970.841, 5223.115, 65.58986, 0, 0, 1, 0, 100, 0),
(1326710, 27, 2976.701, 5215.880, 66.03274, 0, 0, 1, 0, 100, 0),
(1326710, 28, 2969.385, 5209.308, 66.59164, 0, 0, 1, 0, 100, 0),
(1326710, 29, 2957.362, 5209.952, 65.30394, 0, 0, 1, 0, 100, 0),
(1326710, 30, 2946.228, 5221.229, 64.00511, 0, 0, 1, 0, 100, 0),
(1326710, 31, 2934.068, 5224.393, 61.75064, 0, 0, 1, 0, 100, 0),
(1326710, 32, 2911.927, 5219.692, 63.49743, 0, 0, 1, 0, 100, 0),
(1326710, 33, 2892.455, 5212.407, 64.27552, 0, 0, 1, 0, 100, 0),
(1326710, 34, 2870.594, 5213.165, 64.30560, 0, 0, 1, 0, 100, 0),
(1326710, 35, 2832.370, 5224.731, 68.19075, 0, 0, 1, 0, 100, 0),
(1326710, 36, 2820.388, 5198.961, 69.84280, 0, 0, 1, 0, 100, 0),
(1326710, 37, 2809.529, 5162.243, 72.52879, 0, 0, 1, 0, 100, 0),
(1326710, 38, 2824.878, 5211.874, 69.39301, 0, 0, 1, 0, 100, 0),
(1326710, 39, 2839.532, 5249.529, 64.73550, 0, 0, 1, 0, 100, 0),
(1326710, 40, 2824.878, 5211.874, 69.39301, 0, 0, 1, 0, 100, 0),
(1326710, 41, 2809.529, 5162.243, 72.52879, 0, 0, 1, 0, 100, 0),
(1326710, 42, 2820.388, 5198.961, 69.84280, 0, 0, 1, 0, 100, 0),
(1326710, 43, 2832.370, 5224.731, 68.19075, 0, 0, 1, 0, 100, 0),
(1326710, 44, 2870.594, 5213.165, 64.30560, 0, 0, 1, 0, 100, 0),
(1326710, 45, 2892.455, 5212.407, 64.27552, 0, 0, 1, 0, 100, 0),
(1326710, 46, 2911.927, 5219.692, 63.49743, 0, 0, 1, 0, 100, 0),
(1326710, 47, 2934.068, 5224.393, 61.75064, 0, 0, 1, 0, 100, 0),
(1326710, 48, 2946.228, 5221.229, 64.00511, 0, 0, 1, 0, 100, 0),
(1326710, 49, 2957.362, 5209.952, 65.30394, 0, 0, 1, 0, 100, 0),
(1326710, 50, 2969.385, 5209.308, 66.59164, 0, 0, 1, 0, 100, 0),
(1326710, 51, 2976.701, 5215.880, 66.03274, 0, 0, 1, 0, 100, 0),
(1326710, 52, 2970.841, 5223.115, 65.58986, 0, 0, 1, 0, 100, 0),
(1326710, 53, 2958.108, 5238.834, 62.23391, 0, 0, 1, 0, 100, 0),
(1326710, 54, 2945.366, 5243.989, 60.69884, 0, 0, 1, 0, 100, 0),
(1326710, 55, 2994.729, 5214.046, 62.03285, 0, 0, 1, 0, 100, 0),
(1326710, 56, 2985.239, 5218.604, 64.67577, 0, 0, 1, 0, 100, 0),
(1326710, 57, 2975.220, 5228.281, 63.77478, 0, 0, 1, 0, 100, 0),
(1326710, 58, 2990.613, 5241.074, 59.59129, 0, 0, 1, 0, 100, 0),
(1326710, 59, 3005.054, 5234.823, 63.54463, 0, 0, 1, 0, 100, 0),
(1326710, 60, 3009.818, 5218.888, 63.96097, 0, 0, 1, 0, 100, 0),
(1326710, 61, 2996.858, 5128.554, 39.97041, 0, 0, 1, 0, 100, 0),
(1326710, 62, 3012.568, 5130.162, 39.90676, 0, 0, 1, 0, 100, 0),
(1326710, 63, 3025.031, 5143.610, 41.98796, 0, 0, 1, 0, 100, 0),
(1326710, 64, 3028.626, 5151.572, 44.19624, 0, 0, 1, 0, 100, 0),
(1326710, 65, 3031.639, 5165.105, 43.96414, 0, 0, 1, 0, 100, 0),
(1326710, 66, 3015.048, 5176.491, 45.69197, 0, 0, 1, 0, 100, 0),
(1326710, 67, 3011.845, 5183.586, 48.79534, 0, 0, 1, 0, 100, 0),
(1326710, 68, 3009.895, 5191.224, 52.18281, 0, 0, 1, 0, 100, 0),
(1326710, 69, 3009.132, 5196.373, 55.91871, 0, 0, 1, 0, 100, 0),
(1326710, 70, 3006.951, 5199.074, 58.58787, 0, 0, 1, 0, 100, 0),
(1326710, 71, 3003.693, 5203.208, 60.32031, 0, 0, 1, 0, 100, 0),
(1326710, 72, 3066.866, 5204.818, 65.68366, 0, 0, 1, 0, 100, 0),
(1326710, 73, 3088.863, 5224.569, 63.84053, 0, 0, 1, 0, 100, 0),
(1326710, 74, 3084.753, 5248.626, 61.92756, 0, 0, 1, 0, 100, 0),
(1326710, 75, 3076.892, 5261.172, 60.56978, 0, 0, 1, 0, 100, 0),
(1326710, 76, 3078.038, 5275.828, 59.00567, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132671;
INSERT INTO `creature_formations` VALUES
(132671,132671, 0, 0, 515, 0, 0),
(132671,133086,3,270,515, 0, 0),
(132671,133063,3,90,515, 0, 0),
(132671,133053,4,180,515, 0, 0),
(132671,133076,4,360,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132668;
UPDATE `creature_addon` SET `path_id` = 1326680 WHERE `guid` = 132668;
DELETE FROM `waypoint_data` WHERE `id` = 1326680;
INSERT INTO `waypoint_data` VALUES
(1326680, 1, 2908.825, 5313.557, 59.12885, 0, 0, 1, 0, 100, 0),
(1326680, 2, 2888.106, 5318.531, 60.94403, 0, 0, 1, 0, 100, 0),
(1326680, 3, 2856.176, 5325.956, 61.20460, 0, 0, 1, 0, 100, 0),
(1326680, 4, 2829.167, 5323.927, 65.53897, 0, 0, 1, 0, 100, 0),
(1326680, 5, 2787.074, 5312.364, 65.99152, 0, 0, 1, 0, 100, 0),
(1326680, 6, 2787.926, 5335.869, 63.71957, 0, 0, 1, 0, 100, 0),
(1326680, 7, 2791.290, 5359.015, 63.06731, 0, 0, 1, 0, 100, 0),
(1326680, 8, 2817.641, 5363.953, 65.13737, 0, 0, 1, 0, 100, 0),
(1326680, 9, 2859.010, 5371.705, 63.80011, 0, 0, 1, 0, 100, 0),
(1326680, 10, 2817.641, 5363.953, 65.13737, 0, 0, 1, 0, 100, 0),
(1326680, 11, 2791.290, 5359.015, 63.06731, 0, 0, 1, 0, 100, 0),
(1326680, 12, 2787.926, 5335.869, 63.71957, 0, 0, 1, 0, 100, 0),
(1326680, 13, 2787.074, 5312.364, 65.99152, 0, 0, 1, 0, 100, 0),
(1326680, 14, 2829.167, 5323.927, 65.53897, 0, 0, 1, 0, 100, 0),
(1326680, 15, 2856.176, 5325.956, 61.20460, 0, 0, 1, 0, 100, 0),
(1326680, 16, 2888.106, 5318.531, 60.94403, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132668;
INSERT INTO `creature_formations` VALUES
(132668,132668, 0, 0, 515, 0, 0),
(132668,133084,3,270,515, 0, 0),
(132668,133085,3,90,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 133083;
DELETE FROM `creature_addon` WHERE `guid` = 133083;
INSERT INTO `creature_addon` VALUES (133083, 1330830, 0, 0, 0, 0, '');
DELETE FROM `waypoint_data` WHERE `id` = 1330830;
INSERT INTO `waypoint_data` VALUES
(1330830, 1, 2775.316, 5392.425, 61.55022, 0, 0, 1, 0, 100, 0),
(1330830, 2, 2771.010, 5368.900, 60.22773, 0, 0, 1, 0, 100, 0),
(1330830, 3, 2758.813, 5344.582, 59.19929, 0, 0, 1, 0, 100, 0),
(1330830, 4, 2743.394, 5320.708, 63.35964, 0, 0, 1, 0, 100, 0),
(1330830, 5, 2728.582, 5297.774, 66.86511, 0, 0, 1, 0, 100, 0),
(1330830, 6, 2743.394, 5320.708, 63.35964, 0, 0, 1, 0, 100, 0),
(1330830, 7, 2758.813, 5344.582, 59.19929, 0, 0, 1, 0, 100, 0),
(1330830, 8, 2771.010, 5368.900, 60.22773, 0, 0, 1, 0, 100, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132654;
UPDATE `creature_addon` SET `path_id` = 1326540 WHERE `guid` = 132654;
DELETE FROM `waypoint_data` WHERE `id` = 1326540;
INSERT INTO `waypoint_data` VALUES
(1326540, 1, 2806.499, 5419.543, 60.74454, 0, 0, 1, 0, 100, 0),
(1326540, 2, 2793.926, 5432.459, 57.89107, 0, 0, 1, 0, 100, 0),
(1326540, 3, 2786.131, 5460.169, 55.03708, 0, 0, 1, 0, 100, 0),
(1326540, 4, 2807.406, 5471.908, 53.58485, 0, 0, 1, 0, 100, 0),
(1326540, 5, 2835.915, 5467.870, 52.70282, 0, 0, 1, 0, 100, 0),
(1326540, 6, 2858.869, 5443.945, 55.30711, 0, 0, 1, 0, 100, 0),
(1326540, 7, 2874.847, 5411.752, 57.54176, 0, 0, 1, 0, 100, 0),
(1326540, 8, 2887.229, 5383.130, 62.48748, 0, 0, 1, 0, 100, 0),
(1326540, 9, 2905.588, 5358.814, 62.74432, 0, 0, 1, 0, 100, 0),
(1326540, 10, 2887.229, 5383.130, 62.48748, 0, 0, 1, 0, 100, 0),
(1326540, 11, 2874.847, 5411.752, 57.54176, 0, 0, 1, 0, 100, 0),
(1326540, 12, 2858.869, 5443.945, 55.30711, 0, 0, 1, 0, 100, 0),
(1326540, 13, 2835.915, 5467.870, 52.70282, 0, 0, 1, 0, 100, 0),
(1326540, 14, 2807.406, 5471.908, 53.58485, 0, 0, 1, 0, 100, 0),
(1326540, 15, 2786.131, 5460.169, 55.03708, 0, 0, 1, 0, 100, 0),
(1326540, 16, 2793.926, 5432.459, 57.89107, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132654;
INSERT INTO `creature_formations` VALUES
(132654,132654, 0, 0, 515, 0, 0),
(132654,133082,3,90,515, 0, 0),
(132654,133051,3,270,515, 0, 0);
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 132664;
UPDATE `creature_addon` SET `path_id` = 1326640 WHERE `guid` = 132664;
DELETE FROM `waypoint_data` WHERE `id` = 1326640;
INSERT INTO `waypoint_data` VALUES
(1326640, 1, 3014.514, 5372.346, 66.08418, 0, 0, 1, 0, 100, 0),
(1326640, 2, 2994.026, 5397.278, 64.34697, 0, 0, 1, 0, 100, 0),
(1326640, 3, 2990.140, 5422.983, 63.13604, 0, 0, 1, 0, 100, 0),
(1326640, 4, 3012.347, 5454.812, 59.65188, 0, 0, 1, 0, 100, 0),
(1326640, 5, 3040.380, 5464.229, 59.25920, 0, 0, 1, 0, 100, 0),
(1326640, 6, 3072.772, 5455.386, 59.88902, 0, 0, 1, 0, 100, 0),
(1326640, 7, 3092.614, 5421.344, 58.48320, 0, 0, 1, 0, 100, 0),
(1326640, 8, 3087.620, 5391.421, 60.23003, 0, 0, 1, 0, 100, 0),
(1326640, 9, 3059.812, 5343.592, 62.34782, 0, 0, 1, 0, 100, 0),
(1326640, 10, 3031.504, 5315.883, 60.30353, 0, 0, 1, 0, 100, 0),
(1326640, 11, 3010.197, 5320.565, 61.14947, 0, 0, 1, 0, 100, 0),
(1326640, 12, 2999.953, 5335.159, 63.15840, 0, 0, 1, 0, 100, 0),
(1326640, 13, 3010.197, 5320.565, 61.14947, 0, 0, 1, 0, 100, 0),
(1326640, 14, 3031.504, 5315.883, 60.30353, 0, 0, 1, 0, 100, 0),
(1326640, 15, 3059.812, 5343.592, 62.34782, 0, 0, 1, 0, 100, 0),
(1326640, 16, 3087.620, 5391.421, 60.23003, 0, 0, 1, 0, 100, 0),
(1326640, 17, 3092.614, 5421.344, 58.48320, 0, 0, 1, 0, 100, 0),
(1326640, 18, 3072.772, 5455.386, 59.88902, 0, 0, 1, 0, 100, 0),
(1326640, 19, 3040.380, 5464.229, 59.25920, 0, 0, 1, 0, 100, 0),
(1326640, 20, 3012.347, 5454.812, 59.65188, 0, 0, 1, 0, 100, 0),
(1326640, 21, 2990.140, 5422.983, 63.13604, 0, 0, 1, 0, 100, 0),
(1326640, 22, 2994.026, 5397.278, 64.34697, 0, 0, 1, 0, 100, 0);
DELETE FROM `creature_formations` WHERE `leaderGUID` = 132664;
INSERT INTO `creature_formations` VALUES
(132664,132664, 0, 0, 515, 0, 0),
(132664,133067,3,270,515, 0, 0),
(132664,133075,3,90,515, 0, 0);
UPDATE gameobject_template SET data9 = 2 WHERE `entry` = 		 1673;
UPDATE gameobject_template SET data9 = 32 WHERE `entry` = 		 1765;
UPDATE gameobject_template SET data9 = 25 WHERE `entry` = 		 2039;
UPDATE gameobject_template SET data9 = 31 WHERE `entry` = 		 2560;
UPDATE gameobject_template SET data9 = 22 WHERE `entry` = 		 2714;
UPDATE gameobject_template SET data9 = 30 WHERE `entry` = 		 2716;
UPDATE gameobject_template SET data9 = 29 WHERE `entry` = 		 2717;
UPDATE gameobject_template SET data9 = 29 WHERE `entry` = 		 2718;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 2844;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 2845;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 2847;
UPDATE gameobject_template SET data9 = 10 WHERE `entry` = 		 2849;
UPDATE gameobject_template SET data9 = 15 WHERE `entry` = 		 2850;
UPDATE gameobject_template SET data9 = 20 WHERE `entry` = 		 2852;
UPDATE gameobject_template SET data9 = 24 WHERE `entry` = 		 2855;
UPDATE gameobject_template SET data9 = 28 WHERE `entry` = 		 2857;
UPDATE gameobject_template SET data9 = 7 WHERE `entry` = 		 3642;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 3658;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 3660;
UPDATE gameobject_template SET data9 = 7 WHERE `entry` = 		 3661;
UPDATE gameobject_template SET data9 = 21 WHERE `entry` = 		 3689;
UPDATE gameobject_template SET data9 = 26 WHERE `entry` = 		 3703;
UPDATE gameobject_template SET data9 = 26 WHERE `entry` = 		 3704;
UPDATE gameobject_template SET data9 = 17 WHERE `entry` = 		 3715;
UPDATE gameobject_template SET data9 = 20 WHERE `entry` = 		 4096;
UPDATE gameobject_template SET data9 = 34 WHERE `entry` = 		 4149;
UPDATE gameobject_template SET data9 = 10 WHERE `entry` = 		 19019;
UPDATE gameobject_template SET data9 = 17 WHERE `entry` = 		 19020;
UPDATE gameobject_template SET data9 = 17 WHERE `entry` = 		 74447;
UPDATE gameobject_template SET data9 = 18 WHERE `entry` = 		 74448;
UPDATE gameobject_template SET data9 = 11 WHERE `entry` = 		 75293;
UPDATE gameobject_template SET data9 = 14 WHERE `entry` = 		 75295;
UPDATE gameobject_template SET data9 = 25 WHERE `entry` = 		 75296;
UPDATE gameobject_template SET data9 = 16 WHERE `entry` = 		 75298;
UPDATE gameobject_template SET data9 = 23 WHERE `entry` = 		 75299;
UPDATE gameobject_template SET data9 = 26 WHERE `entry` = 		 75300;
UPDATE gameobject_template SET data9 = 25 WHERE `entry` = 		 105578;
UPDATE gameobject_template SET data9 = 25 WHERE `entry` = 		 105579;
UPDATE gameobject_template SET data9 = 28 WHERE `entry` = 		 105581;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 106318;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 106319;
UPDATE gameobject_template SET data9 = 9 WHERE `entry` = 		 111095;
UPDATE gameobject_template SET data9 = 28 WHERE `entry` = 		 131978;
UPDATE gameobject_template SET data9 = 51 WHERE `entry` = 		 141979;
UPDATE gameobject_template SET data9 = 35 WHERE `entry` = 		 142184;
UPDATE gameobject_template SET data9 = 43 WHERE `entry` = 		 153123;
UPDATE gameobject_template SET data9 = 38 WHERE `entry` = 		 153451;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 153453;
UPDATE gameobject_template SET data9 = 47 WHERE `entry` = 		 153454;
UPDATE gameobject_template SET data9 = 47 WHERE `entry` = 		 153468;
UPDATE gameobject_template SET data9 = 50 WHERE `entry` = 		 153469;
UPDATE gameobject_template SET data9 = 46 WHERE `entry` = 		 153471;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 160836;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 160845;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 161495;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 165554;
UPDATE gameobject_template SET data9 = 74 WHERE `entry` = 		 169243;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 173232;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 173234;
UPDATE gameobject_template SET data9 = 51 WHERE `entry` = 		 175245;
UPDATE gameobject_template SET data9 = 50 WHERE `entry` = 		 176224;
UPDATE gameobject_template SET data9 = 50 WHERE `entry` = 		 176325;
UPDATE gameobject_template SET data9 = 51 WHERE `entry` = 		 176327;
UPDATE gameobject_template SET data9 = 24 WHERE `entry` = 		 176582;
UPDATE gameobject_template SET data9 = 51 WHERE `entry` = 		 176944;
UPDATE gameobject_template SET data9 = 12 WHERE `entry` = 		 179486;
UPDATE gameobject_template SET data9 = 11 WHERE `entry` = 		 179487;
UPDATE gameobject_template SET data9 = 12 WHERE `entry` = 		 179488;
UPDATE gameobject_template SET data9 = 11 WHERE `entry` = 		 179489;
UPDATE gameobject_template SET data9 = 33 WHERE `entry` = 		 179490;
UPDATE gameobject_template SET data9 = 23 WHERE `entry` = 		 179491;
UPDATE gameobject_template SET data9 = 33 WHERE `entry` = 		 179492;
UPDATE gameobject_template SET data9 = 29 WHERE `entry` = 		 179493;
UPDATE gameobject_template SET data9 = 38 WHERE `entry` = 		 179494;
UPDATE gameobject_template SET data9 = 35 WHERE `entry` = 		 179496;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 179497;
UPDATE gameobject_template SET data9 = 45 WHERE `entry` = 		 179498;
UPDATE gameobject_template SET data9 = 51 WHERE `entry` = 		 179501;
UPDATE gameobject_template SET data9 = 52 WHERE `entry` = 		 180228;
UPDATE gameobject_template SET data9 = 51 WHERE `entry` = 		 180229;
UPDATE gameobject_template SET data9 = 60 WHERE `entry` = 		 181280;
UPDATE gameobject_template SET data9 = 74 WHERE `entry` = 		 181366;
UPDATE gameobject_template SET data9 = 7 WHERE `entry` = 		 181665;
UPDATE gameobject_template SET data9 = 52 WHERE `entry` = 		 181798;
UPDATE gameobject_template SET data9 = 54 WHERE `entry` = 		 181800;
UPDATE gameobject_template SET data9 = 59 WHERE `entry` = 		 181802;
UPDATE gameobject_template SET data9 = 62 WHERE `entry` = 		 184465;
UPDATE gameobject_template SET data9 = 60 WHERE `entry` = 		 184716;
UPDATE gameobject_template SET data9 = 53 WHERE `entry` = 		 184740;
UPDATE gameobject_template SET data9 = 57 WHERE `entry` = 		 184741;
UPDATE gameobject_template SET data9 = 8 WHERE `entry` = 		 184793;
UPDATE gameobject_template SET data9 = 52 WHERE `entry` = 		 184931;
UPDATE gameobject_template SET data9 = 56 WHERE `entry` = 		 184934;
UPDATE gameobject_template SET data9 = 53 WHERE `entry` = 		 184935;
UPDATE gameobject_template SET data9 = 55 WHERE `entry` = 		 184937;
UPDATE gameobject_template SET data9 = 61 WHERE `entry` = 		 184940;
UPDATE gameobject_template SET data9 = 61 WHERE `entry` = 		 184941;
UPDATE gameobject_template SET data9 = 63 WHERE `entry` = 		 185119;
UPDATE gameobject_template SET data9 = 3 WHERE `entry` = 		 19017;
UPDATE gameobject_template SET data9 = 13 WHERE `entry` = 		 19018;
UPDATE gameobject_template SET data9 = 60 WHERE `entry` = 		 181804;
UPDATE gameobject_template SET data9 = 4 WHERE `entry` = 		 152608;
UPDATE gameobject_template SET data9 = 6 WHERE `entry` = 		 152618;
UPDATE gameobject_template SET data9 = 47 WHERE `entry` = 		 153464;
UPDATE gameobject_template SET data9 = 61 WHERE `entry` = 		 191303;
UPDATE gameobject_template SET data9 = 66 WHERE `entry` = 		 193402;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 193997;
UPDATE gameobject_template SET data9 = 68 WHERE `entry` = 		 190586;
UPDATE gameobject_template SET data9 = 66 WHERE `entry` = 		 190778;
UPDATE gameobject_template SET data9 = 68 WHERE `entry` = 		 193996;
UPDATE gameobject_template SET data9 = 74 WHERE `entry` = 		 193426;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 202180;
UPDATE gameobject_template SET data9 = 74 WHERE `entry` = 		 202240;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 202336;
UPDATE gameobject_template SET data9 = 8 WHERE `entry` = 		 202081;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 201874;
UPDATE gameobject_template SET data9 = 40 WHERE `entry` = 		 202082;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 201872;
UPDATE gameobject_template SET data9 = 74 WHERE `entry` = 		 202238;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 202177;
UPDATE gameobject_template SET data9 = 27 WHERE `entry` = 		 202080;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 201875;
UPDATE gameobject_template SET data9 = 74 WHERE `entry` = 		 202241;
UPDATE gameobject_template SET data9 = 71 WHERE `entry` = 		 202179;
UPDATE gameobject_template SET data9 = 20 WHERE `entry` = 		 202083;
DELETE FROM `creature` WHERE `guid` = 3253933;
DELETE FROM `creature` WHERE `guid` = 15850;
INSERT INTO `creature` VALUES
(15850, 2304, 0, 1, 1, 3769, 1, -1312.829956, -1209.420044, 49.902805, 3.81878, 300, 0, 0, 1057, 0, 0, 0, 0, 0);
UPDATE `creature` SET `position_x` = -17.79, `position_y` = -1762.00, `position_z` = 92.06 WHERE `guid` = 20534;
UPDATE `creature` SET `position_x` = -845.73, `position_y` = -2661.14, `position_z` = 91.78 WHERE `guid` = 18658;
UPDATE `event_scripts` SET `datalong2` = 60000 WHERE `id` = 2048;
UPDATE gameobject_template SET data9 = 0 WHERE `entry` = 190169;UPDATE `creature` SET `position_x` = -1760.4, `position_y` = 8827.02 , `position_z` = 29.7228, `orientation` = 6.05053, `MovementType` = 0 WHERE `guid` = 59727;
UPDATE `creature` SET `position_x` = -1760.78, `position_y` = 8819.14, `position_z` = 29.774,  `orientation` = 5.95985, `MovementType` = 0 WHERE `guid` = 59728;
UPDATE `creature` SET `position_x` = -1763.53, `position_y` = 8837.79, `position_z` = 28.4004, `orientation` = 5.95284, `MovementType` = 0 WHERE `guid` = 59729;
UPDATE `creature` SET `position_x` = -1762.33, `position_y` = 8813.72, `position_z` = 29.5579, `orientation` = 6.21482, `MovementType` = 0 WHERE `guid` = 59730;
UPDATE `creature` SET `position_x` = -1754.54, `position_y` = 8835.31, `position_z` = 31.605,  `orientation` = 5.84137, `MovementType` = 0 WHERE `guid` = 59972;
UPDATE `creature` SET `position_x` = -1765.94, `position_y` = 8828.44, `position_z` = 28.2994, `orientation` = 5.96761, `MovementType` = 0 WHERE `guid` = 59973;
UPDATE `creature` SET `position_x` = -1756.29, `position_y` = 8826.99, `position_z` = 31.286,  `orientation` = 6.00502, `MovementType` = 0 WHERE `guid` = 59974;
UPDATE `creature` SET `position_x` = -1763.74, `position_y` = 8824.98, `position_z` = 28.77,   `orientation` = 5.67054, `MovementType` = 2 WHERE `guid` = 65528;
UPDATE `creature` SET `position_x` = -1772.38, `position_y` = 8821.69, `position_z` = 28.1739, `orientation` = 5.96555, `MovementType` = 0 WHERE `guid` = 68109;
UPDATE `creature` SET `position_x` = -1772.05, `position_y` = 8831.58, `position_z` = 28.0505, `orientation` = 6.01998, `MovementType` = 0 WHERE `guid` = 68110;

DELETE FROM `creature_formations` WHERE `leaderGUID` = 65528;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
(65528, 59727, 12, 350, 515),
(65528, 59728, 12, 15, 515),
(65528, 59729, 21, 330, 515),
(65528, 59730, 17, 40, 515),
(65528, 59972, 13, 330, 515),
(65528, 59973, 18, 350, 515),
(65528, 59974, 8, 345, 515),
(65528, 65528, 0, 0, 515),
(65528, 68109, 24, 10, 515),
(65528, 68110, 25, 355, 515);

DELETE FROM `creature_addon` WHERE `guid` = 65528;
INSERT INTO `creature_addon` VALUES
(65528, 65528, 0, 0, 4097, 0, '');

DELETE FROM `waypoint_data` WHERE `id` = 65528;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `action`, `action_chance`, `wpguid`) VALUES 
(65528, 1, -1722.28, 8819.48, 33.3289, 0, 0, 0, 100, 0),
(65528, 2, -1702.28, 8815.27, 33.3215, 0, 0, 0, 100, 0),
(65528, 3, -1679.72, 8809.79, 37.9979, 0, 0, 0, 100, 0),
(65528, 4, -1665.33, 8807.19, 38.865, 0, 0, 0, 100, 0),
(65528, 5, -1627.73, 8792.1, 36.8225, 0, 0, 0, 100, 0),
(65528, 6, -1544.21, 8763.72, 33.0589, 0, 0, 0, 100, 0),
(65528, 7, -1517.92, 8760.38, 28.7265, 0, 0, 0, 100, 0),
(65528, 8, -1470.43, 8750.48, 28.0288, 0, 0, 0, 100, 0),
(65528, 9, -1425.19, 8726.87, 21.7158, 0, 0, 0, 100, 0),
(65528, 10, -1395.23, 8717.94, 23.6657, 0, 0, 0, 100, 0),
(65528, 11, -1365.8, 8700.39, 28.4882, 0, 0, 0, 100, 0),
(65528, 12, -1347.92, 8663.28, 24.9401, 0, 0, 0, 100, 0),
(65528, 13, -1352.99, 8628.97, 25.1234, 0, 0, 0, 100, 0),
(65528, 14, -1345.75, 8602.56, 22.7522, 0, 0, 0, 100, 0),
(65528, 15, -1332.21, 8590, 19.7613, 0, 0, 0, 100, 0),
(65528, 16, -1288.52, 8597.4, 22.4054, 0, 0, 0, 100, 0),
(65528, 17, -1247.69, 8631.38, 32.358, 0, 0, 0, 100, 0),
(65528, 18, -1221.92, 8669.44, 38.759, 0, 0, 0, 100, 0),
(65528, 19, -1230.99, 8689.39, 37.4873, 0, 0, 0, 100, 0),
(65528, 20, -1246.52, 8711.41, 34.5571, 0, 0, 0, 100, 0),
(65528, 21, -1264.72, 8731.36, 32.453, 0, 0, 0, 100, 0),
(65528, 22, -1283.21, 8742.08, 35.1921, 0, 0, 0, 100, 0),
(65528, 23, -1302.94, 8740.64, 34.1554, 0, 0, 0, 100, 0),
(65528, 24, -1328.93, 8730.21, 29.988, 0, 0, 0, 100, 0),
(65528, 25, -1359.77, 8736.24, 28.6292, 0, 0, 0, 100, 0),
(65528, 26, -1374.58, 8757.73, 31.2344, 0, 0, 0, 100, 0),
(65528, 27, -1378.19, 8802.87, 34.4092, 0, 0, 0, 100, 0),
(65528, 28, -1396.67, 8830.66, 29.1786, 0, 0, 0, 100, 0),
(65528, 29, -1415.89, 8849.03, 32.9179, 0, 0, 0, 100, 0),
(65528, 30, -1439.05, 8862.96, 37.7159, 0, 0, 0, 100, 0),
(65528, 31, -1457.39, 8872.23, 37.3876, 0, 0, 0, 100, 0),
(65528, 32, -1481.63, 8894.11, 41.1984, 0, 0, 0, 100, 0),
(65528, 33, -1491.28, 8909.66, 43.6029, 0, 0, 0, 100, 0),
(65528, 34, -1505.11, 8929.37, 42.0836, 0, 0, 0, 100, 0),
(65528, 35, -1519.29, 8943.56, 38.28, 0, 0, 0, 100, 0),
(65528, 36, -1581.56, 8968.29, 39.1789, 0, 0, 0, 100, 0),
(65528, 37, -1619.78, 8971.95, 41.2737, 0, 0, 0, 100, 0),
(65528, 38, -1655.18, 8964.68, 43.4279, 0, 0, 0, 100, 0),
(65528, 39, -1678.58, 8957.2, 39.4291, 0, 0, 0, 100, 0),
(65528, 40, -1721.1, 8963.63, 48.2463, 0, 0, 0, 100, 0),
(65528, 41, -1759.58, 8971.58, 58.4921, 0, 0, 0, 100, 0),
(65528, 42, -1763.97, 8972.37, 58.6278, 0, 0, 0, 100, 0),
(65528, 43, -1759.46, 8944.18, 58.3036, 10000, 0, 0, 100, 0),
(65528, 44, -1782.38, 8980.54, 54.589, 0, 0, 0, 100, 0),
(65528, 45, -1794.69, 8981.32, 50.3973, 0, 0, 0, 100, 0),
(65528, 46, -1809.13, 8982.4, 46.1651, 0, 0, 0, 100, 0),
(65528, 47, -1815.58, 8976.12, 41.7977, 0, 0, 0, 100, 0),
(65528, 48, -1830.36, 8948.43, 39.7659, 0, 0, 0, 100, 0),
(65528, 49, -1849.44, 8918.3, 37.4994, 0, 0, 0, 100, 0),
(65528, 50, -1853.8, 8911.71, 37.2204, 0, 0, 0, 100, 0),
(65528, 51, -1900.89, 8888.02, 37.0238, 0, 0, 0, 100, 0),
(65528, 52, -1904.16, 8887.94, 37.1452, 0, 0, 0, 100, 0),
(65528, 53, -1947.18, 8879.73, 36.1108, 0, 0, 0, 100, 0),
(65528, 54, -1875.97, 8847.95, 30.9403, 0, 0, 0, 100, 0),
(65528, 55, -1853.32, 8843.79, 30.0997, 0, 0, 0, 100, 0),
(65528, 56, -1807.56, 8834.99, 29.2595, 0, 0, 0, 100, 0),
(65528, 57, -1778.89, 8832.21, 28.8318, 0, 0, 0, 100, 0);
DELETE FROM `creature_loot_template` WHERE `entry` = 24173;
UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 64 WHERE `entry` = 24173;

DELETE FROM `conditions` WHERE `SourceEntry`=43189 AND `SourceTypeOrReferenceId`=17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 43189, 0, 0, 29, 0, 24173, 100, 0, 1, 0, 0, "", "Spell 'Test Horn' cannot be cast if NPC 'Frostgore' is nearby"),
(17, 0, 43189, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, "", "Spell 'Test Horn' cannot be cast if nearby NPC 'Frostgore' is alive");

UPDATE `event_scripts` SET `x`=2932.631, `y`=-4488.105, `z`=285.6876, `o`=3.298672 WHERE `id`=15726;

DELETE FROM `waypoints` WHERE `entry`=24173;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES
(24173, 1, 2918.373, -4505.415, 280.0419, ""),
(24173, 2, 2920.935, -4521.924, 276.0025, ""),
(24173, 3, 2923.956, -4535.499, 274.4535, ""),
(24173, 4, 2944.427, -4555.303, 273.6897, "");

UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=24173;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (24173, 2417300);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(24173, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 10000, 20000, 11, 52058, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Frostgore - In Combat - Cast 'Ground Slam'"),
(24173, 0, 1, 2, 11, 0, 100, 0, 0, 0, 0, 0, 18, 256, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Spawn - Add UNIT_FLAG_IMMUNE_TO_PC"),
(24173, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 53, 1, 24173, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Spawn - Start Waypoint"),
(24173, 0, 3, 0, 40, 0, 100, 0, 4, 0, 0, 0, 80, 2417300, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On WP 4 Reached - Start Script"),
(2417300, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Script - Set home position to summoner's position"),
(2417300, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Script - Turn to summoner"),
(2417300, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Script - Say Text 0"),
(2417300, 9, 3, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Script - Say Text 1"),
(2417300, 9, 4, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 19, 256, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Script - Remove UNIT_FLAG_IMMUNE_TO_PC"),
(2417300, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Frostgore - On Script - Attack summoner");

DELETE FROM `creature_text` WHERE `entry`=24173;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`) VALUES
(24173, 0, 0, 'Are YOU the one who interrupted my dinner? Well, are you?' , 12, 0, 100, 1, 0, 0, 0, "Frostgore"),
(24173, 1, 0, 'You\'re too ugly and scrawny to be a female yeti! And now you\'re making me mad...', 12, 0, 100, 25, 0, 0, 0, "Frostgore");DELETE FROM `smart_scripts` WHERE `entryorguid` = 28948 AND `source_type` = 0;
INSERT INTO `smart_scripts` VALUES (28948,0,0,4,60,0,100,1,1000,1000,0,0,53,0,28948,0,0,0,0,1,0,0,0,0,0,0,0,'start escort'),(28948,0,2,3,61,0,100,1,1,1,0,0,33,28929,0,0,0,0,0,23,0,0,0,0,0,0,0,'Killmonster credit on data set'),(28948,0,3,0,61,0,100,0,0,0,0,0,41,500,500,0,0,0,0,1,0,0,0,0,0,0,0,'forcedespawn linked'),(28948,0,4,0,61,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text linked'),(28948,0,5,0,40,0,100,0,1,0,0,0,1,1,0,0,0,0,0,23,0,0,0,0,0,0,0,'say text 1 on wp reach'),(28948,0,6,0,40,0,100,0,3,0,0,0,1,2,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 2 on wp reach'),(28948,0,7,0,40,0,100,0,5,0,0,0,1,3,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 3 on wp reach'),(28948,0,8,9,40,0,100,0,6,0,0,0,1,4,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 4 on wp reach'),(28948,0,9,0,61,0,100,0,0,0,0,0,54,5000,0,0,0,0,0,1,0,0,0,0,0,0,0,'stop escort linked'),(28948,0,10,0,40,0,100,0,7,0,0,0,1,5,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 5 on wp reach'),(28948,0,11,0,40,0,100,0,9,0,0,0,1,6,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 6 on wp reach'),(28948,0,12,0,40,0,100,0,12,0,0,0,1,7,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 7 on wp reach'),(28948,0,13,0,40,0,100,0,14,0,0,0,1,8,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 8 on wp reach'),(28948,0,14,15,40,0,100,0,16,0,0,0,1,9,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 9 on wp reach'),(28948,0,15,0,61,0,100,0,0,0,0,0,54,5000,0,0,0,0,0,1,0,0,0,0,0,0,0,'stop escort linked'),(28948,0,16,0,40,0,100,0,17,0,0,0,1,10,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 10 on wp reach'),(28948,0,17,0,40,0,100,0,18,0,0,0,1,11,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 11 on wp reach'),(28948,0,18,0,40,0,100,0,19,0,0,0,1,12,0,0,0,0,0,23,0,0,0,0,0,0,0,'say text 12 on wp reach'),(28948,0,19,0,40,0,100,0,21,0,0,0,1,13,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 13 on wp reach'),(28948,0,20,21,40,0,100,0,23,0,0,0,1,14,0,0,0,0,0,1,0,0,0,0,0,0,0,'say text 14 on wp reach'),(28948,0,21,22,61,0,100,0,0,0,0,0,11,53101,2,0,0,0,0,23,0,0,0,0,0,0,0,'Cast Quest Credit'),(28948,0,22,0,61,0,100,0,0,0,0,0,41,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'Despawn');-- https://github.com/SunwellTracker/issues/issues/2450 Asarnan and Dealer Senzik
UPDATE `creature` SET `position_x` = 4196.29, `position_y` = 3019.43, `position_z` = 339.30 WHERE `guid` = 69945;
UPDATE `creature` SET `position_x` = 4200.39, `position_y` = 2998.34, `position_z` = 339.30 WHERE `guid` = 69943;
-- https://github.com/SunwellTracker/issues/issues/2474 Ruby Keepers get attacked by Mobb
UPDATE `creature` SET `unit_flags` = 537133056, `dynamicflags` = 32 WHERE `guid` = 108330;
UPDATE `creature_template` SET `faction` = 35 WHERE `entry` = 27530;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 17716 AND `id` = 1;
UPDATE `quest_template` SET `SpecialFlags` = 0 WHERE `id` = 9678;
-- DB/Quest: Across Transborea - only one guardian can be summoned
UPDATE `quest_template` SET `SourceSpellId` = 0 WHERE `id` = 11930;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 26158;
INSERT INTO `smart_scripts` VALUES
(26158, 0, 0, 0, 19, 0, 100, 0, 11930, 0, 0, 0, 85, 46657, 32, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Mother Tauranook - On Quest Accept (Across Transborea) - Invoker Cast Taunka\'le Evacuee');
UPDATE `smart_scripts` SET `action_param2`='10' WHERE  `entryorguid`=2572900 AND `source_type`=9 AND `id`=4 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 25729 AND `source_type` = 0 AND `id` = 11;
INSERT INTO `smart_scripts` VALUES (25729, 0, 11, 0, 11, 0, 100, 0, 0, 0, 0, 0, 70, 0, 0, 0, 0, 0, 0, 19, 25618, 100, 1, 0, 0, 0, 0, 'On Respawn - Respawn Varidus the Flenser');
UPDATE `smart_scripts` SET `link`='15' WHERE  `entryorguid`=18069 AND `source_type`=0 AND `id`=14 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18069 AND `source_type` = 0 AND `id` = 15;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES ('18069', 0, 15, 61, 41, 5000, 1, 'Mogor - On Just Died - Despawn after 5s');
UPDATE `creature` SET `spawntimesecs`='15' WHERE  `guid`=48191;-- Brokentoe
UPDATE `smart_scripts` SET `link`=3 WHERE  `entryorguid`=18398 AND `source_type`=0 AND `id`=2 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18398 AND `source_type` = 0 AND `id` = 3;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES (18398, 0, 3, 61, 41, 10000, 1, 'Brokentoe - On Just Died - Despawn after 10s');

-- The Blue Brothers
UPDATE `smart_scripts` SET `link`=6 WHERE  `entryorguid`=18399 AND `source_type`=0 AND `id`=5 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18399 AND `source_type` = 0 AND `id` = 6;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES (18399, 0, 6, 61, 41, 10000, 1, 'Murkblood Twin - On Just Died - Despawn after 10s');

-- Rokdar
UPDATE `smart_scripts` SET `link`=7 WHERE  `entryorguid`=18400 AND `source_type`=0 AND `id`=6 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18400 AND `source_type` = 0 AND `id` = 7;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES (18400, 0, 7, 61, 41, 10000, 1, 'Rokdar the Sundered Lord - On Just Died - Despawn after 10s');

-- Skra'gath
UPDATE `smart_scripts` SET `link`=5 WHERE  `entryorguid`=18401 AND `source_type`=0 AND `id`=4 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18401 AND `source_type` = 0 AND `id` = 5;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES (18401, 0, 5, 61, 41, 10000, 1, 'Skra\'gath - On Just Died - Despawn after 10s');

-- The Warmaul Champion
UPDATE `smart_scripts` SET `link`=7 WHERE  `entryorguid`=18402 AND `source_type`=0 AND `id`=6 AND `link`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18402 AND `source_type` = 0 AND `id` = 7;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES (18402, 0, 7, 61, 41, 10000, 1, 'The Warmaul Champion - On Just Died - Despawn after 10s');DELETE FROM `creature` WHERE `guid` IN (1980001,
1980002,
1980003,
1980004,
1980005,
1980006,
1980007,
1980008,
1980009,
1980010,
1980011,
1980060,
2071162,
2073073,
2336867,
2336868);

DELETE FROM `creature` WHERE `guid` BETWEEN 150219 AND 150234;
INSERT INTO `creature` VALUES 
(150219, 22180, 530, 1, 1, 0, 0, 1979.09, 7153.47, 364.452, 2.00989, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150220, 22180, 530, 1, 1, 0, 0, 1825.7, 7371.87, 363.729, 0.788591, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150221, 22180, 530, 1, 1, 0, 0, 2171.07, 7216.58, 364.881, 0.710827, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150222, 22180, 530, 1, 1, 0, 0, 2173.53, 7101.48, 364.853, 1.69257, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150223, 22180, 530, 1, 1, 0, 0, 2282.89, 7163.16, 365.87, 5.9298, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150224, 22180, 530, 1, 1, 0, 0, 2198.31, 7304.56, 364.129, 4.86951, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150225, 22180, 530, 1, 1, 0, 0, 2363.99, 7178.34, 365.139, 4.33937, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150226, 22180, 530, 1, 1, 0, 0, 2609.88, 7190.51, 364.477, 3.4707, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150227, 22180, 530, 1, 1, 0, 0, 2636.51, 7060.92, 369.635, 1.76245, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150228, 22180, 530, 1, 1, 0, 0, 2601.19, 7024.43, 362.248, 4.74696, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150229, 22180, 530, 1, 1, 0, 0, 2375.27, 7013.6, 371.731, 1.23623, 300, 5, 0, 6986, 0, 1, 0, 0, 0),
(150230, 21987, 571, 1, 1, 0, 0, 5868.2, 2107.88, 636.01, 3.67, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(150231, 20735, 571, 1, 1, 0, 1, 5697.52, 577.006, 653.73, 4.13285, 300, 0, 0, 100800, 88140, 0, 0, 0, 0),
(150232, 31439, 571, 1, 1, 0, 1, 5764.94, 522.138, 653.731, 4.11714, 300, 0, 0, 100800, 88140, 0, 0, 0, 0),
(150233, 24648, 571, 1, 1, 17188, 0, 1074.34, -5043.68, 53.421, 5.007, 300, 5, 0, 92910, 0, 1, 0, 0, 0),
(150234, 6491, 0, 1, 1, 0, 0, -1315.88, -3184.42, 37.28, 5.56, 120, 0, 0, 1, 0, 0, 0, 0, 0);

DELETE FROM `creature` WHERE `guid` IN (1980022,1980020, 1980019,1980021);

DELETE FROM `creature` WHERE `guid` = 1980000;
DELETE FROM `creature` WHERE `guid` = 150235;
INSERT INTO `creature` VALUES 
(150235, 22180, 530, 1, 1, 0, 0, 1914.07, 7143.44, 363.932, 0.167345, 300, 5, 0, 6986, 0, 1, 0, 0, 0);

DELETE FROM `creature` WHERE `guid` IN 
(500345,
500346,
500347,
501099,
501100,
501120,
501121,
501122,
501123,
501124,
501125,
501126,
501127,
501128,
501129,
501130,
501131,
501132,
501133,
501511,
501512,
501513,
501514,
501515);

DELETE FROM `creature` WHERE `guid` BETWEEN 150236 AND 150259;
INSERT INTO `creature` VALUES
(150236, 11447, 1, 1, 1, 0, 0, -3799.62, 1063.83, 132.806, 1.20424, 21600, 5, 0, 60000, 0, 0, 0, 0, 0),
(150237, 11497, 1, 1, 1, 0, 0, -3731.22, 1061.32, 132.345, 1.7815, 21600, 5, 0, 73000, 24340, 0, 0, 0, 0),
(150238, 11498, 1, 1, 1, 0, 0, -3690.96, 1077.14, 131.969, 2.65172, 21600, 5, 0, 57000, 0, 0, 0, 0, 0),
(150239, 23057, 556, 2, 1, 0, 0, -88.6554, 288.395, 85.9209, 1.99433, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150240, 23046, 556, 2, 1, 0, 0, -87.5742, 287.856, 26.4832, 0.0877018, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150241, 23058, 556, 2, 1, 0, 0, -60.0178, 259.103, 49.4026, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150242, 23058, 556, 2, 1, 0, 0, -55.5887, 259.197, 49.4026, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150243, 23058, 556, 2, 1, 0, 0, -79.8145, 258.72, 49.4547, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150244, 23058, 556, 2, 1, 0, 0, -84.2373, 258.395, 49.8633, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150245, 23058, 556, 2, 1, 0, 0, -87.9647, 258.651, 49.8035, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150246, 23058, 556, 2, 1, 0, 0, -108.388, 258.668, 49.5745, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150247, 23058, 556, 2, 1, 0, 0, -112.798, 258.679, 49.0179, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150248, 23058, 556, 2, 1, 0, 0, -113.927, 313.998, 49.5604, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150249, 23058, 556, 2, 1, 0, 0, -109.484, 314.084, 49.4821, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150250, 23058, 556, 2, 1, 0, 0, -86.7479, 314.083, 49.0207, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150251, 23058, 556, 2, 1, 0, 0, -82.4557, 314.246, 49.6519, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150252, 23058, 556, 2, 1, 0, 0, -78.768, 314.018, 49.9418, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150253, 23058, 556, 2, 1, 0, 0, -58.9259, 314.055, 49.5131, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150254, 23058, 556, 2, 1, 0, 0, -54.532, 315.237, 49.3668, 5.61655, 7200, 0, 0, 1, 0, 0, 0, 0, 0),
(150255, 8924, 0, 1, 1, 0, 0, -7365.01, -1006.1, 176.902, 2.89516, 108000, 5, 0, 11042, 0, 0, 0, 0, 0),
(150256, 14231, 1, 1, 1, 0, 0, -2799.72, -3129.45, 28.7032, 4.41743, 27000, 5, 0, 1525, 0, 0, 0, 0, 0),
(150257, 14237, 1, 1, 1, 0, 0, -4207.99, -2911.04, 44.9497, 2.051, 180000, 5, 0, 2508, 0, 0, 0, 0, 0),
(150258, 14233, 1, 1, 1, 0, 0, -4158.55, -3446.35, 28.5577, 1.14702, 38000, 5, 0, 1653, 0, 0, 0, 0, 0),
(150259, 14235, 1, 1, 1, 0, 0, -4042.09, -3753.6, 43.2492, 6.26622, 108000, 5, 0, 5620, 0, 0, 0, 0, 0);

DELETE FROM `creature` WHERE `guid` IN 
(501105,
501106,
501107,
501108,
501109,
501110,
501111,
501112,
501113,
501114,
501115,
501116,
501117,
501118,
501119, 501696);

DELETE FROM `creature` WHERE `guid` IN
(150260,
150261,
150262,
150263,
150264,
150265,
150266,
150267,
150268,
150269,
150270,
150271,
150272,
150273,
150274,
150275);
INSERT INTO `creature` VALUES
(150260, 21931, 556, 3, 1, 0, 0, -69.078, 255.062, 27.6692, 2.23169, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150261, 21931, 556, 3, 1, 0, 0, 84.9162, 251.397, 37.0668, 3.20414, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150262, 21931, 556, 3, 1, 0, 0, 82.7928, 251.065, 37.0668, 3.1921, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150263, 21931, 556, 3, 1, 0, 0, -55.3543, 292.414, 27.8438, 3.33441, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150264, 21931, 556, 3, 1, 0, 0, -56.8114, 293.477, 27.8298, 1.0669, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150265, 21931, 556, 3, 1, 0, 0, -109.229, 288.209, 53.3584, 3.24935, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150266, 21931, 556, 3, 1, 0, 0, -77.7733, 299.15, 60.5558, 5.75716, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150267, 21931, 556, 3, 1, 0, 0, -52.3969, 293.288, 27.8296, 1.42095, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150268, 21931, 556, 3, 1, 0, 0, -119.16, 291.958, 26.7305, 0.830551, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150269, 21931, 556, 3, 1, 0, 0, -59.42, 289.1, 27.8929, 2.89434, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150270, 21931, 556, 3, 1, 0, 0, -102.279, 278.86, 56.0703, 5.41457, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150271, 21931, 556, 3, 1, 0, 0, -86.6064, 286.122, 27.4832, 2.95394, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150272, 21931, 556, 3, 1, 0, 0, -68.3353, 281.741, 61.757, 3.84418, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150273, 21931, 556, 3, 1, 0, 0, -71.9701, 272.574, 59.1446, 6.18433, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150274, 21931, 556, 3, 1, 0, 0, -84.3563, 263.728, 27.8079, 4.52383, 7200, 0, 0, 1, 0, 2, 0, 0, 0),
(150275, 16916, 530, 1, 1, 0, 0, 9513.45, -6838.06, 16.5768, 4.28651, 300, 0, 0, 98, 115, 2, 0, 0, 0);

DELETE FROM `creature_addon` WHERE `guid` IN (501105,
501106,
501107,
501108,
501109,
501110,
501111,
501112,
501113,
501114,
501115,
501116,
501117,
501118,
501119);
DELETE FROM `creature_addon` WHERE `guid` IN
(150260,
150261,
150262,
150263,
150264,
150265,
150266,
150267,
150268,
150269,
150270,
150271,
150272,
150273,
150274);
INSERT INTO `creature_addon` VALUES
(150260, 5011050, 0, 0, 0, 0, ''),
(150261, 5011060, 0, 0, 0, 0, ''),
(150262, 5011070, 0, 0, 0, 0, ''),
(150263, 5011080, 0, 0, 0, 0, ''),
(150264, 5011090, 0, 0, 0, 0, ''),
(150265, 5011100, 0, 0, 0, 0, ''),
(150266, 5011110, 0, 0, 0, 0, ''),
(150267, 5011120, 0, 0, 0, 0, ''),
(150268, 5011130, 0, 0, 0, 0, ''),
(150269, 5011140, 0, 0, 0, 0, ''),
(150270, 5011150, 0, 0, 0, 0, ''),
(150271, 5011160, 0, 0, 0, 0, ''),
(150272, 5011170, 0, 0, 0, 0, ''),
(150273, 5011180, 0, 0, 0, 0, ''),
(150274, 5011190, 0, 0, 0, 0, '');

DELETE FROM `creature` WHERE `guid` IN (400300,400301,400302,400303,400304);
DELETE FROM `creature` WHERE `guid` IN (150276,150277,150278,150279,150280);
INSERT INTO `creature` VALUES 
(150276, 210001, 572, 1, 1, 0, 0, 1253.63, 1670.55, 34.2673, 0.72727, 300, 0, 0, 1753, 0, 0, 0, 0, 0),
(150277, 210001, 562, 1, 1, 7103, 0, 6267.18, 295.589, 4.45783, 4.26079, 300, 0, 0, 1753, 0, 0, 0, 0, 0),
(150278, 210001, 559, 1, 1, 7103, 0, 4059.07, 2946.58, 12.7725, 4.58981, 300, 0, 0, 1753, 0, 0, 0, 0, 0),
(150279, 210001, 617, 1, 1, 7103, 0, 1306.68, 777.696, 7.11492, 2.78031, 300, 0, 0, 1753, 0, 0, 0, 0, 0),
(150280, 210001, 618, 1, 1, 0, 0, 764.617, -248.443, 28.2767, 4.67491, 300, 0, 0, 1753, 0, 0, 0, 0, 0);
UPDATE `creature_template` SET AIName = 'SmartAI' WHERE `entry` = 26158;UPDATE `smart_scripts` SET `action_param2` = 34 WHERE entryorguid = 21318 AND source_type = 0 AND id = 3;
UPDATE `gameobject_template` SET `data9` = 0 WHERE `entry` IN (190176, 190174, 190171, 193997, 191019, 190175, 190172, 190170, 191303, 190173, 190169, 189973);UPDATE `item_template` SET `flags` = `flags` | 2048 WHERE `entry` = 35234;UPDATE `spell_proc_event` SET `procPhase` = 2 WHERE `entry` = 74396;UPDATE `gameobject_template` SET `data9` = 0 WHERE `entry` = 141979;UPDATE `gameobject_template` SET `data9` = 0 WHERE `entry` IN (1673,1765,2039,2560,2714,2716,2717,2718,2844,2845,2847,3642,3658,3660,3661,3689,3703,3704,4095,19019,19020);DELETE FROM `trinity_string` WHERE `entry` = 11040;
INSERT INTO `trinity_string`(`entry`, `content_default`) VALUES (11040, 'You must be at least level %u in order to invite to raid group.');-- DB/NPC: Farseer Faedris - remove additional spawn https://github.com/SunwellTracker/issues/issues/2551
-- DB/Loot: Uncured Caribou Hide - slightly increase drop chance https://github.com/SunwellTracker/issues/issues/2547
-- DB/Quest: Battling the Elements - add additional spawns of Seething Revenants
-- DB/Quest: Boiling blood - slightly increase drop chance https://github.com/SunwellTracker/issues/issues/2557
-- DB/NPC: Heb'Drakkar Striker - should not fly dismounted https://github.com/SunwellTracker/issues/issues/2531
-- DB/Quest: The Last Line Of Defense - turret's spells should not target players https://github.com/SunwellTracker/issues/issues/2558
DELETE FROM `creature` WHERE `guid` = 125031 AND `id` = 32250;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -37 WHERE `entry` = 25680 AND `item` = 35288;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -30 WHERE `entry` = 25750 AND `item` = 35288;
DELETE FROM `creature` WHERE `id` = 30120 AND `guid` IN (303308, 303309, 303310, 303311,303312);
INSERT INTO `creature` VALUES 
(303308, 30120, 571, 1, 4, 0, 0, 7238.28, -3617.2, 823.124, 4.48637, 300, 0, 0, 12600, 0, 0, 0, 0, 0),
(303309, 30120, 571, 1, 4, 0, 0, 7199.95, -3644.77, 823.605, 0.681117, 300, 0, 0, 12600, 0, 0, 0, 0, 0),
(303310, 30120, 571, 1, 4, 0, 0, 7238.47, -3694.46, 824.055, 0.300193, 300, 0, 0, 12600, 0, 0, 0, 0, 0),
(303311, 30120, 571, 1, 4, 0, 0, 7249.85, -3669.73, 823.013, 3.30042, 300, 0, 0, 12600, 0, 0, 0, 0, 0),
(303312, 30120, 571, 1, 4, 0, 0, 7219.73, -3689.90, 822.369, 2.01, 300, 0, 0, 12600, 0, 0, 0, 0, 0);

UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -30 WHERE `entry` = 19442 AND `item` = 30425;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -22 WHERE `entry` = 19424 AND `item` = 30425;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -22 WHERE `entry` = 16907 AND `item` = 30425;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -22 WHERE `entry` = 16871 AND `item` = 30425;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -22 WHERE `entry` = 16873 AND `item` = 30425;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -22 WHERE `entry` = 19422 AND `item` = 30425;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -15 WHERE `entry` = 19457 AND `item` = 30425;

UPDATE `creature_template` SET `InhabitType` = 3 WHERE `entry` = 28465;

DELETE FROM `conditions` WHERE `SourceEntry` = 57542;
INSERT INTO `conditions` VALUES 
(13, 1, 57542, 0, 0, 31, 0, 4, 0, 0, 1, 0, 0, '', 'Argent Cannon Assault should not target players');

DELETE FROM `conditions` WHERE `SourceEntry` = 57415;
INSERT INTO `conditions` VALUES 
(13, 1, 57415, 0, 0, 31, 0, 4, 0, 0, 1, 0, 0, '', 'The Reckoning should not target players');
UPDATE `creature` SET `spawntimesecs` = 30 WHERE `id` IN (29061, 29071, 29072,29067,29074,29070,29032,29065,29068,29073);
UPDATE `creature_template` SET AIName = 'SmartAI' WHERE `entry` = 31099;
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `id` = 31099;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 31099 AND `source_type` = 0;UPDATE `creature` SET `spawntimesecs` = 30 WHERE `id` = 29639;UPDATE `item_loot_template` SET `ChanceOrQuestChance`=100 WHERE  `entry`=35792 AND `item`=35783 AND `lootmode`=1;-- DB/NPC: DK starting zone dialogues
UPDATE `creature_text` SET `text` = "Impressive, death knight. Return to me in the world of the living for your reward." WHERE `entry` = 28788;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29032;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29061;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29065;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29067;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29068;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29070;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29074;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29072;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29073;
UPDATE `creature_text` SET `text` = 'Think, $N. Think back. Try and remember the hills and valleys of Elwynn, where you were born. Remember the splendor of life, $Gbrother:sister;. You were a champion of the Alliance once! This isn\'t you.' WHERE `groupid` = 33 AND `entry` = 29071;
UPDATE `creature_text` SET `text` = 'Argh...what...is...' WHERE `groupid` = 33 AND `entry` =  29173;
UPDATE `creature_text` SET `text` = 'That\'s enough; we must move again. Mal\'Ganis awaits.' WHERE `groupid` = 33 AND `entry` = 26499;
UPDATE `creature_text` SET `text` = 'Alas, the only way to defeat the Lich King is to destroy him at the place he was created.' WHERE `groupid` = 33 AND `entry` = 37225;
UPDATE `creature_text` SET `text` = 'You have reached Rank 2: First Lieutenant' WHERE `groupid` = 33 AND `entry` = 15214;
UPDATE `creature_text` SET `text` = 'Corruption found. External influence gaining control over watchers. Sigils compromised.' WHERE `groupid` = 33 AND `entry` = 33874;
UPDATE `creature_text` SET `text` = 'The cunning, powerful eredar warlocks were once our noble kinsmen.' WHERE `groupid` = 33 AND `entry` = 18896;
UPDATE `creature_text` SET `text` = 'Have you ever noticed that all those cute, orc kids look the same?' WHERE `groupid` = 33 AND `entry` = 19228;
UPDATE `creature_text` SET `text` = 'The East Frostwolf Tower is under attack!  If left unchecked, the Horde will destroy it!' WHERE `groupid` = 33 AND `entry` = 14848;
UPDATE `creature_text` SET `text` = '%s begins to unleash a toxic poison cloud!' WHERE `groupid` = 33 AND `entry` = 36476;
UPDATE `creature_text` SET `text` = 'You stumble about in darkness. There is no light here. No mercy. Icecrown has claimed the souls of better heroes than you.' WHERE `groupid` = 33 AND `entry` = 37181;
UPDATE `creature_text` SET `text` = 'You spoiled my grand enterance, rat.' WHERE `groupid` = 33 AND `entry` = 35451;
UPDATE `creature_text` SET `text` = 'It\'s good to have you back, Varok, old friend. I\'m sorry about your boy.' WHERE `groupid` = 33 AND `entry` = 31650;
UPDATE `creature_template` SET AIName = 'SmartAI' WHERE `entry` = 31099;
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `id` = 31099;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 31099 AND `source_type` = 0;
INSERT INTO `smart_scripts`(`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES
(31099, 0, 1, 6, 41, 30000, 1, 'Patchwerk - Just Died - Despawn after 30s');UPDATE `smart_scripts` SET `link` = 7 WHERE `entryorguid` = 25584 AND `source_type` = 0 AND `id` = 4;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 25584 AND `source_type` = 0 AND `id` = 7;
INSERT INTO `smart_scripts`(`entryorguid`, `source_type`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES
(25584, 0, 7, 61, 41, 30000, 1, 'Inquisitor Salrand - Just Died - Despawn after 30s');
UPDATE `creature` SET `spawntimesecs` = 30 WHERE `id` = 25584;UPDATE `gameobject_template` SET `data9` = 0 WHERE `entry` = 169243;-- DB/NPC: Pyrewood Elder - remove furnace spawn
DELETE FROM `creature` WHERE `guid` = 17663 AND `id` = 1895;DELETE FROM `conditions` WHERE `SourceEntry`=32037 AND `SourceTypeOrReferenceId`=17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 32037, 0, 0, 29, 0, 18185, 100, 0, 1, 0, 0, "", "Spell 'Place Feralfen Totem' cannot be cast if NPC 'Feralden Serpent Spirit' is nearby"),
(17, 0, 32037, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, "", "Spell 'Place Feralfen Totem' cannot be cast if nearby NPC 'Feralden Serpent Spirit' is alive");SET @CGUID:= 17663;

DELETE FROM `creature` WHERE `guid` = @CGUID;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelId`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`) VALUES
(@CGUID, 6491, 571, 1, 1, 0, 0, 4105.36, -4818.6, 76.074, 0.149361, 7200, 0);
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 5 WHERE `entry` = 25084 AND item = 4 AND mincountOrRef = -24023;
-- DB/NPC: Stoic Mammoth - update loottable https://github.com/SunwellTracker/issues/issues/2614
DELETE FROM `creature_loot_template` WHERE `entry` = 30260 AND `item` = 1;
INSERT INTO `creature_loot_template` VALUES
(30260, 1, 33.9, 1, 0, -26011, 1);
-- [Quest] Water Terror - Water Terror's spells should not target players #2617 https://github.com/SunwellTracker/issues/issues/2617
DELETE FROM `conditions` WHERE `SourceEntry` = 57652;
INSERT INTO `conditions` VALUES 
(13, 2, 57652, 0, 0, 31, 0, 4, 0, 0, 1, 0, 0, '', 'Water Terrors spells should not target players'),
(13, 4, 57652, 0, 0, 31, 0, 4, 0, 0, 1, 0, 0, '', 'Water Terrors spells should not target players');
DELETE FROM `conditions` WHERE `SourceEntry` = 57668;
INSERT INTO `conditions` VALUES 
(13, 2, 57668, 0, 0, 31, 0, 4, 0, 0, 1, 0, 0, '', 'Water Terrors spells should not target players'),
(13, 1, 57668, 0, 0, 31, 0, 4, 0, 0, 1, 0, 0, '', 'Water Terrors spells should not target players');
DELETE FROM `conditions` WHERE `SourceEntry` = 57665;
INSERT INTO `conditions` VALUES 
(17, 0, 57665, 0, 0, 31, 1, 4, 0, 0, 1, 0, 0, '', 'Water Terrors spells should not target players');
DELETE FROM `creature_loot_template` WHERE entry IN (30451,30452,30449, 31535, 31520, 31534);
INSERT INTO `creature_loot_template` VALUES
(30451, 	40752, 100, 1, 0, 1, 1),
(30452, 	40752, 100, 1, 0, 1, 1),
(30449, 	40752, 100, 1, 0, 1, 1),
(31535, 	40753, 100, 1, 0, 1, 1),
(31520, 	40753, 100, 1, 0, 1, 1),
(31534, 	40753, 100, 1, 0, 1, 1);DELETE FROM `achievement_reward` WHERE entry IN (2137, 2138);
INSERT INTO `achievement_reward` VALUES 
(2137, 0, 0, 44175, 4949, 'Glory of the Rider', 'Dear $N,$B$BI hope ye\'re doing well and that ye\'ve had time to recover from our adventures in Naxxramas.$B$BWe\'ve managed to capture hatchling of plagued drake.  ... We thought perhaps you\'d accept him as a gift.$B$BMay the Elemnts guide you$BThrall, son of Durotan', 0),
(2138, 0, 0, 44164, 32346, 'Glory of the Rider', 'Dear $N,$B$BI hope ye\'re doing well and that ye\'ve had time to recover from our adventures in Naxxramas.$B$BWe\'ve managed to capture hatchling of black drake.  ... We thought perhaps you\'d accept him as a gift.$B$BGood luck,$BJaina Proudmoore', 0);
UPDATE `gameobject` SET `spawntimesecs` = 86400 WHERE `id` = 153464;
UPDATE item_template i SET i.RequiredDisenchantSkill = 0 WHERE i.entry IN (30570,30568, 30597, 30599);DELETE FROM `conditions` WHERE `SourceEntry`=45942 AND `SourceTypeOrReferenceId`=17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 45942, 0, 0, 29, 0, 25829, 100, 0, 1, 0, 0, "", "Spell 'Summon Alluvius' cannot be cast if NPC 'Lord Kryxix' is nearby"),
(17, 0, 45942, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, "", "Spell 'Summon Alluvius' cannot be cast if nearby NPC 'Lord Kryxix' is alive"),
(17, 0, 45942, 0, 0, 29, 0, 25742, 100, 0, 1, 0, 0, "", "Spell 'Summon Alluvius' cannot be cast if NPC 'Alluvius' is nearby");-- DB/NPC: Theramore guards - correct loottables
UPDATE `creature_template` SET `lootid` = 0 WHERE `entry` = 4979;-- DB/NPC: Mjordin Combatant - frostweave cloth drop 
-- DB/Go: Increase number of thorium spawns https://github.com/SunwellTracker/issues/issues/1636 https://github.com/SunwellTracker/issues/issues/2603
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 35 WHERE `entry` = 30037 AND `item` = 26040;
UPDATE `reference_loot_template` SET `maxcount` = 4 WHERE `entry` = 26040 AND `item` = 33470;

UPDATE `pool_template` SET `max_limit` = 35 WHERE `entry` = 1708;
UPDATE `pool_template` SET `max_limit` = 55 WHERE `entry` = 1709;
UPDATE `pool_template` SET `max_limit` = 43 WHERE `entry` = 1722;
UPDATE `pool_template` SET `max_limit` = 25 WHERE `entry` = 1723;
UPDATE `pool_template` SET `max_limit` = 30 WHERE `entry` = 1726;
UPDATE `pool_template` SET `max_limit` = 40 WHERE `entry` = 9916;
UPDATE `smart_scripts` SET `event_flags`='2' WHERE  `entryorguid`=16506 AND `source_type`=0 AND `id`=2 AND `link`=0;UPDATE creature_template SET `flags_extra` = `flags_extra` | 0x80000000 WHERE `entry` IN (29062, 29064, 29063, 29097, 29096, 29098);
DELETE FROM `holiday_dates` WHERE `id` = 141 AND `date_id` = 1;
INSERT INTO `holiday_dates` VALUES (141, 1, 330547840, 0);-- DB/GO: Fix Tin Vein floating above the ground
-- DB/Item: Froststeel Lockbox - fix loot
UPDATE `gameobject` SET `position_z` = 94.73 WHERE `guid` = 5649;

DELETE FROM `item_loot_template` WHERE `entry` = 43622;
INSERT INTO `item_loot_template` VALUES
(43622, 1, 100, 1, 0, -1567, 2);
DELETE FROM `reference_loot_template` WHERE `entry` =1567;
INSERT INTO `reference_loot_template` VALUES
(1567, 35979, 0.5, 1, 1, 1, 1),
(1567, 35980, 0.5, 1, 1, 1, 1),
(1567, 35981, 0.5, 1, 1, 1, 1),
(1567, 35982, 0.5, 1, 1, 1, 1),
(1567, 35983, 0.5, 1, 1, 1, 1),
(1567, 35984, 0.5, 1, 1, 1, 1),
(1567, 35985, 0.5, 1, 1, 1, 1),
(1567, 35986, 0.5, 1, 1, 1, 1),
(1567, 35987, 0.5, 1, 1, 1, 1),
(1567, 35988, 0.5, 1, 1, 1, 1),
(1567, 35989, 0.5, 1, 1, 1, 1),
(1567, 35990, 0.5, 1, 1, 1, 1),
(1567, 35991, 0.5, 1, 1, 1, 1),
(1567, 35992, 0.5, 1, 1, 1, 1),
(1567, 35993, 0.5, 1, 1, 1, 1),
(1567, 35994, 0.5, 1, 1, 1, 1),
(1567, 35995, 0.5, 1, 1, 1, 1),
(1567, 35996, 0.5, 1, 1, 1, 1),
(1567, 35997, 0.5, 1, 1, 1, 1),
(1567, 35998, 0.5, 1, 1, 1, 1),
(1567, 35999, 0.5, 1, 1, 1, 1),
(1567, 36000, 0.5, 1, 1, 1, 1),
(1567, 36001, 0.5, 1, 1, 1, 1),
(1567, 36002, 0.5, 1, 1, 1, 1),
(1567, 36091, 0.5, 1, 1, 1, 1),
(1567, 36092, 0.5, 1, 1, 1, 1),
(1567, 36093, 0.5, 1, 1, 1, 1),
(1567, 36094, 0.5, 1, 1, 1, 1),
(1567, 36095, 0.5, 1, 1, 1, 1),
(1567, 36096, 0.5, 1, 1, 1, 1),
(1567, 36097, 0.5, 1, 1, 1, 1),
(1567, 36098, 0.5, 1, 1, 1, 1),
(1567, 36099, 0.5, 1, 1, 1, 1),
(1567, 36100, 0.5, 1, 1, 1, 1),
(1567, 36101, 0.5, 1, 1, 1, 1),
(1567, 36102, 0.5, 1, 1, 1, 1),
(1567, 36103, 0.5, 1, 1, 1, 1),
(1567, 36104, 0.5, 1, 1, 1, 1),
(1567, 36105, 0.5, 1, 1, 1, 1),
(1567, 36106, 0.5, 1, 1, 1, 1),
(1567, 36107, 0.5, 1, 1, 1, 1),
(1567, 36108, 0.5, 1, 1, 1, 1),
(1567, 36109, 0.5, 1, 1, 1, 1),
(1567, 36110, 0.5, 1, 1, 1, 1),
(1567, 36111, 0.5, 1, 1, 1, 1),
(1567, 36112, 0.5, 1, 1, 1, 1),
(1567, 36113, 0.5, 1, 1, 1, 1),
(1567, 36114, 0.5, 1, 1, 1, 1),
(1567, 36203, 0.5, 1, 1, 1, 1),
(1567, 36204, 0.5, 1, 1, 1, 1),
(1567, 36205, 0.5, 1, 1, 1, 1),
(1567, 36206, 0.5, 1, 1, 1, 1),
(1567, 36207, 0.5, 1, 1, 1, 1),
(1567, 36208, 0.5, 1, 1, 1, 1),
(1567, 36209, 0.5, 1, 1, 1, 1),
(1567, 36210, 0.5, 1, 1, 1, 1),
(1567, 36211, 0.5, 1, 1, 1, 1),
(1567, 36212, 0.5, 1, 1, 1, 1),
(1567, 36213, 0.5, 1, 1, 1, 1),
(1567, 36214, 0.5, 1, 1, 1, 1),
(1567, 36215, 0.5, 1, 1, 1, 1),
(1567, 36216, 0.5, 1, 1, 1, 1),
(1567, 36217, 0.5, 1, 1, 1, 1),
(1567, 36218, 0.5, 1, 1, 1, 1),
(1567, 36219, 0.5, 1, 1, 1, 1),
(1567, 36220, 0.5, 1, 1, 1, 1),
(1567, 36221, 0.5, 1, 1, 1, 1),
(1567, 36222, 0.5, 1, 1, 1, 1),
(1567, 36223, 0.5, 1, 1, 1, 1),
(1567, 36224, 0.5, 1, 1, 1, 1),
(1567, 36225, 0.5, 1, 1, 1, 1),
(1567, 36226, 0.5, 1, 1, 1, 1),
(1567, 36315, 0.5, 1, 1, 1, 1),
(1567, 36316, 0.5, 1, 1, 1, 1),
(1567, 36317, 0.5, 1, 1, 1, 1),
(1567, 36318, 0.5, 1, 1, 1, 1),
(1567, 36319, 0.5, 1, 1, 1, 1),
(1567, 36320, 0.5, 1, 1, 1, 1),
(1567, 36321, 0.5, 1, 1, 1, 1),
(1567, 36322, 0.5, 1, 1, 1, 1),
(1567, 36323, 0.5, 1, 1, 1, 1),
(1567, 36324, 0.5, 1, 1, 1, 1),
(1567, 36325, 0.5, 1, 1, 1, 1),
(1567, 36326, 0.5, 1, 1, 1, 1),
(1567, 36327, 0.5, 1, 1, 1, 1),
(1567, 36328, 0.5, 1, 1, 1, 1),
(1567, 36329, 0.5, 1, 1, 1, 1),
(1567, 36330, 0.5, 1, 1, 1, 1),
(1567, 36331, 0.5, 1, 1, 1, 1),
(1567, 36332, 0.5, 1, 1, 1, 1),
(1567, 36333, 0.5, 1, 1, 1, 1),
(1567, 36334, 0.5, 1, 1, 1, 1),
(1567, 36335, 0.5, 1, 1, 1, 1),
(1567, 36336, 0.5, 1, 1, 1, 1),
(1567, 36337, 0.5, 1, 1, 1, 1),
(1567, 36338, 0.5, 1, 1, 1, 1),
(1567, 36406, 0.5, 1, 1, 1, 1),
(1567, 36407, 0.5, 1, 1, 1, 1),
(1567, 36408, 0.5, 1, 1, 1, 1),
(1567, 36420, 0.5, 1, 1, 1, 1),
(1567, 36421, 0.5, 1, 1, 1, 1),
(1567, 36422, 0.5, 1, 1, 1, 1),
(1567, 36434, 0.5, 1, 1, 1, 1),
(1567, 36435, 0.5, 1, 1, 1, 1),
(1567, 36436, 0.5, 1, 1, 1, 1),
(1567, 36448, 0.5, 1, 1, 1, 1),
(1567, 36449, 0.5, 1, 1, 1, 1),
(1567, 36450, 0.5, 1, 1, 1, 1),
(1567, 36462, 0.5, 1, 1, 1, 1),
(1567, 36463, 0.5, 1, 1, 1, 1),
(1567, 36464, 0.5, 1, 1, 1, 1),
(1567, 36476, 0.5, 1, 1, 1, 1),
(1567, 36478, 0.5, 1, 1, 1, 1),
(1567, 36490, 0.5, 1, 1, 1, 1),
(1567, 36492, 0.5, 1, 1, 1, 1),
(1567, 36506, 0.5, 1, 1, 1, 1),
(1567, 36518, 0.5, 1, 1, 1, 1),
(1567, 36520, 0.5, 1, 1, 1, 1),
(1567, 36532, 0.5, 1, 1, 1, 1),
(1567, 36534, 0.5, 1, 1, 1, 1),
(1567, 36546, 0.5, 1, 1, 1, 1),
(1567, 36548, 0.5, 1, 1, 1, 1),
(1567, 36560, 0.5, 1, 1, 1, 1),
(1567, 36562, 0.5, 1, 1, 1, 1),
(1567, 36574, 0.5, 1, 1, 1, 1),
(1567, 36576, 0.5, 1, 1, 1, 1),
(1567, 36588, 0.5, 1, 1, 1, 1),
(1567, 36590, 0.5, 1, 1, 1, 1),
(1567, 36604, 0.5, 1, 1, 1, 1),
(1567, 36616, 0.5, 1, 1, 1, 1),
(1567, 36618, 0.5, 1, 1, 1, 1),
(1567, 36630, 0.5, 1, 1, 1, 1),
(1567, 36644, 0.5, 1, 1, 1, 1),
(1567, 36658, 0.5, 1, 1, 1, 1),
(1567, 36660, 0.5, 1, 1, 1, 1),
(1567, 36672, 0.5, 1, 1, 1, 1),
(1567, 36674, 0.5, 1, 1, 1, 1),
(1567, 36686, 0.5, 1, 1, 1, 1),
(1567, 36688, 0.5, 1, 1, 1, 1),
(1567, 36700, 0.5, 1, 1, 1, 1),
(1567, 36702, 0.5, 1, 1, 1, 1),
(1567, 36714, 0.5, 1, 1, 1, 1),
(1567, 37754, 0.5, 1, 1, 1, 1),
(1567, 37755, 0.5, 1, 1, 1, 1),
(1567, 37756, 0.5, 1, 1, 1, 1),
(1567, 37764, 0.5, 1, 1, 1, 1),
(1567, 37775, 0.5, 1, 1, 1, 1),
(1567, 37776, 0.5, 1, 1, 1, 1),
(1567, 37785, 0.5, 1, 1, 1, 1),
(1567, 37795, 0.5, 1, 1, 1, 1),
(1567, 37796, 0.5, 1, 1, 1, 1),
(1567, 37802, 0.5, 1, 1, 1, 1),
(1567, 37820, 0.5, 1, 1, 1, 1),
(1567, 37821, 0.5, 1, 1, 1, 1);
UPDATE `creature_template` SET `InhabitType` = 4, `flags_extra` = `flags_extra` | 130 WHERE `entry` = 23361;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_captain_skyshatter" WHERE `entry` = 23348;
DELETE FROM `script_waypoint` WHERE `entry` = 23348;
INSERT INTO `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES 
(23348, 1, -5095, 631, 87, 0, NULL),
(23348, 2, -5098, 663, 88, 0, NULL),
(23348, 3, -5083, 664, 89, 0, NULL),
(23348, 4, -5002, 664, 90, 0, NULL),
(23348, 5, -4988, 663, 89, 0, NULL),
(23348, 6, -4905, 687, 96, 0, NULL),
(23348, 7, -4881, 685, 89, 0, NULL),
(23348, 8, -4868, 665, 86, 0, NULL),
(23348, 9, -4894, 643, 96, 0, NULL),
(23348, 10, -4995, 662, 92, 0, NULL),
(23348, 11, -5050, 664, 92, 0, NULL),
(23348, 12, -5198, 686, 122, 0, NULL),
(23348, 13, -5286, 674, 102, 0, NULL),
(23348, 14, -5277, 501, 83, 0, NULL),
(23348, 15, -5291, 385, 68, 0, NULL),
(23348, 16, -5218, 444, 83, 0, NULL),
(23348, 17, -5276, 342, 91, 0, NULL),
(23348, 18, -5350, 175, 67, 0, NULL),
(23348, 19, -5263, 28, 70, 0, NULL),
(23348, 20, -5179, -84, 115, 0, NULL),
(23348, 21, -5100, -125, 94, 0, NULL),
(23348, 22, -4933, -84, 132, 0, NULL),
(23348, 23, -4915, 23, 114, 0, NULL),
(23348, 24, -5022, 41, 112, 0, NULL),
(23348, 25, -5167, 76, 114, 0, NULL),
(23348, 26, -5210, 191, 104, 0, NULL),
(23348, 27, -5186, 362, 102, 0, NULL),
(23348, 28, -5191, 432, 99, 0, NULL),
(23348, 29, -5140, 570, 111, 0, NULL),
(23348, 30, -5046, 659, 126, 0, NULL),
(23348, 31, -4944, 646, 218, 0, NULL),
(23348, 32, -5003, 537, 195, 0, NULL),
(23348, 33, -5110, 462, 264, 0, NULL),
(23348, 34, -5052, 248, 225, 0, NULL),
(23348, 35, -4954, 212, 194, 0, NULL),
(23348, 36, -5088, 187, 177, 0, NULL),
(23348, 37, -5097, 261, 191, 0, NULL),
(23348, 38, -5080, 340, 203, 0, NULL),
(23348, 39, -4999, 409, 199, 0, NULL),
(23348, 40, -4967, 520, 166, 0, NULL),
(23348, 41, -4969, 441, 154, 0, NULL),
(23348, 42, -4955, 619, 114, 0, NULL),
(23348, 43, -4994, 662, 94, 0, NULL),
(23348, 44, -5083, 664, 88, 0, NULL),
(23348, 45, -5098, 663, 88, 0, NULL),
(23348, 46, -5095, 631, 87, 0, NULL),
(23348, 47, -5074, 622, 86, 0, NULL);

DELETE FROM `creature_text` WHERE `entry` = 23348;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment` ) VALUES 
(23348, 0, 0, 'Prepare a funeral pyre! $N has challenged Skyshatter!', 14, 0, 0, 0, 0, 0, 'Dragonmaw Race: Skyshatter'),
(23348, 1, 0, 'I... I am undone... The new top orc is $N!', 14, 0, 0, 0, 0, 0, 'Dragonmaw Race: Skyshatter'),
(23348, 2, 0, 'I weep for you, $N. You really have no idea what you\'ve gotten yourself into...', 12, 0, 0, 0, 0, 0, 'Dragonmaw Race: Skyshatter');
UPDATE `gameobject` SET `spawntimesecs` = 604800 WHERE `guid` IN (279401, 279400);
UPDATE `gameobject` SET `spawntimesecs` = 86400 WHERE `guid` IN
(151790,
65564,
279398,
279399,
269105,
269104);
-- DB/Loot: Howling Wind - fix ( increase ) drop chance https://github.com/SunwellTracker/issues/issues/2690
-- DB/GO: fix spawn position of some herbs in Western Plaguelands ( Azathoth's report )
-- DB/Loot: Smokywood Pastures Gift Pack should always drop something ( Tide's report )
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 20 WHERE `item` = 24504;
UPDATE `gameobject` SET `position_z` = 55.42 WHERE `guid` = 16022;
UPDATE `item_loot_template` SET `ChanceOrQuestChance` = 0, `groupid` = 1 WHERE `entry` = 17727;UPDATE `spell_proc_event` SET `procFlags` = 87316, `ppmRate` = 3.5, `Cooldown` = 1 WHERE entry = 16864;
UPDATE spell_proc_event SET SpellFamilyMask1 = SpellFamilyMask1 | 32 WHERE entry = -14892;
DELETE FROM event_scripts WHERE id = 9104;

SET @GAHZRANKAPATHID:= 15114 * 10;
DELETE FROM `waypoint_data` WHERE `id` = @GAHZRANKAPATHID;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`) VALUES
(@GAHZRANKAPATHID, 1, -11735.989258, -1797.229858, -5.787302),
(@GAHZRANKAPATHID, 2, -11723.325195, -1772.784180, 5.798630),
(@GAHZRANKAPATHID, 3, -11707.074219, -1748.310425, 8.365822);
-- DB/NPC: Remove duplicate Yulda the Stormspeaker spawn https://github.com/SunwellTracker/issues/issues/2769
-- DB/NPC: Remove duplicate Snowblind Digger spawns https://github.com/SunwellTracker/issues/issues/2768
-- Core/Quest: A Delicate Touch https://github.com/SunwellTracker/issues/issues/2765
-- DB/GO: Arthas Tears - fix floating spawn https://github.com/SunwellTracker/issues/issues/2749
-- DB/Quest: The Scarlet Crusade - Scarlet Armbands should drop only while on quest https://github.com/SunwellTracker/issues/issues/2733
-- DB/Loot: Adjust gold drops from Karazhan mobs https://github.com/SunwellTracker/issues/issues/2717
-- DB/NPC: Add Dark Iron Ambassador rare spawn to Gnomeregan https://github.com/SunwellTracker/issues/issues/2708
DELETE FROM `creature` WHERE `guid` = 104495 AND `id` = 30046;
DELETE FROM `creature` WHERE `guid` IN (209173,209176,209177,209174,209171) AND `id` = 29413;
UPDATE `creature_template` SET `ScriptName` = "npc_improved_land_mine", `AIName` = "" WHERE `entry` = 29475;
UPDATE `gameobject` SET `position_z` = 59.66 WHERE `guid` = 15968;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -100 WHERE `item` = 3266;
UPDATE `creature_template` SET `mingold` = 1519,  `maxgold` = 1988 WHERE `entry` = 15551;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 15687;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 15688;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 15689;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 15690;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 15691;
UPDATE `creature_template` SET `mingold` = 1901,  `maxgold` = 2487 WHERE `entry` = 16389;
UPDATE `creature_template` SET `mingold` = 1605,  `maxgold` = 2101 WHERE `entry` = 16406;
UPDATE `creature_template` SET `mingold` = 1942,  `maxgold` = 2541 WHERE `entry` = 16407;
UPDATE `creature_template` SET `mingold` = 2967,  `maxgold` = 3879 WHERE `entry` = 16408;
UPDATE `creature_template` SET `mingold` = 290,  `maxgold` = 384 WHERE `entry` = 16409;
UPDATE `creature_template` SET `mingold` = 2669,  `maxgold` = 3491 WHERE `entry` = 16410;
UPDATE `creature_template` SET `mingold` = 1945,  `maxgold` = 2545 WHERE `entry` = 16411;
UPDATE `creature_template` SET `mingold` = 1988,  `maxgold` = 2601 WHERE `entry` = 16412;
UPDATE `creature_template` SET `mingold` = 2540,  `maxgold` = 3322 WHERE `entry` = 16414;
UPDATE `creature_template` SET `mingold` = 1945,  `maxgold` = 2545 WHERE `entry` = 16415;
UPDATE `creature_template` SET `mingold` = 1945,  `maxgold` = 2545 WHERE `entry` = 16424;
UPDATE `creature_template` SET `mingold` = 1945,  `maxgold` = 2545 WHERE `entry` = 16425;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 16457;
UPDATE `creature_template` SET `mingold` = 2540,  `maxgold` = 3322 WHERE `entry` = 16459;
UPDATE `creature_template` SET `mingold` = 2115,  `maxgold` = 2767 WHERE `entry` = 16460;
UPDATE `creature_template` SET `mingold` = 2583,  `maxgold` = 3378 WHERE `entry` = 16461;
UPDATE `creature_template` SET `mingold` = 354,  `maxgold` = 467 WHERE `entry` = 16468;
UPDATE `creature_template` SET `mingold` = 100000,  `maxgold` = 120000 WHERE `entry` = 16470;
UPDATE `creature_template` SET `mingold` = 3137,  `maxgold` = 4101 WHERE `entry` = 16471;
UPDATE `creature_template` SET `mingold` = 2880,  `maxgold` = 3766 WHERE `entry` = 16472;
UPDATE `creature_template` SET `mingold` = 2540,  `maxgold` = 3322 WHERE `entry` = 16473;
UPDATE `creature_template` SET `mingold` = 3264,  `maxgold` = 4268 WHERE `entry` = 16481;
UPDATE `creature_template` SET `mingold` = 2627,  `maxgold` = 3435 WHERE `entry` = 16482;
UPDATE `creature_template` SET `mingold` = 3859,  `maxgold` = 5045 WHERE `entry` = 16485;
UPDATE `creature_template` SET `mingold` = 5219,  `maxgold` = 6821 WHERE `entry` = 16504;
UPDATE `creature_template` SET `mingold` = 240000,  `maxgold` = 260000 WHERE `entry` = 16524;
UPDATE `creature_template` SET `mingold` = 1945,  `maxgold` = 2545 WHERE `entry` = 16525;
UPDATE `creature_template` SET `mingold` = 2117,  `maxgold` = 2769 WHERE `entry` = 16526;
UPDATE `creature_template` SET `mingold` = 839,  `maxgold` = 1100 WHERE `entry` = 16539;
UPDATE `creature_template` SET `mingold` = 1607,  `maxgold` = 2103 WHERE `entry` = 16540;
UPDATE `creature_template` SET `mingold` = 3264,  `maxgold` = 4268 WHERE `entry` = 16544;
UPDATE `creature_template` SET `mingold` = 2627,  `maxgold` = 3435 WHERE `entry` = 16545;
UPDATE `creature_template` SET `mingold` = 3264,  `maxgold` = 4268 WHERE `entry` = 16595;
UPDATE `creature_template` SET `mingold` = 5219,  `maxgold` = 6821 WHERE `entry` = 16596;

DELETE FROM `creature` WHERE `guid` = 183608;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES
(183608, 6228, 90, 1, 0, -1, -698.351, 559.918, -282.06, 6.03311, 36000, 0, 0, 4924, 2457, 2);

DELETE FROM `creature_addon` WHERE `guid` = 183608;
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(183608, 1836080, 0, 0, 0, 0, '');

DELETE FROM `waypoint_data` WHERE `id` = 1836080;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES 
(1836080, 1, -698.351, 559.918, -282.06, 0, 0, 0, 100, 0),
(1836080, 2, -736.867, 569.914, -291.06, 0, 0, 0, 100, 0),
(1836080, 3, -743.754, 581.403, -291.06, 0, 0, 0, 100, 0),
(1836080, 4, -734.431, 619.357, -300.06, 0, 0, 0, 100, 0),
(1836080, 5, -726.929, 649.513, -309.06, 0, 0, 0, 100, 0),
(1836080, 6, -717.134, 687.832, -318.06, 0, 0, 0, 100, 0),
(1836080, 7, -705.253, 694.829, -318.06, 0, 0, 0, 100, 0),
(1836080, 8, -717.059, 687.506, -318.06, 0, 0, 0, 100, 0),
(1836080, 9, -726.776, 650.171, -309.06, 0, 0, 0, 100, 0),
(1836080, 10, -734.225, 619.203, -300.06, 0, 0, 0, 100, 0),
(1836080, 11, -743.649, 581.74, -291.06, 0, 0, 0, 100, 0),
(1836080, 12, -736.919, 569.825, -291.06, 0, 0, 0, 100, 0);
DELETE FROM `creature_addon` WHERE `guid` IN
(6445, 
7326 ,
7348 ,
9609 ,
9752 ,
9771 ,
12358,
12361,
13744,
13760,
13766,
13789,
13795,
13808,
13816,
13831,
13833,
13835,
13851,
13855,
13857,
13866,
13871,
13886,
13905,
15864,
15865,
16884,
16897,
16909,
16935,
17313,
17751,
17754,
17910,
17949,
17962,
17975,
19196,
20338,
20343,
20368,
20396,
21117,
21121,
21123,
21124,
21588,
21589,
21590,
21777,
21812,
23314,
23332,
24815,
24819,
24828,
24919,
24920,
27620,
29140,
29301,
29316,
29351,
29380,
29385,
29881,
30652,
30709,
30859,
31276,
31552,
32599,
32600,
32649,
33034,
33035,
33081,
33082,
33087,
34225,
34226,
34228,
35574,
35583,
39676,
40166,
40172,
42360,
49166,
50040,
50041,
50046,
50427,
50428,
50431,
50483,
50485,
50508,
50528,
50530,
50549,
50553,
50561,
50565,
50606,
54600,
79388,
80744,
89766,
89768,
93039,
93040,
93693,
93700,
93703);


INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(6445, 0, 0, 3, 0, 0, '42648'),
(7326, 0, 0, 3, 0, 0, '42648'),
(7348, 0, 0, 3, 0, 0, '42648'),
(9609, 0, 0, 3, 4097, 0, '42648'),
(9752, 0, 0, 3, 0, 0, '42648'),
(9771, 0, 0, 3, 4097, 0, '42648'),
(12358, 0, 0, 3, 0, 0, '42648'),
(12361, 0, 0, 3, 0, 0, '42648'),
(13744, 0, 0, 3, 0, 0, '42648'),
(13760, 0, 0, 3, 4097, 0, '42648'),
(13766, 0, 0, 3, 4097, 0, '42648'),
(13789, 0, 0, 3, 4097, 0, '42648'),
(13795, 0, 0, 3, 0, 0, '42648'),
(13808, 0, 0, 3, 0, 0, '42648'),
(13816, 0, 0, 3, 0, 0, '42648'),
(13831, 0, 0, 3, 4097, 0, '42648'),
(13833, 0, 0, 3, 4097, 0, '42648'),
(13835, 0, 0, 3, 4097, 0, '42648'),
(13851, 0, 0, 3, 0, 0, '42648'),
(13855, 0, 0, 3, 0, 0, '42648'),
(13857, 0, 0, 3, 0, 0, '42648'),
(13866, 0, 0, 3, 4097, 0, '42648'),
(13871, 0, 0, 3, 4097, 0, '42648'),
(13886, 0, 0, 3, 4097, 0, '42648'),
(13905, 0, 0, 3, 4097, 0, '42648'),
(15864, 0, 0, 3, 4097, 0, '42648'),
(15865, 0, 0, 3, 4097, 0, '42648'),
(16884, 0, 0, 3, 0, 0, '42648'),
(16897, 0, 0, 3, 0, 0, '42648'),
(16909, 0, 0, 3, 0, 0, '42648'),
(16935, 0, 0, 3, 0, 0, '42648'),
(17313, 0, 0, 3, 4097, 0, '42648'),
(17751, 0, 0, 3, 0, 0, '42648'),
(17754, 0, 0, 3, 0, 0, '42648'),
(17910, 0, 0, 3, 4097, 0, '42648'),
(17949, 0, 0, 3, 4097, 0, '42648'),
(17962, 0, 0, 3, 4097, 0, '42648'),
(17975, 0, 0, 3, 4097, 0, '42648'),
(19196, 0, 0, 3, 4097, 0, '42648'),
(20338, 0, 0, 3, 4097, 0, '42648'),
(20343, 0, 0, 3, 0, 0, '42648'),
(20368, 0, 0, 3, 4097, 0, '42648'),
(20396, 0, 0, 3, 4097, 0, '42648'),
(21117, 0, 0, 3, 0, 0, '42648'),
(21121, 0, 0, 3, 0, 0, '42648'),
(21123, 0, 0, 3, 0, 0, '42648'),
(21124, 0, 0, 3, 0, 0, '42648'),
(21588, 0, 0, 3, 0, 0, '42648'),
(21589, 0, 0, 3, 0, 0, '42648'),
(21590, 0, 0, 3, 0, 0, '42648'),
(21777, 0, 0, 3, 0, 0, '42648'),
(21812, 0, 0, 3, 0, 0, '42648'),
(23314, 0, 0, 3, 4097, 0, '42648'),
(23332, 0, 0, 3, 4097, 0, '42648'),
(24815, 0, 0, 3, 0, 0, '42648'),
(24819, 0, 0, 3, 0, 0, '42648'),
(24828, 0, 0, 3, 4097, 0, '42648'),
(24919, 0, 0, 3, 0, 0, '42648'),
(24920, 0, 0, 3, 0, 0, '42648'),
(27620, 0, 0, 3, 4097, 27, '42648'),
(29140, 0, 0, 3, 4097, 0, '42648'),
(29301, 0, 0, 3, 4097, 0, '42648'),
(29316, 0, 0, 3, 4097, 0, '42648'),
(29351, 0, 0, 3, 0, 0, '42648'),
(29380, 0, 0, 3, 0, 0, '42648'),
(29385, 0, 0, 3, 0, 0, '42648'),
(29881, 0, 0, 3, 4097, 0, '42648'),
(30652, 0, 0, 3, 4097, 0, '42648'),
(30709, 0, 0, 3, 0, 0, '42648'),
(30859, 0, 0, 3, 4097, 0, '42648'),
(31276, 0, 0, 3, 0, 0, '42648'),
(31552, 0, 0, 3, 0, 0, '42648'),
(32599, 0, 0, 3, 4097, 0, '42648'),
(32600, 0, 0, 3, 4097, 0, '42648'),
(32649, 0, 0, 3, 4097, 0, '42648'),
(33034, 0, 0, 3, 0, 0, '42648'),
(33035, 0, 0, 3, 0, 0, '42648'),
(33081, 0, 0, 3, 0, 0, '42648'),
(33082, 0, 0, 3, 0, 0, '42648'),
(33087, 0, 0, 3, 0, 0, '42648'),
(34225, 0, 0, 3, 0, 0, '42648'),
(34226, 0, 0, 3, 0, 0, '42648'),
(34228, 0, 0, 3, 4097, 0, '42648'),
(35574, 0, 0, 3, 0, 0, '42648'),
(35583, 0, 0, 3, 0, 0, '42648'),
(39676, 0, 0, 3, 4097, 0, '42648'),
(40166, 0, 0, 3, 4097, 0, '42648'),
(40172, 0, 0, 3, 0, 0, '42648'),
(42360, 0, 0, 3, 4097, 0, '42648'),
(49166, 0, 0, 3, 0, 0, '42648'),
(50040, 0, 0, 3, 0, 0, '42648'),
(50041, 0, 0, 3, 0, 0, '42648'),
(50046, 0, 0, 3, 0, 0, '42648'),
(50427, 0, 0, 3, 0, 0, '42648'),
(50428, 0, 0, 3, 0, 0, '42648'),
(50431, 0, 0, 3, 0, 0, '42648'),
(50483, 0, 0, 3, 4097, 0, '42648'),
(50485, 0, 0, 3, 4097, 0, '42648'),
(50508, 0, 0, 3, 4097, 0, '42648'),
(50528, 0, 0, 3, 4097, 0, '42648'),
(50530, 0, 0, 3, 4097, 0, '42648'),
(50549, 0, 0, 3, 4097, 0, '42648'),
(50553, 0, 0, 3, 4097, 0, '42648'),
(50561, 0, 0, 3, 0, 0, '42648'),
(50565, 0, 0, 3, 0, 0, '42648'),
(50606, 0, 0, 3, 0, 0, '42648'),
(54600, 0, 0, 3, 0, 0, '42648'),
(79388, 0, 0, 3, 0, 0, '42648'),
(80744, 0, 0, 3, 0, 0, '42648'),
(89766, 0, 0, 3, 0, 0, '42648'),
(89768, 0, 0, 3, 0, 0, '42648'),
(93039, 0, 0, 3, 0, 0, '42648'),
(93040, 0, 0, 3, 0, 0, '42648'),
(93693, 0, 0, 3, 0, 0, '42648'),
(93700, 0, 0, 3, 0, 0, '42648'),
(93703, 0, 0, 3, 0, 0, '42648');
UPDATE `spell_proc_event` SET `cooldown` = 100 WHERE `entry` = 60524;
-- DB/Quest: Spread the Good Word - fix issue with vehicles not respawning https://github.com/SunwellTracker/issues/issues/2706
-- DB/Quest: Sleeping Giants - correct prequest to "Of Keys and Cages" https://github.com/SunwellTracker/issues/issues/2700
-- DB/Item: Add paladin shoulder pieces to Meminnie vendor https://github.com/SunwellTracker/issues/issues/2695
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 26523;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 26523;
INSERT INTO `smart_scripts` VALUES
(26523, 0, 0, 0, 28, 0, 100, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,0, 'On passanger removed - Despawn');

UPDATE `quest_template` SET `PrevQuestId` = 11231 WHERE `id` = 11432;

DELETE FROM `npc_vendor` WHERE `entry` = 19857 AND `item` IN (33699, 33726, 33753);
INSERT INTO `npc_vendor` VALUES
(19857, 0, 33699, 0, 0, 2288),
(19857, 0, 33726, 0, 0, 2288),
(19857, 0, 33753, 0, 0, 2288);
SET @GAHZRANKAPATHID:= 15114 * 10;
DELETE FROM `waypoint_data` WHERE `id` = @GAHZRANKAPATHID;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`) VALUES
(@GAHZRANKAPATHID, 1, -11679.807, -1696.884, -4.940),
(@GAHZRANKAPATHID, 2, -11678.780, -1713.134, 8.3364),
(@GAHZRANKAPATHID, 3, -11710.557, -1758.050, 8.3364);

UPDATE `spell_target_position` SET `target_position_x` = -11676.006, `target_position_y` = -1698.0500, `target_position_z` = 8.409842 WHERE id IN (24325, 24593);
DELETE FROM `spell_script_names` WHERE `spell_id` = 24325;
INSERT INTO `spell_script_names` VALUES
(24325, "spell_pagle_point_spawn_ghaz");
UPDATE `creature_template`
SET `flags_extra` = `flags_extra` | 1073741824
WHERE `entry` IN (16441, 16427, 16428, 16429, 30057, 30015, 30048, 30018);
UPDATE `spell_proc_event` SET `procEx` = 0, `Cooldown` = 1000 WHERE `entry` = 23688;
UPDATE `spell_proc_event` SET `procFlags`=1258492 WHERE  `entry`=57351;-- DB/Quest: Raising Hodir's Spear - update Stoic Mammoth Hide drop chance https://github.com/SunwellTracker/issues/issues/2787
-- DB/Loot: Firemane Flamecaller - correct loottable https://github.com/SunwellTracker/issues/issues/2734
-- DB/Quest: I Sense A Disturbance - small adjustment's to Kitten's path https://github.com/SunwellTracker/issues/issues/2775
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -29 WHERE `item` = 42542;

DELETE FROM `skinning_loot_template` WHERE `entry` = 4334;
INSERT INTO `skinning_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES
(4334, 1, 100, 1, 0, -4107, 1);

DELETE FROM `reference_loot_template` WHERE `entry` = 4107;
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES
(4107, 4234, 0, 1, 0, 1, 2),
(4107, 4235, 0, 1, 0, 1, 1),
(4107, 4304, 0, 1, 0, 1, 2),
(4107, 8165, 0, 1, 0, 1, 2),
(4107, 8169, 0, 1, 0, 1, 1);

UPDATE `smart_scripts` SET `event_param1` = 3000, `event_param2` = 3000 WHERE `entryorguid` = 2866500 AND `id` = 0;
UPDATE `smart_scripts` SET `event_param1` = 1000, `event_param2` = 1000 WHERE `entryorguid` = 2866500 AND `id` = 1;
 -- DB/NPC: Ghost Howl should be tameable - https://github.com/SunwellTracker/issues/issues/2793
-- DB/Quest: Blessing of Zim'Torga should be a repeatable quest - https://github.com/SunwellTracker/issues/issues/2794
-- DB/Quest: Bringing Down Heb'Jin - despawn bat after Heb Jin is defeated https://github.com/SunwellTracker/issues/issues/2773
UPDATE `creature_template` SET `type_flags` = 1, `family` = 1 WHERE `entry` = 3056;
UPDATE `quest_template` SET `SpecialFlags` = 1 WHERE `id` = 12618;
UPDATE `smart_scripts` SET `link` = 0 WHERE `entryorguid` = 28636 AND `id` = 10;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 28636 AND `id` = 11;
INSERT INTO `smart_scripts` VALUES
(28636, 0, 11, 0, 6, 0, 100, 0, 0, 0, 0, 0, 41, 500, 0, 0, 0, 0, 0, 19, 28639, 5, 0, 0, 0, 0, 0, 'Heb Jin - On Link(Landed) - Remove Bat\'s Immune to PC flag'); UPDATE `script_waypoint` SET `location_x`=-5049, `location_z`=92 WHERE `entry`=23348 AND `pointid`=11;
UPDATE `creature_template` SET `speed_run`='1.14286' WHERE `entry`=23348;-- DB/Quest: Iron Pommel - should drop from chest not slimes, fix issue with chest being not interactable https://github.com/SunwellTracker/issues/issues/2799
DELETE FROM `creature_loot_template` WHERE `item` = 5519;
UPDATE `gameobject_loot_template` SET `ChanceOrQuestChance` = -100 WHERE `item` = 5519 AND `entry` = 2971;
UPDATE `smart_scripts` SET `action_type` = 86, `action_param3` = 7 WHERE `entryorguid` = 3928 AND `id` = 1;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 179533 AND `source_type` = 1 AND `id` = 4;
INSERT INTO `smart_scripts` VALUES 
(179533,1,4,0,61,0,100,0,1,0,0,0,105,16,1,0,0,0,0,1,0,0,0,0,0,0,0,'Warpwood Pod - On Timed Event - Despawn');
UPDATE `smart_scripts` SET `link`=4 WHERE  `entryorguid`=179533 AND `source_type`=1 AND `id`=2;
DELETE FROM `holiday_dates` WHERE `id` = 423 AND `date_id` = 0;
INSERT INTO `holiday_dates` VALUES (423, 0, 336708352, 336); -- Add event to calendar
UPDATE `game_event` SET `start_time`='2018-02-08 12:00:00' WHERE `eventEntry`=8; -- Change game_event date
DELETE FROM `item_loot_template` WHERE  `entry`=54537 AND `item`=40753 AND `lootmode`=1; -- Delete Emblems from Heart-Shaped Box
DELETE FROM `reference_loot_template` WHERE `item` IN ( 51804, 51805, 51806, 51807, 51808); -- Delete 226 necks from event boss
DELETE FROM `holiday_dates` WHERE `id` = 327 AND `date_id` = 2;
INSERT INTO `holiday_dates` VALUES (327, 2, 335921792, 336);
UPDATE `game_event` SET `start_time`='2018-01-24 10:00:00' WHERE `eventEntry`=7;DELETE FROM `creature_onkill_reputation` WHERE `creature_id` IN (31480, 31461, 31462);UPDATE `creature` SET `spawndist` = 0 WHERE `guid` IN (13891,
13883,
59727,
59728,
59729,
59730,
59972,
59973,
59974,
68109,
68110,
131180,
249559,
249560,
249561,
249558,
150257,
150256,
150255,
150238,
150237,
150236,
249562,
249563,
150258,
150259);

UPDATE `creature` SET `spawndist` = 5 WHERE `guid` IN (1887665,5828276);
UPDATE `creature` SET `MovementType` = 0 WHERE `guid` IN (10083, 
33027,
125724,
125729,
125746,
125750,
125826,
125827,
125828,
125829,
125830,
125831,
125832,
125833,
 70854,
 70856,
 70858,
 70861,
 70862,
 70926,
 70927,
 72615,
 72622,
 72634,
 74047,
 77155,
 77163,
 77166,
 77168,
 77169);

DELETE FROM `pool_creature` WHERE `pool_entry` = 358;

DELETE FROM `creature_loot_template` WHERE `entry` = 24173;
INSERT INTO `creature_loot_template` VALUES 
(24173, 26000, 3, 1, 1, -26000, 1),
(24173, 26001, 3, 1, 1, -26001, 1),
(24173, 26004, 1, 1, 1, -26004, 1),
(24173, 26005, 1, 1, 1, -26005, 1),
(24173, 26006, 1, 1, 1, -26006, 1),
(24173, 33358, 0.1, 1, 0, 1, 1),
(24173, 33361, 0.1, 1, 0, 1, 1),
(24173, 33362, 0.1, 1, 0, 1, 1),
(24173, 33363, 0.1, 1, 0, 1, 1),
(24173, 33373, 0.1, 1, 0, 1, 1),
(24173, 33377, 0.1, 1, 0, 1, 1),
(24173, 33390, 0.1, 1, 0, 1, 1),
(24173, 33404, 0.3, 1, 0, 1, 1),
(24173, 33405, 0.1, 1, 0, 1, 1),
(24173, 33407, 0.1, 1, 0, 1, 1),
(24173, 33422, 0.1, 1, 0, 1, 1),
(24173, 33423, 0.3, 1, 0, 1, 1),
(24173, 33425, 0.1, 1, 0, 1, 1),
(24173, 33428, 0.1, 1, 0, 1, 1),
(24173, 33439, 0.1, 1, 0, 1, 1),
(24173, 33575, 81, 1, 0, 1, 1),
(24173, 33576, 19, 1, 0, 1, 1),
(24173, 35972, 0.1, 1, 0, 1, 1),
(24173, 35982, 0.1, 1, 0, 1, 1),
(24173, 36086, 0.1, 1, 0, 1, 1),
(24173, 36194, 2.7, 1, 0, 1, 1),
(24173, 36196, 0.1, 1, 0, 1, 1),
(24173, 36197, 0.1, 1, 0, 1, 1),
(24173, 36321, 0.1, 1, 0, 1, 1),
(24173, 36475, 0.1, 1, 0, 1, 1),
(24173, 36657, 0.1, 1, 0, 1, 1),
(24173, 36684, 0.1, 1, 0, 1, 1),
(24173, 43622, 0.1, 1, 0, 1, 1),
(24173, 45912, 0.1, 1, 0, 1, 1);

DELETE FROM `creature_loot_template` WHERE `entry` = 32260;
INSERT INTO `creature_loot_template` VALUES 
(32260, 5237, 0.9, 1, 0, 1, 1),
(32260, 26001, 3, 1, 1, -26001, 1),
(32260, 26002, 3, 1, 1, -26002, 1),
(32260, 26011, 1, 1, 1, -26011, 1),
(32260, 26012, 1, 1, 1, -26012, 1),
(32260, 26013, 1, 1, 1, -26013, 1),
(32260, 26014, 1, 1, 1, -26014, 1),
(32260, 26028, 0.5, 1, 1, -26028, 1),
(32260, 33365, 0.9, 1, 0, 1, 1),
(32260, 33368, 0.9, 1, 0, 1, 1),
(32260, 33369, 0.9, 1, 0, 1, 1),
(32260, 33370, 1.8, 1, 0, 1, 1),
(32260, 33371, 1.8, 1, 0, 1, 1),
(32260, 33379, 0.9, 1, 0, 1, 1),
(32260, 33381, 2.7, 1, 0, 1, 1),
(32260, 33382, 0.9, 1, 0, 1, 1),
(32260, 33384, 0.9, 1, 0, 1, 1),
(32260, 33385, 1.8, 1, 0, 1, 1),
(32260, 33397, 2.7, 1, 0, 1, 1),
(32260, 33399, 0.9, 1, 0, 1, 1),
(32260, 33400, 1.8, 1, 0, 1, 1),
(32260, 33401, 0.9, 1, 0, 1, 1),
(32260, 33402, 0.9, 1, 0, 1, 1),
(32260, 33403, 2.7, 1, 0, 1, 1),
(32260, 33412, 1.8, 1, 0, 1, 1),
(32260, 33414, 2.7, 1, 0, 1, 1),
(32260, 33415, 2.7, 1, 0, 1, 1),
(32260, 33416, 1.8, 1, 0, 1, 1),
(32260, 33417, 1.8, 1, 0, 1, 1),
(32260, 33419, 0.9, 1, 0, 1, 1),
(32260, 33422, 4.5, 1, 0, 1, 1),
(32260, 33424, 3.6, 1, 0, 1, 1),
(32260, 33425, 0.9, 1, 0, 1, 1),
(32260, 33426, 0.9, 1, 0, 1, 1),
(32260, 33427, 0.9, 1, 0, 1, 1),
(32260, 33428, 4.5, 1, 0, 1, 1),
(32260, 33429, 2.7, 1, 0, 1, 1),
(32260, 33430, 0.9, 1, 0, 1, 1),
(32260, 33431, 2.7, 1, 0, 1, 1),
(32260, 33433, 0.9, 1, 0, 1, 1),
(32260, 33434, 0.9, 1, 0, 1, 1),
(32260, 33436, 2.7, 1, 0, 1, 1),
(32260, 36019, 0.9, 1, 0, 1, 1),
(32260, 36020, 0.9, 1, 0, 1, 1),
(32260, 36025, 0.9, 1, 0, 1, 1),
(32260, 36027, 0.9, 1, 0, 1, 1),
(32260, 36030, 0.9, 1, 0, 1, 1),
(32260, 36036, 0.9, 1, 0, 1, 1),
(32260, 36040, 0.9, 1, 0, 1, 1),
(32260, 36048, 0.9, 1, 0, 1, 1),
(32260, 36137, 0.9, 1, 0, 1, 1),
(32260, 36141, 1.8, 1, 0, 1, 1),
(32260, 36143, 0.9, 1, 0, 1, 1),
(32260, 36150, 0.9, 1, 0, 1, 1),
(32260, 36152, 0.9, 1, 0, 1, 1),
(32260, 36153, 0.9, 1, 0, 1, 1),
(32260, 36248, 0.9, 1, 0, 1, 1),
(32260, 36260, 0.9, 1, 0, 1, 1),
(32260, 36263, 0.9, 1, 0, 1, 1),
(32260, 36269, 0.9, 1, 0, 1, 1),
(32260, 36271, 0.9, 1, 0, 1, 1),
(32260, 36355, 0.9, 1, 0, 1, 1),
(32260, 36361, 0.9, 1, 0, 1, 1),
(32260, 36364, 0.9, 1, 0, 1, 1),
(32260, 36374, 0.9, 1, 0, 1, 1),
(32260, 36375, 0.9, 1, 0, 1, 1),
(32260, 36413, 0.9, 1, 0, 1, 1),
(32260, 36425, 0.9, 1, 0, 1, 1),
(32260, 36426, 0.9, 1, 0, 1, 1),
(32260, 36440, 0.9, 1, 0, 1, 1),
(32260, 36441, 0.9, 1, 0, 1, 1),
(32260, 36469, 0.9, 1, 0, 1, 1),
(32260, 36511, 1.8, 1, 0, 1, 1),
(32260, 36524, 0.9, 1, 0, 1, 1),
(32260, 36565, 0.9, 1, 0, 1, 1),
(32260, 36568, 0.9, 1, 0, 1, 1),
(32260, 36580, 0.9, 1, 0, 1, 1),
(32260, 36666, 0.9, 1, 0, 1, 1),
(32260, 36679, 0.9, 1, 0, 1, 1),
(32260, 36694, 0.9, 1, 0, 1, 1),
(32260, 36708, 0.9, 1, 0, 1, 1),
(32260, 37761, 0.9, 1, 0, 1, 1),
(32260, 45912, 0.1, 1, 0, 1, 1);

UPDATE `creature_template` SET `lootid` = 4979 WHERE `entry` = 4979;
UPDATE `creature_template` SET `lootid` =16051 WHERE `entry` = 16051;
UPDATE `creature_template` SET `lootid` =24071 WHERE `entry` = 24071;
UPDATE `creature_template` SET `lootid` =24196 WHERE `entry` = 24196;
UPDATE `creature_template` SET `lootid` =30449 WHERE `entry` = 30449;
UPDATE `creature_template` SET `lootid` =30451 WHERE `entry` = 30451;
UPDATE `creature_template` SET `lootid` =30932 WHERE `entry` = 30932;
UPDATE `creature_template` SET `lootid` =30957 WHERE `entry` = 30957;
UPDATE `creature_template` SET `lootid` =30958 WHERE `entry` = 30958;
UPDATE `creature_template` SET `lootid` =31534 WHERE `entry` = 31534;
UPDATE `creature_template` SET `lootid` =31535 WHERE `entry` = 31535;

DELETE FROM `reference_loot_template` WHERE `entry` = 4107;
DELETE FROM `reference_loot_template` WHERE `entry` = 1002;
DELETE FROM `reference_loot_template` WHERE `entry` = 50019;

DELETE FROM `creature_formations` WHERE `memberGUID` = 80848;
DELETE FROM `creature_formations` WHERE `memberGUID` = 137557;

-- DB/Loot: Tears of bitter Anguish - slightly increase drop chance
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 0.1 WHERE `entry` = 29570 AND `item` = 43573;-- DB/Loot: Tears of bitter Anguish - slightly increase drop chance https://github.com/SunwellTracker/issues/issues/2815
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 1 WHERE `entry` = 29570 AND `item` = 43573;
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 2208 AND id = 1;
INSERT INTO `gossip_menu_option` VALUES
(2208, 1, 0, 'What plants are in Felwood that might be corrupted?', 1, 1, 0, 0, 0, 0, '');

DELETE FROM `gossip_menu_option` WHERE `menu_id` = 21400 AND `id` = 1;
INSERT INTO `gossip_menu_option` VALUES 
(21400, 1, 0, 'What plants are in Felwood that might be corrupted?', 1, 1, 0, 0, 0, 0, '');

DELETE FROM `conditions` WHERE `SourceEntry` = 54267 AND `ConditionValue3` = 102902;
DELETE FROM `conditions` WHERE `SourceEntry` = 54266 AND `ConditionValue3` = 102901;
DELETE FROM `conditions` WHERE `SourceEntry` = 54265 AND `ConditionValue3` = 102904;
DELETE FROM `conditions` WHERE `SourceEntry` = 54264 AND `ConditionValue3` = 102905;
DELETE FROM `conditions` WHERE `SourceEntry` = 54258 AND `ConditionValue3` = 102906;


-- DB/Quest: Escape Through Force - add post-quest event with Sentinel Selarin - based on Rushor\'s work https://github.com/SunwellTracker/issues/issues/2797
UPDATE `creature_template` SET `speed_walk`=1 WHERE  `entry`=3694;
UPDATE `quest_template` SET `PrevQuestId`=0 WHERE `Id`=990;
-- Terenthis SAI
SET @ENTRY := 3693;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,20,0,100,0,995,0,0,0,12,3694,6,180000,0,0,0,8,0,0,0,6339.14,341.764,24.3387,0.498114,"Terenthis - On Quest 'Escape Through Stealth' Finished - Summon Creature 'Sentinel Selarin'"),
(@ENTRY,0,1,0,20,0,100,0,994,0,0,0,12,3694,6,180000,0,0,0,8,0,0,0,6339.14,341.764,24.3387,0.498114,"Terenthis - On Quest 'Escape Through Force' Finished - Summon Creature 'Sentinel Selarin'");

-- Sentinel Selarin SAI
DELETE FROM `waypoints` WHERE `entry`=3694;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
(3694, 1, 6339.14,341.764,24.3387, 'Sentinel Selarin'),
(3694, 2, 6353.39,354.557,22.3779, 'Sentinel Selarin'),
(3694, 3, 6368.99,357.894,21.5712, 'Sentinel Selarin'),
(3694, 4, 6387.81,359.455,18.9899, 'Sentinel Selarin'),
(3694, 5, 6398.12,363.588,17.366, 'Sentinel Selarin'),
(3694, 6, 6403.68,370.92,15.6815, 'Sentinel Selarin'),
(3694, 7, 6416.57,392.998,12.0215, 'Sentinel Selarin'),
(3694, 8, 6424.95,399.193,10.9586, 'Sentinel Selarin'),
(3694, 9, 6428.93,396.971,11.1736, 'Sentinel Selarin'),
(3694, 10, 6432,388.708,13.7662, 'Sentinel Selarin'),
(3694, 11, 6436.71,375.264,13.9403, 'Sentinel Selarin'),
(3694, 12, 6434.92,367.203,13.9403, 'Sentinel Selarin'), 
(3694, 13, 6436.9,374.833,13.9403, 'Sentinel Selarin'),
(3694, 14, 6431.63,389.723,13.5875, 'Sentinel Selarin'),
(3694, 15, 6428.84,397.45,11.0941, 'Sentinel Selarin'),
(3694, 16, 6424,400.084,10.9784, 'Sentinel Selarin'),
(3694, 17, 6413.76,392.804,12.2825, 'Sentinel Selarin'),
(3694, 18, 6401.4,368.195,16.4249, 'Sentinel Selarin'),
(3694, 19, 6393.69,360.887,18.1549, 'Sentinel Selarin'),
(3694, 20, 6377.21,357.731,20.6589, 'Sentinel Selarin'),
(3694, 21, 6358.35,357.353,22.2106, 'Sentinel Selarin'),
(3694, 22, 6348.45,352.662,22.6056, 'Sentinel Selarin'),
(3694, 23, 6322.42,326.649,25.3338, 'Sentinel Selarin');

SET @ENTRY := 3694;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,2,54,0,100,0,0,0,0,0,53,1,3694,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Just Summoned - Start Waypoint"),
(@ENTRY,0,1,0,40,0,100,0,23,3694,0,0,41,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Waypoint 23 Reached - Despawn Instant"),
(@ENTRY,0,2,5,61,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Just Summoned - Set Active On"),
(@ENTRY,0,3,0,40,0,100,0,1,3694,0,0,80,@ENTRY*100+00,2,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Waypoint 1 Reached - Run Script"),
(@ENTRY,0,4,6,40,0,100,0,12,3694,0,0,80,@ENTRY*100+01,2,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Waypoint 12 Reached - Run Script"),
(@ENTRY,0,5,0,61,0,100,0,0,0,0,0,83,3,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Just Summoned - Remove Npc Flags Gossip & Questgiver");

-- Actionlist SAI
SET @ENTRY := 369400;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,0,0,0,0,54,1000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Script - Pause Waypoint"),
(@ENTRY,9,1,0,0,0,100,0,0,0,0,0,59,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Script - Set Run On");

-- Actionlist SAI 
SET @ENTRY := 369401;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,0,0,0,0,54,60000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - On Script - Pause Waypoint"),
(@ENTRY,9,1,0,0,0,100,0,1000,1000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,4.6089,"Sentinel Selarin - Set Orientation 4,6089"),
(@ENTRY,9,2,0,0,0,100,0,3000,3000,0,0,1,0,4000,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - Say Line 0"),
(@ENTRY,9,3,0,0,0,100,0,5000,5000,0,0,1,15,5000,0,0,0,0,19,3693,30,0,0,0,0,0,"Sentinel Selarin - Say Line 15"),
(@ENTRY,9,4,0,0,0,100,0,5000,500,0,0,1,1,5000,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - Say Line 1"),
(@ENTRY,9,5,0,0,0,100,0,5000,5000,0,0,1,2,5000,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - Say Line 2"),
(@ENTRY,9,6,0,0,0,100,0,0,0,0,0,82,3,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sentinel Selarin - Add Npc Flags Gossip & Questgiver");

DELETE FROM `creature_text` WHERE `entry`=3694;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES 
(3694, 0, 0, 'Terenthis, Raene sent me to find you. The Sentinels are in need of help in Ashenvale.', 12, 7, 100, 0, 0, 0, 'Sentinel Selarin'),
(3694, 1, 0, 'Thank you, Terenthis. I shall remain here for as long as I can.', 12, 7, 100, 0, 0, 0, 'Sentinel Selarin'),
(3694, 2, 0, 'Hopefully I can send a worthy few to help Ashenvale and the Sentinels.', 12, 7, 100, 0, 0, 0, 'Sentinel Selarin'),
(3694, 3, 0, 'Thank you again, Terenthis. I shall return again if more help is needed. Until then, please send more adventurers to Ashenvale', 12, 7, 100, 0, 0, 0, 'Sentinel Selarin');


DELETE FROM `creature_text` WHERE `entry`=3693 AND `groupid`=15;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`) VALUES 
(3693, 15, 0, 'I understand, my friend. I shall find someone to help your master. Go back to him now, or at least stay close.', 12, 0, 0, 0, 0, 0, 0, 'Terenthis');
UPDATE `battleground_template` SET `StartMaxDist`=50 WHERE `id`=3;
UPDATE `battleground_template` SET `StartMaxDist`=20 WHERE `id`=7;-- DB/Quest: All Hail Roanauk - players will be able to speak with Roanauk when Overlord Agmar is not around https://github.com/SunwellTracker/issues/issues/2827
DELETE FROM `conditions` WHERE `SourceGroup` = 9461 AND `ConditionTypeOrReference` = 29;-- DB/NPC: Hide some Demolisionists on Ulduar 25
UPDATE `creature` SET `spawnMask` = 1 WHERE `guid` IN (136361,136369,136362);-- DB/NPC: Remove duplicate Goblin Commoner spawn - https://github.com/SunwellTracker/issues/issues/2848
-- DB/Quest: The Warm Up - make sure player receive credit even if killed without bear - https://github.com/SunwellTracker/issues/issues/2847
DELETE FROM `creature` WHERE `guid` = 91581;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 29352 AND `id` = 2;
INSERT INTO `smart_scripts` VALUES
(29352, 0, 2, 0, 25, 0, 100, 0, 0, 0, 0, 0, 42, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Kirgaraak - On Respawn - Set Invincibilty hp level');
UPDATE `smart_scripts` SET `event_type`='61' WHERE  `entryorguid` IN (176308, 176304) AND `source_type`=1 AND `id`=3 AND `link`=0;
UPDATE `smart_scripts` SET `link`='3' WHERE  `entryorguid` IN (176308, 176304) AND `source_type`=1 AND `id`=2 AND `link`=0;-- DB/NPC: Bael'dun Excavator - remove incorrect spawnpoint https://github.com/SunwellTracker/issues/issues/2857
-- DB/NPC: Cavedweller Worg - add missing spawns https://github.com/SunwellTracker/issues/issues/2832
DELETE FROM `creature` WHERE `guid` = 13614;

DELETE FROM `creature` WHERE `guid` BETWEEN 300013 AND 300023 AND `id` = 30164;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(300013, 30164, 571, 1, 1, 0, 0, 7690.04, -1594.69, 965.279, 3.65759, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(300014, 30164, 571, 1, 1, 0, 0, 7661.55, -1569.36, 968.239, 2.53525, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(300015, 30164, 571, 1, 1, 0, 0, 7665.35, -1561.81, 968.275, 2.87769, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(300016, 30164, 571, 1, 1, 0, 0, 7596.92, -1562.83, 972.53, 0.171203, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(300017, 30164, 571, 1, 1, 0, 0, 7599.82, -1569.22, 972.866, 0.425672, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(300018, 30164, 571, 1, 1, 0, 0, 7585.73, -1531.5, 979.362, 6.1795, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(300019, 30164, 571, 1, 1, 0, 0, 7586.61, -1523.05, 980.795, 6.1795, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(300020, 30164, 571, 1, 1, 0, 0, 7579.45, -1490.33, 978.643, 3.2429, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(300021, 30164, 571, 1, 1, 0, 0, 7576.75, -1484.11, 978.622, 3.55077, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(300022, 30164, 571, 1, 1, 0, 0, 7570.44, -1455.32, 969.913, 3.24054, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(300023, 30164, 571, 1, 1, 0, 0, 7777.41, -1558.2, 943.307, 0.171988, 300, 10, 0, 0, 0, 1, 0, 0, 0);
-- DB/Quest: Battle of the Crimson Watch - correct text to include players name https://github.com/SunwellTracker/issues/issues/2865
UPDATE `creature_text` SET `text` = "So you have defeated the Crimson Sigil. You now seek to challenge my rule? Not even Arthas could defeat me, yet you dare to even harbor such thoughts? Then I say to you, come! Come $N! The Black Temple awaits..." WHERE `groupid` = 11 AND `entry` = 22083;
UPDATE `script_waypoint` SET `location_z`='100' WHERE  `entry`=23348 AND `pointid`=11;UPDATE `gameobject` SET `spawntimesecs` = 3600 WHERE `guid` IN (64904,64856,64857,64858);
DELETE FROM `trinity_string` WHERE `entry` = 10512;
INSERT INTO `trinity_string`(`entry`, `content_default`) VALUES (10512, 'Player %s looted chest while being alone in dungeon.');SET @TRINITY_STRING_ID:= 10513;

DELETE FROM `trinity_string` WHERE `entry` = @TRINITY_STRING_ID;
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(@TRINITY_STRING_ID, "Player (%s) has send multiple (%u) invites in a very short period of time!");
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (18256);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (18256);
INSERT INTO `smart_scripts` VALUES
(18256, 0, 0, 0, 25, 0, 100, 1, 0, 0, 0, 0, 71, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Alliance Halaa Guards - Reset - Load Equipment'),
(18256, 0, 1, 0, 0, 0, 100, 0, 0, 0, 2300, 3900, 11, 23337, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Alliance Halaani Guard - In Combat - Cast \'Shoot\'');
DELETE FROM `spell_script_names` WHERE `spell_id` = 24932;
INSERT INTO `spell_script_names` VALUES (24932, 'spell_dru_leader_of_the_pack');DELETE FROM `creature_loot_template` WHERE `entry` = 31610 AND `item` = 41796;
INSERT INTO `creature_loot_template` VALUES (31610, 41796, 100, 1, 0, 1, 1);UPDATE `spell_proc_event` SET `procFlags` = `procFlags` | 0x00040000 WHERE `entry` = 16864;UPDATE `creature_template` SET `faction` = 2129, `unit_flags` = `unit_flags` | 768 WHERE `entry` = 35984;
UPDATE `smart_scripts` SET `target_type` = 42 WHERE `entryorguid` = 25758 AND `id` = 4;
UPDATE `smart_scripts` SET `target_type` = 42 WHERE `entryorguid` = 25753 AND `id` = 4;
UPDATE `smart_scripts` SET `target_type` = 42 WHERE `entryorguid` = 25792 AND `id` = 2;
DELETE FROM `trinity_string` WHERE `entry` = 10514;
INSERT INTO `trinity_string`(`entry`, `content_default`) VALUES (10514, '|cff77a5c4[%s] - Team has joined arena queue! Triggered by: %s|r');DELETE FROM `smart_scripts` WHERE `entryorguid` = 33429 AND `source_type` = 0 AND `id` = 2;
INSERT INTO `smart_scripts` (`entryorguid`, `id`, `event_type`, `action_type`, `action_param1`, `target_type`, `comment`) VALUES (33429, 2, 6, 41, 60000, 1, 'On Death - Despawn after 60s');
UPDATE `creature` SET `spawntimesecs` = 180 WHERE `id` = 30160;-- Field Marshal Afrasiabi SAI
SET @ENTRY := 14721;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,20,0,100,0,7782,0,0,0,80,@ENTRY*100+00,2,0,0,0,0,1,0,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Quest 'The Lord of Blackrock' Finished - Run Script"),
(@ENTRY,0,1,0,61,0,100,0,7782,0,0,0,64,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Quest 'The Lord of Blackrock' Finished - Store Targetlist");

-- Actionlist SAI
SET @ENTRY := 1472100;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,1000,1000,0,0,83,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Script - Remove Npc Flag Questgiver"),
(@ENTRY,9,1,0,0,0,100,0,5000,5000,0,0,1,0,8000,0,0,0,0,12,1,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Script - Say Line 0"),
(@ENTRY,9,2,0,0,0,100,0,8000,8000,0,0,1,1,10000,0,0,0,0,12,1,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Script - Say Line 1"),
(@ENTRY,9,3,0,0,0,100,0,3000,3000,0,0,50,179882,21600,0,0,0,0,8,0,0,0,-8925.57,496.042,103.767,2.42801,"Field Marshal Afrasiabi - On Script - Summon Gameobject 'The Severed Head of Nefarian'"),
(@ENTRY,9,4,0,0,0,100,0,6000,6000,0,0,11,22888,0,0,0,0,0,1,0,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Script - Cast 'Rallying Cry of the Dragonslayer'"),
(@ENTRY,9,5,0,0,0,100,0,1000,1000,0,0,82,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Field Marshal Afrasiabi - On Script - Add Npc Flag Questgiver");

DELETE FROM `creature_text` WHERE `entry` IN (14721);
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `comment`) VALUES 
(14721, 0, 0, 'Citizens of the Alliance, the Lord of Blackrock is slain! Nefarian has been subdued by the combined might of $N and $Ghis:her; allies!', 14, 0, 100, 0, 0, 0, 'Field Marshal Afrasiabi'),
(14721, 1, 0, 'Let your spirits rise! Rally around your champion, bask in $Ghis:her; glory! Revel in the rallying cry of the dragon slayer!', 14, 0, 100, 0, 0, 0, 'Field Marshal Afrasiabi');

-- Overlord Runthak SAI
SET @ENTRY := 14392;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,20,0,100,0,7784,0,0,0,80,@ENTRY*100+00,2,0,0,0,0,1,0,0,0,0,0,0,0,"Overlord Runthak - On Quest 'The Lord of Blackrock' Finished - Run Script"),
(@ENTRY,0,1,0,61,0,100,0,7784,0,0,0,64,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Overlord Runthak - On Quest 'The Lord of Blackrock' Finished - Store Targetlist");

-- Actionlist SAI
SET @ENTRY := 1439200;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,9,0,0,0,0,100,0,1000,1000,0,0,83,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Overlord Runthak - On Script - Remove Npc Flag Questgiver"),
(@ENTRY,9,1,0,0,0,100,0,2000,2000,0,0,59,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Overlord Runthak - On Script - Set Run Off"),
(@ENTRY,9,2,0,0,0,100,0,0,0,0,0,69,0,0,0,0,0,0,8,0,0,0,1544,-4425.87,10.9056,3.323,"Overlord Runthak - On Script - Move To Position"),
(@ENTRY,9,3,0,0,0,100,0,14000,14000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,0.6367,"Overlord Runthak - On Script - Set Orientation 0,6367"),
(@ENTRY,9,4,0,0,0,100,0,3000,3000,0,0,1,0,17000,0,0,0,0,12,1,0,0,0,0,0,0,"Overlord Runthak - On Script - Say Line 0"),
(@ENTRY,9,5,0,0,0,100,0,17000,17000,0,0,1,1,10000,0,0,0,0,12,1,0,0,0,0,0,0,"Overlord Runthak - On Script - Say Line 1"),
(@ENTRY,9,6,0,0,0,100,0,3000,3000,0,0,50,179881,21600,0,0,0,0,8,0,0,0,1540.28,-4422.19,7.0051,5.22833,"Overlord Runthak - On Script - Summon Gameobject 'The Severed Head of Nefarian'"),
(@ENTRY,9,7,0,0,0,100,0,5000,5000,0,0,9,0,0,0,0,0,0,20,179881,100,0,0,0,0,0,"Overlord Runthak - On Script - Activate Gameobject"),
(@ENTRY,9,8,0,0,0,100,0,5000,5000,0,0,11,22888,0,0,0,0,0,1,0,0,0,0,0,0,0,"Overlord Runthak - On Script - Cast 'Rallying Cry of the Dragonslayer'"),
(@ENTRY,9,9,0,0,0,100,0,10000,10000,0,0,69,0,0,0,0,0,0,8,0,0,0,1568,-4405.87,8.13371,0.3434,"Overlord Runthak - On Script - Move To Position"),
(@ENTRY,9,10,0,0,0,100,0,15000,15000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,3.31613,"Overlord Runthak - On Script - Set Orientation 3,31613"),
(@ENTRY,9,11,0,0,0,100,0,1000,1000,0,0,82,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Overlord Runthak - On Script - Add Npc Flag Questgiver");

DELETE FROM `creature_text` WHERE `entry` IN (14392);
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `comment`) VALUES 
(14392, 0, 0, 'NEFARIAN IS SLAIN! people of Orgrimmar, bow down before the might of $N and his allies for they have laid a blow against the Black Dragonflight that is sure to stir the Aspects from their malaise! This defeat shall surely be felt by the father of the Black Flight: Deathwing reels in pain and anguish this day!', 14, 0, 100, 0, 0, 0, 'Overlord Runthak'),
(14392, 1, 0, 'Be lifted by $N accomplishment! Revel in his rallying cry!', 14, 0, 100, 0, 0, 0, 'Overlord Runthak');
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -20 WHERE `entry` = 27330 AND `item` = 37931;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -20 WHERE `entry` = 27332 AND `item` = 37931;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = -20 WHERE `entry` = 27333 AND `item` = 37931;
-- Shadowlord Deathwail
UPDATE `creature_addon` SET `mount` = 20684 WHERE `guid` = 77084;
UPDATE `creature_template` SET `InhabitType` = 7, `HoverHeight` = 0, `MovementType` = 2 WHERE `entry` = 22006;
UPDATE `creature` SET `MovementType` = 0 WHERE `guid` =  77084;
UPDATE `creature_addon` SET `path_id` = 0 WHERE `guid` =  77084; -- 770840
-- DB/NPC: Chillmaw - reduce respawn rate https://github.com/SunwellTracker/issues/issues/2933
-- DB/Loot: Big Mouth Clam - redo drop table - https://github.com/SunwellTracker/issues/issues/2938
UPDATE `creature` SET `spawntimesecs` = 120 WHERE `id` = 33687;

DELETE FROM `spell_loot_template` WHERE `entry` = 58165;
INSERT INTO `spell_loot_template` (`Entry`, `Item`, `ChanceOrQuestChance`, `LootMode`, `GroupId`,`MinCountOrRef`,`MaxCount`) VALUES
(58165,  7974,100.0,   1, 0, 1, 1),
(58165,  7971,  4.0, 1, 1, 1, 1),
(58165, 13926,  1.7, 1, 1, 1, 1),
(58165,  5500,  2.3, 1, 1, 1, 1),
(58165,  5498,  1.0, 1, 1, 1, 1);
DELETE FROM `trinity_string` WHERE `entry` = 10512;
INSERT INTO `trinity_string`(`entry`, `content_default`) VALUES (10512, 'Player %s looted chest while being alone in dungeon. (Map: %s, Count: %u)');-- DB/NPC: Zerillis - fix spawning in textures - https://github.com/SunwellTracker/issues/issues/2954
UPDATE `creature_addon` SET `path_id` = 0 WHERE `guid` = 138256;
UPDATE `creature` SET `MovementType` = 0 WHERE `guid` = 138256;DELETE FROM `smart_scripts` WHERE `entryorguid` = 34196;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(34196, 0, 0, 0, 0, 0, 100, 2, 10000, 15000, 15000, 25000, 11, 64847, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Runed Flame Jets 10'),
(34196, 0, 1, 0, 0, 0, 100, 4, 10000, 15000, 15000, 25000, 11, 64988, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Runed Flame Jets 25'),
(34196, 0, 2, 0, 0, 0, 100, 2, 5000, 10000, 5000, 10000, 11, 64870, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Lava Burst 10'),
(34196, 0, 3, 0, 0, 0, 100, 4, 5000, 10000, 5000, 10000, 11, 64991, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Lava Burst 25'),
(34196, 0, 4, 0, 0, 0, 100, 2, 5000, 10000, 5000, 10000, 11, 64852, 0, 0, 0, 0, 0, 6, 60, 1, 0, 0, 0, 0, 0, 'Flaming Rune 10'),
(34196, 0, 5, 0, 0, 0, 100, 4, 5000, 10000, 5000, 10000, 11, 64990, 0, 0, 0, 0, 0, 6, 60, 1, 0, 0, 0, 0, 0, 'Flaming Rune 25');

DELETE FROM creature_template_addon WHERE entry IN (33754, 33757, 33755, 33758);
UPDATE `creature_template` SET `ScriptName` = "npc_dark_rune_ravager_thunderer" WHERE `entry` IN (33754, 33755);
UPDATE `item_template` SET `RequiredSkill`=762, `RequiredSkillRank`=150, ScriptName = 'item_bengal_tiger', spellid_1 = 10790, spellcooldown_1 = 330, spellcategory_1 = 3000, spellcategorycooldown_1 = 3000, Description ='Ride me!', `bonding` = 1 WHERE  `entry`=8630; 
DELETE FROM `conditions` WHERE `SourceGroup` = 1 AND `SourceEntry` = 33447;
INSERT INTO `conditions` VALUES
(22, 1, 33447, 0, 0, 29, 0, 33448, 30, 0, 1, 0, 0, '', 'Do not summon Argent Valiant if one already summoned');

DELETE FROM `conditions` WHERE `SourceGroup` = 1 AND `SourceEntry` = 33518;
INSERT INTO `conditions` VALUES
(22, 1, 33518, 0, 0, 29, 0, 33707, 30, 0, 1, 0, 0, '', 'Do not summon Argent Champion if one already summoned');
-- DB/GO: Fix unreachable mining node in icecrown - https://github.com/SunwellTracker/issues/issues/2934
UPDATE `gameobject` SET `position_x` = 8488.05, `position_y` = 1449.59, `position_z` = 655.43 WHERE `guid` = 163813;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 30160 AND `source_type` = 0 AND `id` = 2;DELETE FROM `creature_text` WHERE `entry` = 22972;
INSERT INTO `creature_text` VALUES 
(22972, 0, 0, 'Sparrowhawk spotted Raven Stone Fragment and flies towards it', 16, 0, 100, 113, 0, 0, 0, 'Cenarion Sparrowhawk');
UPDATE `creature_addon` SET `path_id` = 770840 WHERE `guid` = 77084;
UPDATE `creature` SET `MovementType` = 2 WHERE `guid` = 77084;
UPDATE `creature_template` SET `flags_extra` = 0 WHERE `entry` = 22061;
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 22006;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 22061;
INSERT INTO `smart_scripts` VALUES 
(22061, 0, 0, 0, 6, 0, 100, 1, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 77084, 22006, 0, 0, 0, 0, 0, 'Shadowmoon SoulStealer - on just died - Set data 1 1 - On Deathwail');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 22006;
INSERT INTO `smart_scripts` VALUES 
(22006, 0, 0, 0, 0, 0, 100, 0, 0, 0, 3400, 4700, 11, 12471, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Shadowlord Deathwail - In Combat - Cast \'Shadow Bolt\''),
(22006, 0, 1, 0, 0, 0, 100, 0, 8000, 8000, 20000, 22000, 11, 32709, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 'Shadowlord Deathwail - In Combat - Cast \'Death Coil\''),
(22006, 0, 2, 0, 0, 0, 100, 0, 5000, 7000, 15000, 15000, 11, 15245, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Shadowlord Deathwail - In Combat - Cast \'Shadow Bolt Volley\''),
(22006, 0, 3, 0, 38, 0, 100, 0, 1, 1, 0, 0, 23, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathwail - on Data set 1 1 - increase phase'),
(22006, 0, 4, 0, 60, 4, 100, 1, 0, 0, 0, 0, 80, 2200600, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathwail - on 3 soulstealer killed - Run script');
DELETE FROM `gameobject_loot_template` WHERE `entry` = 20529 AND `item` = 29434;
INSERT INTO `gameobject_loot_template` VALUES 
(20529, 29434, 100, 2, 0, 1, 1);
-- DB/Misc: DK starting zone - fix issues with some mobs/ai https://github.com/SunwellTracker/issues/issues/3017
DELETE FROM `creature` WHERE `guid` IN (129545,129544);
DELETE FROM `smart_scripts` WHERE `entryorguid` = 28577;
INSERT INTO `smart_scripts` VALUES
(28577, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Reset - Set Event Phase'),
(28577, 0, 1, 2, 4, 1, 100, 0, 0, 0, 0, 0, 11, 52384, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Aggro - Cast Cosmetic - Periodic Cower Effect'),
(28577, 0, 2, 3, 61, 1, 100, 0, 0, 0, 0, 0, 11, 52385, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Aggro - Cast Cosmetic - Periodic Cower'),
(28577, 0, 3, 4, 61, 1, 100, 0, 0, 0, 0, 0, 11, 51604, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Aggro - Cast Self Stun'),
(28577, 0, 4, 5, 61, 1, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Aggro - Say Line 1'),
(28577, 0, 5, 0, 61, 1, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Aggro - Set Event Phase 2'),
(28577, 0, 6, 0, 4, 2, 50, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Aggro - Say Line 0'),
(28577, 0, 7, 0, 2, 2, 50, 1, 0, 30, 0, 0, 11, 52262, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - Between Health 0-30% - Cast Cornered and Enraged!'),
(28577, 0, 8, 0, 6, 0, 50, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Citizen of Havenshire - On Death - Say Line 2');

UPDATE `smart_scripts` SET `action_param2` = 0 WHERE `entryorguid` = 28611 AND `id` = 0;
-- DB/Misc: correct height of Sunfury Magister beams near Manaforge Bnaar https://github.com/SunwellTracker/issues/issues/2841
UPDATE `creature` SET `position_z` = `position_z` - 3.5 WHERE `id` = 19421;
-- DB/AI: Sunfury technician - update AI to prevent them from going places https://github.com/SunwellTracker/issues/issues/2842
DELETE FROM `smart_scripts` WHERE `entryorguid` = 20218 AND `id` = 18;
INSERT INTO `smart_scripts` VALUES 
(20218, 0, 18, 0, 54, 0, 100, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Sunfury Technician - On Just Summoned - Set Home Position');
-- DB/Creature: Rabid Brown Bear - fix spawn in textures https://github.com/SunwellTracker/issues/issues/2843
UPDATE `creature` SET `position_x` = -22.64, `position_y` = -3549.97, `position_z` = 34.58 WHERE `guid` = 101032;-- DB/GO: Dire Maul - limit number of thorium vein spawns
-- DB/Quest: Kurenai reputation correct reputation gain for some quests ( 250 -> 500 ) https://github.com/SunwellTracker/issues/issues/2516
-- DB/Fishing: Raw Summer Bass/Winter Squid - seasonal fishing https://github.com/SunwellTracker/issues/issues/2838
DELETE FROM `pool_template` WHERE `entry` = 50004;
INSERT INTO `pool_template` VALUES
(50004, 1, 'Dire Maul Vein');
DELETE FROM `pool_gameobject` WHERE `pool_entry` = 50004 AND `guid` IN (269112,269111,269110,120704);
INSERT INTO `pool_gameobject` VALUES
(269112, 50004, 0, ''),
(269111, 50004, 0, ''),
(269110, 50004, 0, ''),
(120704, 50004, 0, '');

UPDATE `quest_template` SET `RewardFactionValueId1` = 7 WHERE `id` IN 
(9830,
9833,
9834,
9835,
9839,
9874,
9878,
9902,
9905,
9917,
9921,
9923,
9924,
9936,
9940,
9954,
10115,
10116,
10476,
10477);

DELETE FROM `reference_loot_template` WHERE `entry` IN (11009, 11005) AND `item` = 13755;
INSERT INTO `reference_loot_template` VALUES
(11009,13755, 15, 1, 1, 1, 1),
(11005,13755, 20, 1, 1, 1, 1);
DELETE FROM `conditions` WHERE `SourceGroup` IN (11009, 11005) AND `SourceEntry` IN (13755, 13756) AND `SourceTypeOrReferenceId` = 3;
INSERT INTO `conditions` VALUES 
(3, 11009, 13755, 0, 0, 12, 0, 2, 0, 0, 0, 0, 0, '', 'Winter Squid can be caught only during Winterveil'),
(3, 11009, 13756, 0, 0, 12, 0, 2, 0, 0, 1, 0, 0, '', 'Raw Summer Bass can\'t be caught only during Winterveil'),
(3, 11005, 13755, 0, 0, 12, 0, 2, 0, 0, 0, 0, 0, '', 'Winter Squid can be caught only during Winterveil'),
(3, 11005, 13756, 0, 0, 12, 0, 2, 0, 0, 1, 0, 0, '', 'Raw Summer Bass can\'t be caught only during Winterveil');
-- DB/Quest: Amped for Revolt - add couple frostwyrms spawns https://github.com/SunwellTracker/issues/issues/2944
-- DB/Quest: Springing The Trap - signal fire should be set ablaze https://github.com/SunwellTracker/issues/issues/2753
DELETE FROM `creature` WHERE `id` = 32767 AND `guid` IN (303342, 303343, 303344, 303345);
INSERT INTO `creature` VALUES 
(303342, 32767, 571, 1, 1, 27064, 0, 7698.87, 1724.61, 456.95, 0.300584, 10, 60, 0, 25200, 0, 1, 0, 0, 0),
(303343, 32767, 571, 1, 1, 27064, 0, 7862.54, 1793.10, 447.59, 3.33399, 10, 0, 0, 25200, 0, 2, 0, 0, 0),
(303344, 32767, 571, 1, 1, 27064, 0, 7554.07, 1904.8, 471.49, 0.300584, 10, 60, 0, 25200, 0, 1, 0, 0, 0),
(303345, 32767, 571, 1, 1, 27064, 0, 7837.28, 1959.09, 462.31, 0.300584, 10, 60, 0, 25200, 0, 1, 0, 0, 0);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 2623700;
INSERT INTO `smart_scripts` VALUES 
(2623700, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 60, 1, 100, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Set Fly'),
(2623700, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 4063.72, 7084.12, 171, 3, 'Script9 - Move Pos'),
(2623700, 9, 2, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 60, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Set Fly false'),
(2623700, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Set Run false'),
(2623700, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 91, 3, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Remove Byte Flag'),
(2623700, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 99, 3, 0, 0, 0, 0, 0, 14, 99716, 194151, 0, 0, 0, 0, 0, 'Script9 - Despawn Signal Fire'),
(2623700, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 50, 194152, 30000, 1, 0, 0, 0, 8, 0, 0, 0, 4061.98, 7109.13, 170, 0, 'Script9 - Spawn Signal Fire'),
(2623700, 9, 7, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 21, 40, 0, 0, 0, 0, 0, 0, 'Script9 - Talk'),
(2623700, 9, 8, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Talk'),
(2623700, 9, 9, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 4037.97, 7083.64, 167.71, 3.2, 'Script9 - Move Pos'),
(2623700, 9, 10, 0, 0, 0, 100, 0, 11000, 11000, 0, 0, 12, 26299, 4, 70000, 0, 0, 0, 8, 0, 0, 0, 4025.15, 7082.42, 162.54, 0.5, 'Script9 - Summon Creature'),
(2623700, 9, 11, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Talk'),
(2623700, 9, 12, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 11, 31962, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Cast Spell'),
(2623700, 9, 13, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 75, 39199, 0, 0, 0, 0, 0, 11, 26299, 30, 0, 0, 0, 0, 0, 'Script9 - Add Aura'),
(2623700, 9, 14, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Talk'),
(2623700, 9, 15, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 11, 46853, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Cast Spell'),
(2623700, 9, 16, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 21, 40, 0, 0, 0, 0, 0, 0, 'Script9 - Talk'),
(2623700, 9, 17, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 86, 46814, 2, 21, 40, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Cross Cast'),
(2623700, 9, 18, 0, 0, 0, 100, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 11, 26299, 30, 0, 0, 0, 0, 0, 'Script9 - Despawn'),
(2623700, 9, 19, 0, 0, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Script9 - Despawn');
UPDATE `trinity_string` SET `content_default`='|cffff9933.xp|cFFFFD700 - changes experience multiplier.|r' WHERE  `entry`=73;
UPDATE creature_template SET flags_extra = flags_extra | 0x01000000 WHERE entry IN (33955, 33288);

