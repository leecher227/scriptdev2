-- #### Quest: Last Rites ####
update creature_template set faction_A=974, faction_H=974, unit_flags=768, type_flags=268435564 where entry=26203;
update creature_template set faction_A=974, faction_H=974, unit_flags=768, type_flags=268435564 where entry=28189;
update creature_template set minhealth=61000, maxhealth=61000 where entry in (28189);
update creature_template set minmana=120000, maxmana=120000 where entry in (28189);
update creature_template set AIName='', Scriptname='npc_thassarian' where entry in (26170);
update creature_template set AIName='', Scriptname='npc_valanar' where entry in (28189);
update creature_template set faction_A=1892, faction_H=1892 where entry=25251;
update creature_template set AIName='' where entry=25250;

-- #### Quest: Word Of Power ####
UPDATE creature_template SET unit_flags=0, faction_A=14, faction_H=14 WHERE entry in (26073, 26076, 25392);
