-- DB/NPC: Shadow of Thel'zan the Duskbringer - fix $N
UPDATE `smart_scripts` SET `target_type` = 18, `target_param1` = 20 WHERE `entryorguid` = 2751000 AND `id` = 0;