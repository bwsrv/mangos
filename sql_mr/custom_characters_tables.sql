-- Random Battleground
-- Commit 0525ca144e282cec2478

DROP TABLE IF EXISTS `character_battleground_random`;
CREATE TABLE `character_battleground_random` (
    `guid` int(11) unsigned NOT NULL default 0,
    PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Anticheat tables from /dev/rsa

DROP TABLE IF EXISTS `anticheat_config`;
CREATE TABLE `anticheat_config` (
    `checktype` mediumint(8) unsigned NOT NULL COMMENT 'Type of check',
    `description` varchar(255),
    `check_period` int(11) unsigned NOT NULL default '0' COMMENT 'Time period of check, in ms, 0 - always',
    `alarmscount` int(11) unsigned NOT NULL default '1'COMMENT 'Count of alarms before action',
    `disableoperation` tinyint(3) unsigned NOT NULL default '0'COMMENT 'Anticheat disable operations in main core code after check fail',
    `messagenum` int(11) NOT NULL default '0' COMMENT 'Number of system message',
    `intparam1` mediumint(8) NOT NULL default '0' COMMENT 'Int parameter 1',
    `intparam2` mediumint(8) NOT NULL default '0' COMMENT 'Int parameter 2',
    `floatparam1` float NOT NULL default '0' COMMENT 'Float parameter 1',
    `floatparam2` float NOT NULL default '0' COMMENT 'Float parameter 2',
    `action1` mediumint(8) NOT NULL default '0' COMMENT 'Action 1',
    `actionparam1` mediumint(8) NOT NULL default '0' COMMENT 'Action parameter 1',
    `action2` mediumint(8) NOT NULL default '0' COMMENT 'Action 1',
    `actionparam2` mediumint(8) NOT NULL default '0' COMMENT 'Action parameter 1',
    PRIMARY KEY (`checktype`)
) DEFAULT CHARSET=utf8 PACK_KEYS=0 COMMENT='Anticheat configuration';

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `anticheat_log` (
    `playername` varchar(32) NOT NULL,
    `checktype` mediumint(8) unsigned NOT NULL,
    `alarm_time` datetime NOT NULL,
    `reason` varchar(255) NOT NULL DEFAULT 'Unknown',
    `guid` int(11) unsigned NOT NULL,
    `action1` mediumint(8) NOT NULL default '0',
    `action2` mediumint(8) NOT NULL default '0',
    PRIMARY KEY (`checktype`, `alarm_time`, `guid`),
    KEY idx_Player (`guid`)
) DEFAULT CHARSET=utf8 COMMENT='Anticheat log table';

-- Anticheat
-- Config

-- Main checks
REPLACE INTO `anticheat_config`
    (`checktype`, `description`, `check_period`, `alarmscount`, `disableoperation`, `messagenum`, `intparam1`, `intparam2`, `floatparam1`, `floatparam2`, `action1`, `actionparam1`, `action2`, `actionparam2`)
VALUES
    -- Main checks
    (0, "Null check",         0, 1, 0, 11000, 0, 0,    0, 0, 1, 0, 0, 0),
    (1, "Movement cheat",     0, 1, 0, 11000, 0, 0,    0, 0, 2, 1, 0, 0),
    (2, "Spell cheat",        0, 1, 0, 11000, 0, 0,    0, 0, 2, 1, 0, 0),
    (3, "Quest cheat",        0, 1, 0, 11000, 0, 0,    0, 0, 2, 1, 0, 0),
    (4, "Transport cheat",    0, 3, 0, 11000, 0, 0, 60.0, 0, 2, 1, 0, 0),
    (5, "Damage cheat",       0, 1, 0, 11000, 0, 0,    0, 0, 2, 1, 0, 0),
    (6, "Item cheat",         0, 1, 0, 11000, 0, 0,    0, 0, 2, 1, 0, 0),
    (7, "Warden check",       0, 1, 0, 11000, 0, 0,    0, 0, 2, 1, 0, 0);

-- Subchecks
REPLACE INTO `anticheat_config`
    (`checktype`, `description`, `check_period`, `alarmscount`, `disableoperation`, `messagenum`, `intparam1`, `intparam2`, `floatparam1`, `floatparam2`, `action1`, `actionparam1`, `action2`, `actionparam2`)
VALUES
    (101, "Speed hack",              500, 5, 0, 11000,    10000, 0, 0.0012,    0, 2, 1, 6, 20000),
    (102, "Fly hack",                500, 5, 0, 11000,    20000, 0,   10.0,    0, 2, 1, 0, 0),
    (103, "Wall climb hack",         500, 2, 0, 11000,    10000, 0, 0.0015, 2.37, 2, 1, 0, 0),
    (104, "Waterwalking hack",      1000, 3, 0, 11000,    20000, 0,      0,    0, 2, 1, 0, 0),
    (105, "Teleport to plane hack",  500, 1, 0, 11000,        0, 0, 0.0001,  0.1, 2, 1, 0, 0),
    (106, "AirJump hack" ,           500, 3, 0, 11000,    30000, 0,   10.0, 25.0, 2, 1, 0, 0),
    (107, "Teleport hack" ,            0, 1, 0, 11000,        0, 0,   50.0,    0, 2, 1, 0, 0),
    (108, "Fall hack" ,                0, 3, 0, 11000,    10000, 0,      0,    0, 2, 1, 0, 0),
    (109, "Z Axis hack" ,              0, 1, 0, 11000,        0, 0,0.00001, 10.0, 2, 1, 0, 0),
    (201, "Spell invalid",             0, 1, 0, 11000,        0, 0,      0,    0, 2, 1, 0, 0),
    (202, "Spellcast in dead state",   0, 1, 0, 11000,        0, 0,      0,    0, 2, 1, 0, 0),
    (203, "Spell not valid for player",0, 1, 0, 11000,        0, 0,      0,    0, 2, 1, 0, 0),
    (204, "Spell not in player book",  0, 1, 0, 11000,        0, 0,      0,    0, 2, 1, 0, 0),
    (501, "Spell damage hack",         0, 1, 0, 11000,        0, 50000,  0,    0, 2, 1, 0, 0),
    (502, "Melee damage hack",         0, 1, 0, 11000,        0, 50000,  0,    0, 2, 1, 0, 0),
    (601, "Item dupe hack",            0, 1, 0, 11000,        0,     0,  0,    0, 2, 1, 0, 0),
    (701, "Warden memory check",       0, 1, 0, 11000,        0,     0,  0,    0, 2, 1, 0, 0),
    (702, "Warden key check",          0, 1, 0, 11000,        0,     0,  0,    0, 2, 1, 0, 0),
    (703, "Warden checksum check",     0, 1, 0, 11000,        0,     0,  0,    0, 2, 1, 0, 0),
    (704, "Warden timeout check",      0, 1, 0, 11000,        0,     0,  0,    0, 2, 1, 0, 0);


-- Auctionhouse Bot
-- Commit 7c97114488b36b4427ed

DROP TABLE IF EXISTS `auctionhousebot`;
CREATE TABLE `auctionhousebot` (
    `auctionhouse` int(11) NOT NULL default '0' COMMENT 'mapID of the auctionhouse.',
    `name` char(25) default NULL COMMENT 'Text name of the auctionhouse.',
    `minitems` int(11) default '0' COMMENT 'This is the minimum number of items you want to keep in the auction house. a 0 here will make it the same as the maximum.',
    `maxitems` int(11) default '0' COMMENT 'This is the number of items you want to keep in the auction house.',
    `mintime` int(11) default '8' COMMENT 'Sets the minimum number of hours for an auction.',
    `maxtime` int(11) default '24' COMMENT 'Sets the maximum number of hours for an auction.',
    `percentgreytradegoods` int(11) default '0' COMMENT 'Sets the percentage of the Grey Trade Goods auction items',
    `percentwhitetradegoods` int(11) default '27' COMMENT 'Sets the percentage of the White Trade Goods auction items',
    `percentgreentradegoods` int(11) default '12' COMMENT 'Sets the percentage of the Green Trade Goods auction items',
    `percentbluetradegoods` int(11) default '10' COMMENT 'Sets the percentage of the Blue Trade Goods auction items',
    `percentpurpletradegoods` int(11) default '1' COMMENT 'Sets the percentage of the Purple Trade Goods auction items',
    `percentorangetradegoods` int(11) default '0' COMMENT 'Sets the percentage of the Orange Trade Goods auction items',
    `percentyellowtradegoods` int(11) default '0' COMMENT 'Sets the percentage of the Yellow Trade Goods auction items',
    `percentgreyitems` int(11) default '0' COMMENT 'Sets the percentage of the non trade Grey auction items',
    `percentwhiteitems` int(11) default '10' COMMENT 'Sets the percentage of the non trade White auction items',
    `percentgreenitems` int(11) default '30' COMMENT 'Sets the percentage of the non trade Green auction items',
    `percentblueitems` int(11) default '8' COMMENT 'Sets the percentage of the non trade Blue auction items',
    `percentpurpleitems` int(11) default '2' COMMENT 'Sets the percentage of the non trade Purple auction items',
    `percentorangeitems` int(11) default '0' COMMENT 'Sets the percentage of the non trade Orange auction items',
    `percentyellowitems` int(11) default '0' COMMENT 'Sets the percentage of the non trade Yellow auction items',
    `minpricegrey` int(11) default '100' COMMENT 'Minimum price of Grey items (percentage).',
    `maxpricegrey` int(11) default '150' COMMENT 'Maximum price of Grey items (percentage).',
    `minpricewhite` int(11) default '150' COMMENT 'Minimum price of White items (percentage).',
    `maxpricewhite` int(11) default '250' COMMENT 'Maximum price of White items (percentage).',
    `minpricegreen` int(11) default '800' COMMENT 'Minimum price of Green items (percentage).',
    `maxpricegreen` int(11) default '1400' COMMENT 'Maximum price of Green items (percentage).',
    `minpriceblue` int(11) default '1250' COMMENT 'Minimum price of Blue items (percentage).',
    `maxpriceblue` int(11) default '1750' COMMENT 'Maximum price of Blue items (percentage).',
    `minpricepurple` int(11) default '2250' COMMENT 'Minimum price of Purple items (percentage).',
    `maxpricepurple` int(11) default '4550' COMMENT 'Maximum price of Purple items (percentage).',
    `minpriceorange` int(11) default '3250' COMMENT 'Minimum price of Orange items (percentage).',
    `maxpriceorange` int(11) default '5550' COMMENT 'Maximum price of Orange items (percentage).',
    `minpriceyellow` int(11) default '5250' COMMENT 'Minimum price of Yellow items (percentage).',
    `maxpriceyellow` int(11) default '6550' COMMENT 'Maximum price of Yellow items (percentage).',
    `minbidpricegrey` int(11) default '70' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 70',
    `maxbidpricegrey` int(11) default '100' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 100',
    `minbidpricewhite` int(11) default '70' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 70',
    `maxbidpricewhite` int(11) default '100' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 100',
    `minbidpricegreen` int(11) default '80' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 80',
    `maxbidpricegreen` int(11) default '100' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 100',
    `minbidpriceblue` int(11) default '75' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 75',
    `maxbidpriceblue` int(11) default '100' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 100',
    `minbidpricepurple` int(11) default '80' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 80',
    `maxbidpricepurple` int(11) default '100' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 100',
    `minbidpriceorange` int(11) default '80' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 80',
    `maxbidpriceorange` int(11) default '100' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 100',
    `minbidpriceyellow` int(11) default '80' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 80',
    `maxbidpriceyellow` int(11) default '100' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 100',
    `maxstackgrey` int(11) default '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `maxstackwhite` int(11) default '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `maxstackgreen` int(11) default '3' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `maxstackblue` int(11) default '2' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `maxstackpurple` int(11) default '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `maxstackorange` int(11) default '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `maxstackyellow` int(11) default '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
    `buyerpricegrey` int(11) default '1' COMMENT 'Multiplier to vendorprice when buying grey items from auctionhouse',
    `buyerpricewhite` int(11) default '1' COMMENT 'Multiplier to vendorprice when buying white items from auctionhouse',
    `buyerpricegreen` int(11) default '5' COMMENT 'Multiplier to vendorprice when buying green items from auctionhouse',
    `buyerpriceblue` int(11) default '12' COMMENT 'Multiplier to vendorprice when buying blue items from auctionhouse',
    `buyerpricepurple` int(11) default '15' COMMENT 'Multiplier to vendorprice when buying purple items from auctionhouse',
    `buyerpriceorange` int(11) default '20' COMMENT 'Multiplier to vendorprice when buying orange items from auctionhouse',
    `buyerpriceyellow` int(11) default '22' COMMENT 'Multiplier to vendorprice when buying yellow items from auctionhouse',
    `buyerbiddinginterval` int(11) default '1' COMMENT 'Interval how frequently AHB bids on each AH. Time in minutes',
    `buyerbidsperinterval` int(11) default '1' COMMENT 'number of bids to put in per bidding interval',
    PRIMARY KEY  (`auctionhouse`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DELETE FROM `auctionhousebot` WHERE (`auctionhouse`='2') OR (`auctionhouse`='6') OR (`auctionhouse`='7');
INSERT INTO `auctionhousebot`
    (`auctionhouse`,`name`,`minitems`,`maxitems`,`mintime`,`maxtime`,`percentgreytradegoods`,`percentwhitetradegoods`,`percentgreentradegoods`,`percentbluetradegoods`,`percentpurpletradegoods`,`percentorangetradegoods`,`percentyellowtradegoods`,`percentgreyitems`,`percentwhiteitems`,`percentgreenitems`,`percentblueitems`,`percentpurpleitems`,`percentorangeitems`,`percentyellowitems`,`minpricegrey`,`maxpricegrey`,`minpricewhite`,`maxpricewhite`,`minpricegreen`,`maxpricegreen`,`minpriceblue`,`maxpriceblue`,`minpricepurple`,`maxpricepurple`,`minpriceorange`,`maxpriceorange`,`minpriceyellow`,`maxpriceyellow`,`minbidpricegrey`,`maxbidpricegrey`,`minbidpricewhite`,`maxbidpricewhite`,`minbidpricegreen`,`maxbidpricegreen`,`minbidpriceblue`,`maxbidpriceblue`,`minbidpricepurple`,`maxbidpricepurple`,`minbidpriceorange`,`maxbidpriceorange`,`minbidpriceyellow`,`maxbidpriceyellow`,`maxstackgrey`,`maxstackwhite`,`maxstackgreen`,`maxstackblue`,`maxstackpurple`,`maxstackorange`,`maxstackyellow`,`buyerpricegrey`,`buyerpricewhite`,`buyerpricegreen`,`buyerpriceblue`,`buyerpricepurple`,`buyerpriceorange`,`buyerpriceyellow`,`buyerbiddinginterval`,`buyerbidsperinterval`)
VALUES
    (2,'Alliance',0,0,8,24,0,27,12,10,1,0,0,0,10,30,8,2,0,0,100,150,150,250,800,1400,1250,1750,2250,4550,3250,5550,5250,6550,70,100,70,100,80,100,75,100,80,100,80,100,80,100,0,0,3,2,1,1,1,1,1,5,12,15,20,22,1,1),
    (6,'Horde',0,0,8,24,0,27,12,10,1,0,0,0,10,30,8,2,0,0,100,150,150,250,800,1400,1250,1750,2250,4550,3250,5550,5250,6550,70,100,70,100,80,100,75,100,80,100,80,100,80,100,0,0,3,2,1,1,1,1,1,5,12,15,20,22,1,1),
    (7,'Neutral',0,0,8,24,0,27,12,10,1,0,0,0,10,30,8,2,0,0,100,150,150,250,800,1400,1250,1750,2250,4550,3250,5550,5250,6550,70,100,70,100,80,100,75,100,80,100,80,100,80,100,0,0,3,2,1,1,1,1,1,5,12,15,20,22,1,1);

-- Autobroadcast
-- Commit 72d1f7a22d13399135d0

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `autobroadcast` (
  `id` int(11) NOT NULL auto_increment,
  `text` longtext NOT NULL,
  `next` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

-- BOP item trade

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `item_soulbound_trade_data` (
    `itemGuid` int(16) unsigned NOT NULL DEFAULT '0',
    `allowedPlayers` varchar(255) NOT NULL DEFAULT '',
    PRIMARY KEY (`itemGuid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='BOP item trade cache';

-- Wow Armory

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `armory_character_stats` (
    `guid` int(11) NOT NULL,
    `data` longtext NOT NULL,
    PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='World of Warcraft Armory table';

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `armory_character_feed_log` (
    `guid` int(11) NOT NULL,
    `type` smallint(1) NOT NULL,
    `data` int(11) NOT NULL,
    `date` int(11) default NULL,
    `counter` int(11) NOT NULL,
    `difficulty` smallint(6) default '-1',
    `item_guid` int(11) default '-1',
    PRIMARY KEY (`guid`, `type`, `data`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='World of Warcraft Armory feed log';

  -- better not drop table here, because of custom data
CREATE TABLE IF NOT EXISTS `armory_game_chart` (
    `gameid` int(11) NOT NULL,
    `teamid` int(11) NOT NULL,
    `guid` int(11) NOT NULL,
    `changeType` int(11) NOT NULL,
    `ratingChange` int(11) NOT NULL,
    `teamRating` int(11) NOT NULL,
    `damageDone` int(11) NOT NULL,
    `deaths` int(11) NOT NULL,
    `healingDone` int(11) NOT NULL,
    `damageTaken` int(11) NOT NULL,
    `healingTaken` int(11) NOT NULL,
    `killingBlows` int(11) NOT NULL,
    `mapId` int(11) NOT NULL,
    `start` int(11) NOT NULL,
    `end` int(11) NOT NULL,
    PRIMARY KEY  (`gameid`, `teamid`, `guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='WoWArmory Game Chart';

-- Group flags and roles support
-- Commit a5e57729fc5211bb6a2f

ALTER TABLE `group_member`
    ADD COLUMN  `roles` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member roles bit mask' AFTER `subgroup`;

ALTER TABLE `group_member`
    CHANGE `assistant` `memberFlags` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member flags bit mask';

ALTER TABLE `groups`
    DROP `mainTank`,
    DROP `mainAssistant`;

-- dungeon DBC encounters support

ALTER TABLE `instance`
    ADD COLUMN `encountersMask` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Dungeon encounter bit mask' AFTER `difficulty`;

-- Instance Extend LFG
-- Commit 020d4e346d38b961bf62

ALTER TABLE `character_instance`
    ADD COLUMN `extend` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Instance extend (bool)' AFTER `permanent`;

-- Refer a friend

ALTER TABLE `characters`
    ADD COLUMN `grantableLevels` tinyint(3) unsigned NOT NULL default '0' AFTER `actionBars`;

-- Saved Variables
-- Commit 0525ca144e282cec2478

ALTER TABLE `saved_variables`
    ADD COLUMN `NextRandomBGResetTime` bigint(40) unsigned NOT NULL default 0 AFTER `NextWeeklyQuestResetTime`;

-- Pet dualspec for hunter pets (mangosR2 repo)

ALTER TABLE `pet_spell`
    ADD COLUMN `spec` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `active`,
    DROP PRIMARY KEY,
    ADD PRIMARY KEY (`guid`, `spell`, `spec`);
	
-- Pet table cleanup
ALTER TABLE `character_pet`
  DROP `resettalents_cost`,
  DROP `resettalents_time`;

DROP TABLE IF EXISTS hidden_rating;
CREATE TABLE IF NOT EXISTS hidden_rating (
    guid INT(11) UNSIGNED NOT NULL,
    rating2 INT(10) UNSIGNED NOT NULL,
    rating3 INT(10) UNSIGNED NOT NULL,
    rating5 INT(10) UNSIGNED NOT NULL,
    PRIMARY KEY  (guid)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

-- ADVANCE CHARACTERS TABLE

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `character_stats`
-- ----------------------------
DROP TABLE IF EXISTS `character_stats`;
CREATE TABLE `character_stats` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier, Low part',
  `maxhealth` int(10) unsigned NOT NULL default '0',
  `maxpower1` int(10) unsigned NOT NULL default '0',
  `maxpower2` int(10) unsigned NOT NULL default '0',
  `maxpower3` int(10) unsigned NOT NULL default '0',
  `maxpower4` int(10) unsigned NOT NULL default '0',
  `maxpower5` int(10) unsigned NOT NULL default '0',
  `maxpower6` int(10) unsigned NOT NULL default '0',
  `maxpower7` int(10) unsigned NOT NULL default '0',
  `strength` int(10) unsigned NOT NULL default '0',
  `agility` int(10) unsigned NOT NULL default '0',
  `stamina` int(10) unsigned NOT NULL default '0',
  `intellect` int(10) unsigned NOT NULL default '0',
  `spirit` int(10) unsigned NOT NULL default '0',
  `armor` int(10) unsigned NOT NULL default '0',
  `resHoly` int(10) unsigned NOT NULL default '0',
  `resFire` int(10) unsigned NOT NULL default '0',
  `resNature` int(10) unsigned NOT NULL default '0',
  `resFrost` int(10) unsigned NOT NULL default '0',
  `resShadow` int(10) unsigned NOT NULL default '0',
  `resArcane` int(10) unsigned NOT NULL default '0',
  `blockPct` float unsigned NOT NULL default '0',
  `dodgePct` float unsigned NOT NULL default '0',
  `parryPct` float unsigned NOT NULL default '0',
  `critPct` float unsigned NOT NULL default '0',
  `rangedCritPct` float unsigned NOT NULL default '0',
  `spellCritPct` float unsigned NOT NULL default '0',
  `attackPower` int(10) unsigned NOT NULL default '0',
  `rangedAttackPower` int(10) unsigned NOT NULL default '0',
  `spellPower` int(10) unsigned NOT NULL default '0',
  `apmelee` int(11) NOT NULL,
  `ranged` int(11) NOT NULL,
  `blockrating` int(11) NOT NULL,
  `defrating` int(11) NOT NULL,
  `dodgerating` int(11) NOT NULL,
  `parryrating` int(11) NOT NULL,
  `resilience` int(11) NOT NULL,
  `manaregen` float NOT NULL,
  `melee_hitrating` int(11) NOT NULL,
  `melee_critrating` int(11) NOT NULL,
  `melee_hasterating` int(11) NOT NULL,
  `melee_mainmindmg` float NOT NULL,
  `melee_mainmaxdmg` float NOT NULL,
  `melee_offmindmg` float NOT NULL,
  `melee_offmaxdmg` float NOT NULL,
  `melee_maintime` float NOT NULL,
  `melee_offtime` float NOT NULL,
  `ranged_critrating` int(11) NOT NULL,
  `ranged_hasterating` int(11) NOT NULL,
  `ranged_hitrating` int(11) NOT NULL,
  `ranged_mindmg` float NOT NULL,
  `ranged_maxdmg` float NOT NULL,
  `ranged_attacktime` float NOT NULL,
  `spell_hitrating` int(11) NOT NULL,
  `spell_critrating` int(11) NOT NULL,
  `spell_hasterating` int(11) NOT NULL,
  `spell_bonusdmg` int(11) NOT NULL,
  `spell_bonusheal` int(11) NOT NULL,
  `spell_critproc` float NOT NULL,
  `account` int(11) unsigned NOT NULL default '0',
  `name` varchar(12) NOT NULL default '',
  `race` tinyint(3) unsigned NOT NULL default '0',
  `class` tinyint(3) unsigned NOT NULL default '0',
  `gender` tinyint(3) unsigned NOT NULL default '0',
  `level` tinyint(3) unsigned NOT NULL default '0',
  `map` int(11) unsigned NOT NULL default '0',
  `money` int(10) unsigned NOT NULL default '0',
  `totaltime` int(11) unsigned NOT NULL default '0',
  `online` int(10) unsigned NOT NULL default '0',
  `arenaPoints` int(10) unsigned NOT NULL default '0',
  `totalHonorPoints` int(10) unsigned NOT NULL default '0',
  `totalKills` int(10) unsigned NOT NULL default '0',
  `equipmentCache` longtext NOT NULL,
  `specCount` tinyint(3) unsigned NOT NULL default '1',
  `activeSpec` tinyint(3) unsigned NOT NULL default '0',
  `data` longtext NOT NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

