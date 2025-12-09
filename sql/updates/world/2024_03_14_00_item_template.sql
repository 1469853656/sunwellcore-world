DELETE FROM `item_template` WHERE `entry`=69903;
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `ItemLevel`, `bonding`, `description`, `Material`) VALUES
(69903,15,0,'Satchel of Exotic Mysteries',39459,7,134217732,139264,1,0,85,1,'The satchel trembles with the weight of potential...',4);

DELETE FROM `command` WHERE `name`='cta';
INSERT INTO `command` VALUES
('cta',0,'Syntax: .cta on|off\nEnables/Disables displaying Call To Arms messages');
