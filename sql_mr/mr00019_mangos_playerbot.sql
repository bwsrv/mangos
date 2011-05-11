-- Playerbot
-- Commit bd270fbf52309c8b0a42

INSERT INTO `command`
    (`name`, `security`, `help`)
VALUES 
    ('bot',0,'Syntax:\r.bot add BOTNAME (add character to world)\r.bot remove BOTNAME (remove character from world)\r.bot co|combatorder BOTNAME COMBATORDER [TARGET]');

INSERT INTO `gossip_menu_option`
    (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `cond_1`, `cond_1_val_1`, `cond_1_val_2`, `cond_2`, `cond_2_val_1`, `cond_2_val_2`, `cond_3`, `cond_3_val_1`, `cond_3_val_2`)
VALUES
    ('0','16','0','GOSSIP_OPTION_BOT','18','1','0','0','0','0','0',NULL,'0','0','0','0','0','0','0','0','0');
