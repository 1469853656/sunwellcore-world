DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=19 AND `SourceEntry` IN (11585, 11586);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(19, 0, 11585, 0, 0, 8, 0, 10212, 0, 0, 1, 0, 0, '', 'Show quest \'Hellscream\'s Vigil\' only if quest \'Hero of the Mag\'har\' is not rewarded'),
(19, 0, 11586, 0, 0, 8, 0, 10212, 0, 0, 0, 0, 0, '', 'Show quest \'Hellscream\'s Vigil\' only if quest \'Hero of the Mag\'har\' is rewarded');
