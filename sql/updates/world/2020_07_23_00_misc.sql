-- DB/Spell: Windblossom berries, cooked glossy mightfish, migthfish steak,Grilled Squid,ightfin Soup, Hot Smoked Bass, Poached Sunscale Salmon,  Runn tum tuber surprise, Dirge's Kickin Chimaerok Chops - should not stack with Well Fed buffs
DELETE FROM `spell_group` WHERE `id` = 1001 AND `spell_id` IN (18191,18192,18193,18194,18222,22730,25661);
INSERT INTO `spell_group` VALUES
(1001, 18191, 0), -- stamina Windblossom berries, cooked glossy mightfish, migthfish steak
(1001, 18192, 0), -- agility Grilled Squid
(1001, 18193, 0), -- spirit Hot Smoked Bass
(1001, 18194, 0), -- mr5 Nightfin Soup
(1001, 18222, 0), -- hr5 Poached Sunscale Salmon
(1001, 22730, 0), -- intellect Runn tum tuber surprise
(1001, 25661, 0); -- stamina Dirge's Kickin Chimaerok Chops