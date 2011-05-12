-- Pet spell auras

DELETE FROM `spell_pet_auras` WHERE `aura`='54566'; -- Deathknight Pet Scaling
DELETE FROM `spell_pet_auras` WHERE `spell` IN (34455,34459,34460); -- Ferocious Inspiration
DELETE FROM `spell_pet_auras` WHERE `spell`='0' AND `pet` IN (1,89,416,417,510,1860,1863,15352,15438,17252,19668,24207,26125,27829,28017,29264,31216,37994);
INSERT INTO `spell_pet_auras`
    (`spell`, `effectId`, `pet`, `aura`)
VALUES
    -- Hunter Pets
    (0,0,1,34902),
    (0,0,1,34903),
    (0,0,1,34904),
    (0,0,1,61017),
    -- Warlock Infernal
    (0,0,89,34947),
    (0,0,89,34956),
    (0,0,89,34957),
    (0,0,89,34958),
    (0,0,89,61013),
    -- Warkock Imp
    (0,0,416,34947),
    (0,0,416,34956),
    (0,0,416,34957),
    (0,0,416,34958),
    (0,0,416,61013),
    -- Warlock Felhunter
    (0,0,417,34947),
    (0,0,417,34956),
    (0,0,417,34957),
    (0,0,417,34958),
    (0,0,417,61013),
    -- Mage Water Elemental (old guardian)
    (0,0,510,34947),
    (0,0,510,34956),
    -- Warlock Voidwalker
    (0,0,1860,34947),
    (0,0,1860,34956),
    (0,0,1860,34957),
    (0,0,1860,34958),
    (0,0,1860,61013),
    -- Warlock Succubus
    (0,0,1863,34947),
    (0,0,1863,34956),
    (0,0,1863,34957),
    (0,0,1863,34958),
    (0,0,1863,61013),
    -- Greater Earth Elemetal
    (0,0,15352,7941),
    (0,0,15352,34947),
    -- Shaman Greater Fire Elemetal
    (0,0,15438,7942),
    (0,0,15438,34956),
    (0,0,15438,34947),
    -- Warlock Felguard
    (0,0,17252,34947),
    (0,0,17252,34956),
    (0,0,17252,34957),
    (0,0,17252,34958),
    (0,0,17252,61013),
    -- Shadowfiend
    (0,0,19668,34947),
    -- Deathknight Army of the Dead Ghoul
    (0,0,24207,51996),
    (0,0,24207,54566),
    (0,0,24207,61697),
    -- Deathknight Ghoul
    (0,0,26125,51996),
    (0,0,26125,54566),
    (0,0,26125,61697),
    -- DK Gargoyle
    (0,0,27829,54566),
    -- Deathknight Bloodworms
    (0,0,28017,50453),
    -- Shaman Wolf
    (0,0,29264,58877),
    (0,0,29264,34902),
    (0,0,29264,34903),
    (0,0,29264,34904),
    (0,0,29264,61783),
    -- Mage Mirror Image
    (0,0,31216,34947),
    -- Mage Water Elemental
    (0,0,37994,34947),
    (0,0,37994,34956),
    -- Ferocious Inspiration
    (34455,0,0,75593),
    (34459,0,0,75446),
    (34460,0,0,75447);

-- Need correct spellcasting for this!
-- UPDATE creature_template SET spell1 = 12470, spell2 = 57984 WHERE entry = 15438;
-- UPDATE creature_template SET spell1 = 36213 WHERE entry = 15352;
UPDATE `creature_template` SET `spell1`='0', `spell2`='0' WHERE `entry`='15438';
UPDATE `creature_template` SET `spell1`= '0', `spell2`='0' WHERE `entry`='15352';
UPDATE `creature_template` SET `spell1`='40133' WHERE `entry`='15439';
UPDATE `creature_template` SET `spell1`='40132' WHERE `entry`='15430';

DELETE FROM `creature_template_addon` WHERE `entry`='89';
INSERT INTO `creature_template_addon`
    (`entry`, `auras`)
VALUES
    (89, 39007);

DELETE FROM `event_scripts` WHERE `id` IN (14859,14858);
INSERT INTO `event_scripts`
    (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`)
VALUES
    (14858, 1, 15, 33663, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Summon greater Earth elemental'),
    (14859, 1, 15, 32982, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Summon greater Fire  elemental');
