-- DB/NPC: Chillmaw - reduce respawn time https://github.com/SunwellTracker/issues/issues/3191
-- Core/Item: Gnomish Death Ray - correct damage https://github.com/SunwellTracker/issues/issues/3184
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `guid` = 200851 AND `id` = 33687;