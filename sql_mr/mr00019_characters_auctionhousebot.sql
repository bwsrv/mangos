-- Auctionhouse Bot
-- Commit 7c97114488b36b4427ed

INSERT INTO `auctionhousebot`
    (`auctionhouse`,`name`,`minitems`,`maxitems`,`mintime`,`maxtime`,`percentgreytradegoods`,`percentwhitetradegoods`,`percentgreentradegoods`,`percentbluetradegoods`,`percentpurpletradegoods`,`percentorangetradegoods`,`percentyellowtradegoods`,`percentgreyitems`,`percentwhiteitems`,`percentgreenitems`,`percentblueitems`,`percentpurpleitems`,`percentorangeitems`,`percentyellowitems`,`minpricegrey`,`maxpricegrey`,`minpricewhite`,`maxpricewhite`,`minpricegreen`,`maxpricegreen`,`minpriceblue`,`maxpriceblue`,`minpricepurple`,`maxpricepurple`,`minpriceorange`,`maxpriceorange`,`minpriceyellow`,`maxpriceyellow`,`minbidpricegrey`,`maxbidpricegrey`,`minbidpricewhite`,`maxbidpricewhite`,`minbidpricegreen`,`maxbidpricegreen`,`minbidpriceblue`,`maxbidpriceblue`,`minbidpricepurple`,`maxbidpricepurple`,`minbidpriceorange`,`maxbidpriceorange`,`minbidpriceyellow`,`maxbidpriceyellow`,`maxstackgrey`,`maxstackwhite`,`maxstackgreen`,`maxstackblue`,`maxstackpurple`,`maxstackorange`,`maxstackyellow`,`buyerpricegrey`,`buyerpricewhite`,`buyerpricegreen`,`buyerpriceblue`,`buyerpricepurple`,`buyerpriceorange`,`buyerpriceyellow`,`buyerbiddinginterval`,`buyerbidsperinterval`)
VALUES
    (2,'Alliance',0,0,8,24,0,27,12,10,1,0,0,0,10,30,8,2,0,0,100,150,150,250,800,1400,1250,1750,2250,4550,3250,5550,5250,6550,70,100,70,100,80,100,75,100,80,100,80,100,80,100,0,0,3,2,1,1,1,1,1,5,12,15,20,22,1,1),
    (6,'Horde',0,0,8,24,0,27,12,10,1,0,0,0,10,30,8,2,0,0,100,150,150,250,800,1400,1250,1750,2250,4550,3250,5550,5250,6550,70,100,70,100,80,100,75,100,80,100,80,100,80,100,0,0,3,2,1,1,1,1,1,5,12,15,20,22,1,1),
    (7,'Neutral',0,0,8,24,0,27,12,10,1,0,0,0,10,30,8,2,0,0,100,150,150,250,800,1400,1250,1750,2250,4550,3250,5550,5250,6550,70,100,70,100,80,100,75,100,80,100,80,100,80,100,0,0,3,2,1,1,1,1,1,5,12,15,20,22,1,1);

-- Commit 5295ec0f7d0c1b5b9a6a

DELETE FROM characters.item_instance WHERE 
SUBSTRING_INDEX(SUBSTRING_INDEX(characters.item_instance.data,' ',4),' ',-1) IN 
(SELECT mangos.item_template.entry
FROM mangos.item_template 
WHERE mangos.item_template.name LIKE "% test%");

DELETE FROM characters.auction USING characters.auction LEFT JOIN characters.item_instance ON characters.auction.itemguid = characters.item_instance.guid WHERE characters.item_instance.guid IS NULL;
DELETE FROM characters.auction USING characters.auction LEFT JOIN mangos.item_template ON auction.item_template = mangos.item_template.entry WHERE mangos.item_template.entry IS NULL;

LOCK TABLES item_instance WRITE ;
START TRANSACTION;
DELETE
FROM item_instance 
WHERE guid NOT IN (SELECT item FROM character_inventory) 
AND guid NOT IN (SELECT itemguid FROM auction) 
AND guid NOT IN (SELECT item_guid FROM guild_bank_item) 
AND guid NOT IN (SELECT item_guid FROM mail_items) 
AND guid NOT IN (SELECT item_guid FROM character_gifts)
;
COMMIT;

UNLOCK TABLES;
