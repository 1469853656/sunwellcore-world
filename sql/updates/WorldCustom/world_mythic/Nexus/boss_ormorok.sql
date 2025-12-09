SET @ORMO_MINION:= 91706;

DELETE FROM `creature_template` WHERE `entry` = @ORMO_MINION;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@ORMO_MINION, 0, 0, 0, 0, 0, 26298, 0, 0, 0, 'Ormorok\'s minion', '', '', 0, 81, 81, 2, 16, 0, 1, 2.8, 1, 1, 304, 436, 0, 296, 10, 2000, 2000, 2, 32832, 2048, 0, 0, 0, 0, 0, 0, 268, 399, 40, 7, 104, 26731, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4155, 6925, '', 0, 3, 1, 12.5, 15, 1, 0, 0, 0, 0, 0, 0, 0, 144, 1, 650854271, 2097152, 'npc_ormorok_minion_mythic', 0, 12340);

UPDATE `creature_template` SET `scale` = 0.3, `MythicTemplateId` = @ORMO_MINION, `speed_run` = 1.14 WHERE `entry` = @ORMO_MINION;

DELETE FROM `creature_template_mythic` WHERE `Id` = @ORMO_MINION;
INSERT INTO `creature_template_mythic` VALUES
(@ORMO_MINION, 1, 81, 0, 25000, 0, 6000, 8500, 0.1, 0.03, 0); 
