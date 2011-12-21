--
-- `vehicle_accessory`
--

CREATE TABLE IF NOT EXISTS `vehicle_accessory` (
    `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
    `accessory_entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
    `seat_id` mediumint(8) NOT NULL DEFAULT '0',
    `minion` tinyint(1) unsigned NOT NULL DEFAULT '0',
    `offset_x` FLOAT NOT NULL DEFAULT '0',
    `offset_y` FLOAT NOT NULL DEFAULT '0',
    `offset_z` FLOAT NOT NULL DEFAULT '0',
    `offset_o` FLOAT NOT NULL DEFAULT '0',
    `description` text NOT NULL,
    PRIMARY KEY (`entry`,`seat_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Vehicle Accessory System';

--
-- `vehicle_accessory`
--

REPLACE INTO `vehicle_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`) VALUES
(36891, 31260, 0, 0, 'Ymirjar Skycaller'),
(27626, 27627, 0, 1, 'Tatjana''s Horse'),
(28312, 28319, 7, 1, 'Wintergrasp Siege Engine'),
(32627, 32629, 7, 1, 'Wintergrasp Siege Engine'),
(32930, 32933, 0, 1, 'Kologarn'),
(32930, 32934, 1, 1, 'Kologarn'),
(33109, 33167, 1, 1, 'Salvaged Demolisher'),
(33109, 33620, 2, 1, 'Salvaged Demolisher'),
(33109, 33167, 3, 1, 'Salvaged Demolisher'),
(33060, 33067, 7, 1, 'Salvaged Siege Engine'),
(29931, 29982, 0, 0, 'Drakkari Rider on Drakkari Rhino'),
(33113, 33114, 2, 1, 'Flame Leviathan'),
(33113, 33114, 3, 1, 'Flame Leviathan'),
(33113, 33139, 7, 1, 'Flame Leviathan'),
(33114, 33142, 1, 1, 'Overload Control Device'),
(33114, 33143, 2, 1, 'Leviathan Defense Turret'),
(36678, 38309, 0, 1, 'Professor Putricide - trigger'),
(36678, 38308, 1, 1, 'Professor Putricide - trigger'),
(33214, 33218, 1, 1, 'Mechanolift 304-A'),
(35637, 34705, 0, 0, 'Marshal Jacob Alerius'' Mount'),
(35633, 34702, 0, 0, 'Ambrose Boltspark''s Mount'),
(35768, 34701, 0, 0, 'Colosos'' Mount'),
(34658, 34657, 0, 0, 'Jaelyne Evensong''s Mount'),
(35636, 34703, 0, 0, 'Lana Stouthammer''s Mount'),
(35638, 35572, 0, 0, 'Mokra the Skullcrusher''s Mount'),
(35635, 35569, 0, 0, 'Eressea Dawnsinger''s Mount'),
(35640, 35571, 0, 0, 'Runok Wildmane''s Mount'),
(35641, 35570, 0, 0, 'Zul''tore''s Mount'),
(35634, 35617, 0, 0, 'Deathstalker Visceri''s Mount'),
(27241, 27268, 0, 0, 'Risen Gryphon'),
(27661, 27662, 0, 0, 'Wintergarde Gryphon'),
(29698, 29699, 0, 0, 'Drakuru Raptor'),
(33778, 33780, 0, 1, 'Tournament Hippogryph'),
(33687, 33695, 0, 1, 'Chillmaw'),
(33687, 33695, 1, 1, 'Chillmaw'),
(33687, 33695, 2, 1, 'Chillmaw'),
(29625, 29694, 0, 0, 'Hyldsmeet Proto-Drake'),
(30330, 30332, 0, 0, 'Jotunheim Proto-Drake'),
(32189, 32190, 0, 0, 'Skybreaker Recon Fighter'),
(32640, 32642, 1, 0, 'Traveler Mammoth (H) - Vendor'),
(32640, 32641, 2, 0, 'Traveler Mammoth (H) - Vendor & Repairer'),
(32633, 32638, 1, 0, 'Traveler Mammoth (A) - Vendor'),
(32633, 32639, 2, 0, 'Traveler Mammoth (A) - Vendor & Repairer'),
(33669, 33666, 0, 0, 'Demolisher Engineer Blastwrench'),
(29555, 29556, 0, 0, 'Goblin Sapper'),
(28018, 28006, 0, 1, 'Thiassi the Light Bringer'),
(28054, 28053, 0, 0, 'Lucky Wilhelm - Apple'),
(28614, 28616, 0, 1, 'Scarlet Gryphon Rider'),
(36476, 36477, 0, 0, 'Krick and Ick'),
(36661, 36658, 0, 0, 'Scourgelord Tyrannus and Rimefang');

-- IOC (to be continued)
DELETE FROM `vehicle_accessory` WHERE entry IN (35069, 34776);
INSERT INTO `vehicle_accessory` (entry, accessory_entry, seat_id, minion, description) VALUES
(35069, 36355, 7, 1, 'Isle of Conquest Siege Engine - main turret (horde)'),
(35069, 34778, 1, 1, 'Isle of Conquest Siege Engine - flame turret 1 (horde)'),
(35069, 34778, 2, 1, 'Isle of Conquest Siege Engine - flame turret 2 (horde)'),
(34776, 34777, 7, 1, 'Isle of Conquest Siege Engine  - main turret (ally)'),
(34776, 36356, 1, 1, 'Isle of Conquest Siege Engine  - flame turret 1 (ally)'),
(34776, 36356, 2, 1, 'Isle of Conquest Siege Engine  - flame turret 2 (ally)');
