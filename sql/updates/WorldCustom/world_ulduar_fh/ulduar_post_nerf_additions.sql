DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceEntry` = 63414 AND `SourceGroup` = 2;
INSERT INTO `conditions` VALUES
(13, 2, 63414, 0, 0, 31, 0, 3, 33432, 0, 0, 0, 0, '', 'Mimiron - P3Wx2 Laser Barrage');

DELETE FROM `spell_script_names` WHERE `spell_id` = 66490;
INSERT INTO `spell_script_names` VALUES
(66490, "spell_spinning_up_root_base");
