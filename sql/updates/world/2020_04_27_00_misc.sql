-- DB/NPC: Valkyrion Aspirant - fix spawn position https://github.com/SunwellTracker/issues/issues/3117
-- DB/Item: Figurine - Emerald Boar - increase boar's damage https://github.com/SunwellTracker/issues/issues/3112
UPDATE `creature` SET `position_x` = 7269.03, `position_y` = -111.28, `position_z` = 772.95 WHERE `guid` = 107727;
UPDATE `creature_template` SET `mindmg` = 40, `maxdmg` = 80 WHERE `entry` = 30568;
