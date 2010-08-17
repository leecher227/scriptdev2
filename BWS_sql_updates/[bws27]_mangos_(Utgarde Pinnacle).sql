UPDATE creature_template SET ScriptName='npc_ritual_channeler' WHERE entry=27281;
UPDATE gameobject_template SET ScriptName = 'go_skadi_harpoon_launcher' WHERE entry IN (192175, 192176, 192177);
UPDATE gameobject_template SET ScriptName = 'go_skadi_harpoon' WHERE entry=192539;
UPDATE creature_template SET ScriptName = 'npc_bjorn_sphere' WHERE entry = 27339;
UPDATE creature_template SET ScriptName='boss_grauf' WHERE entry=26893;
UPDATE creature_template SET ScriptName='npc_skadi_breath_trigger' WHERE entry=28351;
UPDATE creature_template SET ScriptName='npc_skadi_summon' WHERE entry in(26692, 26690, 26691);
UPDATE creature_template SET ScriptName='npc_gortok_orb' WHERE entry=26086; -- 26688
UPDATE gameobject_template SET ScriptName='go_gortok_generator' WHERE entry=188593;
UPDATE creature_template SET ScriptName='npc_worgen' WHERE entry=26683;
UPDATE creature_template SET ScriptName='npc_furlborg' WHERE entry=26684;
UPDATE creature_template SET ScriptName='npc_jormungar' WHERE entry=26685;
UPDATE creature_template SET ScriptName='npc_rhino' WHERE entry=26686;

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
UPDATE creature_template_addon SET vehicle_id=40 WHERE entry=26893;
DELETE FROM vehicle_seat_data WHERE seat=682;
INSERT INTO vehicle_seat_data (seat, flags) VALUES
(682, 1);
DELETE FROM vehicle_data WHERE entry=40;
INSERT INTO vehicle_data (entry, flags, Spell1, Spell2, Spell3, Spell4, Spell5, Spell6, Spell7, Spell8, Spell9, Spell10, req_aura) VALUES
(40, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
