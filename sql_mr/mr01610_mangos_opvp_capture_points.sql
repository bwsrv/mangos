
-- creature and gameobject initial guid
SET @CREATURE := 500000;
SET @GAMEOBJECT := 500000;

-- cleanup
DELETE FROM `creature` WHERE `guid` >= @CREATURE AND `guid` <= (@CREATURE + 1000);
DELETE FROM `gameobject` WHERE `guid` >= @GAMEOBJECT AND `guid` <= (@GAMEOBJECT + 1000);

-- Missing capture points spawns
/* ################################# */
-- use guids from 500xxx
DELETE FROM `gameobject` WHERE `id` in (181899,182096,182097,182098,182174,182173,182175,182522,182523,182528,182529,183104,183411,183412,183413,183414,182210);
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
-- eastern plaguelads
(@GAMEOBJECT+1, 181899, 0, 1, 1, 3181.08, -4379.36, 174.123, -2.03472, -0.065392, 0.119494, -0.842275, 0.521553, 900, 100, 1),
(@GAMEOBJECT+2, 182096, 0, 1, 1, 1860.85, -3731.23, 196.716, -2.53214, 0.033967, -0.131914, 0.944741, -0.298177, 180, 100, 1),
(@GAMEOBJECT+3, 182097, 0, 1, 1, 2574.51, -4794.89, 144.704, -1.45003, -0.097056, 0.095578, -0.656229, 0.742165, 180, 100, 1),
(@GAMEOBJECT+4, 182098, 0, 1, 1, 2962.71, -3042.31, 154.789, 2.08426, -0.074807, -0.113837, 0.855928, 0.49883, 900, 100, 1),
-- hellfire
(@GAMEOBJECT+5, 182174, 530, 1, 1, -184.889, 3476.93, 38.205, -0.017453, 0, 0, -0.00872639, 0.999962, 180, 100, 1),
(@GAMEOBJECT+6, 182173, 530, 1, 1, -290.016, 3702.42, 56.6729, 0.034907, 0, 0, 0.0174526, 0.999848, 180, 100, 1),
(@GAMEOBJECT+7, 182175, 530, 1, 1, -471.462, 3451.09, 34.6432, 0.174533, 0, 0, 0.0871558, 0.996195, 180, 100, 1),
-- zangamarsh
(@GAMEOBJECT+8, 182522, 530, 1, 1, 336.466, 7340.26, 41.4984, -1.58825, 0, 0, -0.713251, 0.700909, 180, 100, 1),
(@GAMEOBJECT+9, 182523, 530, 1, 1, 303.243, 6841.36, 40.1245, -1.58825, 0, 0, -0.713251, 0.700909, 180, 100, 1),
(@GAMEOBJECT+10, 182528, 530, 1, 1, 253.535, 7083.8, 36.9946, -0.0174535, 0, 0, -0.00872664, 0.999962, -300, 100, 1),
(@GAMEOBJECT+11, 182529, 530, 1, 1, 253.535, 7083.8, 36.9946, -0.0174535, 0, 0, -0.00872664, 0.999962, 300, 100, 1),
-- terokkar
(@GAMEOBJECT+12, 183104, 530, 1, 1, -3081.65, 5335.03, 17.1853, -2.14675, 0, 0, -0.878816, 0.477161, 180, 100, 1),
(@GAMEOBJECT+13, 183411, 530, 1, 1, -2939.9, 4788.73, 18.987, 2.77507, 0, 0, 0.983255, 0.182237, 180, 100, 1),
(@GAMEOBJECT+14, 183412, 530, 1, 1, -3174.94, 4440.97, 16.2281, 1.8675, 0, 0, 0.803856, 0.594824, 180, 100, 1),
(@GAMEOBJECT+15, 183413, 530, 1, 1, -3603.31, 4529.15, 20.9077, 0.994838, 0, 0, 0.477159, 0.878817, 180, 100, 1),
(@GAMEOBJECT+16, 183414, 530, 1, 1, -3812.37, 4899.3, 17.7249, 0.087266, 0, 0, 0.0436192, 0.999048, 180, 100, 1),
-- Halaa
(@GAMEOBJECT+17, 182210, 530, 1, 1, -1572.57, 7945.3, -22.475, 2.05949, 0, 0, 0.857168, 0.515037, 180, 100, 1);
/* ################################# */

-- Remove graveyard links - they are set in script
DELETE FROM game_graveyard_zone WHERE id IN (969, 927, 993);

-- Venture bay fixes
/* ################################# */
-- creatures for world pvp - use guid from 300xxx
DELETE FROM `creature` WHERE `id` IN (27730,27760);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(@CREATURE+1, 27730, 571, 1, 1, 0, 0, 2489.92, -1821.4, 11.7098, 5.51804, 604800, 0, 0, 10282, 0, 0, 0),
(@CREATURE+2, 27760, 571, 1, 1, 0, 0, 2483.68, -1836.49, 11.8173, 0.734973, 604800, 0, 0, 20564, 0, 0, 0);
/* ################################# */


-- Zangarmarsh fixes
/* ################################# */
-- the red and blue lights need some updates - from ytdb
DELETE FROM `creature` WHERE `id` IN (18759,18757);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(@CREATURE+3, 18759, 530, 1, 1, 0, 0, 273.866, 7082.68, 87.0635, 3.01942, 180, 0, 0, 1, 0, 0, 0),
(@CREATURE+4, 18759, 530, 1, 1, 0, 0, 375.806, 7332.96, 63.1065, 2.94961, 180, 0, 0, 1, 0, 0, 0),
(@CREATURE+5, 18759, 530, 1, 1, 0, 0, 340.453, 6833.1, 61.798, 3.01942, 180, 0, 0, 1, 0, 0, 0),
(@CREATURE+6, 18757, 530, 1, 1, 0, 0, 273.866, 7082.68, 87.0635, 3.01942, 180, 0, 0, 1, 0, 0, 0),
(@CREATURE+7, 18757, 530, 1, 1, 0, 0, 375.806, 7332.96, 63.1065, 2.94961, 180, 0, 0, 1, 0, 0, 0),
(@CREATURE+8, 18757, 530, 1, 1, 0, 0, 340.453, 6833.1, 61.798, 3.01942, 180, 0, 0, 1, 0, 0, 0);
UPDATE `creature` SET `id` = 18757, `position_x` = 340.453, `position_y` = 6833.1, `position_z` = 61.798 WHERE `guid` = 67037;
UPDATE `creature` SET `MovementType` = 0, `spawndist` = 0 WHERE `id` IN (18757,18759);
-- negative spawntimes for Zanga banner
UPDATE `gameobject` SET `spawntimesecs` = -181 WHERE `id` = 182527;
-- red aura in c_t_a
DELETE FROM `creature_template_addon` WHERE (`entry`=18757);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`, `auras`) VALUES (18757, 0, 0, 1, 0, 0, 0, 32839);
/* ################################# */


-- Zangarmarsh gossip - may not be 100% blizzlike
/* ################################# */
-- Set custom menu id for the horde
Update creature_template set gossip_menu_id = 7723 where entry = 18564;
-- links the gossip menu with the npc text for the horde npc
delete from gossip_menu where entry in (7723);
insert into gossip_menu (entry, text_id) values
(7723,9431);
-- Alliance & horde - stores the text and the script id
delete from gossip_menu_option where menu_id in (30001,30002);
insert into gossip_menu_option (menu_id, id, option_icon, option_text, option_id, npc_option_npcflag, action_script_id) values
(7724,0,0,'[PH] Zangarmarsh PvP Banner',1,1,@CREATURE+1),
(7723,1,1,'I have marks to redeem!',3,128,0),
(7723,0,0,'[PH] Zangarmarsh PvP Banner',1,1,@CREATURE+2);
-- Alliance & horde - stores the gossip script
delete from gossip_scripts where id in (@CREATURE*10+1,@CREATURE*10+2);
insert into gossip_scripts(id, delay, command, datalong, datalong2, comments) values
(@CREATURE*10+1,0,15,32430,0,'Cast Battle Standard - Alliance'),
(@CREATURE*10+2,0,15,32431,0,'Cast Battle Standard - Horde');
/* ################################# */


-- Eastern Plaguelands fixes
/* ################################# */
-- set shrine respawn
UPDATE `gameobject` SET `spawntimesecs` = -180 WHERE `id` in (181682, 181955);
-- correct horde faction
UPDATE `gameobject_template` SET `faction` = 1314 WHERE `entry` = 181955;
-- set flightmaster movement to idle
UPDATE `creature_template` SET `MovementType` = 0 WHERE `entry` = 17209;

-- set soldiers movement
UPDATE `creature_template` SET `MovementType` = 0 WHERE `entry` IN (17647,17996);
UPDATE `creature_template` SET `MovementType` = 2 WHERE `entry` in (17635,17995);
DELETE FROM `creature_movement_template` WHERE `entry` in (17635, 17995);
INSERT INTO `creature_movement_template` (`entry`, `point`, `position_x`, `position_y`, `position_z`) VALUES
 (17635, 1, 2501.349, -4725.994, 90.974),
 (17635, 2, 2491.211, -4693.162, 82.363),
 (17635, 3, 2493.059, -4655.492, 75.194),
 (17635, 4, 2562.227, -4646.404, 79.003),
 (17635, 5, 2699.748, -4567.377, 87.460),
 (17635, 6, 2757.274, -4527.591, 89.080),
 (17635, 7, 2850.868, -4417.565, 89.421),
 (17635, 8, 2888.340, -4328.486, 90.562),
 (17635, 9, 2913.271, -4167.140, 93.919),
 (17635, 10, 3035.656, -4260.176, 96.141),
 (17635, 11, 3088.544, -4250.208, 97.769),
 (17635, 12, 3147.302, -4318.796, 130.410),
 (17635, 13, 3166.813, -4349.198, 137.569),
 
 (17995, 1, 2501.349, -4725.994, 90.974),
 (17995, 2, 2491.211, -4693.162, 82.363),
 (17995, 3, 2493.059, -4655.492, 75.194),
 (17995, 4, 2562.227, -4646.404, 79.003),
 (17995, 5, 2699.748, -4567.377, 87.460),
 (17995, 6, 2757.274, -4527.591, 89.080),
 (17995, 7, 2850.868, -4417.565, 89.421),
 (17995, 8, 2888.340, -4328.486, 90.562),
 (17995, 9, 2913.271, -4167.140, 93.919),
 (17995, 10, 3035.656, -4260.176, 96.141),
 (17995, 11, 3088.544, -4250.208, 97.769),
 (17995, 12, 3147.302, -4318.796, 130.410),
 (17995, 13, 3166.813, -4349.198, 137.569);
 
 -- creature linking for EP soldiers
 INSERT IGNORE INTO creature_linking_template VALUES
 (17647, 0, 17635, 515),
 (17996, 0, 17995, 515);
 /* ################################# */
 
 -- Halaa fixes
/* ################################# */
-- fire bomb target
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=18225;
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=18225;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
('1822501','18225','11','0','100','0','0','0','0','0','11','31961','0','0','0','0','0','0','0','0','0','0','Fire Bomb Target - Cast Fire Bomb on Spawn');

-- spawn npcs and gameobjects on the map
DELETE FROM `creature` WHERE `id` IN (18817,18822,21485,21487,21488,18256);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
-- alliance
(@CREATURE+9, 18817, 530, 1, 1, 0, 0, -1591.18, 8020.39, -22.2042, 4.59022, 600, 0, 0, 59115, 0, 0, 0),
(@CREATURE+10, 18822, 530, 1, 1, 0, 0, -1588.12, 8019.44, -22.2042, 4.06662, 600, 0, 0, 88710, 0, 0, 0),
(@CREATURE+11, 21485, 530, 1, 1, 0, 0, -1521.93, 7927.37, -20.2299, 3.24631, 600, 0, 0, 104790, 0, 0, 0),
(@CREATURE+12, 21487, 530, 1, 1, 0, 0, -1540.33, 7971.95, -20.7186, 3.07178, 600, 0, 0, 104790, 0, 0, 0),
(@CREATURE+13, 21488, 530, 1, 1, 0, 0, -1570.01, 7993.8, -22.4505, 5.02655, 600, 0, 0, 104790, 0, 0, 0),
-- soldiers
(@CREATURE+14, 18256, 530, 1, 1, 0, 0, -1654.06, 8000.46, -26.59, 3.37, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+15, 18256, 530, 1, 1, 0, 0, -1487.18, 7899.1, -19.53, 0.954, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+16, 18256, 530, 1, 1, 0, 0, -1480.88, 7908.79, -19.19, 4.485, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+17, 18256, 530, 1, 1, 0, 0, -1540.56, 7995.44, -20.45, 0.947, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+18, 18256, 530, 1, 1, 0, 0, -1546.95, 8000.85, -20.72, 6.035, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+19, 18256, 530, 1, 1, 0, 0, -1595.31, 7860.53, -21.51, 3.747, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+20, 18256, 530, 1, 1, 0, 0, -1642.31, 7995.59, -25.8, 3.317, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+21, 18256, 530, 1, 1, 0, 0, -1545.46, 7995.35, -20.63, 1.094, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+22, 18256, 530, 1, 1, 0, 0, -1487.58, 7907.99, -19.27, 5.567, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+23, 18256, 530, 1, 1, 0, 0, -1651.54, 7988.56, -26.52, 2.984, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+24, 18256, 530, 1, 1, 0, 0, -1602.46, 7866.43, -22.11, 4.747, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+25, 18256, 530, 1, 1, 0, 0, -1591.22, 7875.29, -22.35, 4.345, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+26, 18256, 530, 1, 1, 0, 0, -1603.75, 8000.36, -24.18, 4.516, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+27, 18256, 530, 1, 1, 0, 0, -1585.73, 7994.68, -23.29, 4.439, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+28, 18256, 530, 1, 1, 0, 0, -1595.5, 7991.27, -23.53, 4.738, 600, 0, 0, 1182800, 0, 0, 0);
DELETE FROM `creature` WHERE `id` IN (18816,18821,21474,21484,21483,18192);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
-- horde
(@CREATURE+29, 18816, 530, 1, 1, 0, 0, -1523.92, 7951.76, -17.6942, 3.51172, 600, 0, 0, 59115, 0, 0, 0),
(@CREATURE+30, 18821, 530, 1, 1, 0, 0, -1527.75, 7952.46, -17.6948, 3.99317, 600, 0, 0, 88710, 0, 0, 0),
(@CREATURE+31, 21474, 530, 1, 1, 0, 0, -1520.14, 7927.11, -20.2527, 3.39389, 600, 0, 0, 104790, 0, 0, 0),
(@CREATURE+32, 21484, 530, 1, 1, 0, 0, -1524.84, 7930.34, -20.182, 3.6405, 600, 0, 0, 104790, 0, 0, 0),
(@CREATURE+33, 21483, 530, 1, 1, 0, 0, -1570.01, 7993.8, -22.4505, 5.02655, 600, 0, 0, 104790, 0, 0, 0),
-- soldiers
(@CREATURE+34, 18192, 530, 1, 1, 0, 0, -1654.06, 8000.46, -26.59, 3.37, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+35, 18192, 530, 1, 1, 0, 0, -1487.18, 7899.1, -19.53, 0.954, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+36, 18192, 530, 1, 1, 0, 0, -1480.88, 7908.79, -19.19, 4.485, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+37, 18192, 530, 1, 1, 0, 0, -1540.56, 7995.44, -20.45, 0.947, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+38, 18192, 530, 1, 1, 0, 0, -1546.95, 8000.85, -20.72, 6.035, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+39, 18192, 530, 1, 1, 0, 0, -1595.31, 7860.53, -21.51, 3.747, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+40, 18192, 530, 1, 1, 0, 0, -1642.31, 7995.59, -25.8, 3.317, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+41, 18192, 530, 1, 1, 0, 0, -1545.46, 7995.35, -20.63, 1.094, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+42, 18192, 530, 1, 1, 0, 0, -1487.58, 7907.99, -19.27, 5.567, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+43, 18192, 530, 1, 1, 0, 0, -1651.54, 7988.56, -26.52, 2.984, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+44, 18192, 530, 1, 1, 0, 0, -1602.46, 7866.43, -22.11, 4.747, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+45, 18192, 530, 1, 1, 0, 0, -1591.22, 7875.29, -22.35, 4.345, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+46, 18192, 530, 1, 1, 0, 0, -1550.6, 7944.45, -21.63, 3.559, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+47, 18192, 530, 1, 1, 0, 0, -1545.57, 7935.83, -21.13, 3.448, 600, 0, 0, 1182800, 0, 0, 0),
(@CREATURE+48, 18192, 530, 1, 1, 0, 0, -1550.86, 7937.56, -21.7, 3.801, 600, 0, 0, 1182800, 0, 0, 0);

-- Gameobjects
-- horde control
DELETE FROM `gameobject` WHERE `id` IN (182281,182282,182273,182274,182266,182275,182276,182277);
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(@GAMEOBJECT+18, 182281, 530, 1, 1, -1385.07, 7779.34, -11.2055, 0.785398, 0, 0, 0.382683, 0.92388, 180, 100, 1),
(@GAMEOBJECT+19, 182282, 530, 1, 1, -1649.99, 7732.57, -15.4506, -2.40855, 0, 0, -0.93358, 0.35837, 180, 100, 1),

(@GAMEOBJECT+20, 182273, 530, 1, 1, -1389.53, 7782.51, -11.6257, -1.51844, 0, 0, -0.688356, 0.725373, 180, 100, 1),
(@GAMEOBJECT+21, 182274, 530, 1, 1, -1662.28, 7735, -15.9663, 1.88495, 0, 0, 0.809015, 0.587788, 180, 100, 1),

(@GAMEOBJECT+22, 182266, 530, 1, 1, -1815.82, 8036.64, -26.2491, -2.89725, 0, 0, -0.992546, 0.121868, -300, 100, 1),
(@GAMEOBJECT+23, 182275, 530, 1, 1, -1507.9, 8132.11, -19.553, -1.3439, 0, 0, -0.622513, 0.782609, -300, 100, 1),
(@GAMEOBJECT+24, 182276, 530, 1, 1, -1384.53, 7779.4, -11.17, -0.575959, 0, 0, -0.284016, 0.95882, -300, 100, 1),
(@GAMEOBJECT+25, 182277, 530, 1, 1, -1650.28, 7732.19, -15.4418, -2.80998, 0, 0, -0.986286, 0.165048, -300, 100, 1);

-- alliance in control
DELETE FROM `gameobject` WHERE `id` IN (182297,182298,182299,182300);
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES

(@GAMEOBJECT+26, 182297, 530, 1, 1, -1815.8, 8036.51, -26.2354, -2.89725, 0, 0, -0.992546, 0.121868, 180, 100, 1),
(@GAMEOBJECT+27, 182298, 530, 1, 1, -1507.95, 8132.1, -19.5585, -1.3439, 0, 0, -0.622513, 0.782609, 180, 100, 1),
(@GAMEOBJECT+28, 182299, 530, 1, 1, -1384.52, 7779.33, -11.1663, -0.575959, 0, 0, -0.284016, 0.95882, 180, 100, 1),
(@GAMEOBJECT+29, 182300, 530, 1, 1, -1650.11, 7732.56, -15.4505, -2.80998, 0, 0, -0.986286, 0.165048, 180, 100, 1);

UPDATE `gameobject` SET `spawntimesecs` = -180 WHERE `id` IN (182301,182302,182303,182304,182305,182306,182307,182308,182297,182298,182299,182300);
UPDATE `gameobject` SET `spawntimesecs` = -180 WHERE `id` IN (182267,182280,182281,182282,182222,182272,182273,182274,182266,182275,182276,182277);
/* ################################# */
