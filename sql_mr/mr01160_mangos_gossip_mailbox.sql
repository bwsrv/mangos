
-- mailbox option to pet's menu
DELETE FROM gossip_menu_option WHERE option_id = 19;
INSERT INTO gossip_menu_option (menu_id, id, option_icon, option_text, option_id, npc_option_npcflag, action_menu_id, action_poi_id, action_script_id, box_coded, box_money, box_text, cond_1, cond_1_val_1, cond_1_val_2, cond_2, cond_2_val_1, cond_2_val_2, cond_3, cond_3_val_1, cond_3_val_2) VALUES
(10318, 2, 0, 'Visit a mailbox.', 19, 1, -1, 0, 0, 0, 0, '', 11, 67334, 0, 20, 2817, 0, 0, 0, 0),
(33239, 2, 0, 'Visit a mailbox.', 19, 1, -1, 0, 0, 0, 0, '', 11, 67401, 0, 20, 2816, 0, 0, 0, 0);
