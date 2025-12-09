SET @NPC_ENTRY := 340007;

DELETE FROM `creature_template` WHERE `entry`=@NPC_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(@NPC_ENTRY,0,0,0,0,0,2147483747,0,0,0,'Grorg Goresword','','',0,80,80,0,35,1,1,1.14286,1,0,0,0,0,0,1,2000,0,4,0,0,8,0,0,0,0,0,0,0,0,9,138936390,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,9.35979,1,1,0,0,0,0,0,0,0,121,1,0,0,'npc_arena_master',12340);


DELETE FROM `spell_dbc` WHERE `id` IN (99998,99999);
INSERT INTO `spell_dbc` (`Id`, `Attributes`, `AttributesEx3`, `AttributesEx5`, `AttributesEx6`, `DurationIndex`, `StackAmount`, `Effect1`, `Effect2`, `EffectBasePoints1`, `EffectBasePoints2`, `EffectImplicitTargetA1`, `EffectImplicitTargetA2`, `EffectApplyAuraName1`, `EffectApplyAuraName2`, `EffectMiscValue1`, `EffectMiscValue2`, `EffectAmplitude1`, `EffectTriggerSpell1`, `Comment`) VALUES
(99998,0,0,0x00000200,0x00000002,21,0,6,0,1,0,1,0,23,0,0,0,30000,99999,'Dementia(Periodic) for SoloQueue'),
(99999,0x04000000,0x20040000,0,0x00000002,21,20,6,6,5,-5,1,1,87,118,127,127,0,0,'Dementia for SoloQueue');

SET @TEXT_ID := 1200000;
DELETE FROM `npc_text` WHERE `ID` BETWEEN @TEXT_ID AND @TEXT_ID + 7;

INSERT INTO `npc_text`(`ID`, `text0_0`) VALUES 
(@TEXT_ID, 'Greetings $N! Are you looking for some new PvP experience? Clear your mind, sharpen your blade and start the competition in the Solo Queue bracket. Take some time and customise your Solo Queue banner first!'),
(@TEXT_ID + 1, 'Customize the style of your Solo Queue banner! Let your enemies feel fear and respect once the arena gates will be opened and they will see your blood-covered flag!'),
(@TEXT_ID + 2, 'Choose the border style.'),
(@TEXT_ID + 3, 'Choose the icon style.'),
(@TEXT_ID + 4, 'Greetings $n! Hit the "Join Solo Queue" button, cross your sword with other heroes of Azeroth and start fighting for glory in the Solo Queue ladder!'),
(@TEXT_ID + 5, 'You\'re already in the queue. Once the arena is ready, accept the invitation and we\'ll start the show!'),
(@TEXT_ID + 6, 'You need 80 level to start playing Solo Queue arenas!'),
(@TEXT_ID + 7, 'You don\'t have enough resilience to start playing Solo Queue arenas!');

SET @OUTFIT_ENTRY := 2147483747;
DELETE FROM `creature_template_outfits` WHERE `entry` = @OUTFIT_ENTRY;
INSERT INTO `creature_template_outfits` VALUES (@OUTFIT_ENTRY,0,2,1,0,0,0,0,0,0,30488,30490,0,30486,0,30489,32793,0,30487,0,0,0,'SoloQueue NPC');

DELETE FROM `creature_equip_template` WHERE `entry` = @NPC_ENTRY;
INSERT INTO `creature_equip_template` VALUES (@NPC_ENTRY, 1, 34699, 34699, 0, 12340);

SET @NPC_GUID := 303351;

DELETE FROM `creature` WHERE `guid` = @NPC_GUID;
INSERT INTO `creature` VALUES (@NPC_GUID,340007,571,1,1,0,1,5798.22, 594.87, 609.15, 2.50,300,0,0,50000,0,0,0,0,0);