-- DB/Quest: Thane of Voldrune - fix issue with Flamebringer spawn
UPDATE `smart_scripts` SET `target_type` = 10, `target_param1` = 110538, `target_param2` = 27292 WHERE `entryorguid` = 27297 AND `id` = 0;
