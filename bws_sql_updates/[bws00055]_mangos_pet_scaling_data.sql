-- Pet 1 - default hunter pet
UPDATE pet_scaling_data SET sta = 75, apbasescale = 428 WHERE creature_entry = 1 AND aura = 0;
UPDATE pet_scaling_data SET sta = 15 WHERE creature_entry = 1 AND aura = 62758;
UPDATE pet_scaling_data SET sta = 30 WHERE creature_entry = 1 AND aura = 62762;

UPDATE pet_scaling_data SET sta = 66 WHERE creature_entry = 27829; -- Guardian 27829 - DK Gargoyle

-- Army of the Dead Ghoul
DELETE FROM `spell_pet_auras` WHERE `aura` = 54566;
DELETE FROM `spell_pet_auras` WHERE `spell` = 0 AND `pet` = 24207;
INSERT INTO `spell_pet_auras` VALUES
(0,0,24207,51996),
(0,0,24207,54566),
(0,0,24207,61697);

-- Army of the Dead Ghoul
DELETE FROM `pet_levelstats` WHERE `creature_entry` = 24207;
INSERT INTO `pet_levelstats` (`creature_entry`, `level`, `hp`, `mana`, `armor`, `mindmg`, `maxdmg`, `attackpower`, `str`, `agi`, `sta`, `inte`, `spi`) VALUES
(24207,  1,   42, 0, 20, 2, 3, 10, 22, 0, 22, 0, 0),
(24207, 80, 5200, 0, 4000, 400, 500, 24, 62, 0, 92, 0, 0);

-- Shadowfiend
UPDATE pet_scaling_data SET sta = 50 WHERE creature_entry = 19668;
DELETE FROM `pet_levelstats` WHERE `creature_entry` = 19668;
INSERT INTO `pet_levelstats` (`creature_entry`, `level`, `hp`, `mana`, `armor`, `mindmg`, `maxdmg`, `attackpower`, `str`, `agi`, `sta`, `inte`, `spi`) VALUES
(19668, 1, 140, 48, 20, 2, 3, 10, 20, 20, 20, 24, 23),
(19668, 66, 6357, 3191, 1872, 0, 0, 0, 141, 33, 97, 306, 255),
(19668, 67, 6401, 3244, 1900, 0, 0, 0, 142, 33, 98, 313, 258),
(19668, 68, 6429, 3298, 1987, 0, 0, 0, 143, 34, 98, 320, 260),
(19668, 69, 6464, 3351, 2008, 0, 0, 0, 144, 36, 100, 327, 262),
(19668, 70, 6528, 3568, 2207, 138, 174, 135, 145, 39, 101, 327, 263),
(19668, 71, 6580, 3632, 2319, 0, 0, 0, 148, 40, 102, 342, 269),
(19668, 72, 6639, 3695, 2431, 0, 0, 0, 150, 41, 104, 351, 274),
(19668, 73, 6699, 3763, 2544, 0, 0, 0, 153, 44, 104, 358, 280),
(19668, 74, 6761, 3830, 2660, 0, 0, 0, 156, 45, 108, 365, 283),
(19668, 75, 6821, 3896, 2760, 0, 0, 0, 159, 46, 109, 373, 288),
(19668, 76, 6882, 3964, 2890, 0, 0, 0, 164, 48, 110, 381, 295),
(19668, 77, 6949, 4033, 2999, 0, 0, 0, 166, 49, 113, 390, 300),
(19668, 78, 7011, 4100, 3110, 0, 0, 0, 168, 50, 114, 397, 308),
(19668, 79, 7073, 4167, 3150, 0, 0, 0, 171, 51, 115, 392, 313),
(19668, 80, 7129, 8228, 6273, 1560, 1600, 200, 297, 79, 118, 420, 367); -- hard hack