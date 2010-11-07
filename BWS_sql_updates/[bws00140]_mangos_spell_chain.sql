-- Lock and Load
DELETE FROM spell_chain WHERE spell_id IN (56342, 56343, 56344);
INSERT INTO spell_chain VALUES
(56342,     0, 56342, 1, 0),
(56343, 56342, 56342, 2, 0),
(56344, 56343, 56342, 3, 0);

-- Safeguard
DELETE FROM spell_chain WHERE spell_id IN (46945, 46949);
INSERT INTO spell_chain VALUES
(46945,     0, 46945, 1, 0),
(46949, 46949, 46945, 2, 0);