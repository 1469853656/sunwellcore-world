DELETE FROM creature_template WHERE entry=190001;
INSERT INTO creature_template (entry, modelid1, NAME, subname, IconName, gossip_menu_id, minlevel, maxlevel, faction, npcflag, speed_walk, speed_run, scale, `rank`, unit_class, unit_flags, TYPE, type_flags, RegenHealth, flags_extra, AiName, ScriptName) VALUES
(190001, 21572, "", "", "", 0, 71, 71, 35, 3, 1, 1.14286, 1.25, 1, 1, 2, 7, 138936390, 1, 2, "", "npc_level_up");
DELETE FROM gossip_menu WHERE entry=60000;
INSERT INTO gossip_menu (entry, text_id) VALUES
(60000, 350000);
DELETE FROM npc_text WHERE ID=350000;
INSERT INTO npc_text (ID, text0_0, em0_1) VALUES
(350000, "Level Up", 0);