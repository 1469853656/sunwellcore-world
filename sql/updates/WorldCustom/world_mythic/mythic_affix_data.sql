
TRUNCATE TABLE `mythic_affix_template`;
INSERT INTO `mythic_affix_template` (`AffixId`, `AffixName`, `AffixFlags`, `AffixChance`, `AffixCooldown`, `AffixKeyRequirement`, `IsInstanceAffix`) VALUES
(1, "InCombat break CC", 0x01, 100, 1000, 0, 0),
(2, "On death aura nearby npc", 0x02, 100, 8000, 0, 0),
(3, "Frenzy on 20% HP", 0x04, 100, 60000, 0, 0),
(4, "Mojo pool on death", 0x02, 100, 0, 0, 0),
(5, "Fire dragon while in combat", 0x08, 100, 0, 0, 1),
(6, "Mortal wound on melee attack", 0x10, 60, 1000, 0, 0),
(7, "Ghoul spawns periodic", 0x08, 100, 0, 0, 1),
(8, "Holy bombs", 0x08, 100, 0, 0, 1),
(9, "Spell bounce", 0x20, 35, 10000, 0, 0);

TRUNCATE TABLE `mythic_affix_presets`;
INSERT INTO `mythic_affix_presets` (`PresetId`, `FirstAffix`, `SecondAffix`, `ThirdAffix`, `FourthAffix`) VALUES
(1, 1, 4, 5, 6);

/** Affix world stuff **/
DELETE FROM `spell_dbc` WHERE `Id` IN (90000, 90001, 90002);
INSERT INTO `spell_dbc` (`Id`, `Dispel`, `Mechanic`, `Attributes`, `AttributesEx`, `AttributesEx2`, `AttributesEx3`, `AttributesEx4`, `AttributesEx5`, `AttributesEx6`, `AttributesEx7`, `Stances`, `StancesNot`, `Targets`, `CastingTimeIndex`, `AuraInterruptFlags`, `ProcFlags`, `ProcChance`, `ProcCharges`, `MaxLevel`, `BaseLevel`, `SpellLevel`, `DurationIndex`, `RangeIndex`, `StackAmount`, `EquippedItemClass`, `EquippedItemSubClassMask`, `EquippedItemInventoryTypeMask`, `Effect1`, `Effect2`, `Effect3`, `EffectDieSides1`, `EffectDieSides2`, `EffectDieSides3`, `EffectRealPointsPerLevel1`, `EffectRealPointsPerLevel2`, `EffectRealPointsPerLevel3`, `EffectBasePoints1`, `EffectBasePoints2`, `EffectBasePoints3`, `EffectMechanic1`, `EffectMechanic2`, `EffectMechanic3`, `EffectImplicitTargetA1`, `EffectImplicitTargetA2`, `EffectImplicitTargetA3`, `EffectImplicitTargetB1`, `EffectImplicitTargetB2`, `EffectImplicitTargetB3`, `EffectRadiusIndex1`, `EffectRadiusIndex2`, `EffectRadiusIndex3`, `EffectApplyAuraName1`, `EffectApplyAuraName2`, `EffectApplyAuraName3`, `EffectAmplitude1`, `EffectAmplitude2`, `EffectAmplitude3`, `EffectMultipleValue1`, `EffectMultipleValue2`, `EffectMultipleValue3`, `EffectItemType1`, `EffectItemType2`, `EffectItemType3`, `EffectMiscValue1`, `EffectMiscValue2`, `EffectMiscValue3`, `EffectMiscValueB1`, `EffectMiscValueB2`, `EffectMiscValueB3`, `EffectTriggerSpell1`, `EffectTriggerSpell2`, `EffectTriggerSpell3`, `EffectSpellClassMaskA1`, `EffectSpellClassMaskA2`, `EffectSpellClassMaskA3`, `EffectSpellClassMaskB1`, `EffectSpellClassMaskB2`, `EffectSpellClassMaskB3`, `EffectSpellClassMaskC1`, `EffectSpellClassMaskC2`, `EffectSpellClassMaskC3`, `MaxTargetLevel`, `SpellFamilyName`, `SpellFamilyFlags1`, `SpellFamilyFlags2`, `SpellFamilyFlags3`, `MaxAffectedTargets`, `DmgClass`, `PreventionType`, `DmgMultiplier1`, `DmgMultiplier2`, `DmgMultiplier3`, `AreaGroupId`, `SchoolMask`, `Comment`) VALUES
(90000, 0, 0, 537135360, 268436480, 5, 269746176, 4096, 0, 8192, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 226, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Mythic periodic remove CC'),
(90001, 0, 0, 537135360, 268436480, 5, 269746176, 4096, 0, 8192, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 226, 0, 0, 4000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Mythic hot feet'),
(90002, 0, 0, 256, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 101, 0, 0, 0, 0, 0, 6, 0, -1, 0, 0, 77, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 22, 0, 7, 7, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 'Mythic buff nearby');

DELETE FROM `spell_script_names` WHERE `spell_id` IN (90000, 90001, 90002);
INSERT INTO `spell_script_names` VALUES
(90000, "spell_mythic_remove_cc_aura"),
(90002, "spell_mythic_buff_nearby_on_death");

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` = 58994;
INSERT INTO `creature_mythic_spell_values` VALUES
(58994, 4500, 0, 0, 60000, 0, 0.1);

/** Affix creatures & triggers **/
SET @CENTRY:= 91700;
DELETE FROM `creature_template` WHERE `entry` IN (@CENTRY, @CENTRY + 1, @CENTRY + 2, @CENTRY + 3);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `MythicTemplateId`, `VerifiedBuild`) VALUES
(@CENTRY, 0, 0, 0, 0, 0, 6377, 0, 0, 0, 'Drogon', '', NULL, 0, 83, 83, 1, 35, 0, 1.2, 1.14286, 1, 3, 316, 450, 0, 320, 1, 2000, 2000, 1, 33554432, 2048, 0, 0, 0, 0, 0, 0, 278, 413, 58, 10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'NullCreatureAI', 0, 3, 1, 1.35, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, '', 0, 12340),
(@CENTRY + 1, 0, 0, 0, 0, 0, 11686, 0, 0, 0, 'Drogon', 'Drogons target', NULL, 0, 83, 83, 1, 16, 0, 1.2, 1.14286, 1, 3, 316, 450, 0, 320, 1, 2000, 2000, 1, 33554432, 2048, 0, 0, 0, 0, 0, 0, 278, 413, 58, 10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'NullCreatureAI', 0, 3, 1, 1.35, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 130, '', 0, 12340),
(@CENTRY + 2, 0, 0, 0, 0, 0, 24999, 0, 0, 0, 'Drudge Ghoul', '', '', 0, 83, 83, 2, 21, 0, 0.8, 1, 1, 1, 450, 500, 0, 300, 9, 2000, 2000, 1, 32832, 2048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4096, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 20, 15, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 8388625, 0, 'npc_drudge_ghoul_mythics', 0, 12340),
(@CENTRY + 3, 0, 0, 0, 0, 0, 31095, 0, 0, 0, 'Holy bomb', '', NULL, 0, 83, 83, 1, 16, 0, 1.2, 1.14286, 0.45, 3, 316, 450, 0, 320, 1, 2000, 2000, 1, 33554432, 2048, 0, 0, 0, 0, 0, 0, 278, 413, 58, 10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'NullCreatureAI', 0, 3, 1, 1.35, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 650854271, 2987152, 'npc_holy_bomb_mythic', 0, 12340);

UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 33554432, `scale` = 0.45, `InhabitType` = 7, `ScriptName` = "npc_drogon_affix_mythic" WHERE `entry` = @CENTRY;
UPDATE `creature_template` SET `InhabitType` = 7, `HoverHeight` = 3.5, `unit_flags` = 0, `type_flags` = 0 WHERE `entry` = @CENTRY + 3;
UPDATE `creature_template` SET `MythicTemplateId` = @CENTRY + 2 WHERE `entry` = @CENTRY + 2;

DELETE FROM `creature_template_mythic` WHERE `Id` IN ( @CENTRY + 2, @CENTRY + 3);
INSERT INTO `creature_template_mythic` VALUES
(@CENTRY + 2, 1, 82, 0, 42000, 100, 5000, 6000, 0.05, 0.05, 0),
(@CENTRY + 3, 6, 82, 0, 45000, 100, 100, 100, 0, 0.03, 0);

UPDATE `creature_template` SET `MythicTemplateId` = @CENTRY + 3 WHERE `entry` = @CENTRY + 3;

DELETE FROM `creature_template_addon` WHERE `entry` = @CENTRY + 3;
INSERT INTO `creature_template_addon` (`entry`, `bytes1`) VALUES
(@CENTRY + 3, 50331648);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (20203, 62354);
INSERT INTO `spell_script_names` VALUES
(20203, "spell_drogon_firenova_mythic"),
(62354, 'spell_broken_bones_mythic_avo');

