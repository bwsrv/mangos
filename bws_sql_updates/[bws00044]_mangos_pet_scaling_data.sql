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