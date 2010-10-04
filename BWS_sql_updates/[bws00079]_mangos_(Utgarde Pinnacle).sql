-- Svala Sorrowgrave
UPDATE creature_template SET ScriptName='npc_ritual_channeler' WHERE entry=27281;
-- Gortok Palehoof
UPDATE gameobject_template SET ScriptName='go_gortok_generator' WHERE entry=188593;
UPDATE creature_template SET ScriptName='npc_gortok_orb' WHERE entry=26086; -- 26688
UPDATE creature_template SET ScriptName='npc_worgen' WHERE entry=26683;
UPDATE creature_template SET ScriptName='npc_furlborg' WHERE entry=26684;
UPDATE creature_template SET ScriptName='npc_jormungar' WHERE entry=26685;
UPDATE creature_template SET ScriptName='npc_rhino' WHERE entry=26686;
-- Skadi the Ruthless
UPDATE gameobject_template SET ScriptName='go_skadi_harpoon_launcher' WHERE entry IN (192175, 192176, 192177);
UPDATE gameobject_template SET ScriptName='go_skadi_harpoon' WHERE entry=192539;
UPDATE creature_template SET ScriptName='boss_grauf' WHERE entry=26893;
-- King Ymiron
UPDATE creature_template SET ScriptName='npc_bjorn_sphere' WHERE entry = 27339;
UPDATE creature_template SET ScriptName='npc_avenging_spirit_summoner' WHERE entry = 27392;

REPLACE INTO spell_script_target (entry, type, targetEntry) values
(48277, 1, 27327),
(48331, 1, 27327),
(48642, 1, 26893), 
(51368, 1, 26893),
(47670, 1, 26687),
(47670, 1, 26684),
(47670, 1, 26683),
(47670, 1, 26685),
(47670, 1, 26686),
(48385, 1, 27339);

-- vehicle data for Grauf
UPDATE creature_template SET VehicleId=40 WHERE entry IN (26893, 30775);
