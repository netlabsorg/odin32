-- $Id: States.sql,v 1.3 2000-07-19 21:29:36 bird Exp $
--
-- Insert states into the state table.
--
USE odin32;

DELETE FROM state;

--      refcode color    weight  name                       description
INSERT INTO state
    VALUES (0, '#000080',   0,  'Not Implemented',          'Not implemented.');
INSERT INTO state
    VALUES (1, '#0000FF',   5,  'Stub',                     'Stub-function. Does nothing.');
INSERT INTO state
    VALUES (2, '#808000',  20,  'Partially',                'Partially implemented. Not Tested.');
INSERT INTO state
    VALUES (3, '#FFFF00',  25,  'Partially Tested',         'Partially implemented. Tested and approved.');
INSERT INTO state
    VALUES (4, '#008000',  90,  'Completely',               'Completely implemented. Not tested.');
INSERT INTO state
    VALUES (5, '#00FF00', 100,  'Completely Tested',        'Completely implemented. Tested and approved.');
INSERT INTO state
    VALUES (6, '#808080',  20,  'Open32 Partially',         'Partially implemented. Not Tested.');
INSERT INTO state
    VALUES (7, '#FFFF80',  25,  'Open32 Partially Tested',  'Partially implemented. Tested and approved.');
INSERT INTO state
    VALUES (8, '#008080',  90,  'Open32 Completely',        'Completely implemented. Not tested.');
INSERT INTO state
    VALUES (9, '#00FF80', 100,  'Open32 Completely Tested', 'Completely implemented. Tested and approved.');
INSERT INTO state
    VALUES (99, '#FF0000', 25,  'Unknown',                  'The state of this function is unknown.');


