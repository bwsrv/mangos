--
DELETE FROM `command` WHERE `name` = 'bot'; 
INSERT INTO `command` (`name`,`security`,`help`) VALUES 
('bot',0,'Syntax:\r.bot add BOTNAME (add character to world)\r.bot remove BOTNAME (remove character from world)\r.bot co|combatorder BOTNAME COMBATORDER [TARGET]');
--
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 0 AND `id` = 16 AND `option_id` = 18;
