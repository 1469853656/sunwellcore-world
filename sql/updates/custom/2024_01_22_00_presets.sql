DROP TABLE IF EXISTS `preset_groups`;
CREATE TABLE `preset_groups` (
	`GroupID` INT(10) UNSIGNED NOT NULL,
	`GroupName` VARCHAR(50) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci',
	PRIMARY KEY (`GroupID`, `GroupName`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB;

INSERT INTO `preset_groups` (`GroupID`, `GroupName`) VALUES 
(0, 'PreBiS'),
(1, 'PathOfWar'),
(2, 'PoW_S5'),
(3, 'PoW_S6'),
(4, 'PoW_S7'),
(5, 'PoW_S8'),
(6, 'PoW_S8BiS'),
(7, 'T7BiS'),
(8, 'T8BiS'),
(9, 'T9BiS'),
(10, 'T10BiS'),
(11, 'Twink19'),
(12, 'Twink79');
