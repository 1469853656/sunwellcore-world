-- DB/Vendor: Transmog - change token price to 28 conquest
UPDATE `npc_vendor` SET `ExtendedCost` = 2607 WHERE `item` = 37742 AND `entry` = 80003 AND `ExtendedCost` = 2540;
UPDATE `npc_vendor` SET `ExtendedCost` = 2607 WHERE `item` = 37711 AND `entry` = 80003 AND `ExtendedCost` = 2534;
