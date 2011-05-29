-- Refer a friend

  -- better not to drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `account_friends` (
    `id` int(11) unsigned NOT NULL default '0',
    `friend_id` int(11) unsigned NOT NULL default '0',
    `bind_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'Bring date',
    `expire_date` datetime NOT NULL DEFAULT 0 COMMENT 'Expire date',
    PRIMARY KEY (`id`, `friend_id`)
) DEFAULT CHARSET=utf8 PACK_KEYS=0 COMMENT='Stores accounts for refer-a-friend system.';

-- Warden data table structure from TOM_RUS
DROP TABLE IF EXISTS `warden_data_result`;
CREATE TABLE `warden_data_result` (
  `id` int(4) NOT NULL auto_increment,
  `check` int(3) default NULL,
  `data` tinytext,
  `str` tinytext,
  `address` int(8) default NULL,
  `length` int(2) default NULL,
  `result` tinytext,
  `comment` text,
  PRIMARY KEY  (`id`)
) DEFAULT CHARSET=utf8;
