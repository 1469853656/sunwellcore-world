
DELETE FROM `smart_scripts` WHERE `entryorguid`=28665 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(28665, 0, 0, 0, 61, 0, 100, 0, 0, 0, 0, 0, 53, 1, 28665, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Just Summoned - Start Waypoint'),
(28665, 0, 1, 0, 40, 0, 100, 0, 1, 28665, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 1 Reached - Say Line 0'),
(28665, 0, 2, 3, 40, 0, 100, 0, 10, 28665, 0, 0, 54, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 10 Reached - Pause Waypoint'),
(28665, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 10 Reached - Say Line 1'),
(28665, 0, 4, 5, 40, 0, 100, 0, 18, 28665, 0, 0, 54, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 18 Reached - Pause Waypoint'),
(28665, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 18 Reached - Set Run Off'),
(28665, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 18 Reached - Say Line 2'),
(28665, 0, 8, 0, 40, 0, 100, 0, 21, 28665, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 28671, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 21 Reached - Say Line 0'),
(28665, 0, 9, 10, 40, 0, 100, 0, 22, 28665, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 22 Reached - Set Run On'),
(28665, 0, 10, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 22 Reached - Say Line 3'),
(28665, 0, 11, 0, 40, 0, 100, 0, 31, 28665, 0, 0, 11, 39870, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 31 Reached - Cast \'Speed Burst\''),
(28665, 0, 12, 13, 40, 0, 100, 0, 34, 28665, 0, 0, 28, 39870, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 34 Reached - Remove Aura \'Speed Burst\''),
(28665, 0, 13, 14, 61, 0, 100, 0, 0, 0, 0, 0, 97, 30, 10, 0, 0, 0, 0, 8, 0, 0, 0, 5651.19, -3790.46, 361.974, 0, 'Har\'koa\'s Kitten - On Waypoint 34 Reached - Jump To Pos'),
(28665, 0, 14, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 2866500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 34 Reached - Run Script'),
(28665, 0, 15, 16, 40, 0, 100, 0, 11, 286650, 0, 0, 1, 5, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 11 Reached - Say Line 5'),
(28665, 0, 16, 17, 61, 0, 100, 0, 0, 0, 0, 0, 28, 25673, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 11 Reached - Remove Aura \'Riding Har\'koa\'s Kitten\''),
(28665, 0, 17, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Har\'koa\'s Kitten - On Waypoint 11 Reached - Despawn In 500 ms');
