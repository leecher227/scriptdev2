-- Mirror Immage
UPDATE creature_template SET speed_walk = 2.5, modelid_1 = 11686, modelid_3 = 11686, minlevel = 80, maxlevel = 80, AIName = '', ScriptName='npc_mirror_image' WHERE entry = 31216;
DELETE FROM creature_ai_scripts WHERE creature_id = 31216;

DELETE FROM spell_bonus_data WHERE entry IN (59637,59638);
INSERT INTO spell_bonus_data VALUES
(59637, 0.10, 0, 0, 'Mirror Image - Fire Blast'),
(59638, 0.05, 0, 0, 'Mirror Image - Frostbolt');

