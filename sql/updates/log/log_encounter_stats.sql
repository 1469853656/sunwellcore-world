DROP TABLE IF EXISTS `log_encounter_stats`;
CREATE TABLE `log_encounter_stats` (
	`EncounterKey` VARCHAR(50) NOT NULL COLLATE 'utf8mb4_unicode_ci',
	`FinishedAtTimestamp` DATETIME NOT NULL,
	`GroupLeaderCharacterID` INT(10) UNSIGNED NOT NULL,
	`GuildID` INT(10) NOT NULL,
	`CreatureID` INT(10) UNSIGNED NULL DEFAULT NULL,
	`MapType` INT(10) UNSIGNED NULL DEFAULT NULL,
	`EncounterName` VARCHAR(100) NULL DEFAULT NULL COLLATE 'utf8mb4_unicode_ci',
	`Slug` VARCHAR(100) NULL DEFAULT NULL COLLATE 'utf8mb4_unicode_ci',
	`ElapsedTime` DOUBLE UNSIGNED NULL DEFAULT NULL,
	`EncounterDifficulty` TINYINT(3) UNSIGNED NULL DEFAULT NULL,
	`MapID` INT(10) UNSIGNED NULL DEFAULT NULL,
	`MapName` VARCHAR(50) NOT NULL COLLATE 'latin1_swedish_ci',
	`AdditionalData` VARCHAR(50) NOT NULL DEFAULT '' COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`FinishedAtTimestamp`, `GroupLeaderCharacterID`, `GuildID`, `EncounterKey`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `log_encounter_player_stats`;
CREATE TABLE `log_encounter_player_stats` (
	`AI` INT(10) NOT NULL AUTO_INCREMENT,
	`EncounterKey` VARCHAR(50) NOT NULL COLLATE 'utf8mb4_unicode_ci',
	`FinishedAtTimestamp` DATETIME NOT NULL,
	`GroupLeaderCharacterID` INT(10) UNSIGNED NOT NULL,
	`GuildID` INT(10) NOT NULL,
	`CharacterID` INT(10) UNSIGNED NOT NULL,
	`DamageDonePerSecond` DOUBLE UNSIGNED NOT NULL,
	`DamageDone` INT(10) UNSIGNED NOT NULL,
	`EffectiveDamageDonePerSecond` DOUBLE UNSIGNED NOT NULL,
	`EffectiveDamageDone` INT(10) UNSIGNED NOT NULL,
	`DamageTaken` INT(10) UNSIGNED NOT NULL,
	`HealDonePerSecond` DOUBLE UNSIGNED NOT NULL,
	`HealDone` INT(10) UNSIGNED NOT NULL,
	`EffectiveHealDonePerSecond` DOUBLE UNSIGNED NOT NULL,
	`EffectiveHealDone` INT(10) UNSIGNED NOT NULL,
	`HealTaken` INT(10) UNSIGNED NOT NULL,
	`AverageItemLevel` FLOAT NOT NULL,
	`Class` TINYINT(3) UNSIGNED NOT NULL,
	`TalentTree` INT(10) UNSIGNED NOT NULL,
	`Race` TINYINT(3) UNSIGNED NOT NULL,
	`Gender` TINYINT(3) UNSIGNED NOT NULL,
	PRIMARY KEY (`AI`) USING BTREE,
	UNIQUE INDEX `EncounterKey` (`CharacterID`, `FinishedAtTimestamp`, `EncounterKey`, `GroupLeaderCharacterID`, `GuildID`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;
