-- Instance Ulduar
UPDATE instance_template SET ScriptName='instance_ulduar' WHERE map=603;
DELETE FROM areatrigger_scripts WHERE entry=5398;
INSERT INTO areatrigger_scripts (entry, ScriptName) VALUES (5398, 'at_ulduar');
UPDATE gameobject_template SET ScriptName='go_ulduar_teleporter' WHERE entry=194569;
-- Instance Ulduar END

-- Flame Leviathan
UPDATE creature_template SET ScriptName = 'boss_flame_leviathan' WHERE entry = 33113;
UPDATE creature_template SET ScriptName = 'mob_defense_turret' WHERE entry = 33142;
-- Flame Leviathan END

-- Ignis the Furnace Master
UPDATE creature_template SET AIName='', ScriptName='boss_ignis' WHERE entry=33118;
UPDATE creature_template SET AIName='', ScriptName='mob_iron_construct' WHERE entry = 33121;
UPDATE creature_template SET AIName='', ScriptName='mob_scorch_target' WHERE entry=33221;
REPLACE INTO spell_script_target (entry, type, targetEntry) VALUES
(64474, 1, 33118),
(64475, 1, 33118);
-- Vehicle data for Ignis
UPDATE creature_template SET VehicleId=342 WHERE entry IN (33118, 33190);
-- Ignis the Furnace Master END

-- Razorscale
-- CUSTOM spawn repaired harpoon guns
delete from gameobject where id=194543;
insert into gameobject (id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) values
(194543, 603, 3, 2, 571.947, -136.012, 391.517, 4.7328, 0, 0, 0, 1, 180, 255, 1),
(194543, 603, 3, 2, 589.923, -133.622, 391.897, 4.7328, 0, 0, 0, 1, 180, 255, 1),
(194543, 603, 2, 2, 558.646, -140.129, 391.517, 4.7328, 0, 0, 0, 1, 180, 255, 1),
(194543, 603, 2, 2, 606.747, -136.945, 391.353, 4.7328, 0, 0, 0, 1, 180, 255, 1);

-- CUSTOM set flag for repaired harpoon gun to make impossible use more than once
update gameobject_template set flags=flags|16 where entry=194543;

-- CUSTOM remove harpoons dummy
delete from creature where id=33282;

REPLACE INTO spell_script_target (entry, type, targetEntry) VALUES
(63659, 1, 33186),
(62505, 1, 33186);

UPDATE gameobject_template SET ScriptName='go_razorscale_harpoon_launcher' WHERE entry=194543;
UPDATE creature_template SET AIName='', ScriptName='boss_razorscale' WHERE entry=33186;
UPDATE creature_template SET AIName='', ScriptName='mob_mole_machine' WHERE entry=33245;
UPDATE creature_template SET AIName='', ScriptName='mob_dark_rune_watcher' WHERE entry=33453;
UPDATE creature_template SET AIName='', ScriptName='mob_dark_rune_sentinel' WHERE entry=33846;
UPDATE creature_template SET AIName='', ScriptName='mob_dark_rune_guardian' WHERE entry=33388;
UPDATE creature_template SET AIName='', ScriptName='npc_expedition_commander' WHERE entry=33210;
UPDATE creature_template SET AIName='', ScriptName='npc_expedition_npc' WHERE entry in (33287, 33816, 33259);
UPDATE creature_template SET AIName='', ScriptName='mob_devouring_flame_target' WHERE entry IN (34189, 34188);
-- Razorscale END

-- XT-002 Deconstructor
UPDATE creature_template SET AIName='', ScriptName='boss_xt002' WHERE entry = 33293;
UPDATE creature_template SET AIName='', ScriptName='mob_pummeler' WHERE entry = 33344;
UPDATE creature_template SET AIName='', ScriptName='mob_boombot' WHERE entry=33346;
UPDATE creature_template SET AIName='', ScriptName='mob_scrapbot' WHERE entry = 33343;
UPDATE creature_template SET AIName='', ScriptName='mob_xtheart' WHERE entry=33329;
UPDATE creature_template SET AIName='', ScriptName='mob_xt002voidzone' WHERE entry=34001;

-- CUSTOM Hard Mode loot
replace into creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid_1, modelid_2, modelid_3, modelid_4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, minhealth, maxhealth, PowerType, minmana, maxmana, armor, faction_A, faction_H, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, unk16, unk17, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, equipment_id, mechanic_immune_mask, flags_extra, ScriptName) values
('332931','338851','0','0','33293','0','28611','0','0','0','XT-002 Deconstructor','','','0','83','83','5000008','5000008','0','0','0','10673','16','16','0','2.8','1.14286','1','3','509','683','0','805','35','1800','0','1','32832','0','0','0','0','0','0','371','535','135','9','32876','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','353','36320','38132','','0','3','358.552','1','0','0','0','0','0','0','0','321','1','0','650854399','1',''),
('338851','0','0','0','33293','0','28611','0','0','0','XT-002 Deconstructor (1)','','','0','83','83','22499978','22499978','0','0','0','10673','16','16','0','2.8','1.14286','1','3','509','683','0','805','70','2000','0','1','32832','0','0','0','0','0','0','371','535','135','9','32876','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','72640','76264','','0','3','1613.48','1','0','0','0','0','0','0','0','321','1','0','650854399','1','');
-- 10 ppl
UPDATE creature_template SET lootid = entry WHERE entry = 33293;
DELETE FROM creature_loot_template WHERE entry = 33293;
INSERT INTO creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
(33293,47241,100,0,1,1,0,0,0),
-- --
(33293,45680,0,1,1,1,0,0,0),
(33293,45694,0,1,1,1,0,0,0),
(33293,45687,0,1,1,1,0,0,0),
(33293,45685,0,1,1,1,0,0,0),
(33293,45686,0,1,1,1,0,0,0),
-- --
(33293,45676,0,2,1,1,0,0,0),
(33293,45679,0,2,1,1,0,0,0),
(33293,45675,0,2,1,1,0,0,0),
(33293,45682,0,2,1,1,0,0,0),
(33293,45677,0,2,1,1,0,0,0);
-- Hard
UPDATE creature_template SET lootid = entry WHERE entry = 332931;
DELETE FROM creature_loot_template WHERE entry = 332931;
INSERT INTO creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
(332931,47241,100,0,1,1,0,0,0),
-- -- 
(332931,45680,0,1,1,1,0,0,0),
(332931,45694,0,1,1,1,0,0,0),
(332931,45687,0,1,1,1,0,0,0),
(332931,45685,0,1,1,1,0,0,0),
(332931,45686,0,1,1,1,0,0,0),
-- --
(332931,45676,0,2,1,1,0,0,0),
(332931,45679,0,2,1,1,0,0,0),
(332931,45675,0,2,1,1,0,0,0),
(332931,45682,0,2,1,1,0,0,0),
(332931,45677,0,2,1,1,0,0,0),
-- 226lvl
(332931,45868,0,3,1,1,0,0,0),
(332931,45867,0,3,1,1,0,0,0),
(332931,45869,0,3,1,1,0,0,0),
(332931,45870,0,3,1,1,0,0,0),
(332931,45871,0,3,1,1,0,0,0);
-- 25 ppl
UPDATE creature_template SET lootid = entry WHERE entry = 33885;
DELETE FROM creature_loot_template WHERE entry = 33885;
INSERT INTO creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
(33885, 45087, 50, 0, 1, 1, 0, 0, 0),
(33885, 47241, 100, 0, 1, 1, 0, 0, 0),
(33885, 45038, 10, 0, 1, 1, 0, 0, 0),
(33885, 1, 10, 0, -900002, 1, 0, 0, 0),
-- --
(33885,45246,0,1,1,1,0,0,0),
(33885,45257,0,1,1,1,0,0,0),
(33885,45254,0,1,1,1,0,0,0),
(33885,45255,0,1,1,1,0,0,0),
(33885,45256,0,1,1,1,0,0,0),
-- -- 
(33885,45260,0,2,1,1,0,0,0),
(33885,45252,0,2,1,1,0,0,0),
(33885,45253,0,2,1,1,0,0,0),
(33885,45259,0,2,1,1,0,0,0),
(33885,45258,0,2,1,1,0,0,0),
-- --
(33885,45249,0,3,1,1,0,0,0),
(33885,45250,0,3,1,1,0,0,0),
(33885,45248,0,3,1,1,0,0,0),
(33885,45251,0,3,1,1,0,0,0),
(33885,45247,0,3,1,1,0,0,0),
-- --
(33885, 46348,2,4,1,1,7,333,1),
(33885, 46027,2,4,1,1,7,333,1);
-- Hard
UPDATE creature_template SET lootid = entry WHERE entry = 338851;
DELETE FROM creature_loot_template WHERE entry = 338851;
INSERT INTO creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
(338851, 45087, 100, 0, 1, 1, 0, 0, 0),
(338851, 47241, 100, 0, 1, 1, 0, 0, 0),
(338851, 45038, 20, 0, 1, 1, 0, 0, 0),
(338851, 1, 20, 0, -900002, 1, 0, 0, 0),
-- --
(338851,45246,0,1,1,1,0,0,0),
(338851,45257,0,1,1,1,0,0,0),
(338851,45254,0,1,1,1,0,0,0),
(338851,45255,0,1,1,1,0,0,0),
(338851,45256,0,1,1,1,0,0,0),
-- -- 
(338851,45260,0,2,1,1,0,0,0),
(338851,45252,0,2,1,1,0,0,0),
(338851,45253,0,2,1,1,0,0,0),
(338851,45259,0,2,1,1,0,0,0),
(338851,45258,0,2,1,1,0,0,0),
-- --
(338851,45249,0,3,1,1,0,0,0),
(338851,45250,0,3,1,1,0,0,0),
(338851,45248,0,3,1,1,0,0,0),
(338851,45251,0,3,1,1,0,0,0),
(338851,45247,0,3,1,1,0,0,0),
-- 239
(338851,45445,0,4,1,1,0,0,0),
(338851,45443,0,4,1,1,0,0,0),
(338851,45444,0,4,1,1,0,0,0),
(338851,45446,0,4,1,1,0,0,0),
(338851,45442,0,4,1,1,0,0,0),
-- -- 
(338851, 46348, 4, 5, 1, 1, 7, 333, 1),
(338851, 46027, 4, 5, 1, 1, 7, 333, 1);
-- XT-002 Deconstructor END

-- Assembly of Iron
UPDATE creature_template SET AIName='', ScriptName='boss_brundir' WHERE entry=32857;
UPDATE creature_template SET AIName='', ScriptName='boss_molgeim' WHERE entry=32927;
UPDATE creature_template SET AIName='', ScriptName='boss_steelbreaker' WHERE entry=32867;
UPDATE creature_template SET AIName='', ScriptName='mob_rune_of_power' WHERE entry=33705;
UPDATE creature_template SET AIName='', ScriptName='mob_rune_of_summoning' WHERE entry=33051;
UPDATE creature_template SET AIName='', ScriptName='mob_ulduar_lightning_elemental' WHERE entry=32958;
-- Assembly of Iron END

-- Kologarn
UPDATE creature_template SET AIName='', ScriptName='boss_kologarn' WHERE entry=32930;
UPDATE creature_template SET AIName='', ScriptName='boss_right_arm' WHERE entry=32934;
UPDATE creature_template SET AIName='', ScriptName='boss_left_arm' WHERE entry=32933;
UPDATE creature_template SET AIName='', ScriptName='mob_ulduar_rubble' WHERE entry=33768;
UPDATE creature_template SET AIName='', ScriptName='mob_focused_eyebeam' WHERE entry in (33632, 33802);
-- Kologarn END

-- Auriaya
UPDATE creature_template SET AIName='', ScriptName='boss_auriaya' WHERE entry=33515;
UPDATE creature_template SET AIName='', ScriptName='mob_feral_defender' WHERE entry=34035;
UPDATE creature_template SET AIName='', ScriptName='mob_seeping_feral_essence' WHERE entry=34098;
UPDATE creature_template SET AIName='', ScriptName='mob_sanctum_sentry' WHERE entry = 34014;
-- Auriaya END

-- Freya
UPDATE creature_template SET ScriptName = 'boss_freya' WHERE entry=32906;
UPDATE creature_template SET ScriptName = 'boss_elder_brightleaf' WHERE entry=32915;
UPDATE creature_template SET ScriptName = 'boss_elder_ironbranch' WHERE entry=32913;
UPDATE creature_template SET ScriptName = 'boss_elder_stonebark' WHERE entry=32914;
UPDATE creature_template SET ScriptName = 'mob_iron_roots' WHERE entry IN (33088, 33168);
UPDATE creature_template SET ScriptName = 'mob_freya_ground' WHERE entry IN (33215, 33228, 33170, 33050, 34129);
UPDATE creature_template SET ScriptName = 'mob_freya_spawned' WHERE entry IN (32916, 32919, 33202, 33203, 32918);
-- some aura fixes, this may be wrong
DELETE FROM spell_script_target WHERE entry IN (62525, 62524, 62521, 62385, 62387, 62386);
INSERT INTO spell_script_target (entry, type, targetEntry) VALUES
(62525, 1, 32906),
(62524, 1, 32906),
(62521, 1, 32906),
(62385, 1, 32906),
(62387, 1, 32906),
(62386, 1, 32906);
-- Added hack for Freya's gift
DELETE FROM gameobject WHERE id IN (194324, 194325, 194326, 194327, 194328, 194329, 194330, 194331);
INSERT INTO gameobject (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUES
(733324, 194324, 603, 1, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733325, 194325, 603, 1, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733326, 194326, 603, 1, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733327, 194327, 603, 1, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733328, 194328, 603, 2, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733329, 194329, 603, 2, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733330, 194330, 603, 2, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1),
(733331, 194331, 603, 2, 65535, 2374.876221, -78.512665, 425.073608, 3.260976, 0, 0, 0.706026, 0.708186, -604800, 100, 1);
-- Freya END

-- Hodir
UPDATE creature_template SET ScriptName = 'boss_hodir' WHERE entry=32845;
UPDATE creature_template SET ScriptName = 'mob_toasty_fire' WHERE entry=33342;
UPDATE creature_template SET ScriptName = 'mob_flashFreeze' WHERE entry=32926;
UPDATE creature_template SET modelid_1 = 15880 WHERE entry = 33174;
UPDATE creature_template SET modelid_2 = 28470, ScriptName = 'mob_icicle' WHERE entry = 33169;
-- flash freeze that will lock the npcs IN iceblock
UPDATE creature_template SET modelid_1 = 25865, ScriptName = 'mob_npc_flashFreeze' WHERE entry IN (32938, 33353);
UPDATE creature SET spawnMask = 3 WHERE id IN (32938);
UPDATE creature SET spawnMask = 2 WHERE id IN (32901, 32900, 32950, 32946,33333, 33330, 33326);
UPDATE creature_template SET ScriptName = 'npc_hodir_priest' WHERE entry IN (32897, 33326, 32948, 33330);
UPDATE creature_template SET ScriptName = 'npc_hodir_druid' WHERE entry IN (33325, 32901, 32941, 33333);
UPDATE creature_template SET ScriptName = 'npc_hodir_shaman' WHERE entry IN (33328, 32900, 33332, 32950);
UPDATE creature_template SET ScriptName = 'npc_hodir_mage' WHERE entry IN (32893, 33327, 33331, 32946);
-- FIXED SOME POSITIONING FOR THE FRIENDLY NPCS, Besides this the freeze aura should also be fixed.
-- fixed npc positioning and added 4 extra flashfreeze for them.
-- 10 man: 
-- mage
UPDATE creature SET position_x = 2000.9, position_y = -231.232 WHERE guid = 131930;
-- priest
UPDATE creature SET position_x = 2009.06, position_y = -244.058 WHERE guid = 131933;
DELETE FROM creature WHERE guid IN (800005);
INSERT INTO creature VALUES
(800005, 32897, 603, 3, 128,0,0, 2009.06, -244.058, 432.687, 1.68485, 7200,0,0, 5647, 0, 0, 0);		-- aly priest 
-- shaman
UPDATE creature SET position_x = 1983.75, position_y = -243.358 WHERE id = 33328;
UPDATE creature SET position_x = 1983.75, position_y = -243.358 WHERE id = 33332;
-- druid
UPDATE creature SET position_x = 2021.12, position_y = -236.648 WHERE id = 32941;
UPDATE creature SET position_x = 2021.12, position_y = -236.648 WHERE id = 33325;
-- 25 man:
-- druid
UPDATE creature SET position_x = 2013.5, position_y = -240.338 WHERE id = 32901;
DELETE FROM creature WHERE guid IN (800006);
INSERT INTO creature VALUES
(800006, 32938, 603, 2, 1,0,0, 2013.5, -240.338, 432.687, 1.68485, 7200,0,0, 5647, 0, 0, 0);
-- shaman:
UPDATE creature SET position_x = 2011.48, position_y = -232.79 WHERE id = 32900;
UPDATE creature SET position_x = 2011.48, position_y = -232.79 WHERE id = 32950;
DELETE FROM creature WHERE guid IN (800007);
INSERT INTO creature VALUES
(800007, 32938, 603, 2, 1,0,0, 2011.48, -232.79, 432.687, 1.68485, 7200,0,0, 5647, 0, 0, 0);
-- mage:
DELETE FROM creature WHERE guid IN (800008, 800010);
INSERT INTO creature VALUES
(800008, 33327, 603, 2, 128,0,0, 1978.49, -241.476, 432.687, 1.68485, 7200,0,0, 5647, 0, 0, 0), -- aly mage
(800010, 32938, 603, 2, 1,0,0, 1978.49, -241.476, 432.687, 1.68485, 7200,0,0, 5647, 0, 0, 0);
-- priest
UPDATE creature SET position_x = 1997.88, position_y = -239.394 WHERE id = 33330;
DELETE FROM creature WHERE guid IN (800009);
INSERT INTO creature VALUES
(800009, 32938, 603, 2, 1,0,0, 1997.88, -239.394, 432.687, 1.68485, 7200,0,0, 5647, 0, 0, 0);
-- Hodir END

-- Mimiron
UPDATE creature_template SET ScriptName = "boss_mimiron" WHERE entry = 33350;
UPDATE creature_template SET RegenHealth= 0, ScriptName = "boss_leviathan_mk" WHERE entry = 33432;
UPDATE creature_template SET ScriptName = "leviathan_turret" WHERE entry = 34071;
UPDATE creature_template SET ScriptName = "mob_proximity_mine" WHERE entry = 34362;
DELETE FROM spell_script_target WHERE entry IN (63274, 63414);
REPLACE INTO spell_script_target (entry, type, targetEntry) values('63414','1','33835');
REPLACE INTO spell_script_target (entry, type, targetEntry) values('63274','1','33835');
REPLACE INTO spell_script_target (entry, type, targetEntry) values('64444','1','33670');
UPDATE creature_template SET RegenHealth= 0, ScriptName = "boss_vx001" WHERE entry = 33651;
UPDATE creature_template SET ScriptName = "mob_assault_bot" WHERE entry = 34057;
UPDATE creature_template SET ScriptName = "mob_bomb_bot" WHERE entry in (33836, 34192);
UPDATE creature_template SET faction_A = 14, faction_H = 14, minlevel = 80, maxlevel = 80, ScriptName = "mob_emergency_bot" WHERE entry = 34147;
UPDATE creature_template SET RegenHealth= 0, ScriptName = "boss_aerial_command_unit" WHERE entry = 33670;
UPDATE creature_template SET ScriptName = "npc_mimiron_focus" WHERE entry = 33835;
UPDATE creature_template SET flags_extra=flags_extra|2 WHERE entry in (34047, 33835, 34143);
UPDATE creature_template SET ScriptName = "mob_magnetic_core" WHERE entry = 34068;
replace into creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) values('34057','46029','100','0','1','1','0','0','0');
UPDATE creature_template SET lootid = 34057 WHERE entry = 34057;
UPDATE gameobject_template SET flags = 6553632, data2 = 2000, ScriptName="go_red_button" WHERE entry = 194739;
UPDATE creature_template SET ScriptName = "npc_mimiron_computer" WHERE entry = 34143;
DELETE FROM creature where id=34143;
UPDATE creature_template SET ScriptName = "mob_frost_bomb" WHERE entry = 34149;
UPDATE creature_template SET ScriptName = "mob_mimiron_flames" WHERE entry in (34363, 34121);
-- Mimiron END

-- Thorim
UPDATE creature_template SET ScriptName = 'boss_thorim' WHERE entry = 32865;
UPDATE creature_template SET ScriptName = 'boss_runic_colossus' WHERE entry = 32872;
UPDATE creature_template SET ScriptName = 'boss_ancient_rune_giant' WHERE entry = 32873;
UPDATE creature_template SET ScriptName = 'npc_lightning_orb' WHERE entry = 33138;
UPDATE creature_template SET ScriptName = 'mob_thorim_trap_bunny' WHERE entry IN (33725, 33054);
UPDATE creature_template SET ScriptName = 'mob_thorim_preadds' WHERE entry IN (32885, 32883, 32907, 32908, 32882);
UPDATE creature SET spawnMask = 3 WHERE id = 32873;
UPDATE creature_template SET ScriptName = 'npc_sif' WHERE entry = 33196;
UPDATE gameobject SET position_y = -286.67, position_z = 419.50 WHERE id IN (194312, 194313, 194314, 194315);
UPDATE gameobject_template SET flags = 6553632, ScriptName='go_thorim_lever' WHERE entry = 194264;
-- adds
UPDATE creature_template SET ScriptName = 'mob_dark_rune_acolyte' WHERE entry = 33110;
UPDATE creature_template SET ScriptName = 'mob_dark_rune_champion' WHERE entry = 32876;
UPDATE creature_template SET ScriptName = 'mob_dark_rune_commoner' WHERE entry = 32904;
UPDATE creature_template SET ScriptName = 'mob_dark_rune_warbringer' WHERE entry = 32877;
UPDATE creature_template SET ScriptName = 'mob_dark_rune_ring_guard' WHERE entry = 32874;
UPDATE creature_template SET ScriptName = 'mob_dark_rune_honor_guard' WHERE entry = 33125;
#DELETE FROM gameobject WHERE id = 194264; 
#INSERT INTO gameobject VALUES (110010,194264,603,3,65535,2173.276, -252.805, 420.146, 3.027,0,0,0,0,604800,0,1);
UPDATE creature SET phaseMask = 128 WHERE id IN (32907, 32883); 	-- horde soldiers: phase 128 for aly: 65535
UPDATE creature SET phaseMask = 64 WHERE id IN (32885, 32908);	-- alliance soldiers: phase 64 for horde
DELETE FROM spell_script_target WHERE entry=62565;
INSERT INTO spell_script_target (entry, type, targetEntry) VALUES (62565, 1, 32865);
-- Thorim END

-- General Vezax
UPDATE creature_template SET ScriptName = 'boss_vezax' WHERE entry = 33271;
UPDATE creature_template SET ScriptName = 'boss_vezax' WHERE entry = 33449;
UPDATE creature_template SET ScriptName = 'mob_saronite_animus' WHERE entry = 33524;
UPDATE creature_template SET ScriptName = 'mob_saronite_vapor' WHERE entry = 33488;
-- General Vezax END

-- Yogg-Saron
UPDATE creature_template SET ScriptName = 'boss_yogg_saron' WHERE entry = 33288;
UPDATE creature_template SET RegenHealth= 0, flags_extra = 1, type_flags = 108, ScriptName = 'boss_sara' WHERE entry = 33134;
UPDATE creature SET spawnMask = 3, MovementType = 0 WHERE id = 33134;
UPDATE creature_template SET RegenHealth= 0, ScriptName = 'boss_brain_of_yogg_saron' WHERE entry = 33890;
UPDATE creature SET spawntimesecs = 604800 WHERE id = 33134;
UPDATE creature_template SET ScriptName = 'mob_corruptor_tentacle' WHERE entry = 33985;
UPDATE creature_template SET ScriptName = 'mob_constrictor_tentacle' WHERE entry = 33983;
UPDATE creature_template SET MinHealth = 40000, MaxHealth = 40000, minLevel = 80, maxLevel = 80, ScriptName = 'mob_vision_tentacle' WHERE entry = 33943;
UPDATE creature_template SET MinHealth = 400000, MaxHealth = 400000, ScriptName = 'mob_crusher_tentacle' WHERE entry = 33966;
UPDATE creature_template SET MinHealth = 220000, MaxHealth = 220000, ScriptName = 'mob_guardian_of_yogg_saron' WHERE entry = 33136;
UPDATE creature_template SET ScriptName = 'mob_immortal_guardian' WHERE entry = 33988;
UPDATE creature_template SET faction_A = 14, faction_H = 14, ScriptName = 'mob_death_orb' WHERE entry = 33882;
UPDATE creature_template SET ScriptName = 'mob_sanity_well' WHERE entry = 33991;
UPDATE creature_template SET scriptname='mob_madness_portal' WHERE entry=34072;
UPDATE creature_template SET scriptname='mob_laughing_skull' WHERE entry=33990;
UPDATE creature_template SET scriptname='mob_ominous_cloud' WHERE entry=33292;
UPDATE creature SET spawnMask = 3 WHERE id = 33292;
-- spells
DELETE FROM spell_script_target WHERE entry=63886;
INSERT INTO spell_script_target (entry, type, targetEntry) VALUES (63886, 1, 33882);
-- Keepers
UPDATE creature_template SET ScriptName = 'keeper_hodir' WHERE entry = 33213;
UPDATE creature_template SET ScriptName = 'keeper_freya' WHERE entry = 33241;
UPDATE creature_template SET ScriptName = 'keeper_thorim' WHERE entry = 33242;
UPDATE creature_template SET ScriptName = 'keeper_mimiron' WHERE entry = 33244;
-- INSERT doors & yoggs brain INTO the brain room
DELETE FROM gameobject WHERE id IN (194635);
INSERT INTO gameobject VALUES (110000,194635,603,3,65535,2022.490,-25.389,261.961,0,0,0,0,0,604800,0,1);
DELETE FROM gameobject WHERE guid = 110001;
INSERT INTO gameobject VALUES (110001,194462,603,3,65535,2104.555, -25.635,242.646,0,0,0,0,0,604800,100,1);
DELETE FROM creature WHERE id IN (33890);
INSERT INTO creature VALUES (800000,33890,603,3,65535,0,0,1981.422,-22.442,255.011,0,604800,0,0,1371428,0,0,0);
-- Yogg-Saron END

-- Algalon
UPDATE creature_template SET ScriptName = 'boss_algalon' WHERE entry = 32871;
UPDATE creature_template SET ScriptName = 'mob_collapsing_star' WHERE entry = 32955;
UPDATE creature_template SET ScriptName = 'mob_living_constellation' WHERE entry = 33052;
UPDATE creature_template SET ScriptName = 'mob_black_hole' WHERE entry = 32953;
UPDATE creature_template SET ScriptName = 'mob_cosmic_smash_target' WHERE entry IN (33105, 33104);
UPDATE creature_template SET minhealth = 39099, maxhealth = 39099 WHERE entry = 33089;
UPDATE gameobject_template SET ScriptName='go_celestial_acces' WHERE entry IN (194628, 194752);
-- Algalon END

-- Keepers
-- Keepers images
UPDATE creature_template SET npcflag = 1, unit_flags = 2, ScriptName = 'hodir_image' WHERE entry = 33411;
UPDATE creature_template SET npcflag = 1, unit_flags = 2, ScriptName = 'freya_image' WHERE entry = 33410;
UPDATE creature_template SET npcflag = 1, unit_flags = 2, ScriptName = 'thorim_image' WHERE entry = 33413;
UPDATE creature_template SET npcflag = 1, unit_flags = 2, ScriptName = 'mimiron_image' WHERE entry = 33412;
-- INSERT keepers imagees INTO the db
DELETE FROM creature WHERE guid IN (800001, 800002, 800003, 800004);
INSERT INTO creature VALUES
(800001, 33410, 603, 3, 65535,0,0, 2036.892, 25.621, 411.358, 3.83, 604800,0,0, 5647, 0, 0, 0),		-- Freya 
(800002, 33412, 603, 3, 65535,0,0, 1939.215, 42.677, 411.355, 5.31, 604800,0,0, 5647, 0, 0, 0),		-- Mimiron
(800003, 33411, 603, 3, 65535,0,0, 1939.195, -90.662, 411.357, 1.06, 604800,0,0, 5647, 0, 0, 0), 	-- Hodir
(800004, 33413, 603, 3, 65535,0,0, 2036.674, -73.814, 411.355, 2.51, 604800,0,0, 5647, 0, 0, 0); 	-- Thorim
-- Keepers END
