SET @NPC_ANTUSUL_SPIRIT_WOLF:= 91504;

DELETE FROM `creature_template` WHERE `entry` IN (@NPC_ANTUSUL_SPIRIT_WOLF);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@NPC_ANTUSUL_SPIRIT_WOLF, 0, 0, 0, 0, 0, 29673, 0, 0, 0, 'Electric wolf', 'Antu\'sul guardian', '', 0, 83, 83, 0, 14, 0, 1, 1.14286, 0.85, 0, 0, 0, 0, 0, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, 'npc_electric_wolf_antusul', 0, 12340);

UPDATE `creature_template` SET `type_flags` = 1024, `MythicTemplateId` = @NPC_ANTUSUL_SPIRIT_WOLF WHERE `entry` = @NPC_ANTUSUL_SPIRIT_WOLF;

DELETE FROM `creature_template_mythic` WHERE `Id` = @NPC_ANTUSUL_SPIRIT_WOLF;
INSERT INTO `creature_template_mythic` VALUES
(@NPC_ANTUSUL_SPIRIT_WOLF, 1, 83, 0, 31000, 0, 3000, 3800, 0.08, 0.08, 0);
