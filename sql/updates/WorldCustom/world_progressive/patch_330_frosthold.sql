-- Toravon
DELETE FROM `creature` WHERE `guid` = 202600 AND `id` = 38433;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(202600, 38433, 624, 3, 1, 0, 1, -43.3316, -288.708, 91.2511, 1.58825, 604800, 0, 0, 1, 0, 0, 0, 0, 0);
-- Disable quests "Proof of demise: ..."
DELETE FROM `creature_queststarter` WHERE `quest` IN (13245, 13246, 13247, 13248, 13249, 13250, 13251, 13252, 13253, 13254, 13255, 13256);
DELETE FROM `creature_questender` WHERE `quest` IN (13245, 13246, 13247, 13248, 13249, 13250, 13251, 13252, 13253, 13254, 13255, 13256);
-- Disable quests "Timear forces: ..."
DELETE FROM `creature_queststarter` WHERE `quest` IN (13240, 13241, 13243, 13244);
DELETE FROM `creature_questender` WHERE `quest` IN (13240, 13241, 13243, 13244);
-- Add Frozo
DELETE FROM `creature` WHERE `guid` = 202846;
INSERT INTO `creature` VALUES 
(202846, 40160, 571, 1, 1, 0, 1, 5910.08, 720.278, 643.425, 3.26377, 120, 0, 0, 8508, 7981, 0, 0, 0, 0);
-- Add Portable Hole
DELETE FROM `npc_vendor` WHERE `entry` = 18756 AND `item` = 51809;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(18756, 0, 51809, 0, 0, 0);
SET @CONQUEST = 45624;
SET @FROST = 49426;
SET @TRIUMPH = 47241;
SET @HEROISM = 40752;
SET @VALOR = 40753;


UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `item` = @CONQUEST;
-- Marrowgar
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (37958, 37959, 36612, 37957);
-- Lady D
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (38297,38296, 36855, 38106);
-- Festergut
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (36626, 37504, 37505, 37506);
-- Rotface
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (36627, 38390, 38549, 38550);
-- Putricide
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (36678,38431, 38585, 38586);
-- Prince
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (37970,38401,38784,38785);
-- Blood Queen
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (37955, 38434, 38435, 38436);
-- Svalna
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (37126);
-- Sindragosa
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (36853,38265,38266, 38267);
-- Lich King
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (36597, 39166, 39167, 39168);
-- Chesty ICC Gunship Valithria Saurfang-
UPDATE `gameobject_loot_template` SET `item` = @FROST WHERE `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM) AND `entry` IN (28052,28082,28064,28096,28046,28074,28058,28088,28045,28072,28045,28072,28057,28090);
-- Pozostałe GO upgrade conqest -> triumph
UPDATE `gameobject_loot_template` SET `item` = @TRIUMPH WHERE `item` = @CONQUEST;
-- Toravon 
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` = @TRIUMPH AND `entry` = 38433;
UPDATE `creature_loot_template` SET `item` = @FROST WHERE `item` = @TRIUMPH AND `entry` = 38462;


DELETE FROM `creature_loot_template` WHERE `entry` = 32915 AND `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM);
INSERT INTO `creature_loot_template` VALUES
(32915, @TRIUMPH, 100, 1, 0, 1, 1);

UPDATE `reference_loot_template` SET `item` = @TRIUMPH WHERE `item` = @CONQUEST;
UPDATE `item_loot_template` SET `item` = @TRIUMPH WHERE `item` = @CONQUEST;
UPDATE `item_loot_template` SET `item` = @FROST WHERE `entry` = 52006 AND `item` IN (@TRIUMPH,@CONQUEST,@VALOR,@HEROISM);
-- Disable proof of demise
DELETE FROM `disables` WHERE `sourceType` = 4 AND `entry` IN (7300,7301,7302,7303,7304,7305,7306,7307,7308,7309,7310,7311);
INSERT INTO `disables` VALUES
(4,7300,0,'','','Proof of Demise'),
(4,7301,0,'','','Proof of Demise'),
(4,7302,0,'','','Proof of Demise'),
(4,7303,0,'','','Proof of Demise'),
(4,7304,0,'','','Proof of Demise'),
(4,7305,0,'','','Proof of Demise'),
(4,7306,0,'','','Proof of Demise'),
(4,7307,0,'','','Proof of Demise'),
(4,7308,0,'','','Proof of Demise'),
(4,7309,0,'','','Proof of Demise'),
(4,7310,0,'','','Proof of Demise'),
(4,7311,0,'','','Proof of Demise');
DELETE FROM `disables` WHERE `sourceType` = 4 AND `entry` IN (7297,7298,7299,7296);
INSERT INTO `disables` VALUES
(4,7297,0,'','','Timear Foresees'),
(4,7298,0,'','','Timear Foresees'),
(4,7299,0,'','','Timear Foresees'),
(4,7296,0,'','','Timear Foresees');
-- Transmog tokens
DELETE FROM `npc_vendor` WHERE `entry` = 80003;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES 
(80003, 0, 37711, 0, 0, 2997),
(80003, 0, 37711, 0, 0, 2570),
(80003, 0, 37742, 0, 0, 2431),
(80003, 0, 37742, 0, 0, 2687);

SET @EmblemHighPREV := 47241; # Emblem of Triumph
SET @EmblemLowPREV  := 45624; # Emblem of Conquest

SET @EmblemHighCURR := 49426; # Emblem of Frost
SET @EmblemLowCURR  := 47241; # Emblem of Triumph

-- Crusader Orb & Carpet 
DELETE FROM `npc_vendor` WHERE `entry` = 40160 AND `item` IN (47556, 54798);
INSERT INTO `npc_vendor` VALUES 
(40160, 0, 47556, 0, 0, 2986),
(40160, 0, 54798, 0, 0, 2986);

-- JC Rings
DELETE FROM `npc_vendor` WHERE `entry` = 32172 AND `item` IN (51557, 51558, 51559, 51560);
INSERT INTO `npc_vendor` VALUES
(32172, 0, 51557, 0, 0, 2735),
(32172, 0, 51558, 0, 0, 2737),
(32172, 0, 51559, 0, 0, 2736),
(32172, 0, 51560, 0, 0, 2738);

-- WG PVP Alliance
DELETE FROM `npc_vendor` WHERE `entry` = 39172 AND `item` IN (51568,51569,51570,51571, 51572, 51573,51574, 51575, 51576, 51577, 51578, 51579, 51580, 51581);
INSERT INTO `npc_vendor` VALUES
(39172, 0, 51568, 0, 0, 2578),
(39172, 0, 51569, 0, 0, 2578),
(39172, 0, 51570, 0, 0, 2578),
(39172, 0, 51571, 0, 0, 2578),
(39172, 0, 51572, 0, 0, 2579),
(39172, 0, 51573, 0, 0, 2579),
(39172, 0, 51574, 0, 0, 2579),
(39172, 0, 51575, 0, 0, 2579),
(39172, 0, 51576, 0, 0, 2579),
(39172, 0, 51577, 0, 0, 2579),
(39172, 0, 51578, 0, 0, 2579),
(39172, 0, 51579, 0, 0, 2579),
(39172, 0, 51580, 0, 0, 2579),
(39172, 0, 51581, 0, 0, 2579);
-- PvP WG Horde
DELETE FROM `npc_vendor` WHERE `entry` = 39173 AND `item` IN (51568,51569,51570,51571,51572,51573,51574,51575,51576,51577,51578,51579,51580,51581);
INSERT INTO `npc_vendor` VALUES
(39173, 0, 51568, 0, 0, 2578),
(39173, 0, 51569, 0, 0, 2578),
(39173, 0, 51570, 0, 0, 2578),
(39173, 0, 51571, 0, 0, 2578),
(39173, 0, 51572, 0, 0, 2579),
(39173, 0, 51573, 0, 0, 2579),
(39173, 0, 51574, 0, 0, 2579),
(39173, 0, 51575, 0, 0, 2579),
(39173, 0, 51576, 0, 0, 2579),
(39173, 0, 51577, 0, 0, 2579),
(39173, 0, 51578, 0, 0, 2579),
(39173, 0, 51579, 0, 0, 2579),
(39173, 0, 51580, 0, 0, 2579),
(39173, 0, 51581, 0, 0, 2579);

-- RDF rewards
UPDATE `quest_template` SET `RewardItemId1`=@EmblemHighCURR, `RewardItemCount1`=2 WHERE `Id`=24788; -- 1st daily heroic random
UPDATE `quest_template` SET `RewardItemId1`=@EmblemLowCURR, `RewardItemCount1`=2 WHERE `Id`=24789; -- nth daily heroic random
UPDATE `quest_template` SET `RewardItemId1`=@EmblemLowCURR, `RewardItemCount1`=2 WHERE `Id`=24790; -- 1st daily normal random


-- Archmage Lan'dalock
DELETE FROM `creature_queststarter` WHERE `id`=20735;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES 
(20735, 24579),
(20735, 24580),
(20735, 24581),
(20735, 24582),
(20735, 24583),
(20735, 24584),
(20735, 24585),
(20735, 24586),
(20735, 24587),
(20735, 24588),
(20735, 24589),
(20735, 24590);
DELETE FROM `creature_questender` WHERE `id`=20735;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES 
(20735, 24579),
(20735, 24580),
(20735, 24581),
(20735, 24582),
(20735, 24583),
(20735, 24584),
(20735, 24585),
(20735, 24586),
(20735, 24587),
(20735, 24588),
(20735, 24589),
(20735, 24590);
-- Update Archmage Landalock quest rewards
UPDATE `quest_template` SET `RewardItemId1` = @EmblemHighCURR, `RewardItemId2` = @EmblemLowCURR WHERE `id` IN (SELECT `quest` FROM `creature_queststarter` WHERE `id` = 20735);

-- EoF Vendors
DELETE FROM `creature` WHERE `guid` IN (202582, 4761);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(202582, 37941, 571, 1, 1, 0, 1, 5936.93, 509.132, 650.263, 2.28638, 300, 0, 0, 10080, 8814, 0, 0, 0, 0), 
(4761, 37942, 571, 1, 1, 0, 1, 5765.45, 734.38, 653.748, 2.7227, 300, 0, 0, 1, 0, 0, 0, 0, 0);

-- ICC 0% buff
DELETE FROM `spell_area` WHERE `spell` IN (73822, 73821, 73820, 73819, 73818, 73828, 73827, 73826, 73825, 73824);

-- Keg Shaped Treasure
DELETE FROM `item_loot_template` WHERE `entry` = 54535 AND `item` = 49426;
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(54535, 49426, 100, 1, 1, 2, 2);
-- remove reputation
DELETE FROM creature_onkill_reputation WHERE creature_id IN (
37232,
38362, -- Nerubar Broodling
37782,
38312, -- Flesh-eating Insect
37868,
37863,
37934,
37886,
38171,
38727,
38737,
38167,
38725,
38735,
38166,
38724,
38734,
38170,
38723,
38733,
36791,
38169,
38721,
38722, -- Valithria adds
37695,
39309,
39310,
39311, -- Drudge Ghoul
38104,
38105 -- Plagued Zombie
);

-- Brom Gayserstick - furious weapons
SET @NPC = 220000;

DELETE FROM `npc_vendor` WHERE `entry` = @NPC;
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2257 FROM item_template WHERE name LIKE 'Furious%' AND class = 2 AND itemlevel = 232 AND InventoryType != 17 AND entry != 42237 AND entry != 42238; -- Add 1h deadly weapons for 25k honor
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2570 FROM item_template WHERE name LIKE 'Furious%' AND class = 2 AND itemlevel = 232 AND InventoryType = 17 AND entry != 42238; -- Add 2h deadly weapons for 50k honor
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2570 FROM item_template WHERE name LIKE 'Furious%' AND class = 4 AND subclass IN (0,6,8,9,7,10) AND itemlevel = 232; -- Add 2h deadly weapons for 50k honor

DELETE FROM `gameobject_loot_template` WHERE `entry` = 24524 AND `item` = 52676;
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(24524, 52676, 100, 2, 0, 1, 1);

UPDATE `creature` SET `phaseMask` = 1 WHERE `guid` IN 
(201234,201662,201245,201322,201226,
201593,201560,201499,
247161,247165,201253,
247171,247168,247164,
247170,247169,247160,
247166,247162,247163,
247167,201240,200947,
200935,201247,247159,
247158,201433,201572,
201255,201244,201433,201294);

-- Eregos remove weird loot condition
DELETE FROM conditions WHERE sourceentry = 52676 AND sourcegroup = 24524 AND sourcetypeorreferenceid = 4;

-- ICC trash
DELETE FROM `creature_template` WHERE `entry` IN (92000,92001) AND `modelid1` = 26577;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(92000, 92001, 0, 0, 0, 0, 26577, 0, 0, 0, 'Vengeful Fleshreaper', '', '', 0, 80, 80, 2, 21, 0, 2, 1.5, 1, 1, 698, 950, 0, 300, 7, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 341, 506, 80, 6, 0, 37038, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2190, 13090, '', 0, 3, 1, 12.5, 1, 1, 0, 0, 0, 0, 0, 0, 0, 151, 1, 8388624, 536870912, 'npc_icc_vengeful_fleshreaper', 12340),
(92001, 0, 0, 0, 0, 0, 26577, 0, 0, 0, 'Vengeful Fleshreaper (1)', '', '', 0, 80, 80, 2, 21, 0, 1, 1.5, 1, 1, 665, 905, 0, 300, 13, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 341, 506, 80, 6, 0, 38063, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 50, 1, 1, 0, 0, 0, 0, 0, 0, 0, 151, 1, 8388624, 536870912, '', 12340);

DELETE FROM `creature_onkill_reputation` WHERE `creature_id` = 37038;
DELETE FROM `creature_onkill_reputation` WHERE `creature_id` = 38063;

DELETE FROM `creature_onkill_reputation` WHERE `creature_id` = 92000;
INSERT INTO `creature_onkill_reputation` (`creature_id`, `RewOnKillRepFaction1`, `RewOnKillRepFaction2`, `MaxStanding1`, `IsTeamAward1`, `RewOnKillRepValue1`, `MaxStanding2`, `IsTeamAward2`, `RewOnKillRepValue2`, `TeamDependent`) VALUES 
(92000, 1156, 0, 7, 0, 15, 0, 0, 0, 0);
DELETE FROM `creature_onkill_reputation` WHERE `creature_id` = 92001;
INSERT INTO `creature_onkill_reputation` (`creature_id`, `RewOnKillRepFaction1`, `RewOnKillRepFaction2`, `MaxStanding1`, `IsTeamAward1`, `RewOnKillRepValue1`, `MaxStanding2`, `IsTeamAward2`, `RewOnKillRepValue2`, `TeamDependent`) VALUES 
(92001, 1156, 0, 7, 0, 15, 0, 0, 0, 0);


UPDATE `creature` SET `id` = 92000 WHERE `id` = 37038;
UPDATE `creature` SET `id` = 92001 WHERE `id` = 38063;