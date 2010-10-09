-- Death Knight
-- (49004) Scent of Blood 
DELETE FROM spell_proc_event WHERE entry = 49004;
INSERT INTO spell_proc_event VALUES (49004, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000, 0x00000000, 0x00000000, 0x00000008, 0x00000031, 0, 15, 0);

-- (51123) Killing Machine
DELETE FROM spell_proc_event WHERE entry = 51123;
INSERT INTO spell_proc_event VALUES (51123, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00000000, 5, 100, 0);

-- (49200) Acclimation
DELETE FROM spell_proc_event WHERE entry = 49200;

-- (48988) Bloody Vengeance
DELETE FROM spell_proc_event WHERE entry = 48988;
INSERT INTO spell_proc_event VALUES (48988, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00011154, 0x00000002, 0, 100, 0);

-- (49149) Chill of the Grave
DELETE FROM spell_proc_event WHERE entry = 49149;
INSERT INTO spell_proc_event VALUES (49149, 0x00, 0x0F, 0x00000006, 0x00000006, 0x00000006, 0x00020002, 0x00020002, 0x00020002, 0x00000000, 0x00000000, 0x00000000, 0x00015410, 0x00000000, 0, 100, 0);

-- (49223) Dirge
DELETE FROM spell_proc_event WHERE entry = 49223;
INSERT INTO spell_proc_event VALUES (49223, 0x00, 0x0F, 0x00000011, 0x00000011, 0x00000011, 0x08000000, 0x08000000, 0x08000000, 0x00000000, 0x00000000, 0x00000000, 0x00015410, 0x00000000, 0, 100, 0);

-- (55666) Desecration
DELETE FROM spell_proc_event WHERE entry = 55666;
INSERT INTO spell_proc_event VALUES (55666, 0x00, 0x0F, 0x00000001,0x00000001, 0x00000001, 0x08000000, 0x08000000, 0x08000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (49194) Unholy Blight
DELETE FROM spell_proc_event WHERE entry = 49194;
INSERT INTO spell_proc_event VALUES (49194, 0x00, 0x0F, 0x00002000, 0x00002000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 100, 0);

-- (49032) Crypt Fever
DELETE FROM spell_proc_event WHERE entry = 49032;
INSERT INTO spell_proc_event VALUES (49032, 0x0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 0, 0);

-- (51099) Ebon Plaguebringer
DELETE FROM spell_proc_event WHERE entry = 51099;
INSERT INTO spell_proc_event VALUES (51099, 0x0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 0, 0);

-- (49222) Bone Shield
DELETE FROM spell_proc_event WHERE entry = 49222;
INSERT INTO spell_proc_event VALUES (49222, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0002A2A8, 0x00000000, 0, 100, 4);

-- (70652) Item - Death Knight T10 Tank 4P Bonus
DELETE FROM spell_proc_event WHERE entry = 70652;
INSERT INTO spell_proc_event VALUES (70652, 0x00, 0x0F, 0x00000008, 0x00000008, 0x00000008, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0, 100, 0);

-- (67384) Item - Death Knight T9 DPS Relic (Obliterate, Scourge Strike, Death Strike)
DELETE FROM spell_proc_event WHERE entry = 67384;
INSERT INTO spell_proc_event VALUES (67384, 0x00, 0x0F, 0x00000010, 0x00000010, 0x00000010, 0x08020000, 0x08020000, 0x08020000, 0x00000080, 0x00000080, 0x00000080, 0x00000010, 0x00000000, 0, 80, 0);

-- (67381) Item - Death Knight T9 Tank Relic (Rune Strike)
DELETE FROM spell_proc_event WHERE entry = 67381;
INSERT INTO spell_proc_event VALUES (67381, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x20000000, 0x20000000, 0x20000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 80, 0);

-- (64964) Item - Death Knight T8 Tank Relic (Rune Strike)
DELETE FROM spell_proc_event WHERE entry = 64964;
INSERT INTO spell_proc_event VALUES (64964, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x20000000, 0x20000000, 0x20000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (62147) Icy Touch Defense Increase
DELETE FROM spell_proc_event WHERE entry = 62147;
INSERT INTO spell_proc_event VALUES (62147, 0x00, 0x0F, 0x00000002, 0x00000002, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010100, 0x00000000, 0, 100, 0);

-- (50880) Icy Talons
DELETE FROM spell_proc_event WHERE entry = 50880;
INSERT INTO spell_proc_event VALUES (50880, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x04000000, 0x04000000, 0x04000000, 0x00000000, 0x00000000, 0x00000000, 0x00070000, 0x00000000, 0, 100, 0);

-- Death Rune Mastery
DELETE FROM spell_proc_event WHERE entry IN (54639,49467,49208);
INSERT INTO spell_proc_event VALUES
(49467,0,15,16,0,0,131072,0,0,0,0,0,0,0,0,0,0),
(54639,0,15,4194304,0,0,65536,0,0,0,0,0,0,0,0,0,0),
(49208,0,15,4194304,0,0,65536,0,0,0,0,0,0,0,0,0,0);

-- (71228) Item - Death Knight T10 Tank Relic (Runestrike)
DELETE FROM spell_proc_event WHERE entry = 71228;
INSERT INTO spell_proc_event VALUES (71228, 0x00, 0x0F, 0x00000000, 0x00000000, 0x00000000, 0x20000000, 0x20000000, 0x20000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (71226) Item - Death Knight T10 DPS Relic (Obliterate, Scourge Strike, Death Strike)
DELETE FROM spell_proc_event WHERE entry = 71226;
INSERT INTO spell_proc_event VALUES (71226, 0x00, 0x0F, 0x00000010, 0x00000010, 0x00000010, 0x08020000, 0x08020000, 0x08020000, 0x00000080, 0x00000080, 0x00000080, 0x00000010, 0x00000000, 0, 100, 0);

-- Druid
-- (48492) King of the Jungle 
DELETE FROM spell_proc_event WHERE entry IN (48492,48494,48495);
INSERT INTO spell_proc_event VALUES 
(48492,0,7,524288,524288,524288,0,0,0,2048,2048,2048,16384,0,0,0,0),
(48494,0,7,524288,524288,524288,0,0,0,2048,2048,2048,16384,0,0,0,0),
(48495,0,7,524288,524288,524288,0,0,0,2048,2048,2048,16384,0,0,0,0);

-- (22812) Barkskin
DELETE FROM spell_proc_event WHERE entry = 22812;
INSERT INTO spell_proc_event VALUES (22812, 0x00, 0x07, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000003, 0, 100, 0);

-- (60726) LK Arena 6 Gladiator's Idol of Steadfastness
DELETE FROM spell_proc_event WHERE entry = 60726;
INSERT INTO spell_proc_event VALUES (60726, 0x00, 0x07, 0x00000002, 0x00000002, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0);

-- (60701) LK Arena 6 Gladiator's Idol of Resolve
DELETE FROM spell_proc_event WHERE entry = 60701;
INSERT INTO spell_proc_event VALUES (60701, 0x00, 0x07, 0x00000000, 0x00000000, 0x00000000, 0x00000440, 0x00000440, 0x00000440, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00000000, 0, 100, 0);

-- (71176) Item - Druid T10 Balance Relic (Moonfire and Insect Swarm)
DELETE FROM spell_proc_event WHERE entry = 71176;
INSERT INTO spell_proc_event VALUES (71176, 0x00, 0x07, 0x00200002, 0x00200002, 0x00200002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 100, 0);

-- (71174) Item - Druid T10 Feral Relic (Rake and Lacerate)
DELETE FROM spell_proc_event WHERE entry = 71174;
INSERT INTO spell_proc_event VALUES (71174, 0x00, 0x07, 0x00801000, 0x00801000, 0x00801000, 0x00000000, 0x00000000,  0x00000000, 0x00200000, 0x00200000, 0x00200000, 0x00040000, 0x00000000, 0, 100, 0);

-- (71178) Item - Druid T10 Restoration Relic (Rejuvenation)
DELETE FROM spell_proc_event WHERE entry = 71178;
INSERT INTO spell_proc_event VALUES (71178, 0x00, 0x07, 0x00000010, 0x00000010, 0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 100, 0);

-- (60724) LK Arena 5 Gladiator's Idol of Steadfastness
DELETE FROM spell_proc_event WHERE entry = 60724;
INSERT INTO spell_proc_event VALUES (60724, 0x00, 0x07, 0x00000002, 0x00000002, 0x00000002,  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0);

-- (67353) Item - Druid T9 Feral Relic (Lacerate, Swipe, Mangle, and Shred)
DELETE FROM spell_proc_event WHERE entry = 67353;
INSERT INTO spell_proc_event VALUES (67353, 0x00, 0x07, 0x00008000, 0x00008000, 0x00008000, 0x00100500, 0x00100500, 0x00100500, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 70, 0);

-- (67361) Item - Druid T9 Balance Relic (Moonfire)
DELETE FROM spell_proc_event WHERE entry = 67361;
INSERT INTO spell_proc_event VALUES (67361, 0x00, 0x07, 0x00000002, 0x00000002, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 70, 6);

-- (67356) Item - Druid T9 Restoration Relic (Rejuvenation)
DELETE FROM spell_proc_event WHERE entry = 67356;
INSERT INTO spell_proc_event VALUES (67356, 0x00, 0x07, 0x00000010, 0x00000010, 0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00044000, 0x00000000, 0, 80, 0);

-- Warrior
-- (12317) Enrage
DELETE FROM spell_proc_event WHERE entry = 12317;

-- (58872) Damage Shield
DELETE FROM spell_proc_event WHERE entry = 58872;
INSERT INTO spell_proc_event VALUES (58872, 0x00, 0x04, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000028, 0x00002043, 0, 100, 0);

-- (58388) Glyph of Devastate
DELETE FROM spell_proc_event WHERE entry = 58388;
INSERT INTO spell_proc_event VALUES (58388, 0x00, 0x04, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00000040, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 16, 0x00000000, 0, 100, 0);

-- (58375) Glyph of Blocking
DELETE FROM spell_proc_event WHERE entry = 58375;
INSERT INTO spell_proc_event VALUES (58375, 0x00, 0x04, 0x00000000, 0x00000000, 0x00000000, 0x00000200, 0x00000200, 0x00000200, 0x00000000, 0x00000000, 0x00000000, 16, 0x00000000, 0, 100, 0);

-- Shaman
-- (70830) Item - Shaman T10 Enhancement 2P Bonus 
DELETE FROM spell_proc_event WHERE entry = 70830;
INSERT INTO spell_proc_event VALUES (70830, 0x00, 0x0B, 0x00000000, 0x00000000, 0x00000000, 0x00020000, 0x00020000, 0x00020000, 0x00000000, 0x00000000, 0x00000000, 16384, 0x00000000, 0, 100, 0);

-- (71198) Item - Shaman T10 Elemental Relic (Shocks)
DELETE FROM spell_proc_event WHERE entry = 71198;
INSERT INTO spell_proc_event VALUES (71198, 0x00, 0x0B, 0x10000000, 0x10000000, 0x10000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 100, 0);

-- (71217) Item - Shaman T10 Restoration Relic (Riptide)
DELETE FROM spell_proc_event WHERE entry = 71217;
INSERT INTO spell_proc_event VALUES (71217, 0x00, 0x0B, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000010, 0x00000010, 0x00004000, 0x00000000, 0, 100, 0);

-- (71214) Item - Shaman T10 Enhancement Relic (Stormstrike)
DELETE FROM spell_proc_event WHERE entry = 71214;
INSERT INTO spell_proc_event VALUES (71214, 0x00, 0x0B, 0x00000000, 0x00000000, 0x00000000, 0x01000000, 0x01000000, 0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (70808) Item - Shaman T10 Restoration 4P Bonus
DELETE FROM spell_proc_event WHERE entry = 70808;
INSERT INTO spell_proc_event VALUES (70808, 0x00, 0x0B, 0x00000100, 0x00000100, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0, 100, 0);

-- (67389) Item - Shaman T9 Restoration Relic (Chain Heal)
DELETE FROM spell_proc_event WHERE entry = 67389;
INSERT INTO spell_proc_event VALUES (67389, 0x00, 0x0B, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0, 70, 0);

-- (67392) Item - Shaman T9 Enhancement Relic (Lava Lash)
DELETE FROM spell_proc_event WHERE entry = 67392;
INSERT INTO spell_proc_event VALUES (67392, 0x00, 0x0B, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00000004, 0x00000004, 0x00011110, 0x00000000, 0, 80, 0);

-- (67389) Item - Shaman T9 Restoration Relic (Chain Heal)
DELETE FROM spell_proc_event WHERE entry = 67389;
INSERT INTO spell_proc_event VALUES (67389, 0x00, 0x0B, 0x00000100, 0x00000100, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0, 70, 0);

-- (67386) Item - Shaman T9 Elemental Relic (Lightning Bolt)
DELETE FROM spell_proc_event WHERE entry = 67386;
INSERT INTO spell_proc_event VALUES (67386, 0x00, 0x0B, 0x00000001, 0x00000001, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 70, 0);

-- (60765) Nax 25 Melee Shaman Totem
DELETE FROM spell_proc_event WHERE entry = 60765;
INSERT INTO spell_proc_event VALUES (60765, 0x00, 0x0B, 0x00000000, 0x00000000, 0x00000000, 0x01000000, 0x01000000, 0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (60770) Totem of the Elemental Plane
DELETE FROM spell_proc_event WHERE entry = 60770;
INSERT INTO spell_proc_event VALUES (60770, 0x00, 0x0B, 0x00000001, 0x00000001, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 15, 0);

-- (43750) Energized
DELETE FROM spell_proc_event WHERE entry = 43750;
INSERT INTO spell_proc_event VALUES (43750, 0x00, 0x0B, 0x00000001, 0x00000001, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 15, 0);

-- Paladin
-- (70756) Item - Paladin T10 Holy 4P Bonus
DELETE FROM spell_proc_event WHERE entry = 70756;
INSERT INTO spell_proc_event VALUES (70756, 0x00, 0x0A, 0x00200000, 0x00200000, 0x00200000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 16384, 0x00000000, 0, 100, 0);

-- (71194) Item - Paladin T10 Protection Relic (Shield of Righteousness)
DELETE FROM spell_proc_event WHERE entry = 71194;
INSERT INTO spell_proc_event VALUES (71194, 0x00, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00100000, 0x00100000, 0x00100000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (71191) Item - Paladin T10 Holy Relic (Holy Shock)
DELETE FROM spell_proc_event WHERE entry = 71191;
INSERT INTO spell_proc_event VALUES (71191, 0x01, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00010000, 0x00010000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0, 100, 0);

-- (71186) Item - Paladin T10 Retribution Relic (Crusader Strike)
DELETE FROM spell_proc_event WHERE entry = 71186;
INSERT INTO spell_proc_event VALUES (71186, 0x00, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00008000, 0x00008000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (67365) Item - Paladin T9 Retribution Relic
DELETE FROM spell_proc_event WHERE entry = 67365;
INSERT INTO spell_proc_event VALUES (67365, 0x00, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00400800, 0x00400800, 0x00400800, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 70, 0);

-- (67379) Item - Paladin T9 Protection Relic (Hammer of The Righteous)
DELETE FROM spell_proc_event WHERE entry = 67379;
INSERT INTO spell_proc_event VALUES (67379, 0x00, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x00040000, 0x00040000,  0x00000000, 0x00000000, 0x00000000, 0x00011110, 0x00000000, 0, 80, 0);

-- (67363) Item - Paladin T9 Holy Relic (Judgement)
DELETE FROM spell_proc_event WHERE entry = 67363;
INSERT INTO spell_proc_event VALUES (67363, 0x00, 0x0A, 0x80000000, 0x80000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0, 70, 0);

-- (60635) LK Arena 5 Gladiator's Libram of Fortitude
DELETE FROM spell_proc_event WHERE entry = 60635;
INSERT INTO spell_proc_event VALUES (60635, 0x00, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00008000, 0x00008000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- (60636) LK Arena 6 Gladiator's Libram of Fortitude
DELETE FROM spell_proc_event WHERE entry = 60636;
INSERT INTO spell_proc_event VALUES (60636, 0x00, 0x0A, 0x00000000, 0x00000000, 0x00000000, 0x00008000, 0x00008000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000000, 0, 100, 0);

-- Mage
-- (12654) Ignite
DELETE FROM spell_proc_event WHERE entry = 12654;
INSERT INTO spell_proc_event VALUES (12654, 0x00,  0x03, 0x08000000, 0x08000000, 0x08000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0, 100, 0);

-- (44543) Fingers of Frost
DELETE FROM spell_proc_event WHERE entry = 44543;
INSERT INTO spell_proc_event VALUES (44543, 0x00, 0x03, 0x00100000,0x00100000, 0x00100000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 7, 0);

-- (31571) Arcane Potency
DELETE FROM spell_proc_event WHERE entry = 31571;
INSERT INTO spell_proc_event VALUES (31571, 0x00, 0x03, 0x00000000, 0x00000000, 0x00000000, 0x00000022, 0x00000022, 0x00000022, 0x00000000, 0x00000000, 0x00000000, 16384, 0x00000000, 0, 100, 0);

-- (36032) Arcane Blast
DELETE FROM spell_proc_event WHERE entry = 36032;
INSERT INTO spell_proc_event VALUES (36032, 0x00, 0x03, 0x20000000, 0x20000000, 0x20000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0);

-- (44404) Missile Barrage
DELETE FROM spell_proc_event WHERE entry IN (44404, 54486, 54488, 54489, 54490);
INSERT INTO spell_proc_event VALUES 
(44404, 0x00, 0x03, 0x20000021, 0x20000021, 0x20000021,  0x00009000, 0x00009000, 0x00009000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 8, 0),
(54486, 0x00, 0x03, 0x20000021, 0x20000021, 0x20000021,  0x00009000, 0x00009000, 0x00009000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 16, 0),
(54488, 0x00, 0x03, 0x20000021, 0x20000021, 0x20000021,  0x00009000, 0x00009000, 0x00009000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 24, 0),
(54489, 0x00, 0x03, 0x20000021, 0x20000021, 0x20000021,  0x00009000, 0x00009000, 0x00009000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 32, 0),
(54490, 0x00, 0x03, 0x20000021, 0x20000021, 0x20000021,  0x00009000, 0x00009000, 0x00009000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 40, 0);

-- (64867) Item - Mage T8 2P Bonus
DELETE FROM spell_proc_event WHERE entry = 64867;
INSERT INTO spell_proc_event VALUES (64867, 0x00, 0x03, 0x20000021, 0x20000021, 0x20000021, 0x00001000, 0x00001000, 0x00001000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 25, 0);

-- Warlock
-- (47230) Fel Synergy
DELETE FROM spell_proc_event WHERE entry = 47230;
INSERT INTO spell_proc_event VALUES (47230, 0x00, 0x05, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 50, 0);

-- Priest
-- (59907) Lightwell Charges
DELETE FROM spell_proc_event WHERE entry = 59907;
INSERT INTO spell_proc_event VALUES (59907, 0x00, 0x06, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00004000, 0x00004000, 0x00000000, 0x00000000, 0, 100, 0);

-- Rogue
-- (51698) Honored Among Thieves
DELETE FROM spell_proc_event WHERE entry = 51698;
INSERT INTO spell_proc_event VALUES (51698, 0x00, 0x08, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 16|256|4096|16384|65536|262144, 0x00000002, 0, 100, 1);

-- Hunter
-- (64860) Item - Hunter T8 4P Bonus
DELETE FROM spell_proc_event WHERE entry = 64860;
INSERT INTO spell_proc_event VALUES (64860, 0x00, 0x09, 0x00000000,  0x00000000,  0x00000000 ,0x00000001, 0x00000001,  0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000100, 0x00000000, 0, 10, 0); 

-- (67151) Item - Hunter T9 4P Bonus (Steady Shot)
DELETE FROM spell_proc_event WHERE entry = 67151;
INSERT INTO spell_proc_event VALUES (67151, 0x00, 0x09, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000140, 0x00000000, 0, 35, 0);

-- (70727) Item - Hunter T10 2P Bonus
DELETE FROM spell_proc_event WHERE entry = 70727;
INSERT INTO spell_proc_event VALUES (70727, 0x00, 0x09, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00000000, 0, 5, 0);

-- (70730) Item - Hunter T10 4P Bonus
DELETE FROM spell_proc_event WHERE entry = 70730;
INSERT INTO spell_proc_event VALUES (70730, 0x00, 0x09, 0x00004000, 0x00004000, 0x00004000, 0x00000100, 0x00000100, 0x00000100,  0x00000000,  0x00000000, 0x00000000, 0x00040000, 0x00000000, 0, 5, 0);

-- (56342) Lock and Load
DELETE FROM spell_proc_event WHERE entry IN (56342, 56343, 56344);
INSERT INTO spell_proc_event VALUES (56342, 0x00, 0x09, 0x0C, 0x0C, 0x0C, 0x8000000, 0x8000000, 0x8000000, 0x60000, 0x60000, 0x60000, 0x00000000, 0x00000000, 0, 100, 22);

-- Other Items
-- (65005) Alacrity of the Elements
DELETE FROM spell_proc_event WHERE entry = 65005;
INSERT INTO spell_proc_event VALUES (65005, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0, 10, 45);

-- (33953) Essence of Life
DELETE FROM spell_proc_event WHERE entry = 33953;
INSERT INTO spell_proc_event VALUES (33953, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00044000, 0x00000000, 0, 10, 45);

-- (60063) Now is the Time!
DELETE FROM spell_proc_event WHERE entry = 60063;
INSERT INTO spell_proc_event VALUES (60063, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00011000, 0x00000000, 0, 10, 45);

-- (55640) Lightweave Embroidery
DELETE FROM spell_proc_event WHERE entry = 55640;
INSERT INTO spell_proc_event VALUES (55640, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00054000, 0x00000000, 0, 35, 45);

-- (60493) Dying Curse
DELETE FROM spell_proc_event WHERE entry = 60493;
INSERT INTO spell_proc_event VALUES (60493, 0x00, 0x03, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00054000, 0x00000000, 0, 15, 45);

-- (57345) Darkmoon Card: Greatness
DELETE FROM spell_proc_event WHERE entry = 57345;
INSERT INTO spell_proc_event VALUES (57345, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00055554, 0x00000000, 0, 35, 45);

-- (62114) Flow of Knowledge
DELETE FROM spell_proc_event WHERE entry = 62114;
INSERT INTO spell_proc_event VALUES (62114, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00054000, 0x00000000, 0, 10, 45);
