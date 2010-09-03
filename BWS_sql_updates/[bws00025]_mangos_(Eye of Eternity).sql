UPDATE instance_template SET script='instance_eye_of_eternity' where map=616;
UPDATE gameobject_template SET ScriptName='go_focusing_iris' WHERE entry IN (193958, 193960);
UPDATE creature_template SET ScriptName='boss_malygos' WHERE entry=28859;
UPDATE creature_template SET ScriptName='npc_power_spark' WHERE entry=30084;
UPDATE creature_template SET ScriptName='npc_nexus_lord' WHERE entry=30245;
UPDATE creature_template SET ScriptName='npc_scion_of_eternity' WHERE entry=30249;
UPDATE creature_template SET ScriptName='npc_hover_disk' WHERE entry=30248;
UPDATE creature_template SET ScriptName='npc_alexstrasza' WHERE entry=32295;

-- CUSTOM - Malygos InhabitType
UPDATE creature_template SET InhabitType=1 WHERE entry in (28859, 31734);

-- spawn Alexstrasza's Gift and Heart of Magic
DELETE FROM gameobject WHERE id IN (193905, 193967, 194158, 194159);
INSERT INTO gameobject (id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(193905, 616, 1, 1, 754.544, 1301.71, 220.083, 0, 0, 0, 0, 0, -604800, 100, 1),
(193967, 616, 2, 1, 754.544, 1301.71, 220.083, 0, 0, 0, 0, 0, -604800, 100, 1),
(194158, 616, 1, 1, 759.544, 1306.71, 225.083, 0, 0, 0, 0, 0, -604800, 100, 1),
(194159, 616, 2, 1, 759.544, 1306.71, 225.083, 0, 0, 0, 0, 0, -604800, 100, 1);

-- vehicle data for Hover Disks and Wyrmrest Skytalons (normal and heroic)
DELETE FROM creature_template_addon WHERE entry IN (30161, 31752, 30248, 31749);
INSERT INTO creature_template_addon (entry, mount, bytes1, bytes2, emote, moveflags, vehicle_id, passengers, auras) VALUES
(30161, 0, 0, 0, 0, 0, 194, NULL, NULL),
(31752, 0, 0, 0, 0, 0, 194, NULL, NULL),
(30248, 0, 0, 0, 0, 0, 224, NULL, NULL),
(31749, 0, 0, 0, 0, 0, 224, NULL, NULL);
DELETE FROM vehicle_data WHERE entry IN (194, 224);
INSERT INTO vehicle_data (entry, flags, Spell1, Spell2, Spell3, Spell4, Spell5, Spell6, Spell7, Spell8, Spell9, Spell10, req_aura) VALUES
(194, 280, 56091, 56092, 57090, 57143, 57108, 57092, 0, 0, 0, 0, 0),
(224, 1802, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
DELETE FROM vehicle_seat_data WHERE seat IN (2102, 2224);
INSERT INTO vehicle_seat_data (seat, flags) VALUES
(2102, 3),
(2224, 7);
DELETE FROM npc_spellclick_spells WHERE npc_entry=30248;
INSERT INTO npc_spellclick_spells (npc_entry, spell_id, quest_start, quest_start_active, quest_end, cast_flags) VALUES
(30248, 48754, 0, 0, 0, 1);
