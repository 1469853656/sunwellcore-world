-- Brom Gayserstick - deadly weapons
SET @NPC = 220000;

DELETE FROM `npc_vendor` WHERE `entry` = @NPC;
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2257 FROM item_template WHERE name LIKE 'Deadly%' AND class = 2 AND itemlevel = 213 AND InventoryType != 17 AND entry != 42237; -- Add 1h deadly weapons for 25k honor
INSERT INTO `npc_vendor` SELECT @NPC, 0, entry, 0, 0, 2570 FROM item_template WHERE name LIKE 'Deadly%' AND class = 2 AND itemlevel = 213 AND InventoryType = 17; -- Add 2h deadly weapons for 50k honor
INSERT INTO `npc_vendor` VALUES -- shields, offhands
(@NPC, 0, 42564, 0, 0, 2257),
(@NPC, 0, 42570, 0, 0, 2257),
(@NPC, 0, 42559, 0, 0, 2257),
(@NPC, 0, 42525, 0, 0, 2257),
(@NPC, 0, 42531, 0, 0, 2257),
(@NPC, 0, 42537, 0, 0, 2257),
(@NPC, 0, 42588, 0, 0, 2257),
(@NPC, 0, 42583, 0, 0, 2257),
(@NPC, 0, 42578, 0, 0, 2257),
(@NPC, 0, 42852, 0, 0, 2257),
(@NPC, 0, 42614, 0, 0, 2257),
(@NPC, 0, 42620, 0, 0, 2257),
(@NPC, 0, 42607, 0, 0, 2257),
(@NPC, 0, 42602, 0, 0, 2257),
(@NPC, 0, 42597, 0, 0, 2257);