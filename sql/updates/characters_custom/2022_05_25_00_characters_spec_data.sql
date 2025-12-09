CREATE TABLE `character_spec_data` (
	`CharacterID` INT(10) UNSIGNED NOT NULL,
	`SpecID` INT(10) UNSIGNED NOT NULL,
	`SpecName` VARCHAR(20) NOT NULL,
	PRIMARY KEY (`CharacterID`, `SpecID`)
);
