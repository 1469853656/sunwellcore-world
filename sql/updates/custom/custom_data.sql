CREATE TABLE `characters_data` (
	`CharacterID` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
	`CurrentRacial` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
	`RacialChosen` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`CharacterID`)
)
COMMENT='Custom Player System';
