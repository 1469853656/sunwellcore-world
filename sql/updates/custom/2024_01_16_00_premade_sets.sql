DROP TABLE IF EXISTS `premade_sets`;
CREATE TABLE `premade_sets` (
	`SetName` VARCHAR(50) NOT NULL COLLATE 'latin1_swedish_ci',
	`SetLevel` INT(10) UNSIGNED NOT NULL,
	`SetClass` INT(10) UNSIGNED NOT NULL,
	`SetSpec` INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`SetName`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `premade_set_bag_items`;
CREATE TABLE `premade_set_bag_items` (
	`SetName` VARCHAR(50) NOT NULL COLLATE 'latin1_swedish_ci',
	`ItemID` INT(10) UNSIGNED NOT NULL,
	`ItemCount` INT(10) UNSIGNED NOT NULL DEFAULT '1',
	INDEX `SetName` (`SetName`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `premade_set_equipment_items`;
CREATE TABLE `premade_set_equipment_items` (
	`SetName` VARCHAR(50) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci',
	`EquipmentSlot` INT(10) UNSIGNED NOT NULL,
	`ItemID` INT(10) UNSIGNED NOT NULL,
	`EnchantmentID` INT(10) NOT NULL DEFAULT '0',
	`GemID_0` INT(10) NOT NULL DEFAULT '0',
	`GemID_1` INT(10) NOT NULL DEFAULT '0',
	`GemID_2` INT(10) NOT NULL DEFAULT '0',
	`PrismaticGemID` INT(10) NOT NULL DEFAULT '0',
	PRIMARY KEY (`SetName`, `EquipmentSlot`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `premade_set_talents`;
CREATE TABLE `premade_set_talents` (
	`SetName` VARCHAR(50) NOT NULL COLLATE 'latin1_swedish_ci',
	`TalentID` INT(10) UNSIGNED NOT NULL,
	`TalentRank` INT(10) UNSIGNED NOT NULL,
	INDEX `SetName` (`SetName`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;
