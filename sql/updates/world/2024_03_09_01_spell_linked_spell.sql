DELETE FROM `spell_linked_spell` WHERE `spell_trigger`=54581;
DELETE FROM `spell_script_names` WHERE `spell_id`=54581;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(54581, "spell_mammoth_explosion_spell_spawner");
DELETE FROM `smart_scripts` WHERE `entryorguid`=29524 AND `source_type`=0;
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_mammoth_meat_bunny" WHERE `entry`=29524;
DELETE FROM `smart_scripts` WHERE `entryorguid`=29402 AND `source_type`=0;
UPDATE `creature_template` SET `AIName`="", `ScriptName`="npc_ironwool_mammoth" WHERE `entry`=29402;
