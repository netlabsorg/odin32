/* $Id: odin.cmd,v 1.23 2000-12-30 10:18:36 sandervl Exp $
 *
 * Odin32 API WarpIn installation script generator.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 *  Util to build WarpIn archive.
 *  Syntax: Odin.cmd <dlldir> [dlldir2] [dlldir3] [dlldir4]
 *      Where <dlldir> is the subdirectory name in /bin.
 *
 */


/*
 * Load REXX util functions.
 */
Call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
Call SysLoadFuncs


/*
 * Parse arguments.
 */
parse arg asArg.1 asArg.2 asArg.3 asArg.4 asArg.5 asArg.6 asArg.7 asArg.8 asArg.9


/* If no arguments Then display syntax and fail. */
if (asArg.1 = '') then
do
    call Syntax;
    exit(1);
end

/*
 * Process arguments.
 */
i = 1;
do while ((asArg.i <> '') & (i < 9))
    /*
     * Look for unsupported options.
     */
    ch = substr(asArg.i, 1, 1);
    if (ch = '-' || ch = '/') then
    do  /* option */
        call Syntax;
        exit(1);
    end

    /*
     * The argument specifies a dlldirectory under bin.
     * Make distribution.
     */
    if (CreateWarpInScript('Odin32Inst.wis', asArg.i) <> 0) then
    do
        say 'failed to create WarpIn script.'
        exit(2);
    end
    if (PackFiles('odin32inst.wis', asArg.i, MakeArchiveName(asArg.i)) <> 0) then
    do
        say 'failed to create WarpIn script.'
        exit(3);
    end
    i = i + 1;
end

/*
 * successfull exit
 */
exit(0);


/*
 * Display syntax.
 */
Syntax: procedure;
say 'Util to build WarpIn archive.';
say 'Syntax: Odin.cmd <dlldir> [dlldir2] [dlldir3] [dlldir4]';
say '    Where <dlldir> is the subdirectory name in /bin.';
return;


/*
 * Create the install script file
 */
CreateWarpInScript: procedure
parse arg sInstFile, sDlldir

call SysFileDelete sInstFile;

/* create warpin installation script */
rc = lineout(sInstFile, '<WARPIN>', 1);
rc = lineout(sInstFile, '<HEAD>');
rc = lineout(sInstFile, '<PCK INDEX=1');
rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Odin Core Files\0\0\5"');
rc = lineout(sInstFile, '     TARGET="C:\ODIN" BASE');
title = "     TITLE=""Odin Daily Build ("date()")""";
rc = lineout(sInstFile, title);
rc = lineout(sInstFile, '     SELECT NODESELECT');
rc = lineout(sInstFile, '     >Installation of Odin Daily Build .</PCK>');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '<PCK INDEX=2');
rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Odin System Files\0\0\5"');
rc = lineout(sInstFile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(sInstFile, '     FIXED SELECT NODESELECT');
title = "     TITLE=""Odin Daily Build System Files ("date()")""";
rc = lineout(sInstFile, title);
rc = lineout(sInstFile, '     EXECUTE="odininst.exe"');
rc = lineout(sInstFile, '     CONFIGSYS="LIBPATH=$(1)\SYSTEM32 | ADDRIGHT"');
rc = lineout(sInstFile, '     CONFIGSYS="SET PATH=$(1)\SYSTEM32 | ADDRIGHT"');
rc = lineout(sInstFile, '     >Installation of Odin System files .</PCK>');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '<PCK INDEX=3');
rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Add Win32k.sys to Config.sys\0\0\5"');
rc = lineout(sInstFile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(sInstFile, '     FIXED');
title = "     TITLE=""Add Win32k.sys to Config.sys""";
rc = lineout(sInstFile, title);
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=WIN32K.SYS | REMOVELINE"');
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=$(1)\SYSTEM32\Win32k.sys -pe:pe -Dllfixes:D | UNIQUE ADDTOP"');
rc = lineout(sInstFile, '     >Add Win32k.sys to Config.sys .</PCK>');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '</HEAD>');
rc = lineout(sInstFile, '<BODY>');
rc = lineout(sInstFile, '<PAGE INDEX=1 TYPE=TEXT>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=2>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Project Odin Information');
rc = lineout(sInstFile, '========================');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Odin is a free Open Source project with the goal to eventually');
rc = lineout(sInstFile, 'provide a fully working Win32 (Win 95/98/NT/2000) runtime environment, ');
rc = lineout(sInstFile, 'making it possible to run your favorite Windows programs under OS/2, ');
rc = lineout(sInstFile, 'as if they were native OS/2 programs. The second goal of Odin is to');
rc = lineout(sInstFile, 'provide the full Win32 application programming interface (API) so');
rc = lineout(sInstFile, 'that it is possible to build OS/2 programs using Windows source');
rc = lineout(sInstFile, 'code. This is also refered to as Odin32 whereas Odin is the name');
rc = lineout(sInstFile, 'of the Win32 binary emulator.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Odin is available thanks to the work of the following people:');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '     Christoph Bratschi');
rc = lineout(sInstFile, '     Oliver Braun');
rc = lineout(sInstFile, '     Edgar Buerkle');
rc = lineout(sInstFile, '     Yuri Dario');
rc = lineout(sInstFile, '     Przemyslaw Dobrowolski');
rc = lineout(sInstFile, '     Daniela Engert');
rc = lineout(sInstFile, '     Peter Fitzsimmons');
rc = lineout(sInstFile, '     Adrian Gschwend');
rc = lineout(sInstFile, '     Jeroen van den Horn');
rc = lineout(sInstFile, '     Patrick Haller');
rc = lineout(sInstFile, '     Achim Hasenmueller');
rc = lineout(sInstFile, '     Kevin Langman');
rc = lineout(sInstFile, '     Bart van Leeuwen');
rc = lineout(sInstFile, '     Sander van Leeuwen');
rc = lineout(sInstFile, '     Felix Maschek');
rc = lineout(sInstFile, '     Chris McKillop');
rc = lineout(sInstFile, '     Nenad Milenkovic');
rc = lineout(sInstFile, '     Markus Montkowski');
rc = lineout(sInstFile, '     Michal Necasek');
rc = lineout(sInstFile, '     Ondrej Necasek');
rc = lineout(sInstFile, '     Knut Stange Osmundsen');
rc = lineout(sInstFile, '     Vitali E. Pelenyov');
rc = lineout(sInstFile, '     Rene Pronk');
rc = lineout(sInstFile, '     David Raison');
rc = lineout(sInstFile, '     Kai Sommerfeld');
rc = lineout(sInstFile, '     Carsten Tenbrink');
rc = lineout(sInstFile, '     Vit Timchishin');
rc = lineout(sInstFile, '     Joel Troster');
rc = lineout(sInstFile, '     Jens Wiessner');
rc = lineout(sInstFile, '     Chris Wohlgemuth');
rc = lineout(sInstFile, '     Vince Vielhaber');
rc = lineout(sInstFile, '     All the WINE people');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=2 TYPE=README>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=3>~I agree</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'By pressing the "I agree" button, you agree to all terms and conditions to the below licence agreement.');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '<README>');
rc = lineout(sInstFile, 'Project Odin Software License');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You may, without charge, royalty, or other payment, copy and');
rc = lineout(sInstFile, 'distribute copies of this work and derivative works of this work');
rc = lineout(sInstFile, 'in source or binary form provided that:');
rc = lineout(sInstFile, '(1) you appropriately publish on each copy an appropriate');
rc = lineout(sInstFile, 'copyright notice; (2) faithfully reproduce all prior copyright');
rc = lineout(sInstFile, 'included in the original work; and (3) agree to indemnify and');
rc = lineout(sInstFile, 'hold all prior authors, copyright holders and licensors of the');
rc = lineout(sInstFile, 'work harmless from and against all damages arising from use of');
rc = lineout(sInstFile, 'or the inability to use the work.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You may distribute binaries and sources of derivative works of');
rc = lineout(sInstFile, 'the work provided that: (1) all modifications to the original');
rc = lineout(sInstFile, 'source code are publically available; (2) all modifications are');
rc = lineout(sInstFile, 'released under this license; (3) all copyrights of the original');
rc = lineout(sInstFile, 'work are included; (4) the modifications are a direct advantage');
rc = lineout(sInstFile, 'for the functionality and completeness of the product or improve');
rc = lineout(sInstFile, 'interoperability of the product with another product and do not');
rc = lineout(sInstFile, 'interfere with the goal of an open source and free emulation');
rc = lineout(sInstFile, 'product.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You may NOT use the work as base of another work that is');
rc = lineout(sInstFile, 'not released under this license agreement meaning that you');
rc = lineout(sInstFile, 'especially are not allowed to develop any other product which');
rc = lineout(sInstFile, 'is based on the sources and/or binaries of the work.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'However, you are encouraged to use the work for porting your');
rc = lineout(sInstFile, 'free or commercial applications. You can ship software');
rc = lineout(sInstFile, 'converted using the work as long as the original copyright');
rc = lineout(sInstFile, 'for the work is mentioned and you do not charge for');
rc = lineout(sInstFile, 'using any part of the work. You may also ship the work in');
rc = lineout(sInstFile, 'binaries and sources for use with your applications as long');
rc = lineout(sInstFile, 'as you do not violate the distribution regulations of this license.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'These license terms can be amended by a custom license');
rc = lineout(sInstFile, 'agreement between a third party and the administration');
rc = lineout(sInstFile, 'team. Such a custom license might only limit the above terms');
rc = lineout(sInstFile, 'if the changes cause a direct advantage for the functionality');
rc = lineout(sInstFile, 'or completeness of the product and do not interfere with the');
rc = lineout(sInstFile, 'goal of an open-source and free emulation product.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Parts of the work may be governed by a license other than');
rc = lineout(sInstFile, 'this one, if the source directory of the part contains a');
rc = lineout(sInstFile, 'license file called "LICENSE.TXT" of its own. If a different');
rc = lineout(sInstFile, 'license exists, it supercedes this license. Removing the');
rc = lineout(sInstFile, 'the part specific license will automatically put back the');
rc = lineout(sInstFile, 'part under the terms of this license. Only the Odin management');
rc = lineout(sInstFile, 'team is allowed to put parts of the work under a different');
rc = lineout(sInstFile, 'license and remove the part specific licenses. The purpose');
rc = lineout(sInstFile, 'of using different licenses is solely for allowing code');
rc = lineout(sInstFile, 'sharing of certain parts of the work with other projects that');
rc = lineout(sInstFile, 'have incompatible licenses. These parts should either have');
rc = lineout(sInstFile, 'this license in a version that makes it compatible with the');
rc = lineout(sInstFile, 'license of the second project, or the license of the second');
rc = lineout(sInstFile, 'project as long as it conforms to the goals of an open source');
rc = lineout(sInstFile, 'and free emulation software.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Warranty');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'EXCEPT AS OTHERWISE RESTRICTED BY LAW, THIS WORK IS PROVIDED');
rc = lineout(sInstFile, 'WITHOUT ANY EXPRESSED OR IMPLIED WARRANTIES OF ANY KIND, INCLUDING');
rc = lineout(sInstFile, 'BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A');
rc = lineout(sInstFile, 'PARTICULAR PURPOSE, MERCHANTABILITY OR TITLE.  EXCEPT AS');
rc = lineout(sInstFile, 'OTHERWISE PROVIDED BY LAW, NO AUTHOR, COPYRIGHT HOLDER OR');
rc = lineout(sInstFile, 'LICENSOR SHALL BE LIABLE TO YOU FOR DAMAGES OF ANY KIND, EVEN IF');
rc = lineout(sInstFile, 'THEY HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.');
rc = lineout(sInstFile, '</README>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=3 TYPE=README>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=4>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'List of changes and bugfixes for this Odin daily build ('date()')');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '<README>');

/* Insert ChangeLog. */
sChangeLog = '..\..\ChangeLog';
sLogLine   = linein(sChangeLog);
do while (lines(sChangeLog) > 0)
    rc = lineout(sInstFile, sLogLine);
    sLogLine = linein(sChangeLog);
end
call stream sChangeLog, 'c', 'close';

rc = lineout(sInstFile, '</README>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=4 TYPE=CONTAINER>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=5>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Please select the packages which are to be installed. You may change the target paths for the packages.');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=5 TYPE=CONFIGURE>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=6>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Please select additional configuration that WarpIN should perform after installing this archive.');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=6 TYPE=TEXT>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=0>I~nstall</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Press "Install" to begin installing Odin.</TEXT>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '</BODY>');
rc = lineout(sInstFile, '</WARPIN>');
rc = lineout(sInstFile, '');
lrc= lineout(sInstFile);
return rc;



/*
 * Determins the archive name.
 */
MakeArchiveName: procedure;
parse arg sDllDir
sDllDir = filespec('name', sDllDir);
return directory()||'\odin32bin-'||date('S')||'-'||sDllDir||'.wpi';



/*
 * Procedure which packs the files and script into the .wpi file.
 */
PackFiles: procedure;
parse arg sInstFile, sDllDir, sInstallArchive

/*
 * Determin directories and wic.exe commandline.
 */
sCurDir = directory();
sMainDir = filespec('path', sCurDir);
sMainDir = filespec('path', substr(sMainDir, 1, length(sMainDir) - 1));
sMainDir = filespec('drive', sCurDir) || substr(sMainDir, 1, length(sMainDir) - 1);
sDocDir = sMainDir||'\doc'
sBinDir = sMainDir||'\bin';
sDllDir = sBinDir||'\'||sDllDir;
sWICCmd = 'wic.exe '||sInstallArchive ' -a';

/*
 * Remove any old target install archive file.
 */
call SysFileDelete sInstallArchive

/*
 * Script
 */
'wic.exe' sInstallArchive '-s' sInstFile

/*
 * Packet 1
 */
sWICCmd '1 -c'||sMainDir 'ChangeLog LICENSE.TXT';
if (rc <> 0) then return rc;
sWICCmd '1 -c'||sDocDir 'ChangeLog-1999 ChangeLog-2000 Readme.txt ReportingBugs.txt Logging.txt Odin.ini.txt';
if (rc <> 0) then return rc;

/*
 * Packet 2
 */
say sWICCmd '2 -c'||sBinDir' *.ini 2 -c'||sDllDir '*.dll pe.exe pec.exe odininst.exe regsvr32.exe win32k.sys win32k.ddp Win32kCC.exe kRx.exe';
/* sWICCmd '2 -c'||sBinDir' odin.ini'; */
if (rc <> 0) then return rc;
sWICCmd '2 -c'||sDllDir 'pe.exe *.dll pec.exe odininst.exe regsvr32.exe win32k.sys win32k.ddp Win32kCC.exe kRx.exe';
if (rc <> 0) then return rc;
if (pos('DEBUG', translate(filespec('name', sDllDir))) > 0) then
do
    sWICCmd '2 -c'||sDllDir '*.sym';
    if (rc <> 0) then return rc;
end

/*
 * Packet 3 is only win32k.sys config.sys line but it seems like we have to add something.
 */
sWICCmd '3 -c'||sDllDir '*.ddp';
if (rc <> 0) then return rc;

return rc;

