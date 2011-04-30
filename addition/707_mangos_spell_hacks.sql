/* Arcane Potency */
DELETE FROM spell_chain WHERE spell_ID IN (57529, 57531);
INSERT INTO spell_chain	(spell_id, prev_spell, first_spell, rank, req_spell) VALUES
(57529, 0, 57529, 1, 0),
(57531,57529, 57529, 2,0);

DELETE FROM spell_proc_event WHERE entry IN (57529, 57531);
INSERT INTO spell_proc_event (entry, SchoolMask, SpellFamilyName, SpellFamilyMaskA0, SpellFamilyMaskA1, SpellFamilyMaskA2, SpellFamilyMaskB0, SpellFamilyMaskB1, SpellFamilyMaskB2, SpellFamilyMaskC0, SpellFamilyMaskC1, SpellFamilyMaskC2, procFlags, procEx, ppmRate, CustomChance, Cooldown) VALUES
(57529, 0x00, 3, 0x61400035 | 0x200000 | 0x20000 | 0x0000040 | 0x000080 | 0x00000002 | 0x00000200 | 0x00001000 | 0x00800000  , 0,0   , 0x00001000 | 0x00008000 | 0x00000002 | 0x00001000 | 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 0, 0);
--               dbc        | arc. mis.| frost no| ice lance | blizzard | fire blast | cone of co | arc. expl. | dragon's br.,       , dbc        | arc. barr  | fire Blast | frotfireb. | blast wave

-- from FallenangelX
DELETE FROM `spell_proc_event` WHERE `entry` IN (51682); 
INSERT INTO `spell_proc_event` VALUES 
(51682, 0x00,  8, 0x10014000, 0x10014000, 0x10014000, 0x00080000, 0x00080000, 0x00080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000,  0);
