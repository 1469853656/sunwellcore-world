-- DB/Quest: Kickin' Nass and Takin' Manes - Nass should be summoned when accepted quest mounted https://github.com/SunwellTracker/issues/issues/3299
-- DB/Quest: Vartrus the Ancient - add option to aquire both Lok'delar, Stave of the Ancient Keepers and Rhok'delar, Longbow of the Ancient Keepers https://github.com/SunwellTracker/issues/issues/3244
DELETE FROM `smart_scripts` WHERE `entryorguid` = 28518;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(28518, 0, 0, 0, 62, 0, 100, 0, 9709, 0, 0, 0, 43, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Gossip Option 0 Selected - Dismount'),
(28518, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 11, 51864, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Gossip Option 0 Selected - Cast \'Player Summon Nass\''),
(28518, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Gossip Option 0 Selected - Say Line 0'),
(28518, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Gossip Option 0 Selected - Close Gossip'),
(28518, 0, 4, 5, 19, 0, 100, 0, 12630, 0, 0, 0, 43, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Quest \'Kickin\' Nass and Takin\' Manes\' Taken - Dismount'),
(28518, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 51889, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Quest \'Kickin\' Nass and Takin\' Manes\' Taken - Cast \'Quest Accept Summon Nass\''),
(28518, 0, 6, 0, 62, 0, 100, 0, 9709, 1, 0, 0, 11, 53810, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Gossip Option 1 Selected - Cast \'Push Ensorceled Choker\''),
(28518, 0, 7, 0, 19, 0, 100, 0, 12676, 0, 0, 0, 85, 55413, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Quest \'Sabotage\' Taken - Invoker Cast \'Clearquest\''),
(28518, 0, 8, 0, 19, 0, 100, 0, 12669, 0, 0, 0, 85, 55411, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Quest \'So Far, So Bad\' Taken - Invoker Cast \'Clearquest\''),
(28518, 0, 9, 0, 19, 0, 100, 0, 12677, 0, 0, 0, 85, 55412, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Quest \'Hazardous Materials\' Taken - Invoker Cast \'Clearquest\''),
(28518, 0, 10, 0, 19, 0, 100, 0, 12661, 0, 0, 0, 85, 55410, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Stefan Vadu - On Quest \'Infiltrating Voltarus\' Taken - Invoker Cast \'Clearquests\'');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 14524;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(14524, 0, 0, 2, 62, 0, 100, 0, 30201, 0, 0, 0, 56, 18715, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On Gossip Select - Add Item'),
(14524, 0, 1, 2, 62, 0, 100, 0, 30201, 1, 0, 0, 56, 18713, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On Gossip Select - Invoker Cast'),
(14524, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On Gossip Select - Close Gossip');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `ConditionValue1` IN (18715, 18713) AND `SourceGroup` = 30201;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 30201, 1, 0, 1, 2, 0, 18715, 1, 0, 0, 0, 0, '', 'Must have item 18715'),
(15, 30201, 1, 0, 1, 2, 0, 18713, 1, 0, 1, 0, 0, '', 'Must not have item 18713'),
(15, 30201, 0, 0, 1, 2, 0, 18713, 1, 0, 0, 0, 0, '', 'Must have item 18713'),
(15, 30201, 0, 0, 1, 2, 0, 18715, 1, 0, 1, 0, 0, '', 'Must not have item 18715');

UPDATE `gossip_menu_option` SET `option_text` = "Can you create the Rhok'delar?" WHERE `menu_id` = 30201 AND `id` = 0;
UPDATE `gossip_menu_option` SET `option_text` = "Can you create the Lok'delar?" WHERE `menu_id` = 30201 AND `id` = 1;
