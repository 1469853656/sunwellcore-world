ALTER TABLE `creature_template`
	ADD COLUMN `Experience_mod` FLOAT NOT NULL DEFAULT '1' AFTER `Armor_mod`;

UPDATE `creature_template` SET `Experience_mod`=5 WHERE `entry`=2275;

