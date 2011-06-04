-- tempfix for GO rotations
UPDATE gameobject SET rotation2 = '0',rotation3 = '1' WHERE id IN (176080,176081,176082,176083,176084,176085);