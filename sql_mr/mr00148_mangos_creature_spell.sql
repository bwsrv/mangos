ALTER TABLE `creature_spell`
    DROP PRIMARY KEY,
    ADD PRIMARY KEY (`guid`,`index`,`active`);
