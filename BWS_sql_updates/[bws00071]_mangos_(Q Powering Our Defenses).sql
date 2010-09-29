-- Quest: Powering Our Defenses (8490)

UPDATE creature_template SET ScriptName='npc_infused_crystal' WHERE entry = 16364;

-- Enraged Wraith
DELETE FROM creature WHERE id = 17086;
UPDATE creature_template SET flags_extra=flags_extra|64 WHERE entry=17086;
