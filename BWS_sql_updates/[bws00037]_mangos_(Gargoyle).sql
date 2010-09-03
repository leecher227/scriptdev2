-- DK Gargoyle
UPDATE `creature_template` SET `ScriptName` = 'npc_death_knight_gargoyle' WHERE `entry` = '27829';
-- Gargoyle Strike AP bonus
DELETE FROM spell_bonus_data WHERE entry IN (51963);
INSERT INTO spell_bonus_data VALUES
(51963, 0, 0, 0.42, 'Gargoyle Strike');