ALTER TABLE `character_instance`
	ADD COLUMN `CompletedAscendedEncounters` INT(10) UNSIGNED NOT NULL DEFAULT (0) AFTER `extended`;
