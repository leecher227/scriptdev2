update `creature_template` set `ScriptName`='mob_crystal_spike' where `entry`=27099;
update `creature_template` set `ScriptName`='mob_crystalline_tangler' where `entry`=32665;

-- ##### Ice Breath #####
DELETE FROM `creature` WHERE `id`=27048;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(126611, 27048, 576, 3, 1, 1126, 0, 319.794, 13.732, 1.02, 3.79, 7200, 5, 0, 8982, 0, 0, 0),
(126612, 27048, 576, 3, 1, 1126, 0, 285.653, -23.363, 1.13, 0.805, 7200, 5, 0, 8982, 0, 0, 0),
(126613, 27048, 576, 3, 1, 1126, 0, 284.662, 13.757, 1.151, 5.57, 7200, 5, 0, 8982, 0, 0, 0);

UPDATE `creature_template` set AIName = '', ScriptName = 'npc_firebreath_caster' where entry=27048;

