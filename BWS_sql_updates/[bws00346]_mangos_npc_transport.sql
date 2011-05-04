ALTER TABLE creature
ADD COLUMN trans_x float NOT NULL DEFAULT '0' AFTER orientation,
ADD COLUMN trans_y float NOT NULL DEFAULT '0' AFTER trans_x,
ADD COLUMN trans_z float NOT NULL DEFAULT '0' AFTER trans_y,
ADD COLUMN trans_o float NOT NULL DEFAULT '0' AFTER trans_z,
ADD COLUMN transguid bigint(20) unsigned NOT NULL AFTER trans_o;
