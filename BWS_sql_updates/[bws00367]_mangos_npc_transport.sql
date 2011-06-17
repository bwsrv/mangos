-- npc on transport use transActive column (hack)
ALTER TABLE creature
    ADD COLUMN transActive int(255) NOT NULL DEFAULT 0 AFTER orientation;