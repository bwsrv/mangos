ALTER TABLE creature_template
<<<<<<< HEAD
  ADD COLUMN spell5 mediumint(8) unsigned NOT NULL default '0' AFTER spell4,
  ADD COLUMN spell6 mediumint(8) unsigned NOT NULL default '0' AFTER spell5,
  ADD COLUMN spell7 mediumint(8) unsigned NOT NULL default '0' AFTER spell6,
  ADD COLUMN spell8 mediumint(8) unsigned NOT NULL default '0' AFTER spell7,
  ADD COLUMN VehicleId mediumint(8) unsigned NOT NULL default '0' AFTER PetSpellDataId,
  ADD COLUMN PowerType tinyint(3) unsigned NOT NULL default '0' AFTER MaxHealth;
=======
  ADD COLUMN `spell5` mediumint(8) unsigned NOT NULL default '0' AFTER `spell4`,
  ADD COLUMN `spell6` mediumint(8) unsigned NOT NULL default '0' AFTER `spell5`,
  ADD COLUMN `spell7` mediumint(8) unsigned NOT NULL default '0' AFTER `spell6`,
  ADD COLUMN `spell8` mediumint(8) unsigned NOT NULL default '0' AFTER `spell7`,
  ADD COLUMN `PowerType` tinyint(3) unsigned NOT NULL default '0' AFTER `MaxHealth`;
>>>>>>> b8aa3f20a1a6ff924ce90b586dbfec74ae444b41

