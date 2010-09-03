DELETE FROM gameobject_scripts WHERE id=24533;
INSERT INTO gameobject_scripts (id, delay, command, datalong, datalong2, dataint, x, y, z, o) VALUES
(24533, 3, 10, 17225, 3000000, 0, -11154.85, -1894.43, 91.47, 0.69);
UPDATE creature_template SET ScriptName = 'boss_nightbane' WHERE entry=17225;
