-- DB/Quest: Trial of the sea lion - fix Half pendant of Aquatic Agility container https://github.com/SunwellTracker/issues/issues/3245
UPDATE `gameobject` SET `spawntimesecs` = 50 WHERE `guid` = 13523;
