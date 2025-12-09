-- Felguard
UPDATE `creature_template` SET `health_mod`=87 WHERE `entry`=110010;
-- Spirit Wolf
UPDATE `creature_template` SET `health_mod`=5.2 WHERE `entry`=110013;
-- Elemental Shaman (Horde)
UPDATE `creature_template` SET `modelid1`=29788 WHERE `entry`=110001;
-- Elemental Shaman (Alliance)
UPDATE `creature_template` SET `modelid1`=29773 WHERE `entry`=110006;
-- Feral Druid (Horde)
UPDATE `creature_template` SET `modelid1`=1058 WHERE `entry`=110002;
-- Acid-Drenched Mandibles
SET @SPELL_ENTRY := 67863;
DELETE FROM `spell_script_names` WHERE `spell_id` IN (@SPELL_ENTRY, 65775);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(@SPELL_ENTRY, "spell_anubarak_acid_drenched_mandibles");
