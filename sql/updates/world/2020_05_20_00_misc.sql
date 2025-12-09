-- DB/Quest: Defending Wyrmrest Temple - fix exploit https://github.com/SunwellWoW/Sunwell-WOTLK/issues/195
-- DB/NPC: Val'kyr Battle-Maiden https://github.com/SunwellWoW/Sunwell-WOTLK/issues/49#issuecomment-631705913
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27629;
INSERT INTO `conditions` VALUES
(22, 2, 27629, 0, 1, 1, 0, 40120, 0, 0, 1, 172, 99, '', 'Summon Wyrmrest Defender  - dont cast when in Flight form'),
(22, 2, 27629, 0, 1, 1, 0, 33943, 0, 0, 1, 172, 99, '', 'Summon Wyrmrest Defender  - dont cast when in Flight form');

DELETE FROM `creature` WHERE `guid` = 130494;
