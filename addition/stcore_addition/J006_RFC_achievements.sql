-- Race Faction Change - Achievements

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `player_factionchange_achievements`
-- ----------------------------
DROP TABLE IF EXISTS `player_factionchange_achievements`;
CREATE TABLE `player_factionchange_achievements` (
  `alliance_id` int(8) NOT NULL,
  `horde_id` int(8) NOT NULL,
  `CommentA` varchar(255) NOT NULL,
  `CommentH` varchar(255) NOT NULL,
  PRIMARY KEY (`alliance_id`,`horde_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of player_factionchange_achievements
-- ----------------------------
INSERT INTO `player_factionchange_achievements` VALUES ('33', '1358', 'Complete 130 quests in Borean Tundra', 'Complete 150 quests in Borean Tundra');
INSERT INTO `player_factionchange_achievements` VALUES ('34', '1356', 'Complete 130 quests in Howling Fjord', 'Complete 105 quests in Howling Fjord');
INSERT INTO `player_factionchange_achievements` VALUES ('35', '1359', 'Complete 115 quests in Dragonblight', 'Complete 130 quests in Dragonblight');
INSERT INTO `player_factionchange_achievements` VALUES ('37', '1357', 'Complete 85 quests in Grizzly Hills', 'Complete 75 quests in Grizzly Hills');
INSERT INTO `player_factionchange_achievements` VALUES ('203', '1251', 'In a single Warsong Gulch battle, kill 2 flag carriers before they leave the Silverwing Flag Room', 'In a single Warsong Gulch battle, kill 2 flag carriers before they leave the Warsong Flag Room');
INSERT INTO `player_factionchange_achievements` VALUES ('220', '873', 'Win Alterac Valley without losing a tower or captain. You must also control all of the Horde\'s towers', 'Win Alterac Valley without losing a tower or captain. You must also control all of the Alliance\'s towers');
INSERT INTO `player_factionchange_achievements` VALUES ('433', '443', 'Earned the title, Grand Marshal', 'Earned the title, High Warlord');
INSERT INTO `player_factionchange_achievements` VALUES ('434', '445', 'Earned the title, Field Marshal', 'Earned the title, Warlord');
INSERT INTO `player_factionchange_achievements` VALUES ('435', '444', 'Earned the title, Commander', 'Earned the title, Lieutenant General');
INSERT INTO `player_factionchange_achievements` VALUES ('436', '447', 'Earned the title, Lieutenant Commander', 'Earned the title, Champion');
INSERT INTO `player_factionchange_achievements` VALUES ('437', '448', 'Earned the title, Knight-Champion', 'Earned the title, Centurion');
INSERT INTO `player_factionchange_achievements` VALUES ('438', '469', 'Earned the title, Knight-Captain', 'Earned the title, Legionnaire');
INSERT INTO `player_factionchange_achievements` VALUES ('439', '451', 'Earned the title, Knight', 'Earned the title, Stone Guard');
INSERT INTO `player_factionchange_achievements` VALUES ('440', '452', 'Earned the title, Sergeant Major', 'Earned the title, First Sergeant');
INSERT INTO `player_factionchange_achievements` VALUES ('441', '450', 'Earned the title, Master Sergeant', 'Earned the title, Senior Sergeant');
INSERT INTO `player_factionchange_achievements` VALUES ('442', '454', 'Earned the title, Private', 'Earned the title, Scout');
INSERT INTO `player_factionchange_achievements` VALUES ('470', '468', 'Earned the title, Corporal', 'Earned the title, Grunt');
INSERT INTO `player_factionchange_achievements` VALUES ('472', '449', 'Earned the title, Knight-Lieutenant', 'Earned the title, Blood Guard');
INSERT INTO `player_factionchange_achievements` VALUES ('473', '446', 'Earned the title, Marshal', 'Earned the title, General');
INSERT INTO `player_factionchange_achievements` VALUES ('604', '603', 'Kill 5 Horde players in each of the cities listed below', 'Kill 5 Alliance players in each of the cities listed below');
INSERT INTO `player_factionchange_achievements` VALUES ('610', '615', 'Kill Thrall', 'Kill King Varian Wrynn');
INSERT INTO `player_factionchange_achievements` VALUES ('611', '616', 'Kill Cairne Bloodhoof', 'Kill King Magni Bronzebeard');
INSERT INTO `player_factionchange_achievements` VALUES ('612', '617', 'Kill Lady Sylvanas Windrunner', 'Kill High Priestess Tyrande Whisperwind');
INSERT INTO `player_factionchange_achievements` VALUES ('613', '618', 'Kill Lor\'themar Theron', 'Kill Prophet Velen');
INSERT INTO `player_factionchange_achievements` VALUES ('614', '619', 'Slay the leaders of the Horde', 'Slay the leaders of the Alliance');
INSERT INTO `player_factionchange_achievements` VALUES ('701', '700', 'Obtain an Insignia or Medallion of the Alliance', 'Obtain an Insignia or Medallion of the Horde');
INSERT INTO `player_factionchange_achievements` VALUES ('707', '706', 'Obtain a Stormpike Battle Charger', 'Obtain a Frostwolf Howler');
INSERT INTO `player_factionchange_achievements` VALUES ('709', '708', 'Gain exalted reputation with the Stormpike Guard', 'Gain exalted reputation with the Frostwolf Clan');
INSERT INTO `player_factionchange_achievements` VALUES ('711', '710', 'Gain exalted reputation with The League of Arathor', 'Gain exalted reputation with The Forsaken Defilers');
INSERT INTO `player_factionchange_achievements` VALUES ('713', '712', 'Gain exalted reputation with the Silverwing Sentinels', 'Gain exalted reputation with the Warsong Outriders');
INSERT INTO `player_factionchange_achievements` VALUES ('899', '901', 'Earn exalted status with the Kurenai', 'Earn exalted status with The Mag\'har');
INSERT INTO `player_factionchange_achievements` VALUES ('942', '943', 'Raise your reputation level from unfriendly to exalted with Timbermaw Hold, Sporeggar and the Kurenai', 'Raise your reputation level from unfriendly to exalted with Timbermaw Hold, Sporeggar and The Mag\'har');
INSERT INTO `player_factionchange_achievements` VALUES ('1012', '1011', 'Gain exalted reputation with the Alliance Vanguard', 'Gain exalted reputation with the Horde Expedition');
INSERT INTO `player_factionchange_achievements` VALUES ('1028', '1031', 'Desecrate the Horde\'s bonfires in Eastern Kingdoms', 'Desecrate the Alliance\'s bonfires in Eastern Kingdoms');
INSERT INTO `player_factionchange_achievements` VALUES ('1029', '1032', 'Desecrate the Horde\'s bonfires in Kalimdor', 'Desecrate the Alliance\'s bonfires in Kalimdor');
INSERT INTO `player_factionchange_achievements` VALUES ('1030', '1033', 'Desecrate the Horde\'s bonfires in Outland', 'Desecrate the Alliance\'s bonfires in Outland');
INSERT INTO `player_factionchange_achievements` VALUES ('1034', '1036', 'Complete the Flame Warden of Eastern Kingdoms, Kalimdor and Outland achievements', 'Complete the Flame Keeper of Eastern Kingdoms, Kalimdor and Outland achievements');
INSERT INTO `player_factionchange_achievements` VALUES ('1040', '1041', 'Ruin Hallow\'s End for the Horde by completing Sergeant Hartman\'s quests which involve crashing the wickerman festival and cleaning up the stinkbombs from Southshore', 'Ruin Hallow\'s End for the Alliance by completing Darkcaller Yanka\'s quests which involve going to Southshore, ruining the kegs with rotten eggs and tossing stinkbombs into the town');
INSERT INTO `player_factionchange_achievements` VALUES ('1151', '224', 'In Alterac Valley, kill 50 enemy players in the Hall of the Stormpike', 'In Alterac Valley, kill 50 enemy players in the Hall of the Frostwolf');
INSERT INTO `player_factionchange_achievements` VALUES ('1189', '1271', 'Complete 80 quests in Hellfire Peninsula', 'Complete 90 quests in Hellfire Peninsula');
INSERT INTO `player_factionchange_achievements` VALUES ('1191', '1272', 'Complete 63 quests in Terokkar Forest', 'Complete 68 quests in Terokkar Forest');
INSERT INTO `player_factionchange_achievements` VALUES ('1192', '1273', 'Complete 75 quests in Nagrand', 'Complete 87 quests in Nagrand');
INSERT INTO `player_factionchange_achievements` VALUES ('1255', '259', 'Throw a snowball at King Magni Bronzebeard during the Feast of Winter Veil', 'Throw a snowball at Cairne Bloodhoof during the Feast of Winter Veil');
INSERT INTO `player_factionchange_achievements` VALUES ('1279', '1280', 'Get completely smashed, put on your best perfume, throw a handful of rose petals on Sraaz and then kiss him. You\'ll regret it in the morning', 'Get completely smashed, put on your best perfume, throw a handful of rose petals on Jeremiah Payson and then kiss him. You\'ll regret it in the morning');
INSERT INTO `player_factionchange_achievements` VALUES ('1676', '1677', 'Complete 700 quests in Eastern Kingdoms', 'Complete 550 quests in Eastern Kingdoms');
INSERT INTO `player_factionchange_achievements` VALUES ('1678', '1680', 'Complete 700 quests in Kalimdor', 'Complete 685 quests in Kalimdor');
INSERT INTO `player_factionchange_achievements` VALUES ('1686', '1685', 'Use Mistletoe on the Alliance Brothers during the Feast of Winter Veil', 'Use Mistletoe on the Horde Brothers during the Feast of Winter Veil');
INSERT INTO `player_factionchange_achievements` VALUES ('1782', '1783', 'Complete each of the cooking daily quests offered by Katherine Lee in Dalaran', 'Complete each of the cooking daily quests offered by Awilo Lon\'gomba in Dalaran');
INSERT INTO `player_factionchange_achievements` VALUES ('2421', '2420', 'Hide a Brightly Colored Egg in Stormwind City', 'Hide a Brightly Colored Egg in Silvermoon City');
INSERT INTO `player_factionchange_achievements` VALUES ('3556', '3557', 'Acquire the Spirit of Sharing from a complete Bountiful Table feast at every Alliance capital', 'Acquire the Spirit of Sharing from a complete Bountiful Table feast at every Horde capital');
