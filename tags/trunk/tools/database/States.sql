-- $Id: States.sql,v 1.1 1999-09-05 02:53:05 bird Exp $
--
-- Insert states into the state table.
--
USE odin32;

DELETE FROM state;

INSERT INTO state
    VALUES (0, '#000080', 'Not Implemented', 'Not implemented.');
INSERT INTO state
    VALUES (1, '#0000FF', 'Stub', 'Stub-function. Does nothing.');
INSERT INTO state
    VALUES (2, '#808000', 'Partially', 'Partially implemented. Not Tested.');
INSERT INTO state
    VALUES (3, '#FFFF00', 'Partially Tested', 'Partially implemented. Tested and approved.');
INSERT INTO state
    VALUES (4, '#008000', 'Completely', 'Completely implemented. Not tested.');
INSERT INTO state
    VALUES (5, '#00FF00', 'Completely Tested', 'Completely implemented. Tested and approved.');
INSERT INTO state
    VALUES (99, '#FF0000', 'Unknown', 'The state of this function is unknown.');


