-- Additions for spell_script_target
/* NOTICE: Targets, which need often a spell_script_target entry are:
    Target_Script - 38
    Target_AreaEffect_Instant
    maybe some more, but i think not
*/

-- Molten Fury - Sartharion encounter - target Lava Blazes only
DELETE FROM spell_script_target WHERE spell_script_target.entry = 60430;
INSERT INTO spell_script_target VALUES
(60430, 1, 30643);

-- Berserk - Sartharion encounter - target dragon bosses only
DELETE FROM spell_script_target WHERE spell_script_target.entry = 61632;
INSERT INTO spell_script_target VALUES
(61632, 1, 28860),
(61632, 1, 30452),
(61632, 1, 30451),
(61632, 1, 30449);
