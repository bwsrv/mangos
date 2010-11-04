-- Ancestral Awakening
DELETE FROM spell_bonus_data WHERE entry = 52752;
INSERT INTO spell_bonus_data (entry) VALUES (52752);

-- Glyph of Prayer of Healing
DELETE FROM spell_bonus_data WHERE entry = 56161;
INSERT INTO spell_bonus_data (entry) VALUES (56161);

-- Arcane Blast
DELETE FROM spell_bonus_data WHERE entry = 30451;
INSERT INTO spell_bonus_data VALUES ('30451','0.714286','0.122','0','Mage - Arcane Blast');