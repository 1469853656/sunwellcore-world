-- DB/Quest: The Demon Scarred CLoak - update drop chance for quest item
-- DB/Quest: Trinkets... - Hoard of the Black Dragonflight should not contain Preserved Pheromone Mixture
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 90 WHERE `entry` = 3056 AND `item` = 4854;
DELETE FROM `item_loot_template` WHERE `entry` = 10569 AND `item` = 11570;