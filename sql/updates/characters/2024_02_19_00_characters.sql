ALTER TABLE `characters`
	ADD COLUMN `spellQueueWindow` INT UNSIGNED NULL DEFAULT '400' AFTER `isCountedAsMaxLevelPathOfWarCharacter`;
