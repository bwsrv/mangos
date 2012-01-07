ALTER TABLE `characters` ADD COLUMN `RP_model` mediumint(8) unsigned NOT NULL default '0' AFTER `grantableLevels`;
ALTER TABLE `characters` ADD COLUMN `RP_scale` float NULL default '1' AFTER `RP_model`;
ALTER TABLE `characters` ADD COLUMN `RP_speed_run` float NOT NULL default '1' AFTER `RP_scale`;
