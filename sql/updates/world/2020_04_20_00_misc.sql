-- DB/Quest: Valduran the Stormborn - fix issue with Valduran sometimes being stuck/non interactive https://github.com/SunwellTracker/issues/issues/3082 
UPDATE `smart_scripts` SET `target_type` = 25 WHERE `entryorguid` = 2936800 AND `id` = 9;
