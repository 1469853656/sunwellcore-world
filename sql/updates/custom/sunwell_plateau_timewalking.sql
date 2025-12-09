
SET @DIFFICULTY_NORMAL := 0;
SET @MAPID_SUNWELL_PLATEAU := 580;
SET @LEVEL_TRASH := 81;
SET @LEVEL_TRASH_ELITE := 82;
SET @LEVEL_BOSS := 83;

DELETE FROM `boost_stat_values` WHERE `MapID`=@MAPID_SUNWELL_PLATEAU;
SET @NPC_SUNBLADE_PROTECTOR := 25507;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_PROTECTOR, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Protector');

SET @NPC_SUNBLADE_DUSK_PRIEST := 25370;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_DUSK_PRIEST, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Dusk Priest');

SET @NPC_SUNBLADE_SLAYER := 25368;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_SLAYER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Slayer');

SET @NPC_SUNBLADE_DRAGONHAWK := 25367;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_DRAGONHAWK, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Dragonhawk');

SET @NPC_SUNBLADE_VINDICATOR := 25369;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_VINDICATOR, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Vindicator');

SET @NPC_SUNBLADE_DAWN_PRIEST := 25371;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_DAWN_PRIEST, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Dawn Priest');

SET @NPC_SUNBLADE_CABALIST := 25363;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_CABALIST, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Cabalist');

SET @NPC_SUNBLADE_SCOUT := 25372;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_SCOUT, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Scout');

SET @NPC_SUNBLADE_DRAGONHAWK := 25867;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SUNBLADE_DRAGONHAWK, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Sunblade Dragonhawk');

SET @NPC_SHADOWSWORD_SOULBINDER := 25373;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_SOULBINDER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Shadowsword Soulbinder');

SET @NPC_SHADOWSWORD_VANQUISHER := 25486;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_VANQUISHER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Shadowsword Vanquisher');

SET @NPC_SHADOWSWORD_LIFESHAPER := 25506;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_LIFESHAPER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Shadowsword Lifeshaper');

SET @NPC_SHADOWSWORD_MANAFIEND := 25483;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_MANAFIEND, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Shadowsword Manafiend');

SET @NPC_VOLATILE_FIEND := 25851;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_VOLATILE_FIEND, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Volatile Fiend');

SET @NPC_SHADOWSWORD_COMMANDER := 25837;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_COMMANDER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Shadowsword Commander');

SET @NPC_SHADOWSWORD_ASSASSIN := 25484;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_ASSASSIN, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Shadowsword Assassin');

SET @NPC_OBLIVION_MAGE := 25597;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_OBLIVION_MAGE, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Oblivion Mage');

SET @NPC_DOOMFIRE_DESTROYER := 25592;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_DOOMFIRE_DESTROYER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Doomfire Destroyer');

SET @NPC_PAINBRINGER := 25591;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_PAINBRINGER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Painbringer');

SET @NPC_PRIESTESS_OF_TORMENT := 25509;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_PRIESTESS_OF_TORMENT, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Priestess of Torment');

SET @NPC_CHAOS_GAZER := 25595;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_CHAOS_GAZER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Chaos Gazer');

SET @NPC_APOCALYPSE_GUARD := 25593;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_APOCALYPSE_GUARD, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Apocalypse Guard');

SET @NPC_CATACLYSM_HOUND := 25599;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_CATACLYSM_HOUND, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH, @LEVEL_TRASH, 'Sunwell Plateau - Cataclysm Hound');

SET @NPC_SHADOWSWORD_GUARD := 25508;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_SHADOWSWORD_GUARD, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH_ELITE, @LEVEL_TRASH_ELITE, 'Sunwell Plateau - Shadowsword Guard');

SET @NPC_HAND_OF_THE_DECIEVER := 25588;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @NPC_HAND_OF_THE_DECIEVER, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_TRASH_ELITE, @LEVEL_TRASH_ELITE, 'Sunwell Plateau - Hand of the Deciever');

SET @BOSS_KALECGOS := 24850;
SET @BOSS_SARTHOVARR := 24892;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @BOSS_KALECGOS, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, 'Sunwell Plateau - Kalecgos'),

('Level', @BOSS_SARTHOVARR, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, 'Sunwell Plateau - Kalecgos - Sarthovarr');

SET @BOSS_BRUTALLUS := 24882;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @BOSS_BRUTALLUS, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, 'Sunwell Plateau - Brutallus');

SET @BOSS_FELMYST := 25038;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @BOSS_FELMYST, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, 'Sunwell Plateau - Felmyst');

SET @BOSS_LADY_SACROLASH := 25165;
SET @BOSS_GRAND_WARLOCK_ALYTHESS := 25166;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @BOSS_LADY_SACROLASH, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, 'Sunwell Plateau - Lady Sacrolash'),
('Level', @BOSS_GRAND_WARLOCK_ALYTHESS, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, 'Sunwell Plateau - Grand Warlock Alythess');

SET @BOSS_MURU := 25741;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @BOSS_MURU, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, "Sunwell Plateau - M'uru");

SET @BOSS_KILJAEDEN := 25315;
INSERT INTO `boost_stat_values` (`StatType`, `CreatureID`, `MapID`, `Difficulty`, `ValueMin`, `ValueMax`, `Comment`) VALUES 
('Level', @BOSS_KILJAEDEN, @MAPID_SUNWELL_PLATEAU, @DIFFICULTY_NORMAL, @LEVEL_BOSS, @LEVEL_BOSS, "Sunwell Plateau - Kil'jaeden");
