
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2208 AND `id` = 1);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2208, 0, 1, 0, 0, 0, 100, 0, 2700, 2900, 7900, 8100, 11, 744, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Greymist Tidehunter - In Combat - Cast \'744\'');
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2338;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2338);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2338, 0, 0, 0, 2, 0, 100, 1, 40, 80, 0, 0, 11, 2054, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Twilight Disciple - Between 40-80% Health - Cast \'2054\' (No Repeat)'),
(2338, 0, 1, 0, 2, 0, 100, 1, 5, 20, 0, 0, 11, 6074, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Twilight Disciple - Between 5-20% Health - Cast \'6074\' (No Repeat)');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2339;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2339);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2339, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 7165, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Twilight Thug - On Aggro - Cast \'7165\''),
(2339, 0, 1, 0, 0, 0, 100, 1, 6500, 10000, 0, 0, 11, 5242, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Twilight Thug - In Combat - Cast \'5242\' (No Repeat)'),
(2339, 0, 2, 0, 2, 0, 100, 0, 5, 30, 0, 0, 11, 6713, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Twilight Thug - Between 5-30% Health - Cast \'6713\'');
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2339;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2339);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2339, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 7165, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Twilight Thug - On Aggro - Cast \'7165\''),
(2339, 0, 1, 0, 0, 0, 100, 1, 6500, 10000, 0, 0, 11, 5242, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Twilight Thug - In Combat - Cast \'5242\' (No Repeat)'),
(2339, 0, 2, 0, 2, 0, 100, 0, 5, 30, 0, 0, 11, 6713, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Twilight Thug - Between 5-30% Health - Cast \'6713\'');
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2156;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2156);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2156, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 5810, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cracked Golem - On Aggro - Cast \'5810\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2231;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2231);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2231, 0, 0, 0, 2, 0, 100, 1, 30, 60, 0, 0, 11, 5424, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Pygmy Tide Crawler - Between 30-60% Health - Cast \'5424\' (No Repeat)');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2234;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2234);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2234, 0, 0, 0, 2, 0, 100, 1, 30, 60, 0, 0, 11, 5424, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Young Reef Crawler - Between 30-60% Health - Cast \'5424\' (No Repeat)');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2174;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2174);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2174, 0, 0, 0, 2, 0, 100, 1, 5, 15, 0, 0, 11, 3019, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Coastal Frenzy - Between 5-15% Health - Cast \'3019\' (No Repeat)');
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2235;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2235);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2235, 0, 0, 0, 0, 0, 100, 0, 2300, 2600, 8900, 9400, 11, 12166, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Reef Crawler - In Combat - Cast \'12166\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2236;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2236);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2236, 0, 0, 0, 0, 0, 100, 0, 2700, 3100, 9500, 10100, 11, 3391, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Raging Reef Crawler - In Combat - Cast \'3391\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2182;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2182);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2182, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 12544, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Stormscale Sorceress - On Aggro - Cast \'12544\''),
(2182, 0, 1, 0, 0, 0, 100, 0, 2300, 2700, 6800, 7400, 11, 20792, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Stormscale Sorceress - In Combat - Cast \'20792\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2181;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2181);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2181, 0, 0, 0, 2, 0, 100, 1, 30, 60, 0, 0, 11, 5164, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Stormscale Myrmidon - Between 30-60% Health - Cast \'5164\' (No Repeat)');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 3660;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 3660);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(3660, 0, 0, 0, 0, 0, 100, 0, 2700, 3100, 6800, 7200, 11, 7641, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Athrikus Narassin - In Combat - Cast \'7641\''),
(3660, 0, 1, 0, 2, 0, 100, 1, 30, 60, 0, 0, 11, 5782, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Athrikus Narassin - Between 30-60% Health - Cast \'5782\' (No Repeat)');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 10159;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 10159);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(10159, 0, 0, 0, 0, 0, 100, 0, 1000, 1200, 5600, 5700, 11, 8921, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Young Moonkin - In Combat - Cast \'8921\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2189;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2189);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2189, 0, 0, 0, 0, 0, 100, 0, 2400, 2900, 32400, 32900, 11, 744, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Vile Sprite - In Combat - Cast \'744\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2322;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2322);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2322, 0, 0, 0, 0, 0, 100, 0, 2100, 3100, 6900, 7400, 11, 3391, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Foreststrider - In Combat - Cast \'3391\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2321;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2321);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2321, 0, 0, 0, 0, 0, 100, 0, 2100, 3100, 6900, 7400, 11, 3391, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Foreststrider Fledgling - In Combat - Cast \'3391\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2069;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2069);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2069, 0, 0, 0, 0, 0, 100, 0, 1200, 1300, 12200, 14300, 11, 24331, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Moonstalker - In Combat - Cast \'24331\'');


UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2323;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2323);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2323, 0, 0, 0, 0, 0, 100, 0, 2100, 3100, 6900, 7400, 11, 3391, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Giant Foreststrider - In Combat - Cast \'3391\'');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 2070;
DELETE FROM `smart_scripts` WHERE (source_type = 0 AND entryorguid = 2070);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2070, 0, 0, 0, 0, 0, 100, 0, 2100, 2300, 7800, 8900, 11, 6595, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Moonstalker Runt - In Combat - Cast \'6595\'');


