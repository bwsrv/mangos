REPLACE INTO `command` VALUES
   ('nameannounce',1,'Syntax: .nameannounce $MessageToBroadcast\r\n\r\nSend a global message to all players online in chat log with colored sender\'s name.');
REPLACE INTO `mangos_string` VALUES
   (10000,'|c1f40af20 <Mod>|cffff0000[%s]|c1f40af20 announce:|cffffff00 %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
   (10001,'|c1f40af20 <GM>|cffff0000[%s]|c1f40af20 announce:|cffffff00 %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL), 
   (10002,'|c1f40af20 <Admin>|cffff0000[%s]|c1f40af20 announce:|cffffff00 %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);