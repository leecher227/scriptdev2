-- ##### Quest: Betreyal ######
UPDATE creature_template SET maxhealth = '550000', minhealth = '550000', unit_flags = '0', ScriptName = "npc_overlord_drakuru" WHERE entry = 28503;
update `creature_template` set `faction_A`='974', `faction_H`='974', `unit_flags`='768', `type_flags`='268435564' where `entry`=28498;
UPDATE creature_template SET flags_extra=flags_extra|2 WHERE entry = 28931;

UPDATE `item_template` set spellid_1 = '53102', ScriptName = '' where entry = 39664;
update `creature_template` set spell1='54136', spell2='54132', spell3='54135', `unit_flags`='0', `type_flags`='0' where `entry`=28931;

REPLACE INTO `spell_script_target` (`entry`, `type`, `targetEntry`) values('54250','1','28492');
REPLACE INTO `spell_script_target` (`entry`, `type`, `targetEntry`) values('54209','1','28492');
REPLACE INTO `spell_script_target` (`entry`, `type`, `targetEntry`) values('54236','1','28503');

-- ##### Quest: Leave No One Behind #####
UPDATE quest_template SET `ReqSpellCast1`='0', `ReqSpellCast2`='0', `ReqSpellCast3`='0' WHERE entry = 12512;
update `creature` set `DeathState`='0' where id in (28143, 28141, 28133);
update `creature_template` set `AIName`='', `ScriptName`='npc_rescue_knights' where entry in (28143,28141,28133, 28148,28142,28136);
