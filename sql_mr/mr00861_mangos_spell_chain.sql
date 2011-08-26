-- Culling the Herd
DELETE FROM spell_chain WHERE spell_id IN (61680, 61681, 52858);
INSERT INTO spell_chain VALUES
(61680, 0, 61680, 1, 0),
(61681, 61680, 61680, 2, 0),
(52858, 61681, 61680, 3, 0);