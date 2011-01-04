REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES
(300000, 0, 7203, 'Forcefield 000', '', '', '', 1375, 32, 0.05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET npcflag=1, `ScriptName` = 'npc_sa_vendor' WHERE entry in (29260,29262);
-- Seaforium source
DELETE FROM `creature_template` WHERE `entry` in (50000);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `modelid_3`, `modelid_4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `unk16`, `unk17`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(50000, 0, 0, 0, 0, 0, 27462, 0, 27462, 0, 'Carga de Seforio', NULL, NULL, 0, 80, 80, 5000, 5000, 0, 0, 9730, 7, 7, 0, 1, 0.5, 0, 420, 630, 0, 157, 1, 2000, 2000, 1, 0, 0, 0, 0, 0, 0, 0, 336, 504, 126, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 94, 1, 0, 0, 0, 'npc_sa_bomb');
-- Doors
UPDATE `gameobject_template` SET `faction` = 14 WHERE `entry` in (192549,190727,190726,190723,190724,190722);
-- Seaforium barrels
DELETE FROM `gameobject_template` WHERE `entry` in (190753,194086);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES
(190753, 22, 8582, 'Barril de seforio', '', '', '', 12, 0, 2, 0, 0, 0, 0, 0, 0, 52415, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(194086, 22, 8582, 'Barril de seforio', '', '', '', 29, 0, 2, 0, 0, 0, 0, 0, 0, 52415, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');
-- Demolisher
UPDATE `creature_template` SET `npcflag` = '1', minhealth = '80000', maxhealth = '80000', `ScriptName` = 'npc_sa_demolisher' WHERE `entry` = '28781';
DeleTe From `npc_spellclick_spells` WHERE npc_entry in (28781);
INSERT INTO `npc_spellclick_spells` VALUES ('28781', '60968', '0', '0', '0', '0');

-- AP Cannon
UPDATE `creature_template` SET `npcflag` = '1', unit_flags = 8, minhealth = '60000', maxhealth = '60000', `faction_A` = '35', `faction_H` = '35', `ScriptName` = 'npc_sa_cannon' WHERE entry = '27894';
DeleTe From `npc_spellclick_spells` WHERE npc_entry in (27894);
INSERT INTO `npc_spellclick_spells` VALUES ('27894', '60968', '0', '0', '0', '0');

DELETE FROM `gameobject` WHERE `guid` in (200001,200002,200003,200004,200005,200006,200007);
INSERT INTO `gameobject` (`guid` ,`id` ,`map` ,`spawnMask` ,`phaseMask` ,`position_x` ,`position_y` ,`position_z` ,`orientation` ,`rotation0` ,`rotation1` ,`rotation2` ,`rotation3` ,`spawntimesecs` ,`animprogress` ,`state` ) VALUES
(200001, 192829, 607, 1, 1, 836.502, -108.811, 111.587, 0.121379, 0, 0, 0.0606524, 0.998159, 5, 0, 1),
(200002, 190727, 607, 1, 1, 1054.47, -107.76, 82.16, 0.06, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200003, 190726, 607, 1, 1, 1228.62, -212.12, 55.34, 0.48, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200004, 190723, 607, 1, 1, 1215.82, 80.64, 53.38, 5.68, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200005, 190724, 607, 1, 1, 1431.05, -219.21, 30.89, 0.83, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200006, 190722, 607, 1, 1, 1413.15, 107.78, 28.69, 5.42, 0, 0, 0.008726535, 0.9999619, 600, 0, 1),
(200007, 192549, 607, 1, 1, 873.3, -108.286, 117.171, 0.00894308, 0, 0, 0.00447152, 0.99999, 600, 0, 1);

DELETE FROM `gameobject_battleground` WHERE `guid` in (200001,200002,200003,200004,200005,200006,200007);
INSERT INTO `gameobject_battleground` (`guid` ,`event1` ,`event2`) VALUES
('200001', '8', '0'),
('200002', '8', '0'),
('200003', '8', '0'),
('200004', '8', '0'),
('200005', '8', '0'),
('200006', '8', '0'),
('200007', '8', '0');

DELETE FROM `creature` WHERE `guid` in (200001,200002,200003,200004,200005,200006,200007,200008,200009,200010,200011,200012,200013,200014,200015,200016,200027,200028,200029,200030);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
-- 
(200001, 29260, 607, 1, 1, 0, 0, 1360.81, -322.18, 36.83, 0.32, 800, 0, 0, 6986, 0, 0, 0),
(200002, 29262, 607, 1, 1, 0, 0, 1363.87, 220.95, 37.06, 2.67, 800, 0, 0, 6986, 0, 0, 0),
--
(200003, 27894, 607, 1, 1, 0, 0, 1421.94, -196.53, 42.18, 0.90, 800, 0, 0, 25000, 0, 0, 0),
(200004, 27894, 607, 1, 1, 0, 0, 1455.09, -219.92, 41.95, 1.03, 800, 0, 0, 25000, 0, 0, 0),
(200005, 27894, 607, 1, 1, 0, 0, 1405.33, 84.25, 41.18, 5.40, 800, 0, 0, 25000, 0, 0, 0),
(200006, 27894, 607, 1, 1, 0, 0, 1436.51, 110.02, 41.40, 5.32, 800, 0, 0, 25000, 0, 0, 0),
(200007, 27894, 607, 1, 1, 0, 0, 1236.33, 92.17, 64.96, 5.68, 800, 0, 0, 25000, 0, 0, 0),
(200008, 27894, 607, 1, 1, 0, 0, 1215.04, 57.73, 64.73, 5.71, 800, 0, 0, 25000, 0, 0, 0),
(200009, 27894, 607, 1, 1, 0, 0, 1232.35, -187.34, 66.94, 0.37, 800, 0, 0, 25000, 0, 0, 0),
(200010, 27894, 607, 1, 1, 0, 0, 1249.95, -223.74, 66.72, 0.43, 800, 0, 0, 25000, 0, 0, 0),
(200011, 27894, 607, 1, 1, 0, 0, 1068.82, -127.38, 96.44, 0.06, 800, 0, 0, 25000, 0, 0, 0),
(200012, 27894, 607, 1, 1, 0, 0, 1068.35, -87.04, 93.80, 6.24, 800, 0, 0, 25000, 0, 0, 0),
-- 
(200013, 28781, 607, 1, 1, 0, 0, 1582.57, -96.37, 8.47, 5.57, 450, 0, 0, 50000, 0, 0, 0),
(200014, 28781, 607, 1, 1, 0, 0, 1610.55, -117.52, 8.77, 2.44, 450, 0, 0, 50000, 0, 0, 0),
(200015, 28781, 607, 1, 1, 0, 0, 1594.24, 39.16, 7.49, 0.62, 450, 0, 0, 50000, 0, 0, 0),
(200016, 28781, 607, 1, 1, 0, 0, 1618.60, 62.26, 7.17, 3.61, 450, 0, 0, 50000, 0, 0, 0),
-- 
(200027, 28781, 607, 1, 1, 0, 0, 1353.28, 224.092, 35.2432, 4.35363, 600, 5, 0, 50000, 0, 0, 0),
(200028, 28781, 607, 1, 1, 0, 0, 1347.65, 208.805, 34.2892, 4.39378, 600, 5, 0, 50000, 0, 0, 0),
(200029, 28781, 607, 1, 1, 0, 0, 1371.2, -317.169, 34.9982, 1.93504, 600, 5, 0, 50000, 0, 0, 0),
(200030, 28781, 607, 1, 1, 0, 0, 1365.52, -301.854, 34.0439, 1.91998, 600, 5, 0, 50000, 0, 0, 0);

DELETE FROM `creature_battleground` WHERE `guid` in (200001,200002,200003,200004,200005,200006,200007,200008,200009,200010,200011,200012,200013,200014,200015,200016,200027,200028,200029,200030);
INSERT INTO `creature_battleground` (`guid` ,`event1` ,`event2`) VALUES
('200001', '7', '0'),
('200002', '7', '0'),
('200003', '7', '0'),
('200004', '7', '0'),
('200005', '7', '0'),
('200006', '7', '0'),
('200007', '7', '0'),
('200008', '7', '0'),
('200009', '7', '0'),
('200010', '7', '0'),
('200011', '7', '0'),
('200012', '7', '0'),
('200013', '7', '0'),
('200014', '7', '0'),
('200015', '7', '0'),
('200016', '7', '0'),
('200027', '9', '0'),
('200028', '9', '0'),
('200029', '10', '0'),
('200030', '10', '0');

DELETE FROM `battleground_events` WHERE `map` in (607);
INSERT INTO `battleground_events` (`map`, `event1`, `event2`, `description`) VALUES
('607', '254', '0', 'Door'),
('607', '6', '0', 'Bomb ALLIANCE'),
('607', '6', '1', 'Bomb HORDE'),
('607', '5', '3', 'Spirit for controll ALLIANCE'),
('607', '5', '4', 'Spirit for controll HORDE'),
('607', '8', '0', 'Spawn all GO'),
('607', '9', '0', 'Spawn Vehicle E'),
('607', '10', '0', 'Spawn Vehicle W'),
('607', '7', '0', 'Spawn all NPC'),
('607', '0', '1', 'A ocup'),
('607', '0', '2', 'H ocup'),
('607', '0', '3', 'A seizes'),
('607', '0', '4', 'H seizes'),
('607', '1', '1', 'A ocup'),
('607', '1', '2', 'H ocup'),
('607', '1', '3', 'A seizes'),
('607', '1', '4', 'H seizes'),
('607', '2', '1', 'A ocup'),
('607', '2', '2', 'H ocup'),
('607', '2', '3', 'A seizes'),
('607', '2', '4', 'H seizes');

DELETE FROM `creature` WHERE `guid` in (200017,200018,200019,200020,200021,200022,200023,200024,200025,200026);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(200017, 13117, 607, 1, 1, 0, 0, 1448.78, -52.9605, 5.74272, 0.0530517, 25, 0, 0, 37420, 6310, 0, 0),
(200018, 13116, 607, 1, 1, 0, 0, 1448.78, -52.9605, 5.74272, 0.0530517, 25, 0, 0, 37890, 6310, 0, 0),
(200019, 13117, 607, 1, 1, 0, 0, 1389.22, 205.673, 32.0597, 4.445, 25, 0, 0, 37420, 6310, 0, 0),
(200020, 13116, 607, 1, 1, 0, 0, 1389.22, 205.673, 32.0597, 4.445, 25, 0, 0, 37890, 6310, 0, 0),
(200021, 13117, 607, 1, 1, 0, 0, 1400.49, -293.963, 32.0608, 2.08096, 25, 0, 0, 37420, 6310, 0, 0),
(200022, 13116, 607, 1, 1, 0, 0, 1400.49, -293.963, 32.0608, 2.08096, 25, 0, 0, 37890, 6310, 0, 0),
(200023, 13117, 607, 1, 1, 0, 0, 1111.58, 6.40605, 69.2963, 6.20037, 25, 0, 0, 37420, 6310, 0, 0),
(200024, 13116, 607, 1, 1, 0, 0, 1111.58, 6.40605, 69.2963, 6.20037, 25, 0, 0, 37890, 6310, 0, 0),
(200025, 13117, 607, 1, 1, 0, 0, 952.963, -192.742, 92.3584, 0.35779, 25, 0, 0, 37420, 6310, 0, 0),
(200026, 13116, 607, 1, 1, 0, 0, 952.963, -192.742, 92.3584, 0.35779, 25, 0, 0, 37890, 6310, 0, 0);
DELETE FROM `creature_battleground` WHERE `guid` in (200017,200018,200019,200020,200021,200022,200023,200024,200025,200026);
INSERT INTO `creature_battleground` (`guid` ,`event1` ,`event2`) VALUES
('200017', '5', '3'),
('200018', '5', '4'),
('200025', '5', '4'),
('200026', '5', '3'),
('200019', '1', '4'),
('200020', '1', '3'),
('200021', '0', '4'),
('200022', '0', '3'),
('200023', '2', '4'),
('200024', '2', '3');

DELETE FROM `mangos_string` WHERE `entry` in (650,651,20000,20001,20002,20003,20004,20005,20006,20007,20008,20009,20010,20011,20012,20013,20014,20015,20016,20017,20018,20019,20020,20021,20022);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(650, 'Alianza', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(651, 'Horda', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20000, 'La batalla por Playa de los Ancestros comenzara en 2 minutos.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20001, 'La batalla por Playa de los Ancestros comenzara en 1 minuto.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20002, 'La batalla por Playa de los Ancestros comenzara en 30 segundos.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20003, '?La batalla por Playa de los Ancestros ha comenzado!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20004, '?Las puertas %s estan bajo asedio!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20005, '?Las puertas %s estan danadas!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20006, '?Las puertas %s han sido destruidas!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20007, 'Esmeralda Verde', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20008, 'Zafiro Azul', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20009, 'Amatista Purpura', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20010, 'Sol Rojo', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20011, 'Luna Amarilla', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20012, 'Camara de Reliquias Antiguas', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20013, '?%s precipita el %s!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20014, '?%s se apodera de %s!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20015, 'Cementerio Este', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20016, 'Cementerio Oeste', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20017, 'Cementerio del Sur', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20018, '$n ha colocado una Carga de Seforio!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20019, '?La Alianza gana la primera ronda, la Reliquia de Titan ha sido capturada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20020, '?La Horda gana la primera ronda, la Reliquia de Titan no ha sido capturada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20021, '?La Horda gana la segunda ronda, la Reliquia de Titan ha sido capturada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20022, '?La Alianza gana la segunda ronda, la Reliquia de Titan no ha sido capturada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

-- For SendWarningToAll(entry)
DELETE FROM `mangos_string` WHERE `entry` IN (20024, 20025, 20026, 20027, 20028, 20029, 20030, 20031, 20032, 20033, 20034, 20035, 20036, 20037, 20038, 20039, 20040, 20041, 20042, 20043, 20044, 20045, 20046, 20047);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(20024, '?La Horda esta conquistando el Cementerio Oriental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20025, '?La Horda esta conquistando el Cementerio Occidental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20026, '?La Horda esta conquistando el Cementerio Sur!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20027, '?La Alianza esta conquistando el Cementerio Oriental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20028, '?La Alianza esta conquistando el Cementerio Occidental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20029, '?La Alianza esta conquistando el Cementerio Sur!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20030, '?La Horda ha conquistado el Cementerio Oriental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20031, '?La Horda ha conquistado el Cementerio Occidental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20032, '?La Horda ha conquistado el Cementerio Sur!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20033, '?La Alianza ha conquistado el Cementerio Oriental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20034, '?La Alianza ha conquistado el Cementerio Occidental!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20035, '?La Alianza ha conquistado el Cementerio Sur!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20036, '?La puerta de la Camara de Reliquias antiguas ha sido destruida!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20037, '?La puerta Esmeralda Verde ha sido destruida!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20038, '?La puerta Zafiro Azul ha sido destruida!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20039, '?La puerta Amatista Purpura ha sido destruida!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20040, '?La puerta Sol Rojo ha sido destruida!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20041, '?La puerta Luna Amarilla ha sido destruida!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),

(20042, '?La puerta de la Camara de Reliquias antiguas esta danada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20043, '?La puerta Esmeralda Verde esta danada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20044, '?La puerta Zafiro Azul esta danada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20045, '?La puerta Amatista Purpura esta danada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20046, '?La puerta Sol Rojo esta danada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(20047, '?La puerta Luna Amarilla esta danada!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

DELETE FROM `gameobject` WHERE `guid` in (200008,200009,200010,200011,200012,200013,200014,200015,200016,200017,200018,200019,200020,200021,200022,200023,200024,200025,200026,200027,200028);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(200008, 180058, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Alliance Banner
(200009, 180059, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Contested Banner
(200010, 180060, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Horde Banner
(200011, 180061, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Contested Banner
(200012, 180100, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Alliance Banner Aura
(200013, 180101, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Horde Banner Aura
(200014, 191311, 607, 1, 1, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0.982541, -0.186048, 86400, 100, 1),-- Flagpole

(200015, 180058, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Alliance Banner
(200016, 180059, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Contested Banner
(200017, 180060, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Horde Banner
(200018, 180061, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Contested Banner
(200019, 180100, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Alliance Banner Aura
(200020, 180101, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Horde Banner Aura
(200021, 191311, 607, 1, 1, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0.989551, 0.144186, 86400, 100, 1),-- Flagpole

(200022, 180058, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),-- Alliance Banner
(200023, 180059, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),-- Contested Banner
(200024, 180060, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),-- Horde Banner
(200025, 180061, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),-- Contested Banner
(200026, 180100, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),-- Alliance Banner Aura
(200027, 180101, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1),-- Horde Banner Aura
(200028, 191311, 607, 1, 1, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 1, -0.00059919, 86400, 100, 1);-- Flagpole

DELETE FROM `gameobject_battleground` WHERE `guid` in (200008,200009,200010,200011,200012,200013,200014,200015,200016,200017,200018,200019,200020,200021,200022,200023,200024,200025,200026,200027,200028,200029,200030,200031,200032,200033,200034,200035,200036,200037,200038,200039,200040,200041,200042,200043,200044,200045,200046,200047,200048,200049,200050,200051,200052,200053,200054,200055,200056,200057,200058,200059,200060,200061,200062,200063,200064,200065,200066,200067,200068,200069,200070,200071,200072,200073,200074,200075,200076,200077,200078,200079,200080,200081,200082,200083,200084,200085,200086);
INSERT INTO `gameobject_battleground` (`guid` ,`event1` ,`event2`) VALUES
('200008', '0', '3'),
('200009', '0', '1'),
('200010', '0', '4'),
('200011', '0', '2'),
('200012', '0', '3'),
('200013', '0', '4'),
('200014', '8', '0'),
('200015', '1', '3'),
('200016', '1', '1'),
('200017', '1', '4'),
('200018', '1', '2'),
('200019', '1', '3'),
('200020', '1', '4'),
('200021', '8', '0'),
('200022', '2', '3'),
('200023', '2', '1'),
('200024', '2', '4'),
('200025', '2', '2'),
('200026', '2', '3'),
('200027', '2', '4'),
('200028', '8', '0'),
('200029', '8', '0'),
('200030', '8', '0'),
('200031', '8', '0'),
('200032', '8', '0'),
('200033', '8', '0'),
('200034', '8', '0'),
('200035', '8', '0'),
('200036', '1', '3'),
('200037', '1', '3'),
('200038', '1', '3'),
('200039', '1', '3'),
('200044', '1', '4'),
('200045', '1', '4'),
('200046', '1', '4'),
('200047', '1', '4'),
('200040', '0', '3'),
('200041', '0', '3'),
('200042', '0', '3'),
('200043', '0', '3'),
('200048', '0', '4'),
('200049', '0', '4'),
('200050', '0', '4'),
('200051', '0', '4'),

('200052', '6', '0'),
('200053', '6', '0'),
('200054', '6', '0'),
('200055', '6', '0'),
('200056', '6', '0'),
('200057', '6', '0'),
('200058', '6', '0'),
('200059', '6', '0'),
('200060', '6', '0'),
('200061', '6', '0'),
('200062', '6', '0'),
('200063', '6', '0'),
('200064', '6', '0'),
('200065', '6', '0'),
('200066', '6', '0'),
('200067', '6', '0'),

('200068', '6', '1'),
('200069', '6', '1'),
('200070', '6', '1'),
('200071', '6', '1'),
('200072', '6', '1'),
('200073', '6', '1'),
('200074', '6', '1'),
('200075', '6', '1'),
('200076', '6', '1'),
('200077', '6', '1'),
('200078', '6', '1'),
('200079', '6', '1'),
('200080', '6', '1'),
('200081', '6', '1'),
('200082', '6', '1'),
('200083', '6', '1'),

('200084', '254', '0'),
('200085', '254', '0'),
('200086', '254', '0');

UPDATE `gameobject_template` Set `ScriptName` = 'go_sa_def_portal' WHERE entry = 191575;
DELETE FROM `gameobject` WHERE `guid` in (200029,200030,200031,200032,200033,200034,200035);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(200029, 191575, 607, 1, 1, 1468.12, -225.7, 30.8969, 5.68042, 0, 0, 0.296839, -0.954928, 25, 0, 1),
(200030, 191575, 607, 1, 1, 1394.07, 72.3632, 31.0541, 0.818809, 0, 0, 0.398063, 0.917358, 25, 0, 1),
(200031, 191575, 607, 1, 1, 1216.12, 47.7665, 54.2785, 4.05465, 0, 0, 0.897588, -0.440835, 25, 0, 1),
(200032, 191575, 607, 1, 1, 1255.73, -233.153, 56.4357, 5.01833, 0, 0, 0.591105, -0.806595, 25, 0, 1),
(200033, 191575, 607, 1, 1, 1065.02, -89.9522, 81.0758, 1.58771, 0, 0, 0.71306, 0.701103, 25, 0, 1),
(200034, 191575, 607, 1, 1, 880.162, -95.979, 109.835, 3.14672, 0, 0, 0.999997, -0.00256531, 25, 0, 1),
(200035, 191575, 607, 1, 1, 880.68, -120.799, 109.835, 3.16636, 0, 0, 0.999923, -0.0123816, 25, 0, 1);


DELETE FROM `gameobject` WHERE `guid` in (200052,200053,200054,200055,200056,200057,200058,200059,200060,200061,200062,200063,200064,200065,200066,200067,200068,200069,200070,200071,200072,200073,200074,200075,200076,200077,200078,200079,200080,200081,200082,200083,200084,200085,200086);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(200052, 190753, 607, 1, 1, 1619.71, -122.735, 9.08799, 4.11749, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(200068, 194086, 607, 1, 1, 1619.71, -122.735, 9.08799, 4.11749, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(200053, 190753, 607, 1, 1, 1620.92, -120.946, 9.08799, 4.11749, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(200069, 194086, 607, 1, 1, 1620.92, -120.946, 9.08799, 4.11749, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(200054, 190753, 607, 1, 1, 1622.24, -119.09, 9.08799, 4.1332, 0, 0, 0.879586, -0.475739, 25, 0, 1),
(200070, 194086, 607, 1, 1, 1622.24, -119.09, 9.08799, 4.1332, 0, 0, 0.879586, -0.475739, 25, 0, 1),
(200055, 190753, 607, 1, 1, 1623.53, -117.128, 9.08799, 4.1332, 0, 0, 0.879586, -0.475739, 25, 0, 1),
(200071, 194086, 607, 1, 1, 1623.53, -117.128, 9.08799, 4.1332, 0, 0, 0.879586, -0.475739, 25, 0, 1),
(200056, 190753, 607, 1, 1, 1571.3, -95.7584, 8.45841, 4.12299, 0, 0, 0.882004, -0.471242, 25, 0, 1),
(200072, 194086, 607, 1, 1, 1571.3, -95.7584, 8.45841, 4.12299, 0, 0, 0.882004, -0.471242, 25, 0, 1),
(200057, 190753, 607, 1, 1, 1572.54, -93.9067, 8.45695, 4.12299, 0, 0, 0.882004, -0.471242, 25, 0, 1),
(200073, 194086, 607, 1, 1, 1572.54, -93.9067, 8.45695, 4.12299, 0, 0, 0.882004, -0.471242, 25, 0, 1),
(200058, 190753, 607, 1, 1, 1573.7, -92.1515, 8.45554, 4.12692, 0, 0, 0.881077, -0.472973, 25, 0, 1),
(200074, 194086, 607, 1, 1, 1573.7, -92.1515, 8.45554, 4.12692, 0, 0, 0.881077, -0.472973, 25, 0, 1),
(200059, 190753, 607, 1, 1, 1574.72, -90.5222, 8.45269, 4.17011, 0, 0, 0.870657, -0.49189, 25, 0, 1),
(200075, 194086, 607, 1, 1, 1574.72, -90.5222, 8.45269, 4.17011, 0, 0, 0.870657, -0.49189, 25, 0, 1),
(200060, 190753, 607, 1, 1, 1586.23, 32.0426, 7.79361, 2.37156, 0, 0, 0.926792, 0.375575, 25, 0, 1),
(200076, 194086, 607, 1, 1, 1586.23, 32.0426, 7.79361, 2.37156, 0, 0, 0.926792, 0.375575, 25, 0, 1),
(200061, 190753, 607, 1, 1, 1587.73, 30.6612, 7.79361, 2.39905, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(200077, 194086, 607, 1, 1, 1587.73, 30.6612, 7.79361, 2.39905, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(200062, 190753, 607, 1, 1, 1589.32, 29.2037, 7.79361, 2.39905, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(200078, 194086, 607, 1, 1, 1589.32, 29.2037, 7.79361, 2.39905, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(200063, 190753, 607, 1, 1, 1591.09, 27.585, 7.79361, 2.40297, 0, 0, 0.932577, 0.360971, 25, 0, 1),
(200079, 194086, 607, 1, 1, 1591.09, 27.585, 7.79361, 2.40297, 0, 0, 0.932577, 0.360971, 25, 0, 1),
(200064, 190753, 607, 1, 1, 1621.55, 74.7503, 7.16584, 2.35192, 0, 0, 0.92306, 0.384655, 25, 0, 1),
(200080, 194086, 607, 1, 1, 1621.55, 74.7503, 7.16584, 2.35192, 0, 0, 0.92306, 0.384655, 25, 0, 1),
(200065, 190753, 607, 1, 1, 1622.85, 73.5074, 7.16443, 2.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(200081, 194086, 607, 1, 1, 1622.85, 73.5074, 7.16443, 2.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(200066, 190753, 607, 1, 1, 1624.15, 72.2676, 7.16443, 2.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(200082, 194086, 607, 1, 1, 1624.15, 72.2676, 7.16443, 2.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(200067, 190753, 607, 1, 1, 1625.5, 70.9844, 7.16443, 2.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(200083, 194086, 607, 1, 1, 1625.5, 70.9844, 7.16443, 2.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1);

DELETE FROM `gameobject` WHERE `guid` in (200036,200037,200038,200039,200040,200041,200042,200043,200044,200045,200046,200047,200048,200049,200050,200051);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(200036, 190753, 607, 1, 1, 1352.69, 237.328, 34.9859, 2.95982, 0, 0, 0.995873, 0.0907596, 60, 0, 1),
(200044, 194086, 607, 1, 1, 1352.69, 237.328, 34.9859, 2.95982, 0, 0, 0.995873, 0.0907596, 60, 0, 1),
(200037, 190753, 607, 1, 1, 1356.65, 236.624, 35.0095, 2.99124, 0, 0, 0.997176, 0.0751059, 60, 0, 1),
(200045, 194086, 607, 1, 1, 1356.65, 236.624, 35.0095, 2.99124, 0, 0, 0.997176, 0.0751059, 60, 0, 1),
(200038, 190753, 607, 1, 1, 1359.74, 236.177, 35.0288, 2.99517, 0, 0, 0.997321, 0.0731478, 60, 0, 1),
(200046, 194086, 607, 1, 1, 1359.74, 236.177, 35.0288, 2.99517, 0, 0, 0.997321, 0.0731478, 60, 0, 1),
(200039, 190753, 607, 1, 1, 1363.2, 235.804, 35.0499, 3.03836, 0, 0, 0.998668, 0.0515918, 60, 0, 1),
(200047, 194086, 607, 1, 1, 1363.2, 235.804, 35.0499, 3.03836, 0, 0, 0.998668, 0.0515918, 60, 0, 1),
(200040, 190753, 607, 1, 1, 1379.51, -325.782, 34.7503, 0.717493, 0, 0, 0.351101, 0.936338, 60, 0, 1),
(200048, 194086, 607, 1, 1, 1379.51, -325.782, 34.7503, 0.717493, 0, 0, 0.351101, 0.936338, 60, 0, 1),
(200041, 190753, 607, 1, 1, 1376.67, -327.915, 34.7709, 0.713566, 0, 0, 0.349262, 0.937025, 60, 0, 1),
(200049, 194086, 607, 1, 1, 1376.67, -327.915, 34.7709, 0.713566, 0, 0, 0.349262, 0.937025, 60, 0, 1),
(200042, 190753, 607, 1, 1, 1373.95, -330.271, 34.7918, 0.713566, 0, 0, 0.349262, 0.937025, 60, 0, 1),
(200050, 194086, 607, 1, 1, 1373.95, -330.271, 34.7918, 0.713566, 0, 0, 0.349262, 0.937025, 60, 0, 1),
(200043, 190753, 607, 1, 1, 1371.55, -332.456, 34.8104, 0.741055, 0, 0, 0.362107, 0.932136, 60, 0, 1),
(200051, 194086, 607, 1, 1, 1371.55, -332.456, 34.8104, 0.741055, 0, 0, 0.362107, 0.932136, 60, 0, 1);

DELETE FROM `gameobject` WHERE `guid` in (200084,200085,200086);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(200084, 300000, 607, 1, 1, 1803.93, -168.457, 60.549, 2.74522, 0, 0, 0.980426, 0.196889, 250000, 0, 1),
(200085, 300000, 607, 1, 1, 1803.71, 118.601, 59.8234, 3.56313, 0, 0, 0.97787, -0.209212, 250000, 0, 1),
(200086, 184719, 607, 1, 1, 1200.67, -67.87, 70.08, 6.28297, 0, 0, 0.000106724, -1, 250000, 0, 1);

-- Delete the defender's forcefield
DELETE FROM gameobject WHERE guid = '200086';
DELETE FROM game_event_gameobject WHERE guid = '200086';
DELETE FROM gameobject_battleground WHERE guid = '200086';