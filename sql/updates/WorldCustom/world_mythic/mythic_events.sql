-- Crystals Event 
SET @ENTRY := 302100;
DELETE FROM `gameobject_template` WHERE `entry` BETWEEN @ENTRY AND @ENTRY + 3;
INSERT INTO `gameobject_template` VALUES 
(@ENTRY + 0,1,327,'Mythic Crystal','','','',35,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_crystal',12340),
(@ENTRY + 1,1,5811,'Mythic Crystal','','','',35,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_crystal',12340),
(@ENTRY + 2,1,6706,'Mythic Crystal','','','',35,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_crystal',12340),
(@ENTRY + 3,1,6451,'Mythic Crystal','','','',35,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_crystal',12340);

SET @ENTRY := 230000;
DELETE FROM `creature_template` WHERE `entry` = @ENTRY;
INSERT INTO `creature_template` VALUES (@ENTRY,0,0,0,0,0,29077,0,0,0,'Ghost of Crystals','','',0,70,70,1,35,0,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,33555200,2048,0,0,0,0,0,0,215,320,44,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,4,1,1,1,1,0,0,0,0,0,0,0,0,1,0,128,'npc_mythic_crystals_trigger',12340,0);

-- Lost NPCs Event
SET @ENTRY = 1210000;
DELETE FROM `npc_text` WHERE `ID` BETWEEN @ENTRY AND @ENTRY + 2;
INSERT INTO `npc_text`(`ID`, `text0_0`) VALUES (@ENTRY, 'Ishnu-alah $N!\nI\'ve found a favour of Elune once again, that\'s good to see you. My little wisps are so naughty tonight. I didn\'t look at them by a moment and look they\'ve entered to this dungeon.
\nI am too weak to defeat all of these cruel monsters ... please find my children, they are hidden in the darkest corners of this dungeon. I feel that they are too scared to appear and find a path to me.\nI will give you an amulet, a crystal inside of it is infused by a power of stars, it will let you to open your eyes and mind as well, so you will be able to see hidden wisps inside of the dungeon!
The amulet bounds our souls together, so I will be whispering to you if I will feel a wisp is close.'),
(@ENTRY + 1, 'Greetings $N!\nIt\'s good to see one of the Azeroth\'s heroes once again.\nMy scouts have told me that some of the terrifying rituals have a place in the darkest corners of the dungeon called Blood Furnace.\nI\'ve taken my best warriors and we\'ve decided to investigate these horrifying halls. 
As I see the rituals have broken easily will of my warriors and they have ran off with a cry. They are well-trained berserkers, but their will is not unbreakable.
I will give you a chance to prove your worth, if your mission will be successful we will stand on your side while fighting against Keli\'dan the Breaker.
I will give you the horn, once you will blow into this horn my warriors will hear an order to fight once again, it will let you to find them and lead them to me. 
You have to go, time is ticking, let the path of blood and glory lead your way through these halls!'),
(@ENTRY + 2, 'Hello $n!\n I see that my flares which have been sent into the air have not failed. You\'ve found us. That\'s good to see you!\nWe planned this expedition for months, I\'ve found the best engineers in Gadgetzan and decide to move with them to Zul\'Farrrak. 
Unfortunately, we\'ve met some kind of ... unexpected ... visitors. Yes, we didn\'t know that the place is still occupied by half-living trolls, necromancers and a hydra!
My engineers have lost in the corners of this city, I will not leave them!\nIf you will find all of them we\'ll show you some fancy goblin tricks which supposedly may help you with defeating Chief Ukroz Sanscalp and Ruuzlu. 
I will give you my best flare gun, it will help you to find the lost engineers. That\'s our deal, my hero!');

SET @ENTRY := 230001;
DELETE FROM `creature_template` WHERE `entry` BETWEEN @ENTRY AND @ENTRY + 2;
INSERT INTO `creature_template` VALUES 
(@ENTRY,0,0,0,0,0,4862,0,0,0,'Daelylah Lightseeker','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_lost_npcs_main',12340,0),
(@ENTRY + 1,0,0,0,0,0,29496,0,0,0,'Ukash Doomtale','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_lost_npcs_main',12340,0),
(@ENTRY + 2,0,0,0,0,0,18290,0,0,0,'Grexle Goldbutton','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_lost_npcs_main',12340,0);

DELETE FROM `creature_text` WHERE `entry` BETWEEN @ENTRY AND @ENTRY + 2;
INSERT INTO `creature_text` VALUES
(@ENTRY, 0, 0,'One of my children is fascinated by mighty soldiers, but I\'ve seen that their bodies are frozen by winds of Northrend.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs Nexus whisp'),
(@ENTRY, 1, 0,'Look at these Mage Hunters, they know nothing about true magic, for sure one of my wisps has been pushed to the wall and is too much afraid to sneak near them.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs Nexus whisp'),
(@ENTRY, 2, 0,'The view is breathtaking while standing on one of these platforms, Nexus is looking like a so peaceful place, but to be honest it\'s just a lie.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs Nexus whisp'),
(@ENTRY, 3, 0,'My love probably tried to find a different exit, I suppose that it has smelled a northern wind and went in this direction.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs Nexus whisp'),
(@ENTRY, 4, 0,'Crystals on the frozen plants looks so wondrous, I know the wisps too long, they\'ve tried to break off one of these.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs Nexus whisp'),
(@ENTRY, 5, 0,'Ancient defenders are so common for Night Elves, wisps enjoying just flying around them and gently smooth their leaves.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs Nexus whisp'),
(@ENTRY + 1, 0, 0,'There\'s a hidden path, I suppose that\'s worth investigating.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs BF whisp'),
(@ENTRY + 1, 1, 0,'The upper parts of Blood Furnace has an interesting view for the rituals of Keli\'dan.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs BF whisp'),
(@ENTRY + 1, 2, 0,'I feel that one of my warriors has been trapped by Shadowmoon Technicians, I don\'t want to know what experiments they are doing right now.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs BF whisp'),
(@ENTRY + 1, 3, 0,'A well-trained warrior knows how to use an environment to protect himself. I could say that these tables would be a nice protection ... at least for a moment. ', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs BF whisp'),
(@ENTRY + 1, 4, 0,'Fel Orcs\' armoury looks like a good place to steal some weapons.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs BF whisp'),
(@ENTRY + 1, 5, 0,'So close to Keli\'Dan, just an ambush and the ritual will end...', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs BF whisp'),
(@ENTRY + 2, 0, 0,' I\'ve felt a weird demigod energy nearby, I\'m afraid one of my engineers might be there captured by this creature!', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs ZF whisp'),
(@ENTRY + 2, 1, 0,'The three-headed snake fountain was once the most refreshing place here, I suppose that one of the engineers may be resting there.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs ZF whisp'),
(@ENTRY + 2, 2, 0,'My comrades are interested in wild animals that might be a bit annoying, but goblins enjoy playing with basilisks.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs ZF whisp'),
(@ENTRY + 2, 3, 0,'Awful experiments are being done by one of the trolls, his experiments consist of trying to revive the dead. I think that one of my workers was taken by him!', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs ZF whisp'),
(@ENTRY + 2, 4, 0,'Once a great temple, now a place for executions.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs ZF whisp'),
(@ENTRY + 2, 5, 0,'That may be a bit dangerous, but these tents look like a nice place to be hidden. I think that one of my goblins is hiding behind trolls\' backs.', 15, 0, 100, 0, 0, 0, 0, 'Mythic lost npcs ZF whisp');

SET @ENTRY := 230010;
DELETE FROM `creature_template` WHERE `entry` BETWEEN @ENTRY AND @ENTRY + 2;
INSERT INTO `creature_template` VALUES 
(@ENTRY,0,0,0,0,0,10045,0,0,0,'Wisp','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_lost_npcs',12340,0),
(@ENTRY + 1,0,0,0,0,0,4259,4260,4601,0,'Entrapped Warrior','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_lost_npcs',12340,0),
(@ENTRY + 2,0,0,0,0,0,7217,0,0,0,'Lost Engineer','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_lost_npcs',12340,0);


SET @ENTRY := 18565;
DELETE FROM `item_template` WHERE `entry` = @ENTRY;
INSERT INTO `item_template` VALUES (@ENTRY,15,0,-1,'Amulet of an Imprisoned Wisp',37541,5,64,0,1,0,0,0,-1,-1,70,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,70522,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,1,'Tiny sparkles are flying around inside of the amulet\'s crystal.',0,0,0,0,0,-1,0,0,0,0,0,0,0,576,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,'item_mythic_lost_npc',0,0,0,0,0,1); 

SET @ENTRY := 5515;
DELETE FROM `item_template` WHERE `entry` = @ENTRY;
INSERT INTO `item_template` VALUES (@ENTRY,15,0,-1,'Horn of the Command',7985,5,64,0,1,0,0,0,-1,-1,70,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,70522,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,1,'...just an old, blood-covered Mammoth\'s horn...',0,0,0,0,0,-1,0,0,0,0,0,0,0,542,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,'item_mythic_lost_npc',0,0,0,0,0,1); 

SET @ENTRY := 3686;
DELETE FROM `item_template` WHERE `entry` = @ENTRY;
INSERT INTO `item_template` VALUES (@ENTRY,15,0,-1,'Goblin Flare Gun',7092,5,64,0,1,0,0,0,-1,-1,70,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,70522,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,1,'Read the instruction before the first usage',0,0,0,0,0,-1,0,0,0,0,0,0,0,209,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,'item_mythic_lost_npc',0,0,0,0,0,1); 

-- Alchemist Event
SET @ENTRY := 230020;
DELETE FROM `creature_template` WHERE `entry` BETWEEN @ENTRY AND @ENTRY + 1;
INSERT INTO `creature_template` VALUES 
(@ENTRY,0,0,0,0,0,1823,0,0,0,'Apprentice Ignazio','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_alchemist',12340,0),
(@ENTRY + 1,0,0,0,0,0,1823,0,0,0,'Apprentice Ignazio','','',0,70,70,1,35,1,1,1.14286,1,0,252,357,0,304,1,2000,2000,1,0,2048,0,0,0,0,0,0,215,320,44,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,'npc_mythic_alchemist_summon',12340,0);

SET @ENTRY := 1210010;
DELETE FROM `npc_text` WHERE `ID` BETWEEN @ENTRY AND @ENTRY + 4;
INSERT INTO `npc_text`(`ID`, `text0_0`) VALUES 
(@ENTRY, 'I am glad to see you $N!\nMy master has sent me to find a recipe which is needed to prepare a powerful elixir called Might of the Ancients. I\'ve been studying the old scrolls and according to the manuscripts written by high elves, the recipe should be hidden in the darkest corners of this dungeon.\nI am just alchemist\'s apprentice I am not familiar with arms or some destructive magic, so I have to ask you for a favour.\nFind shattered fragments of the recipe and once you will gather three parts we\'ll try to prepare a small portion of the elixir.\nI will give you the bell, it\'s not just a bell, it\'s an old family heirloom. Once you will use that I will find you in the dark and appear next to you.'),
(@ENTRY + 1, 'Hello once again $N!\nI\'ve heard the bell, is that means that we are going to start the work?'),
(@ENTRY + 2, 'Old manuscripts tell that we have to melt two ingredients together at the beginning, what should we use?'),
(@ENTRY + 3, 'Sounds clear, what should be the next? I remember that the elixir needs trophies of dangerous Azeroth\'s creatures. Luckily, I\'ve met an experienced hunter yesterday so I should have some of the ingredients in my bag'),
(@ENTRY + 4, 'Looks like we almost end. I am starting to be impatient...');


SET @PAGE_ENTRY := 4000;
DELETE FROM `page_text` WHERE `entry` BETWEEN @PAGE_ENTRY AND @PAGE_ENTRY + 2;
INSERT INTO `page_text`(`entry`, `text`) VALUES
(@PAGE_ENTRY + 0, 'Melt a $5000w portions of Dream Dust with $5001w glasses of Ethereal Oil in a small stockpot on medium heat.$B$BAdd $5002w pieces of Sungrass and continually stir until tender.$B$BTry to don\'t let the mixture boiling...'),
(@PAGE_ENTRY + 1, 'Add $5003w powdered Large Fangs and $5004w chopped Small Flame Sacs.$B$BAdd $5005w pinches of Dream Dust to make the elixir more clear...'),
(@PAGE_ENTRY + 2, 'Stir occasionally, wait for the moment when small sparkles will start appearing at top of the prepared mixture and add $5006w pieces of Arthas\' Tears.');

SET @ENTRY := 302200;
DELETE FROM `gameobject_template` WHERE `entry` BETWEEN @ENTRY AND @ENTRY + 2;
INSERT INTO `gameobject_template` VALUES 
(@ENTRY + 0,9,7883,'Alchemist\'s Book','','','',0,0,1,0,0,0,0,0,0,@PAGE_ENTRY + 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_alchemist_book',12340),
(@ENTRY + 1,9,7883,'Alchemist\'s Book','','','',0,0,1,0,0,0,0,0,0,@PAGE_ENTRY + 1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_alchemist_book',12340),
(@ENTRY + 2,9,7883,'Alchemist\'s Book','','','',0,0,1,0,0,0,0,0,0,@PAGE_ENTRY + 2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','go_mythic_event_alchemist_book',12340);

SET @ENTRY := 40232;
DELETE FROM `item_template` WHERE `entry` = @ENTRY;
INSERT INTO `item_template` VALUES (@ENTRY,15,0,-1,'Ignazio\'s Family Heirloom',39930,5,64,0,1,0,0,0,-1,-1,70,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16872,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,0,0,0,0,-1,0,-1,1,'A gold bell with runic signs',0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,'item_mythic_alchemist_whistle',0,0,0,0,0,1); 
