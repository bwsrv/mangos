-- Update Ebon Gargoyle (Death Knight guardian) stats scaling
UPDATE `pet_scaling_data` SET
`str` = 100,
`sta` = 100,
`armor` = 35,
`apbasescale` = 200,
`attackpower` = 200
WHERE `creature_entry` = 27829;
