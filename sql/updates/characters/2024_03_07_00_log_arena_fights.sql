ALTER TABLE `log_arena_fights`
DROP COLUMN `winner`,
DROP COLUMN `loser`,
DROP COLUMN `winner_tr`,
DROP COLUMN `winner_tr_change`,
DROP COLUMN `loser_tr`,
DROP COLUMN `loser_tr_change`; 

ALTER TABLE `log_arena_memberstats`
DROP COLUMN `team`;
