-- Faction Champions, Lifebloom
SET @SPELL_ENTRY := 66093;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_faction_champions_lifebloom");

-- Faction Champions - Boost:
-- Monroth the Exiled (Horde)
SET @CREATURE_ENTRY := 110000;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 20122, 0, 0, 0, 'Monroth the Exiled', 'Demon Hunter', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 388, 583, 0, 400, 13, 1300, 0, 4, 2, 2048, 8, 0, 0, 0, 0, 0, 0, 0, 0, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_demon_hunter', 12340);
-- Haruln Soundstorm (Horde)
SET @CREATURE_ENTRY := 110001;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29781, 0, 0, 0, 'Haruln Soundstorm', 'Shaman', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 496, 674, 0, 783, 13, 0, 0, 2, 2, 2048, 8, 0, 0, 0, 0, 0, 365, 529, 98, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 250, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_elemental_shaman', 12340);
-- Krall Sharpclaw (Horde)
SET @CREATURE_ENTRY := 110002;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29776, 0, 0, 0, 'Krall Sharpclaw', 'Druid', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 388, 583, 0, 400, 13, 1300, 0, 4, 2, 2048, 8, 0, 0, 0, 0, 0, 0, 0, 0, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_feral_druid', 12340);
-- Yal'u (Horde)
SET @CREATURE_ENTRY := 110003;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29786, 0, 0, 0, 'Yal\'u', 'Hunter', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 496, 674, 0, 783, 18, 0, 0, 2, 2, 2048, 8, 0, 0, 0, 0, 0, 496, 674, 783, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 250, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_beast_master_hunter', 12340);
-- Gor'gaz the Terror (Horde)
SET @CREATURE_ENTRY := 110004;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29792, 0, 0, 0, 'Gor\'gaz the Terror', 'Warlock', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 370, 531, 0, 343, 13, 0, 0, 8, 2, 2048, 8, 0, 0, 0, 0, 0, 365, 542, 84, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_demonology_warlock', 12340);
-- Elaris Chaosbringer (Alliance)
SET @CREATURE_ENTRY := 110005;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 20130, 0, 0, 0, 'Elaris Chaosbringer', 'Demon Hunter', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 388, 583, 0, 400, 13, 1300, 0, 4, 2, 2048, 8, 0, 0, 0, 0, 0, 0, 0, 0, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_demon_hunter', 12340);
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 32837, 32837, 0, 0);
-- Shareem (Alliance)
SET @CREATURE_ENTRY := 110006;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29764, 0, 0, 0, 'Shareem', 'Shaman', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 496, 674, 0, 783, 13, 0, 0, 2, 2, 2048, 8, 0, 0, 0, 0, 0, 365, 529, 98, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 250, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_elemental_shaman', 12340);
-- Uladir the Wild (Alliance)
SET @CREATURE_ENTRY := 110007;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29776, 0, 0, 0, 'Uladir the Wild', 'Druid', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 388, 583, 0, 400, 13, 1300, 0, 4, 2, 2048, 8, 0, 0, 0, 0, 0, 0, 0, 0, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_feral_druid', 12340);
-- Nathan Bloodshot (Alliance)
SET @CREATURE_ENTRY := 110008;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29770, 0, 0, 0, 'Nathan Bloodshot', 'Hunter', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 496, 674, 0, 783, 18, 0, 0, 2, 2, 2048, 8, 0, 0, 0, 0, 0, 496, 674, 783, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 250, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_beast_master_hunter', 12340);
-- Darthon Emberstorm (Alliance)
SET @CREATURE_ENTRY := 110009;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 29777, 0, 0, 0, 'Darthon Emberstorm', 'Warlock', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1, 3, 370, 531, 0, 343, 13, 0, 0, 8, 2, 2048, 8, 0, 0, 0, 0, 0, 365, 542, 84, 7, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 256, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1048833, 'npc_toc_demonology_warlock', 12340);
-- Felguard (Warlock Pet)
SET @CREATURE_ENTRY := 110010;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 14255, 0, 0, 0, 'Felguard', '', '', 0, 80, 80, 2, 16, 0, 1, 1.14286, 1, 0, 417, 582, 0, 608, 1, 0, 0, 2, 0, 2048, 8, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 10, 10, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 1, 1049089, 'npc_toc_pet_warlock_felguard', 12340);
-- Loquenahak (Hunter Pet)
SET @CREATURE_ENTRY := 110011;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 28010, 0, 0, 0, 'Loque\'nahak', 'Mate of Har\'koa', '', 0, 80, 80, 2, 16, 0, 1, 1.14286, 1, 0, 422, 586, 0, 642, 5, 0, 0, 1, 0, 2048, 8, 2, 0, 0, 0, 0, 345, 509, 103, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 55, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1049089, 'npc_toc_pet_hunter_loquenahak', 12340);
-- King Krush (Hunter Pet)
SET @CREATURE_ENTRY := 110012;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 28052, 0, 0, 0, 'King Krush', '', '', 0, 80, 80, 2, 16, 0, 1, 1.14286, 1, 0, 422, 586, 0, 642, 5, 0, 0, 1, 0, 2048, 8, 2, 0, 0, 0, 0, 345, 509, 103, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 55, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1049089, 'npc_toc_pet_hunter_king_krush', 12340);
-- Feral Spirit (Shaman Pet)
SET @CREATURE_ENTRY := 110013;
DELETE FROM `creature_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 0, 0, 0, 0, 0, 21114, 0, 0, 0, 'Spirit Wolf', '', '', 0, 70, 70, 2, 14, 0, 2, 2.14286, 1, 0, 4500, 5500, 0, 304, 1, 1500, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 215, 320, 44, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58861, 58875, 58867, 58857, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 191, 1, 652967935, 0, 'npc_toc_pet_feral_spirit', 12340);

-- Models
-- Monroth the Exiled
SET @CREATURE_ENTRY := 110000;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 32837, 32837, 0, 0);
-- Elaris Chaosbringer
SET @CREATURE_ENTRY := 110005;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 32837, 32837, 0, 0);
-- Gorgrim Shadowcleave
UPDATE `creature_equip_template` SET `itemEntry1`=47884 WHERE `entry`=34458;
-- Birana Stormhoof 
UPDATE `creature_equip_template` SET `itemEntry1`=47892 WHERE `entry`=34451;
-- Ruj'kah
UPDATE `creature_equip_template` SET `itemEntry1`=47302, `itemEntry3`=47267 WHERE `entry`=34448;
-- Ginselle Blightslinger
UPDATE `creature_equip_template` SET `itemEntry1`=48701, `itemEntry2`=47309 WHERE `entry`=34449;
-- Liandra Suncaller
UPDATE `creature_equip_template` SET `itemEntry1`=47322, `itemEntry2`=47287 WHERE `entry`=34445;
-- Malithas Brightblade
UPDATE `creature_equip_template` SET `itemEntry1`=47329 WHERE `entry`=34456;
-- Caiphus the Stern
UPDATE `creature_equip_template` SET `itemEntry1`=47322, `itemEntry2`=47309 WHERE `entry` IN (36119, 34447);
-- Vivienne Blackwhisper
UPDATE `creature_equip_template` SET `itemEntry1`=47892, `itemEntry2`=0 WHERE `entry`=34441;
-- Maz'dinah
UPDATE `creature_equip_template` SET `itemEntry1`=47300, `itemEntry2`=47300 WHERE `entry` IN (36121, 34454);
-- Thrakgar
UPDATE `creature_equip_template` SET `itemEntry1`=47261, `itemEntry2`=47287 WHERE `entry`=34444;
-- Broln Stouthorn
UPDATE `creature_equip_template` SET `itemEntry1`=47314, `itemEntry2`=47314 WHERE `entry`=34455;
-- Harkzog
UPDATE `creature_equip_template` SET `itemEntry1`=47261, `itemEntry2`=47309 WHERE `entry` IN (36124, 34450);
-- Narrhok Steelbreaker
UPDATE `creature_equip_template` SET `itemEntry1`=48695 WHERE `entry` IN (36122, 34453);
-- Tyrius Duskblade
UPDATE `creature_equip_template` SET `itemEntry1`=47737 WHERE `entry`=34461;
-- Kavina Grovesong
UPDATE `creature_equip_template` SET `itemEntry1`=47743 WHERE `entry`=34460;
-- Alyssia Moonstalker
UPDATE `creature_equip_template` SET `itemEntry1`=47114, `itemEntry3`=48711 WHERE `entry`=34448;
-- Noozle Whizzlestick
UPDATE `creature_equip_template` SET `itemEntry1`=48708, `itemEntry2`=47742 WHERE `entry` IN (36114, 34468);
-- Velanaa
UPDATE `creature_equip_template` SET `itemEntry1`=47809, `itemEntry2`=47079 WHERE `entry`=34465;
-- Baelnor Lightbearer
UPDATE `creature_equip_template` SET `itemEntry1`=47519 WHERE `entry` IN (36109, 34471);
-- Anthar Forgemender
UPDATE `creature_equip_template` SET `itemEntry1`=47809, `itemEntry2`=47742 WHERE `entry` IN (36108, 34466);
-- Brienna Nightfell
UPDATE `creature_equip_template` SET `itemEntry1`=47743 WHERE `entry`=34473;
-- Irieth Shadowstep
UPDATE `creature_equip_template` SET `itemEntry1`=46969, `itemEntry2`=46969 WHERE `entry`=34445;
-- Saamul
UPDATE `creature_equip_template` SET `itemEntry1`=47724, `itemEntry2`=47079 WHERE `entry`=34470;
-- Shaabad
UPDATE `creature_equip_template` SET `itemEntry1`=47816, `itemEntry2`=47816 WHERE `entry`=34463;
-- Serissa Grimdabbler
UPDATE `creature_equip_template` SET `itemEntry1`=47724, `itemEntry2`=47742 WHERE `entry`=34474;
-- Shocuul
UPDATE `creature_equip_template` SET `itemEntry1`=47069 WHERE `entry` IN (36118, 34475);
-- Shareem
SET @CREATURE_ENTRY := 110006;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 47724, 47079, 0, 0);
-- Darthon Emberstorm
SET @CREATURE_ENTRY := 110009;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 47724, 47742, 0, 0);
-- Nathan Bloodshot
SET @CREATURE_ENTRY := 110008;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 47114, 0, 48711, 0);
-- Haruln Soundstorm
SET @CREATURE_ENTRY := 110001;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 47261, 47287, 0, 0);
-- Gor'gaz the Terror
SET @CREATURE_ENTRY := 110004;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 47261, 47309, 0, 0);
-- Yal'u
SET @CREATURE_ENTRY := 110003;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 47302, 0, 47267, 0);
