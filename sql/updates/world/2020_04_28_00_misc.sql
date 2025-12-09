-- DB/Quest: Fix exploit Stranglethorn Fever - Azathothh's report
UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 64, `skinloot` = 0 WHERE `entry` IN (1511,1516,1514);

