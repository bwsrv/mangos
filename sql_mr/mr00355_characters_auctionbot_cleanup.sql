-- Auction bot cleanup
LOCK TABLES `auction` WRITE ;
DELETE FROM `auction` WHERE `itemowner` = 0;
UNLOCK TABLES;
