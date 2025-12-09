SET @AIR_PHASE_TRIGGER:= 91527;
SET @CRYSTALFIREBREATH_TRIGGER:= 91528;
SET @KERI_WHELP:= 91529;
SET @WATER_BUBBLE_TRIGGER:= 91530;
SET @FROSTSPHERE:= 91705;
SET @EGGRESTORE:= 91708;

UPDATE `creature` SET `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 126453;
 
DELETE FROM `creature_template` WHERE `entry` IN (@EGGRESTORE, @FROSTSPHERE, @AIR_PHASE_TRIGGER, @CRYSTALFIREBREATH_TRIGGER, @KERI_WHELP, @WATER_BUBBLE_TRIGGER);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@AIR_PHASE_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Keristrasza air trigger', '', '', 0, 83, 83, 2, 35, 0, 1, 2.8, 1, 1, 304, 436, 0, 296, 10, 2000, 2000, 2, 32832, 2048, 0, 0, 0, 0, 0, 0, 268, 399, 40, 7, 104, 26731, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4155, 6925, '', 0, 3, 1, 12.5, 15, 1, 0, 0, 0, 0, 0, 0, 0, 144, 1, 650854271, 0, '', 0, 12340),
(@CRYSTALFIREBREATH_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Keristrasza breath trigger', '', '', 0, 83, 83, 2, 35, 0, 1, 2.8, 1, 1, 304, 436, 0, 296, 10, 2000, 2000, 2, 32832, 2048, 0, 0, 0, 0, 0, 0, 268, 399, 40, 7, 104, 26731, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4155, 6925, '', 0, 3, 1, 12.5, 15, 1, 0, 0, 0, 0, 0, 0, 0, 144, 1, 650854271, 128, '', 0, 12340),
(@KERI_WHELP, 0, 0, 0, 0, 0, 20138, 0, 0, 0, 'Keristrasza Whelp', '', NULL, 0, 83, 83, 1, 16, 0, 1, 1.14286, 1, 0, 204, 287, 0, 272, 1, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 170, 253, 31, 2, 0, 20021, 0, 70171, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 7, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, '', 0, 12340),
(@WATER_BUBBLE_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Water bubble', '', NULL, 0, 83, 83, 1, 16, 0, 1, 1.14286, 1, 0, 204, 287, 0, 272, 1, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 170, 253, 31, 2, 0, 20021, 0, 70171, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 7, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 128, '', 0, 12340),
(@FROSTSPHERE, 0, 0, 0, 0, 0, 25144, 0, 0, 0, 'Frost Sphere', '', '', 0, 83, 83, 0, 35, 0, 1, 1.14286, 1, 3, 496, 674, 0, 783, 35, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 365, 529, 98, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 'npc_frost_sphere_keristrasza_mythic', 0, 12340),
(@EGGRESTORE, 0, 0, 0, 0, 0, 15512, 0, 0, 0, 'Egg of the Renewed Life', NULL, NULL, 0, 83, 83, 0, 16, 0, 1, 1.14286, 1, 0, 94, 125, 0, 228, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 65, 95, 23, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'PassiveAI', 0, 3, 1, 10, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, '', 0, 12340);

UPDATE `creature_template` SET `speed_run` = 1.14 WHERE `entry` IN (26723, 30540);
UPDATE `creature_template` SET `MythicTemplateId` = @EGGRESTORE WHERE entry = @EGGRESTORE;
UPDATE `creature_template` SET `InhabitType` = 7, `HoverHeight` = 1.5, `scale` = 1.6, `MythicTemplateId` = @KERI_WHELP WHERE `entry` = @KERI_WHELP;
DELETE FROM `creature_template_mythic` WHERE `Id` IN (@KERI_WHELP, @EGGRESTORE);
INSERT INTO `creature_template_mythic` VALUES
(@KERI_WHELP, 1, 81, 0, 35000, 0, 3000, 4000, 0.02, 0.03, 0),
(@EGGRESTORE, 6, 81, 0, 350000, 0, 100, 100, 0, 0.17, 0);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (48096, 35923, 48094);
INSERT INTO `spell_script_names` VALUES
(48096, "spell_keristrasza_crystalfire_breath"),
(35923, "spell_water_bubble_head"),
(48094, "spell_keristasza_intense_cold");

DELETE FROM `creature_template_addon` WHERE `entry` = @KERI_WHELP;
INSERT INTO `creature_template_addon` (`entry`, `bytes1`) VALUES
(@KERI_WHELP, 50331648);

SET @GO_PORTAL:= 302104;
DELETE FROM `gameobject_template` WHERE `entry` = @GO_PORTAL;
INSERT INTO `gameobject_template` VALUES
(@GO_PORTAL, 10, 1327, 'Exit Portal', '', '', '', 35, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60474, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '', '', 12340);

UPDATE `gameobject_template` SET `ScriptName` = "go_portal_keristrasza_mythic", `faction` = 35, `data0` = 43, `data1` = -1, `data4` = 1, `data5` = 1 WHERE `entry` = @GO_PORTAL;



 