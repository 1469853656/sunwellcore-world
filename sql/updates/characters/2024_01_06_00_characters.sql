ALTER TABLE `characters`
	ADD COLUMN `totalSeasonHonorPoints` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `totalHonorPoints`,
	ADD COLUMN `isCountedAsMaxLevelPathOfWarCharacter` TINYINT(3) UNSIGNED NULL DEFAULT '0' AFTER `deleteInfos_Name`;
