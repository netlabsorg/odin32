-- $Id: States.sql,v 1.5 2000-08-02 01:01:36 bird Exp $
--
-- Insert states into the state table.
--
--USE odin32;

DELETE FROM state;

--      refcode color    weight  name                       description
INSERT INTO state
    VALUES (0, '#000000',   0,  'Not Implemented',
'Used only in the API Database.<br>\n(You can''t have a function which is
 Not Implemented in the source, it will then become a Stub.)'
);

INSERT INTO state
    VALUES (1, '#0010E0',   5,  'Stub',
'Stub-function. A function which does nothing.<br>\n
It logs the call (optional) and returns.<p>\n
<tt>Example:</tt> AreFileApisANSI() logs that the function is called and then it returns TRUE.'
);

INSERT INTO state
    VALUES (2, '#afaf00',  45,  'Partially',
'Partially implemented but <tt>not</tt> tested.<br>\n
The entire functionality of the API is not implemented.<p>\n
<tt>Example:</tt> If GetModuleFileName didn''t work for System Dlls, like
Kernel32, User32, GDI32,..., it is only partially implemented.'
);

INSERT INTO state
    VALUES (3, '#e0e000',  50,  'Partially Tested',
'Partially implemented, tested and approved.<br>
The implemented part of the API is tested and approved by someone. (Ideally
this someone is not the one who wrote the code... But do as you like.)'
);

INSERT INTO state
    VALUES (4, '#00cc00',  90,  'Completely',
'Completely implemented but not tested.<br>\n
You have completed the implementation. It behaves like the original Win32 API.
(At least you believe this is the case, though tests may tell something else...)'
);

INSERT INTO state
    VALUES (5, '#00ff00', 100,  'Completely Tested',
'Completely implemented, tested and approved.<br>\n
Someone have actually verified that this API behaves like the original Win32 API.
(Ideally this someone is not the one who wrote the code... But do as you like.)'
);

INSERT INTO state
    VALUES (6, '#50afaf',  45,  'Open32 Partially',
'Wrapping an Open32 call. Partially implemented and not tested.<br>\n
The entire functionality of the API is not implemented.'
);

INSERT INTO state
    VALUES (7, '#70e0e0',  50,  'Open32 Partially Tested',
'Wrapping an Open32 call. Partially implemented, tested and approved.<br>\n
The implemented part of the API is tested and approved by someone.'
);

INSERT INTO state
    VALUES (8, '#7090d8',  90,  'Open32 Completely',
'Wrapping an Open32 call. Completely implemented, but not tested.<br>\n
You have completed the implementation. It behaves like the original Win32 API.
(At least you believe this is the case, though tests may tell something else...)'
);

INSERT INTO state
    VALUES (9, '#afbfff', 100,  'Open32 Completely Tested',
'Wrapping an Open32 call. Completely implemented, tested and approved.
Someone have actually verified that this API behaves like the original Win32 API.'
);

INSERT INTO state
    VALUES (99,'#aa0000',  25,  'Unknown',
'Used only by the Odin32 API Database update utility when it cannot determinate the
function state.<br>
This API requires attention, check the logs!'
);

