SET @CONQUEST = 45624;
SET @HEROISM = 40752;
SET @VALOR = 40753;
SET @TRIUMPH = 47241;
SET @FROST = 49426;
-- Delete emblem from Keg-Shaped Treasure Chest
DELETE FROM `item_loot_template` WHERE `entry` = 54535 AND `item` IN (@CONQUEST, @HEROISM, @VALOR, @TRIUMPH);

-- Update quest rewards for
-- 24511 - [Echoes of Tortured Souls]
-- 24499 - [Echoes of Tortured Souls]
-- 24710 - [Deliverance from the Pit]
-- 24712 - [Deliverance from the Pit]
-- 24500 - [Wrath of the Lich King]
-- 24802 - [Wrath of the Lich King]
UPDATE `quest_template` SET `RewardItemId1` = @FROST WHERE `id` IN (24511,24499,24710,24712,24500,24802) AND `RewardItemId1` IN (@CONQUEST, @HEROISM, @VALOR, @TRIUMPH);

-- Spawn Dark Ranger Vorel, Apprentice Nelphi in Dalaran
DELETE FROM `creature` WHERE `guid` = 101236 AND `id` = 37776;
DELETE FROM `creature` WHERE `guid` = 133649 AND `id` = 37780;
INSERT INTO `creature` VALUES 
(101236, 37776, 571, 1, 1, 0, 1, 5689.76, 671.083, 646.08, 0.244346, 120, 0, 0, 1, 0, 2, 0, 0, 0),
(133649, 37780, 571, 1, 1, 0, 1, 5915.75, 626.745, 645.996, 0.244346, 120, 0, 0, 1, 0, 2, 0, 0, 0);

-- Bronjahm 5hc 36498
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 36498 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- Devourer of Souls 5hc 37677
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 37677 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- Forgemaster Garfrost 5hc 37613
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 37613 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- Ick 5hc 37627
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 37627 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- Scourgelord Tyrannus 5hc 36938
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 36938 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- Falric 5hc 38599
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 38599 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- Marwyn 5hc 38603
UPDATE `creature_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 38603 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
-- The Captains Chest 5hc 38603
UPDATE `gameobject_loot_template` SET `item` = @TRIUMPH WHERE `entry` = 27993 AND `item` IN (@CONQUEST, @HEROISM, @VALOR);
