-- Spell bonus data
-- Commit 7277073f958e81627b10

-- Lifebloom
UPDATE `spell_bonus_data` SET `direct_bonus`='0.8' WHERE `entry`='33763';

DELETE FROM `spell_bonus_data` WHERE entry IN (30451,51963,52042,52752,54158,56161);
INSERT INTO `spell_bonus_data`
    (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`)
VALUES
    -- Arcane Blast
    (30451, 0.714286, 0.122, 0, 0, 'Mage - Arcane Blast'),
    -- Gargoyle Strike
    (51963,0,0,0,0,'Gargoyle Strike'),
    -- Healing Stream Totem
    (52042,0,0,0,0,'Healing Stream Totem'),
    -- Ancestral Awakening
    (52752,0,0,0,0,'Ancestral Awakening'),
    -- Judgement
    (54158,0,0,0,0,'Judgement'),
    -- Glyph of Prayer of Healing
    (56161,0,0,0,0,'Glyph of Prayer of Healing');
