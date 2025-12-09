-- DB/GameObject: Netherwing Egg Pooling ( add new spawns ) https://github.com/TrinityCore/TrinityCore/commit/e069372de914b2bf7e6d4c14bdafc2a89a7a25a6
-- DB/Creature: Add formations for some creatures in SWP  https://github.com/TrinityCore/TrinityCore/commit/5fc54f948170ee53780171617632c3d7ea61710c
-- DB/Spell: Add missing dummy DBC Spells https://github.com/TrinityCore/TrinityCore/commit/95d12a262740260740665f04b24556a58a0b57d3
-- DB/Loot: Add missing bandages to Arathi Basin care packages https://github.com/TrinityCore/TrinityCore/commit/4a8382d9059186b7e5c4132f86b2fb75dcc2cc35 
SET @OGUID:=  300044;
SET @POOL :=  50050;

DELETE FROM `pool_gameobject` WHERE `pool_entry`=@POOL;
INSERT INTO `pool_gameobject` (`guid`, `pool_entry`, `chance`, `description`) VALUES 
(50417, @POOL, 0, "Netherwing Egg"),
(50418, @POOL, 0, "Netherwing Egg"),
(50419, @POOL, 0, "Netherwing Egg"),
(50420, @POOL, 0, "Netherwing Egg"),
(50421, @POOL, 0, "Netherwing Egg"),
(50422, @POOL, 0, "Netherwing Egg"),
(50423, @POOL, 0, "Netherwing Egg"),
(50424, @POOL, 0, "Netherwing Egg"),
(50425, @POOL, 0, "Netherwing Egg"),
(151889, @POOL, 0, "Netherwing Egg"),
(151890, @POOL, 0, "Netherwing Egg"),
(151891, @POOL, 0, "Netherwing Egg"),
(@OGUID+0, @POOL, 0, "Netherwing Egg"),
(@OGUID+1, @POOL, 0, "Netherwing Egg"),
(@OGUID+2, @POOL, 0, "Netherwing Egg"),
(@OGUID+3, @POOL, 0, "Netherwing Egg"),
(@OGUID+4, @POOL, 0, "Netherwing Egg"),
(@OGUID+5, @POOL, 0, "Netherwing Egg"),
(@OGUID+6, @POOL, 0, "Netherwing Egg"),
(@OGUID+7, @POOL, 0, "Netherwing Egg"),
(@OGUID+8, @POOL, 0, "Netherwing Egg"),
(@OGUID+9, @POOL, 0, "Netherwing Egg"),
(@OGUID+10, @POOL, 0, "Netherwing Egg"),
(@OGUID+11, @POOL, 0, "Netherwing Egg"),
(@OGUID+12, @POOL, 0, "Netherwing Egg"),
(@OGUID+13, @POOL, 0, "Netherwing Egg"),
(@OGUID+14, @POOL, 0, "Netherwing Egg"),
(@OGUID+15, @POOL, 0, "Netherwing Egg"),
(@OGUID+16, @POOL, 0, "Netherwing Egg"),
(@OGUID+17, @POOL, 0, "Netherwing Egg"),
(@OGUID+18, @POOL, 0, "Netherwing Egg"),
(@OGUID+19, @POOL, 0, "Netherwing Egg"),
(@OGUID+20, @POOL, 0, "Netherwing Egg"),
(@OGUID+21, @POOL, 0, "Netherwing Egg"),
(@OGUID+22, @POOL, 0, "Netherwing Egg"),
(@OGUID+23, @POOL, 0, "Netherwing Egg");

DELETE FROM `pool_template` WHERE `entry` =@POOL;
INSERT INTO `pool_template` (`entry`, `max_limit`, `description`) VALUES 
(@POOL, 10, 'Netherwing Egg 6 of 36');

UPDATE `gameobject` SET `spawntimesecs`=3600 WHERE  `id`=185915;
DELETE FROM `gameobject` WHERE `id`=185915 AND `guid` BETWEEN @OGUID+0 AND @OGUID+24;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`,  `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID+0, 185915, 530, 1, 1, -5116.493, 673.1073, 37.64555, 2.967041, 0, 0, 0.9961939, 0.08716504, 3600, 255, 1, 15595), -- 185915 (Area: 3965 - Difficulty: 0)
(@OGUID+1, 185915, 530, 1, 1, -4867.7, 531.2372, -1.91996, 1.134463, 0, 0, 0.5372992, 0.8433917, 3600, 255, 1, 15595), -- 185915 (Area: 3965 - Difficulty: 0)
(@OGUID+2, 185915, 530, 1, 1, -5034.838, 612.646, 184.4103, 2.984498, 0, 0, 0.9969168, 0.07846643, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+3, 185915, 530, 1, 1, -5040.481, 393.2956, 170.8373, 5.98648, 0, 0, -0.147809, 0.9890159, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+4, 185915, 530, 1, 1, -5117.958, 89.7627, -12.05266, 5.323256, 0, 0, -0.4617481, 0.8870111, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+5, 185915, 530, 1, 1, -5088.855, 23.24989, 81.81818, 0.1396245, 0, 0, 0.06975555, 0.9975641, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+6, 185915, 530, 1, 1, -5031.912, 289.6906, 4.984869, 0.5410506, 0, 0, 0.2672377, 0.9636307, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+7, 185915, 530, 1, 1, -5282.702, 654.8848, 40.72212, -1.378809, 0, 0, -0.6360779, 0.7716249, 3600, 255, 1, 13623), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+8, 185915, 530, 1, 1, -5341.819, -43.58724, 90.34805, -2.984498, 0, 0, -0.9969168, 0.07846643, 3600, 255, 1, 13623), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+9, 185915, 530, 1, 1, -5003.992, -113.3952, -4.667536, 0.2443456, 0, 0, 0.1218691, 0.9925462, 3600, 255, 1, 13623), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+10, 185915, 530, 1, 1, -4234.241, 526.788, 78.41892, -2.426003, 0, 0, -0.9366713, 0.3502098, 3600, 255, 1, 14007), -- 185915 (Area: 5012 - Difficulty: 0)
(@OGUID+11, 185915, 530, 1, 1, -4689.656, 771.3027, 25.21626, -2.268925, 0, 0, -0.9063072, 0.4226195, 3600, 255, 1, 14007), -- 185915 (Area: 5012 - Difficulty: 0)
(@OGUID+12, 185915, 530, 1, 1, -4931.305, 426.1053, 0.003176, 1.431168, 0, 0, 0.6560583, 0.7547102, 3600, 255, 1, 14007), -- 185915 (Area: 5012 - Difficulty: 0)
(@OGUID+13, 185915, 530, 1, 1, -4934.188, 323.5568, 138.5504, 4.188792, 0, 0, -0.866025, 0.5000008, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+14, 185915, 530, 1, 1, -5096.626, 151.4355, -11.33789, 1.099556, 0, 0, 0.5224981, 0.8526405, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+15, 185915, 530, 1, 1, -5143.51, 190.4579, 150.3326, 0.7504908, 0, 0, 0.3665009, 0.9304177, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+16, 185915, 530, 1, 1, -4987.638, 451.117, 6.23003, 5.288348, 0, 0, -0.4771585, 0.8788173, 3600, 255, 1, 15595), -- 185915 (Area: 3965 - Difficulty: 0)
(@OGUID+17, 185915, 530, 1, 1, -5025.026, 359.934, 1.24235, 1.448622, 0, 0, 0.6626196, 0.7489561, 3600, 255, 1, 15595), -- 185915 (Area: 3965 - Difficulty: 0)
(@OGUID+18, 185915, 530, 1, 1, -5170.725, 251.7406, -32.01391, 2.879789, 0, 0, 0.9914446, 0.1305283, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+19, 185915, 530, 1, 1, -5260.721, 186.911, 128.8412, 5.201083, 0, 0, -0.5150375, 0.8571676, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+20, 185915, 530, 1, 1, -5004.113, 14.80371, 76.3614, 4.677484, 0, 0, -0.7193394, 0.6946588, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+21, 185915, 530, 1, 1, -4901.969, 597.2923, 4.663516, 0.4188786, 0, 0, 0.2079115, 0.9781476, 3600, 255, 1, 15595), -- 185915 (Area: 3759 - Difficulty: 0)
(@OGUID+22, 185915, 530, 1, 1, -4286.163, 408.8362, 129.7793, 1.361356, 0, 0, 0.6293201, 0.7771462, 3600, 255, 1, 14007), -- 185915 (Area: 4666 - Difficulty: 0)
(@OGUID+23, 185915, 530, 1, 1,  -5199.026, 624.2134, 45.12667, 0.5061446, 0, 0, 0.2503796, 0.9681478, 3600, 255, 1, 15595); -- 185915 (Area: 3966 - Difficulty: 0)

DELETE FROM `creature_formations` WHERE `leaderGUID` IN (54817,48360,54820,54824,54825,48372,54827,48162,54860,54992,48160);
INSERT INTO `creature_formations` (`leaderGUID`,`memberGUID`,`dist`,`angle`,`groupAI`) VALUES
(54817,43654,0,0,3),
(54817,42573,0,0,3),
(54817,42574,0,0,3),
(54817,54834,0,0,3),
(54817,42656,0,0,3),
(54817,54817,0,0,3),
(48360,54819,0,0,3),
(48360,43440,0,0,3),
(48360,43738,0,0,3),
(48360,54833,0,0,3),
(48360,42655,0,0,3),
(48360,48360,0,0,3),
(48360,42593,0,0,3),
(54820,54832,0,0,3),
(54820,42582,0,0,3),
(54820,42894,0,0,3),
(54820,43439,0,0,3),
(54820,54820,0,0,3),
(54820,42647,0,0,3),
(54824,42640,0,0,3),
(54824,54829,0,0,3),
(54824,42587,0,0,3),
(54824,42659,0,0,3),
(54824,54824,0,0,3),
(54824,42586,0,0,3),
(54825,42591,0,0,3),
(54825,54825,0,0,3),
(54825,42660,0,0,3),
(54825,42657,0,0,3),
(54825,54835,0,0,3),
(54825,42592,0,0,3),
(48372,41817,0,0,3),
(48372,54838,0,0,3),
(48372,48372,0,0,3),
(48372,42595,0,0,3),
(48372,43739,0,0,3),
(48372,54826,0,0,3),
(48372,42661,0,0,3),
(54827,54827,0,0,3),
(54827,42585,0,0,3),
(54827,54830,0,0,3),
(54827,42584,0,0,3),
(54827,42644,0,0,3),
(54827,42870,0,0,3),
(48162,54836,0,0,3),
(48162,54818,0,0,3),
(48162,42658,0,0,3),
(48162,43655,0,0,3),
(48162,48162,0,0,3),
(48162,42594,0,0,3),
(48162,42590,0,0,3),
(54860,54860,0,0,3),
(54860,42874,0,0,3),
(54860,54831,0,0,3),
(54860,54828,0,0,3),
(54860,54821,0,0,3),
(54860,42588,0,0,3),
(54860,57041,0,0,3),
(54992,54822,0,0,3),
(54992,42583,0,0,3),
(54992,42871,0,0,3),
(54992,54992,0,0,3),
(54992,56933,0,0,3),
(54992,54823,0,0,3),
(54992,42645,0,0,3),
(48160,43661,0,0,3),
(48160,54988,0,0,3),
(48160,42589,0,0,3),
(48160,54858,0,0,3),
(48160,54816,0,0,3),
(48160,54837,0,0,3),
(48160,56686,0,0,3),
(48160,48160,0,0,3);

DELETE FROM `spell_dbc` WHERE `Id` IN (53258,47805,58568,19229);
INSERT INTO `spell_dbc` (Id, Dispel, Mechanic, Attributes, AttributesEx, AttributesEx2, AttributesEx3, AttributesEx4, AttributesEx5, AttributesEx6, AttributesEx7, Stances, StancesNot, Targets, CastingTimeIndex, AuraInterruptFlags, ProcFlags, ProcChance, ProcCharges, MaxLevel, BaseLevel, SpellLevel, DurationIndex, RangeIndex, StackAmount, EquippedItemClass, EquippedItemSubClassMask, EquippedItemInventoryTypeMask, Effect1, Effect2, Effect3, EffectDieSides1, EffectDieSides2, EffectDieSides3, EffectRealPointsPerLevel1, EffectRealPointsPerLevel2, EffectRealPointsPerLevel3, EffectBasePoints1, EffectBasePoints2, EffectBasePoints3, EffectMechanic1, EffectMechanic2, EffectMechanic3, EffectImplicitTargetA1, EffectImplicitTargetA2, EffectImplicitTargetA3, EffectImplicitTargetB1, EffectImplicitTargetB2, EffectImplicitTargetB3, EffectRadiusIndex1, EffectRadiusIndex2, EffectRadiusIndex3, EffectApplyAuraName1, EffectApplyAuraName2, EffectApplyAuraName3, EffectAmplitude1, EffectAmplitude2, EffectAmplitude3, EffectMultipleValue1, EffectMultipleValue2, EffectMultipleValue3, EffectItemType1, EffectItemType2, EffectItemType3, EffectMiscValue1, EffectMiscValue2, EffectMiscValue3, EffectMiscValueB1, EffectMiscValueB2, EffectMiscValueB3, EffectTriggerSpell1, EffectTriggerSpell2, EffectTriggerSpell3, EffectSpellClassMaskA1, EffectSpellClassMaskA2, EffectSpellClassMaskA3, EffectSpellClassMaskB1, EffectSpellClassMaskB2, EffectSpellClassMaskB3, EffectSpellClassMaskC1, EffectSpellClassMaskC2, EffectSpellClassMaskC3, Comment, MaxTargetLevel, SpellFamilyName, SpellFamilyFlags1, SpellFamilyFlags2, SpellFamilyFlags3, MaxAffectedTargets, DmgClass, PreventionType, DmgMultiplier1, DmgMultiplier2, DmgMultiplier3, AreaGroupId, SchoolMask) VALUES
(53258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Empower Rune Weapon", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1),
(47805, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Chains of Ice", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1),
(58568, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Sunder Armor", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1),
(19229, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 21, 1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Wing Clip", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);

DELETE FROM `item_loot_template` WHERE `Item` IN (20065,20066,20067);
INSERT INTO `item_loot_template` (`Entry`,`Item`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MinCountOrRef`,`MaxCount`) VALUES
(20230,20065,100,1,0,20,20),
(20236,20065,100,1,0,20,20),
(20228,20066,100,1,0,20,20),
(20231,20066,100,1,0,20,20),
(20229,20067,100,1,0,20,20),
(20233,20067,100,1,0,20,20);
