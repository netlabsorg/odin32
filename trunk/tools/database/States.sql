-- $Id: States.sql,v 1.4 2000-07-21 03:43:59 bird Exp $
--
-- Insert states into the state table.
--
USE odin32;

DELETE FROM state;

--      refcode color    weight  name                       description
INSERT INTO state
    VALUES (0, '#000000',   0,  'Not Implemented',          'Not implemented.');
INSERT INTO state
    VALUES (1, '#0010E0',   5,  'Stub',                     'Stub-function. Does nothing.');
INSERT INTO state
    VALUES (2, '#afaf00',  20,  'Partially',                'Partially implemented. Not Tested.');
INSERT INTO state
    VALUES (3, '#e0e000',  25,  'Partially Tested',         'Partially implemented. Tested and approved.');
INSERT INTO state
    VALUES (4, '#00cc00',  90,  'Completely',               'Completely implemented. Not tested.');
INSERT INTO state
    VALUES (5, '#00ff00', 100,  'Completely Tested',        'Completely implemented. Tested and approved.');
INSERT INTO state
    VALUES (6, '#50afaf',  20,  'Open32 Partially',         'Partially implemented. Not Tested.');
INSERT INTO state
    VALUES (7, '#70e0e0',  25,  'Open32 Partially Tested',  'Partially implemented. Tested and approved.');
INSERT INTO state
    VALUES (8, '#7090d8',  90,  'Open32 Completely',        'Completely implemented. Not tested.');
INSERT INTO state
    VALUES (9, '#afbfff', 100,  'Open32 Completely Tested', 'Completely implemented. Tested and approved.');
INSERT INTO state
    VALUES (99, '#aa0000', 25,  'Unknown',                  'The state of this function is unknown.');


