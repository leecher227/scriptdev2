UPDATE creature_template SET ScriptName='mob_fire_cyclone' WHERE entry = 30648;
UPDATE creature_template SET ScriptName='mob_flame_tsunami' WHERE entry = 30616;

-- CUSTOM hack to different hard mode loot
REPLACE INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid_1, modelid_2, modelid_3, modelid_4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, minhealth, maxhealth, PowerType, minmana, maxmana, armor, faction_A, faction_H, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, unk16, unk17, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, equipment_id, mechanic_immune_mask, flags_extra, ScriptName) VALUES
(288601, 31311, 0, 0, 0, 0, 27035, 0, 0, 0, 'Sartharion', 'The Onyx Guardian', '', 0, 83, 83, 2510100, 2510100, 0, 0, 0, 10673, 103, 103, 0, 1.6, 1.42857, 1, 3, 509, 683, 0, 805, 35, 2000, 0, 1, 32832, 0, 0, 0, 0, 0, 0, 371, 535, 135, 2, 108, 288601, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 180, 1, 0, 0, 0, 0, 0, 0, 0, 150, 1, 0, 617299803, 1, ''),
(288602, 31311, 0, 0, 0, 0, 27035, 0, 0, 0, 'Sartharion', 'The Onyx Guardian', '', 0, 83, 83, 2510100, 2510100, 0, 0, 0, 10673, 103, 103, 0, 1.6, 1.42857, 1, 3, 509, 683, 0, 805, 35, 2000, 0, 1, 32832, 0, 0, 0, 0, 0, 0, 371, 535, 135, 2, 108, 288602, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 180, 1, 0, 0, 0, 0, 0, 0, 0, 150, 1, 0, 617299803, 1, ''),
(288603, 31311, 0, 0, 0, 0, 27035, 0, 0, 0, 'Sartharion', 'The Onyx Guardian', '', 0, 83, 83, 2510100, 2510100, 0, 0, 0, 10673, 103, 103, 0, 1.6, 1.42857, 1, 3, 509, 683, 0, 805, 35, 2000, 0, 1, 32832, 0, 0, 0, 0, 0, 0, 371, 535, 135, 2, 108, 288603, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 180, 1, 0, 0, 0, 0, 0, 0, 0, 150, 1, 0, 617299803, 1, ''),
(313111, 0, 0, 0, 28860, 0, 27035, 0, 0, 0, 'Sartharion (1)', 'The Onyx Guardian', '', 0, 83, 83, 7669750, 7669750, 0, 0, 0, 10673, 103, 103, 0, 1.6, 1.42857, 1, 3, 509, 683, 0, 805, 70, 2000, 0, 1, 32832, 0, 0, 0, 0, 0, 0, 371, 535, 135, 2, 108, 313111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 550, 1, 0, 0, 0, 0, 0, 0, 0, 150, 1, 0, 617299803, 1, ''),
(313112, 0, 0, 0, 28860, 0, 27035, 0, 0, 0, 'Sartharion (1)', 'The Onyx Guardian', '', 0, 83, 83, 7669750, 7669750, 0, 0, 0, 10673, 103, 103, 0, 1.6, 1.42857, 1, 3, 509, 683, 0, 805, 70, 2000, 0, 1, 32832, 0, 0, 0, 0, 0, 0, 371, 535, 135, 2, 108, 313112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 550, 1, 0, 0, 0, 0, 0, 0, 0, 150, 1, 0, 617299803, 1, ''),
(313113, 0, 0, 0, 28860, 0, 27035, 0, 0, 0, 'Sartharion (1)', 'The Onyx Guardian', '', 0, 83, 83, 7669750, 7669750, 0, 0, 0, 10673, 103, 103, 0, 1.6, 1.42857, 1, 3, 509, 683, 0, 805, 70, 2000, 0, 1, 32832, 0, 0, 0, 0, 0, 0, 371, 535, 135, 2, 108, 313113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 550, 1, 0, 0, 0, 0, 0, 0, 0, 150, 1, 0, 617299803, 1, '');

-- boss loot
DELETE FROM creature_loot_template WHERE entry IN (30452, 30451, 30449, 28860, 288601, 288602, 288603, 31534, 31520, 31535, 31311, 313111, 313112, 313113);
-- Tenebron, Shadron, Vesperon
UPDATE creature_template SET lootid=30449 WHERE entry IN (30452, 30451, 30449, 31534, 31520, 31535);
INSERT INTO creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
(30449, 45624, 100, 0, 1, 1, 0, 0, 0);
-- Sartharion
-- references
DELETE FROM reference_loot_template WHERE entry IN (615000, 615001, 615002, 615010, 615011, 615012);
INSERT INTO reference_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
-- normal, no drakes
(615000, 40426, 0, 2, 1, 1, 0, 0, 0),
(615000, 40427, 0, 2, 1, 1, 0, 0, 0),
(615000, 40428, 0, 2, 1, 1, 0, 0, 0),
(615000, 40429, 0, 2, 1, 1, 0, 0, 0),
(615000, 40430, 0, 2, 1, 1, 0, 0, 0),
(615000, 40613, 0, 1, 1, 1, 0, 0, 0),
(615000, 40614, 0, 1, 1, 1, 0, 0, 0),
(615000, 40615, 0, 1, 1, 1, 0, 0, 0),
(615000, 43345, 100, 0, 1, 1, 0, 0, 0),
(615000, 43347, 100, 0, 1, 1, 0, 0, 0),
-- normal, 1 drake
(615001, 43992, 0, 1, 1, 1, 0, 0, 0),
(615001, 43988, 0, 1, 1, 1, 0, 0, 0),
(615001, 43990, 0, 1, 1, 1, 0, 0, 0),
(615001, 43989, 0, 1, 1, 1, 0, 0, 0),
(615001, 43991, 0, 1, 1, 1, 0, 0, 0),
-- normal, 2 drakes
(615002, 43995, 0, 1, 1, 1, 0, 0, 0),
(615002, 43998, 0, 1, 1, 1, 0, 0, 0),
(615002, 43994, 0, 1, 1, 1, 0, 0, 0),
(615002, 43996, 0, 1, 1, 1, 0, 0, 0),
(615002, 43993, 0, 1, 1, 1, 0, 0, 0),
-- heroic, no drakes
(615010, 40431, 0, 2, 1, 1, 0, 0, 0),
(615010, 40432, 0, 3, 1, 1, 0, 0, 0),
(615010, 40433, 0, 3, 1, 1, 0, 0, 0),
(615010, 40437, 0, 2, 1, 1, 0, 0, 0),
(615010, 40438, 0, 3, 1, 1, 0, 0, 0),
(615010, 40439, 0, 3, 1, 1, 0, 0, 0),
(615010, 40446, 0, 2, 1, 1, 0, 0, 0),
(615010, 40451, 0, 2, 1, 1, 0, 0, 0),
(615010, 40453, 0, 3, 1, 1, 0, 0, 0),
(615010, 40455, 0, 2, 1, 1, 0, 0, 0),
(615010, 40628, 0, 1, 1, 1, 0, 0, 0),
(615010, 40629, 0, 1, 1, 1, 0, 0, 0),
(615010, 40630, 0, 1, 1, 1, 0, 0, 0),
(615010, 43345, 100, 0, 1, 1, 0, 0, 0),
(615010, 43346, 100, 0, 1, 1, 0, 0, 0),
-- heroic, 1 drake
(615011, 44003, 0, 1, 1, 1, 0, 0, 0),
(615011, 44002, 0, 1, 1, 1, 0, 0, 0),
(615011, 44000, 0, 1, 1, 1, 0, 0, 0),
(615011, 44004, 0, 1, 1, 1, 0, 0, 0),
-- heroic, 2 drakes
(615012, 44007, 0, 1, 1, 1, 0, 0, 0),
(615012, 44006, 0, 1, 1, 1, 0, 0, 0),
(615012, 44005, 0, 1, 1, 1, 0, 0, 0),
(615012, 44008, 0, 1, 1, 1, 0, 0, 0),
(615012, 44011, 0, 1, 1, 1, 0, 0, 0);
UPDATE creature_template SET lootid=entry WHERE entry IN (28860, 288601, 288602, 288603, 31311, 313111, 313112, 313113);
INSERT INTO creature_loot_template (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUES
-- normal, no drakes
(28860, 615000, 100, 0, -615000, 1, 0, 0, 0),
(28860, 45624, 100, 0, 1, 1, 0, 0, 0),
-- normal, 1 drake
(288601, 615000, 100, 0, -615000, 1, 0, 0, 0),
(288601, 615001, 100, 0, -615001, 1, 0, 0, 0),
(288601, 45624, 100, 0, 2, 2, 0, 0, 0),
-- normal, 2 drakes
(288602, 615000, 100, 0, -615000, 1, 0, 0, 0),
(288602, 615001, 100, 0, -615001, 1, 0, 0, 0),
(288602, 615002, 100, 0, -615002, 1, 0, 0, 0),
(288602, 45624, 100, 0, 3, 3, 0, 0, 0),
-- normal, 3 drakes
(288603, 615000, 100, 0, -615000, 1, 0, 0, 0),
(288603, 615001, 100, 0, -615001, 1, 0, 0, 0),
(288603, 615002, 100, 0, -615002, 1, 0, 0, 0),
(288603, 43986, 100, 0, 1, 1, 0, 0, 0),
(288603, 45624, 100, 0, 4, 4, 0, 0, 0),
-- heroic, no drakes
(31311, 615010, 100, 0, -615010, 1, 0, 0, 0),
(31311, 45624, 100, 0, 1, 1, 0, 0, 0),
-- heroic, 1 drake
(313111, 615010, 100, 0, -615010, 1, 0, 0, 0),
(313111, 615011, 100, 0, -615011, 1, 0, 0, 0),
(313111, 45624, 100, 0, 2, 2, 0, 0, 0),
-- heroic, 2 drakes
(313112, 615010, 100, 0, -615010, 1, 0, 0, 0),
(313112, 615011, 100, 0, -615011, 1, 0, 0, 0),
(313112, 615012, 100, 0, -615012, 1, 0, 0, 0),
(313112, 45624, 100, 0, 3, 3, 0, 0, 0),
-- heroic, 3 drakes
(313113, 615010, 100, 0, -615010, 1, 0, 0, 0),
(313113, 615011, 100, 0, -615011, 1, 0, 0, 0),
(313113, 615012, 100, 0, -615012, 1, 0, 0, 0),
(313113, 43954, 100, 0, 1, 1, 0, 0, 0),
(313113, 45624, 100, 0, 4, 4, 0, 0, 0);
