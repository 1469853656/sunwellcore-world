
SET @KEYSTONE_ENTRY:= 302000;
SET @DALARAN_QUESTGIVER:= 302050;
DELETE FROM `gameobject_template` WHERE `entry` IN (@KEYSTONE_ENTRY, @DALARAN_QUESTGIVER);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES
(@KEYSTONE_ENTRY, 2, 235, 'Mythic Obelisk', '', '', '', 35, 0, 0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'go_mythic_crystal', 12340),
(@DALARAN_QUESTGIVER, 2, 235, 'Mythic Obelisk', '', '', '', 35, 0, 0.75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 12340);

DELETE FROM `npc_text` WHERE `Id` IN (91526, 91527, 91528, 91529, 91531);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(91526, "Once player finishes the quest, the same player will be able to set specific level of Mythic dungeon."),
(91527, "Set the level of Mythic dungeon. Once the level is set it cannot be changed."),

(91528, "Hello $n!
My name is Willson and I'm able to convert your group to Mythic difficulty!
Once it's created you can visit selected dungeons which are way harder compared to their well-known condition. Just say a word and you can start even now! If you change your mind and don't want to be in Mythic difficulty group anymore you have to recreate your party."),
(91529, "Fair enough! Your group is prepared to face below dungeons on Mythic state. I may teleport you to the entrance of instance you would like to encounter."),
-- 91530 zajete
(91531, "Hello $n! I can show you some magic tricks but they work only if you are the party leader!");

SET @DALARAN_NPC_ENTRY:= 91709;
DELETE FROM `creature_template` WHERE `entry` = @DALARAN_NPC_ENTRY;
INSERT INTO `creature_template` VALUES
(@DALARAN_NPC_ENTRY, 0, 0, 0, 0, 0, 21504, 0, 0, 0, 'Willson Bubbletea', NULL, NULL, 0, 83, 83, 0, 35, 0, 1, 1.14286, 4.5, 0, 94, 125, 0, 228, 1, 2000, 2000, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 65, 95, 23, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'PassiveAI', 0, 3, 1, 10, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 'npc_mythic_handler', 0, 12340);

SET @BUBBLETEA_GUID:= 310008;
DELETE FROM `creature` WHERE `guid` = @BUBBLETEA_GUID;
INSERT INTO `creature` VALUES 
(@BUBBLETEA_GUID, 91709, 571, 1, 1, 0, 0, 5733.2, 509.51, 647.453, 1.64, 300, 0, 0, 58080, 0, 0, 0, 0, 0);

SET @OBELISK_QUESTGIVER_GUID:= 300089;
DELETE FROM `gameobject` WHERE `guid` = @OBELISK_QUESTGIVER_GUID;
INSERT INTO `gameobject` VALUES 
(@OBELISK_QUESTGIVER_GUID, 302050, 571, 1, 1, 5731.95, 518.29, 647.452, 5.16407, -0, -0, -0.530812, 0.84749, 300, 0, 1, 0);


DELETE FROM `command` WHERE `name` = "mythic";
INSERT INTO `command` VALUES ("mythic", 0, "");

SET @MYTHIC_QUEST_ID:= 35000;
SET @MYTHIC_KEY_ID:= 47395;

DELETE FROM `quest_template` WHERE `Id` = @MYTHIC_QUEST_ID;
INSERT INTO `quest_template` (`Id`, `Method`, `Level`, `MinLevel`, `MaxLevel`, `ZoneOrSort`, `Type`, `SuggestedPlayers`, `LimitTime`, `RequiredClasses`, `RequiredRaces`, `RequiredSkillId`, `RequiredSkillPoints`, `RequiredFactionId1`, `RequiredFactionId2`, `RequiredFactionValue1`, `RequiredFactionValue2`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestIdChain`, `RewardXPId`, `RewardOrRequiredMoney`, `RewardMoneyMaxLevel`, `RewardSpell`, `RewardSpellCast`, `RewardHonor`, `RewardHonorMultiplier`, `RewardMailTemplateId`, `RewardMailDelay`, `SourceItemId`, `SourceItemCount`, `SourceSpellId`, `Flags`, `SpecialFlags`, `RewardTitleId`, `RequiredPlayerKills`, `RewardTalents`, `RewardArenaPoints`, `RewardItemId1`, `RewardItemId2`, `RewardItemId3`, `RewardItemId4`, `RewardItemCount1`, `RewardItemCount2`, `RewardItemCount3`, `RewardItemCount4`, `RewardChoiceItemId1`, `RewardChoiceItemId2`, `RewardChoiceItemId3`, `RewardChoiceItemId4`, `RewardChoiceItemId5`, `RewardChoiceItemId6`, `RewardChoiceItemCount1`, `RewardChoiceItemCount2`, `RewardChoiceItemCount3`, `RewardChoiceItemCount4`, `RewardChoiceItemCount5`, `RewardChoiceItemCount6`, `RewardFactionId1`, `RewardFactionId2`, `RewardFactionId3`, `RewardFactionId4`, `RewardFactionId5`, `RewardFactionValueId1`, `RewardFactionValueId2`, `RewardFactionValueId3`, `RewardFactionValueId4`, `RewardFactionValueId5`, `RewardFactionValueIdOverride1`, `RewardFactionValueIdOverride2`, `RewardFactionValueIdOverride3`, `RewardFactionValueIdOverride4`, `RewardFactionValueIdOverride5`, `PointMapId`, `PointX`, `PointY`, `PointOption`, `Title`, `Objectives`, `Details`, `EndText`, `OfferRewardText`, `RequestItemsText`, `CompletedText`, `RequiredNpcOrGo1`, `RequiredNpcOrGo2`, `RequiredNpcOrGo3`, `RequiredNpcOrGo4`, `RequiredNpcOrGoCount1`, `RequiredNpcOrGoCount2`, `RequiredNpcOrGoCount3`, `RequiredNpcOrGoCount4`, `RequiredSourceItemId1`, `RequiredSourceItemId2`, `RequiredSourceItemId3`, `RequiredSourceItemId4`, `RequiredSourceItemCount1`, `RequiredSourceItemCount2`, `RequiredSourceItemCount3`, `RequiredSourceItemCount4`, `RequiredItemId1`, `RequiredItemId2`, `RequiredItemId3`, `RequiredItemId4`, `RequiredItemId5`, `RequiredItemId6`, `RequiredItemCount1`, `RequiredItemCount2`, `RequiredItemCount3`, `RequiredItemCount4`, `RequiredItemCount5`, `RequiredItemCount6`, `Unknown0`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `EmoteOnIncomplete`, `EmoteOnComplete`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `VerifiedBuild`) VALUES
(@MYTHIC_QUEST_ID, 0, 1, 80, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Start Mythic', '', '', '', 'A few tips for mythic:$B$B Dont suck$B$B Git Gud', NULL, 'Try to complete dungeon in the best possible time within so you will be greatly awarded.', 0, 0, 0, 0, 0, 0, 0, 0, 47395, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12340);

UPDATE `quest_template` SET `ZoneOrSort` = 0, `RequiredItemId1` = @MYTHIC_KEY_ID, `RequiredItemCount1` = 1, `Type` = 83, `RewardXPId` = 0, `RewardOrRequiredMoney` = 0, `RewardMoneyMaxLevel` = 0, `Flags` = 0, `SpecialFlags` = 1, `RewardItemId1` = 0, `RewardItemCount1` = 0,
`Title` = "Mythic start", `RequestItemsText` = "Mythic keys can be obtained via daily quests in Dalaran.", `Objectives` = "Unlock Mythic dungeon.", `Details` = "Unlock Mythic dungeon.", `EndText` = "Unlock Mythic dungeon.", `OfferRewardText` = "Try to complete dungeon in the best possible time within so you will be greatly awarded.", `CompletedText` = "Try to complete dungeon in the best possible time within so you will be greatly awarded.", `RequiredNpcOrGo1` = 0, `RequiredNpcOrGoCount1` = 0 WHERE `Id` = @MYTHIC_QUEST_ID;

DELETE FROM `gameobject_queststarter` WHERE `Id` = 302000;
INSERT INTO `gameobject_queststarter` VALUES
(302000, @MYTHIC_QUEST_ID);

DELETE FROM `gameobject_questender` WHERE `Id` = 302000;
INSERT INTO `gameobject_questender` VALUES
(302000, @MYTHIC_QUEST_ID);

DELETE FROM `item_template` WHERE `entry` = @MYTHIC_KEY_ID;
INSERT INTO `item_template` VALUES
(@MYTHIC_KEY_ID, 12, 0, -1, 'Mythic Key', 20219, 2, 0, 0, 1, 0, 0, 0, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 250, 250, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 1, 'Allows you to enable mythic mode in selected instances.', 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 8192, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 12340);

DELETE FROM `creature_template` WHERE `entry` IN (360001, 360002);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
(360001, 0, 0, 0, 0, 0, 23156, 0, 0, 0, 'Random Dungeon Last Encounter', '', '', 0, 80, 80, 0, 16, 1, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 0, 4, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 9, 138936390, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 9.35979, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, '', 12340),
(360002, 0, 0, 0, 0, 0, 23156, 0, 0, 0, 'Random Battleground Completed', '', '', 0, 80, 80, 0, 16, 1, 1, 1.14286, 2, 0, 0, 0, 0, 0, 1, 2000, 0, 4, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 9, 138936390, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 9.35979, 1, 1, 0, 0, 0, 0, 0, 0, 0, 121, 1, 0, 0, '', 12340);

DELETE FROM `quest_template` WHERE `id` IN (26035, 26036, 26037);
INSERT INTO `quest_template` (`Id`, `Method`, `Level`, `MinLevel`, `MaxLevel`, `ZoneOrSort`, `Type`, `SuggestedPlayers`, `LimitTime`, `RequiredClasses`, `RequiredRaces`, `RequiredSkillId`, `RequiredSkillPoints`, `RequiredFactionId1`, `RequiredFactionId2`, `RequiredFactionValue1`, `RequiredFactionValue2`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestIdChain`, `RewardXPId`, `RewardOrRequiredMoney`, `RewardMoneyMaxLevel`, `RewardSpell`, `RewardSpellCast`, `RewardHonor`, `RewardHonorMultiplier`, `RewardMailTemplateId`, `RewardMailDelay`, `SourceItemId`, `SourceItemCount`, `SourceSpellId`, `Flags`, `SpecialFlags`, `RewardTitleId`, `RequiredPlayerKills`, `RewardTalents`, `RewardArenaPoints`, `RewardItemId1`, `RewardItemId2`, `RewardItemId3`, `RewardItemId4`, `RewardItemCount1`, `RewardItemCount2`, `RewardItemCount3`, `RewardItemCount4`, `RewardChoiceItemId1`, `RewardChoiceItemId2`, `RewardChoiceItemId3`, `RewardChoiceItemId4`, `RewardChoiceItemId5`, `RewardChoiceItemId6`, `RewardChoiceItemCount1`, `RewardChoiceItemCount2`, `RewardChoiceItemCount3`, `RewardChoiceItemCount4`, `RewardChoiceItemCount5`, `RewardChoiceItemCount6`, `RewardFactionId1`, `RewardFactionId2`, `RewardFactionId3`, `RewardFactionId4`, `RewardFactionId5`, `RewardFactionValueId1`, `RewardFactionValueId2`, `RewardFactionValueId3`, `RewardFactionValueId4`, `RewardFactionValueId5`, `RewardFactionValueIdOverride1`, `RewardFactionValueIdOverride2`, `RewardFactionValueIdOverride3`, `RewardFactionValueIdOverride4`, `RewardFactionValueIdOverride5`, `PointMapId`, `PointX`, `PointY`, `PointOption`, `Title`, `Objectives`, `Details`, `EndText`, `OfferRewardText`, `RequestItemsText`, `CompletedText`, `RequiredNpcOrGo1`, `RequiredNpcOrGo2`, `RequiredNpcOrGo3`, `RequiredNpcOrGo4`, `RequiredNpcOrGoCount1`, `RequiredNpcOrGoCount2`, `RequiredNpcOrGoCount3`, `RequiredNpcOrGoCount4`, `RequiredSourceItemId1`, `RequiredSourceItemId2`, `RequiredSourceItemId3`, `RequiredSourceItemId4`, `RequiredSourceItemCount1`, `RequiredSourceItemCount2`, `RequiredSourceItemCount3`, `RequiredSourceItemCount4`, `RequiredItemId1`, `RequiredItemId2`, `RequiredItemId3`, `RequiredItemId4`, `RequiredItemId5`, `RequiredItemId6`, `RequiredItemCount1`, `RequiredItemCount2`, `RequiredItemCount3`, `RequiredItemCount4`, `RequiredItemCount5`, `RequiredItemCount6`, `Unknown0`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `EmoteOnIncomplete`, `EmoteOnComplete`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `VerifiedBuild`) VALUES 
(26035, 2, 80, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 1, 0, 0, 0, 0, 47395, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'The Dungeon', 'Finish three random dungeon instances.', 'Do you remember the excitement of exploring older dungeons for the first time? It was quite exciting! Nearly no one is exploring them now, quite unfortunate. Do me a favor champion, explore two random dungeons like we used to do back in the good old days?', '', 'Wasn\'t it magnificent? Old tales never die my friend...', 'I see you\'re still struggling with this task. I didn\'t change my mind - come back once you\'ve completed it!', NULL, 360001, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'dungeons completed', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12340),
(26036, 2, 80, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 1, 0, 0, 0, 0, 47395, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Kill \'em all!', 'Win two random battlegrounds.', 'I have seen many dreadful battles before my time when I was younger and more in shape... Blood demands blood! If you wish to receive a Mythic Keystone, slay enemies in my name. Conquer one of the famous battlegrounds of your choosing and the reward will be yours... simple, eh?', '', 'Ahh, ha ha ha! That\'s what I was talking about! Splendid, hero, take this Mythic Keystone which you had earned. Come and find me later if you lose it.', 'Still not covered in blood? Come back once you finish the task.', NULL, 360002, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'battlegrounds won', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12340),
(26037,2,80,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41125,47395,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Mythic Companion!','Greetings heroes! At the beginning of this new journey you are able to get a trustworthy assistance! Ronin, the leader of this great city managed to organize extraordinary companions for the defenders of Dalaran. Seek our for him in his chambers!','Speak with Ronin','','We have never thanked you for what have you done for us. You stopped wizard Kel\'Thuzad. You slayed one of the old gods prisoned beneath Northrend\'s grounds. You killed the king of nerubians... and finally you freed us from Arthas\' madness. Please, take this companion as a small gift... as a small gift for all the courages things you\'ve done.','',NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',NULL,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1);

UPDATE `quest_template` SET `flags` = 64, `ZoneOrSort` = 4395 WHERE `id` IN (26035, 26036, 26037);
UPDATE `quest_template` SET `Type` = 41 WHERE `Id` = 26036;

DELETE FROM `gameobject_queststarter` WHERE `Id` = @DALARAN_QUESTGIVER;
INSERT INTO `gameobject_queststarter` VALUES
(@DALARAN_QUESTGIVER, 26035),
(@DALARAN_QUESTGIVER, 26036),
(@DALARAN_QUESTGIVER, 26037);

DELETE FROM `gameobject_questender` WHERE `Id` = @DALARAN_QUESTGIVER;
INSERT INTO `gameobject_questender` VALUES
(@DALARAN_QUESTGIVER, 26035),
(@DALARAN_QUESTGIVER, 26036);

DELETE FROM `creature_questender` WHERE `Id` = 16128 AND `quest` = 26037;
INSERT INTO `creature_questender` VALUES
(16128, 26037);

UPDATE `quest_template` SET `RewardItemId1` = @MYTHIC_KEY_ID, `RewardItemCount1` = 1 WHERE `Id` IN (26035, 26036);

SET @GO_ENTRY:= 302325;
SET @GO_LOOT_ID:= @GO_ENTRY;
SET @GO_REF_LOOT_ID_FOOD:= @GO_LOOT_ID + 1;
SET @GO_REF_LOOT_ID_POTIONS:= @GO_LOOT_ID + 2;
SET @GO_REF_LOOT_ID_FLASKS:= @GO_LOOT_ID + 3;

DELETE FROM `gameobject_template` WHERE `entry` IN (@GO_ENTRY, @GO_ENTRY + 1);
INSERT INTO `gameobject_template` VALUES
(@GO_ENTRY, 3, 9280, 'Mythic Chest', '', '', '', 35, 0, 3, 0, 0, 0, 0, 0, 0, 57, @GO_LOOT_ID, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', 'go_mythic_chest_spawn_in', 12340),
(@GO_ENTRY + 1, 3, 9281, 'Mythic Chest', '', '', '', 35, 0, 3, 0, 0, 0, 0, 0, 0, 57, @GO_LOOT_ID, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', 'go_mythic_chest_spawn_in', 11723);

-- consumbles
DELETE FROM `reference_loot_template` WHERE `entry` IN (@GO_REF_LOOT_ID_POTIONS, @GO_REF_LOOT_ID_FOOD, @GO_REF_LOOT_ID_FLASKS);
INSERT INTO `reference_loot_template` VALUES
-- food min 1 max 5
(@GO_REF_LOOT_ID_FOOD, 43015, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43480, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43478, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43000, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34768, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34769, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42995, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43005, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34748, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34749, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34750, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34751, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34752, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34754, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34755, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34756, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34757, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34758, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34763, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34764, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34765, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34766, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 34767, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42942, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42993, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42994, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42996, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42997, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 42999, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43001, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43004, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43488, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43490, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43491, 0, 1, 1, 1, 5),
(@GO_REF_LOOT_ID_FOOD, 43492, 0, 1, 1, 1, 5),

-- potions
(@GO_REF_LOOT_ID_POTIONS, 40093, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40211, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40212, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 33448, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 33447, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40087, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 22828, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40077, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40214, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40213, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40215, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40216, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 40217, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 13462, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 20008, 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 9172 , 0, 1, 2, 1, 5),
(@GO_REF_LOOT_ID_POTIONS, 5634 , 0, 1, 2, 1, 5),

(@GO_REF_LOOT_ID_FLASKS, 46376, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 46377, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 46378, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 46379, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40072, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40078, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40097, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40109, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44328, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44332, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 39666, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40068, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40070, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40073, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 40076, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44325, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44327, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44329, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44330, 0, 1, 3, 1, 3),
(@GO_REF_LOOT_ID_FLASKS, 44331, 0, 1, 3, 1, 3);

		
DELETE FROM `gameobject_loot_template` WHERE `entry` IN (@GO_LOOT_ID);
INSERT INTO `gameobject_loot_template` VALUES
(@GO_LOOT_ID, @GO_REF_LOOT_ID_FOOD, 100, 1, 1, -@GO_REF_LOOT_ID_FOOD, 2),
(@GO_LOOT_ID, @GO_REF_LOOT_ID_POTIONS, 100, 1, 2, -@GO_REF_LOOT_ID_POTIONS, 1),
(@GO_LOOT_ID, @GO_REF_LOOT_ID_FLASKS, 100, 1, 3, -@GO_REF_LOOT_ID_FLASKS, 1);

DELETE FROM `spell_script_names` WHERE `spell_id` = 35089;
INSERT INTO `spell_script_names` VALUES
(35089, "spell_mythic_companion_summon");

DELETE FROM `item_template` WHERE `entry` = 41125;
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `flagsCustom`, `VerifiedBuild`) VALUES
(41125, 7, 1, -1, 'Companion Whistle', 52742, 5, 0, 0, 1, 10000, 0, 0, -1, -1, 300, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2000, 0, 0, 35089, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 1, 'Use to summon Mythic companion, use again to dismiss it.', 0, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 0, 0, 128, 161, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 15595);

