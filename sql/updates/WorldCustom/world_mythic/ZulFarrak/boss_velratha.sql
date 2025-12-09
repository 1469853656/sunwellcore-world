SET @GAHZRILLA_2:= 91506;
SET @FROZEN_ORB_TRIGGER:= 91507;
SET @WATER_TOMB_TRIGGER:= 91704;

DELETE FROM `creature_template` WHERE `entry` IN (@GAHZRILLA_2, @FROZEN_ORB_TRIGGER, @WATER_TOMB_TRIGGER);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@GAHZRILLA_2, 0, 0, 0, 0, 0, 7271, 0, 0, 0, 'Gahz\'rilla', NULL, NULL, 0, 46, 46, 0, 107, 0, 1.6, 1.42857, 1, 1, 77, 102, 0, 188, 7.5, 2000, 1402, 1, 32768, 2048, 0, 0, 0, 0, 0, 0, 53, 78, 17, 10, 0, 7273, 0, 7273, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 1, 3, 1, 10, 2, 1, 0, 8707, 0, 0, 0, 0, 0, 150, 1, 0, 0, 'npc_gahzrilla_mythic', 19, 12340),
(@FROZEN_ORB_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Hydromancer Velratha', 'frozenorb trigger', NULL, 0, 46, 46, 0, 107, 0, 1.6, 1.42857, 1, 1, 77, 102, 0, 188, 7.5, 2000, 1402, 1, 32768, 2048, 0, 0, 0, 0, 0, 0, 53, 78, 17, 10, 0, 7273, 0, 7273, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 1, 7, 3, 10, 2, 1, 0, 8707, 0, 0, 0, 0, 0, 150, 1, 0, 128, 'npc_velratha_frozen_orb_mythic', 0, 12340),
(@WATER_TOMB_TRIGGER, 0, 0, 0, 0, 0, 20782, 0, 0, 0, 'Water Tomb', '', NULL, 0, 80, 80, 0, 107, 0, 1.6, 1.42857, 1, 1, 77, 102, 0, 188, 7.5, 2000, 1402, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 53, 78, 17, 10, 0, 7273, 0, 7273, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 1, 7, 3, 10, 2, 1, 0, 8707, 0, 0, 0, 0, 0, 150, 1, 0, 0, 'npc_velratha_water_tomb', 0, 12340);

UPDATE `creature_template` SET `HoverHeight` = 2.5 WHERE `entry` = @FROZEN_ORB_TRIGGER;
UPDATE `creature_template` SET `ScriptName` = "npc_massive_geyser_trigger" WHERE `entry` = 14122;

DELETE FROM `creature_template_addon` WHERE `entry` = @FROZEN_ORB_TRIGGER;
INSERT INTO `creature_template_addon` (`entry`, `bytes1`) VALUES
(@FROZEN_ORB_TRIGGER, 50331648);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (63136, 72082);
INSERT INTO `spell_script_names` VALUES
(63136, "spell_frozen_embrace_remove"),
(72082, "spell_frozen_orb_damage_mythic");
