-- #### QUEST: Orbaz Fate ####
UPDATE creature_template SET `equipment_id`='365',`minlevel`='83', `maxlevel`='83', `minhealth`='11156000', `maxhealth`='11156000', `armor`='10627', `faction_A`='35', `faction_H`='35', `npcflag`='0', `mindmg`='120', `maxdmg`='240', `attackpower`='163', `dmg_multiplier`='35', `baseattacktime`='2000' WHERE entry = 31290;
UPDATE creature_template SET `AIName`='', ScriptName = "npc_orbaz_bloodbane" WHERE entry = 31283;
UPDATE creature_template SET `AIName`='', ScriptName = "npc_darion_mograine_fb" WHERE entry = 31290;

-- #### QUEST: The Admiral Revealed ####
DELETE FROM `creature` WHERE `id`=29621;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(887062, 29621, 571, 1, 1, 0, 0, 7494.72, 4871.69, -12.6614, 1.538, 300, 0, 0, 226800, 0, 0, 0);

update `creature_template` set `baseattacktime`= 2000, `dmg_multiplier`=2.7, `attackpower`=157, `mindmg`=420, `maxdmg`=630, `armor`=9765, `faction_A`=14, `faction_H`=14, `mechanic_immune_mask`=12584976, `minmana`=340000, `maxmana`=340000, `minhealth`=226800, `maxhealth`=226800, `minlevel`=80, `maxlevel`=80 where entry = 29620;
UPDATE creature_template SET `AIName`='', ScriptName = "boss_hight_admiral_westwind" WHERE entry = 29621;

-- #### QUEST: The Second Chance ####
UPDATE creature_template SET `AIName`='', ScriptName = "npc_archbishop_landgren" WHERE entry = 29542;
UPDATE creature_template SET `AIName`='', ScriptName = "npc_lord_commander_arete" WHERE entry = 29560;
replace into spell_script_target values (46685, 1, 29572);
update `creature_template` set `InhabitType`=4 where entry=29572;