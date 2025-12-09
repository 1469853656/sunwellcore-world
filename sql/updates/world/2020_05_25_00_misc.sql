-- DB/Loot: reinforced crate loot - will drop 2-3 frostweave cloth https://github.com/SunwellTracker/issues/issues/3212
UPDATE `item_loot_template` SET `mincountOrRef` = 2, `maxcount` = 3 WHERE `entry` = 44475 AND `item` = 33470;
