-- instance extend support
ALTER TABLE `character_instance` ADD COLUMN `extend` 
TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Instance extend (bool)'
AFTER `permanent`;