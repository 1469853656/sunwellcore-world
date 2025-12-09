SET @AMZ_TRIGGER_ANOMALUS:= 91523;
SET @CHAOTIC_RIFT_MYTHIC:= 91524;
SET @CHAOTIC_RIFT_WRAITH:= 91525;

DELETE FROM `creature_template` WHERE `entry` IN (@AMZ_TRIGGER_ANOMALUS, @CHAOTIC_RIFT_MYTHIC, @CHAOTIC_RIFT_WRAITH);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@AMZ_TRIGGER_ANOMALUS, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Anti-Magic Zone', '', '', 0, 83, 83, 0, 35, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, 'npc_anomalus_mythic_amz', 0, 12340),
(@CHAOTIC_RIFT_MYTHIC, 0, 0, 0, 0, 0, 25206, 0, 0, 0, 'Chaotic Rift', '', '', 0, 83, 83, 2, 16, 0, 1, 1.14286, 1.5, 1, 299, 426, 0, 308, 7.5, 2000, 2000, 1, 131076, 2048, 0, 0, 0, 0, 0, 0, 262, 389, 48, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1.8, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 650854271, 2097152, 'npc_chaotic_rift_mythic', 73, 12340),
(@CHAOTIC_RIFT_WRAITH, 0, 0, 0, 0, 0, 27810, 0, 0, 0, 'Crazed Mana-Wraith', '', '', 0, 71, 71, 2, 16, 0, 1, 1.14286, 1, 0, 255, 373, 0, 137, 1, 2000, 2000, 8, 32768, 2048, 0, 0, 0, 0, 0, 0, 240, 358, 31, 4, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 0.75, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 64, 'npc_crazed_mana_wraith_nexus_mythic', 72, 12340);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (63491, 33832);
INSERT INTO `spell_script_names` VALUES
(63491, "spell_lightning_blast_anomalus"),
(33832, "spell_arcane_wraith_arcane_missiles_mythic");

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` = 33833;
INSERT INTO `creature_mythic_spell_values` VALUES
(33833, 500, 0, 0, 0, 1, 0.05);

SET @MYTHIC_TEMPLATE_ID:= 82;
DELETE FROM `creature_template_mythic` WHERE `Id` = @MYTHIC_TEMPLATE_ID;
INSERT INTO `creature_template_mythic` VALUES
(@MYTHIC_TEMPLATE_ID, 1, 83, 0, 1000000, 0, 15000, 25000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = @MYTHIC_TEMPLATE_ID WHERE `entry` = 26763;
