-- $Id: Codes.sql,v 1.3 2001-09-07 10:31:43 bird Exp $
--
-- Codes.
--


--
-- Dll type codes.
--
--       codegroup   code   description
INSERT INTO code
    VALUES ('DTYP', 'A   ', 'Odin32 API DLL');
INSERT INTO code
    VALUES ('DTYP', 'I   ', 'Internal Odin32 DLL');
INSERT INTO code
    VALUES ('DTYP', 'S   ', 'Support stuff (ie. pe.exe and win32k.sys)');
INSERT INTO code
    VALUES ('DTYP', 'T   ', 'Tools (executables and DLLs)');


--
-- Function type codes.
--
--       codegroup   code   description
INSERT INTO code
    VALUES ('FTYP', 'A   ', 'Odin32 API');
INSERT INTO code
    VALUES ('FTYP', 'I   ', 'Internal Odin32 API');
INSERT INTO code
    VALUES ('FTYP', 'O   ', 'Other function');


