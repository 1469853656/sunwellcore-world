-- DB/Loot: Remove loot from creature Water Terror https://github.com/TrinityCore/TrinityCore/commit/abd8140681c007a110a78ccb97a5677816bb94ba
-- DB/Creature: Remove static spawns of Arcane Fiend & Amani Bear Mount 
-- DB/Creature: Bogblossom Bunny - Bogblossom Bunny SAI (Source: https://www.youtube.com/watch?v=EIFhKHtvOe0)  https://github.com/TrinityCore/TrinityCore/commit/ea05844c59e77358c6ab1978a914e90e4b8dd21d
-- DB/Spell: Fix rampart of skulls and upper spire teleport orientation https://github.com/TrinityCore/TrinityCore/commit/73d55f6968a3f9510d4b0d8fbafc4480652173bd 
-- DB/Loot: Remove item Carved Ivory Bone from creatures loot https://github.com/TrinityCore/TrinityCore/commit/7b4b04653be041e53c51121a0cf80b33a5fd4de5
DELETE FROM `creature_loot_template` WHERE `entry` = 30633;
UPDATE `creature_template` SET `lootid` = 0 WHERE `entry` = 30633;

DELETE FROM `creature` WHERE `id` = 18429 AND `guid` IN (91245,91246);
DELETE FROM `creature` WHERE `id` = 24217 AND `guid` IN (89266,89267,89272,89275);
DELETE FROM `linked_respawn` WHERE `guid` IN (91245,91246,89266,89267,89272,89275);

SET @ENTRY := 23104;
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid` = @ENTRY AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,54,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Bogblossom Bunny - On Just Summoned - Say Line 0"),
(@ENTRY,0,1,0,61,0,100,0,0,0,0,0,11,40532,0,0,0,0,0,1,0,0,0,0,0,0,0,"Bogblossom Bunny - On Link - Cast 'Bogblossom Knockback'");

DELETE FROM `creature_text` WHERE `entry` = 23104;
INSERT INTO `creature_text` (`entry`,`GroupID`,`ID`,`Text`,`Type`,`Language`,`Probability`,`Emote`,`Duration`,`Sound`,`TextRange`,`comment`) VALUES 
(23104,0,0,"The bogblossom explodes, spraying pollen wildly.",16,0,100,0,0,0,0,'Bogblossom Bunny');

UPDATE `spell_target_position` SET `target_orientation`=6.25 WHERE `id` IN (70859,70857);

DELETE FROM `creature_loot_template` WHERE `Item` = 24281 AND `mincountOrRef` > 0;