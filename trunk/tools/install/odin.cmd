/* $Id: odin.cmd,v 1.36 2001-08-08 12:05:21 sandervl Exp $
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
 * Defaults.
 */
sType = 'Daily Build';


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
    if (ch = '-' | ch = '/') then
    do  /* option */
        sOption = translate(substr(asArg.i, 2));
        select
            when (sOption = 'WEEKLY') then
                sType = 'Weekly Build';
            when (sOption = 'DAILY') then
                sType = 'Daily Build';
            when (sOption = 'ALPHA') then
                sType = 'Alpha Release';
            otherwise
                call Syntax;
                exit(1);
        end
        i = i + 1;
        iterate;
    end

    /*
     * The argument specifies a dlldirectory under bin.
     * Make distribution.
     */
    if (CreateWarpInScript('Odin32Inst.wis', asArg.i, sType) <> 0) then
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
say 'Syntax: Odin.cmd [options] <dlldir> [options] [dlldir2] [options] [dlldir3]';
say '    Where <dlldir> is the subdirectory name in /bin.';
say '    And options is -Daily, -Weekly or -Alpha. Default: -Daily';
return;


/*
 * Create the install script file
 */
CreateWarpInScript: procedure
parse arg sInstFile, sDlldir, sType

/* Find include directory */
sCurDir = directory();
sMainDir = filespec('path', sCurDir);
sMainDir = filespec('path', substr(sMainDir, 1, length(sMainDir) - 1));
sMainDir = filespec('drive', sCurDir) || substr(sMainDir, 1, length(sMainDir) - 1);
sDocDir = sMainDir||'\include';

/* Odin32 version and build number */
sVerMajor = LookupDefine('#define=ODIN32_VERSION_MAJOR,'sDocDir'\odinbuild.h');
sVerMinor = LookupDefine('#define=ODIN32_VERSION_MINOR,'sDocDir'\odinbuild.h');
sVerBuild = LookupDefine('#define=ODIN32_BUILD_NR,'sDocDir'\odinbuild.h');
sPackVer = sVerMajor'\'sVerMinor'\'sVerBuild
sVer = 'Version 'sVerMajor'.'sVerMinor' Build no.'sVerBuild

/* remove any previous script */
call SysFileDelete sInstFile;

/* create warpin installation script */
rc = lineout(sInstFile, '<WARPIN>', 1);
rc = lineout(sInstFile, 'VERSION=0.9.6');
rc = lineout(sInstFile, 'OS=OS2_3x');
rc = lineout(sInstFile, '<HEAD>');
rc = lineout(sInstFile, '<TITLE>Odin32 'sType' - 'sVer'</TITLE>');
rc = lineout(sInstFile, '<PCK INDEX=1');
rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Odin Core Files\'sPackVer'"');
rc = lineout(sInstFile, '     CREATEOBJECT="WPFolder|Odin|<WP_DESKTOP>|OBJECTID=<ODINFOLDER>;"');
rc = lineout(sInstFile, '     CREATEOBJECT="WPProgram|Readme|<ODINFOLDER>|EXENAME=E.EXE;PARAMETERS=$(1)\Readme.txt;"');
rc = lineout(sInstFile, '     CREATEOBJECT="WPProgram|ChangeLog|<ODINFOLDER>|EXENAME=E.EXE;PARAMETERS=$(1)\ChangeLog;"');
rc = lineout(sInstFile, '     CREATEOBJECT="WPProgram|License|<ODINFOLDER>|EXENAME=E.EXE;PARAMETERS=$(1)\LICENSE.TXT;"');
rc = lineout(sInstFile, '     CREATEOBJECT="WPProgram|WGSS50 License|<ODINFOLDER>|EXENAME=E.EXE;PARAMETERS=$(1)\WGSS50.lic;"');
rc = lineout(sInstFile, '     CREATEOBJECT="WPProgram|Registry Editor|<ODINFOLDER>|EXENAME=REGEDIT2.EXE;"');
rc = lineout(sInstFile, '     TARGET="C:\ODIN" BASE');
title = "     TITLE=""Odin "sType" ("date()")""";
rc = lineout(sInstFile, title);
rc = lineout(sInstFile, '     SELECT NODESELECT');
rc = lineout(sInstFile, '     >Installation of Odin 'sType' .</PCK>');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '<PCK INDEX=2');
rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Odin System Files\'sPackVer'"');
rc = lineout(sInstFile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(sInstFile, '     FIXED SELECT NODESELECT');
title = "     TITLE=""Odin "sType" System Files ("date()")""";
rc = lineout(sInstFile, title);
rc = lineout(sInstFile, '     EXECUTE="odininst.exe"');
rc = lineout(sInstFile, '     CONFIGSYS="LIBPATH=$(1)\SYSTEM32 | ADDRIGHT"');
rc = lineout(sInstFile, '     CONFIGSYS="SET PATH=$(1)\SYSTEM32 | ADDRIGHT"');
rc = lineout(sInstFile, '     >Installation of Odin System files .</PCK>');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '<PCK INDEX=3');
rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Add Win32k.sys to Config.sys\'sPackVer'"');
rc = lineout(sInstFile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(sInstFile, '     FIXED');
title = "     TITLE=""Add Win32k.sys to Config.sys""";
rc = lineout(sInstFile, title);
rc = lineout(sInstFile, '     CONFIGSYS="DEVICE=$(1)\SYSTEM32\Win32k.sys -pe:pe -Dllfixes:D | UNIQUE(WIN32K.SYS) ADDTOP"');
rc = lineout(sInstFile, '     >Add Win32k.sys to Config.sys.</PCK>');
rc = lineout(sInstFile, '');

/*
 * For release packages it is optionally to install the debug symbol files.
 */
if (pos('DEBUG', translate(filespec('name', sDllDir))) <= 0) then
do
    rc = lineout(sInstFile, '<PCK INDEX=4');
    rc = lineout(sInstFile, '     PACKAGEID="Odin\Odin\Odin .sym files\'sPackVer'"');
    rc = lineout(sInstFile, '     TARGET="C:\ODIN\SYSTEM32"');
    rc = lineout(sInstFile, '     FIXED');
    title = "     TITLE=""Odin "sType" .sym Files ("date()")""";
    rc = lineout(sInstFile, title);
    rc = lineout(sInstFile, '     >Installation of Odin .sym files.</PCK>');
    rc = lineout(sInstFile, '');
end

rc = lineout(sInstFile, '</HEAD>');
rc = lineout(sInstFile, '<BODY>');
rc = lineout(sInstFile, '<PAGE INDEX=1 TYPE=TEXT>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=2>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Project Odin Information');
rc = lineout(sInstFile, '========================');
rc = lineout(sInstFile, '');
rc = charout(sInstFile, 'Odin is a free Open Source project with the goal to eventually ');
rc = charout(sInstFile, 'provide a fully working Win32 (Win 95/98/NT/2000) runtime environment, ');
rc = charout(sInstFile, 'making it possible to run your favorite Windows programs under OS/2, ');
rc = charout(sInstFile, 'as if they were native OS/2 programs. The second goal of Odin is to ');
rc = charout(sInstFile, 'provide the full Win32 application programming interface (API) so ');
rc = charout(sInstFile, 'that it is possible to build OS/2 programs using Windows source ');
rc = charout(sInstFile, 'code. This is also refered to as Odin32 whereas Odin is the name ');
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
rc = lineout(sInstFile, '     Dietrich Teickner');
rc = lineout(sInstFile, '     Kai Sommerfeld');
rc = lineout(sInstFile, '     Carsten Tenbrink');
rc = lineout(sInstFile, '     Vit Timchishin');
rc = lineout(sInstFile, '     Joel Troster');
rc = lineout(sInstFile, '     Shingo Tsuda');
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
rc = lineout(sInstFile, 'You may, without charge, royalty, or other payment, copy and ');
rc = lineout(sInstFile, 'distribute copies of this work and derivative works of this work ');
rc = lineout(sInstFile, 'in source or binary form provided that:');
rc = lineout(sInstFile, '(1) you appropriately publish on each copy an appropriate ');
rc = lineout(sInstFile, 'copyright notice; (2) faithfully reproduce all prior copyright ');
rc = lineout(sInstFile, 'included in the original work; and (3) agree to indemnify and ');
rc = lineout(sInstFile, 'hold all prior authors, copyright holders and licensors of the ');
rc = lineout(sInstFile, 'work harmless from and against all damages arising from use of ');
rc = lineout(sInstFile, 'or the inability to use the work.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You may distribute binaries and sources of derivative works of ');
rc = lineout(sInstFile, 'the work provided that: (1) all modifications to the original ');
rc = lineout(sInstFile, 'source code are publically available; (2) all modifications are ');
rc = lineout(sInstFile, 'released under this license; (3) all copyrights of the original ');
rc = lineout(sInstFile, 'work are included; (4) the modifications are a direct advantage ');
rc = lineout(sInstFile, 'for the functionality and completeness of the product or improve ');
rc = lineout(sInstFile, 'interoperability of the product with another product and do not ');
rc = lineout(sInstFile, 'interfere with the goal of an open source and free emulation ');
rc = lineout(sInstFile, 'product.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You may NOT use the work as base of another work that is ');
rc = lineout(sInstFile, 'not released under this license agreement meaning that you ');
rc = lineout(sInstFile, 'especially are not allowed to develop any other product which ');
rc = lineout(sInstFile, 'is based on the sources and/or binaries of the work.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'However, you are encouraged to use the work for porting your ');
rc = lineout(sInstFile, 'free or commercial applications. You can ship software ');
rc = lineout(sInstFile, 'converted using the work as long as the original copyright ');
rc = lineout(sInstFile, 'for the work is mentioned and you do not charge for ');
rc = lineout(sInstFile, 'using any part of the work. You may also ship the work in ');
rc = lineout(sInstFile, 'binaries and sources for use with your applications as long ');
rc = lineout(sInstFile, 'as you do not violate the distribution regulations of this license.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'These license terms can be amended by a custom license ');
rc = lineout(sInstFile, 'agreement between a third party and the administration ');
rc = lineout(sInstFile, 'team. Such a custom license might only limit the above terms ');
rc = lineout(sInstFile, 'if the changes cause a direct advantage for the functionality ');
rc = lineout(sInstFile, 'or completeness of the product and do not interfere with the ');
rc = lineout(sInstFile, 'goal of an open-source and free emulation product.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Parts of the work may be governed by a license other than ');
rc = lineout(sInstFile, 'this one, if the source directory of the part contains a ');
rc = lineout(sInstFile, 'license file called "LICENSE.TXT" of its own. If a different ');
rc = lineout(sInstFile, 'license exists, it supercedes this license. Removing the ');
rc = lineout(sInstFile, 'the part specific license will automatically put back the ');
rc = lineout(sInstFile, 'part under the terms of this license. Only the Odin management ');
rc = lineout(sInstFile, 'team is allowed to put parts of the work under a different ');
rc = lineout(sInstFile, 'license and remove the part specific licenses. The purpose ');
rc = lineout(sInstFile, 'of using different licenses is solely for allowing code ');
rc = lineout(sInstFile, 'sharing of certain parts of the work with other projects that ');
rc = lineout(sInstFile, 'have incompatible licenses. These parts should either have ');
rc = lineout(sInstFile, 'this license in a version that makes it compatible with the ');
rc = lineout(sInstFile, 'license of the second project, or the license of the second ');
rc = lineout(sInstFile, 'project as long as it conforms to the goals of an open source ');
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
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=4>~I agree</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'By pressing the "I agree" button, you agree to all terms and conditions to the below licence agreement.');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '<README>');
rc = lineout(sInstFile, 'WGSS50 LIBRARY LICENSE');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'BEFORE USING THE CODE AND DOCUMENTATION CONTAINED IN THIS PACKAGE,');
rc = lineout(sInstFile, '(HEREINAFTER REFERRED TO AS THE "PROGRAM"), YOU SHOULD CAREFULLY READ');
rc = lineout(sInstFile, 'THE FOLLOWING TERMS AND CONDITIONS. USING THE PROGRAM INDICATES YOUR');
rc = lineout(sInstFile, 'ACCEPTANCE OF THESE TERMS AND CONDITIONS.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'The PROGRAM is licensed not sold. InnoTek grants you a license for the');
rc = lineout(sInstFile, 'PROGRAM only in the country where you acquired the PROGRAM. You obtain');
rc = lineout(sInstFile, 'no rights other than those granted you under this license. The term');
rc = lineout(sInstFile, '"PROGRAM" means the original and all whole or partial copies of it,');
rc = lineout(sInstFile, 'including modified copies or portions merged into other programs.');
rc = lineout(sInstFile, 'InnoTek has licensed from the owner copyrights in the PROGRAM.');
rc = lineout(sInstFile, 'You are responsible for the selection of the PROGRAM and for the');
rc = lineout(sInstFile, 'installation of, use of, and results obtained from, the PROGRAM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '1. License');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Under this license, you may:');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '1) use the PROGRAM only if you are a licensee of OS/2;');
rc = lineout(sInstFile, '2) use the PROGRAM on only one machine at any one time;');
rc = lineout(sInstFile, '3) copy the PROGRAM for backup or in order to modify it;');
rc = lineout(sInstFile, '4) modify the PROGRAM and/or merge it into another program as an essential');
rc = lineout(sInstFile, '   utilization of the PROGRAM in conjunction with your machine, and;');
rc = lineout(sInstFile, '5) transfer the possession of the PROGRAM to another party.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'If you transfer the PROGRAM, you must transfer a copy of this license,');
rc = lineout(sInstFile, 'all other documentation and at least one complete, unaltered copy of the');
rc = lineout(sInstFile, 'PROGRAM to the other party. You must, at the same time, either transfer all');
rc = lineout(sInstFile, 'your other copies of the PROGRAM to the other party or destroy them.');
rc = lineout(sInstFile, 'Your license is then terminated. The other party agrees to these terms');
rc = lineout(sInstFile, 'and conditions by its first use of the PROGRAM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You must reproduce the copyright notice(s) on each copy, or partial copy,');
rc = lineout(sInstFile, 'of the PROGRAM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Under this license, you may not:');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '1) use the PROGRAM unless you are a licensee of OS/2;');
rc = lineout(sInstFile, '2) use, copy, modify, merge, or transfer copies of the PROGRAM except');
rc = lineout(sInstFile, '   as provided in this license;');
rc = lineout(sInstFile, '3) reverse assemble or reverse compile the PROGRAM; or');
rc = lineout(sInstFile, '4) sublicense, rent, lease, or assign the PROGRAM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '2. Warranty');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'THE PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.');
rc = lineout(sInstFile, 'INNOTEK DISCLAIMS ALL WARRANTIES FOR THE PROGRAM, EITHER EXPRESS OR');
rc = lineout(sInstFile, 'IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF');
rc = lineout(sInstFile, 'MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Some jurisdictions do not allow exclusions of implied warranties,');
rc = lineout(sInstFile, 'so the above exclusions may not apply to you.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '3. Limitation of Remedies');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'IN NO EVENT WILL INNOTEK BE LIABLE FOR ANY LOST PROFITS, LOST SAVINGS,');
rc = lineout(sInstFile, 'INCIDENTAL OR INDIRECT DAMAGES OR OTHER ECONOMIC CONSEQUENTIAL DAMAGES,');
rc = lineout(sInstFile, 'EVEN IF INNOTEK OR ITS AUTHORIZED SUPPLIER HAS BEEN ADVISED OF THE');
rc = lineout(sInstFile, 'POSSIBILITY OF SUCH DAMAGES. IN ADDITION, INNOTEK WILL NOT BE LIABLE FOR');
rc = lineout(sInstFile, 'ANY DAMAGES CLAIMED BY YOU BASED ON ANY THIRD-PARTY CLAIM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'The above limitation of remedies also applies to any developer or');
rc = lineout(sInstFile, 'supplier of the PROGRAM. Such developer and/or supplier is an intended');
rc = lineout(sInstFile, 'beneficiary of this Section.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Some jurisdictions do not allow these limitations or exclusions, so');
rc = lineout(sInstFile, 'they may not apply to you.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, '4. General');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You may terminate your license at any time. InnoTek may terminate your');
rc = lineout(sInstFile, 'license if you fail to comply with the terms and conditions of this');
rc = lineout(sInstFile, 'license. In either event, you must destroy all your copies of the PROGRAM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'You are responsible for payment of any taxes, including personal property');
rc = lineout(sInstFile, 'taxes, resulting from this license.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'Neither party may bring an action, regardless of form, more than two');
rc = lineout(sInstFile, 'years after the cause of the action arose.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'If you acquire the PROGRAM in the United States, this license is governed');
rc = lineout(sInstFile, 'by the laws of the State of New York.  If you acquired the PROGRAM in');
rc = lineout(sInstFile, 'Canada, this license is governed by the laws of the Province of Ontario.');
rc = lineout(sInstFile, 'Otherwise, this license is governed by the laws of the country in which');
rc = lineout(sInstFile, 'you acquired the PROGRAM.');
rc = lineout(sInstFile, '');
rc = lineout(sInstFile, 'InnoTek Systemberatung GmbH');
rc = lineout(sInstFile, 'Aspenweg 16');
rc = lineout(sInstFile, '88097 Eriskirch');
rc = lineout(sInstFile, 'Germany');
rc = lineout(sInstFile, 'http://www.innotek.de');
rc = lineout(sInstFile, 'info@innotek.de');
rc = lineout(sInstFile, '</README>');
rc = lineout(sInstFile, '</PAGE>');

rc = lineout(sInstFile, '<PAGE INDEX=4 TYPE=README>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=5>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'List of changes and bugfixes for this Odin 'sType);
rc = lineout(sInstFile, '('sVer'  - 'date()')');
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
rc = lineout(sInstFile, '<PAGE INDEX=5 TYPE=CONTAINER>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=6>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Please select the packages which are to be installed. You may change the target paths for the packages.');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=6 TYPE=CONFIGURE>');
rc = lineout(sInstFile, '<NEXTBUTTON TARGET=7>~Next</NEXTBUTTON>');
rc = lineout(sInstFile, '<TEXT>');
rc = lineout(sInstFile, 'Please select additional configuration that WarpIN should perform after installing this archive.');
rc = lineout(sInstFile, '</TEXT>');
rc = lineout(sInstFile, '</PAGE>');
rc = lineout(sInstFile, '<PAGE INDEX=7 TYPE=TEXT>');
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
sDocDir = sMainDir||'\doc';
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
sWICCmd '1 -c'||sMainDir 'ChangeLog LICENSE.TXT WGSS50.lic';
if (rc <> 0) then return rc;
sWICCmd '1 -c'||sDocDir 'ChangeLog-1999 ChangeLog-2000 ChangeLog-2001 Readme.txt ReportingBugs.txt Logging.txt Odin.ini.txt';
if (rc <> 0) then return rc;

/*
 * Packet 2
 */
/* sWICCmd '2 -c'||sBinDir' odin.ini'; */
/* if (rc <> 0) then return rc; */
sWICCmd '2 -c'||sDllDir||' pe.exe *.dll pec.exe odininst.exe regsvr32.exe win32k.sys win32k.ddp Win32kCC.exe kRx.exe Xx2Lx.exe';
if (rc <> 0) then return rc;
sWICCmd '2 -c'||sBinDir||' wgss50.dll';
if (rc <> 0) then return rc;

if (pos('DEBUG', translate(filespec('name', sDllDir)))) then
do
    iDbg = lastpos('DEBUG', translate(sDllDir));
    sRelDir = substr(sDllDir, 1, iDbg - 1)||'release'||substr(sDllDir, iDbg + 5);
    sWICCmd '2 -c'||sRelDir 'odincrt.dll odincrt.sym';
    if (rc <> 0) then return rc;

    sWICCmd '2 -c'||sDllDir '*.sym';
    if (rc <> 0) then return rc;
end


/*
 * Packet 3 is only win32k.sys config.sys line but it seems like we have to add something.
 */
sWICCmd '3 -c'||sDllDir '*.ddp';
if (rc <> 0) then return rc;


/*
 * Packet 4 is the .sym files and only present in release mode.
 */
if (pos('DEBUG', translate(filespec('name', sDllDir))) <= 0) then
do
    sWICCmd '4 -c'||sDllDir '*.sym';
    if (rc <> 0) then return rc;
end


return rc;


/**
 * Search for a #define in an C/C++ header or source file.
 *
 * @returns String containing the defined value
 *          found for the define in the header file.
 *          Quits on fatal errors.
 * @param   A string on the form: "#define=DEFINETOFIND,includefile.h"
 * @remark  Write only code... - let's hope it works.
 *          (From bldlevelinf.cmd)
 */
LookupDefine: procedure
    parse arg '#'sDefine'='sMacro','sIncludeFile

    /*
     * Validate parameters.
     */
    sMacro = strip(sMacro);
    sIncludeFile = strip(sIncludeFile);
    if (sMacro = '') then
    do
        say 'syntax error: #define=<DEFINE_NAME>,<includefile.h>.';
        say '    <DEFINE_NAME> was empty.';
        exit(-20);
    end
    if (sIncludeFile = '') then
    do
        say 'syntax error: #define=<DEFINE_NAME>,<includefile.h>.';
        say '    <includefile.h> was empty.';
        exit(-20);
    end


    sIllegal = translate(translate(sMacro),,
                         '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!',,
                         'ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_');

    if (strip(translate(sIllegal, ' ', '!')) <> '') then
    do
        say 'syntax error: #define=<DEFINE_NAME>,<includefile.h>.';
        say '    <DEFINE_NAME> contains illegal charater(s).'
        say '        'sMacro;
        say '        'translate(sIllegal, ' ', '!');
        exit(-20);
    end

    /*
     * Open include file.
     */
    sRc = stream(sIncludeFile, 'c', 'open read');
    if (pos('READY', sRc) <> 1) then
    do  /* search INCLUDE variable */
        sFile = SysSearchPath('INCLUDE', sIncludeFile);
        if (sFile = '') then
        do
            say 'Can''t find include file 'sIncludeFile'.';
            exit(-20);
        end
        sIncludeFile = sFile;

        sRc = stream(sIncludeFile, 'c', 'open read');
        if (pos('READY', sRc) <> 1) then
        do
            say 'Failed to open include file' sIncludeFile'.';
            exit(-20);
        end
    end

    /*
     * Search the file line by line.
     *  We'll check for lines starting with a hash (#) char.
     *  Then check that the word after the hash is 'define'.
     *  Then match the next word with the macro name.
     *  Then then get the next rest of the line to comment or continuation char.
     *      (continuation is not supported)
     *  Finally strip quotes.
     */
    sValue = '';
    do while (lines(sIncludeFile) > 0)
        sLine = strip(linein(sIncludeFile));
        if (sLine = '') then
            iterate;
        if (substr(sLine, 1, 1) <> '#') then
            iterate;
        sLine = substr(sLine, 2);
        if (word(sLine, 1) <> 'define') then
            iterate;
        sLine = strip(substr(sLine, wordpos(sLine, 1) + length('define')+1));
        if (  substr(sLine, 1, length(sMacro)) <> sMacro,
            | substr(sLine, length(sMacro)+1, 1) <> ' ') then
            iterate;
        sLine = strip(substr(sLine, length(sMacro) + 1));
        if (sLine = '') then
        do
            say 'error: #define' sMacro' is empty.';
            call stream sIncludeFile, 'c', 'close';
            exit(-20);
        end

        chQuote = substr(sLine, 1, 1);
        if (chQuote = '"' | chQuote = "'") then
        do  /* quoted string */
            iLastQuote = 0;
            do forever
                iLast = pos(chQuote, sLine, 2);
                if (iLast <= 0) then
                    leave;
                if (substr(sLine, iLast, 1) = '\') then
                    iterate;
                iLastQuote = iLast;
                leave;
            end

            if (iLastQuote <= 0) then
            do
                say 'C/C++ syntax error in 'sIncludefile': didn''t find end quote.';
                call stream sIncludeFile, 'c', 'close';
                exit(-20);
            end

            call stream sIncludeFile, 'c', 'close';
            sValue = substr(sLine, 2, iLastQuote - 2);
            say 'Found 'sMacro'='sValue;
            return sValue;
        end
        else
        do
            iCommentCPP = pos('//',sLine);
            iCommentC   = pos('/*',sLine);
            if (iCommentC > 0 & iCommentCPP > 0 & iCommentC > iCommentCPP) then
                iComment = iCommentCPP;
            else if (iCommentC > 0 & iCommentCPP > 0 & iCommentC < iCommentCPP) then
                iComment = iCommentC;
            else if (iCommentCPP > 0) then
                iComment = iCommentCPP;
            else if (iCommentC > 0) then
                iComment = iCommentC;
            else
                iComment = 0;

            if (iComment > 0) then
                sValue = strip(substr(sLine, 1, iComment-1));
            else
                sValue = strip(sLine);

            if (sValue <> '') then
            do
                if (substr(sValue, length(sValue)) = '\') then
                do
                    say 'Found continuation char: Multiline definitions are not supported!\n';
                    call stream sIncludeFile, 'c', 'close';
                    exit(-20);
                end
            end

            if (sValue = '') then
                say 'warning: The #define has no value.';

            call stream sIncludeFile, 'c', 'close';
            say 'Found 'sMacro'='sValue;
            return sValue;
        end
    end

    call stream sIncludeFile, 'c', 'close';
    say 'error: didn''t find #define' sMacro'.';
    exit(-20);

