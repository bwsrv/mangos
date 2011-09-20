-- npc on transport use transMap column (hack)
ALTER TABLE creature
    ADD COLUMN transMap mediumint(10) NOT NULL DEFAULT 0 AFTER orientation;
