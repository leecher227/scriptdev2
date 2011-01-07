-- #### QUEST: My Old Enemy ####
DELETE FROM `creature` WHERE `id`=27210;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(300015, 27210, 571, 1, 1, 0, 1608, 2660.69, -359.911, 144.236, 0.15061, 180, 0, 0, 96100, 16545, 0, 0);

DELETE FROM `creature` WHERE `id`=27951;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(85322, 27951, 571, 1, 1, 0, 0, 2660.44, -356.726, 144.312, 5.98648, 25, 0, 0, 126000, 0, 0, 0);

UPDATE creature_template SET `AIName`='', ScriptName = "npc_admiral_westwind" WHERE entry = 27951;
UPDATE creature_template SET `AIName`='', ScriptName = "boss_hight_general_abbendis" WHERE entry = 27210;

-- #### Quest: Lumber Hack ####
update `creature_template` set `IconName`="vehichleCursor", `VehicleId`=300, `spell1`=47939, `maxlevel`=74, `minlevel`=74 where entry=27061;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 27061;
INSERT INTO `npc_spellclick_spells` VALUES
(27061, 47920, 0, 0, 0, 1);  -- zick crosher