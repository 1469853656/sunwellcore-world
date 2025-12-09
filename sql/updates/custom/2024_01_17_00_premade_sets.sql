ALTER TABLE `premade_set_bag_items`
	ADD COLUMN `EnchantmentID` INT(10) NOT NULL DEFAULT '0' AFTER `ItemCount`,
	ADD COLUMN `GemID_0` INT(10) NOT NULL DEFAULT '0' AFTER `EnchantmentID`,
	ADD COLUMN `GemID_1` INT(10) NOT NULL DEFAULT '0' AFTER `GemID_0`,
	ADD COLUMN `GemID_2` INT(10) NOT NULL DEFAULT '0' AFTER `GemID_1`,
	ADD COLUMN `PrismaticGemID` INT(10) NOT NULL DEFAULT '0' AFTER `GemID_2`;

DROP TABLE IF EXISTS `premade_set_glyphs`;
CREATE TABLE `premade_set_glyphs` (
	`SetName` VARCHAR(50) NOT NULL COLLATE 'latin1_swedish_ci',
	`GlyphID` INT(10) UNSIGNED NOT NULL,
	`GlyphIndex` INT(10) UNSIGNED NOT NULL,
	INDEX `SetName` (`SetName`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;
