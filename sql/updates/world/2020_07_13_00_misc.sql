-- DB/Vendor: Coreiel - restore two "exploit" related items. They can not be sold or disenchanted
DELETE FROM `npc_vendor` WHERE `entry` = 21474 AND `item` IN (30570,30568);
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES 
(21474, 0, 30568, 3, 1800, 0),
(21474, 0, 30570, 3, 1800, 0);

UPDATE `item_template` SET `Flags` = `Flags` | 32768, `SellPrice` = 0, `DisenchantID` = 0 WHERE `entry` IN (30570,30568); 