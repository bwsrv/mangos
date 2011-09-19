-- Spell bonus data
-- Lifebloom
UPDATE `spell_bonus_data` SET `direct_bonus`= 0.8 WHERE `entry`= 33763;

DELETE FROM `spell_bonus_data` WHERE entry IN (30451, 51963, 52042, 52752, 54158, 56161, 58621, 64844, 64085);
INSERT INTO `spell_bonus_data`
    (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`)
VALUES
    -- Gargoyle Strike
    (51963, 0, 0, 0, 0,'Gargoyle Strike'),
    -- Healing Stream Totem
    (52042, 0, 0, 0, 0,'Healing Stream Totem'),
    -- Ancestral Awakening
    (52752, 0, 0, 0, 0,'Ancestral Awakening'),
    -- Judgement
    (54158, 0, 0, 0, 0,'Judgement'),
    -- Glyph of  Prayer of Healing
    (56161, 0, 0, 0, 0,'Glyph of Prayer of Healing'),
    -- Glyph of Chains of Ice (it's correct, but bonus value is simple)
    (58621, 0, 0, 0.08, 0, 'Glyph of Chains of Ice'),
    -- Priest - Divine Hymn
    (64844, 0.564, 0, 0, 0, 'Priest - Divine Hymn'),
	-- Priest - Vampiric Touch Dispel
    (64085, 2.4, 0, 0, 0, 'Priest - Vampiric Touch Dispel');
	
