-- DB/Loot: Titanstone Pendant should not drop from Ulduar 10 man trash
-- DB/Quest: Fueling the Demolishers - should be rewarded by Siegesmith Stronghoof https://github.com/SunwellTracker/issues/issues/3137
-- DB/Loot: Weather-Beaten Journal should not drop if you already have Find fish learned https://github.com/SunwellTracker/issues/issues/3134
DELETE FROM `reference_loot_template` WHERE `entry` = 34112 AND `item` = 7 AND `mincountOrRef` = -34111;

DELETE FROM `creature_questender` WHERE `id` = 31091 AND `quest` = 13200;
DELETE FROM `creature_questender` WHERE `id` = 31106 AND `quest` = 13200;
INSERT INTO `creature_questender` VALUES
(31106, 13200);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 5 AND `SourceEntry` = 34109 AND `ConditionTypeOrReference` = 25;
INSERT INTO `conditions` VALUES
(5, 20708, 34109, 0, 0, 25, 0, 43308, 0, 0, 1, 0, 0, '', 'Weather-Beaten Journal should not drop if player have Find Fish Spell Learned'),
(5, 21228, 34109, 0, 0, 25, 0, 43308, 0, 0, 1, 0, 0, '', 'Weather-Beaten Journal should not drop if player have Find Fish Spell Learned'),
(5, 27481, 34109, 0, 0, 25, 0, 43308, 0, 0, 1, 0, 0, '', 'Weather-Beaten Journal should not drop if player have Find Fish Spell Learned'),
(5, 27513, 34109, 0, 0, 25, 0, 43308, 0, 0, 1, 0, 0, '', 'Weather-Beaten Journal should not drop if player have Find Fish Spell Learned'),
(5, 21113, 34109, 0, 0, 25, 0, 43308, 0, 0, 1, 0, 0, '', 'Weather-Beaten Journal should not drop if player have Find Fish Spell Learned'),
(5, 21150, 34109, 0, 0, 25, 0, 43308, 0, 0, 1, 0, 0, '', 'Weather-Beaten Journal should not drop if player have Find Fish Spell Learned');
