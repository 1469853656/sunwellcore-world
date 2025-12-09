-- Fix Leviathan 10 emblems for 3 difficulties
UPDATE `creature_loot_template` SET `item` = 45624 WHERE `item` = 40753 AND `entry`= 33113 AND `lootmode` = 8;
DELETE FROM `creature_loot_template` WHERE `entry` = 33113 AND `item` = 40753 AND `lootmode` = 16;
UPDATE `creature_loot_template` SET `mincountOrRef` = 2, `maxcount` = 2 WHERE `item` = 45624 AND `entry`= 33113 AND `lootmode` = 16;
UPDATE `creature_loot_template` SET `item`= 45624 WHERE `item` IN (40752, 40753) AND `entry` = 33113;