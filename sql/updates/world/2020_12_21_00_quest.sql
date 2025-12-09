-- DB/Quest: Bombing Run - remove prequest requirement from non-druid version
UPDATE `quest_template` SET `NextQuestId` = 0 WHERE `id` = 11058;