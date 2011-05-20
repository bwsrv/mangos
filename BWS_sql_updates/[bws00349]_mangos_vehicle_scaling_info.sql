DROP TABLE IF EXISTS vehicle_scaling_info;
CREATE TABLE vehicle_scaling_info (
  entry mediumint(8) unsigned NOT NULL default '0',
  baseItemLevel float NOT NULL default '0',
  scalingFactor float NOT NULL default '0',
  PRIMARY KEY (entry)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED;

INSERT INTO vehicle_scaling_info VALUES 
(336,170,0.01),
(335,170,0.01),
(338,170,0.01);