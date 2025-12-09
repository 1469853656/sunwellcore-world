-- Issue 3) Betrayal http://db.darkwizard.pl/?quest=12713 Not necessarily bugged, but big % of players don't do enough damage to it (friendly mobs damage), and they end up making a ton of tickets. If we could remove requirement for % of damage to award quest item, it would help
-- Solution: add flag, increase pets damage
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 16777216 WHERE `entry` = 28998;
UPDATE `creature_template` SET `dmg_multiplier` = 10 WHERE `entry` = 28931;

-- Issue 6) Tome of Divinity (Human) http://db.darkwizard.pl/?item=6775 If player deletes this, I didn't see the quest to get it back (repetable if lost)
-- Solution: add custom gossip to restore item to npc 6171. Conditions: Quest 1641 rewarded, Quest 1642 not taken, item not in bag/bank
UPDATE `creature_template` SET `npcflag` = `npcflag` | 1, `gossip_menu_id` = 57000, `AIName` = "SmartAI" WHERE `entry` = 6171;

DELETE FROM conditions WHERE SourceTypeOrReferenceId=15 AND SourceGroup=57000 AND SourceEntry=1;
INSERT INTO conditions VALUES
(15, 57000, 1, 0, 0, 8, 0, 1641, 0, 0, 0, 0, 0, '', 'The Tome of Divinity 1641 rewarded'),
(15, 57000, 1, 0, 0, 14, 0, 1642, 0, 0, 0, 0, 0, '', 'The Tome of Divinity 1642 not taken'),
(15, 57000, 1, 0, 0, 2, 0, 6775, 1, 0, 1, 0, 0, '', 'Must not have item 6775');
DELETE FROM gossip_menu_option WHERE menu_id=57000 AND id=1;
INSERT INTO gossip_menu_option VALUES
(57000, 1, 0, "I've lost my Tome of Divinity.", 1, 1, 0, 0, 0, 0, "");
DELETE FROM smart_scripts WHERE entryorguid=6171 AND source_type=0 AND id IN(0,1);
INSERT INTO smart_scripts VALUES
(6171, 0, 0, 1, 62, 0, 100, 0, 57000, 1, 0, 0, 56, 6775, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "On Gossip Select - Add item"),
(6171, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "On Gossip Select - Close Gossip");
-- Issue 8) Un'goro Pylons http://db.darkwizard.pl/?quest=4287 http://db.darkwizard.pl/?quest=4285 http://db.darkwizard.pl/?quest=4288 If player is either mounted or in a party group, quest won't complete and he yells bugged server. If we could make it complete even while in a party or mounted, it would decrease ticket amount.
-- Solution: changed smart_script's action type
UPDATE `smart_scripts` SET `action_type` = 26 WHERE `entryorguid` = 164957 AND `id` = 0 AND `source_type` = 1;
UPDATE `smart_scripts` SET `action_type` = 26 WHERE `entryorguid` = 164955 AND `id` = 0 AND `source_type` = 1;
UPDATE `smart_scripts` SET `action_type` = 26 WHERE `entryorguid` = 164956 AND `id` = 0 AND `source_type` = 1;
-- Issue 10) The Light of Dawn http://db.darkwizard.pl/?quest=12801 Not really bugged, but creates confusion and player opens ticket. Once script finishes, Highlord Darion Mograine is despawned and his corpse is back on the hill, takes a bit for him to respawn, wonder if we could speed up his respawn timer.
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `id` = 29173;

-- Issue 13) The Purification of Quel'Delar http://db.darkwizard.pl/?search=The+Purification+of+Quel%27Delar Not really bugged, but almost everyone enters Sunwell Plateau in raid group to complete it :| and opens tickets, could we possibly change it to be completed even while in a raid group? Works normally in normal party group
UPDATE `quest_template` SET `Flags` = `Flags` | 64, `Type` = 88 WHERE `id` = 24553;
-- 14) Watcher Leesa'oh ❌ http://db.darkwizard.pl/?quest=9697 No idea how it gets bugged, we get at least ~50 tickets per every realm lifespan, npc gets stuck and dosent continue RP script.
-- Ticket "Dear GM´s, Subject: NPC Watcher Leesa'oh / ID 17831 / bugged after event. Topic on bugtracker forum: [Zangarmarsh - Quest] Watcher Leesa'oh uncompletable #727 The NPC is bugged after an event. It should reset after 5 min.
-- I am waitung for \~13 min. Please fix / unbug the NPC. Thank you in advance. BR Jimmylizzle"
DELETE FROM `creature_template` WHERE `entry` = 17891 AND `modelid1` = 17271;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`, `MythicTemplateId`) VALUES
(17891, 0, 0, 0, 0, 0, 17271, 0, 0, 0, 'Watcher Leesa\'oh', NULL, NULL, 7495, 65, 65, 1, 1660, 3, 1.125, 1.14286, 1, 0, 186, 260, 0, 262, 1, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 153, 227, 29, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'SmartAI', 1, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, '', 12340, 0);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 17831;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 1783100;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(17831, 0, 0, 1, 20, 1, 100, 0, 9709, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Quest Reward - Store Target List'),
(17831, 0, 1, 0, 61, 1, 100, 0, 0, 0, 0, 0, 80, 1783100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Quest Reward - Start Script'),
(17831, 0, 2, 0, 25, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Reset - Set Phase 1'),
(1783100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 17891, 3, 117000, 0, 0, 0, 8, 0, 0, 0, -283.86, 8302.74, 19.71, 0.78, 'Watcher Leesa\'oh - Script - Summon Leesa Temp'),
(1783100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 9, 17891, 0, 10, 1, 0, 0, 0, 'Watcher Leesa\'oh - Script - Send Stored target to Leesa'),
(1783100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Set Phase 2'),
(1783100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh -Script - Set Active True'),
(1783100, 9, 4, 0, 0, 0, 100, 0, 60000, 60000, 10000, 10000, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh -Script - Set Phase 1'),
(1783100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Set Active False');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 17891;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(17891, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 12, 18193, 3, 117000, 0, 0, 0, 8, 0, 0, 0, -287.32, 8308.58, 19.84, 0.1, 'Watcher Leesa\'oh - Summon Buddy - Run Script'),
(17891, 0, 1, 6, 61, 0, 100, 0, 0, 0, 0, 0, 80, 1789101, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Just Summoned - Run Script'),
(17891, 0, 2, 3, 40, 0, 100, 0, 13, 17831, 0, 0, 54, 50000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Reached WP21 - Pause WP'),
(17891, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 1789100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Reached WP13 - Run Script'),
(17891, 0, 4, 5, 40, 0, 100, 0, 30, 17831, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.925025, 'Watcher Leesa\'oh - On Reached WP41 - Set Orientation'),
(17891, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - On Reached WP31 - Set NPC Flags'),
(17891, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Quest Reward - Set Phase 1'),
(17891, 0, 7, 8, 1, 1, 100, 0, 300000, 300000, 0, 0, 81, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - OOC (Phase 1) - Set NPC Flags'),
(17891, 0, 8, 0, 61, 1, 100, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - OOC (Phase 1) - Set Phase 0');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 1789101;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(1789101, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script 2 - Set Npc Flags'),
(1789101, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 7000, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script 2 - Say Line 1'),
(1789101, 9, 3, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 17953, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script 2 - Set Data 1 1 on Buddy'),
(1789101, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 53, 0, 17831, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script 2 - Start WP');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 1789100;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(1789100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Say Line 2'),
(1789100, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 11, 32618, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Cast'),
(1789100, 9, 2, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Say Line 3'),
(1789100, 9, 3, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 50, 182073, 21, 0, 0, 0, 0, 8, 0, 0, 0, -293.135, 8218.52, 22.2616, 2.75761, 'Watcher Leesa\'oh - Script - Spawn Grown Mushroom'),
(1789100, 9, 4, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 12, 17955, 1, 30000, 0, 0, 0, 8, 0, 0, 0, -362.376, 8215.58, 25.2591, 0.379451, 'Watcher Leesa\'oh - Script - Spawn Hungry Boglord'),
(1789100, 9, 5, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 17955, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Say Line 1 on Hungry Boglord'),
(1789100, 9, 6, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 17953, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Set Data 2 2 on Buddy'),
(1789100, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Say Line 4'),
(1789100, 9, 8, 0, 0, 0, 100, 0, 9000, 9000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 19, 17955, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Say Line 2 on Hungry Boglord'),
(1789100, 9, 9, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'Watcher Leesa\'oh - Script - Say Line 4');

DELETE FROM `creature_text` WHERE `entry` = 17891;
INSERT INTO `creature_text` VALUES 
(17891, 0, 0, '$n, let\'s go see if this plan will work.  Come along, Buddy!', 12, 0, 100, 0, 0, 0, 0, 'Watcher Leesa\'oh'),
(17891, 1, 0, 'Ok, let\'s see about using those mushrooms you brought back to grow the bog lords a new food supply.', 12, 0, 100, 0, 0, 0, 0, 'Watcher Leesa\'oh'),
(17891, 2, 0, 'Wait and see what happens!', 12, 0, 100, 0, 0, 0, 0, 'Watcher Leesa\'oh'),
(17891, 3, 0, 'Oh Buddy, you big baby!  Look, it\'s working.  It\'s really working!!  Now all I need to do is grow more of these mushrooms here and the bog lords will likely leave the sporelings alone!', 12, 0, 100, 0, 0, 0, 0, 'Watcher Leesa\'oh'),
(17891, 4, 0, 'Oh, thank you, $n!  You\'ve made it possible to save both the sporelings from the bog lords, and the bog lords from our wrath.', 12, 0, 100, 0, 0, 0, 0, 'Watcher Leesa\'oh');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 18193;
INSERT INTO `smart_scripts` VALUES
(18193, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 29, 2, 180, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Buddy - On Summon - Follow Watcher Leesa\'oh '),
(18193, 0, 1, 2, 1, 0, 100, 0, 0, 30000, 60000, 75000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - OOC - Play emote OneShotAttack1H'),
(18193, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 4, 643, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - OOC - Play Sound ID 643'),
(18193, 0, 3, 0, 25, 0, 100, 0, 0, 0, 0, 0, 89, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - On Reset Set Random Movement'),
(18193, 0, 4, 5, 38, 0, 100, 0, 2, 2, 0, 0, 29, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - On Data Set - Follow Watcher Leesa\'oh '),
(18193, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 53, 0, 17953, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - OOC - Play Sound ID 643'),
(18193, 0, 6, 0, 40, 0, 100, 0, 1, 17953, 0, 0, 89, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - On Reached WP1 - Set Random Movement');

DELETE FROM `creature_template` WHERE `entry` = 18193 AND `modelid1` = 6448;
INSERT INTO `creature_template` VALUES 
(18193, 0, 0, 0, 0, 0, 6448, 0, 0, 0, 'Buddy', '', NULL, 0, 65, 65, 1, 1710, 0, 1.125, 1.14286, 1, 0, 200, 281, 0, 278, 1, 2000, 2000, 1, 4096, 2048, 0, 0, 0, 0, 0, 0, 166, 246, 35, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'SmartAI', 1, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 146, 1, 0, 2, '', 12340, 0);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 17953;
INSERT INTO `smart_scripts` VALUES 
(17953, 0, 0, 1, 1, 0, 100, 0, 0, 30000, 60000, 75000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - OOC - Play emote OneShotAttack1H'),
(17953, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 4, 643, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - OOC - Play Sound ID 643'),
(17953, 0, 2, 0, 25, 0, 100, 0, 0, 0, 0, 0, 89, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Buddy - On Reset Set Random Movement');
-- 19. Patching Up ❌ http://db.darkwizard.pl/?quest=11894 There are not enough invisible objects for players and they end up opening tickets, please add these objects at these locations 268986 (Entry: 188086) - [Geyser Fields Steam Vent X:3855.689941 Y:4743.490234 Z:-5.769280 MapId:571] 
SET @OGUID = 300071;
DELETE FROM `gameobject` WHERE `id` = 188086 AND `guid` BETWEEN @OGUID AND @OGUID+8;
INSERT INTO `gameobject` VALUES
(@OGUID, 188086, 571, 1, 1,4032.67,4498.12,9.73, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+1, 188086, 571, 1, 1,3981.02,4622.97,10.18, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+2, 188086, 571, 1, 1,4285.14,4742.22,21.50, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+3, 188086, 571, 1, 1,3565.62,4607.36,-4.94, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+4, 188086, 571, 1, 1,3507.74,4648.24,-11.91, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+5, 188086, 571, 1, 1,3466.82,4712.69,-7.54, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+6, 188086, 571, 1, 1,3608.25,4695.55,-9.71, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+7, 188086, 571, 1, 1,3646.28,4807.99,-9.65, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0),
(@OGUID+8, 188086, 571, 1, 1,3722.99,4740.36,-8.65, 0, 0, 0, 0.53903, 0.842287, 300, 0, 1, 0);
