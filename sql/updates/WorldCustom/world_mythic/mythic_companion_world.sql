SET @MYTHIC_COMPANION_ENTRY:= 91501;
DELETE FROM `creature_template` WHERE `entry` = @MYTHIC_COMPANION_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@MYTHIC_COMPANION_ENTRY, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Mythic Companion', '', '', 0, 1, 1, 0, 188, 0, 1, 1.14286, 1, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 33536, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 134217728, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, 'npc_mythic_custom_companion', 0, 12340);

UPDATE `creature_template` SET `flags_extra` = 2 WHERE `entry` = @MYTHIC_COMPANION_ENTRY;

DELETE FROM `npc_text` WHERE `Id` BETWEEN 91501 AND 91525;
DELETE FROM `npc_text` WHERE `Id` = 91530;
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(91501, "Greetings $n! I'm your trustworthy Mythic Companion. If you or your friends will ever get lost in these dark corners of Mythic Dungeons I will be glad to help you get on the right track!

I am not a well-trained warrior, but, I know these dungeons better than my own pockets, if you ever need my help just ask me and I will tell you about your current progress in this mythic dungeon."),
(91502, "The arcane magic of Kirin Tor doesn't hide any secrets from me, so if you are willing to change my appearance just let me know. I am not familiar with current trends and my style maybe sometimes out of season!"),
(91503, "It’s time $n! Set the level of Mythic dungeon. Once the level is set it cannot be changed.."),
(91504, "There is nothing to see here."),
(91506, "List of items you can use for transmogrification purposes."),
(91507, "You can change my helmet if you want to!"),
(91508, "If you don't like those shoulders just let me know."),
(91509, "Hmm... which shirt should I put on myself today?"),
(91510, "Let's change this chest piece!"),
(91511, "I may have gained some weight recently... Let's pick a more comfortable belt."),
(91512, "I have a rocket in my pocket!"),
(91513, "You sure you want me to take those off?"),
(91514, "Those are shiny! I want one of those."),
(91515, "Let's pick a pair of gloves to wear on my hands"),
(91516, "Choose my cape!"),
(91517, "I would love to wield an extraordinary weapon!"),
(91518, "Don't forget about my off hand!"),
(91519, "I've always enjoyed playing Duck Hunt!"),
(91520, "We can change my tabard if you feel it doesn't match my look!"),
(91521, "Don't be afraid of using my special abilities in need!"),
(91522, "This is the progress of my leveling journey! Along with my increasing level I will get new abilities and features to be even more helpful to you and your group!"),
(91523, "In this tab you can track your Mythic Power currency state! In addition, when you finish Mythic dungeon you can seek the reward for its completion right here!"),
(91524, "Use your mythic power to unlock new tools and obtain otherwise unobtainable items!"),
(91525, "List of items you can use for transmogrification purposes."),
-- 91526 taken by obelisk
(91530, "Hold onto your butts!");

SET @ENTRY := 303000;
DELETE FROM `gameobject_template` WHERE `entry` = @ENTRY;
INSERT INTO `gameobject_template` VALUES
(@ENTRY,34,8171,'Portable Guild Bank','','','',0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_companion_guild_bank',-18019);

UPDATE `item_template` SET `Quality` = 4 WHERE `entry` IN (28023,3774,13612,1195,2081,31824,28489,44236,29645,5600,2705,1911,1902,12787,2810,2184,2715,12943,2550,9701,9700,33089,43076,43077,43078,43079,43072,43073,43074,43075,43080,43081,43082,43083,43068,43069,43070,43071,22476,22477,22478,22481,22482,22483,22479,22480,22436,22437,22438,22441,22442,22443,22439,22440,22416,22417,22418,22421,22422,22423,22419,22420,22488,22489,22490,22493,22494,22495,22491,22492,22496,22497,22498,22501,22502,22503,22499,22500,22504,22505,22506,22509,22510,22511,22507,22508,22425,22424,22426,22427,22428,22431,22429,22430,22464,22465,22466,22469,22470,22471,22467,22468,22512,22513,22514,22517,22518,22519,22515,22516,3934172849984,13316,29410,28905,143,29419,34589,34590,34591,17142,49733,50248,50431,49689,43093,43617,43618,43619,42776,39754,49873,49708,49645,43267,42940,49340,44705,32615,52062,52011,35757,29712,30414,37579,22658,22652,22655,22654,22700,22661,22663,22662,22701,22664,22665,22666,22702,22669,22671,22670,22699,22709,13262,50442,50840,49706);