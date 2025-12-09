DROP TABLE IF EXISTS `boost_items`;

CREATE TABLE `boost_items` (
	`class` INT NOT NULL,
	`spec` INT NOT NULL,	-- 0 - melee dps, 1 - ranged dps, 2 - healer, 3 - tank
	`item` VARCHAR(300),
	PRIMARY KEY (`class`, `spec`)
);

INSERT INTO `boost_items` VALUES
(8, 1, '37746;37748;27433;34702;24397;27452;27889;27795;30709;27411;27491;28227;26055;28418;28386;27842'), -- mage dps
(9, 1, '37746;37748;27433;34702;24397;27452;27889;27795;30709;27411;27491;28227;26055;28418;28386;27842'), -- warlock dps
(5, 1, '37746;37748;27433;34702;24397;27452;27889;27795;30709;27411;27491;28227;26055;28418;28386;27842'), -- priest dps
(5, 2, '37746;37748;27433;34702;24397;27452;27889;27795;30709;27411;27491;28259;24390;28190;27885;27791'), -- priest healer
(4, 0, '27415;27779;27995;27892;30933;31175;24365;28124;24466;27467;27925;27761;28034;27416;27913;24356;27631'), -- rouge dps
(11, 0, '27415;27779;27995;27892;30933;31175;24365;28124;24466;27467;27925;27761;28034;27416;27877;37573'), -- druid feral dps
(11, 2, '25682;37748;31190;34702;37745;27827;28214;27783;27492;28251;27491;28259;24390;28190;22398;27791'), -- druid healer dps
(11, 1, '25682;37748;31190;34702;37745;27827;28214;27783;27492;28251;27491;28227;26055;28418;23197;27842'), -- druid balance dps
(7, 0, '27414;27779;28344;27892;28401;34698;27745;27478;31240;27915;27925;27761;28034;27416;27980;27980;27815'), -- shaman enh dps
(7, 1, '25955;37748;27454;34702;27912;28194;34791;27743;27458;27549;27491;28227;26055;28418;28248;31287;27543'), -- shaman ele dps
(7, 2, '25955;37748;27454;34702;24357;28194;34791;27835;27458;27549;27491;28259;24390;28190;27538;31287;27544'), -- shaman resto heal
(3, 1, '27414;27779;28344;27892;28401;34698;27745;27478;31240;27915;27925;27761;28034;27416;31291;31303'), -- hunter dps
(2, 0, '28761;28509;28207;28371;28403;27918;27497;28375;24456;27788;27761;27460;28034;27416;27484;31299'), -- paladin dps
(2, 3, '28761;27871;27803;27550;28205;27459;27475;27672;27527;27813;28265;28407;24376;27529;27917;28166;31234'), -- paladin tank
(2, 2, '28560;37748;27739;34702;31152;27489;34700;31202;27893;27420;27491;28259;24390;28190;28296;31287;27538'), -- paladin heal
(1, 0, '28559;28509;28207;28371;28403;27918;27497;28375;24456;27788;27761;27460;28034;27416;31303;27986;25944'), -- warrior dps
(1, 3, '28559;27871;27803;27550;28205;27459;27475;27672;27527;27813;28265;28407;24376;27529;32780;28166;31234'); -- warrior tank

SET @NPC_ENTRY := 360000;
DELETE FROM `creature_template` WHERE `entry`=@NPC_ENTRY;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(@NPC_ENTRY,0,0,0,0,0,23156,0,0,0,'Aeriash, Herald of the Frozen Winds','Character boost','',0,80,80,0,35,1,1,1.14286,1,0,0,0,0,0,1,2000,0,4,0,0,8,0,0,0,0,0,0,0,0,9,138936390,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,9.35979,1,1,0,0,0,0,0,0,0,121,1,0,0,'npc_character_boost',12340);

UPDATE `creature_template` SET `scale` = 2 WHERE `entry` = @NPC_ENTRY;

DELETE FROM `npc_text` WHERE `ID` BETWEEN @NPC_ENTRY * 10 AND @NPC_ENTRY * 10 + 4;
INSERT INTO `npc_text`(`ID`, `text0_0`) VALUES
(@NPC_ENTRY * 10 + 0, 'You have selected Melee DPS specialization.\nAre you sure?'),
(@NPC_ENTRY * 10 + 1, 'You have selected Ranged DPS specialization.\nAre you sure?'),
(@NPC_ENTRY * 10 + 2, 'You have selected Healer specialization.\nAre you sure?'),
(@NPC_ENTRY * 10 + 3, 'You have selected Tank specialization.\nAre you sure?'),
(@NPC_ENTRY * 10 + 4, 'Greetings $N!\nIf you are willing to start a new adventure and support forces of your faction in the Icecrown Citadel offensive let me know.\nI am known as Herald of the Frozen winds, I am preparing newly come Azeroth heroes to strike against the dark forces of the Lich King.\nIf you want to fight against them tell me your story, the story about the mighty Azeroth hero. Choose the most desired equipment which will be matching to your role and that\'s it! The frozen ground knowns as Northrend will be waiting for you.\nMake sure that your choice will be correct, there\'s no retreat!');

SET @CGUID := 310000;
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID AND @CGUID + 7;
INSERT INTO `creature` VALUES 
(@CGUID + 0,@NPC_ENTRY,530,1,1,0,0,-3955.970459,-13942.941406,101.034004,2.040438,300,0,0,50000,0,0,0,0,0),
(@CGUID + 1,@NPC_ENTRY,530,1,1,0,0,10340.881836,-6373.701172,35.499180,1.172204,300,0,0,50000,0,0,0,0,0),
(@CGUID + 2,@NPC_ENTRY,0,1,1,0,0,-8926.001953,-109.856277,83.125221,4.652258,300,0,0,50000,0,0,0,0,0),
(@CGUID + 3,@NPC_ENTRY,1,1,1,0,0,-590.010254,-4224.067871,38.289394,3.993593,300,0,0,50000,0,0,0,0,0),
(@CGUID + 4,@NPC_ENTRY,1,1,1,0,0,-2898.922852,-230.488419,53.885365,4.570064,300,0,0,50000,0,0,0,0,0),
(@CGUID + 5,@NPC_ENTRY,1,1,1,0,0,10321.885742,818.076782,1326.443726,1.522928,300,0,0,50000,0,0,0,0,0),
(@CGUID + 6,@NPC_ENTRY,0,1,1,0,0,-6231.326172,346.761902,383.675171,4.732026,300,0,0,50000,0,0,0,0,0),
(@CGUID + 7,@NPC_ENTRY,0,1,1,0,0,1656.739136,1673.723755,120.718811,0.032361,300,0,0,50000,0,0,0,0,0);

UPDATE `item_template` i SET i.SellPrice = 0, i.DisenchantId = 0 WHERE i.entry IN (28386,37746,27433,24397,27452,27889,27795,30709,27411,27885,27913,24356,27631,27415,27995,30933,31175,24365,28124,24466,27467,27877,37573,22398,27791,25682,31190,37745,27827,28214,27783,27492,28251,23197,27842,27980,27815,27912,27743,28227,26055,28418,28248,27543,25955,27454,24357,28194,34791,27835,27458,27549,27544,27414,27779,28344,27892,28401,34698,27745,27478,31240,27915,27925,31291,27484,31299,28761,27917,28560,37748,27739,34702,31152,27489,34700,31202,27893,27420,27491,28259,24390,28190,28296,31287,27538,28509,28207,28371,28403,27918,27497,28375,24456,27788,27761,27460,28034,27416,31303,27986,25944,28559,27871,27803,27550,28205,27459,27475,27672,27527,27813,28265,28407,24376,27529,32780,28166,31234);

DELETE FROM `creature_template_addon` WHERE `entry` = @NPC_ENTRY;
INSERT INTO `creature_template_addon` VALUES (@NPC_ENTRY, 0, 0, 0, 0, 0, "58837");