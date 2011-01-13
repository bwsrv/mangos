-- Info http://www.wowwiki.com/Threat
-- Improved Revenge
DELETE FROM spell_threat WHERE entry IN (12797,12799);
INSERT INTO spell_threat (entry, Threat) VALUES
(12797,25), -- Rank 1
(12799,25); -- Rank 2

-- Sunder Armor
DELETE FROM spell_threat WHERE entry = 47467;
INSERT INTO spell_threat (entry, Threat) VALUES
(47467,345); -- Rank 7

-- Thunder Clap
DELETE FROM spell_threat WHERE entry IN (47501,47502);
INSERT INTO spell_threat (entry, Threat) VALUES
(47501,457), -- Rank 8
(47502,555); -- Rank 9

-- Heroic Throw
DELETE FROM spell_threat WHERE entry = 57755;
INSERT INTO spell_threat (entry, Threat) VALUES
(57755,770); -- Rank 1