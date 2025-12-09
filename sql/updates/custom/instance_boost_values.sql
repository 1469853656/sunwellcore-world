CREATE TABLE `instance_boost_values` (
	`InstanceID` INT(11) NOT NULL,
	`BossID` INT(11) NOT NULL,
	`BoostID` INT(11) NOT NULL,
	`Value1` INT(11) NOT NULL DEFAULT '0',
	`Value2` INT(11) NOT NULL DEFAULT '0',
	`Comment` VARCHAR(100) NOT NULL DEFAULT '',
	PRIMARY KEY (`InstanceID`, `BossID`, `BoostID`)
);
