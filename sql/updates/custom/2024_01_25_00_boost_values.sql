DROP TABLE IF EXISTS `boost_spell_values`;
CREATE TABLE `boost_spell_values`
(
	`SpellType` ENUM('Effect0','Effect1','Effect2','AuraDuration','Effect0Modifier','Effect1Modifier','Effect2Modifier','Effect0Radius','Effect1Radius','Effect2Radius') NOT NULL COLLATE 'utf8mb4_unicode_ci',
	`SpellID` INT(10) UNSIGNED NOT NULL,
	`MapID` INT(10) UNSIGNED NOT NULL,
	`Difficulty` INT(10) DEFAULT '-1',
	`ValueMin` INT(10) NULL DEFAULT NULL,
	`ValueMax` INT(10) NULL DEFAULT NULL,
	`Comment` VARCHAR(75) NULL DEFAULT '' COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`SpellType`, `SpellID`, `MapID`, `Difficulty`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

DROP TABLE IF EXISTS `boost_stat_values`;
CREATE TABLE `boost_stat_values`
(
	`StatType` ENUM('Health','Mana','MainhandDamage','OffhandDamage','Level','HealthModifier','ManaModifier','MainhandDamageModifier','OffhandDamageModifier') NOT NULL COLLATE 'utf8mb4_unicode_ci',
	`CreatureID` INT(10) UNSIGNED NOT NULL,
	`MapID` INT(10) UNSIGNED NOT NULL,
	`Difficulty` INT(10) DEFAULT '-1',
	`ValueMin` INT(10) NULL DEFAULT NULL,
	`ValueMax` INT(10) NULL DEFAULT NULL,
	`Comment` VARCHAR(75) NULL DEFAULT '' COLLATE 'utf8mb4_unicode_ci',
	PRIMARY KEY (`StatType`, `CreatureID`, `MapID`, `Difficulty`) USING BTREE
)
COLLATE='utf8mb4_unicode_ci'
ENGINE=InnoDB;

SET @DIFFICULTY_10_NORMAL := 0;
SET @DIFFICULTY_25_NORMAL := 1;

/*

	NAXXRAMAS (25)

*/
SET @MAPID_NAXXRAMAS := 533;
DELETE FROM `boost_stat_values` WHERE `MapID`=@MAPID_NAXXRAMAS AND `Difficulty`=@DIFFICULTY_25_NORMAL;
DELETE FROM `boost_spell_values` WHERE `MapID`=@MAPID_NAXXRAMAS AND `Difficulty`=@DIFFICULTY_25_NORMAL;

-- Patchwerk
SET @BOSS_PATCHWERK := 16028;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_PATCHWERK, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 25100000, 25100000, 'Naxxramas - Patchwerk'),
('MainhandDamage', @BOSS_PATCHWERK, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 45000, 55000, 'Naxxramas - Patchwerk');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 59192, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 80000, 85000, 'Naxxramas - Patchwerk - Hateful Strike');

-- Grobbulus
SET @BOSS_GROBBULUS := 15931;
SET @NPC_FALLOUT_SLIME := 16290;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_GROBBULUS, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 19100000, 19100000, 'Naxxramas - Grobbulus'),
('MainhandDamage', @BOSS_GROBBULUS, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 50000, 60000, 'Naxxramas - Grobbulus'),
('Health', @NPC_FALLOUT_SLIME, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 550000, 550000, 'Naxxramas - Grobbulus - Fallout Slime'),
('MainhandDamage', @NPC_FALLOUT_SLIME, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 5000, 5000, 'Naxxramas - Grobbulus - Fallout Slime');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES
('Effect0', 54363, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 7800, 8200, 'Naxxramas - Grobbulus - Poison Cloud'),
('Effect0', 54364, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 19000, 21000, 'Naxxramas - Grobbulus - Slime Spray'),
('Effect0', 54368, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 2800, 3200, 'Naxxramas - Grobbulus - Fallout Slime - Disease Cloud'),
('Effect0Radius', 54368, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 500, 500, 'Naxxramas - Grobbulus - Fallout Slime - Disease Cloud');

-- Gluth
SET @BOSS_GLUTH := 15932;
SET @NPC_ZOMBIE_CHOW := 16360;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_GLUTH, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 16900000, 16900000, 'Naxxramas - Gluth'),
('MainhandDamage', @BOSS_GLUTH, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 32000, 38000, 'Naxxramas - Gluth'),
('Health', @NPC_ZOMBIE_CHOW, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 1000000, 1000000, 'Naxxramas - Gluth - Zombie Chow');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('AuraDuration', 25646, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 40000, 40000, 'Naxxramas - Gluth - Mortal Wound');

-- Thaddius
SET @BOSS_THADDIUS := 15928;
SET @NPC_FEUGEN := 15930;
SET @NPC_STALAGG := 15929;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_THADDIUS, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 55600000, 55600000, 'Naxxramas - Thaddius'),
('MainhandDamage', @BOSS_THADDIUS, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 50000, 60000, 'Naxxramas - Thaddius'),
('Health', @NPC_FEUGEN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 3800000, 3800000, 'Naxxramas - Thaddius - Feugen'),
('MainhandDamage', @NPC_FEUGEN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 32000, 38000, 'Naxxramas - Thaddius - Feugen'),
('Health', @NPC_STALAGG, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 3800000, 3800000, 'Naxxramas - Thaddius - Stalagg'),
('MainhandDamage', @NPC_STALAGG, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 32000, 38000, 'Naxxramas - Thaddius - Stalagg');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 45185, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 18850, 21151, 'Naxxramas - Thaddius - Stomp'),
('Effect0', 54531, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 8125, 8876, 'Naxxramas - Thaddius - Chain Lightning');

-- Anub'rekhan
SET @BOSS_ANUBREKHAN := 15956;
SET @NPC_CRYPT_GUARD := 16573;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_ANUBREKHAN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 16900000, 16900000, 'Naxxramas - Anubrekhan'),
('MainhandDamage', @BOSS_ANUBREKHAN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 32000, 38000, 'Naxxramas - Anubrekhan'),
('Health', @NPC_CRYPT_GUARD, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 1000000, 1000000, 'Naxxramas - Anubrekhan - Crypt Guard'),
('MainhandDamage', @NPC_CRYPT_GUARD, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 10000, 15000, 'Naxxramas - Anubrekhan- Crypt Guard');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 56090, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 6800, 7200, 'Naxxramas - Anubrekhan - Impale');

-- Faerlina
SET @BOSS_FAERLINA := 15953;
SET @NPC_NAXXRAMAS_WORSHIPPER := 16506;
SET @NPC_NAXXRAMAS_FOLLOWER := 16505;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_FAERLINA, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 17200000, 17200000, 'Naxxramas - Faerlina'),
('MainhandDamage', @BOSS_FAERLINA, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 26000, 32000, 'Naxxramas - Faerlina'),
('Health', @NPC_NAXXRAMAS_WORSHIPPER, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 420000, 420000, 'Naxxramas - Faerlina - Naxxramas Worshipper'),
('MainhandDamage', @NPC_NAXXRAMAS_WORSHIPPER, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 6000, 6500, 'Naxxramas - Faerlina - Naxxramas Worshipper'),
('Health', @NPC_NAXXRAMAS_FOLLOWER, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 1000000, 1000000, 'Naxxramas - Faerlina - Naxxramas Follower'),
('MainhandDamage', @NPC_NAXXRAMAS_FOLLOWER, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 11000, 13000, 'Naxxramas - Faerlina - Naxxramas Follower');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect1', 54098, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 3800, 4200, 'Naxxramas - Faerlina - Poison Bolt Volley'),
('Effect0', 54099, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 8000, 9000, 'Naxxramas - Faerlina - Rain of Fire'),
('Effect0', 54096, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 7000, 7700, 'Naxxramas - Faerlina - Naxxramas Worshipper - Firewall');

-- Maexxna
SET @BOSS_MAEXXNA := 15952;
SET @NPC_WEB_WRAP := 16486;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_MAEXXNA, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 16400000, 16400000, 'Naxxramas - Maexxna'),
('MainhandDamage', @BOSS_MAEXXNA, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Maexxna'),
('Health', @NPC_WEB_WRAP, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 40000, 40000, 'Naxxramas - Maexxna - Web Wrap');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 54122, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 12500, 13500, 'Naxxramas - Maexxna - Poison Shock');

-- Razuvious
SET @BOSS_RAZUVIOUS := 16061;
SET @NPC_DEATH_KNIGHT_UNDERSTUDY := 16803;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_RAZUVIOUS, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 15100000, 15100000, 'Naxxramas - Razuvious'),
('MainhandDamage', @BOSS_RAZUVIOUS, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 85000, 95000, 'Naxxramas - Razuvious'),
('Health', @NPC_DEATH_KNIGHT_UNDERSTUDY, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 607000, 607000, 'Naxxramas - Razuvious - Death Knight Understudy'),
('MainhandDamage', @NPC_DEATH_KNIGHT_UNDERSTUDY, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 11000, 13000, 'Naxxramas - Razuvious - Death Knight Understudy');

-- Gothik
SET @BOSS_GOTHIK := 16060;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_GOTHIK, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4500000, 4500000, 'Naxxramas - Gothik');

-- Four Horsemen
SET @BOSS_KORTHAZZ := 16064;
SET @BOSS_BLAUMEUX := 16065;
SET @BOSS_MOGRAINE := 30549;
SET @BOSS_ZELIEK := 16063;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_KORTHAZZ, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4800000, 4800000, 'Naxxramas - Korthazz'),
('MainhandDamage', @BOSS_KORTHAZZ, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Korthazz'),
('Health', @BOSS_BLAUMEUX, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4800000, 4800000, 'Naxxramas - Blaumeux'),
('MainhandDamage', @BOSS_BLAUMEUX, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Blaumeux'),
('Health', @BOSS_MOGRAINE, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4800000, 4800000, 'Naxxramas - Mograine'),
('MainhandDamage', @BOSS_MOGRAINE, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Mograine'),
('Health', @BOSS_ZELIEK, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4800000, 4800000, 'Naxxramas - Zeliek'),
('MainhandDamage', @BOSS_ZELIEK, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Zeliek');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 57467, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 98000, 102000, 'Naxxramas - Korthazz - Meteor'),
('Effect0', 57369, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 9500, 10500, 'Naxxramas - Mograine - Unholy Shadow'),
('Effect1', 57369, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 1000, 1000, 'Naxxramas - Mograine - Unholy Shadow'),
('AuraDuration', 57369, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 12000, 12000, 'Naxxramas - Mograine - Unholy Shadow'),
('Effect0', 57464, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 7800, 8200, 'Naxxramas - Blaumeux - Shadow Bolt'),
('Effect0', 28865, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 7800, 8200, 'Naxxramas - Blaumeux - Consumption'),
('Effect0', 57466, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4800, 5200, 'Naxxramas - Zeliek - Holy Wrath');

-- Noth
SET @BOSS_NOTH := 15954;
SET @NPC_PLAGUED_WARRIOR  := 16984;
SET @NPC_PLAGUED_CHAMPION := 16983;
SET @NPC_PLAGUED_GUARDIAN := 16981;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_NOTH, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 16500000, 16500000, 'Naxxramas - Noth'),
('MainhandDamage', @BOSS_NOTH, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Noth'),
('Health', @NPC_PLAGUED_WARRIOR, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 313000, 313000, 'Naxxramas - Noth - Plagued Warrior'),
('MainhandDamage', @NPC_PLAGUED_WARRIOR, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 12000, 14000, 'Naxxramas - Noth - Plagued Warrior'),
('Health', @NPC_PLAGUED_CHAMPION, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 313000, 313000, 'Naxxramas - Noth - Plagued Champion'),
('MainhandDamage', @NPC_PLAGUED_CHAMPION, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 13000, 15000, 'Naxxramas - Noth - Plagued Champion'),
('Health', @NPC_PLAGUED_GUARDIAN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 252000, 252000, 'Naxxramas - Noth - Plagued Guardian'),
('MainhandDamage', @NPC_PLAGUED_GUARDIAN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 10000, 11000, 'Naxxramas - Noth - Plagued Guardian');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 54889, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4600, 5400, 'Naxxramas - Noth - Plagued Champion - Shadow Shock'),
('Effect0', 54891, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 19000, 21000, 'Naxxramas - Noth - Plagued Guardian - Arcane Explosion');

-- Heigan
SET @BOSS_HEIGAN := 15936;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_HEIGAN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 18300000, 18300000, 'Naxxramas - Heigan'),
('MainhandDamage', @BOSS_HEIGAN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 24000, 30000, 'Naxxramas - Heigan');

-- Loatheb
SET @BOSS_LOATHEB := 16011;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_LOATHEB, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 34000000, 34000000, 'Naxxramas - Loatheb'),
('MainhandDamage', @BOSS_LOATHEB, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 18000, 22000, 'Naxxramas - Loatheb');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 55601, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 4600, 5400, 'Naxxramas - Loatheb - Deathbloom');

-- Sapphiron
SET @BOSS_SAPPHIRON := 15989;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_SAPPHIRON, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 33100000, 33100000, 'Naxxramas - Sapphiron'),
('MainhandDamage', @BOSS_SAPPHIRON, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 34000, 38000, 'Naxxramas - Sapphiron');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 55799, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 6700, 7300, 'Naxxramas - Sapphiron - Frost Aura'),
('Effect1', 28522, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 9500, 10500, 'Naxxramas - Sapphiron - Icebolt');
 
-- Kel'thuzad
SET @BOSS_KELTHUZAD := 15990;
SET @NPC_GUARDIAN_OF_ICECROWN := 16441;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_KELTHUZAD, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 31440000, 31440000, 'Naxxramas - Kelthuzad'),
('MainhandDamage', @NPC_GUARDIAN_OF_ICECROWN, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 150000, 150000, 'Naxxramas - Kelthuzad - Guardian of Icecrown');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 55802, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 58000, 62000, 'Naxxramas - Kelthuzad - Frostbolt'),
('Effect0', 55807, @MAPID_NAXXRAMAS, @DIFFICULTY_25_NORMAL, 12000, 17000, 'Naxxramas - Kelthuzad - Frostbolt Volley');

/*

	OBSIDIAN SANCTUM (25)

*/
SET @MAPID_OBSIDIAN_SANCTUM := 615;
DELETE FROM `boost_stat_values` WHERE `MapID`=@MAPID_OBSIDIAN_SANCTUM AND `Difficulty`=@DIFFICULTY_25_NORMAL;
DELETE FROM `boost_spell_values` WHERE `MapID`=@MAPID_OBSIDIAN_SANCTUM AND `Difficulty`=@DIFFICULTY_25_NORMAL;

-- Sartharion
SET @BOSS_SARTHARION := 28860;
SET @NPC_TENEBRON := 30452;
SET @NPC_SHADRON := 30451;
SET @NPC_VESPERON := 30449;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_SARTHARION, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_25_NORMAL, 12700000, 12700000, 'Obsidian Sanctum - Sartharion'),
('Health', @NPC_TENEBRON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_25_NORMAL, 3570000, 3570000, 'Obsidian Sanctum - Tenebron'),
('Health', @NPC_SHADRON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_25_NORMAL, 3570000, 3570000, 'Obsidian Sanctum - Shadron'),
('Health', @NPC_VESPERON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_25_NORMAL, 3570000, 3570000, 'Obsidian Sanctum - Vesperon');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('AuraDuration', 56910, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_25_NORMAL, 5000, 5000, 'Obsidian Sanctum - Sartharion - Tail Lash'),
('Effect0', 57581, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_25_NORMAL, 148000, 152000, 'Obsidian Sanctum - Sartharion - Shadow Fissure');

/*

	EYE OF ETERNITY (25)

*/
SET @MAPID_EYE_OF_ETERNITY := 616;
DELETE FROM `boost_stat_values` WHERE `MapID`=@MAPID_EYE_OF_ETERNITY AND `Difficulty`=@DIFFICULTY_25_NORMAL;
DELETE FROM `boost_spell_values` WHERE `MapID`=@MAPID_EYE_OF_ETERNITY AND `Difficulty`=@DIFFICULTY_25_NORMAL;

-- Malygos
SET @BOSS_MALYGOS := 28859;
SET @NPC_SCION_OF_ETERNITY := 30249;
SET @NPC_NEXUS_LORD := 30245;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Health', @BOSS_MALYGOS, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_25_NORMAL, 33300000, 33300000, 'Eye of Eternity - Malygos'),
('Health', @NPC_SCION_OF_ETERNITY, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_25_NORMAL, 423000, 423000, 'Eye of Eternity - Malygos - Scion of Eternity'),
('Health', @NPC_NEXUS_LORD, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_25_NORMAL, 793000, 793000, 'Eye of Eternity - Malygos - Nexus Lord');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0', 57432, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_25_NORMAL, 100000, 110000, 'Eye of Eternity - Malygos - Arcane Pulse');

/*

	NAXXRAMAS (10)

*/

-- Patchwerk
SET @BOSS_PATCHWERK := 16028;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_PATCHWERK, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Patchwerk'),
('MainhandDamageModifier', @BOSS_PATCHWERK, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Patchwerk');

INSERT INTO `boost_spell_values` (`SpellType`, `SpellID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Effect0Modifier', 41926, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Patchwerk - Hateful Strike');

-- Grobbulus
SET @BOSS_GROBBULUS := 15931;
SET @NPC_FALLOUT_SLIME := 16290;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_GROBBULUS, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Grobbulus'),
('MainhandDamageModifier', @BOSS_GROBBULUS, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Grobbulus'),
('HealthModifier', @NPC_FALLOUT_SLIME, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 30, 30, 'Naxxramas - Grobbulus - Fallout Slime');

-- Gluth
SET @BOSS_GLUTH := 15932;
SET @NPC_ZOMBIE_CHOW := 16360;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_GLUTH, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Gluth'),
('MainhandDamageModifier', @BOSS_GLUTH, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Gluth'),
('HealthModifier', @NPC_ZOMBIE_CHOW, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 30, 30, 'Naxxramas - Gluth - Zombie Chow');

-- Thaddius
SET @BOSS_THADDIUS := 15928;
SET @NPC_FEUGEN := 15930;
SET @NPC_STALAGG := 15929;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_THADDIUS, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Thaddius'),
('MainhandDamageModifier', @BOSS_THADDIUS, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Thaddius'),
('HealthModifier', @NPC_FEUGEN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Thaddius - Feugen'),
('MainhandDamageModifier', @NPC_FEUGEN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Thaddius - Feugen'),
('HealthModifier', @NPC_STALAGG, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Thaddius - Stalagg'),
('MainhandDamageModifier', @NPC_STALAGG, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Thaddius - Stalagg');

-- Anub'rekhan
SET @BOSS_ANUBREKHAN := 15956;
SET @NPC_CRYPT_GUARD := 16573;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_ANUBREKHAN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Anubrekhan'),
('MainhandDamageModifier', @BOSS_ANUBREKHAN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Anubrekhan'),
('HealthModifier', @NPC_CRYPT_GUARD, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Anubrekhan - Crypt Guard'),
('MainhandDamageModifier', @NPC_CRYPT_GUARD, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Anubrekhan - Crypt Guard');

-- Faerlina
SET @BOSS_FAERLINA := 15953;
SET @NPC_NAXXRAMAS_WORSHIPPER := 16506;
SET @NPC_NAXXRAMAS_FOLLOWER := 16505;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_FAERLINA, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Faerlina'),
('MainhandDamageModifier', @BOSS_FAERLINA, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Faerlina'),
('HealthModifier', @NPC_NAXXRAMAS_WORSHIPPER, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Faerlina - Naxxramas Worshipper'),
('MainhandDamageModifier', @NPC_NAXXRAMAS_WORSHIPPER, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Faerlina - Naxxramas Worshipper');

-- Maexxna
SET @BOSS_MAEXXNA := 15952;
SET @NPC_WEB_WRAP := 16486;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_MAEXXNA, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Maexxna'),
('MainhandDamageModifier', @BOSS_MAEXXNA, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Maexxna'),
('HealthModifier', @NPC_WEB_WRAP, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Maexxna - Web Wrap');

-- Razuvious
SET @BOSS_RAZUVIOUS := 16061;
SET @NPC_DEATH_KNIGHT_UNDERSTUDY := 16803;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_RAZUVIOUS, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Razuvious'),
('MainhandDamageModifier', @BOSS_RAZUVIOUS, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Razuvious'),
('HealthModifier', @NPC_DEATH_KNIGHT_UNDERSTUDY, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Razuvious - Death Knight Understudy'),
('MainhandDamageModifier', @NPC_DEATH_KNIGHT_UNDERSTUDY, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Razuvious - Death Knight Understudy');

-- Gothik
SET @BOSS_GOTHIK := 16060;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_GOTHIK, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Gothik'),
('MainhandDamageModifier', @BOSS_GOTHIK, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Gothik');

-- Four Horsemen
SET @BOSS_KORTHAZZ := 16064;
SET @BOSS_BLAUMEUX := 16065;
SET @BOSS_MOGRAINE := 30549;
SET @BOSS_ZELIEK := 16063;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_KORTHAZZ, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Korthazz'),
('MainhandDamageModifier', @BOSS_KORTHAZZ, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Korthazz'),
('HealthModifier', @BOSS_BLAUMEUX, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Blaumeux'),
('MainhandDamageModifier', @BOSS_BLAUMEUX, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Blaumeux'),
('HealthModifier', @BOSS_MOGRAINE, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Mograine'),
('MainhandDamageModifier', @BOSS_MOGRAINE, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Mograine'),
('HealthModifier', @BOSS_ZELIEK, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Zeliek'),
('MainhandDamageModifier', @BOSS_ZELIEK, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Zeliek');

-- Noth
SET @BOSS_NOTH := 15954;
SET @NPC_PLAGUED_WARRIOR  := 16984;
SET @NPC_PLAGUED_CHAMPION := 16983;
SET @NPC_PLAGUED_GUARDIAN := 16981;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_NOTH, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Noth'),
('MainhandDamageModifier', @BOSS_NOTH, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Noth'),
('HealthModifier', @NPC_PLAGUED_WARRIOR, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Noth - Plagued Warrior'),
('MainhandDamageModifier', @NPC_PLAGUED_WARRIOR, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Noth - Plagued Warrior'),
('HealthModifier', @NPC_PLAGUED_CHAMPION, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Noth - Plagued Champion'),
('MainhandDamageModifier', @NPC_PLAGUED_CHAMPION, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Noth - Plagued Champion'),
('HealthModifier', @NPC_PLAGUED_GUARDIAN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Noth - Plagued Guardian'),
('MainhandDamageModifier', @NPC_PLAGUED_GUARDIAN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Noth - Plagued Guardian');

-- Heigan
SET @BOSS_HEIGAN := 15936;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_HEIGAN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Heigan'),
('MainhandDamageModifier', @BOSS_HEIGAN, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Heigan');

-- Loatheb
SET @BOSS_LOATHEB := 16011;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_LOATHEB, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Loatheb'),
('MainhandDamageModifier', @BOSS_LOATHEB, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 60, 60, 'Naxxramas - Loatheb');

-- Sapphiron
SET @BOSS_SAPPHIRON := 15989;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_SAPPHIRON, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 35, 35, 'Naxxramas - Sapphiron'),
('MainhandDamageModifier', @BOSS_SAPPHIRON, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Sapphiron');

-- Kel'thuzad
SET @BOSS_KELTHUZAD := 15990;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_KELTHUZAD, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 30, 30, 'Naxxramas - Kelthuzad'),
('MainhandDamageModifier', @BOSS_KELTHUZAD, @MAPID_NAXXRAMAS, @DIFFICULTY_10_NORMAL, 70, 70, 'Naxxramas - Kelthuzad');

/*

	OBSIDIAN SANCTUM (10)

*/
DELETE FROM `boost_stat_values` WHERE `MapID`=@MAPID_OBSIDIAN_SANCTUM AND `Difficulty`=@DIFFICULTY_10_NORMAL;
DELETE FROM `boost_spell_values` WHERE `MapID`=@MAPID_OBSIDIAN_SANCTUM AND `Difficulty`=@DIFFICULTY_10_NORMAL;

-- Sartharion
SET @BOSS_SARTHARION := 28860;
SET @NPC_TENEBRON := 30452;
SET @NPC_SHADRON := 30451;
SET @NPC_VESPERON := 30449;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_SARTHARION, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 35, 35, 'Obsidian Sanctum - Sartharion'),
('HealthModifier', @NPC_TENEBRON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 35, 35, 'Obsidian Sanctum - Tenebron'),
('HealthModifier', @NPC_SHADRON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 35, 35, 'Obsidian Sanctum - Shadron'),
('HealthModifier', @NPC_VESPERON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 35, 35, 'Obsidian Sanctum - Vesperon'),
('MainhandDamageModifier', @BOSS_SARTHARION, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 70, 70, 'Obsidian Sanctum - Sartharion'),
('MainhandDamageModifier', @NPC_TENEBRON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 70, 70, 'Obsidian Sanctum - Tenebron'),
('MainhandDamageModifier', @NPC_SHADRON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 70, 70, 'Obsidian Sanctum - Shadron'),
('MainhandDamageModifier', @NPC_VESPERON, @MAPID_OBSIDIAN_SANCTUM, @DIFFICULTY_10_NORMAL, 70, 70, 'Obsidian Sanctum - Vesperon');

/*

	EYE OF ETERNITY (10)

*/
DELETE FROM `boost_stat_values` WHERE `MapID`=@MAPID_EYE_OF_ETERNITY AND `Difficulty`=@DIFFICULTY_10_NORMAL;
DELETE FROM `boost_spell_values` WHERE `MapID`=@MAPID_EYE_OF_ETERNITY AND `Difficulty`=@DIFFICULTY_10_NORMAL;

-- Malygos
SET @BOSS_MALYGOS := 28859;
SET @NPC_SCION_OF_ETERNITY := 30249;
SET @NPC_NEXUS_LORD := 30245;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('HealthModifier', @BOSS_MALYGOS, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_10_NORMAL, 35, 35, 'Eye of Eternity - Malygos'),
('HealthModifier', @NPC_SCION_OF_ETERNITY, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_10_NORMAL, 35, 35, 'Eye of Eternity - Malygos - Scion of Eternity'),
('HealthModifier', @NPC_NEXUS_LORD, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_10_NORMAL, 35, 35, 'Eye of Eternity - Malygos - Nexus Lord'),
('MainhandDamageModifier', @BOSS_MALYGOS, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_10_NORMAL, 70, 70, 'Eye of Eternity - Malygos'),
('MainhandDamageModifier', @NPC_SCION_OF_ETERNITY, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_10_NORMAL, 70, 70, 'Eye of Eternity - Malygos - Scion of Eternity'),
('MainhandDamageModifier', @NPC_NEXUS_LORD, @MAPID_EYE_OF_ETERNITY, @DIFFICULTY_10_NORMAL, 70, 70, 'Eye of Eternity - Malygos - Nexus Lord');

