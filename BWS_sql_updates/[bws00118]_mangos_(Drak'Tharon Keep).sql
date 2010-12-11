-- ScriptName
UPDATE creature_template SET AIName='', ScriptName='npc_drakuru_dt' WHERE entry=28016;
UPDATE creature_template SET AIName='', ScriptName='boss_trollgore' WHERE entry=26630;
UPDATE creature_template SET AIName='', ScriptName='npc_drakkari_invader' WHERE entry=27709;
UPDATE creature_template SET AIName='', ScriptName='boss_novos' WHERE entry=26631;
UPDATE gameobject_template SET ScriptName='go_novos_crystal' WHERE entry IN (189301, 189300, 189302, 189299);
UPDATE creature_template SET AIName='', ScriptName='npc_crystal_handler' WHERE entry=26627;
UPDATE creature_template SET AIName='', ScriptName='boss_dred' WHERE entry=27483;
UPDATE creature_template SET AIName='', ScriptName='boss_tharonja' WHERE entry = 26632;

-- spell_script_target
DELETE FROM spell_script_target WHERE entry IN (51825, 49555, 59087, 52106);
INSERT INTO spell_script_target (entry, type, targetEntry) VALUES
(51825, 1, 28016),
(49555, 1, 27709),
(59807, 1, 27709),
(52106, 1, 26631);

-- Trollgore
UPDATE creature_template SET InhabitType=3 WHERE entry in (27709, 27754);
DELETE from creature WHERE id=27753;
DELETE from creature WHERE id=27709;
UPDATE creature_template SET VehicleId=40 WHERE entry=27724;

-- Novos
UPDATE gameobject_template SET faction=0, flags=48 WHERE entry IN (189301, 189300, 189302, 189299);
DELETE FROM creature WHERE id=26712;
UPDATE creature_template SET InhabitType=InhabitType|4 WHERE entry=26712;
UPDATE creature SET spawndist=0, MovementType=0 WHERE id=26631;
UPDATE creature_template SET flags_extra=0 WHERE entry=26712;
INSERT INTO creature (id, map, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, DeathState, MovementType) VALUES
(26712, 600, 3, 1, 17188, 0, -365.477, -724.849, 32.2241, 3.92699, 3600, 5, 0, 4050, 0, 0, 0),
(26712, 600, 3, 1, 17188, 0, -365.368, -751.128, 32.3213, 2.35619, 3600, 5, 0, 4050, 0, 0, 0),
(26712, 600, 3, 1, 17188, 0, -392.123, -750.941, 32.2796, 0.680678, 3600, 5, 0, 4050, 0, 0, 0),
(26712, 600, 3, 1, 17188, 0, -392.455, -724.809, 32.1685, 5.35816, 3600, 5, 0, 4050, 0, 0, 0);
UPDATE gameobject SET spawntimesecs=1 WHERE id IN (189301, 189300, 189302, 189299);

-- Dred
DELETE FROM creature WHERE id IN (27709, 27753, 27490);

-- Drakuru Event
UPDATE creature_template SET faction_A=974, faction_H=974, unit_flags=768, type_flags=268435564 WHERE entry=28498;
-- Hack. Object must be summoned by spell... What spell?
DELETE from gameobject WHERE id IN (190595, 190596, 190597);
insert INTO gameobject (id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(190595, 600, 3, 1, -233.398, -674.122, 131.85, 3.80858, 0, 0, 0.944904, -0.327346, 25, 0, 1),
(190596, 600, 3, 1, -237.207, -680.423, 131.849, 1.65266, 0, 0, 0.73545, 0.677579, 25, 0, 1),
(190597, 600, 3, 1, -240.957, -674.1, 131.849, 5.85847, 0, 0, 0.210766, -0.977537, 25, 0, 1);
