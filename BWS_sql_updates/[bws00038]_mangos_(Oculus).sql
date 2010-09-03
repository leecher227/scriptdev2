UPDATE instance_template SET script='instance_oculus' WHERE map=578;
UPDATE item_template set ScriptName = 'item_summon_drake', spellid_1 = '43185' WHERE entry IN (37860,37859,37815);
UPDATE creature_template set ScriptName = 'npc_unstable_sphere' WHERE entry = 28166;
UPDATE creature_template set ScriptName = 'boss_drakos' WHERE entry = 27654;
UPDATE creature_template set ScriptName = 'npc_oculus_vehicle' WHERE entry IN (27755,27692,27756);
UPDATE creature_template set ScriptName = 'npc_oculus_drake' WHERE entry IN (27657,27658,27659);
UPDATE creature_template set ScriptName = 'npc_azure_dragon' WHERE entry = 27638;
UPDATE creature_template set ScriptName = 'boss_eregos' WHERE entry = 27656;
UPDATE creature_template set ScriptName = 'boss_varos' WHERE entry = 27447;
UPDATE creature_template set ScriptName = 'npc_varos_orb' WHERE entry = 28183;
UPDATE creature_template set ScriptName = 'npc_vapos_beam_target' WHERE entry = 28239;
UPDATE creature_template set ScriptName = 'npc_robot' WHERE entry = 27641;
UPDATE creature_template set ScriptName = 'boss_urom' WHERE entry = 27655;
UPDATE creature_template set ScriptName = 'npc_planar_anomaly' WHERE entry = 30879;
UPDATE creature_template set ScriptName = 'npc_oculus_hatch' WHERE entry = 28276;

REPLACE INTO spell_script_target (entry, type, targetEntry) values
(61407, 1, 27447),
(51024, 1, 28239),
(51022, 1, 28239),
(57963, 1, 27656);

UPDATE creature_template SET InhabitType=3 WHERE entry IN (27638, 27656, 28276);
UPDATE gameobject_template SET faction =0 WHERE entry = 193995;
DELETE FROM creature WHERE id IN (28239, 28166);

-- Vehicle data (Ruby Drake, Amber Drake, Emerald Drake)
REPLACE INTO vehicle_data (entry, flags, Spell1, Spell2, Spell3, Spell4, Spell5, Spell6, Spell7, Spell8, Spell9, Spell10, req_aura) VALUES
(43, 12, 50232, 50248, 50240, 50253, 0, 0, 0, 0, 0, 0, 0),
(29, 12, 49840, 49838, 49592, 0, 0, 0, 0, 0, 0, 0, 0),
(39, 12, 50328, 50341, 50344, 0, 0, 0, 0, 0, 0, 0, 0);
REPLACE INTO  creature_template_addon (entry, mount, bytes1, bytes2, emote, moveflags, vehicle_id, passengers, auras) VALUES
(27756, 0, 0, 0, 0, 0, 43, NULL, '53112 0 53112 1'),
(27755, 0, 0, 0, 0, 0, 29, NULL, '53112 0 53112 1'),
(27692, 0, 0, 0, 0, 0, 39, NULL, '53112 0 53112 1');
REPLACE INTO vehicle_seat_data (seat,  flags) VALUES
(742, 3),
(422, 3),
(662, 3);
