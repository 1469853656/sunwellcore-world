-- Felguard
SET @CREATURE_ENTRY := 110010;
UPDATE `creature_template` SET `mindmg`=2000, `maxdmg`=3000, `dmg_multiplier`=1 WHERE `entry`=@CREATURE_ENTRY;
DELETE FROM `creature_equip_template` WHERE `entry`=@CREATURE_ENTRY;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`, `VerifiedBuild`) VALUES 
(@CREATURE_ENTRY, 1, 22199, 0, 0, 18019);
-- Demon Hunter
UPDATE `creature_template` SET `modelid1`=20122 WHERE `entry`=110005;
UPDATE `creature_template` SET `modelid1`=20130 WHERE `entry`=110000;
-- Acid-Drenched Mandibles
SET @SPELL_ENTRY := 65775;
DELETE FROM `spell_script_names` WHERE `spell_id`=@SPELL_ENTRY;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_anubarak_acid_drenched_mandibles");
