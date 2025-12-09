-- DB/Quest: Gaining the advantage - Nether residue - Add Missing drops  https://github.com/TrinityCore/TrinityCore/commit/16d4a39c7d32e6d269f7926228b0df539722f44e
-- DB/Loot: Add references for rare items from RFK, RFD & Scarlet Monastery https://github.com/TrinityCore/TrinityCore/commit/874b149923594c71cbd0b49fa8ef8c5e9cd0ea6e
-- DB/Loot: Fix some loots in The Obsidian Sanctum  https://github.com/TrinityCore/TrinityCore/commit/267e707fe3404788df8316166bd31ab139909fd1
DELETE FROM `gameobject_loot_template` WHERE `Entry` IN (18111,18112,18113,19627,19628,19629,18359,18361,18363,26861) AND `Item` = 35229;
INSERT INTO `gameobject_loot_template` (`Entry`,`Item`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MinCountOrRef`,`MaxCount`) VALUES
(18111,35229,-25,1,0,1,1),
(18112,35229,-25,1,0,1,1),
(18113,35229,-25,1,0,1,1),
(19627,35229,-25,1,0,1,1),
(19628,35229,-25,1,0,1,1),
(19629,35229,-25,1,0,1,1),
(18359,35229,-25,1,0,1,1),
(18361,35229,-25,1,0,1,1),
(18363,35229,-25,1,0,1,1),
(26861,35229,-25,1,0,1,1);

DELETE FROM `skinning_loot_template` WHERE `Entry` IN (70171,70172,70170,70169,70168,70167,70166,70165,70164,70163,70162,70161) AND `Item`=35229;
INSERT INTO `skinning_loot_template` (`Entry`, `Item`, `ChanceOrQuestChance`, `LootMode`, `GroupId`, `MinCountOrRef`, `MaxCount`) VALUES 
(70172, 35229, -25, 1, 0, 1, 1),
(70171, 35229, -25, 1, 0, 1, 1),
(70170, 35229, -25, 1, 0, 1, 1),
(70169, 35229, -25, 1, 0, 1, 1),
(70168, 35229, -25, 1, 0, 1, 1),
(70167, 35229, -25, 1, 0, 1, 1),
(70166, 35229, -25, 1, 0, 1, 1),
(70165, 35229, -25, 1, 0, 1, 1),
(70164, 35229, -25, 1, 0, 1, 1),
(70163, 35229, -25, 1, 0, 1, 1),
(70162, 35229, -25, 1, 0, 1, 1),
(70161, 35229, -25, 1, 0, 1, 1);

-- RFK
SET @ENTRY := 14102;
DELETE FROM `reference_loot_template` WHERE `Entry` = @ENTRY;
INSERT INTO `reference_loot_template` (`Entry`,`Item`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MinCountOrRef`,`MaxCount`) VALUES
(@ENTRY,776,0,1,1,1,1),
(@ENTRY,1488,0,1,1,1,1),
(@ENTRY,1727,0,1,1,1,1),
(@ENTRY,1975,0,1,1,1,1),
(@ENTRY,1976,0,1,1,1,1),
(@ENTRY,1978,0,1,1,1,1),
(@ENTRY,2039,0,1,1,1,1),
(@ENTRY,2264,0,1,1,1,1),
(@ENTRY,2549,0,1,1,1,1),
(@ENTRY,4438,0,1,1,1,1);

DELETE FROM `creature_loot_template` WHERE `Entry` IN (4427,4435,4436,4437,4438,4440,4442,4511,4512,4514,4515,4516,4517,4518,4519,4520,4522,4523,4525,4530,4531,4532,4538,4539,4623,6035,6168) AND ((`Item` IN (776,1488,1727,1975,1976,1978,2039,2264,2549,4438)) OR `MinCountOrRef` = -@ENTRY);
INSERT INTO `creature_loot_template` (`Entry`,`Item`,`MinCountOrRef`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MaxCount`) VALUES
(4427,@ENTRY,-@ENTRY,0.1,1,0,1),
(4435,@ENTRY,-@ENTRY,0.1,1,0,1),
(4436,@ENTRY,-@ENTRY,0.1,1,0,1),
(4437,@ENTRY,-@ENTRY,0.1,1,0,1),
(4438,@ENTRY,-@ENTRY,0.1,1,0,1),
(4440,@ENTRY,-@ENTRY,0.1,1,0,1),
(4442,@ENTRY,-@ENTRY,0.1,1,0,1),
(4511,@ENTRY,-@ENTRY,0.1,1,0,1),
(4512,@ENTRY,-@ENTRY,0.1,1,0,1),
(4514,@ENTRY,-@ENTRY,0.1,1,0,1),
(4515,@ENTRY,-@ENTRY,0.1,1,0,1),
(4516,@ENTRY,-@ENTRY,0.1,1,0,1),
(4517,@ENTRY,-@ENTRY,0.1,1,0,1),
(4518,@ENTRY,-@ENTRY,0.1,1,0,1),
(4519,@ENTRY,-@ENTRY,0.1,1,0,1),
(4520,@ENTRY,-@ENTRY,0.1,1,0,1),
(4522,@ENTRY,-@ENTRY,0.1,1,0,1),
(4523,@ENTRY,-@ENTRY,0.1,1,0,1),
(4525,@ENTRY,-@ENTRY,0.1,1,0,1),
(4530,@ENTRY,-@ENTRY,0.1,1,0,1),
(4531,@ENTRY,-@ENTRY,0.1,1,0,1),
(4532,@ENTRY,-@ENTRY,0.1,1,0,1),
(4538,@ENTRY,-@ENTRY,0.1,1,0,1),
(4539,@ENTRY,-@ENTRY,0.1,1,0,1),
(4623,@ENTRY,-@ENTRY,0.1,1,0,1),
(6035,@ENTRY,-@ENTRY,0.1,1,0,1),
(6168,@ENTRY,-@ENTRY,0.1,1,0,1);

-- RFD
SET @ENTRY := 14103;
DELETE FROM `reference_loot_template` WHERE `Entry` = @ENTRY;
INSERT INTO `reference_loot_template` (`Entry`,`Item`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MinCountOrRef`,`MaxCount`) VALUES
(@ENTRY,10567,0,1,1,1,1),
(@ENTRY,10570,0,1,1,1,1),
(@ENTRY,10571,0,1,1,1,1),
(@ENTRY,10572,0,1,1,1,1),
(@ENTRY,10573,0,1,1,1,1),
(@ENTRY,10574,0,1,1,1,1),
(@ENTRY,10578,0,1,1,1,1),
(@ENTRY,10581,0,1,1,1,1),
(@ENTRY,10582,0,1,1,1,1),
(@ENTRY,10583,0,1,1,1,1),
(@ENTRY,10584,0,1,1,1,1);

DELETE FROM `creature_loot_template` WHERE `Entry` IN (7327,7328,7329,7332,7335,7337,7341,7342,7345,7347,7348,7349,7351,7352,7353) AND ((`Item` IN (10567,10570,10571,10572,10573,10574,10578,10581,10582,10583,10584)) OR `MinCountOrRef` = -@ENTRY);
INSERT INTO `creature_loot_template` (`Entry`,`Item`,`MinCountOrRef`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MaxCount`) VALUES
(7327,@ENTRY,-@ENTRY,0.1,1,0,1),
(7328,@ENTRY,-@ENTRY,0.1,1,0,1),
(7329,@ENTRY,-@ENTRY,0.1,1,0,1),
(7332,@ENTRY,-@ENTRY,0.1,1,0,1),
(7335,@ENTRY,-@ENTRY,0.1,1,0,1),
(7337,@ENTRY,-@ENTRY,0.1,1,0,1),
(7341,@ENTRY,-@ENTRY,0.1,1,0,1),
(7342,@ENTRY,-@ENTRY,0.1,1,0,1),
(7345,@ENTRY,-@ENTRY,0.1,1,0,1),
(7347,@ENTRY,-@ENTRY,0.1,1,0,1),
(7348,@ENTRY,-@ENTRY,0.1,1,0,1),
(7349,@ENTRY,-@ENTRY,0.1,1,0,1),
(7351,@ENTRY,-@ENTRY,0.1,1,0,1),
(7352,@ENTRY,-@ENTRY,0.1,1,0,1),
(7353,@ENTRY,-@ENTRY,0.1,1,0,1);
-- (7872,@ENTRY,-@ENTRY,0.1,1,0,1),
-- (7873,@ENTRY,-@ENTRY,0.1,1,0,1),
-- (7874,@ENTRY,-@ENTRY,0.1,1,0,1),
-- (12865,@ENTRY,-@ENTRY,0.1,1,0,1);


-- Scarlet Monastery(Inside)
SET @ENTRY := 14104;
DELETE FROM `reference_loot_template` WHERE `Entry` = @ENTRY;
INSERT INTO `reference_loot_template` (`Entry`,`Item`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MinCountOrRef`,`MaxCount`) VALUES
(@ENTRY,1992,0,1,1,1,1),
(@ENTRY,2262,0,1,1,1,1),
(@ENTRY,5756,0,1,1,1,1),
(@ENTRY,5819,0,1,1,1,1),
(@ENTRY,7727,0,1,1,1,1),
(@ENTRY,7728,0,1,1,1,1),
(@ENTRY,7729,0,1,1,1,1),
(@ENTRY,7730,0,1,1,1,1),
(@ENTRY,7736,0,1,1,1,1),
(@ENTRY,7752,0,1,1,1,1),
(@ENTRY,7753,0,1,1,1,1),
(@ENTRY,7754,0,1,1,1,1),
(@ENTRY,7755,0,1,1,1,1),
(@ENTRY,7757,0,1,1,1,1),
(@ENTRY,7758,0,1,1,1,1),
(@ENTRY,7759,0,1,1,1,1),
(@ENTRY,7760,0,1,1,1,1),
(@ENTRY,7761,0,1,1,1,1),
(@ENTRY,7786,0,1,1,1,1),
(@ENTRY,7787,0,1,1,1,1),
(@ENTRY,8225,0,1,1,1,1),
(@ENTRY,8226,0,1,1,1,1),
(@ENTRY,10332,0,1,1,1,1);

DELETE FROM `creature_loot_template` WHERE `Entry` IN (4286,4287,4288,4289,4290,4291,4292,4293,4294,4295,4296,4297,4298,4299,4300,4301,4302,4303,4304,4306,4308,4540,6426,6427) AND ((`Item` IN (1992,2262,5756,5819,7727,7728,7729,7730,7736,7752,7753,7754,7755,7757,7758,7759,7760,7761,7786,7787,8225,8226,10332)) OR `MinCountOrRef` = -@ENTRY);
INSERT INTO `creature_loot_template` (`Entry`,`Item`,`MinCountOrRef`,`ChanceOrQuestChance`,`LootMode`,`GroupId`,`MaxCount`) VALUES
(4286,@ENTRY,-@ENTRY,0.1,1,0,1),
(4287,@ENTRY,-@ENTRY,0.1,1,0,1),
(4288,@ENTRY,-@ENTRY,0.1,1,0,1),
(4289,@ENTRY,-@ENTRY,0.1,1,0,1),
(4290,@ENTRY,-@ENTRY,0.1,1,0,1),
(4291,@ENTRY,-@ENTRY,0.1,1,0,1),
(4292,@ENTRY,-@ENTRY,0.1,1,0,1),
(4293,@ENTRY,-@ENTRY,0.1,1,0,1),
(4294,@ENTRY,-@ENTRY,0.1,1,0,1),
(4295,@ENTRY,-@ENTRY,0.1,1,0,1),
(4296,@ENTRY,-@ENTRY,0.1,1,0,1),
(4297,@ENTRY,-@ENTRY,0.1,1,0,1),
(4298,@ENTRY,-@ENTRY,0.1,1,0,1),
(4299,@ENTRY,-@ENTRY,0.1,1,0,1),
(4300,@ENTRY,-@ENTRY,0.1,1,0,1),
(4301,@ENTRY,-@ENTRY,0.1,1,0,1),
(4302,@ENTRY,-@ENTRY,0.1,1,0,1),
(4303,@ENTRY,-@ENTRY,0.1,1,0,1),
(4304,@ENTRY,-@ENTRY,0.1,1,0,1),
(4306,@ENTRY,-@ENTRY,0.1,1,0,1),
(4308,@ENTRY,-@ENTRY,0.1,1,0,1),
(4540,@ENTRY,-@ENTRY,0.1,1,0,1),
(6426,@ENTRY,-@ENTRY,0.1,1,0,1),
(6427,@ENTRY,-@ENTRY,0.1,1,0,1);

UPDATE `creature_template` SET `mingold` = 21855, `maxgold` = 23530 WHERE `entry` = 30453;
UPDATE `creature_template` SET `mingold` = 43710, `maxgold` = 47060 WHERE `entry` = 31001;
UPDATE `creature_template` SET `mingold` = 1800000, `maxgold` = 2000000 WHERE `entry` = 28860;
UPDATE `creature_template` SET `skinloot`=28860, `mingold` = 2000000, `maxgold` = 2250000 WHERE `entry` = 31311;
UPDATE `creature_template` SET `mingold` = 21855, `maxgold` = 23530 WHERE `entry` IN (30681, 30680);
UPDATE `creature_template` SET `mingold` = 43710, `maxgold` = 47060 WHERE `entry` IN (30998, 30999);

