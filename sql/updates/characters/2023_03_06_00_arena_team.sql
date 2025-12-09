ALTER TABLE `arena_team`
ADD COLUMN `charName` CHAR(255) DEFAULT NULL AFTER `name`,
ADD COLUMN `matchmakerRating` SMALLINT UNSIGNED DEFAULT 0 NOT NULL AFTER `rating`,
ADD COLUMN `maxMMR` SMALLINT UNSIGNED DEFAULT 0 NOT NULL AFTER `matchmakerRating`;

DROP TABLE `arena_team_member`;
DROP TABLE `character_arena_stats`;
