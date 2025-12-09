CREATE TABLE `log_gamemaster` (
	`AccountID` INT(11) NOT NULL,
	`GMName` VARCHAR(30) NOT NULL,
	`IP` VARCHAR(30) NOT NULL,
	`log_message` VARCHAR(500) NOT NULL,
	`date` DATETIME NOT NULL,
	PRIMARY KEY (`AccountID`, `log_message`, `date`)
);
