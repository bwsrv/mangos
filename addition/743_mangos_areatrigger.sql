-- Areatrigger table format change

ALTER TABLE `areatrigger_teleport` CHANGE `required_quest_done` `required_quest_done_A` int(11) UNSIGNED NOT NULL DEFAULT "0"
COMMENT "Alliance quest";

ALTER TABLE `areatrigger_teleport` CHANGE `required_quest_done_heroic` `required_quest_done_heroic_A` int(11) UNSIGNED NOT NULL DEFAULT "0"
COMMENT "Alliance heroic quest";

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`required_quest_done_H` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Horde quest"
AFTER `required_quest_done_A`;

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`required_quest_done_heroic_H` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Horde heroic quest"
AFTER `required_quest_done_heroic_A`;

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`minGS` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Min player gear score";

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`maxGS` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Max player gear score";

UPDATE `areatrigger_teleport` SET `required_quest_done_H` = `required_quest_done_A`
WHERE `required_quest_done_A` > 0;

UPDATE `areatrigger_teleport` SET `required_quest_done_heroic_H` = `required_quest_done_heroic_A`
WHERE `required_quest_done_heroic_A` > 0;
