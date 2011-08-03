-- Custom rate xp System
ALTER TABLE account
    ADD COLUMN premium int(255) NOT NULL default 0 AFTER locale;
