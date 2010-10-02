-- Training Dummy don't die and combat movement.
DELETE FROM creature_ai_scripts WHERE (creature_id=31146);
INSERT INTO creature_ai_scripts VALUES 
(3114601, 31146, 1, 0, 100, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - don''t die'),
(3114602, 31146, 0, 0, 100, 1, 120000, 120000, 120000, 120000, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - evade'),
(3114603, 31146, 4, 0, 100, 0, 0, 0, 0, 0, 20, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - disable autoattack and combat movement'),
(3114604, 31146, 0, 0, 100, 1, 5000, 5000, 5000, 5000, 11, 43978, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - cast Lay on Hands');
UPDATE creature_template SET AIName = 'EventAI' WHERE entry = 31146;

DELETE FROM creature_ai_scripts WHERE creature_id = 32547;
INSERT INTO creature_ai_scripts VALUES 
(3254701, 32547, 1, 0, 100, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - don''t die'),
(3254702, 32547, 0, 0, 100, 1, 120000, 120000, 120000, 120000, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - evade'),
(3254703, 32547, 4, 0, 100, 0, 0, 0, 0, 0, 20, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - disable autoattack and combat movement'),
(3254704, 32547, 0, 0, 100, 1, 5000, 5000, 5000, 5000, 11, 43978, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Training Dummy - cast Lay on Hands');
UPDATE creature_template SET AIName = 'EventAI', unit_flags = unit_flags|131072, mechanic_immune_mask = 12656656 WHERE entry = 32547;