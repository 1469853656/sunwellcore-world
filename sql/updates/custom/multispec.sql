ALTER TABLE `characters_data`
	ADD COLUMN `MultipleSpecsFlag` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `RacialChosen`;
	