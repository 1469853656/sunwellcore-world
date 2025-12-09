SET @SETMASK_PREBIS := 1;
SET @SETMASK_T7BIS := 128;
REPLACE INTO `premade_sets` (`SetName`, `SetLevel`, `SetClass`, `SetSpec`, `SetMask`) VALUES 
('DKBloodPrebis',               80, 6,  0, @SETMASK_PREBIS),
('DKFrostPrebis',               80, 6,  1, @SETMASK_PREBIS),
('DKUnholyPrebis',              80, 6,  2, @SETMASK_PREBIS),

('DruidBalaPrebis',             80, 11, 0, @SETMASK_PREBIS),
('DruidFeralDPSPrebis',         80, 11, 1, @SETMASK_PREBIS),
('DruidRestoPrebis',            80, 11, 2, @SETMASK_PREBIS),
('DruidFeralTankPrebis',        80, 11, 3, @SETMASK_PREBIS),

('HuntBMPrebis',                80, 3,  0, @SETMASK_PREBIS),
('HuntMMPrebis',                80, 3,  1, @SETMASK_PREBIS),
('HuntSurviPrebis',             80, 3,  2, @SETMASK_PREBIS),

('MageArcanePrebis',            80, 8,  0, @SETMASK_PREBIS),
('MageFirePrebis',              80, 8,  1, @SETMASK_PREBIS),
('MageFrostPrebis',             80, 8,  2, @SETMASK_PREBIS),

('PalHolyPrebis',               80, 2,  0, @SETMASK_PREBIS),
('PalProtPrebis',               80, 2,  1, @SETMASK_PREBIS),
('PalRetriPrebis',              80, 2,  2, @SETMASK_PREBIS),

('PriDiscPrebis',               80, 5,  0, @SETMASK_PREBIS),
('PriHolyPrebis',               80, 5,  1, @SETMASK_PREBIS),
('PriShadPrebis',               80, 5,  2, @SETMASK_PREBIS),

('RogAssaPrebis',               80, 4,  0, @SETMASK_PREBIS),
('RogCombPrebis',               80, 4,  1, @SETMASK_PREBIS),
('RogSubPrebis',                80, 4,  2, @SETMASK_PREBIS),

('ShamElePrebis',               80, 7,  0, @SETMASK_PREBIS),
('ShamEnhPrebis',               80, 7,  1, @SETMASK_PREBIS),
('ShamRestoPre',                80, 7,  2, @SETMASK_PREBIS),

('WarArmsPrebis',               80, 1,  0, @SETMASK_PREBIS),
('WarFuryPrebis',               80, 1,  1, @SETMASK_PREBIS),
('WarProtPrebis',               80, 1,  2, @SETMASK_PREBIS),

('WarlockAffliPrebis',          80, 9,  0, @SETMASK_PREBIS),
('WarlockDemoPrebis',           80, 9,  1, @SETMASK_PREBIS),
('WarlockDestroPrebis',         80, 9,  2, @SETMASK_PREBIS),

('WarArmsT7BiS',                80, 1,  0, @SETMASK_T7BIS);
