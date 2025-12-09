-- DB/Go: Ammo for Rumbleshot Quest - reduce spawntime https://github.com/SunwellTracker/issues/issues/3107
UPDATE `gameobject` SET `spawntimesecs` = 60 WHERE `guid` = 10663;
