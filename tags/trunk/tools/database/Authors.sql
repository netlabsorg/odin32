-- $Id: Authors.sql,v 1.4 2000-07-18 17:56:49 bird Exp $
--
-- Insert authors.
--
-- Note: these are taken from the assignment list(s).
--
USE odin32;

DELETE FROM author;
--                         refcode name               initials alias        email                           country              city/location
--                                                             (cvs user)
INSERT INTO author VALUES ( 1, 'Sander van Leeuwen',    'SvL', 'sandervl', 'sandervl@xs4all.nl',            'the Netherlands',   'Delft');
INSERT INTO author VALUES ( 2, 'Peter Fitzsimmons',     'PF',  NULL,       'pfitzsim@home.com',             'Canada',            'Mississauga, Ontario');
INSERT INTO author VALUES ( 3, 'Patrick Haller',        'PT',  'phaller',  'phaller@gmx.net',               'Germany',           NULL);
INSERT INTO author VALUES ( 4, 'Joel Troster',          'JT',  'jtroster', 'jtroster@atitech.ca',           'Canada',            'Thornhill, Ontario');
INSERT INTO author VALUES ( 5, 'Vince Vielhaber',       'VV',  'vince',    'vev@michvhf.com',               'USA',               'Oxford, MI');
INSERT INTO author VALUES ( 6, 'Chris McKillop',        'CMK', NULL,       'cdmckill@engmail.uwaterloo.ca', 'Canada',            'Waterloo, Ontario');
INSERT INTO author VALUES ( 7, 'Felix Maschek',         'FM',  NULL,       'felix@maschek.com',             'Germany',           'Ammersbek');

INSERT INTO author VALUES ( 8, 'Mark Stead',            'MS',  NULL,       'mar-@unico.com.au',             NULL,                NULL);
INSERT INTO author VALUES ( 9, 'Henk Kelder',           'HK',  NULL,       'henk.kelde-@capgemini.nl',      NULL,                NULL);
INSERT INTO author VALUES (10, 'Daniela Engert',        'DE',  'dengert',  'dani@ngrt.de',                  NULL,                NULL);
INSERT INTO author VALUES (11, 'Jens B„ckman',          'JB',  NULL,       'os2war-@hem.passagen.se',       NULL,                NULL);
INSERT INTO author VALUES (12, 'Ed Ng',                 'EG',  NULL,       'en-@hubcap.clemson.edu',        NULL,                NULL);
INSERT INTO author VALUES (13, 'Ken Ames',              'KA',  NULL,       'kename-@eathlink.net',          NULL,                NULL);
INSERT INTO author VALUES (14, 'Craig Bradney',         'CrB', NULL,       'cbradne-@zip.com.au',           NULL,                NULL);
INSERT INTO author VALUES (15, 'Achim Hasenmueller',    'AH',  'achimha',  'achimha@innotek.de',            NULL,                NULL);
INSERT INTO author VALUES (16, 'Rene Pronk',            'RPr', NULL,       'r.pron-@twi.tudelft.nl',        NULL,                NULL);
INSERT INTO author VALUES (17, 'Christoph Bratschi',    'ChB', 'cbratschi','cbratsch-@datacomm.ch',         NULL,                NULL);
INSERT INTO author VALUES (18, 'Przemyslaw Dobrowolski','PD',  NULL,       'dobrawka@asua.org.pl',          NULL,                NULL);
INSERT INTO author VALUES (19, 'Bartosz Tomasik',       'BT',  NULL,       'bart-@asua.org.pl',             NULL,                NULL);
INSERT INTO author VALUES (20, 'Radu Trimbitas',        'RT',  NULL,       'rad-@rds.ro',                   NULL,                NULL);
INSERT INTO author VALUES (21, 'Erast V. Kunenkov',     'ER',  NULL,       'eras-@tips-gw.ips.ac.ru',       NULL,                NULL);
INSERT INTO author VALUES (22, 'Ulrich Muller',         'UM',  NULL,       'ulrich.moelle-@rz.hu-berlin.de',NULL,               NULL);
INSERT INTO author VALUES (23, 'Joachim Schneider',     'JS',  NULL,       'joaschneide-@debitel.net',      NULL,                NULL);
INSERT INTO author VALUES (24, 'David Raison',          'DR',  'davidr',   'djr@lemur.co.uk',               NULL,                NULL);
INSERT INTO author VALUES (25, 'Robert Pouliot',        'RPo', NULL,       'kryno-@clic.net',               NULL,                NULL);
INSERT INTO author VALUES (26, 'Markus Montkowski',     'MM',  NULL,       'mmontkowski@gmx.de',            NULL,                NULL);
INSERT INTO author VALUES (27, 'Marty ??',              'M??', NULL,       'mart-@rochester.rr.com',        NULL,                NULL);
INSERT INTO author VALUES (28, 'Josef Chmel',           'JCh', NULL,       'chme-@mujweb.cz',               NULL,                NULL);
INSERT INTO author VALUES (29, 'Lee Riemenschneider',   'LR',  NULL,       'lwrieme-@wcic.cioe.com',        NULL,                NULL);
INSERT INTO author VALUES (30, 'Jeppe Cramon',          'JCr', NULL,       'jepp-@cramon.dk',               NULL,                NULL);
INSERT INTO author VALUES (31, 'Edgar Buerkle',         'EB',  'buerkle',  'Edgar.Buerkle@gmx.net',         NULL,                NULL);
INSERT INTO author VALUES (32, 'Yuri Dario',            'UD',  NULL,       'mc6530@mclink.it',              NULL,                NULL);
INSERT INTO author VALUES (33, 'Adrian Gschwend',       'AG',  NULL,       NULL,                            NULL,                NULL);
-- correct cvs user?
INSERT INTO author VALUES (34, 'Jeroen van den Horn',   'JvH', 'hugh',     'J.vandenHorn@fibre.a2000.nl',   NULL,                NULL);
INSERT INTO author VALUES (35, 'Bart van Leeuwen',      'BvL', NULL,       NULL,                            NULL,                NULL);
INSERT INTO author VALUES (36, 'Nenad Milenkovic',      'NM',  NULL,       NULL,                            NULL,                NULL);
INSERT INTO author VALUES (37, 'Michal Necasek',        'MN', 'mike',      'mike@mendelu.cz',               NULL,                NULL);
INSERT INTO author VALUES (38, 'Vitali E. Pelenyov',    'VEP', NULL,       'tvv@sbs.kiev.ua',               NULL,                NULL);
INSERT INTO author VALUES (39, 'Vit Timchishin',        'VT',  NULL,       NULL,                            NULL,                NULL);
INSERT INTO author VALUES (40, 'Jens Wiessner',         'JW',  'jens',     'wiessnej@rfi.de',               NULL,                NULL);
INSERT INTO author VALUES (41, 'Christian Langanke',    'CL',  'cla',      'cbratschi@datacomm.ch',         NULL,                NULL);
INSERT INTO author VALUES (4095,'knut st. osmundsen',   'kso', 'bird',     'knut.stange.osmundsen@mynd.no', 'Norway',            'Oslo');











