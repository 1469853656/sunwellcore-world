ALTER TABLE `log_encounter_stats`
	ADD COLUMN `CustomDifficulty` INT UNSIGNED NULL DEFAULT '0' AFTER `EncounterDifficulty`;
