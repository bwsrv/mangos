DROP TABLE IF EXISTS `creature_script_spawn`;
CREATE TABLE `creature_script_spawn` (
  `spawn_guid` int(10) unsigned NOT NULL auto_increment COMMENT 'Global Unique Identifier',
  `team_control` int(10) unsigned NOT NULL default '0',
  `add_control` int(10) unsigned NOT NULL default '0',
  `remove_control` int(10) unsigned NOT NULL default '0',
  `description` varchar(255) NOT NULL,
  PRIMARY KEY  (`spawn_guid`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ROW_FORMAT=DYNAMIC COMMENT='Script Spawn System';

DROP TABLE IF EXISTS `gameobject_script_spawn`;
CREATE TABLE `gameobject_script_spawn` (
  `spawn_guid` int(10) unsigned NOT NULL auto_increment COMMENT 'Global Unique Identifier',
  `team_control` int(10) unsigned NOT NULL default '0',
  `add_control` int(10) unsigned NOT NULL default '0',
  `remove_control` int(10) unsigned NOT NULL default '0',
  `description` varchar(255) NOT NULL,
  PRIMARY KEY  (`spawn_guid`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci ROW_FORMAT=DYNAMIC COMMENT='Script Spawn System';