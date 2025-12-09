ALTER TABLE `guardian_spell`
	CHANGE COLUMN `active` `active` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `spell`,
	ADD COLUMN `value_type` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `active`,
	DROP PRIMARY KEY,
	ADD PRIMARY KEY (`player_guid`, `spell`, `value_type`);
