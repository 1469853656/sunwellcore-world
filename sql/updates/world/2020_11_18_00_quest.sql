-- DB/Quest: Gambling Debt - fix Silvermoon Harry interaction https://github.com/SunwellTracker/issues/issues/2647
DELETE FROM `smart_scripts` WHERE `entryorguid` = 24539;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(24539, 0, 0, 1, 62, 0, 100, 0, 9010, 0, 0, 0, 2, 14, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Option 0 Selected - Set Faction 14'),
(24539, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Option 0 Selected - Close Gossip'),
(24539, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Option 0 Selected - Say Line 0'),
(24539, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Option 0 Selected - Store Target'),
(24539, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Option 0 Selected - Set Event Phase 1'),
(24539, 0, 5, 0, 2, 0, 100, 0, 0, 50, 0, 0, 80, 2453900, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - Between 1-50% Health - Run Script'),
(24539, 0, 6, 0, 64, 2, 100, 0, 0, 0, 0, 0, 98, 9011, 12175, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Hello - Send Gossip'),
(24539, 0, 8, 0, 62, 2, 100, 0, 9011, 0, 0, 0, 80, 2453901, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Gossip Option 0 Selected - Run Script'),
(24539, 0, 9, 0, 0, 0, 100, 0, 3000, 6000, 15000, 22000, 11, 15091, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - In Combat - Cast \'Blast Wave\''),
(24539, 0, 10, 0, 0, 0, 100, 0, 2500, 4000, 4000, 5000, 11, 50183, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - In Combat - Cast \'Scorch\'');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 2453901;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(2453901, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Script - Close Gossip'),
(2453901, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 56, 34115, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Script - Add Item \'Silvermoon Harry\'s Debt\' 1 Time'),
(2453901, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 102, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Script - Set Health Regeneration On'),
(2453901, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Script - Despawn In 10000 ms'),
(2453901, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '\'Silvermoon\' Harry - On Script - Set Event Phase 0');
