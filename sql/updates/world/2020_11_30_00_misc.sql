-- DB/Spell: Kreeg's Stout Beatdown should not stack with spirit and food buffs https://github.com/SunwellTracker/issues/issues/2786
DELETE FROM `spell_group` WHERE `id` = 1001 AND `spell_id` = 22790;
INSERT INTO `spell_group` VALUES
(1001, 22790, 0);