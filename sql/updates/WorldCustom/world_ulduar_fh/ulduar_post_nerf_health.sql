-- scrapbots snare flag
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` & ~1024 WHERE `entry` IN (33343, 33887);
-- dark rune champion disarm flag
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` & ~4 WHERE `entry` IN (32876, 33158);


UPDATE `creature_template_addon` SET `auras` = "" WHERE `entry` = 34147;

SET @ALGALON_BASE_HEALTH_MOD:= 2500;
UPDATE `creature_template` SET `Health_mod` = @ALGALON_BASE_HEALTH_MOD + (@ALGALON_BASE_HEALTH_MOD * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = 33070;

-- Ignis
SET @IGNIS_10:= 33118;
SET @IGNIS_25:= 33190;
SET @IGNIS_10_BASE_HEALTH:= 400;
SET @IGNIS_25_BASE_HEALTH:= 1400;

-- 10 man
UPDATE `creature_template` SET `health_mod` = @IGNIS_10_BASE_HEALTH + (@IGNIS_10_BASE_HEALTH * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @IGNIS_10;

-- 25 man
UPDATE `creature_template` SET `health_mod` = @IGNIS_25_BASE_HEALTH + (@IGNIS_25_BASE_HEALTH * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @IGNIS_25;

-- XT-002
SET @XT_10:= 33293;
SET @XT_25:= 33885;
SET @XT_HEART_25:= 33995;

SET @XT_BASE_HEALTH_10:= 358.552;
SET @XT_BASE_HEALTH_25:= 1613.48;
SET @XT_HEART_BASE_HEALTH_25:= 537.88;

-- 10m
UPDATE `creature_template` SET `health_mod` = @XT_BASE_HEALTH_10 + (@XT_BASE_HEALTH_10 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @XT_10;
-- 25m
UPDATE `creature_template` SET `health_mod` = @XT_BASE_HEALTH_25 + (@XT_BASE_HEALTH_25 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @XT_25;
UPDATE `creature_template` SET `Health_mod`= @XT_HEART_BASE_HEALTH_25 + (@XT_HEART_BASE_HEALTH_25 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE  `entry`= @XT_HEART_25;

SET @PUMMELER_10:= 33344;
SET @PUMMELER_25:= 33888;
SET @PUMMELER_BASE_10:= 15;
SET @PUMMELER_BASE_25:= 40;
SET @LIFE_SPARK_25:= 34005;
SET @LIFE_SPARK_10:= 34004;
SET @LIFE_SPARK_BASE_10:= 4;
SET @LIFE_SPARK_BASE_25:= 14;

-- 10m
UPDATE `creature_template` SET `health_mod` = @PUMMELER_BASE_10 + (@PUMMELER_BASE_10 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @PUMMELER_10;
-- 25m
UPDATE `creature_template` SET `health_mod` = @PUMMELER_BASE_25 + (@PUMMELER_BASE_25 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @PUMMELER_25;

-- life spark 10m
UPDATE `creature_template` SET `health_mod` = @LIFE_SPARK_BASE_10 + (@LIFE_SPARK_BASE_10 * 0) WHERE `entry` = @LIFE_SPARK_10;

-- life spark 25m
UPDATE `creature_template` SET `health_mod` = @LIFE_SPARK_BASE_25 + (@LIFE_SPARK_BASE_25 * 0) WHERE `entry` = @LIFE_SPARK_25;

-- Razorscale
SET @RAZORSCALE_10:= 33186;
SET @RAZORSCALE_BASE_HEALTH_10:= 255;
SET @RAZORSCALE_25:= 33724;
SET @RAZORSCALE_BASE_HEALTH_25:= 900;

-- 10 man
UPDATE `creature_template` SET `health_mod` = @RAZORSCALE_BASE_HEALTH_10 + (@RAZORSCALE_BASE_HEALTH_10 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @RAZORSCALE_10;

-- 25 man
UPDATE `creature_template` SET `health_mod` = @RAZORSCALE_BASE_HEALTH_25 + (@RAZORSCALE_BASE_HEALTH_25 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @RAZORSCALE_25;

SET @DARK_SENTINEL_10:= 33846;
SET @DARK_SENTINEL_10_BASE_HP:= 13;

-- 10 man
UPDATE `creature_template` SET `health_mod` = @DARK_SENTINEL_10_BASE_HP + (@DARK_SENTINEL_10_BASE_HP * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @DARK_SENTINEL_10;

-- Flame Leviathan
SET @WARD_OF_LIFE_10:= 34275;
SET @WARD_OF_LIFE_25:= 34276;
SET @WARD_BASE_HEALTH_10:= 24;
SET @WARD_BASE_HEALTH_25:= 40;

SET @WRITHING_LASHER10:= 33387;
SET @WRITHING_LASHER25:= 34277;
SET @WRITHING_LASHER10_BASE_HP:= 5.1;
SET @WRITHING_LASHER25_BASE_HP:= 15.1;


-- 10 man
UPDATE `creature_template` SET `health_mod` = @WRITHING_LASHER10_BASE_HP + (@WRITHING_LASHER10_BASE_HP * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @WRITHING_LASHER10;
UPDATE `creature_template` SET `health_mod` = @WARD_BASE_HEALTH_10 + (@WARD_BASE_HEALTH_10 * 0) WHERE `entry` = @WARD_OF_LIFE_10;

-- 25 man
UPDATE `creature_template` SET `health_mod` = @WRITHING_LASHER25_BASE_HP + (@WRITHING_LASHER25_BASE_HP * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @WRITHING_LASHER25;
UPDATE `creature_template` SET `health_mod` = @WARD_BASE_HEALTH_25 + (@WARD_BASE_HEALTH_25 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @WARD_OF_LIFE_25;

-- kologarn
SET @LEFT_ARM_10:= 32933;
SET @LEFT_ARM_25:= 33910;
SET @RIGHT_ARM_10:= 32934;
SET @RIGHT_ARM_25:= 33911;
SET @KOLOGARN_10:= 32930;
SET @KOLOGARN_25:= 33909;

SET @LEFT_ARM_10_BASE:= 39;
SET @LEFT_ARM_25_BASE:= 165;

SET @RIGHT_ARM_10_BASE:= 39;
SET @RIGHT_ARM_25_BASE:= 165;

SET @KOLOGARN_10_BASE:= 260;
SET @KOLOGARN_25_BASE:= 1100;

-- left arm
UPDATE `creature_template` SET `health_mod` = @LEFT_ARM_10_BASE + (@LEFT_ARM_10_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @LEFT_ARM_10;
UPDATE `creature_template` SET `health_mod` = @LEFT_ARM_25_BASE + (@LEFT_ARM_25_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @LEFT_ARM_25;
-- right arm
UPDATE `creature_template` SET `health_mod` = @RIGHT_ARM_10_BASE + (@RIGHT_ARM_10_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @RIGHT_ARM_10;
UPDATE `creature_template` SET `health_mod` = @RIGHT_ARM_25_BASE + (@RIGHT_ARM_25_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @RIGHT_ARM_25;
-- Kologarn
UPDATE `creature_template` SET `health_mod` = @KOLOGARN_10_BASE + (@KOLOGARN_10_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @KOLOGARN_10;
UPDATE `creature_template` SET `health_mod` = @KOLOGARN_25_BASE + (@KOLOGARN_25_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @KOLOGARN_25;

-- Assault bot
SET @ASSAULT_BOT_25:= 34115;
SET @ASSAULT_BOT_25_BASE:= 50;

UPDATE `creature_template` SET `health_mod` = @ASSAULT_BOT_25_BASE + (@ASSAULT_BOT_25_BASE * 0), dmg_multiplier = 14, `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @ASSAULT_BOT_25;
UPDATE `creature_template` SET `dmg_multiplier` = 10, `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = 34057; -- assault bot 10

-- General Vezax
SET @GENERAL_VEZAX_10:= 33271;
SET @GENERAL_VEZAX_25:= 33449;

SET @SARONITE_ANIMUS_10:= 33524;
SET @SARONITE_ANIMUS_25:= 34152;

SET @GENERAL_VEZAX_BASE_10:= 450;
SET @GENERAL_VEZAX_BASE_25:= 1648.09;

SET @SARONITE_ANIMUS_10_BASE:= 113;
SET @SARONITE_ANIMUS_25_BASE:= 595.28;

-- 10 man
UPDATE `creature_template` SET `health_mod` = @GENERAL_VEZAX_BASE_10 + (@GENERAL_VEZAX_BASE_10 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @GENERAL_VEZAX_10;
-- 25 man
UPDATE `creature_template` SET `health_mod` = @GENERAL_VEZAX_BASE_25 + (@GENERAL_VEZAX_BASE_25 * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @GENERAL_VEZAX_25;
-- 10 man
UPDATE `creature_template` SET `health_mod` = @SARONITE_ANIMUS_10_BASE + (@SARONITE_ANIMUS_10_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @SARONITE_ANIMUS_10;
-- 25 man
UPDATE `creature_template` SET `health_mod` = @SARONITE_ANIMUS_25_BASE - (@SARONITE_ANIMUS_25_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @SARONITE_ANIMUS_25;

-- yogg saron
SET @YOGG_SARON_25:= 33955;

SET @YOGG_SARON_25_BASE:= 3155.2;

UPDATE `creature_template` SET `health_mod` = @YOGG_SARON_25_BASE + (@YOGG_SARON_25_BASE * 0), `flags_extra` = `flags_extra` | 0x00000008 WHERE `entry` = @YOGG_SARON_25;

-- Allow parry-hasten for every creature except Kologarn
UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 8 WHERE `entry` IN
(22515,
24921,
32780,
32845,
32857,
32865,
32867,
32879,
32892,
32906,
32913,
32914,
32915,
32922,
32923,
32924,
32925,
32927,
33054,
33059,
33060,
33062,
33063,
33089,
33109,
33113,
33118,
33121,
33134,
33140,
33141,
33186,
33213,
33214,
33235,
33236,
33237,
33241,
33242,
33244,
33264,
33271,
33282,
33293,
33350,
33354,
33355,
33378,
33430,
33431,
33432,
33515,
33525,
33526,
33527,
33528,
33571,
33575,
33576,
33579,
33620,
33622,
33624,
33626,
33627,
33629,
33661,
33662,
33669,
33672,
33686,
33696,
33699,
33701,
33721,
33722,
33725,
33754,
33755,
33772,
33774,
33775,
33779,
33818,
33819,
33820,
33822,
33823,
33824,
33874,
33956,
33957,
34014,
34054,
34069,
34085,
34086,
34096,
34133,
34134,
34135,
34146,
34150,
34151,
34164,
34183,
34190,
34191,
34192,
34193,
34196,
34197,
34198,
34199,
34234,
34267,
34269,
34271,
34273,
32846,
33694,
33147,
33693,
33360,
33392,
33393,
33391,
33692,
34045,
34221,
34003,
33190,
33191,
34332,
33724,
34113,
34105,
33449,
33885,
33729,
33737,
33732,
33731,
34106,
34175,
33735,
33734,
33741,
33733,
33700,
33723,
33757,
33758,
33773,
33827,
33829,
33830,
33828,
33832,
33831,
34166,
34185,
34186,
34201,
34139,
34141,
34142,
34165,
34214,
34229,
34217,
34216,
34220,
34245,
34226,
34236,
34237,
34235,
34268,
34270,
34272,
34274,
32871,
33070);
