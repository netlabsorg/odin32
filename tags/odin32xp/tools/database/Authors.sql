-- $Id: Authors.sql,v 1.9 2000-08-02 18:09:24 bird Exp $
--
-- Insert authors.
--
-- Note: these are taken from the assignment list(s).
--
-- USE odin32;

DELETE FROM author;
--                       refcode name                        first,  last                  initials alias        email                           country             city/location                 description
--                                                                                                  (cvs user)
INSERT INTO author VALUES ( 1,  'Sander van Leeuwen',       'Sander', 'van Leeuwen',        'SvL',  'sandervl', 'sandervl@xs4all.nl',           'the Netherlands',  'Delft',                      NULL);
INSERT INTO author VALUES ( 2,  'Peter Fitzsimmons',        'Peter', 'Fitzsimmons',         'PF',   NULL,       'pfitzsim@home.com',            'Canada',           'Mississauga, Ontario',       NULL);
INSERT INTO author VALUES ( 3,  'Patrick Haller',           'Patrick', 'Haller',            'PT',   'phaller',  'phaller@gmx.net',              'Germany',          NULL,                         NULL);
INSERT INTO author VALUES ( 4,  'Joel Troster',             'Joel', 'Troster',              'JT',   'jtroster', 'jtroster@atitech.ca',          'Canada',           'Thornhill, Ontario',         NULL);
INSERT INTO author VALUES ( 5,  'Vince Vielhaber',          'Vince', 'Vielhaber',           'VV',   'vince',    'vev@michvhf.com',              'USA',              'Oxford, MI',                 NULL);
INSERT INTO author VALUES ( 6,  'Chris McKillop',           'Chris', 'McKillop',            'CMK',  NULL,       'cdmckill@engmail.uwaterloo.ca','Canada',           'Waterloo, Ontario',          NULL);
INSERT INTO author VALUES ( 7,  'Felix Maschek',            'Felix', 'Maschek',             'FM',   NULL,       'felix@maschek.com',            'Germany',          'Ammersbek',                  NULL);

INSERT INTO author VALUES ( 8,  'Mark Stead',               'Mark', 'Stead',                'MS',   NULL,       'mar-@unico.com.au',            NULL,               NULL,                         NULL);
INSERT INTO author VALUES ( 9,  'Henk Kelder',              'Henk', 'Kelder',               'HK',   NULL,       'henk.kelde-@capgemini.nl',     NULL,               NULL,                         NULL);
INSERT INTO author VALUES (10,  'Daniela Engert',           'Daniela', 'Engert',            'DE',   'dengert',  'dani@ngrt.de',                 NULL,               NULL,                         NULL);
INSERT INTO author VALUES (11,  'Jens B„ckman',             'Jens', 'B„ckman',              'JB',   NULL,       'os2war-@hem.passagen.se',      NULL,               NULL,                         NULL);
INSERT INTO author VALUES (12,  'Ed Ng',                    'Ed', 'Ng',                     'EG',   NULL,       'en-@hubcap.clemson.edu',       NULL,               NULL,                         NULL);
INSERT INTO author VALUES (13,  'Ken Ames',                 'Ken', 'Ames',                  'KA',   NULL,       'kename-@eathlink.net',         NULL,               NULL,                         NULL);
INSERT INTO author VALUES (14,  'Craig Bradney',            'Craig', 'Bradney',             'CrB',  NULL,       'cbradne-@zip.com.au',          NULL,               NULL,                         NULL);
INSERT INTO author VALUES (15,  'Achim Hasenmueller',       'Achim', 'Hasenmueller',        'AH',   'achimha',  'achimha@innotek.de',           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (16,  'Rene Pronk',               'Rene', 'Pronk',                'RPr',  NULL,       'r.pron-@twi.tudelft.nl',       NULL,               NULL,                         NULL);
INSERT INTO author VALUES (17,  'Christoph Bratschi',       'Christoph', 'Bratschi',        'ChB',  'cbratschi','cbratsch-@datacomm.ch',        NULL,               NULL,                         NULL);
INSERT INTO author VALUES (18,  'Przemyslaw Dobrowolski',   'Przemyslaw ', 'Dobrowolski',   'PD',   NULL,       'dobrawka@asua.org.pl',         NULL,               NULL,                         NULL);
INSERT INTO author VALUES (19,  'Bartosz Tomasik',          'Bartosz', 'Tomasik',           'BT',   NULL,       'bart-@asua.org.pl',            NULL,               NULL,                         NULL);
INSERT INTO author VALUES (20,  'Radu Trimbitas',           'Radu', 'Trimbitas',            'RT',   NULL,       'rad-@rds.ro',                  NULL,               NULL,                         NULL);
INSERT INTO author VALUES (21,  'Erast V. Kunenkov',        'Erast V.', 'Kunenkov',         'ER',   NULL,       'eras-@tips-gw.ips.ac.ru',      NULL,               NULL,                         NULL);
INSERT INTO author VALUES (22,  'Ulrich Muller',            'Ulrich', 'Muller',             'UM',   NULL,       'ulrich.moelle-@rz.hu-berlin.de',NULL,              NULL,                         NULL);
INSERT INTO author VALUES (23,  'Joachim Schneider',        'Joachim', 'Schneider',         'JS',   NULL,       'joaschneide-@debitel.net',     NULL,               NULL,                         NULL);
INSERT INTO author VALUES (24,  'David Raison',             'David', 'Raison',              'DR',   'davidr',   'djr@lemur.co.uk',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (25,  'Robert Pouliot',           'Robert', 'Pouliot',            'RPo',  NULL,       'kryno-@clic.net',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (26,  'Markus Montkowski',        'Markus', 'Montkowski',         'MM',   NULL,       'mmontkowski@gmx.de',           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (27,  'Marty Amodeo',             'Marty', 'Amodeo',              'MA',   NULL,       'mart-@rochester.rr.com',       NULL,               NULL,                         NULL);
INSERT INTO author VALUES (28,  'Josef Chmel',              'Josef', 'Chmel',               'JCh',  NULL,       'chme-@mujweb.cz',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (29,  'Lee Riemenschneider',      'Lee', 'Riemenschneider',       'LR',   NULL,       'lwrieme-@wcic.cioe.com',       NULL,               NULL,                         NULL);
INSERT INTO author VALUES (30,  'Jeppe Cramon',             'Jeppe', 'Cramon',              'JCr',  NULL,       'jepp-@cramon.dk',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (31,  'Edgar Buerkle',            'Edgar', 'Buerkle',             'EB',   'buerkle',  'Edgar.Buerkle@gmx.net',        NULL,               NULL,                         NULL);
INSERT INTO author VALUES (32,  'Yuri Dario',               'Yuri', 'Dario',                'UD',   NULL,       'mc6530@mclink.it',             NULL,               NULL,                         NULL);
INSERT INTO author VALUES (33,  'Adrian Gschwend',          'Adrian', 'Gschwend',           'AG',   NULL,       NULL,                           NULL,               NULL,                         NULL);
-- correct cvs user?
INSERT INTO author VALUES (34,  'Jeroen van den Horn',      'Jeroen', 'van den Horn',       'JvH',  'hugh',     'J.vandenHorn@fibre.a2000.nl',  NULL,               NULL,                         NULL);
INSERT INTO author VALUES (35,  'Bart van Leeuwen',         'Bart', 'van Leeuwen',          'BvL',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (36,  'Nenad Milenkovic',         'Nenad', 'Milenkovic',          'NM',   NULL,       'nenad_milenkovic@softhome.net',NULL,               NULL,                         NULL);
INSERT INTO author VALUES (37,  'Michal Necasek',           'Michal', 'Necasek',            'MN',  'mike',      'mike@mendelu.cz',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (38,  'Vitali E. Pelenyov',       'Vitali E.', 'Pelenyov',        'VEP',  NULL,       'tvv@sbs.kiev.ua',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (39,  'Vit Timchishin',           'Vit', 'Timchishin',            'VT',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (40,  'Jens Wiessner',            'Jens', 'Wiessner',             'JW',   'jens',     'wiessnej@rfi.de',              NULL,               NULL,                         NULL);
INSERT INTO author VALUES (41,  'Christian Langanke',       'Christian', 'Langanke',        'CL',   'cla',      'cbratschi@datacomm.ch',        NULL,               NULL,                         NULL);
INSERT INTO author VALUES (42,  'Marco A. Morales',         'Marco A.', 'Morales',          'MAM',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (43,  'Richard P. Burke',         'Richard P.', 'Burke',          'RPB',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (44,  'TechnicalDirector@...',    '','TechnicalDirector@...',     'TDi', NULL, 'TechnicalDirector@webwombat.com.au', NULL,               NULL,                         NULL);
INSERT INTO author VALUES (45,  'Charles Hunter',           'Charles', 'Hunter',            'CH',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (46,  'Ahti Heinla',              'Ahti', 'Heinla',               'AHe',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (47,  'John P. Baker',            'John P.', 'Baker',              'JPB',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (48,  'Alan Lo',                  'Alan', 'Lo',                   'AL',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (49,  'Andrew Hagen',             'Andrew', 'Hagen',              'AHa',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (50,  'Carsten Tenbrink',         'Carsten', 'Tenbrink',          'CT',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (51,  'Eric Norman',              'Eric', 'Norman',               'EN',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (52,  'Oliver Wilcock',           'Oliver', 'Wilcock',            'OW',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (53,  'omurata@ga2.so-net.ne.jp', '','omurata@ga2.so-net.ne.jp',  'OM',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (54,  'rprice@wdn.com',           '','rprice@wdn.com',            'RPi',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (55,  'Samuel Audet',             'Samuel', 'Audet',              'SA',   NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (56,  'Timothy J. Massey',        'Timothy J.', 'Massey',         'TJM',  NULL,       NULL,                           NULL,               NULL,                         NULL);
INSERT INTO author VALUES (57,  'Abey George',              'Abey', 'George',               'AbB',  NULL,       NULL,                           NULL,               NULL,                         'Wine?');
--INSERT INTO author VALUES (,  '', '', NULL, NULL, NULL,    NULL);
--INSERT INTO author VALUES (,  '', '', NULL, NULL, NULL,    NULL);

INSERT INTO author VALUES (4095,'knut st. osmundsen',       'knut st.', 'osmundsen',        'kso', 'bird',     'knut.stange.osmundsen@mynd.no', 'Norway',           'Oslo',                       NULL);











