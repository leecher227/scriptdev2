-- scriptnames
UPDATE creature_template SET AIName='', ScriptName='npc_drakuru_dt' WHERE entry=28016;
UPDATE creature_template SET AIName='', ScriptName='boss_trollgore' WHERE entry=26630;
UPDATE creature_template SET AIName='', ScriptName='npc_drakkari_invaider' WHERE entry IN (27753, 27709, 27754);
UPDATE creature_template SET AIName='', ScriptName='boss_novos' WHERE entry=26631;
UPDATE gameobject_template SET ScriptName='go_novos_crystal' WHERE entry IN (189301, 189300, 189302, 189299);
UPDATE creature_template SET AIName='', ScriptName='npc_risen_shadowcaster' WHERE entry=27600;
UPDATE creature_template SET AIName='', ScriptName='npc_crystal_handler' WHERE entry=26627;

-- spell_script_target
REPLACE INTO spell_script_target (entry, type, targetEntry) VALUES
('51825','1','28016'),
('49555','1','27753'),
('59087','1','27753'),
('49555','1','27709'),
('59087','1','27709'),
('49555','1','27754'),
('59087','1','27754'),
('52106','1','26631');

-- Drakuru Event
UPDATE `creature_template` set `faction_A`='974', `faction_H`='974', `unit_flags`='768', `type_flags`='268435564' where `entry`=28498;

-- hack object must be summoned spell... What is spell?
delete from `gameobject` where `id` in (190595, 190596, 190597);
insert INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(190595, 600, 3, 1, -233.398, -674.122, 131.85, 3.80858, 0, 0, 0.944904, -0.327346, 25, 0, 1),
(190596, 600, 3, 1, -237.207, -680.423, 131.849, 1.65266, 0, 0, 0.73545, 0.677579, 25, 0, 1),
(190597, 600, 3, 1, -240.957, -674.1, 131.849, 5.85847, 0, 0, 0.210766, -0.977537, 25, 0, 1);

-- Trollgore
UPDATE `creature_template` set `InhabitType`='3' where entry in (27753, 27709, 27754);
delete from `creature` where `id`=27753;
delete from `creature` where `id`=27709;
update `creature_template` set `VehicleId`=40 where `entry`=27724;

-- Novos
UPDATE gameobject_template SET faction = '0', flags='48' WHERE entry IN (189301, 189300, 189302, 189299);

DELETE FROM `creature` WHERE `id`=26712;
UPDATE creature_template SET InhabitType=InhabitType|4 where entry=26712;
update `creature` set `MovementType`=0 where `id`=26631;
update `creature_template` set `flags_extra`=0, `unit_flags`=33554950 where entry=26712;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(927500, 26712, 600, 3, 1, 17188, 0, -365.477, -724.849, 32.2241, 3.92699, 3600, 5, 0, 4050, 0, 0, 0),
(927501, 26712, 600, 3, 1, 17188, 0, -365.368, -751.128, 32.3213, 2.35619, 3600, 5, 0, 4050, 0, 0, 0),
(927502, 26712, 600, 3, 1, 17188, 0, -392.123, -750.941, 32.2796, 0.680678, 3600, 5, 0, 4050, 0, 0, 0),
(927503, 26712, 600, 3, 1, 17188, 0, -392.455, -724.809, 32.1685, 5.35816, 3600, 5, 0, 4050, 0, 0, 0);
update `creature` set `spawntimesecs`=1 where id in (189301,189300,189302,189299);

-- Dred
UPDATE `creature_template` set AIName = '', ScriptName = 'boss_dred' where entry = 27483;
DELETE FROM creature WHERE id in (27709, 27753, 27490);

-- tharonja
UPDATE `creature_template` set AIName = '', ScriptName = 'boss_tharonja' where entry = 26632;
