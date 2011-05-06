-- New Vendor
DELETE FROM creature_template WHERE entry = 99321;
INSERT INTO creature_template VALUES
(99321, 0, 0, 0, 0, 0, 25811, 0, 25811, 0, 'Pandora receller', 'Moneychanger', '', 0, 80, 80, 22000, 22000, 0, 0, 9730, 2007, 2007, 128, 1, 1.14286, 1, 0, 420, 630, 0, 158, 1, 2000, 2000, 1, 33282, 0, 0, 0, 0, 0, 0, 336, 504, 126, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, '');

DELETE FROM locales_creature WHERE entry = 99321;
INSERT INTO locales_creature VALUES
(99321, 'Pandora reseller', '', '', '', '', '', '', 'Перекупщик Пандоры', 'Moneychanger', NULL, NULL, NULL, NULL, NULL, NULL, 'Меняла');

DELETE FROM creature WHERE guid = 99321;
INSERT INTO creature  VALUES (99321, 99321, 571, 1, 1, 25811, 0, 5809.08, 623.377, 647.744, 2.0217, 600, 0, 0, 22000, 0, 0, 0);

-- Items
DELETE FROM npc_vendor WHERE entry = 99321;
INSERT INTO npc_vendor VALUES (99321, 41596, 0, 0 ,2707);
INSERT INTO npc_vendor VALUES (99321, 47241, 0, 0 ,1909);
INSERT INTO npc_vendor VALUES (99321, 44990, 0, 0 ,2707);
INSERT INTO npc_vendor VALUES (99321, 43228, 0, 0 ,2707);
INSERT INTO npc_vendor VALUES (99321, 47241, 0, 0 ,2589);
INSERT INTO npc_vendor VALUES (99321, 47241, 0, 0 ,2637);
INSERT INTO npc_vendor VALUES (99321, 47241, 0, 0 ,2743);
INSERT INTO npc_vendor VALUES (99321, 49426, 0, 0 ,2723);
INSERT INTO npc_vendor VALUES (99321, 43589, 0, 0 ,2707);
INSERT INTO npc_vendor VALUES (99321, 49908, 0, 0 ,2723);
-- INSERT INTO npc_vendor VALUES (99321, ??, 0, 0 ,2707); //1 эмблема триумфа
-- INSERT INTO npc_vendor VALUES (99321, ??, 0, 0 ,2723); //15 эмблем триумфа
