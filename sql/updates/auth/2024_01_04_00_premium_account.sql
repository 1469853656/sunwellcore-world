ALTER TABLE `premium_account`
	CHANGE COLUMN `RealmID` `RealmID` INT(10) NOT NULL DEFAULT '-1' AFTER `AccountID`,
	DROP PRIMARY KEY,
	ADD PRIMARY KEY (`AccountID`, `PremiumType`, `RealmID`) USING BTREE;
