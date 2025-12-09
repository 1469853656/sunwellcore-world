SET @ENTRY := 40000;
SET @NPC_ENTRY := 350000;

DELETE FROM `quest_template` WHERE `Id` IN (@ENTRY, @ENTRY + 1);
INSERT INTO `quest_template` VALUES 
(@ENTRY,2,80,80,0,-25,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4098,1,0,0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Path of the Gladiator','Win 3 rated arena matches and return to an Alliance Brigadier General or a Horde Warbringer at any capital city, Wintergrasp, Dalaran, or Shattrath.','The true gladiator doesn\'t know what it means to be afraid, his blade always hungers for enemies\' blood, also an arena\'s crowd loves him!\n\nToday you will have a chance to prove your worth. Take a place in the arena\'s preparation room, pass the arena\'s gates and show your enemies how the true fearless gladiator looks like!',NULL,'I\'ve seen proudly fluttering standards across the horizon, looks like you have brought victory to your faction!\n\nHonor of the champion is the best award you\'d ever get.','You are back, let\'s hope that you are bringing news of victory!',NULL,@NPC_ENTRY,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Arenas won',NULL,NULL,NULL,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,12340),
(@ENTRY + 1,2,80,80,0,-25,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25000,0,0,0,0,0,0,32770,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Protecting the Homeland','Win 5 battleground matches and return to an Alliance Brigadier General or a Horde Warbringer at any capital city, Wintergrasp, Dalaran, or Shattrath.','Even while fighting against Lich King and his dark forces we have to protect our homeland!\n\nYou will stand as an Azeroth\'s soldier who is bringing a victory to your faction! Go and fight then, return as a victorious combatant not as a filthy deserter!',NULL,'The song of victory has arrived to me before you $N! Looks like your blade is still blood-covered, go away and take a rest, you\'ve deserved for that!','Good to see you $N! Are you enough brave to be called as a gladiator? ',NULL,@NPC_ENTRY + 1,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Battlegrounds won',NULL,NULL,NULL,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,12340);


DELETE FROM `creature_template` WHERE `entry` IN (@NPC_ENTRY, @NPC_ENTRY + 1);
INSERT INTO `creature_template` VALUES 
(@NPC_ENTRY,0,0,0,0,0,11686,0,0,0,'Daily arena quest credit','',NULL,0,60,60,0,114,0,1.125,1.14286,1,0,104,138,0,252,1,2000,2000,1,33555200,2048,0,0,0,0,0,0,72,106,26,10,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,'',1,3,1,1.35,1,1,0,0,0,0,0,0,0,0,1,0,128,'',12340),
(@NPC_ENTRY + 1,0,0,0,0,0,11686,0,0,0,'Weekly battleground quest credit','',NULL,0,60,60,0,114,0,1.125,1.14286,1,0,104,138,0,252,1,2000,2000,1,33555200,2048,0,0,0,0,0,0,72,106,26,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',1,3,1,1.35,1,1,0,0,0,0,0,0,0,0,1,0,128,'',12340);

DELETE FROM `creature_queststarter` WHERE `quest` IN (@ENTRY, @ENTRY + 1);
INSERT INTO `creature_queststarter` VALUES 
(15351, @ENTRY),
(15351, @ENTRY + 1),
(15350, @ENTRY),
(15350, @ENTRY + 1);

DELETE FROM `creature_questender` WHERE `quest` IN (@ENTRY, @ENTRY + 1);
INSERT INTO `creature_questender`VALUES 
(15351, @ENTRY),
(15351, @ENTRY + 1),
(15350, @ENTRY),
(15350, @ENTRY + 1);
