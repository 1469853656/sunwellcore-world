DROP TABLE IF EXISTS `custom_account_transmog`;
CREATE TABLE `custom_account_transmog` 
(
	`accountid` INT(10) UNSIGNED NOT NULL,
	`type` INT(10) UNSIGNED NOT NULL,
	`entry` INT(10) UNSIGNED NOT NULL,
	`purchased` TINYINT(3) UNSIGNED NULL DEFAULT '0',
	PRIMARY KEY (`accountid`, `type`, `entry`) USING BTREE,
	INDEX `accountid` (`accountid`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `custom_purchased_transmogrification_sets`;
CREATE TABLE `custom_purchased_transmogrification_sets` 
(
	`owner` INT(10) UNSIGNED NOT NULL,
	`presetid` TINYINT(3) UNSIGNED NOT NULL,
	`setname` TEXT NULL DEFAULT NULL COLLATE 'utf8mb4_unicode_ci',
	`setdata` TEXT NULL DEFAULT NULL COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`owner`, `presetid`) USING BTREE,
	INDEX `Owner` (`owner`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `custom_transmogrification_sets`;
CREATE TABLE `custom_transmogrification_sets` 
(
	`owner` INT(10) UNSIGNED NOT NULL,
	`presetid` INT(10) UNSIGNED NOT NULL,
	`setname` TEXT NULL DEFAULT NULL COLLATE 'utf8mb4_unicode_ci',
	`setdata` TEXT NULL DEFAULT NULL COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`owner`, `presetid`) USING BTREE,
	INDEX `Owner` (`owner`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;
