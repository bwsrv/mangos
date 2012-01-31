ALTER TABLE account
    ADD COLUMN rp_allow tinyint(3) NOT NULL default 0 AFTER premium;
