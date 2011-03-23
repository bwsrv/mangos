-- dungeon DBC encounters support
ALTER TABLE `instance` ADD COLUMN `encountersMask` 
INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Dungeon encounter bit mask'
AFTER `difficulty`;