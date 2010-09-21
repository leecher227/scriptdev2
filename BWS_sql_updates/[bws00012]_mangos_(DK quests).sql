-- Into the Realm of Shadows
UPDATE `creature_template` SET `ScriptName`='npc_salanar_the_horseman' WHERE `entry`='28653';
UPDATE `creature_template` SET `ScriptName`='mob_dark_rider_of_acherus' WHERE `entry`='28768';

-- The Gift That Keeps On Giving
UPDATE `creature_template` SET `ScriptName`='mob_scarlet_miner' WHERE `entry`='28822';

-- How To Win Friends And Influence Enemies
UPDATE `creature_template` SET `ScriptName`='npc_crusade_persuaded' WHERE `entry` IN (28939,28940,28610);

-- Ambush At The Overlook
DELETE FROM `creature` WHERE `id` = 28957;
INSERT INTO `creature` (`id`, `map`, `phaseMask`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES 
(28957, 609, 4, 1, 0, 0, 1416.466, -6017.807, 116.357, 1, 360, 0, 0, 1, 0, 0, 1);
UPDATE `creature_template` SET `ScriptName`='mob_scarlet_courier_controller',flags_extra=130 WHERE `entry`='28957';
UPDATE `creature_template` SET `ScriptName`='mob_scarlet_courier' WHERE `entry`='29076';

-- Bloody Breakout
UPDATE `creature_template` SET `ScriptName`='mob_high_inquisitor_valroth', minmana=6489, maxmana=6489, unit_flags=32768 WHERE `entry`='29001';

-- The Light of Dawn (to do)
UPDATE `creature_template` SET `ScriptName`='npc_highlord_darion_mograine' WHERE `entry`='29173';
UPDATE `creature_template` SET `ScriptName`='npc_the_lich_king_tirion_dawn' WHERE `entry` in (29183,29175);
DELETE FROM `spell_script_target` WHERE `entry` in (53658, 53679, 53701, 53705, 53706, 53677, 53685);
-- insert into spell_script_target values (53658, 1, 29173);
insert into spell_script_target values (53679, 1, 29183);
insert into spell_script_target values (53701, 1, 29175);
insert into spell_script_target values (53705, 1, 29183);
insert into spell_script_target values (53706, 1, 29183);
insert into spell_script_target values (53677, 1, 29227);
insert into spell_script_target values (53685, 1, 29175);

DELETE FROM `gameobject` WHERE `id` IN (191330);
INSERT INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
('191330','609','1','128','2283.896','-5287.914','83.066','1','0.0','0.0','0.0','1.0','180','0','1');
