-- Strand of the Ancients
DELETE FROM battleground_template WHERE id = 9;
INSERT INTO battleground_template (id, MinPlayersPerTeam, MaxPlayersPerTeam, AllianceStartLoc, AllianceStartO, HordeStartLoc, HordeStartO) VALUES (9, 5, 15, 1367, 0, 1368, 0);
-- Rigger Sparklight
UPDATE creature_template SET npcflag = 1, ScriptName = 'npc_sa_vendor' WHERE entry IN (29260, 29262);
-- Seaforium source
-- left this to remove old bad hack
-- DELETE FROM creature_template WHERE entry = 50000;
-- modify some "should not be changed" data in DB
-- dummy radius for passing some check, only 1 charge and 10 seconds delay
UPDATE gameobject_template SET data2 = 1, data4 = 1, data7 = 10 WHERE entry = 190752;
-- set factions for 2 Massive Seaforium Bombs
-- use these if needed in Your DB (maybe already have better factions?)
-- UPDATE `gameobject_template` SET faction = 3 WHERE entry = 190753;
-- UPDATE `gameobject_template` SET faction = 6 WHERE entry = 194086;

-- Doors
UPDATE gameobject_template SET faction = 14 WHERE entry IN (192549, 190727, 190726, 190723, 190724, 190722);
-- make Titan Relic clickable (hack, must be done via core, when door to Chamber of Ancient Relics is destroyed)
UPDATE gameobject_template SET flags=flags&~16 WHERE entry = 192829;
-- make Defender's Portal usable by all factions (rest is handled by script)
UPDATE gameobject_template SET faction=0 , ScriptName = 'go_sa_def_portal' WHERE entry = 191575;

-- Seaforium barrels
DELETE FROM gameobject_template WHERE entry IN (190753, 194086);
INSERT INTO gameobject_template (entry, type, displayId, name, IconName, castBarCaption, unk1, faction, flags, size, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10, data11, data12, data13, data14, data15, data16, data17, data18, data19, data20, data21, data22, data23, ScriptName) VALUES
(190753, 22, 8582, 'Seaforium Barrel', '', '', '', 12, 0, 2, 0, 0, 0, 0, 0, 0, 52415, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(194086, 22, 8582, 'Seaforium Barrel', '', '', '', 29, 0, 2, 0, 0, 0, 0, 0, 0, 52415, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE locales_gameobject SET name_loc3 = 'Zephyriumbombe', name_loc6 = 'Barril de Seforio', name_loc7 = 'Barril de Seforio' WHERE entry IN (190753, 194086);

-- Banners - horde/alli occupied, not clickable banner
UPDATE gameobject_template SET flags = 4 WHERE entry IN (191309, 191310);

-- Vehicles support
-- Battleground Demolisher
UPDATE creature_template SET npcflag = 1, minlevel = 70, maxlevel = 70, minhealth = 80000, maxhealth = 80000, spell1 = 52338, spell2 = 60206, mechanic_immune_mask = mechanic_immune_mask|1|2|8|16|32|64|128|1024|2048|4096|8192|131072|262144|8388608|16777216|67108864, ScriptName = 'npc_sa_demolisher', RegenHealth = 0 WHERE entry = 28781;
UPDATE creature_template SET npcflag = 1, minlevel = 80, maxlevel = 80, minhealth = 80000, maxhealth = 80000, spell1 = 52338, spell2 = 60206, mechanic_immune_mask = mechanic_immune_mask|1|2|8|16|32|64|128|1024|2048|4096|8192|131072|262144|8388608|16777216|67108864, RegenHealth = 0, ScriptName = '' WHERE entry = 32796;
DELETE FROM npc_spellclick_spells WHERE npc_entry IN (28781, 32796);
INSERT INTO npc_spellclick_spells VALUES
(28781, 60968, 0, 0, 0, 0),
(32796, 60968, 0, 0, 0, 0);
-- Demolisher not must heal's
DELETE FROM creature_template_addon WHERE entry IN (28781, 32796);
INSERT INTO creature_template_addon  VALUES 
(28781, 0, 0, 0, 0, 0, 0, 52455),
(32796, 0, 0, 0, 0, 0, 0, 52455);

-- Antipersonnel Cannon
UPDATE creature_template SET npcflag = 1, minlevel = 70, maxlevel = 70, unit_flags=4, minhealth = 44910, maxhealth = 44910, faction_A = 35, faction_H = 35, spell1 = 49872, RegenHealth=0, mechanic_immune_mask = mechanic_immune_mask|1|2|8|16|32|64|128|1024|2048|4096|8192|131072|262144|8388608|16777216|67108864, ScriptName = 'npc_sa_cannon' WHERE entry = 27894;
UPDATE creature_template SET npcflag = 1, minlevel = 80, maxlevel = 80, unit_flags=4, minhealth = 63000, maxhealth = 63000, faction_A = 35, faction_H = 35, spell1 = 49872, RegenHealth=0, mechanic_immune_mask = mechanic_immune_mask|1|2|8|16|32|64|128|1024|2048|4096|8192|131072|262144|8388608|16777216|67108864, ScriptName = '' WHERE entry = 32795;
DELETE FROM npc_spellclick_spells WHERE npc_entry IN (27894, 32795);
INSERT INTO npc_spellclick_spells VALUES
(27894, 60968, 0, 0, 0, 0),
(32795, 60968, 0, 0, 0, 0);

-- Titan Relic & Doors
DELETE FROM gameobject WHERE guid IN (200001, 200002, 200003, 200004, 200005, 200006, 200007);
INSERT INTO gameobject VALUES
(200001, 192829, 607, 3, 1, 836.502, -108.811, 111.587, 0.121379, 0, 0, 0.0606524, 0.998159, 5, 0, 1),
(200002, 190727, 607, 3, 1, 1054.47, -107.76, 82.16, 0.06, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200003, 190726, 607, 3, 1, 1228.62, -212.12, 55.34, 0.48, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200004, 190723, 607, 3, 1, 1215.82, 80.64, 53.38, 5.68, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200005, 190724, 607, 3, 1, 1431.05, -219.21, 30.89, 0.83, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200006, 190722, 607, 3, 1, 1413.15, 107.78, 28.69, 5.42, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200007, 192549, 607, 3, 1, 873.3, -108.286, 117.171, 0.00894308, 0, 0, 0.00447152, 0.99999, 600, 0, 1);

DELETE FROM gameobject_battleground WHERE guid in (200001, 200002, 200003, 200004, 200005, 200006, 200007);
INSERT INTO gameobject_battleground VALUES
(200001, 8, 0),
(200002, 8, 0),
(200003, 8, 0),
(200004, 8, 0),
(200005, 8, 0),
(200006, 8, 0),
(200007, 8, 0);

-- Rigger Sparklight
DELETE FROM creature WHERE guid IN (200001, 200002, 200003, 200004, 200005, 200006, 200007, 200008, 200009, 200010, 200011, 200012, 200013, 200014, 200015, 200016, 200027, 200028, 200029, 200030);
INSERT INTO creature (guid,id,map,spawnMask,phaseMask,modelid,equipment_id,position_x,position_y,position_z,orientation,spawntimesecs,spawndist,currentwaypoint,curhealth,curmana,DeathState,MovementType)  VALUES
(200001, 29260, 607, 3, 1, 0, 0, 1360.81, -322.18, 36.83, 0.32, 800, 0, 0, 6986, 0, 0, 0),
(200002, 29262, 607, 3, 1, 0, 0, 1363.87, 220.95, 37.06, 2.67, 800, 0, 0, 6986, 0, 0, 0),
-- Antipersonnel Cannon
(200003, 27894, 607, 3, 1, 0, 0, 1421.94, -196.53, 42.18, 0.90, 800, 0, 0, 25000, 0, 0, 0),
(200004, 27894, 607, 3, 1, 0, 0, 1455.09, -219.92, 41.95, 1.03, 800, 0, 0, 25000, 0, 0, 0),
(200005, 27894, 607, 3, 1, 0, 0, 1405.33, 84.25, 41.18, 5.40, 800, 0, 0, 25000, 0, 0, 0),
(200006, 27894, 607, 3, 1, 0, 0, 1436.51, 110.02, 41.40, 5.32, 800, 0, 0, 25000, 0, 0, 0),
(200007, 27894, 607, 3, 1, 0, 0, 1236.33, 92.17, 64.96, 5.68, 800, 0, 0, 25000, 0, 0, 0),
(200008, 27894, 607, 3, 1, 0, 0, 1215.04, 57.73, 64.73, 5.71, 800, 0, 0, 25000, 0, 0, 0),
(200009, 27894, 607, 3, 1, 0, 0, 1232.35, -187.34, 66.94, 0.37, 800, 0, 0, 25000, 0, 0, 0),
(200010, 27894, 607, 3, 1, 0, 0, 1249.95, -223.74, 66.72, 0.43, 800, 0, 0, 25000, 0, 0, 0),
(200011, 27894, 607, 3, 1, 0, 0, 1068.82, -127.38, 96.44, 0.06, 800, 0, 0, 25000, 0, 0, 0),
(200012, 27894, 607, 3, 1, 0, 0, 1068.35, -87.04, 93.80, 6.24, 800, 0, 0, 25000, 0, 0, 0),
-- Battleground Demolisher
(200013, 28781, 607, 3, 1, 0, 0, 1582.57, -96.37, 8.47, 5.57, 450, 0, 0, 50000, 0, 0, 0),
(200014, 28781, 607, 3, 1, 0, 0, 1610.55, -117.52, 8.77, 2.44, 450, 0, 0, 50000, 0, 0, 0),
(200015, 28781, 607, 3, 1, 0, 0, 1594.24, 39.16, 7.49, 0.62, 450, 0, 0, 50000, 0, 0, 0),
(200016, 28781, 607, 3, 1, 0, 0, 1618.60, 62.26, 7.17, 3.61, 450, 0, 0, 50000, 0, 0, 0),
(200027, 28781, 607, 3, 1, 0, 0, 1353.28, 224.092, 35.2432, 4.35363, 600, 0, 0, 50000, 0, 0, 0),
(200028, 28781, 607, 3, 1, 0, 0, 1347.65, 208.805, 34.2892, 4.39378, 600, 0, 0, 50000, 0, 0, 0),
(200029, 28781, 607, 3, 1, 0, 0, 1371.2, -317.169, 34.9982, 1.93504, 600, 0, 0, 50000, 0, 0, 0),
(200030, 28781, 607, 3, 1, 0, 0, 1365.52, -301.854, 34.0439, 1.91998, 600, 0, 0, 50000, 0, 0, 0);

DELETE FROM creature_battleground WHERE guid in (200001, 200002, 200003, 200004, 200005, 200006, 200007, 200008, 200009, 200010, 200011, 200012, 200013, 200014, 200015, 200016, 200027, 200028, 200029, 200030);
INSERT INTO creature_battleground VALUES
(200001, 7, 0),
(200002, 7, 0),
(200003, 7, 0),
(200004, 7, 0),
(200005, 7, 0),
(200006, 7, 0),
(200007, 7, 0),
(200008, 7, 0),
(200009, 7, 0),
(200010, 7, 0),
(200011, 7, 0),
(200012, 7, 0),
(200013, 7, 0),
(200014, 7, 0),
(200015, 7, 0),
(200016, 7, 0),
(200027, 9, 0),
(200028, 9, 0),
(200029, 10, 0),
(200030, 10, 0);

DELETE FROM battleground_events WHERE map = 607;
INSERT INTO battleground_events (map, event1, event2, description) VALUES
(607, 0, 1, 'East - A contested'),
(607, 0, 2, 'East - H contested'),
(607, 0, 3, 'East - A occupied'),
(607, 0, 4, 'East - H occupied'),
(607, 1, 1, 'West - A contested'),
(607, 1, 2, 'West - H contested'),
(607, 1, 3, 'West - A occupied'),
(607, 1, 4, 'West - H occupied'),
(607, 2, 1, 'Middle - A contested'),
(607, 2, 2, 'Middle - H contested'),
(607, 2, 3, 'Middle - A occupied'),
(607, 2, 4, 'Middle - H occupied'),
(607, 5, 3, 'Beach - allliance attacking'),
(607, 5, 4, 'Beach - horde attacking'),
(607, 6, 0, 'Bomb ALLIANCE'),
(607, 6, 1, 'Bomb HORDE'),
(607, 7, 0, 'Spawn all NPC'),
(607, 8, 0, 'Spawn all GO'),
(607, 9, 0, 'Spawn Vehicle E'),
(607, 10, 0, 'Spawn Vehicle W');


-- Horde and Alliance Spirit Guide
DELETE FROM creature WHERE guid in (200017, 200018, 200019, 200020, 200021, 200022, 200023, 200024, 200025, 200026);
INSERT INTO creature (guid, id, map, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, DeathState, MovementType) VALUES
(200017, 13117, 607, 3, 1, 0, 0, 1448.78, -52.9605, 5.74272, 0.0530517, 25, 0, 0, 37420, 6310, 0, 0),
(200018, 13116, 607, 3, 1, 0, 0, 1448.78, -52.9605, 5.74272, 0.0530517, 25, 0, 0, 37890, 6310, 0, 0),
(200019, 13117, 607, 3, 1, 0, 0, 1389.22, 205.673, 32.0597, 4.445, 25, 0, 0, 37420, 6310, 0, 0),
(200020, 13116, 607, 3, 1, 0, 0, 1389.22, 205.673, 32.0597, 4.445, 25, 0, 0, 37890, 6310, 0, 0),
(200021, 13117, 607, 3, 1, 0, 0, 1400.49, -293.963, 32.0608, 2.08096, 25, 0, 0, 37420, 6310, 0, 0),
(200022, 13116, 607, 3, 1, 0, 0, 1400.49, -293.963, 32.0608, 2.08096, 25, 0, 0, 37890, 6310, 0, 0),
(200023, 13117, 607, 3, 1, 0, 0, 1111.58, 6.40605, 69.2963, 6.20037, 25, 0, 0, 37420, 6310, 0, 0),
(200024, 13116, 607, 3, 1, 0, 0, 1111.58, 6.40605, 69.2963, 6.20037, 25, 0, 0, 37890, 6310, 0, 0),
(200025, 13117, 607, 3, 1, 0, 0, 952.963, -192.742, 92.3584, 0.35779, 25, 0, 0, 37420, 6310, 0, 0),
(200026, 13116, 607, 3, 1, 0, 0, 952.963, -192.742, 92.3584, 0.35779, 25, 0, 0, 37890, 6310, 0, 0);

DELETE FROM creature_battleground WHERE guid in (200017, 200018, 200019, 200020, 200021, 200022, 200023, 200024, 200025, 200026);
INSERT INTO creature_battleground VALUES
(200017, 5, 3),
(200018, 5, 4),
(200025, 5, 4),
(200026, 5, 3),
(200019, 1, 4),
(200020, 1, 3),
(200021, 0, 4),
(200022, 0, 3),
(200023, 2, 4),
(200024, 2, 3);

DELETE FROM mangos_string WHERE entry IN (20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20009, 20010, 20011, 20012, 20013, 20014, 20015, 20016, 20017, 20018, 20019, 20020, 20021, 20022, 20023, 20024, 20025, 20026, 20027, 20028, 20029, 20030, 20031, 20032, 20033, 20034, 20035, 20036, 20037, 20038, 20039, 20040, 20041, 20042, 20043, 20044);
INSERT INTO mangos_string (entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8) VALUES
(20000, 'The battle for Strand of the Ancients begins in 2 minute.', NULL, NULL, 'Die Schlacht um den Strand der Uralten beginnt in 2 Minuten.', NULL, NULL, 'La batalla por la Playa de los Ancestros comenzará en 2 minutos.', 'La batalla por la Playa de los Ancestros comenzará en 2 minutos.', 'Битва за Берег Древних начнется через 2 минуты.'),
(20001, 'The battle for Strand of the Ancients begins in 1 minute.', NULL, NULL, 'Die Schlacht um den Strand der Uralten beginnt in 1 Minute.', NULL, NULL, 'La batalla por la Playa de los Ancestros comenzará en 1 minuto.', 'La batalla por la Playa de los Ancestros comenzará en 1 minuto.', 'Битва за Берег Древних начнется через 1 минуту.'),
(20002, 'The battle for Strand of the Ancients begins in 30 seconds.', NULL, NULL, 'Die Schlacht um den Strand der Uralten beginnt in 30 Sekunden.', NULL, NULL, 'La batalla por la Playa de los Ancestros comenzará en 30 segundos. ¡Preparaos!', 'La batalla por la Playa de los Ancestros comenzará en 30 segundos. ¡Preparaos!', 'Битва за Берег Древних начнется через 30 секунд. Приготовтесь!'),
(20003, 'The battle for Strand of the Ancients has begun!', NULL, NULL, 'Lasst die Schlacht um den Strand der Uralten beginnen!', NULL, NULL, '¡Que comience la batalla por la Playa de los Ancestros!', '¡Que comience la batalla por la Playa de los Ancestros!', 'Битва за Берег Древних началась! '),
(20004, 'The %s is under attack!', NULL, NULL, '%s wird angegriffen!', NULL, NULL, '¡La puerta %s esta siendo asediada!', '¡La puerta %s esta siendo asediada!', 'Врата %s подвергаются нападению!'),
(20005, 'The %s is damaged!', NULL, NULL, '%s ist beschadigt!', NULL, NULL, '¡La puerta %s ha sido dañada!', '¡La puerta %s ha sido dañada!', 'Врата %s повреждены!'),
(20006, 'The %s was destroyed!', NULL, NULL, '%s ist zerstort!', NULL, NULL, '¡La puerta %s ha sido destruida!', '¡La puerta %s ha sido destruida!', 'Врата %s уничтожены!'),
(20007, 'Gate of the Green Emerald', NULL, NULL, 'Das Tor des Smaragdhorizonts', NULL, NULL, 'de la Esmeralda Verde', 'de la Esmeralda Verde', 'Зеленого изумруда'),
(20008, 'Gate of the Blue Sapphire', NULL, NULL, 'Das Tor des Saphirhimmels', NULL, NULL, 'del Zafiro Azul', 'del Zafiro Azul', 'Синего сапфира'),
(20009, 'Gate of the Purple Amethyst', NULL, NULL, 'Das Tor des Amethysystems', NULL, NULL, 'de la Amatista Púrpura', 'de la Amatista Púrpura', 'Лилового аметиста'),
(20010, 'Gate of the Red Sun', NULL, NULL, 'Das Tor der Rubinsonne', NULL, NULL, 'del Sol Rojo', 'del Sol Rojo', 'Красного солнца'),
(20011, 'Gate of the Yellow Moon', NULL, NULL, 'Das Tor des Goldmondes', NULL, NULL, 'de la Luna Amarilla', 'de la Luna Amarilla', 'Желтой луны'),
(20012, 'Chamber of Ancient Relics', NULL, NULL,'Die Kammer der Uralten', NULL, NULL, 'Cámara de Reliquias Antiguas', 'Cámara de Reliquias Antiguas', 'Комната древних святынь'),
(20013, 'East graveyard!', NULL, NULL, 'ostlicher Friedhof!', NULL, NULL, 'Cementerio Este', 'Cementerio Este', 'Восточное кладбище!'),
(20014, 'West graveyard!', NULL, NULL, 'westlicher Friedhof!', NULL, NULL, 'Cementerio Oeste', 'Cementerio Oeste', 'Западное кладбище!'),
(20015, 'South graveyard!', NULL, NULL, 'sudlicher Friedhof!', NULL, NULL, 'Cementerio Sur', 'Cementerio Sur', 'Южное кладбище!'),
(20016, '$n placed a Seaforium Charge!', NULL, NULL, '$n plaziert eine Zephyriumladung!', NULL, NULL, '¡$n ha colocado una carga de seforio!', '¡$n ha colocado una carga de seforio!', '$n заложил сифориевый заряд!'),
(20017, 'The Alliance won the first round, the Relic of Titan was captured!', NULL, NULL, 'Die Allianz hat die erste Runde gewonnen, das Titanenportal erobert!', NULL, NULL, '¡La Alianza gana la primera ronda! ¡La reliquia de Titán ha sido capturada!', '¡La Alianza gana la primera ronda! ¡La reliquia de Titán ha sido capturada!', 'Первый раунд за Альянсом! Реликвия Титана захвачена!'),
(20018, 'The Horde won the first round, the Relic of Titan was captured!', NULL, NULL, 'Die Horde hat die erste Runde gewonnen, das Titanenportal erobert!', NULL, NULL, '¡La Horda gana la primera ronda! ¡La reliquia de Titán ha sido capturada!', '¡La Horda gana la primera ronda! ¡La reliquia de Titán ha sido capturada!', 'Первый раунд за Ордой! Реликвия Титана захвачена!'),
(20019, 'The Alliance won the second round, the Relic of Titan was captured!', NULL, NULL, 'Die Allianz hat die zweite Runde gewonnen, das Titanenportal erobert!', NULL, NULL, '¡La Horda gana la segunda ronda! ¡La reliquia de Titán ha sido capturada!', '¡La Horda gana la segunda ronda! ¡La reliquia de Titán ha sido capturada!', 'Второй раунд за Альянсом! Реликвия Титана захвачена!'),
(20020, 'The Horde won the second round, the Relic of Titan was captured!', NULL, NULL, 'Die Horde hat die zweite Runde gewonnen, das Titanenportal erobert!', NULL, NULL, '¡La Alianza gana la segunda ronda! ¡La reliquia de Titán ha sido capturada!', '¡La Alianza gana la segunda ronda! ¡La reliquia de Titán ha sido capturada!', 'Второй раунд за Ордой! Реликвия Титана захвачена!'),
(20021, 'The Eastern Graveyard has been captured by the Horde!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La Horda ha capturado el cementerio oriental!',  '¡La Horda ha capturado el cementerio oriental!',  NULL),
(20022, 'The Western Graveyard has been captured by the Horde!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La Horda ha capturado el cementerio occidental!', '¡La Horda ha capturado el cementerio occidental!',  NULL),
(20023, 'The Southern Graveyard has been captured by the Horde!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La Horda ha capturado el cementerio sur!', '¡La Horda ha capturado el cementerio sur!',  NULL),
(20024, 'The Eastern Graveyard has been captured by the Alliance!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La Alianza ha capturado el cementerio oriental!', '¡La Alianza ha capturado el cementerio oriental!',  NULL),
(20025, 'The Western Graveyard has been captured by the Alliance!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La Alianza ha capturado el cementerio occidental!', '¡La Alianza ha capturado el cementerio occidental!',  NULL),
(20026, 'The Southern Graveyard has been captured by the Alliance!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La Alianza ha capturado el cementerio sur!', '¡La Alianza ha capturado el cementerio sur!',  NULL),
(20027, 'The chamber has been breached! The Titan Relic is vulnerable!',  NULL,  NULL,  NULL,  NULL,  NULL,  '¡La puerta de la Cámara de Reliquias antiguas ha sido destruida! ¡La reliquia de titán es vulnerable!',  '¡La puerta de la Cámara de Reliquias antiguas ha sido destruida! ¡La reliquia de titán es vulnerable!',  NULL),
(20028, 'The Gate of the Green Emerald was destroyed!', NULL, NULL, 'Das Tor des Smaragdhorizonts ist zerstort!', NULL, NULL, '¡La puerta de la Esmeralda Verde ha sido destruida!', '¡La puerta de la Esmeralda Verde ha sido destruida!', 'Врата Зеленого Изумруда разрушены!'),
(20029, 'The Gate of the Blue Sapphire was destroyed!', NULL, NULL, 'Das Tor des Saphirhimmels ist zerstort!', NULL, NULL, '¡La puerta del Zafiro Azul ha sido destruida!', '¡La puerta del Zafiro Azul ha sido destruida!', 'Врата Синего Сапфира разрушены!'),
(20030, 'The Gate of the Purple Amethyst was destroyed!', NULL, NULL, 'Das Tor des Amethyststerns ist zerstort worden', NULL, NULL, '¡La puerta de la Amatista Púrpura ha sido destruida!', '¡La puerta de la Amatista Púrpura ha sido destruida!', 'Врата Лилового Аметиста разрушены!'),
(20031, 'The Gate of the Red Sun was destroyed!', NULL, NULL, 'Das Tor der Rubinsonne ist zerstort worden!', NULL, NULL, '¡La puerta del Sol Rojo ha sido destruida!', '¡La puerta del Sol Rojo ha sido destruida!', 'Врата Красного Солнца разрушены!'),
(20032, 'The Gate of the Yellow Moon was destroyed!', NULL, NULL, 'Das Tor des Goldmondes ist zerstort worden!', NULL, NULL, '¡La puerta de la Luna Amarilla ha sido destruida!', '¡La puerta de la Luna Amarilla ha sido destruida!', 'Врата Желтой Луны разрушены!'),
(20033, 'The Chamber of Ancient Relics is under attack!',  NULL,  NULL,  NULL,  NULL,  NULL, '¡La puerta de la Cámara de Reliquias antiguas esta siendo asediada!',  '¡La puerta de la Cámara de Reliquias antiguas esta siendo asediada!',  NULL),
(20034, 'The Gate of the Green Emerald is damaged!', NULL, NULL, 'Das Tor des Smaragdhorizonts ist beschadigt worden', NULL, NULL, '¡La puerta de la Esmeralda Verde ha sido dañada!', '¡La puerta de la Esmeralda Verde ha sido dañada!', 'Врата Зеленого Изумруда повреждены!'),
(20035, 'The Gate of the Blue Sapphire is damaged!', NULL, NULL, 'Das Tor des Saphirhimmels ist beschadigt worden!', NULL, NULL, '¡La puerta del Zafiro Azul ha sido dañada!', '¡La puerta del Zafiro Azul ha sido dañada!', 'Врата Синего Сапфира повреждены!'),
(20036, 'The Gate of the Purple Amethyst is damaged!', NULL, NULL, 'Das Tor des Amethyststerns ist beschadigt worden!', NULL, NULL, '¡La puerta de la Amatista Púrpura ha sido dañada!', '¡La puerta de la Amatista Púrpura ha sido dañada!', 'Врата Лилового Аметиста повреждены!'),
(20037, 'The Gate of the Red Sun is damaged!', NULL, NULL, 'Das Tor der Rubinsonne ist beschadigt worden!', NULL, NULL, '¡La puerta del Sol Rojo ha sido dañada!', '¡La puerta del Sol Rojo ha sido dañada!', 'Врата Красного Солнца повреждены!'),
(20038, 'The Gate of the Yellow Moon is damaged!', NULL, NULL, 'Das Tor des Goldmondes ist beschadigt worden!', NULL, NULL, '¡La puerta de la Luna Amarilla ha sido dañada!', '¡La puerta de la Luna Amarilla ha sido dañada!', 'Врата Желтой Луны повреждены!'),
(20039, 'The Horde won the first round, the Relic of Titan was no captured!', NULL, NULL, 'Runde 1 - Beendet!', NULL, NULL, '¡Ronda 1 acabada!', '¡Ronda 1 acabada!', 'Первый раунд за Ордой! Реликвия Титана не была захвачена!'),
(20040, 'The Alliance won the first round, the Relic of Titan was no captured!', NULL, NULL, 'Runde 1 - Beendet!', NULL, NULL, '¡Ronda 1 acabada!', '¡Ronda 1 acabada!', 'Первый раунд за Альянсом! Реликвия Титана не была захвачена!'),
(20041, 'The Horde won the second round, the Relic of Titan was no captured!', NULL, NULL, 'Runde 2 - Beendet!', NULL, NULL, '¡Ronda 2 acabada!', '¡Ronda 2 acabada!', 'Второй раунд за Ордой! Реликвия Титана не была захвачена!'),
(20042, 'The Alliance won the second round, the Relic of Titan was no captured!', NULL, NULL, 'Runde 2 - Beendet!', NULL, NULL, '¡Ronda 2 acabada!', '¡Ronda 2 acabada!', 'Второй раунд за Альянсом! Реликвия Титана не была захвачена!'),
(20043, 'Round 1 - Finished!',  NULL,  NULL,  'Runde 1 - Beendet!',  NULL,  NULL,  '¡Ronda 1 acabada!', '¡Ronda 1 acabada!', NULL),
(20044, 'The game has ended in a draw!', NULL, NULL, 'Das Spiel ist unentschieden!!!', NULL, NULL, '¡El campo de batalla ha acabado en empate!', '¡El campo de batalla ha acabado en empate!', 'Ничья!!!');

DELETE FROM gameobject WHERE guid BETWEEN 200008 AND 200028;
INSERT INTO gameobject VALUES
(200008, 180058, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Alliance Banner        -- Alli contested banner
(200009, 191310, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Alliance Banner        -- Alli occupied banner
(200010, 180060, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Horde Banner           -- Horde contested banner (not clickable)
(200011, 191309, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Horde Banner           -- Horde occupied banner...
(200012, 180100, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Alliance Banner Aura
(200013, 180101, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Horde Banner Aura
(200014, 191311, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1), -- Flagpole

(200015, 180058, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Alliance Banner
(200016, 191310, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Alliance Banner
(200017, 180060, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Horde Banner
(200018, 191309, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Horde Banner
(200019, 180100, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Alliance Banner Aura
(200020, 180101, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Horde Banner Aura
(200021, 191311, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1), -- Flagpole

(200022, 180058, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1), -- Alliance Banner
(200023, 191310, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1), -- Alliance Banner
(200024, 180060, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1), -- Horde Banner
(200025, 191309, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1), -- Horde Banner
(200026, 180100, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1), -- Alliance Banner Aura
(200027, 180101, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1), -- Horde Banner Aura
(200028, 191311, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1); -- Flagpole


DELETE FROM gameobject_battleground WHERE guid BETWEEN 200008 AND 200180;
INSERT INTO gameobject_battleground VALUES
(200008, 0, 1),
(200009, 0, 3),
(200010, 0, 2),
(200011, 0, 4),
(200012, 0, 1),
(200013, 0, 2),
(200014, 8, 0),
(200015, 1, 1),
(200016, 1, 3),
(200017, 1, 2),
(200018, 1, 4),
(200019, 1, 1),
(200020, 1, 2),
(200021, 8, 0),
(200022, 2, 1),
(200023, 2, 3),
(200024, 2, 2),
(200025, 2, 4),
(200026, 2, 1),
(200027, 2, 2),
(200028, 8, 0),
(200029, 8, 0),
(200030, 8, 0),
(200031, 8, 0),
(200032, 8, 0),
(200033, 8, 0),
(200034, 8, 0),
(200035, 8, 0),

(200036, 1, 3),
(200037, 1, 3),
(200038, 1, 3),
(200039, 1, 3),
(200040, 1, 3),
(200041, 1, 3),
(200042, 1, 3),
(200043, 1, 3),
(200044, 1, 3),
(200045, 1, 3),
(200046, 1, 3),
(200047, 1, 3),

(200048, 1, 4),
(200049, 1, 4),
(200050, 1, 4),
(200051, 1, 4),
(200052, 1, 4),
(200053, 1, 4),
(200054, 1, 4),
(200055, 1, 4),
(200056, 1, 4),
(200057, 1, 4),
(200058, 1, 4),
(200059, 1, 4),

(200060, 6, 0),
(200061, 6, 0),
(200062, 6, 0),
(200063, 6, 0),
(200064, 6, 0),
(200065, 6, 0),
(200066, 6, 0),
(200067, 6, 0),
(200068, 6, 0),
(200069, 6, 0),
(200070, 6, 0),
(200071, 6, 0),
(200072, 6, 0),
(200073, 6, 0),
(200074, 6, 0),
(200075, 6, 0),
(200076, 6, 0),
(200077, 6, 0),
(200078, 6, 0),
(200079, 6, 0),
(200080, 6, 0),
(200081, 6, 0),

(200082, 6, 1),
(200083, 6, 1),

-- (200084, 254, 0),
-- (200085, 254, 0),
-- (200086, 254, 0);

(200087, 6, 1),
(200088, 6, 1),
(200089, 6, 1),
(200090, 6, 1),
(200091, 6, 1),
(200092, 6, 1),
(200093, 6, 1),
(200094, 6, 1),
(200095, 6, 1),
(200096, 6, 1),
(200097, 6, 1),
(200098, 6, 1),
(200099, 6, 1),
(200100, 6, 1),
(200101, 6, 1),
(200102, 6, 1),
(200103, 6, 1),
(200104, 6, 1),
(200105, 6, 1),
(200106, 6, 1),

(200107, 0, 3),
(200108, 0, 3),
(200109, 0, 3),
(200110, 0, 3),
(200111, 0, 3),
(200112, 0, 3),
(200113, 0, 3),
(200114, 0, 3),
(200115, 0, 3),

(200116, 0, 4),
(200117, 0, 4),
(200118, 0, 4),
(200119, 0, 4),
(200120, 0, 4),
(200121, 0, 4),
(200122, 0, 4),
(200123, 0, 4),
(200124, 0, 4),

(200125, 2, 3),
(200126, 2, 3),
(200127, 2, 3),
(200128, 2, 3),
(200129, 2, 3),
(200130, 2, 3),
(200131, 2, 3),
(200132, 2, 3),
(200133, 2, 3),
(200134, 2, 3),
(200135, 2, 3),
(200136, 2, 3),
(200137, 2, 3),
(200138, 2, 3),
(200139, 2, 3),
(200140, 2, 3),
(200141, 2, 3),
(200142, 2, 3),
(200143, 2, 3),
(200144, 2, 3),
(200145, 2, 3),
(200146, 2, 3),
(200147, 2, 3),
(200148, 2, 3),
(200149, 2, 3),

(200150, 2, 4),
(200151, 2, 4),
(200152, 2, 4),
(200153, 2, 4),
(200154, 2, 4),
(200155, 2, 4),
(200156, 2, 4),
(200157, 2, 4),
(200158, 2, 4),
(200159, 2, 4),
(200160, 2, 4),
(200161, 2, 4),
(200162, 2, 4),
(200163, 2, 4),
(200164, 2, 4),
(200165, 2, 4),
(200166, 2, 4),
(200167, 2, 4),
(200168, 2, 4),
(200170, 2, 4),
(200171, 2, 4),
(200172, 2, 4),
(200173, 2, 4),
(200174, 2, 4),

(200175, 0, 3),
(200176, 0, 4),
(200177, 1, 3),
(200178, 1, 4),
(200179, 2, 3),
(200180, 2, 4);

-- Defender's Portal
DELETE FROM gameobject WHERE guid BETWEEN 200029 AND 200035;
INSERT INTO gameobject VALUES
(200029, 191575, 607, 3, 1, 1468.12, -225.7, 30.8969, 5.68042, 0, 0, 0.296839, -0.954928, 25, 0, 1),
(200030, 191575, 607, 3, 1, 1394.07, 72.3632, 31.0541, 0.818809, 0, 0, 0.398063, 0.917358, 25, 0, 1),
(200031, 191575, 607, 3, 1, 1216.12, 47.7665, 54.2785, 4.05465, 0, 0, 0.897588, -0.440835, 25, 0, 1),
(200032, 191575, 607, 3, 1, 1255.73, -233.153, 56.4357, 5.01833, 0, 0, 0.591105, -0.806595, 25, 0, 1),
(200033, 191575, 607, 3, 1, 1065.02, -89.9522, 81.0758, 1.58771, 0, 0, 0.71306, 0.701103, 25, 0, 1),
(200034, 191575, 607, 3, 1, 880.162, -95.979, 109.835, 3.14672, 0, 0, 0.999997, -0.00256531, 25, 0, 1),
(200035, 191575, 607, 3, 1, 880.68, -120.799, 109.835, 3.16636, 0, 0, 0.999923, -0.0123816, 25, 0, 1);

-- Seaforium Bomb

-- Western base bombs
DELETE FROM gameobject WHERE guid BETWEEN 200036 AND 200059;
INSERT INTO gameobject (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(200036 , 190753 , 607 , 3 , 1 , 1333.45 , 211.354 , 31.0538 , 5.03666 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200037 , 190753 , 607 , 3 , 1 , 1334.29 , 209.582 , 31.0532 , 1.28088 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200038 , 190753 , 607 , 3 , 1 , 1332.72 , 210.049 , 31.0532 , 1.28088 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200039 , 190753 , 607 , 3 , 1 , 1334.28 , 210.78 , 31.0538 , 3.85856 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200040 , 190753 , 607 , 3 , 1 , 1332.64 , 211.39 , 31.0532 , 1.29266 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200041 , 190753 , 607 , 3 , 1 , 1371.41 , 194.028 , 31.5107 , 0.753095 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200042 , 190753 , 607 , 3 , 1 , 1372.39 , 194.951 , 31.4679 , 0.753095 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200043 , 190753 , 607 , 3 , 1 , 1371.58 , 196.942 , 30.9349 , 1.01777 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200044 , 190753 , 607 , 3 , 1 , 1370.43 , 196.614 , 30.9349 , 0.957299 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200045 , 190753 , 607 , 3 , 1 , 1369.46 , 196.877 , 30.9351 , 2.45348 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200046 , 190753 , 607 , 3 , 1 , 1370.35 , 197.361 , 30.9349 , 1.08689 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200047 , 190753 , 607 , 3 , 1 , 1369.47 , 197.941 , 30.9349 , 0.984787 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200048 , 194086 , 607 , 3 , 1 , 1333.45 , 211.354 , 31.0538 , 5.03666 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200049 , 194086 , 607 , 3 , 1 , 1334.29 , 209.582 , 31.0532 , 1.28088 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200050 , 194086 , 607 , 3 , 1 , 1332.72 , 210.049 , 31.0532 , 1.28088 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200051 , 194086 , 607 , 3 , 1 , 1334.28 , 210.78 , 31.0538 , 3.85856 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200052 , 194086 , 607 , 3 , 1 , 1332.64 , 211.39 , 31.0532 , 1.29266 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200053 , 194086 , 607 , 3 , 1 , 1371.41 , 194.028 , 31.5107 , 0.753095 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200054 , 194086 , 607 , 3 , 1 , 1372.39 , 194.951 , 31.4679 , 0.753095 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200055 , 194086 , 607 , 3 , 1 , 1371.58 , 196.942 , 30.9349 , 1.01777 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200056 , 194086 , 607 , 3 , 1 , 1370.43 , 196.614 , 30.9349 , 0.957299 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200057 , 194086 , 607 , 3 , 1 , 1369.46 , 196.877 , 30.9351 , 2.45348 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200058 , 194086 , 607 , 3 , 1 , 1370.35 , 197.361 , 30.9349 , 1.08689 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200059 , 194086 , 607 , 3 , 1 , 1369.47 , 197.941 , 30.9349 , 0.984787 , 0 , 0 , 0 , 0 , 0 , 0 , 0);

-- Eastern base bombs
DELETE FROM gameobject WHERE guid BETWEEN 200060 AND 200106;
INSERT INTO gameobject (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(200060 , 190753 , 607 , 3 , 1 , 1592.49 , 47.5969 , 7.52271 , 4.63218 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200061 , 190753 , 607 , 3 , 1 , 1593.91 , 47.8036 , 7.65856 , 4.63218 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200062 , 190753 , 607 , 3 , 1 , 1593.13 , 46.8106 , 7.54073 , 4.63218 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200063 , 190753 , 607 , 3 , 1 , 1589.22 , 36.3616 , 7.45975 , 4.64396 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200064 , 190753 , 607 , 3 , 1 , 1588.24 , 35.5842 , 7.55613 , 4.79564 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200065 , 190753 , 607 , 3 , 1 , 1588.14 , 36.7611 , 7.49675 , 4.79564 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200066 , 190753 , 607 , 3 , 1 , 1595.74 , 35.5278 , 7.46602 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200067 , 190753 , 607 , 3 , 1 , 1596 , 36.6475 , 7.47991 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200068 , 190753 , 607 , 3 , 1 , 1597.03 , 36.2356 , 7.48631 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200069 , 190753 , 607 , 3 , 1 , 1597.93 , 37.1214 , 7.51725 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200070 , 190753 , 607 , 3 , 1 , 1598.16 , 35.888 , 7.50018 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200071 , 190753 , 607 , 3 , 1 , 1579.6 , -98.0917 , 8.48478 , 1.37996 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200072 , 190753 , 607 , 3 , 1 , 1581.2 , -98.401 , 8.47483 , 1.37996 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200073 , 190753 , 607 , 3 , 1 , 1580.38 , -98.9556 , 8.4772 , 1.38781 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200074 , 190753 , 607 , 3 , 1 , 1585.68 , -104.966 , 8.88551 , 0.493246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200075 , 190753 , 607 , 3 , 1 , 1586.15 , -106.033 , 9.10616 , 0.493246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200076 , 190753 , 607 , 3 , 1 , 1584.88 , -105.394 , 8.82985 , 0.493246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200077 , 190753 , 607 , 3 , 1 , 1581.87 , -100.899 , 8.46164 , 0.929142 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200078 , 190753 , 607 , 3 , 1 , 1581.48 , -99.4657 , 8.46926 , 0.929142 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200079 , 190753 , 607 , 3 , 1 , 1583.2 , -91.2291 , 8.49227 , 1.40038 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200080 , 190753 , 607 , 3 , 1 , 1581.94 , -91.0119 , 8.49977 , 1.40038 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200081 , 190753 , 607 , 3 , 1 , 1582.33 , -91.951 , 8.49353 , 1.1844 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200082 , 194086 , 607 , 3 , 1 , 1592.49 , 47.5969 , 7.52271 , 4.63218 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200083 , 194086 , 607 , 3 , 1 , 1593.91 , 47.8036 , 7.65856 , 4.63218 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
-- 200084 - 200086???
(200087 , 194086 , 607 , 3 , 1 , 1593.13 , 46.8106 , 7.54073 , 4.63218 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200088 , 194086 , 607 , 3 , 1 , 1589.22 , 36.3616 , 7.45975 , 4.64396 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200089 , 194086 , 607 , 3 , 1 , 1588.24 , 35.5842 , 7.55613 , 4.79564 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200090 , 194086 , 607 , 3 , 1 , 1588.14 , 36.7611 , 7.49675 , 4.79564 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200091 , 194086 , 607 , 3 , 1 , 1595.74 , 35.5278 , 7.46602 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200092 , 194086 , 607 , 3 , 1 , 1596 , 36.6475 , 7.47991 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200093 , 194086 , 607 , 3 , 1 , 1597.03 , 36.2356 , 7.48631 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200094 , 194086 , 607 , 3 , 1 , 1597.93 , 37.1214 , 7.51725 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200095 , 194086 , 607 , 3 , 1 , 1598.16 , 35.888 , 7.50018 , 4.90246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200096 , 194086 , 607 , 3 , 1 , 1579.6 , -98.0917 , 8.48478 , 1.37996 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200097 , 194086 , 607 , 3 , 1 , 1581.2 , -98.401 , 8.47483 , 1.37996 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200098 , 194086 , 607 , 3 , 1 , 1580.38 , -98.9556 , 8.4772 , 1.38781 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200099 , 194086 , 607 , 3 , 1 , 1585.68 , -104.966 , 8.88551 , 0.493246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200100 , 194086 , 607 , 3 , 1 , 1586.15 , -106.033 , 9.10616 , 0.493246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200101 , 194086 , 607 , 3 , 1 , 1584.88 , -105.394 , 8.82985 , 0.493246 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200102 , 194086 , 607 , 3 , 1 , 1581.87 , -100.899 , 8.46164 , 0.929142 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200103 , 194086 , 607 , 3 , 1 , 1581.48 , -99.4657 , 8.46926 , 0.929142 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200104 , 194086 , 607 , 3 , 1 , 1583.2 , -91.2291 , 8.49227 , 1.40038 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200105 , 194086 , 607 , 3 , 1 , 1581.94 , -91.0119 , 8.49977 , 1.40038 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200106 , 194086 , 607 , 3 , 1 , 1582.33 , -91.951 , 8.49353 , 1.1844 , 0 , 0 , 0 , 0 , 0 , 0 , 0);

-- Dock bombs
DELETE FROM gameobject WHERE guid BETWEEN 200107 AND 200124;
INSERT INTO gameobject (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(200107 , 190753 , 607 , 3 , 1 , 1342.06 , -304.049 , 30.9532 , 5.59507 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200108 , 190753 , 607 , 3 , 1 , 1340.96 , -304.536 , 30.9458 , 1.28323 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200109 , 190753 , 607 , 3 , 1 , 1341.22 , -303.316 , 30.9413 , 0.486051 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200110 , 190753 , 607 , 3 , 1 , 1342.22 , -302.939 , 30.986 , 4.87643 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200111 , 190753 , 607 , 3 , 1 , 1382.16 , -287.466 , 32.3063 , 4.80968 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200112 , 190753 , 607 , 3 , 1 , 1381 , -287.58 , 32.2805 , 4.80968 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200113 , 190753 , 607 , 3 , 1 , 1381.55 , -286.536 , 32.3929 , 2.84225 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200114 , 190753 , 607 , 3 , 1 , 1382.75 , -286.354 , 32.4099 , 1.00442 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200115 , 190753 , 607 , 3 , 1 , 1379.92 , -287.34 , 32.2872 , 3.81615 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200116 , 194086 , 607 , 3 , 1 , 1342.06 , -304.049 , 30.9532 , 5.59507 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200117 , 194086 , 607 , 3 , 1 , 1340.96 , -304.536 , 30.9458 , 1.28323 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200118 , 194086 , 607 , 3 , 1 , 1341.22 , -303.316 , 30.9413 , 0.486051 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200119 , 194086 , 607 , 3 , 1 , 1342.22 , -302.939 , 30.986 , 4.87643 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200120 , 194086 , 607 , 3 , 1 , 1382.16 , -287.466 , 32.3063 , 4.80968 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200121 , 194086 , 607 , 3 , 1 , 1381 , -287.58 , 32.2805 , 4.80968 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200122 , 194086 , 607 , 3 , 1 , 1381.55 , -286.536 , 32.3929 , 2.84225 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200123 , 194086 , 607 , 3 , 1 , 1382.75 , -286.354 , 32.4099 , 1.00442 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200124 , 194086 , 607 , 3 , 1 , 1379.92 , -287.34 , 32.2872 , 3.81615 , 0 , 0 , 0 , 0 , 0 , 0 , 0);

-- Middle bombs (both beach and relic side)
DELETE FROM gameobject WHERE guid BETWEEN 200125 AND 200174;
INSERT INTO gameobject (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(200125 , 190753 , 607 , 3 , 1 , 1100.52 , -2.41391 , 70.2984 , 0.131054 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200126 , 190753 , 607 , 3 , 1 , 1099.35 , -2.13851 , 70.3375 , 4.4586 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200127 , 190753 , 607 , 3 , 1 , 1099.59 , -1.00329 , 70.238 , 2.49903 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200128 , 190753 , 607 , 3 , 1 , 1097.79 , 0.571316 , 70.159 , 4.00307 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200129 , 190753 , 607 , 3 , 1 , 1098.74 , -7.23252 , 70.7972 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200130 , 190753 , 607 , 3 , 1 , 1098.46 , -5.91443 , 70.6715 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200131 , 190753 , 607 , 3 , 1 , 1097.53 , -7.39704 , 70.7959 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200132 , 190753 , 607 , 3 , 1 , 1097.32 , -6.64233 , 70.7424 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200133 , 190753 , 607 , 3 , 1 , 1096.45 , -5.96664 , 70.7242 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200134 , 190753 , 607 , 3 , 1 , 971.725 , 0.496763 , 86.8467 , 2.09233 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200135 , 190753 , 607 , 3 , 1 , 973.589 , 0.119518 , 86.7985 , 3.17225 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200136 , 190753 , 607 , 3 , 1 , 972.524 , 1.25333 , 86.8351 , 5.28497 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200137 , 190753 , 607 , 3 , 1 , 971.993 , 2.05668 , 86.8584 , 5.28497 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200138 , 190753 , 607 , 3 , 1 , 973.635 , 2.11805 , 86.8197 , 2.36722 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200139 , 190753 , 607 , 3 , 1 , 974.791 , 1.74679 , 86.7942 , 1.5936 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200140 , 190753 , 607 , 3 , 1 , 974.771 , 3.0445 , 86.8125 , 0.647199 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200141 , 190753 , 607 , 3 , 1 , 979.554 , 3.6037 , 86.7923 , 1.69178 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200142 , 190753 , 607 , 3 , 1 , 979.758 , 2.57519 , 86.7748 , 1.76639 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200143 , 190753 , 607 , 3 , 1 , 980.769 , 3.48904 , 86.7939 , 1.76639 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200144 , 190753 , 607 , 3 , 1 , 979.122 , 2.87109 , 86.7794 , 1.76639 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200145 , 190753 , 607 , 3 , 1 , 986.167 , 4.85363 , 86.8439 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200146 , 190753 , 607 , 3 , 1 , 986.176 , 3.50367 , 86.8217 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200147 , 190753 , 607 , 3 , 1 , 987.33 , 4.67389 , 86.8486 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200148 , 190753 , 607 , 3 , 1 , 985.23 , 4.65898 , 86.8368 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200149 , 190753 , 607 , 3 , 1 , 984.556 , 3.54097 , 86.8137 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200150 , 194086 , 607 , 3 , 1 , 1100.52 , -2.41391 , 70.2984 , 0.131054 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200151 , 194086 , 607 , 3 , 1 , 1099.35 , -2.13851 , 70.3375 , 4.4586 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200152 , 194086 , 607 , 3 , 1 , 1099.59 , -1.00329 , 70.238 , 2.49903 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200153 , 194086 , 607 , 3 , 1 , 1097.79 , 0.571316 , 70.159 , 4.00307 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200154 , 194086 , 607 , 3 , 1 , 1098.74 , -7.23252 , 70.7972 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200155 , 194086 , 607 , 3 , 1 , 1098.46 , -5.91443 , 70.6715 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200156 , 194086 , 607 , 3 , 1 , 1097.53 , -7.39704 , 70.7959 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200157 , 194086 , 607 , 3 , 1 , 1097.32 , -6.64233 , 70.7424 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200158 , 194086 , 607 , 3 , 1 , 1096.45 , -5.96664 , 70.7242 , 4.1523 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200159 , 194086 , 607 , 3 , 1 , 971.725 , 0.496763 , 86.8467 , 2.09233 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200160 , 194086 , 607 , 3 , 1 , 973.589 , 0.119518 , 86.7985 , 3.17225 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200161 , 194086 , 607 , 3 , 1 , 972.524 , 1.25333 , 86.8351 , 5.28497 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200162 , 194086 , 607 , 3 , 1 , 971.993 , 2.05668 , 86.8584 , 5.28497 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200163 , 194086 , 607 , 3 , 1 , 973.635 , 2.11805 , 86.8197 , 2.36722 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200164 , 194086 , 607 , 3 , 1 , 974.791 , 1.74679 , 86.7942 , 1.5936 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200165 , 194086 , 607 , 3 , 1 , 974.771 , 3.0445 , 86.8125 , 0.647199 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200166 , 194086 , 607 , 3 , 1 , 979.554 , 3.6037 , 86.7923 , 1.69178 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200167 , 194086 , 607 , 3 , 1 , 979.758 , 2.57519 , 86.7748 , 1.76639 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200168 , 194086 , 607 , 3 , 1 , 980.769 , 3.48904 , 86.7939 , 1.76639 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200169 , 194086 , 607 , 3 , 1 , 979.122 , 2.87109 , 86.7794 , 1.76639 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200170 , 194086 , 607 , 3 , 1 , 986.167 , 4.85363 , 86.8439 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200171 , 194086 , 607 , 3 , 1 , 986.176 , 3.50367 , 86.8217 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200172 , 194086 , 607 , 3 , 1 , 987.33 , 4.67389 , 86.8486 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200173 , 194086 , 607 , 3 , 1 , 985.23 , 4.65898 , 86.8368 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0),
(200174 , 194086 , 607 , 3 , 1 , 984.556 , 3.54097 , 86.8137 , 1.5779 , 0 , 0 , 0 , 0 , 0 , 0 , 0);

-- additional banner auras
DELETE FROM gameobject WHERE guid BETWEEN 200175 AND 200180;
INSERT INTO gameobject VALUES
(200175, 180100, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),
(200176, 180101, 607, 3, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),
(200177, 180100, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),
(200178, 180101, 607, 3, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),
(200179, 180100, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),
(200180, 180101, 607, 3, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1);

-- Fix Rotation for all Objects in Map
UPDATE gameobject SET rotation0=0, rotation1=0, rotation2=SIN(orientation*0.5), rotation3=COS(orientation*0.5) WHERE map = 607;

-- DELETE FROM gameobject WHERE id=190724;
-- INSERT INTO gameobject (guid,id,map,spawnMask,phaseMask,position_x,position_y,position_z,orientation,rotation0,rotation1,rotation2,rotation3,spawntimesecs,animprogress,state) VALUES
-- (200005, 190724, 607, 3, 1, 1431.05, -219.21, 30.89, 0.83, 0, 0, 0.47319, 0.915116, 600, 0, 1);
