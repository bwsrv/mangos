DROP TABLE IF EXISTS area_flags;
CREATE TABLE area_flags (
  area_id mediumint(6) NOT NULL DEFAULT '0',
  area_flag int(2) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY  (area_id)
) ENGINE = MyISAM DEFAULT CHARSET = utf8;
