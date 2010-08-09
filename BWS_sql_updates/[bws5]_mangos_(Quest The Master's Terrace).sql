-- event script for Medivh's Journal
delete from event_scripts where id = 10951;
insert into event_scripts values 
(10951, 0, 10, 17651, 180000, '', -11159, -1907.22, 91.48, 0);

-- add script name
update creature_template set ScriptName = 'npc_image_of_medivh' where entry = 17651;
