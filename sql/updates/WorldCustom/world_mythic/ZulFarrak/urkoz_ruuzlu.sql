SET @BLACKBEAM_TRIGGER:= 91508;
SET @FIRE_TRIGGER_CHIEF:= 91509;
SET @SNAKE_HEAD_TRIGGER:= 91510;
SET @IMPALING_AXE_TRIGGER:= 91511;
SET @ZOMBIE_TROLL:= 91707;

DELETE FROM `creature_template` WHERE `entry` IN (@ZOMBIE_TROLL, @BLACKBEAM_TRIGGER, @FIRE_TRIGGER_CHIEF, @SNAKE_HEAD_TRIGGER, @IMPALING_AXE_TRIGGER);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@BLACKBEAM_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Chief Ukorz Sandscalp beam target', 'Ukorz blackbeam trigger', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, '', 0, 12340),
(@FIRE_TRIGGER_CHIEF, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Chief Ukorz Sandscalp', 'Ukorz blackbeam trigger', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, 'npc_urkoz_fire_path_trigger', 0, 12340),
(@SNAKE_HEAD_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Chief Ukorz Sandscalp', 'Snakehead blackbeam caster trigger', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, '', 0, 12340),
(@IMPALING_AXE_TRIGGER, 0, 0, 0, 0, 0, 31004, 0, 0, 0, 'Impaling Axe', '', 'Interact', 0, 80, 80, 2, 2209, 16777216, 1, 1.71429, 1, 1, 2, 2, 0, 24, 7.5, 2000, 2000, 1, 2, 2048, 0, 0, 0, 0, 0, 0, 1, 1, 0, 6, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 14, 1, 1, 0, 0, 0, 0, 0, 0, 0, 164, 1, 8388624, 2097152, '', 0, 12340),
(@ZOMBIE_TROLL, 0, 0, 0, 0, 0, 6417, 6418, 0, 0, 'Zul\'Farrak Zombie', NULL, NULL, 0, 43, 44, 0, 16, 0, 1.68, 1.14286, 1, 1, 72, 96, 0, 174, 7.5, 1258, 1384, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 49, 73, 16, 6, 0, 7286, 7286, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 300, 396, '', 1, 3, 1, 3, 1, 1, 0, 9523, 0, 0, 0, 0, 0, 0, 1, 8388624, 0, 'npc_zulfarrak_zombie_ukorz', 16, 12340);

UPDATE `creature_template` SET `speed_run` = 0.5 WHERE `entry` = @ZOMBIE_TROLL;
UPDATE `creature_template` SET `InhabitType` = 7 WHERE `entry` = @SNAKE_HEAD_TRIGGER;
DELETE FROM `creature_equip_template` WHERE `entry` = @IMPALING_AXE_TRIGGER;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES
(@IMPALING_AXE_TRIGGER, 1, 5288, 0, 0, 0);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (68442, 53240, 69660, 78723);
INSERT INTO `spell_script_names` VALUES
(68442, "spell_kneel_mythic"),
(53240, "spell_weapon_throw_ruuzlu"),
(69660, "spell_mythic_burning_pitch_mod_dest"),
(78723, "spell_ruuzlu_enrage_proc");

UPDATE `creature_template` SET `ScriptName` = "npc_tornado_ruuzlu_mythic" WHERE `entry` = 19922;

DELETE FROM `spell_script_names` WHERE `spell_id` = 17148;
INSERT INTO `spell_script_names` VALUES
(17148, "spell_zombie_brain_hacker");

DELETE FROM `creature_template_mythic` WHERE `Id` = @ZOMBIE_TROLL;
INSERT INTO `creature_template_mythic` VALUES
(@ZOMBIE_TROLL, 1, 81, 0, 50000, 0, 4000, 5000, 0.03, 0.03, 0);

UPDATE `creature_template` SET `MythicTemplateId` = @ZOMBIE_TROLL WHERE `entry` = @ZOMBIE_TROLL;