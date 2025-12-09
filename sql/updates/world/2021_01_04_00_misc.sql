-- DB/NPC: Minion of Kaw - reduce respawn https://github.com/SunwellTracker/issues/issues/3443 
UPDATE `creature` SET `spawntimesecs` = 120 WHERE `id` = 25880;
