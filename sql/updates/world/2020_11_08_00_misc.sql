-- DB/Quest: Protecting the Homeland - custom bg quest - fix text https://github.com/SunwellWoW/Sunwell-WOTLK/issues/212
UPDATE `quest_template` SET `RequestItemsText` = 'Good to see you $N! Are you brave enough to be called a gladiator?' WHERE `id` = 40001;
