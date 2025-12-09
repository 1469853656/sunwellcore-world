UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_alarmed_blightguard", `flags_extra`=`flags_extra`|0x20000000, `speed_walk`=1.6 WHERE `entry`=28745;
DELETE FROM `creature` WHERE `id`=28745;
DELETE FROM `spell_script_names` WHERE `spell_id`=52227;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(52227, "spell_dilute_blight_cauldron");
