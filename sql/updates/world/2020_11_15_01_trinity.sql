-- DB/Creature: Update equipment of Darkmoon Carnie & Sayge https://github.com/TrinityCore/TrinityCore/commit/21777e2f79b6ce1ec19bd2e1f9bd354b9cf93ce6 
DELETE FROM `creature_equip_template` WHERE (`entry`=14849 AND `ID`=2) OR (`entry`=14849 AND `ID`=3);
INSERT INTO `creature_equip_template` (`entry`, `ID`, `Itementry1`, `Itementry2`, `Itementry3`) VALUES
(14849, 2, 3362, 13604, 0), -- Darkmoon Carnie
(14849, 3, 3362, 13605, 0); -- Darkmoon Carnie

UPDATE `creature` SET `equipment_id` = 2 WHERE `id` = 14849 AND `guid` IN (SELECT `guid` FROM `game_event_creature` WHERE `eventEntry` IN (3,4,5));
UPDATE `creature_equip_template` SET `ItemEntry2`=12865 WHERE (`entry`=14822 AND `ID`=1); -- Sayge

-- DB/Texts: Fix CompletionText of quest Panther Prowess https://github.com/TrinityCore/TrinityCore/commit/954790862471b102b232a71f58fd684439f121fb
UPDATE `quest_template` SET `RequestItemsText` = 'What are you doing here, $n! You should be crawling through the brush, trying to kill 10 Shadowmaw Panthers. I thought you fancied yourself a big game hunter?' WHERE `ID` = 192;
