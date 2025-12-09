DROP TABLE IF EXISTS `xp_rate_brackets`;
CREATE TABLE `xp_rate_brackets` (
	`Level` TINYINT(3) UNSIGNED NOT NULL,
	`ExperienceRate` FLOAT NOT NULL,
	PRIMARY KEY (`Level`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;
