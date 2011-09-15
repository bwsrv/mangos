-- Race Faction Change - Reputations

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `player_factionchange_reputations`
-- ----------------------------
DROP TABLE IF EXISTS `player_factionchange_reputations`;

CREATE TABLE `player_factionchange_reputations` (
    `race_A` int(8) NOT NULL DEFAULT '0',
    `alliance_id` int(8) NOT NULL,
    `commentA` varchar(255) DEFAULT NULL,
    `race_H` int(8) NOT NULL DEFAULT '0',
    `horde_id` int(8) NOT NULL,
    `commentH` varchar(255) DEFAULT NULL,
    PRIMARY KEY (`race_A`, `alliance_id`, `race_H`, `horde_id`)
) DEFAULT CHARSET=UTF8;

-- ----------------------------
-- Records of player_factionchange_reputations
-- ----------------------------
INSERT INTO `player_factionchange_reputations` VALUES ('0', '47', 'Ironforge', '0', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '54', 'Gnomeregan Exiles', '0', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '69', 'Darnassus', '0', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '72', 'Stormwind', '0', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '509', 'The League of Arathor', '0', '510', 'The Defilers');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '730', 'Stormpike Guard', '0', '729', 'Frostwolf Clan');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '890', 'Silverwing Sentinels', '0', '889', 'Warsong Outriders');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '930', 'Exodar', '0', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '946', 'Honor Hold', '0', '947', 'Thrallmar');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '978', 'Kurenai', '0', '941', 'The Mag\'har');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '1037', 'Alliance Vanguard', '0', '1052', 'Horde Expedition');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '1050', 'Valiance Expedition', '0', '1085', 'Warsong Offensive');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '1068', 'Explorers\' League', '0', '1064', 'The Taunka');
INSERT INTO `player_factionchange_reputations` VALUES ('0', '1126', 'The Frostborn', '0', '1067', 'The Hand of Vengeance');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '69', 'Darnassus', '5', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '72', 'Stormwind', '5', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '54', 'Gnomeregan Exiles', '6', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '72', 'Stormwind', '6', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '47', 'Ironforge', '8', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '72', 'Stormwind', '8', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '930', 'Exodar', '10', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('1', '72', 'Stormwind', '10', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '47', 'Ironforge', '2', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '72', 'Stormwind', '2', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '69', 'Darnassus', '5', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '47', 'Ironforge', '5', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '54', 'Gnomeregan Exiles', '6', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '47', 'Ironforge', '6', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '930', 'Exodar', '10', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('3', '47', 'Ironforge', '10', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '69', 'Darnassus', '2', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '72', 'Stormwind', '2', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '69', 'Darnassus', '6', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '54', 'Gnomeregan Exiles', '6', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '69', 'Darnassus', '8', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '47', 'Ironforge', '8', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '69', 'Darnassus', '10', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('4', '930', 'Exodar', '10', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '54', 'Gnomeregan Exiles', '2', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '72', 'Stormwind', '2', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '69', 'Darnassus', '5', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '54', 'Gnomeregan Exiles', '5', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '54', 'Gnomeregan Exiles', '8', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '47', 'Ironforge', '8', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '930', 'Exodar', '10', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('7', '54', 'Gnomeregan Exiles', '10', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '930', 'Exodar', '2', '76', 'Orgrimmar');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '72', 'Stormwind', '2', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '69', 'Darnassus', '5', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '930', 'Exodar', '5', '68', 'Undercity');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '930', 'Exodar', '6', '81', 'Thunder Bluff');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '54', 'Gnomeregan Exiles', '6', '911', 'Silvermoon City');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '930', 'Exodar', '8', '530', 'Darkspear Trolls');
INSERT INTO `player_factionchange_reputations` VALUES ('11', '47', 'Ironforge', '8', '911', 'Silvermoon City');