DROP TABLE IF EXISTS `warden_lua_failures`;
CREATE TABLE `warden_lua_failures`
(
	`AccountId` MEDIUMINT unsigned NOT NULL DEFAULT 0,
	`CharacterGuid` BIGINT unsigned NOT NULL DEFAULT 0,
	`CharacterName` varchar(20) NOT NULL DEFAULT "Unknown player name",
	`FailureDate` BIGINT unsigned NOT NULL DEFAULT 0 COMMENT "Unix timestamp",
	`CallStack` TEXT(500) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

