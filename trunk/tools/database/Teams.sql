-- $Id: Teams.sql,v 1.5 2000-08-02 01:01:36 bird Exp $
--
-- Team data.
--
-- IMPORTANT! Before you load this file, you'll have to import dlls and apis.
-- This is done by issuing a 'nmake apiimport' from /src. (Database utils
-- will have to be compiled first of course.)
--


DELETE FROM team;
DELETE FROM tgroup;
DELETE FROM tgroupmember;
DELETE FROM tgroupdll;
DELETE FROM tgroupapigroup;


--
-- MAD team
--
INSERT INTO team(refcode,
                 name,
                 longname,
                 description,
                 tasks,
                 notes)
VALUES (
1,
'MAD',
'Management, Administration, Decision-making'
,
'The MAD Team consists of the core developers of the old Win32-OS/2 project
team and initiators of it\'s current incarnation, the open-source based
Project Odin.'
,
'<li>Final decisions about project directions\n
<li>Packaging and releasing of official distributions\n
<li>Dealing with the IBM and other software vendors\n
<li>Offical contacts regarding the project\n'
,
'MAD Team decisions are backed by competence and authority, which is based
on amount of work and contribution to the project so far. Since Odin is
open-source based, no developer or project member should consider
himself/herself constrained with MAD Team decisions since they apply only to
official development tree and you can still use the source and take another
development direction if you feel that it\'s better. After all, everyone can
prove his point WITH strongest of all arguments: code that works!'
);


INSERT INTO tgroup(refcode, team,    name, description)
    VALUES        (   1000,    1, 'dummy',     'dummy');

INSERT INTO tgroupmember(tgroup, author)
    SELECT 1000, refcode FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Achim Hasenmueller',
                        'Peter Fitzsimmons',
                        'Vince Vielhaber',
                        'Patrick Haller',
                        'Christian Langanke',
                        'Adrian Gschwend',
                        'Nenad Milenkovic',
                        'knut st. osmundsen'
                       );



--
-- PEL team
--
INSERT INTO team(refcode,
                 name,
                 longname,
                 description,
                 tasks,
                 notes)
VALUES (
2,
'PEL',
'Portable Executable Loader'
,
'Implementing binary compatibility for Win32 executables in the OS/2
environment (PE loader).'
,
'<li>Kernel hooks that allow OS/2 to recognize PE binaries as it\'s own.\n
<li>Converting PE objects/resources into LX format\n
<li>Adapting Win32 executables to the OS/2 memorymodel during loading\n'
,
'Currently working loader is based on Knut\'s
driver (<tt>WIN32K.SYS</tt>) that hooks into
kernel and extends some kernel functions, but it
suffers from \'well-known\' limitations that
result from different memory models in OS/2 and
Win32 environments so only limited number of
programs will work, even if all API functions
they needed are there.
<p>\n
Sander has developed a new Ring 3 loader
that overcomes some of those memory model related issues.
However, there is no way to overcome the per-process memory
limitation, so for Win32 programs that load or allocate RAM
over 512MB you will <b>require</b> OS/2 versions
that don\'t have that limit, that is either
WarpServer 4 SMP (with 3GB limit) or latest
WarpServer for e-business, aka Aurora (with 4GB
per-process limit). Since the Win32 environment has
a 2GB per-process limitation, either one should be
sufficient for demanding applications. This new
loader will be integrated with <tt>WIN32K.SYS</tt>
later.<p>\n'
);


INSERT INTO tgroup(refcode, team,    name, description)
    VALUES        (   2000,    2, 'dummy',     'dummy');

INSERT INTO tgroupmember(tgroup, author)
    SELECT 2000, refcode FROM author WHERE name = 'Sander van Leeuwen';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 2000, refcode FROM author WHERE name = 'Mark Stead';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 2000, refcode FROM author WHERE name = 'Daniela Engert';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 2000, refcode FROM author WHERE name = 'Henk Kelder';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 2000, refcode FROM author WHERE name = 'knut st. osmundsen';



--'
-- WAI team
--
INSERT INTO team(refcode,
                 name,
                 longname,
                 description,
                 tasks,
                 notes)
VALUES (
3,
'WAI',
'Win32 API Implementation'
,

'WAI Team is implementing the Odin32, API that aims to clone Win32
functionality, behaviour, bugs, etc. Odin32 is, actually, the Win32
implementation on OS/2.
<p>\n
This is the largest team, since the Win32 API is a moving target and has
many functions. The WAI Team consists of several groups that are
working on specific sets of API functions, usualy separated and
implemented in separate DLL files.\n'
,
NULL
,
'Due to the rich set of options in OS/2 various methods
are used for implementing various API sets. Some are
implemented from scratch, others are simply mapped to
Open32, some are implemented with help of Open32,
others are ported from WINE, some are partly based on
WINE code etc. The goal was to use the best possible
solution in each specific case, that is: one that will be the
easiest to implement, will have best possible
performance, will intergrate with OS/2 as much as
possible, etc. You may think that the wrong decision is
chosen in some case: feel free to suggest or implement
another one. If it proves good, it will be included in the
project.\n'
);


-- Kernel32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3001,    3, 'KERNEL32'
,'Implements functions that are found in <tt>KERNEL32.DLL</tt> library, that
is file I/O, console and other non-PEL related functions (PEL Team will be
responsible for some parts of <tt>KERNEL32.DLL</tt> that are related with
loading programs and resources into memory).'
,
'<li>Sander, Knut, Patrick and Peter did those in old project\n
<li>Console API functions are completed (Patrick Haller was responsible
for the console APIs in the old project)\n'
);
INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3001, refcode, 'Y' FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Knut St. Osmundsen',
                        'Patrick Haller',
                        'Achim Hasenmueller'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3001, refcode FROM author
        WHERE name IN (
                        'Peter Fitzsimmons',
                        'Mark Stead',
                        'Daniela Engert',
                        'Edgar Buerkle',
                        'Jens B„ckman',
                        'Charles Hunter',
                        'Ed Ng',
                        'Ken Ames',
                        'Craig Bradney'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3001, refcode FROM dll WHERE name = 'kernel32';


-- User32 & Gdi32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3002,    3, 'USER32 & GDI32'
,'Implements basic windowing API functions, for example creating windows,
populating menus, poping up dialogs, displaying GUI controls, using fonts,
presenting graphics (GDI) in windows, etc.'
,
'<li>Sander did those in old project, Patrick worked on GDI32\n
<li>Current code heavily depends on Open32\n
<li>GUI controls that are not available in OS/2 should be ported from WINE to PM (or designed from scratch)\n
<li>Might be good idea to separate this from Open32 to avoid certain limitations\n'
);
INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3002, refcode, 'Y' FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Knut St. Osmundsen',
                        'Patrick Haller',
                        'Achim Hasenmueller'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3002, refcode FROM author
        WHERE name IN (
                        'Daniela Engert',
                        'Edgar Buerkle',
                        'Charles Hunter',
                        'Christoph Bratschi',
                        'Rene Pronk',
                        'Marty Amodeo',
                        'Henk Kelder'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3002, refcode FROM dll WHERE name IN ('USER32', 'GDI32');


-- ComDlg32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3003,    3, 'COMDLG32'
,'Mapping to OS/2 counterparts common Windows dialogs (file open/save.
font/color selection, standard print dialog, etc.) and cloning the \'look & feel\'
of Win32 \'advanced\' dialogs that are not available in OS/2.'
,
'<li>Open32 does basic (\'good enough\') job here, but for full functionality,
it must be extended and later replaced with advanced Win32 dialogs.\n
<li>Win32 file dialogs from WINE are ported (and fixed), but are not
enabled yet.\n
<li>We will try to replace standard OS/2\'s File Open/Save dialog with this
advanced Win-like version, so other programs can benefit, too.\n'
);
--'
INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3003, refcode, 'Y' FROM author
        WHERE name IN (
                        'Achim Hasenmueller',
                        'Christoph Bratschi'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3003, refcode FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Przemyslaw Dobrowolski',
                        'Radu Trimbitas'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3003, refcode FROM dll WHERE name IN ('COMDLG32');



-- Shell32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3004,    3, 'SHELL32'
,'Support for drag & drop, GUI and shell extensions, loading of resources,
etc., Important for many applications and other API groups. Integration of
Win32 drag & drop into PM/WPS model.'
,
'<li>Patrick Haller did those in original project\n
<li>This set of API functions is important for many applications and other API groups\n
<li>WINE stuff may be used for this\n
<li>Plan is to provide real OS/2 WPS integration of Win32 applications via the SHELL32 interface\n
<li>SHELL32 is largely undocumented\n'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3004, refcode, 'Y' FROM author
        WHERE name IN (
                        'Patrick Haller',
                        'Achim Hasenmueller'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3004, refcode FROM author
        WHERE name IN (
                        'Charles Hunter',
                        'Christoph Bratschi',
                        'Lee Riemenschneider'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3004, refcode FROM dll WHERE name IN ('SHELL32');


-- ComCtl32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3005,    3, 'COMCTL32'
,'Realizing common controls widely used by programs (buttons, toolbars, tabbed
dialogs, etc.) that are implemented in <tt>COMCTL32.DLL</tt> (which is not
part of basic Win32 API, but necessary for most applications).'
,
'<li>COMCTL32 is not part of the basic Win32 API, but is necessary for most applications
<li>It\'s based on other APIs, should be ported from WINE
<li>not-existing in current code, original <tt>COMCTL32.DLL</tt> might even
work if sufficient number of necessary base API is implemented, but
implementing it would provide better performance and give us more control over
bugs or undocumented features'
);
--'
INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3005, refcode, 'Y' FROM author
        WHERE name IN (
                        'Achim Hasenmueller',
                        'Christoph Bratschi'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3005, refcode FROM author
        WHERE name IN (
                        'Erast V. Kunenkov',
                        'Przemyslaw Dobrowolski',
                        'Ulrich Muller',
                        'Charles Hunter',
                        'Radu Trimbitas',
                        'Joachim Schneider'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3005, refcode FROM dll WHERE name IN ('COMCTL32');



-- WinMM
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3006,    3, 'WINMM'
,'Implementing Windows multimedia API functions (Wave audio, MIDI music, video
output) and support (codecs).'
,
'<li>Sander van Leeuwen did wave out part in old project\n
<li>Joel Troster was responsible for WINMM in old project (did MIDI)\n
<li>Wave should be done through DART, MIDI with RTMIDI, DIVE should be quite
    sufficient for video output\n
<li>What about codecs? MainConcept used to offer some free OS/2 codecs before,
    but some should be implemented, probably as native OS/2 codecs\n
<li>WINE probably totaly useless here because of different sound models\n'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3006, refcode, 'Y' FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Patrick Haller'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3006, refcode FROM author
        WHERE name IN (
                        'Joel Troster',
                        'Josef Chmel',
                        'Ahti Heinla'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3006, refcode FROM dll WHERE name IN ('WINMM');


-- Direct*
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3007,    3, 'DirectX'
,'Implementing DirectDraw, DirectSound, DirectInput, Direct3D and other
\'high-performance\' multimedia APIs should also take care about OpenGL).'
,
'<li>Sander did DirectDraw in old project, plus much additional work by Markus
     which never made to the project.\n
<li>Sander did skeleton for Direct3D API, and DirectSound is only stubs.\n
<li>Wine uses Mesa (Open GL compatible library) for Direct3D, so code from
    Wine could be used with OS/2\'s native OpenGL. However, due to \'low-level\'
    nature of Direct3D, implementgin it via \'high-level\' API such as OpenGL
    (even with hardware acceleration) would be unefficient.\n
<li>SciTech Software will provide Direct3D compatible \'vectors\' in the next
    version of their OS/2 drivers. Kendall Bennett FROM SciTech also offered
    full specification of their Nucleus driver architecture to the team, so
    those working on Direct3D can utilize it\'s features for real and directly
    accelerated Direct3D in OS/2.\n
<li>DirectX v3 (first usable version) functionality should be set as mid-goal,
    many popular games (non-3D) will be happy with it, for example the most
    popular game of 1998, StarCraft.\n'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3007, refcode, 'Y' FROM author
        WHERE name IN (
                        'Markus Montkowski '
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3007, refcode FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Marty Amodeo',
                        'Ahti Heinla'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3007, refcode FROM dll WHERE name LIKE 'D%';
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3007, refcode FROM dll WHERE name LIKE 'OPENGL%';


-- Winsock
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3008,    3, 'Winsock'
,'To implement networking support (TCP/IP and NetBIOS primarily) via
<tt>WSOCK32.DLL</tt>, <tt>NETAPI32.DLL</tt> and <tt>MPR.DLL</tt>'
,
'<li>Vince Vielhaber did Winsock part in old project (practicaly completed
    Winsock 1.1 support).\n
<li>Winsock should be priority, NetBIOS support maybe not so important.\n
<li>Netbios is now part of Winsock 2.0, so implementing it would suffice
    most new netbios applications that most probably use Winsock v2. And
    for old applications...?\n
<li>NetBIOS in OS/2 and Windows is pretty much comparable when it comes
    to features, but MS changed a lot few years ago, and 32-bit API they
    introduced is quite different compared to Net* calls in OS/2\n
<li>Some security related NetBIOS APIs might be impossible to do because
    of lack of documentation.\n'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3008, refcode, 'Y' FROM author
        WHERE name IN (
                        'Patrick Haller'
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3008, refcode FROM author
        WHERE name IN (
                        'Vince Vielhaber'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3008, refcode FROM dll WHERE name IN ('WSOCK32', 'NETAPI32', 'MPR');



-- Printing
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3009,    3, 'Printing'
,'Goal of this group is to implement printing via <tt>WINSPOOL.DLL</tt>.'
,
'<li>Open32 might be good choice here because of necessary interaction with
    OS/2 printer drivers.
<li>\'Open32 supports the Win32 DeviceCapabilities function but does not
    support the old Windows method of loading the printer driver and accessing
    an internal DeviceCapabilities function. Any application that tries to use
    DeviceCapabilities or a DEVMODE structure to set or query a printer driver
    must have the proper Dynamic Job Properties DJP)-enabled OS/2 print drivers
    installed.\' <i>(Open32 Programming Guide and Reference)</i>\n
<li>\'I wrote an article for EDM/2 on Dynamic Job Properties mentioned above.
    It\'s an undocumented API that IBM used to implement Open32 and has
    released to some large companies. In response to my article in EDM/2,
    someone sent me documentation and example programs from IBM. So, if no one
    else has or knows about it, keep in mind that I do and will be happy to
    pass information along.\' (Jason Koeninger)\n
<li>Wine has semi-functional PostScript output and may also use WIN16 printer
    drivers.\n'
);
--'
--INSERT INTO tgroupmember(tgroup, author, codemaintainer)
--    SELECT 3009, refcode, 'Y' FROM author
--        WHERE name IN (
--                          <nobody>
--                        );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3009, refcode FROM author
        WHERE name IN (
                        'Lee Riemenschneider',
                        'Jason Koeninger',
                        'Jeppe Cramon'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3009, refcode FROM dll WHERE name IN ('WINSPOOL');



-- Ole32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3010,    3, 'OLE32'
,'Support for OLE (Object Linking and Embedding) inter-process communication,
ActiveX and related API functions.'
,
'<li>For non OLE programs that like to know that OLE is around all should be OK.\n
<li>Simple VB apps now run (some repaint problems & comboboxes don\'t work)\n
<li>Next area of investigation - Connectable objects (a.k.a Events) & Typelibs.\n
<li>The next \'Target\' for OLE is to get a simple \'Hello World\' VB program running.\n
<li>Actuall implementation is being ported from WINE, much of WORK has been done lately on this (mainly from Corel)\n'
);
--'
INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3010, refcode, 'Y' FROM author
        WHERE name IN (
                        'David Raison '
                       );
--INSERT INTO tgroupmember(tgroup, author)
--    SELECT 3010, refcode FROM author
--        WHERE name IN (
--                          <nobody>
--                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3010, refcode FROM dll WHERE name LIKE '%OLE%';



-- Version
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3011,    3, 'VERSION'
,'Implement versioning API'
,
'<li>Sander did those in old project (\'Completed, except for some exotic apis.\')\n'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3011, refcode, 'Y' FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'Knut St. Osmundsen',
                        'Patrick Haller',
                        'Achim Hasenmueller'
                       );
--INSERT INTO tgroupmember(tgroup, author)
--    SELECT 3011, refcode FROM author
--        WHERE name IN (
--                          <nobody>
--                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3011, refcode FROM dll WHERE name IN ('VERSION');



-- ADVAPI32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3012,    3, 'ADVAPI32'
,'Implementing registry API and connecting it to OS/2 registry manager,
implementing NT services API and eventually NT Security API using
OS/2\'s SES.'
,
'<li>Registry API 99% implemented through Open32, should stay this way for
    compatibility reasons (Lotus SmartSuite), no need for DOING registry
    manager and we have RegEdit from IBM.\n
<li>Sander did registry API in old project, Patrick Haller added stubs
<li>NT Security not priority (can be ported from WINE later)
<li>John P. Baker stated that he\'s interested in using SES to implement
    security API'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3012, refcode, 'Y' FROM author
        WHERE name IN (
                        'Patrick Haller '
                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3012, refcode FROM author
        WHERE name IN (
                        'Sander van Leeuwen',
                        'John P. Baker'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3012, refcode FROM dll WHERE name IN ('ADVAPI32');



-- NTDLL
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3013,    3, 'NTDLL'
,'Low level NT system services. Also found in <tt>KERNEL32.DLL</tt>, but some
apps reference NTDLL instead. Also contains security related apis.'
,
'<li>Patrick Haller did those for old project\n
<li>Ported Wine code.'
);

INSERT INTO tgroupmember(tgroup, author, codemaintainer)
    SELECT 3013, refcode, 'Y' FROM author
        WHERE name IN (
                        'Patrick Haller'
                       );
--INSERT INTO tgroupmember(tgroup, author)
--    SELECT 3013, refcode FROM author
--        WHERE name IN (
--                          <nobody>
--                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3013, refcode FROM dll WHERE name IN ('NTDLL');



-- TAPI32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3014,    3, 'TAPI32'
,'Implementing Telephony API, needed by many communication applications,
fax/voice software, etc.'
,
'<li>Should be ported from Wine (?).'
);

--INSERT INTO tgroupmember(tgroup, author, codemaintainer)
--    SELECT 3014, refcode, 'Y' FROM author
--        WHERE name IN (
--                       );
--INSERT INTO tgroupmember(tgroup, author)
--    SELECT 3014, refcode FROM author
--        WHERE name IN (
--                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3014, refcode FROM dll WHERE name IN ('TAPI32');



-- CAPI32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3015,    3, 'CAPI32'
,'CAPI v2.0 ISDN API implementation through <tt>CAPI2032.DLL</tt>'
,
'<li>Felix Maschek did those in old project.'
);

--INSERT INTO tgroupmember(tgroup, author, codemaintainer)
--    SELECT 3015, refcode, 'Y' FROM author
--        WHERE name IN (
--                       );
INSERT INTO tgroupmember(tgroup, author)
    SELECT 3015, refcode FROM author
        WHERE name IN (
                        'Felix Maschek'
                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3015, refcode FROM dll WHERE name IN ('CAPI2032');



-- MAPI32
INSERT INTO tgroup(refcode, team,        name,  description, notes)
    VALUES        (   3016,    3, 'MAPI32'
,'Implementing \'high-level\' MAIL API so MAPI compilant applications can
directly send mail using OS/2 messaging software and native mail packages.'
,
'<li>Might be implemented \'openly\', with support for \'plugins\', so various
     software can be used as back-end for MAPI compilant applications.'
);

--INSERT INTO tgroupmember(tgroup, author, codemaintainer)
--    SELECT 3016, refcode, 'Y' FROM author
--        WHERE name IN (
--                       );
--INSERT INTO tgroupmember(tgroup, author)
--    SELECT 3016, refcode FROM author
--        WHERE name IN (
--                       );
INSERT INTO tgroupdll(tgroup, dll)
    SELECT 3016, refcode FROM dll WHERE name IN ('MAPI32');



INSERT INTO tgroup(refcode, team,        name,  description)
    VALUES        (   3999,    3, 'Unassigned',
'To get out of this group and join one that works on something! :)\n
<i>Note: Consisting mostly of lurkers that offered their help but have yet to
decide (or to be told) which group to join and what to do.</i>'
);

INSERT INTO tgroupmember(tgroup, author)
    SELECT 3999, refcode FROM author
        WHERE name IN ( 'Alan Lo',
                        'Andrew Hagen',
                        'Carsten Tenbrink',
                        'Christoph Bratschi',
                        'Eric Norman',
                        'Oliver Wilcock',
                        'omurata@ga2.so-net.ne.jp',
                        'rprice@wdn.com',
                        'Samuel Audet',
                        'Timothy J. Massey'
                       );



--
-- DEM team
--
INSERT INTO team(refcode,
                 name,
                 longname,
                 description,
                 tasks,
                 notes)
VALUES (
4,
'DEM',
'Documentation, Evaluation, Marketing'
,
''
,
'<li>Writing end-user documentaion\n
<li>Writing development (cross-team) documentation\n
<li>Maintaining Web page and ftp directory\n
<li>Administering CVS server\n
<li>Making relevant informatons easily accessable\n
<li>First-step testing and evaluation\n
<li>Writing and distributing announcements\n
<li>Promoting project'
,
'Providing all necessary and relevant information and data
easily accessable to team members and interested parties
over the internet is the primary task for this team. This
includes CVS server for code store and update, on-line
database of Odin32 API functions with web front-end,
selected external news and informations relevant for
project, promoting project to users, user groups and
companies, etc.'
);


INSERT INTO tgroup(refcode, team,    name, description)
    VALUES        (   4000,    4, 'dummy',     'dummy');

INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'Achim Hasenmueller';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'Adrian Gschwend';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'Christian Langanke';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'Nenad Milenkovic';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'Marco A. Morales';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'Richard P. Burke';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'TechnicalDirector@...';
INSERT INTO tgroupmember(tgroup, author)
    SELECT 4000, refcode FROM author WHERE name = 'knut st. osmundsen';



--
-- TES team
--
INSERT INTO team(refcode,
                 name,
                 longname,
                 description,
                 tasks,
                 notes)
VALUES (
5,
'TES',
'Testing, Evangelism, Support'
,
'The TES Team is external, envisioned as the Team of active project
supporters that have no time, resources or knowledge to contribute
directly. They participate by bug testing non-official releases, providing
Odin banners and links on their home pages, stating Team membership
in their SIGs, providing help to new and unexperienced users in
newsgroups, advocating project and it\'s goals, donating nice logos and
graphics, finding out applications that work, better name for this team,
etc.
<p>\n
Some sort of \'formal\' membership will probably be established if there\'s
enough interest. In the meantime, if you feel like you are willing to do
anything from above list, consider yourself member of Project Odin,
TES Team!
<p>\n
Remember: the more visibility for this project we achieve, the better
chances are that IBM or any other we need help from will consider what
are we doing here and maybe even help! If you support this project,
support it actively!\n'
,
NULL
,
NULL
);


INSERT INTO tgroup(refcode, team,    name, description)
    VALUES        (   5000,    5, 'dummy',     'dummy');

