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

-- #### QUEST: Tirion's Gambit ####
UPDATE creature_template SET `AIName`='', ScriptName = "npc_ebon_knight" WHERE entry in (32309, 32312, 32311, 32310, 32175);
UPDATE creature_template SET `AIName`='', ScriptName = "npc_tirionTG" WHERE entry = 32239;

UPDATE creature_template SET equipment_id = 365 WHERE entry = 32312;
UPDATE creature_template SET equipment_id = 647 WHERE entry = 32311;
UPDATE creature_template SET equipment_id = 647 WHERE entry = 32310;
UPDATE creature_template SET equipment_id = 32309 WHERE entry = 32309;

replace creature_equip_template (`entry`, `equipentry1`, `equipentry2`, `equipentry3`) VALUES (32309, 38707, 0, 0);
replace creature_equip_template (`entry`, `equipentry1`, `equipentry2`, `equipentry3`) VALUES (32241, 13160, 0, 0);

UPDATE `creature_template` SET `unit_flags`=514, `baseattacktime`=2000, `dmg_multiplier`=2, `attackpower`=175, `mindmg`=468, `maxdmg`=702, `armor`=10627, `faction_A`=14, `faction_H`=14, `mechanic_immune_mask`=12584976, `minmana`=0, `maxmana`=0, `minhealth`=27890000, `maxhealth`=27890000, `minlevel`=83, `maxlevel`=83 WHERE entry=32184;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=2.2, `attackpower`=163, `mindmg`=435, `maxdmg`=653, `armor`=10627, `faction_A`=2070, `faction_H`=2070, `mechanic_immune_mask`=12584976, `minmana`=4258, `maxmana`=4258, `minhealth`=2789000, `maxhealth`=2789000, `minlevel`=83, `maxlevel`=83 WHERE entry=32239;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=1.1, `attackpower`=146, `mindmg`=391, `maxdmg`=585, `armor`=9633, `faction_A`=2070, `faction_H`=2070, `mechanic_immune_mask`=0, `minmana`=19970, `maxmana`=19970, `minhealth`=63000, `maxhealth`=63000, `minlevel`=80, `maxlevel`=80 WHERE entry=32241;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=2.2, `attackpower`=146, `mindmg`=391, `maxdmg`=585, `armor`=9633, `faction_A`=233, `faction_H`=233, `mechanic_immune_mask`=0, `minmana`=19970, `maxmana`=19970, `minhealth`=126000, `maxhealth`=126000, `minlevel`=80, `maxlevel`=80 WHERE entry=32272;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=2.2, `attackpower`=146, `mindmg`=391, `maxdmg`=585, `armor`=9633, `faction_A`=2144, `faction_H`=2144, `mechanic_immune_mask`=0, `minmana`=19970, `maxmana`=19970, `minhealth`=63000, `maxhealth`=63000, `minlevel`=80, `maxlevel`=80 WHERE entry=32309;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=2, `attackpower`=157, `mindmg`=420, `maxdmg`=630, `armor`=9730, `faction_A`=2036, `faction_H`=2036, `mechanic_immune_mask`=0, `minmana`=0, `maxmana`=0, `minhealth`=252000, `maxhealth`=252000, `minlevel`=80, `maxlevel`=80 WHERE entry=32310;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=2.2, `attackpower`=146, `mindmg`=391, `maxdmg`=585, `armor`=9633, `faction_A`=2140, `faction_H`=2140, `mechanic_immune_mask`=0, `minmana`=79880, `maxmana`=79880, `minhealth`=252000, `maxhealth`=252000, `minlevel`=80, `maxlevel`=80 WHERE entry=32311;
UPDATE `creature_template` SET `baseattacktime`=2000, `dmg_multiplier`=19.5, `attackpower`=163, `mindmg`=491, `maxdmg`=653, `armor`=10521, `faction_A`=2144, `faction_H`=2144, `mechanic_immune_mask`=12584976, `minmana`=3406400, `maxmana`=3406400, `minhealth`=11156000, `maxhealth`=11156000, `minlevel`=83, `maxlevel`=83 WHERE entry=32312;

DELETE FROM `creature` WHERE `id`=32239;
DELETE FROM `creature` WHERE `id`=32312;
DELETE FROM `creature` WHERE `id`=32311;
DELETE FROM `creature` WHERE `id`=32310;
DELETE FROM `creature` WHERE `id`=32241;
DELETE FROM `creature` WHERE `id`=32272;

update creature set spawntimesecs = 25 where id=32175;

INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(4457407, 32241, 571, 1, 1, 0, 0, 6242.29, 2669.85, 570.25, 5.25485, 25, 0, 0, 63000, 19970, 0, 0),
(4457406, 32241, 571, 1, 1, 0, 0, 6255.12, 2664.1, 570.25, 3.39346, 25, 0, 0, 63000, 19970, 0, 0),
(4457405, 32241, 571, 1, 1, 0, 0, 6250.73, 2654.77, 570.25, 2.03472, 25, 0, 0, 63000, 19970, 0, 0);

INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(4457403, 32272, 571, 1, 1, 0, 0, 6130.59, 2765.31, 573.915, 5.12832, 25, 0, 0, 126000, 19970, 0, 0);

INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(4457404, 32239, 571, 1, 1, 0, 0, 6238.71, 2659.87, 570.25, 0.432504, 362, 0, 0, 2789000, 4258, 0, 0);

DELETE FROM `gameobject` WHERE `id`=193941;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(785247, 193941, 571, 1, 0, 6135.29, 2755.13, 573.914, 1.15124, 0, 0, 0.544355, 0.838855, -60, 0, 1);

DELETE FROM `gameobject` WHERE `id`=193794;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(785246, 193794, 571, 1, 0, 6132.78, 2760.67, 573.914, 1.9979, 0, 0, 0.840902, 0.541187, -120, 0, 1);

UPDATE `gameobject_template` SET `faction`=1375 WHERE `entry`=193794;
UPDATE `gameobject_template` SET `ScriptName`="go_escape_portal" WHERE entry = 193941;

-- #### QUEST: Banshee Revange ####
UPDATE creature_template SET ScriptName = "npc_vardmadra" WHERE entry = 31029;
UPDATE creature_template SET ScriptName = "npc_balargarde" WHERE entry = 31016;
UPDATE gameobject_template SET ScriptName="go_balargarde_horn" WHERE entry = 193028;
update `creature_template` set `faction_A`='974', `faction_H`='974', `unit_flags`='768', `type_flags`='268435564' where `entry`=31083;
update `creature_template` set `VehicleId`=40 where `entry` in (31050, 31056);

-- ##### QUEST: The Air Stand Still #####
UPDATE `creature_template` SET `equipment_id`='365',`minlevel`='83', `maxlevel`='83', `minhealth`='11156000', `maxhealth`='11156000', `armor`='10627', `faction_A`='35', `faction_H`='35', `npcflag`='0', `mindmg`='120', `maxdmg`='240', `attackpower`='163', `dmg_multiplier`='35', `baseattacktime`='2000', `ScriptName`="npc_death_knight_as" WHERE entry = 30838;
UPDATE `creature_template` SET `equipment_id`='30839',`baseattacktime`= 2000, `dmg_multiplier`= 2.2, `attackpower`=146, `mindmg`=391, `maxdmg`=585, `armor`=9633, `faction_A`=2006, `faction_H`=2006, `mechanic_immune_mask`=1, `minmana`=39940, `maxmana`=39940, `minhealth`=441000, `maxhealth`=441000, `minlevel`=80, `maxlevel`=80, `ScriptName`="npc_death_knight_as" WHERE entry=30839; -- From Lanc
UPDATE `creature_template` SET `equipment_id`='448', `baseattacktime`= 2000, `dmg_multiplier`= 2.2, `attackpower`=146, `mindmg`=391, `maxdmg`=585, `armor`=9633, `faction_A`=2006, `faction_H`=2006, `mechanic_immune_mask`=1, `minmana`=39940, `maxmana`=39940, `minhealth`=441000, `maxhealth`=441000, `minlevel`=80, `maxlevel`=80, `ScriptName`="npc_death_knight_as" WHERE entry=30840; -- From Lanc Big Thanks
UPDATE creature_template SET ScriptName = "npc_death_gate_as" WHERE entry in (30841, 30850, 30852);
UPDATE creature_template SET ScriptName = "npc_scourge_generals_as" WHERE entry in (30829, 30830, 30831);
 
DELETE FROM `creature_equip_template` WHERE (`entry`=30839);
INSERT INTO `creature_equip_template` (`entry`, `equipentry1`, `equipentry2`, `equipentry3`) VALUES (30839, 38632, 0, 0);
