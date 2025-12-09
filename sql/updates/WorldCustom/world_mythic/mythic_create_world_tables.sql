DROP TABLE IF EXISTS `creature_template_mythic`;
DROP TABLE IF EXISTS `creature_mythic_spells`;
DROP TABLE IF EXISTS `creature_mythic_spell_values`;
DROP TABLE IF EXISTS `mythic_affix_template`;
DROP TABLE IF EXISTS `mythic_affix_presets`;

ALTER TABLE `creature_template` DROP COLUMN `MythicTemplateId`;
ALTER TABLE `creature_template` ADD COLUMN `MythicTemplateId` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `VerifiedBuild`;

CREATE TABLE `creature_template_mythic`
(
	`Id` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY,
	`CreatureType` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
	`CreatureLevel` MEDIUMINT UNSIGNED NOT NULL DEFAULT 80,
	`EncounterAbilityMask` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
	`BaseMythicHealth` INT UNSIGNED NOT NULL DEFAULT 0,
	`BaseMythicPower` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
	`MinimumRawMeleeDamage` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Minimum raw melee damage',
	`MaximumRawMeleeDamage` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Minimum raw melee damage',
	`KeyVarianceMelee` FLOAT NOT NULL DEFAULT 1,
	`KeyVarianceStats` FLOAT NOT NULL DEFAULT 1,
	`MythicLootId` INT(6) UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `creature_mythic_spells`
(
	`CreatureType` MEDIUMINT UNSIGNED NOT NULL DEFAULT 0,
	`SpellId` 			INT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY,
	`SpellEffect0Value` INT NOT NULL DEFAULT 0,
	`SpellEffect1Value` INT NOT NULL DEFAULT 0,
	`SpellEffect2Value` INT NOT NULL DEFAULT 0,
	`SpellDuration` 	INT NOT NULL DEFAULT 0,
	`SpellMaxTargets`	INT UNSIGNED NOT NULL DEFAULT 0,
	`SpellValueVariance` FLOAT NOT NULL DEFAULT 1,
	`Timers`			VARCHAR(100) NOT NULL DEFAULT "" COMMENT 'ScheduleTime;RepeatTime seperated by ; IN MILLISECONDS',
	`TargetType`		INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `creature_mythic_spell_values`
(
	`SpellId` 			INT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY,
	`SpellEffect0Value` INT NOT NULL DEFAULT 0,
	`SpellEffect1Value` INT NOT NULL DEFAULT 0,
	`SpellEffect2Value` INT NOT NULL DEFAULT 0,
	`SpellDuration` 	INT NOT NULL DEFAULT 0,
	`SpellMaxTargets`	INT UNSIGNED NOT NULL DEFAULT 0,
	`SpellValueVariance` FLOAT NOT NULL DEFAULT 1
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `mythic_affix_template`
(
	`AffixId` INT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY,
	`AffixName` VARCHAR(50) NOT NULL DEFAULT "",
	`AffixFlags` INT UNSIGNED NOT NULL DEFAULT 0,
	`AffixChance` INT UNSIGNED NOT NULL DEFAULT 100,
	`AffixCooldown` INT UNSIGNED NOT NULL DEFAULT 1000 COMMENT 'in milliseconds, default 1000ms',
	`AffixKeyRequirement` INT UNSIGNED NOT NULL DEFAULT 1 COMMENT 'level bracket at which this affix comes into play, check Mythic::MythicAffixBrackets for more.',
	`IsInstanceAffix` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT '1 means its instance affix, creatures cant use this one'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `mythic_affix_presets`
(
	`PresetId` INT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY,
	`FirstAffix` INT UNSIGNED NOT NULL DEFAULT 0,
	`SecondAffix` INT UNSIGNED NOT NULL DEFAULT 0,
	`ThirdAffix` INT UNSIGNED NOT NULL DEFAULT 0,
	`FourthAffix` INT UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDb DEFAULT CHARSET=utf8;

