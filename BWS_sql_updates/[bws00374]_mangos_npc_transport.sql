-- ALTER TABLE creature
-- DROP trans_x;
-- DROP trans_y;
-- DROP trans_z;
-- DROP trans_o;
-- DROP trans_guid;

-- npc on transport use transActive column (hack)
ALTER TABLE creature
    ADD COLUMN transActive mediumint(10) NOT NULL DEFAULT 0 AFTER orientation;