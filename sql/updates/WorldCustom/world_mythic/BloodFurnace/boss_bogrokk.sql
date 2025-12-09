SET @SHADOW_PORTAL_TRIGGER:= 91515;
SET @BROGOKK_BEAM_TRIGGER:= 91516;
SET @BROGOKK_BEAM_VICTIM:= 91517;
-- xxx18 and xxx19 is used by kelidan
SET @BROGOKK_GAZER:= 91520;

DELETE FROM `creature_template` WHERE `entry` IN (@SHADOW_PORTAL_TRIGGER, @BROGOKK_BEAM_TRIGGER, @BROGOKK_BEAM_VICTIM, @BROGOKK_GAZER);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@SHADOW_PORTAL_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Shadow portal broggok', '', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, '', 0, 12340),
(@BROGOKK_BEAM_TRIGGER, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Broggok beam', '', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, '', 0, 12340),
(@BROGOKK_BEAM_VICTIM, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Broggok beam target', '', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 128, 'npc_broggok_beam_victim', 0, 12340),
(@BROGOKK_GAZER, 0, 0, 0, 0, 0, 23266, 0, 0, 0, 'Gazer', '', '', 0, 83, 83, 1, 16, 0, 2.4, 1.42857, 1, 1, 2, 2, 0, 24, 7.5, 2000, 2000, 1, 33587456, 2048, 0, 0, 0, 0, 0, 0, 1, 1, 0, 3, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 0.7, 1, 1, 0, 0, 0, 0, 0, 0, 0, 152, 1, 0, 0, 'npc_broggok_gazer_mythic', 0, 12340);

UPDATE `creature_template` SET `InhabitType`  = 7 WHERE `entry` IN (@BROGOKK_BEAM_TRIGGER, @BROGOKK_BEAM_VICTIM);
UPDATE `creature_template` SET `scale` = 0.4, unit_class = 8, `MythicTemplateId` = @BROGOKK_GAZER WHERE `entry` = @BROGOKK_GAZER;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` = 35845;
INSERT INTO `creature_mythic_spell_values` VALUES
(35845, 3000, 0, 0, 0, 0, 0.02);

DELETE FROM `creature_template_mythic` WHERE `Id` = @BROGOKK_GAZER;
INSERT INTO `creature_template_mythic` VALUES
(@BROGOKK_GAZER, 1, 83, 0, 32000, 0, 4100, 4900, 0.05, 0.05, 0);

