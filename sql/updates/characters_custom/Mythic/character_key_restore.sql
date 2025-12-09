DROP TABLE IF EXISTS `character_mythic_key_store`;
CREATE TABLE `character_mythic_key_store`
(
	`PlayerGUID` INT(10) UNSIGNED NOT NULL DEFAULT 0,
	`Time` INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
