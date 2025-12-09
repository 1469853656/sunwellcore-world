ALTER TABLE `characters_data`
	ADD COLUMN `PremiumBoostFlag` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `MultipleSpecsFlag`;
	
ALTER TABLE `characters_data`
	ADD COLUMN `TokenUsePhase` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `PremiumBoostFlag`;

ALTER TABLE `characters_data`
	ADD COLUMN `ProfessionTokenUsePhase` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0' AFTER `TokenUsePhase`;