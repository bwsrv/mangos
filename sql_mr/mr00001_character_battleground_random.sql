-- Commit 0525ca144e282cec2478
DROP TABLE IF EXISTS character_battleground_random;
CREATE TABLE character_battleground_random (
  guid int(11) unsigned NOT NULL default 0,
  PRIMARY KEY  (guid)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
