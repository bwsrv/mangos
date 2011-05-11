-- Areatrigger table format change

ALTER TABLE areatrigger_teleport CHANGE required_quest_done required_quest_done_A int(11) UNSIGNED NOT NULL DEFAULT "0"
COMMENT "Alliance quest";

ALTER TABLE areatrigger_teleport CHANGE required_quest_done_heroic required_quest_done_heroic_A int(11) UNSIGNED NOT NULL DEFAULT "0"
COMMENT "Alliance heroic quest";

ALTER TABLE areatrigger_teleport ADD COLUMN
required_quest_done_H int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Horde quest"
AFTER required_quest_done_A;

ALTER TABLE areatrigger_teleport ADD COLUMN
required_quest_done_heroic_H int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Horde heroic quest"
AFTER required_quest_done_heroic_A;

ALTER TABLE areatrigger_teleport ADD COLUMN
minGS int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Min player gear score";

ALTER TABLE areatrigger_teleport ADD COLUMN
maxGS int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Max player gear score";

UPDATE areatrigger_teleport SET required_quest_done_H = required_quest_done_A
WHERE required_quest_done_A > 0;

UPDATE areatrigger_teleport SET required_quest_done_heroic_H = required_quest_done_heroic_A
WHERE required_quest_done_heroic_A > 0;

-- Creature on kill Reputation

ALTER TABLE creature_onkill_reputation
 ADD COLUMN ChampioningAura int(11) unsigned NOT NULL default 0 AFTER TeamDependent;

-- Implement DBC encounters

DROP TABLE IF EXISTS `instance_encounters`;
CREATE TABLE `instance_encounters` (
    `entry` int(10) unsigned NOT NULL COMMENT 'Unique entry from DungeonEncounter.dbc',
    `creditType` tinyint(3) unsigned NOT NULL DEFAULT '0',
    `creditEntry` int(10) unsigned NOT NULL DEFAULT '0',
    `lastEncounterDungeon` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT 'If not 0, LfgDungeon.dbc entry for the instance it is last encounter in',
    `comment` varchar(255) NOT NULL DEFAULT '',
     PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- LFG dungeon reward structure from TC

DROP TABLE IF EXISTS `lfg_dungeon_rewards`;
CREATE TABLE `lfg_dungeon_rewards` (
    `dungeonId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Dungeon entry from dbc',
    `maxLevel` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Max level at which this reward is rewarded',
    `firstQuestId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest id with rewards for first dungeon this day',
    `firstMoneyVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Money multiplier for completing the dungeon first time in a day with less players than max',
    `firstXPVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Experience multiplier for completing the dungeon first time in a day with less players than max',
    `otherQuestId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest id with rewards for Nth dungeon this day',
    `otherMoneyVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Money multiplier for completing the dungeon Nth time in a day with less players than max',
    `otherXPVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Experience multiplier for completing the dungeon Nth time in a day with less players than max',
    PRIMARY KEY (`dungeonId`, `maxLevel)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Pet levelstats

ALTER TABLE pet_levelstats ADD mindmg MEDIUMINT( 11 ) NOT NULL DEFAULT 0 COMMENT 'Min base damage' AFTER armor ,                                                                            
    ADD maxdmg MEDIUMINT( 11 ) NOT NULL DEFAULT 0 COMMENT 'Max base damage' AFTER mindmg,
    ADD attackpower MEDIUMINT( 11 ) NOT NULL DEFAULT 0 COMMENT 'Attack power' AFTER maxdmg;
