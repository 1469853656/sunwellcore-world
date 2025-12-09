DELETE FROM `spell_script_names` WHERE `spell_id` IN (30940, 38841, 30939);
INSERT INTO `spell_script_names` VALUES
(30940, "spell_kelidan_burning_nova"),
(38841, "spell_kelidan_fiery_marker"),
(30939, "spell_rage_of_magtheridon_targets");


SET @KELIDAN_FLAMES_TRIGGER:= 91518;
SET @KELIDAN_FLAMES_TRIGGER2:= 91519;
SET @KELIDAN_SAFE_ZONE_MARKER:= 91550;
SET @KELIDAN_METEOR_TARGET:= 91551;
SET @KELIDAN_MAGTHERIDON_ABYSSAL_TARGET:= 91552;
SET @KELIDAN_ELEMENTAL:= 91553;

DELETE FROM `creature_template` WHERE `entry` IN (@KELIDAN_FLAMES_TRIGGER, @KELIDAN_FLAMES_TRIGGER2, @KELIDAN_SAFE_ZONE_MARKER, @KELIDAN_METEOR_TARGET, @KELIDAN_MAGTHERIDON_ABYSSAL_TARGET, @KELIDAN_ELEMENTAL);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@KELIDAN_FLAMES_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Flames', 'Kelidan mythic initial flames', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, 'npc_kelidan_flames_initial', 0, 12340),
(@KELIDAN_FLAMES_TRIGGER2, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Flames', 'Kelidan mythic flames', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, 'npc_kelidan_flame_secondary', 0, 12340),
(@KELIDAN_SAFE_ZONE_MARKER, 0, 0, 0, 0, 0, 25144, 0, 0, 0, 'Safe zone marker', '', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, '', 0, 12340),
(@KELIDAN_METEOR_TARGET, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Keli\'dan the breaker', 'meteor target', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, 'npc_kelidan_meteor_target_mythic', 0, 12340),
(@KELIDAN_MAGTHERIDON_ABYSSAL_TARGET, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Magtheridon meteor target', 'meteor target', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, '', 0, 12340),
(@KELIDAN_ELEMENTAL, 0, 0, 0, 0, 0, 19113, 0, 0, 0, 'Essence of the light', '', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, 'npc_kelidan_mythic_elemental', 0, 12340);

UPDATE `creature_template` SET `InhabitType` = 7 WHERE `entry` IN (@KELIDAN_METEOR_TARGET, @KELIDAN_SAFE_ZONE_MARKER);
UPDATE `creature_template` SET `scale` = 0.75 WHERE `entry` IN (@KELIDAN_FLAMES_TRIGGER, @KELIDAN_FLAMES_TRIGGER2);
UPDATE `gameobject_template` SET data11 = 0 WHERE entry IN (181921, 181922, 181923, 181924, 181925, 181926, 181927);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (64566, 59998);
INSERT INTO `spell_script_names` VALUES
(64566, "spell_flames_kelidan_mythic"),
(59998, "spell_kelidan_conse_immunity");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceEntry` = 62304;
INSERT INTO `conditions` VALUES
(13, 1, 62304, 0, 0, 31, 0, 3, 33104, 0, 0, 0, 0, '', 'Cosmic Smash - target trigger'),
(13, 1, 62304, 0, 1, 31, 0, 3, @KELIDAN_METEOR_TARGET, 0, 0, 0, 0, '', 'Cosmic Smash - target trigger');

UPDATE `creature_template` SET `ScriptName` = "npc_magtheridon_furnace_mythic" WHERE `entry` = 21174;

DELETE FROM `creature_template_mythic` WHERE `Id` IN (17454, @KELIDAN_ELEMENTAL);
INSERT INTO `creature_template_mythic` VALUES
(17454, 1, 83, 0, 100000, 100000, 10000, 10000, 0, 0, 0),
(@KELIDAN_ELEMENTAL, 1, 83, 0, 35000, 35000, 0, 0, 0, 0, 0);

UPDATE `creature_template` SET `MythicTemplateId` = @KELIDAN_ELEMENTAL WHERE `entry` = @KELIDAN_ELEMENTAL;

UPDATE `creature_template` SET `MythicTemplateId` = 17454, `ScriptName` = "npc_burning_abyssmal_mythic" WHERE `entry` = 17454;

DELETE FROM `creature_template_mythic` WHERE `Id` = 21174;
INSERT INTO `creature_template_mythic` VALUES
(21174, 1, 83, 0, 133333, 0, 35000, 45000, 0.03, 1, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 21174 WHERE `entry` = 21174;
