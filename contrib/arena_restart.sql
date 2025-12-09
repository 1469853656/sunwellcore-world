-- Reset all arena teams including solo queue
UPDATE `solo_queue` SET `rating` = 0, `weekGames` = 0, `weekWins` = 0, `seasonGames` = 0, `seasonWins` = 0;
UPDATE `arena_team` SET `rating` = 0, `seasonGames` = 0, `seasonWins` = 0, `weekGames` = 0, `weekWins` = 0, `rank` = 1;
UPDATE `arena_team_member` SET `personalRating` = 0, `weekGames` = 0, `weekWins` = 0, `seasonGames` = 0, `seasonWins` = 0, `personal_rating` = 0;

-- Set base MMR of 1500
UPDATE `character_arena_stats` SET `matchMakerRating` = 1500, `maxMMR` = 1500;
UPDATE solo_queue SET matchMakerRating = 1500, personalRating = 0;

-- Delete all refund instance rows, people store items before season reset and sell them right after
DELETE FROM `item_refund_instance`;
/*
-- Delete commendation of bravery
DELETE FROM item_instance WHERE itemEntry = 45706;
*/

-- Remove all arena points
UPDATE `characters` SET `arenaPoints` = 0;
