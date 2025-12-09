SET @CGUID = 303346;
SET @NPC = 220000;

DELETE FROM `creature` WHERE `guid` = @CGUID;
INSERT INTO `creature` VALUES (@CGUID + 0, @NPC, 571, 1, 1, 0, 0, 5752.17, 586.91, 614.96, 0.78, 300, 0, 0, 1, 0, 0, 0, 0, 0);

DELETE FROM `creature_template` WHERE `entry` = @NPC;
INSERT INTO `creature_template` VALUES (@NPC,0,0,0,0,0,29077,0,0,0,'Brom Gayserstick','','',0,70,70,1,35,128,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'',12340);

DELETE FROM `npc_vendor` WHERE `entry` = @NPC;
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2570 FROM item_template WHERE name LIKE 'Hateful%' AND class = 2  AND ItemLevel = 200 AND InventoryType = 17; -- Add 2h hateful hateful for 50k honor
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2257 FROM item_template WHERE name LIKE 'Hateful%' AND class = 2  AND ItemLevel = 200 AND InventoryType != 17 AND entry != 42236; -- Add 1h hateful weapons for 25k honor
INSERT INTO `npc_vendor` VALUES -- shields, offhands
(@NPC, 0, 42536, 0, 0, 2257),
(@NPC, 0, 42530, 0, 0, 2257),
(@NPC, 0, 42524, 0, 0, 2257),
(@NPC, 0, 42558, 0, 0, 2257),
(@NPC, 0, 42569, 0, 0, 2257),
(@NPC, 0, 42563, 0, 0, 2257);

 -- Arena spectator NPC
SET @NPC = 210000;
DELETE FROM `creature` WHERE `guid` = @CGUID + 1;
INSERT INTO `creature` VALUES (@CGUID + 1, @NPC, 571, 1, 1, 0, 0, 5795.25, 591.01, 609.15, 2.58, 300, 0, 0, 1, 0, 0, 0, 0, 0);

-- Arena battlemasters in Durotar and Elwynn Forest
SET @NPC = 29533;
DELETE FROM `creature` WHERE `guid` = @CGUID + 2;
INSERT INTO `creature` VALUES (@CGUID + 2, @NPC, 1, 1, 1, 0, 0, 1334.55, -4397.02, 28.01, 1.72, 300, 0, 0, 1, 0, 0, 0, 0, 0);

SET @NPC = 29533;
DELETE FROM `creature` WHERE `guid` = @CGUID + 3;
INSERT INTO `creature` VALUES (@CGUID + 3, @NPC, 0, 1, 1, 0, 0, -9109.79, 387.39, 92.53, 2.38, 300, 0, 0, 1, 0, 0, 0, 0, 0);
