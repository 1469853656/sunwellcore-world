CREATE TABLE IF NOT EXISTS `guardian_spell` (
	`player_guid` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
	`spell` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
	`active` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	PRIMARY KEY (`player_guid`, `spell`)
)
COMMENT='Guardian System'
