ALTER TABLE `characters_data`
	CHANGE COLUMN `RacialChosen` `CustomCharacterFlags` BIGINT UNSIGNED NOT NULL DEFAULT (0) AFTER `CurrentRacial`;
