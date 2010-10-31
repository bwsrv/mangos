-- Ancestral Awakening
DELETE FROM spell_bonus_data WHERE entry = 52752;
INSERT INTO spell_bonus_data (entry) VALUES (52752);

-- Glyph of Prayer of Healing
DELETE FROM spell_bonus_data WHERE entry = 56161;
INSERT INTO spell_bonus_data (entry) VALUES (56161);

-- Death Coil
DELETE FROM spell_bonus_data WHERE entry = 47632;
INSERT INTO spell_bonus_data VALUES (47632, 0, 0, 0.1, 'Death Knight - Death Coil Damage');