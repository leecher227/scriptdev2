-- Support of quest: The Totem of Kar'dash (9879)

DELETE FROM script_waypoint WHERE entry = 18209;
INSERT INTO script_waypoint
(entry, pointid, location_x, location_y, location_z, waittime, point_comment)
VALUES
('18209', '1', '-1518.593872', '8469.956055', '-4.101756', '0', NULL),
('18209', '2', '-1511.784546', '8480.531250', '-4,033919', '0', 'first combat'),
('18209', '3', '-1513.447754', '8509.764698', '-0.109888', '0', NULL),
('18209', '4', '-1498.857788', '8508.805664', '0.844676', '0', NULL),
('18209', '5', '-1454.215088', '8495.808594', '4.912439', '0', 'second combat'),
('18209', '6', '-1437.483521', '8504.165039', '6.879123', '0', NULL),
('18209', '7', '-1387.885680', '8539.817383', '11.075749', '30000', 'quest complete');