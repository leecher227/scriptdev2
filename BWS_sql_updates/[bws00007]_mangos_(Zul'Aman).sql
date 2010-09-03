-- Nalorakk last position (hack, must be removed when trash waves will be inplemented).
UPDATE creature SET position_x=-77.4956, position_y=1294.76, position_z=48.487, orientation=1.66278 WHERE id = 23576;
-- rehack
-- UPDATE creature SET position_x=16.9616, position_y=1414.6, position_z=11.9265, orientation=6.23082 WHERE id = 23576;

UPDATE `creature_template` set `ScriptName` = 'mob_akilzon_eagle' where `entry`=24858; /*default event ai*/

UPDATE `creature_template` SET `ScriptName` = 'mob_halazzi_lynx' WHERE `entry` = '24143';

UPDATE `creature_template` SET `ScriptName` = 'mob_janalai_hatcher' WHERE `entry` = '23818';
UPDATE `creature_template` SET `ScriptName` = 'mob_janalai_hatchling' WHERE `entry` = '23598';
UPDATE `creature_template` SET `ScriptName` = 'mob_janalai_egg' WHERE `entry` = '23817';
UPDATE `creature_template` SET `ScriptName` = 'mob_janalai_firebomb' WHERE `entry` = '23920';

UPDATE `creature_template` SET `ScriptName` = 'boss_alyson_antille' WHERE `entry` =24240;
UPDATE `creature_template` SET `ScriptName` = 'boss_thurg' WHERE `entry` =24241;
UPDATE `creature_template` SET `ScriptName` = 'boss_slither' WHERE `entry` =24242;
UPDATE `creature_template` SET `ScriptName` = 'boss_lord_raadan' WHERE `entry` =24243;
UPDATE `creature_template` SET `ScriptName` = 'boss_gazakroth' WHERE `entry` =24244;
UPDATE `creature_template` SET `ScriptName` = 'boss_fenstalker' WHERE `entry` =24245;
UPDATE `creature_template` SET `ScriptName` = 'boss_darkheart' WHERE `entry` =24246;
UPDATE `creature_template` SET `ScriptName` = 'boss_koragg' WHERE `entry` =24247;

UPDATE `creature_template` SET `ScriptName` = 'do_nothing' WHERE `entry` = '24187';
UPDATE `creature_template` SET `ScriptName` = 'mob_zuljin_vortex' WHERE `entry` = '24136';
UPDATE `creature_template` SET `minlevel` = '73', `maxlevel` = '73' WHERE `entry` IN (24187, 24136);
DELETE FROM `spell_script_target` WHERE `entry` = 42577;
INSERT INTO `spell_script_target` VALUES ('42577', '1', '24136');

UPDATE creature SET position_x=118.339, position_y=921.828, position_z=33.8893, orientation=1.60965 WHERE guid=89357;

DELETE FROM `creature_loot_template` WHERE `entry` IN (23790, 23999, 24024, 24001);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33480, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33481, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33483, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33489, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33590, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33591, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33805, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23790, 33971, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23999, 33490, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23999, 33491, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23999, 33492, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23999, 33493, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23999, 33494, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (23999, 33495, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (24024, 33496, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (24024, 33497, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (24024, 33498, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (24024, 33499, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (24024, 33500, 0, 1);
INSERT INTO `creature_loot_template` (`entry`, `item`, `chanceorquestchance`, `groupid`) VALUES (24001, 33809, 100, 0);
UPDATE `creature_template` SET `lootid` = `entry`, `ScriptName` = 'npc_zulaman_hostage' WHERE `entry` IN (23790, 23999, 24024, 24001);

/*doors*/
DELETE FROM `gameobject` WHERE `id` IN (186303, 186304, 186859, 186868);
INSERT INTO `gameobject`  (`id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(186303, 568, 1, 307.178, 1122.81, 10.0798, 0.0546852, 0, 0, 0.0273392, 0.999626, 25, 0, 1),
(186304, 568, 1, 125, 915, 34, 4.67869, 0, 0, 0.718921, -0.695091, 25, 0, 1),
(186859, 568, 1, 120.526, 733.926, 45.1114, 4.72569, 0, 0, 0.70239, -0.711792, 25, 0, 0),
(186858, 568, 1, 337.269, 1396.05, 74.4265, 0.320266, 0, 0, 0.15945, 0.987206, 25, 0, 0);
