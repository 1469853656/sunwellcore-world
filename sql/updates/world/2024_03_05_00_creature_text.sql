DELETE FROM `creature_text` WHERE `entry` IN (25823, 25818, 25820, 25821, 25822);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`) VALUES 
(25823, 0, 0, 'Reporting for duty, sir!', 12, 0, 100, 0, 0, 0, 0, ''),
(25818, 0, 0, 'Reporting for duty, sir!', 12, 0, 100, 0, 0, 0, 0, ''),
(25820, 0, 0, 'Reporting for duty, sir!', 12, 0, 100, 0, 0, 0, 0, ''),
(25821, 0, 0, 'Reporting for duty, sir!', 12, 0, 100, 0, 0, 0, 0, ''),
(25822, 0, 0, 'Reporting for duty, sir!', 12, 0, 100, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_reprogrammed_robot" WHERE `entry` IN (25823, 25818, 25820, 25821, 25822);
