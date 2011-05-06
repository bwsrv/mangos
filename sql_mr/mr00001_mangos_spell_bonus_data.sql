-- Commit 7277073f958e81627b10
UPDATE spell_bonus_data SET direct_bonus = 0.8 WHERE entry = 33763;
-- Ancestral Awakening
DELETE FROM spell_bonus_data WHERE entry = 52752;
INSERT INTO spell_bonus_data (entry) VALUES (52752);
-- Glyph of Prayer of Healing
DELETE FROM spell_bonus_data WHERE entry = 56161;
INSERT INTO spell_bonus_data (entry) VALUES (56161);
-- Arcane Blast
DELETE FROM spell_bonus_data WHERE entry = 30451;
INSERT INTO spell_bonus_data VALUES ('30451','0.714286','0.122','0','Mage - Arcane Blast');
-- Judgement
DELETE FROM spell_bonus_data WHERE entry = 54158;
-- Gargoyle Strike
DELETE FROM spell_bonus_data WHERE entry = 51963;
-- Healing Stream Totem
DELETE FROM spell_bonus_data WHERE entry = 52042;
