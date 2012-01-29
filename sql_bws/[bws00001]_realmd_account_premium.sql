ALTER TABLE account
    ADD COLUMN premium int(11) NOT NULL default 0 AFTER locale;
