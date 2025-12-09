-- azure warder
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (56777,17741);
INSERT INTO `creature_mythic_spell_values` VALUES
(17741, 260000, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 60;
INSERT INTO `creature_template_mythic` VALUES
(60, 1, 82, 0, 130000, 0, 10600, 15000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 60 WHERE `entry` = 26716;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_azure_warder_nexus' WHERE `entry` = 26716;
 
-- mage ascendant
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (50182, 15244, 36808, 34933, 12737, 12466);
INSERT INTO `creature_mythic_spell_values` VALUES
(50182, 50, 250, 0, 0, 0, 0.05),
(15244, 0, 900, 0, 0, 0, 0.05),
(36808, 900, 0, 0, 0, 0, 0.05),
(34933, 1200, 0, 0, 0, 0, 0.05),
(12737, 1000, 0, 0, 0, 0, 0.05),
(12466, 900, 0, 0, 0, 0, 0.07);


DELETE FROM `spell_script_names` WHERE `spell_id` IN (35191, 27024, 48191, 47687);
INSERT INTO `spell_script_names` VALUES
(35191, "spell_arcane_explosion_nexus"),
(27024 , "spell_nexus_immolation_trap"),
(48191, "spell_nexus_rapid_shot_periodic"),
(47687, "spell_chaotic_rift_aura_nexus");

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_mage_hunter_ascendant_nexus", `MythicTemplateId` = 61 WHERE `entry` = 26727;
DELETE FROM `creature_template_mythic` WHERE `Id`=  61;
INSERT INTO `creature_template_mythic` VALUES
(61, 2, 82, 0, 63000, 40000, 8800, 13600, 0.5, 0.5, 0);

-- mage slayer
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (50131);
INSERT INTO `creature_mythic_spell_values` VALUES
(50131, 100, 0, 0, 0, 0, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 62;
INSERT INTO `creature_template_mythic` VALUES
(62, 1, 82, 0, 63000, 20000, 8000, 12700, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 62 WHERE `entry` = 26730;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_mage_slayer_nexus' WHERE `entry` = 26730;

-- alliance ranger
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (48191,15620, 47777);
INSERT INTO `creature_mythic_spell_values` VALUES
(15620, 750, 0, 0, 0, 0, 0.05),
(48191, 0, 0, 0, 0, 0, 0.05),
(47777, 4500, 2500, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 63;
INSERT INTO `creature_template_mythic` VALUES
(63, 5, 82, 0, 63000, 20000, 7900, 11400, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 63 WHERE `entry` IN (26802, 26801);
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_alliance_ranger_nexus' WHERE `entry` = 26802;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_alliance_ranger_nexus' WHERE `entry` = 26801;

-- alliance cleric
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (17139,47697);
INSERT INTO `creature_mythic_spell_values` VALUES
(17139, 50000, 0, 0, 0, 0, 0.07),
(47697, 7500, 0, 0, 0, 0, 0.04);

DELETE FROM `creature_template_mythic` WHERE `Id` = 64;
INSERT INTO `creature_template_mythic` VALUES
(64, 4, 82, 0, 63000, 49000, 5100, 7200, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 64 WHERE `entry` IN (26805, 26803);
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_alliance_cleric_nexus' WHERE `entry` IN (26805, 26803);

-- commander stoutbeard
DELETE FROM `creature_template_mythic` WHERE `Id` = 65;
INSERT INTO `creature_template_mythic` VALUES
(65, 3, 83, 0, 510000, 0, 15800, 22000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 65 WHERE `entry` = 26796;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (31403, 60067);
INSERT INTO `creature_mythic_spell_values` VALUES
(31403, 1000, 0, 0, 0, 0, 0),
(60067, 150, 0, 0, 0, 0.05, 0);

-- steward
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (47780,47779);
INSERT INTO `creature_mythic_spell_values` VALUES
(47779, 0, 0, 0, 0, 0, 0),
(47780, 150, 0, 0, 0, 0, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 66;
INSERT INTO `creature_template_mythic` VALUES
(66, 1, 82, 0, 63000, 0, 7900, 12100, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 66 WHERE `entry` = 26729;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_steward_nexus' WHERE `entry` = 26729;

-- mage hunter initiate
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (25058,50198,17682);
INSERT INTO `creature_mythic_spell_values` VALUES
(17682, 500, 0, 0, 0, 0, 0.03),
(50198, 4500, 2000, 0, 5000, 1, 0.05),
(25058, 650, 0, 0, 0, 0, 0.03);

DELETE FROM `creature_template_mythic` WHERE `Id` = 67;
INSERT INTO `creature_template_mythic` VALUES
(67, 2, 82, 0, 63000, 49000, 4900, 7400, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 67 WHERE `entry` = 26728;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_mage_hunter_initiate_nexus' WHERE `entry` = 26728;

-- Grand magus telestra
DELETE FROM `creature_template_mythic` WHERE `Id` = 68;
INSERT INTO `creature_template_mythic` VALUES
(68, 2, 83, 0, 499000, 213000, 13900, 19300, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 68 WHERE `entry` = 26731;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (47772, 47773);
INSERT INTO `creature_mythic_spell_values` VALUES
(47772, 8000, 0, 0, 0, 0, 0.05),
(47773, 15000, 0, 0, 0, 0, 0.05);

-- grand magus telestra fire
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (47723,47721);
INSERT INTO `creature_mythic_spell_values` VALUES
(47721, 8000, 0, 0, 0, 0, 0.05),
(47723, 7500, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 69;
INSERT INTO `creature_template_mythic` VALUES
(69, 2, 83, 0, 112900, 71200, 6000, 10200, 0.005, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 69 WHERE `entry` = 26928;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_grand_magus_telestra_fire' WHERE `entry` = 26928;

-- grand magus time stopper
UPDATE `creature_template` SET `MythicTemplateId` = 69 WHERE `entry` = 26929;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_grand_magus_telestra_time' WHERE `entry` = 26929;

-- grand magus frost
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (47729,47727);
INSERT INTO `creature_mythic_spell_values` VALUES
(47727, 9000, 0, 0, 12000, 5, 0.05),
(47729, 4500, 0, 0, 0, 0, 0.05);

UPDATE `creature_template` SET `MythicTemplateId` = 69 WHERE `entry` = 26930;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_grand_telestra_frost' WHERE `entry` = 26930;

-- azure enforcer
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (15284,58460);
INSERT INTO `creature_mythic_spell_values` VALUES
(58460, 200, 0, 0, 0, 0, 0),
(15284, 150, 0, 0, 0, 0, 0.01);

DELETE FROM `creature_template_mythic` WHERE `Id` = 70;
INSERT INTO `creature_template_mythic` VALUES
(70, 1, 82, 0, 63000, 0, 8500, 12100, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 70 WHERE `entry` = 26734;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_azure_enforcer_nexus' WHERE `entry` = 26734;

-- azure scale-binder
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (15586,38881);
INSERT INTO `creature_mythic_spell_values` VALUES
(38881, 6500, 0, 0, 0, 0, 0.05),
(15586, 3000, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 71;
INSERT INTO `creature_template_mythic` VALUES
(71, 2, 82, 0, 63000, 49000, 7100, 9900, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 71 WHERE `entry` = 26735;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_azure_scale_binder' WHERE `entry` = 26735;

-- crazed mana-wraith
UPDATE `creature_template` SET `MythicTemplateId` = 72 WHERE `entry` = 26746;

DELETE FROM `creature_template_mythic` WHERE `Id` = 72;
INSERT INTO `creature_template_mythic` VALUES
(72, 1, 82, 0, 19000, 35800, 3200, 6100, 0.05, 0.05, 0);

UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crazed_mana_wraith_nexus' WHERE `entry` = 26746;

-- chaotic rift
DELETE FROM `creature_template_mythic` WHERE `Id` = 73;
INSERT INTO `creature_template_mythic` VALUES
(73, 6, 83, 0, 63000, 0, 1000, 1000, 0, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 73 WHERE `entry` = 26918;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_chaotic_rift_nexus' WHERE `entry` = 26918;

-- crazed mana-surge
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (29882,48054,47696);
INSERT INTO `creature_mythic_spell_values` VALUES
(47696, 3500, 20, 0, 0, 10, 0.02),
(48054, 4500, 0, 0, 0, 0, 0.05),
(29882, 1500, 0, 0, 0, 5, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 74;
INSERT INTO `creature_template_mythic` VALUES
(74, 1, 82, 0, 89000, 0, 7400, 10200, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 74 WHERE `entry` = 26737;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crazed_mana_surge_nexus' WHERE `entry` = 26737;

/* ToDo
-- anomalus
DELETE FROM `creature_template_mythic` WHERE `Id` = 75;
INSERT INTO `creature_template_mythic` VALUES
(75, 1, 83, 0, 641000, 0, 17500, 29100, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 75 WHERE `entry` = 26763;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (47751);
INSERT INTO `creature_mythic_spell_values` VALUES
(47751, 13500, 0, 0, 0, 0, 0.05);
**/
-- crystalline frayer
DELETE FROM `creature_template_mythic` WHERE `Id` = 76;
INSERT INTO `creature_template_mythic` VALUES
(76, 1, 82, 0, 14500, 0, 12000, 18000, 0.05, 0.05, 0);
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crystalline_frayer' WHERE `entry` = 26793;

-- crystalline keeper
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (33688);
INSERT INTO `creature_mythic_spell_values` VALUES
(33688, 200, 0, 0, 0, 0, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 77;
INSERT INTO `creature_template_mythic` VALUES
(77, 1, 82, 0, 63000, 0, 8900, 12100, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 77 WHERE `entry` = 26782;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crystalline_keeper_nexus' WHERE `entry` = 26782;

-- Crystalline tender
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (51972);
INSERT INTO `creature_mythic_spell_values` VALUES
(51972, 15000, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 78;
INSERT INTO `creature_template_mythic` VALUES
(78, 1, 82, 0, 63000, 19000, 7200, 10300, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 78 WHERE `entry` = 28231;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crystalline_tender_nexus' WHERE `entry` = 28231;

-- crystalline protector
DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (50302,47698,30633);
INSERT INTO `creature_mythic_spell_values` VALUES
(30633, 8000, 0, 0, 0, 5, 0.05),
(47698, 0, 6000, 0, 0, 0, 0.05),
(50302, 5000, 0, 0, 0, 0, 0.05);

DELETE FROM `creature_template_mythic` WHERE `Id` = 79;
INSERT INTO `creature_template_mythic` VALUES
(79, 1, 82, 0, 132000, 0, 14000, 20000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 79 WHERE `entry` = 26792;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crystalline_protector' WHERE `entry` = 26792;

-- ormorok
UPDATE `creature_template` SET `MythicTemplateId` = 80 WHERE `entry` = 26794;
DELETE FROM `creature_template_mythic` WHERE `Id` = 80;
INSERT INTO `creature_template_mythic` VALUES
(80, 1, 83, 0, 701000, 0, 17100, 28100, 0.05, 0.05, 0);

-- Keristrasza
UPDATE `creature_template` SET `MythicTemplateId` = 81 WHERE `entry` = 26723;
DELETE FROM `creature_template_mythic` WHERE `Id` = 81;
INSERT INTO `creature_template_mythic` VALUES
(81, 1, 83, 0, 724500, 410000, 16500, 27400, 0.05, 0.05, 0);

-- alliance berserker
UPDATE `creature_template` SET `MythicTemplateId` = 26800, `AIName` = "", `ScriptName` = "npc_alliance_berserker" WHERE `entry` = 26800;
UPDATE `creature_template` SET `MythicTemplateId` = 26800, `AIName` = "", `ScriptName` = "npc_alliance_berserker" WHERE `entry` = 26799;
DELETE FROM `creature_template_mythic` WHERE `Id` = 26800;
INSERT INTO `creature_template_mythic` VALUES
(26800, 1, 82, 0, 701000, 0, 17100, 28100, 0.05, 0.05, 0);

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` = 38682;
INSERT INTO `creature_mythic_spell_values` VALUES
(38682, 0, 100, 0, 0, 5, 0);

DELETE FROM `creature_template_mythic` WHERE `Id` = 26793;
INSERT INTO `creature_template_mythic` VALUES
(26793, 1, 81, 0, 15000, 0, 1500, 3000, 0.1, 0.1, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 26793 WHERE `entry` = 26793;

DELETE FROM `creature_template_mythic` WHERE `Id` = 26722;
INSERT INTO `creature_template_mythic` VALUES
(26722, 2, 82, 0, 100000, 100000, 8000, 12000, 0.05, 0.05, 0);

UPDATE `creature_template` SET `MythicTemplateId` = 26722, `AIName` = "", `ScriptName` = "npc_azure_magus_nexus" WHERE `entry` = 26722;

DELETE FROM `creature_mythic_spell_values` WHERE `SpellId` IN (15530, 37132);
INSERT INTO `creature_mythic_spell_values` VALUES
(15530, 1000, 0, 0, 0, 0, 0.04),
(37132, 8000, 2000, 0, 0, 0, 0.05);

