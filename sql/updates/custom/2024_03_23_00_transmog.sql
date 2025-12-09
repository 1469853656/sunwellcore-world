DROP TABLE IF EXISTS `transmog_enchant_names`;
CREATE TABLE `transmog_enchant_names` 
(
	`EnchantmentID` INT(10) UNSIGNED NOT NULL,
	`Name` VARCHAR(100) NOT NULL COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`EnchantmentID`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

INSERT INTO `transmog_enchant_names` (`EnchantmentID`, `Name`) VALUES 
(1, 'Frozen Rune Weapon, Razorice, Shadow Oil'),
(2, 'Impact, Striking'),
(24, 'Superior Impact'),
(25, 'Fiery, Demonslaying, Icebreaker, Giant Slayer, Lichbane'),
(26, 'Poison'),
(27, 'Frost Oil'),
(28, 'Sharpened'),
(29, 'Spirit, Intellect'),
(31, 'Beastslayer, Elemental Slayer, Scourgebane, Righteous Weapon Coating'),
(32, 'Flametongue'),
(33, 'Frostbrand'),
(42, 'Striking, Impact, Blessed Weapon Coating'),
(61, 'Rockbiter'),
(81, 'Windfury'),
(101, 'Savagery'),
(102, 'Healing'),
(103, 'Crusader, Lifeward'),
(104, 'Holy Glow'),
(105, 'Arcane Glow'),
(106, 'Potency, Earthliving'),
(107, 'Feedback, Spellpower'),
(125, 'Agility'),
(126, 'Icy Chill'),
(128, 'Arcane Surge'),
(139, 'Dancing Rune Weapon'),
(151, 'Radiance'),
(155, 'Mongoose'),
(156, 'Savagery, Rune of Swordshattering, Rune of Swordbreaking'),
(157, 'Soulfrost'),
(158, 'Sunfire'),
(159, 'Battlemaster, Titanium Weapon Chain'),
(160, 'Spellsurge, Rune of the Fallen Crusader'),
(161, 'Unholy Weapon, Black Magic, Rune of Spellshattering, Rune of Spellbreaking'),
(164, 'Blood Draining'),
(165, 'Executioner'),
(166, 'Deathfrost, Rune of Cinderglacier'),
(172, 'Lichflame'),
(173, 'Empower Rune Weapon'),
(178, 'Berserking'),
(186, 'Blade Ward');

DROP TABLE IF EXISTS `transmog_sets`;
CREATE TABLE `transmog_sets` 
(
	`SetID` INT(10) UNSIGNED NOT NULL,
	`SetName` VARCHAR(100) NOT NULL COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`SetID`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `transmog_set_items`;
CREATE TABLE `transmog_set_items` 
(
	`SetID` INT(10) NOT NULL,
	`ItemID` INT(10) UNSIGNED NOT NULL,
	`SlotID` INT(10) UNSIGNED NULL DEFAULT NULL,
	PRIMARY KEY (`SetID`, `ItemID`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;
