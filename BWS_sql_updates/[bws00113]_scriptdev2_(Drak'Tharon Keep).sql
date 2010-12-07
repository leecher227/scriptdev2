DELETE FROM script_texts WHERE entry in (-1600020, -1600021);
INSERT INTO script_texts (`entry`,`content_default`,`sound`,`type`,`language`,`comment`) values
('-1600020','King Dred raises his talon menacingly!','0','5','0','King Dred Talon'),
('-1600021','King Dred calls for a raptor!','0','5','0','King Dred Call for Raptor');

-- Drakuru Event
DELETE FROM `script_texts` WHERE `comment` = 28016;
INSERT INTO `script_texts` (`entry`,`content_default`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
# Pre Ritual
(-1568270, 'Ahhh, Back in Draktharon at last!', '����, ������� � ����� � ����������!', 14006,1,0,4, '28016'),
(-1568271, 'Ya done well, mon....', '�������...', 14007,0,0,0, '28016'),
(-1568272, 'Ya surpassed my greatest hopes.', '� ���� �� ������ �� ��� ������!', 14008,0,0,0, '28016'),
(-1568273, 'For your efforts, you be gettin\' da greatest of rewards....', '�� ���� �������� ��� ���� ���������� �� ������!', 14009,0,0,0, '28016'),
(-1568274, 'Revelation!', '����������!', 14010,0,0,20, '28016'),

# Lich king Talk
(-1568275, 'Be still, friend, and behold da truth of things!', '�������! � ����� ���� ��������� ������!', 14011,0,0,0, '28016'),
(-1568276, 'Master, my mission be complete.', '��������, ���� ������� ���������!', 14012,0,0,0, '28016'),
(-1568277, 'With da help of these mortals, dis region been cleansed of all who oppose us.', '� ������� ���� �������� �� �������� ��� ���������, ��� ����, ��� ��� �������������!', 14013,0,0,0, '28016'),
(-1568278, 'You have done well, Drakuru.', '�� ��������� �������� �������.', 14749,0,0,1, '28016'),
(-1568279, 'Your betrayal of the Drakkari Empire has borne me a new army.', '���� ������������� ������� ��������, �������� ��� ����� �����...', 14750,0,0,1, '28016'),
(-1568280, 'Your army....', '���� �����!', 14751,0,0,0, '28016'),
(-1568281, 'Arise, and accept my gift.', '��������, � ����� ��� ���!', 14752,0,0,1, '28016'),
(-1568282, 'I charge you now with the cleansing of Zul\'Drak, Drakuru.', '������ �� ��������� �� �������� ��������, �������!', 14753,0,0,25, '28016'),
(-1568283, 'As you wish, master.', '��� ���������, ��������.', 14014,0,0,1, '28016'),
(-1568284, 'Fail me and it shall be your undoing...', '�� ����������, � � ������� ����.', 14754,0,0,1, '28016'),
(-1568285, 'Succeed, and even greater power shall be yours!', '����������, � �������� ��� ������ ����.', 14755,0,0,1, '28016'),
(-1568286, 'Forgive my deception, mon. It all been for your own good.', '�������� ��� ���� ����� ������, �� ��� ��� ������ �� �����.', 14015,0,0,1, '28016'),
(-1568287, 'Ya needed ta prove yer worthiness....', '��� ���� ���� ���������, ��� ��� ����� ��������.', 14016,0,0,1, '28016'),
(-1568288, 'I be needin\' worthy underlings in Zul\'Drak, mon. Ones ready to wield real power.', '��� ����� ����� �������� ����� � ��������, ����� ������� ��������� ����������������� ������.', 14017,0,0,1, '28016'),
(-1568289, 'You see for yourself now. Da Lich King be generous to dem who obey.', '������ �� ������? ������-��� ����� ���������� ���, ��� �������� ���.', 14018,0,0,1, '28016'),
(-1568290, 'Follow dis path, mon, and soon you be tastin\' immortality!', '������� ��� � ����� �� ������� ����������.', 14019,0,0,1, '28016');
