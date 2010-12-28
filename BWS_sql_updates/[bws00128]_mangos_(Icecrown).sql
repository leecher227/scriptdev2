-- #### QUEST: Orbaz Fate ####
UPDATE creature_template SET `equipment_id`='365',`minlevel`='83', `maxlevel`='83', `minhealth`='11156000', `maxhealth`='11156000', `armor`='10627', `faction_A`='35', `faction_H`='35', `npcflag`='0', `mindmg`='120', `maxdmg`='240', `attackpower`='163', `dmg_multiplier`='35', `baseattacktime`='2000' WHERE entry = 31290;
UPDATE creature_template SET `AIName`='', ScriptName = "npc_orbaz_bloodbane" WHERE entry = 31283;
UPDATE creature_template SET `AIName`='', ScriptName = "npc_darion_mograine_fb" WHERE entry = 31290;
