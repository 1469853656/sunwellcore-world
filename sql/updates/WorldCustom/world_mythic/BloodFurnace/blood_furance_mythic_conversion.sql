-- enforcer
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (14516,15655);
INSERT INTO `creature_mythic_spell_values` VALUES
(15655, 0, 6500, 0, 3500, 1, 0.04),
(14516, 1000, 0, 0, 0, 1, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 36;
INSERT INTO `creature_template_mythic` VALUES
(36, 4, 82, 0, 71000, 0, 10300, 15700, 0.04, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 36 WHERE `entry` = 17370;

UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_laughing_skull_enforcer' WHERE `entry` = 17370;

-- shadowmoon adept
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (11978);
INSERT INTO `creature_mythic_spell_values` VALUES
(11978, 1000, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 37;
INSERT INTO `creature_template_mythic` VALUES
(37, 1, 82, 0, 51000, 0, 6600, 10200, 0.04, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 37 WHERE `entry` = 17397;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_shadowmoon_adept' WHERE `entry` = 17397;

-- hellfire imp
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (15242,16144);
INSERT INTO `creature_mythic_spell_values` VALUES
(16144, 650, 0, 0, 0, 0, 0.04),
(15242, 650, 0, 0, 0, 0, 0.04);

DELETE FROM `creature_template_mythic` WHERE `Id` = 38;
INSERT INTO `creature_template_mythic` VALUES
(38, 2, 82, 0, 19000, 41300, 5700, 8000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 38 WHERE `entry` = 17477;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_hellfire_imp_blood_furnace' WHERE `entry` = 17477;

-- ganarg tinkerer
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (32003);
INSERT INTO `creature_mythic_spell_values` VALUES
(32003, 800, 500, 500, 0, 5, 0.03);

DELETE FROM `creature_template_mythic` WHERE `Id` = 39;
INSERT INTO `creature_template_mythic` VALUES
(39, 1, 82, 0, 63000, 0, 5700, 8000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 39 WHERE `entry` = 17151;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_ganarg_tinkerer' WHERE `entry` = 17151;

-- Shadowmoon warlock
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (33111,13338 ,12739,32197);
INSERT INTO `creature_mythic_spell_values` VALUES
(12739, 350, 0, 0, 0, 0, 0.01),
(32197, 500, 0, 0, 0, 1, 0.02),
(13338 , 0, 0, 0, 0, 0, 0),
(33111, 0, 0, 0, 0, 1, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 40;
INSERT INTO `creature_template_mythic` VALUES
(40, 2, 82, 0, 63000, 79000, 8100, 11300, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 40 WHERE `entry` = 17371;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_shadowmoon_warlock_blood_furnace' WHERE `entry` = 17371;

-- Laughing skull legionnaire
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (10966);
INSERT INTO `creature_mythic_spell_values` VALUES
(10966, 100, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 41;
INSERT INTO `creature_template_mythic` VALUES
(41, 1, 82, 0, 63000, 0, 10300, 15700, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 41 WHERE `entry` = 17626;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_laughing_skull_legionnaire' WHERE `entry` = 17626;

-- shadowmoon summoner
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (30853,18399);
INSERT INTO `creature_mythic_spell_values` VALUES
(30853, 0, 0, 0, 360000, 0, 0),
(18399, 1000, 1000, 0, 14000, 1, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 42;
INSERT INTO `creature_template_mythic` VALUES
(42, 2, 82, 0, 51000, 56000, 5800, 7400, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 42 WHERE `entry` = 17395;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_shadowmoon_summoner_blood_furnace' WHERE `entry` = 17395;

-- shadowmoon technican
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (6726,30846,40062, 30844);
INSERT INTO `creature_mythic_spell_values` VALUES
(30846, 0, 0, 0, 0, 0, 0),
(40062, 8000, 0, 0, 0, 5, 0.05),
(6726, 0, 0, 0, 8000, 0, 0),
(30844, 4500, 0, 0, 0, 0, 0.02);

DELETE FROM `spell_script_names` WHERE `spell_id` = 30844;
INSERT INTO `spell_script_names` VALUES
(30844, "spell_proximity_bomb_explosion");

DELETE FROM `creature_template_mythic` WHERE `Id` = 43;
INSERT INTO `creature_template_mythic` VALUES
(43, 1, 82, 0, 63000, 0, 8600, 10200, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 43 WHERE `entry` = 17414;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_shadowmoon_technican_blood_furnace' WHERE `entry` = 17414;

-- laughing skull warden
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (38551,9128);
INSERT INTO `creature_mythic_spell_values` VALUES
(9128, 800, 0, 0, 0, 5, 0.05),
(38551, 0, 0, 0, 0, 0, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 44;
INSERT INTO `creature_template_mythic` VALUES
(44, 1, 82, 0, 63000, 0, 10200, 14000, 0.07, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 44 WHERE `entry` = 17624;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_laughing_skull_warden' WHERE `entry` = 17624;

-- The maker
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (30925, 20276, 38163);
INSERT INTO `creature_mythic_spell_values` (`SpellId`, `SpellEffect0Value`, `SpellEffect1Value`, `SpellEffect2Value`, `SpellDuration`, `SpellMaxTargets`, `SpellValueVariance`) VALUES
(30925, 12500, 0, 0, 0, 5, 0.05),
(20276, 0, 20000, 0, 0, 0, 0.05),
(38163, 0, 3500, 0, 0, 0, 0.05);

DELETE FROM `spell_script_names` WHERE `spell_id` = 38153;
INSERT INTO `spell_script_names` VALUES
(38153 , "spell_acid_spray_the_maker");

UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'boss_the_maker', `MythicTemplateId` = 45 WHERE `entry` = 17381;

DELETE FROM `creature_template_mythic` WHERE `Id` = 45;
INSERT INTO `creature_template_mythic` VALUES
(45, 1, 83, 0, 531000, 0, 19000, 27100, 0.06, 0.06, 0);

-- Nascent Fel Orc
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (31900,22427);
INSERT INTO `creature_mythic_spell_values` VALUES
(22427, 10000, 0, 0, 0, 0, 0.02),
(31900, 2000, 0, 0, 0, 5, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 46;
INSERT INTO `creature_template_mythic` VALUES
(46, 1, 82, 0, 49100, 0, 5700, 9000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 46 WHERE `entry` = 17398;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_nascent_orc_blood_furnace' WHERE `entry` = 17398;

-- Broggok
UPDATE `creature_template` SET `MythicTemplateId` = 47 WHERE `entry` = 17380;

DELETE FROM `creature_template_mythic` WHERE `Id` = 47;
INSERT INTO `creature_template_mythic` VALUES
(47, 1, 83, 0, 512700, 0, 15300, 28000, 0.05, 0.05, 0);

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (30913, 30917, 30915);
INSERT INTO `creature_mythic_spell_values` VALUES
(30913, 7500, 0, 0, 0, 0, 0.05),
(30917, 7500, 1000, 0, 0, 0, 0.03),
(30915, 15000, 0, 0, 0, 0, 0.05);

-- felguard brute
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (15615,18072);
INSERT INTO `creature_mythic_spell_values` VALUES
(18072, 200, 0, 0, 0, 0, 0.01),
(15615, 150, 0, 0, 0, 1, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 48;
INSERT INTO `creature_template_mythic` VALUES
(48, 1, 83, 0, 132000, 0, 11200, 16700, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 48 WHERE `entry` = 18894;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_felguard_brute_blood_furnace' WHERE `entry` = 18894;

-- shadowmoon channeler
UPDATE `creature_template` SET `MythicTemplateId` = 49 WHERE `entry` = 17653;

DELETE FROM `creature_template_mythic` WHERE `Id` = 49;
INSERT INTO `creature_template_mythic` VALUES
(49, 1, 83, 0, 63000, 71300, 4700, 6900, 0.05, 0.05, 0);

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` = 30937;
INSERT INTO `creature_mythic_spell_values` VALUES
(30937, 5000, 0, 0, 0, 0, 0.05);

-- Kelidan
DELETE FROM `creature_template_mythic` WHERE `Id` = 50;
INSERT INTO `creature_template_mythic` VALUES
(50, 1, 83, 0, 559000, 714000, 16600, 28100, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 50 WHERE `entry` = 17377;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (28599,30938, 33132);
INSERT INTO `creature_mythic_spell_values` VALUES
(28599, 7000, 0, 0, 0, 0, 0.07),
(30938, 1750, 0, 0, 0, 2, 0.05),
(33132, 10000, 0, 0, 0, 5, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 17491;
INSERT INTO `creature_template_mythic` VALUES
(17491, 1, 82, 0, 100000, 0, 6000, 7000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 17491, `AIName` = "", `ScriptName` = "npc_laughing_skull_rogue" WHERE `entry` = 17491;
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` = 34969;
INSERT INTO `creature_mythic_spell_values` VALUES
(34969, 200, 0, 0, 0, 1, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 17401;
INSERT INTO `creature_template_mythic` VALUES
(17401, 1, 82, 0, 100000, 65000, 2500, 4500, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 17401, `AIName` = "", `ScriptName` = "npc_felhunter_blood_furnace_summoned" WHERE `entry` = 17401;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (13321, 32202);
INSERT INTO `creature_mythic_spell_values` VALUES
(13321, 500, 0, 0, 0, 1, 0.05),
(32202, 500, 0, 0, 0, 1, 0.05);

UPDATE `creature_template` SET `MythicTemplateId` = 17401, `AIName` = "", `ScriptName` = "npc_seductress_blood_furnace" WHERE `entry` = 17399;


