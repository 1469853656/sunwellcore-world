DELETE FROM `achievement_reward` WHERE `entry`=431;
INSERT INTO `achievement_reward` VALUES
(431,64,64,0,0,'','',0);

UPDATE `creature_queststarter` SET `quest`=10888 WHERE `quest`=13430;
UPDATE `creature_queststarter` SET `quest`=10445 WHERE `quest`=13432;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (742,744,745,6789);
INSERT INTO `achievement_criteria_data` VALUES
(742,11,0,0,'achievement_champion_of_the_naaru'),
(744,11,0,0,'achievement_hand_of_adal'),
(745,11,0,0,'achievement_hand_of_adal'),
(6789,11,0,0,'achievement_champion_of_the_naaru');

DELETE FROM `disables` WHERE `sourceType`=1 AND `entry` IN (10888,10445);
