/* Odin installation script */
Call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
Call SysLoadFuncs

"del odininst.wis"

/* create warpin installation script */
instfile = 'odininst.wis'
rc = lineout(instfile, '<WARPIN>', 1);
rc = lineout(instfile, '<HEAD>');
rc = lineout(instfile, '<PCK INDEX=1');
rc = lineout(instfile, '     PACKAGEID="Odin\Odin\Odin Core Files\0\0\5"');
rc = lineout(instfile, '     TARGET="C:\ODIN" BASE');
title = "     TITLE=""Odin Daily Build ("date()")""";
rc = lineout(instfile, title);
rc = lineout(instfile, '     SELECT NODESELECT');
rc = lineout(instfile, '     >Installation of Odin Daily Build .</PCK>');
rc = lineout(instfile, '');
rc = lineout(instfile, '<PCK INDEX=2');
rc = lineout(instfile, '     PACKAGEID="Odin\Odin\Odin System Files\0\0\5"');
rc = lineout(instfile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(instfile, '     FIXED SELECT NODESELECT');
title = "     TITLE=""Odin Daily Build System Files ("date()")""";
rc = lineout(instfile, title);
rc = lineout(instfile, '     EXECUTE="odininst.exe"');
rc = lineout(instfile, '     CONFIGSYS="LIBPATH=$(1)\SYSTEM32 | ADDRIGHT"');
rc = lineout(instfile, '     CONFIGSYS="SET PATH=$(1)\SYSTEM32 | ADDRIGHT"');
rc = lineout(instfile, '     >Installation of Odin System files .</PCK>');
rc = lineout(instfile, '');
rc = lineout(instfile, '<PCK INDEX=3');
rc = lineout(instfile, '     PACKAGEID="Odin\Odin\Odin 3dfx Voodoo OpenGL Files\0\0\5"');
rc = lineout(instfile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(instfile, '     FIXED');
title = "     TITLE=""Odin Daily Build 3dfx Voodoo OpenGL Files ("date()")""";
rc = lineout(instfile, title);
rc = lineout(instfile, '     >Installation of Odin 3dfx Voodoo OpenGL files .</PCK>');
rc = lineout(instfile, '');
rc = lineout(instfile, '<PCK INDEX=4');
rc = lineout(instfile, '     PACKAGEID="Odin\Odin\Odin 3dfx Voodoo 1 Glide Files\0\0\5"');
rc = lineout(instfile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(instfile, '     FIXED');
title = "     TITLE=""Odin Daily Build 3dfx Voodoo 1 Glide Files ("date()")""";
rc = lineout(instfile, title);
rc = lineout(instfile, '     >Installation of Odin 3dfx Voodoo 1 Glide files .</PCK>');
rc = lineout(instfile, '');
rc = lineout(instfile, '<PCK INDEX=5');
rc = lineout(instfile, '     PACKAGEID="Odin\Odin\Odin 3dfx Voodoo 2 Glide Files\0\0\5"');
rc = lineout(instfile, '     TARGET="C:\ODIN\SYSTEM32"');
rc = lineout(instfile, '     FIXED');
title = "     TITLE=""Odin Daily Build 3dfx Voodoo 2 Glide Files ("date()")""";
rc = lineout(instfile, title);
rc = lineout(instfile, '     >Installation of Odin 3dfx Voodoo 2 Glide files .</PCK>');
rc = lineout(instfile, '</HEAD>');
rc = lineout(instfile, '<BODY>');
rc = lineout(instfile, '<PAGE INDEX=1 TYPE=TEXT>');
rc = lineout(instfile, '<NEXTBUTTON TARGET=2>~Next</NEXTBUTTON>');
rc = lineout(instfile, '<TEXT>');
rc = lineout(instfile, 'Project Odin Information');
rc = lineout(instfile, '========================');
rc = lineout(instfile, '');
rc = lineout(instfile, 'Odin is a free Open Source project with the goal to eventually');
rc = lineout(instfile, 'provide a fully working Win32 (Win 95/98/NT/2000) runtime environment,');
rc = lineout(instfile, 'making it possible to run your favorite Windows programs under OS/2,');
rc = lineout(instfile, 'as if they were native OS/2 programs. The second goal of Odin is to');
rc = lineout(instfile, 'provide the full Win32 application programming interface (API) so');
rc = lineout(instfile, 'that it is possible to build OS/2 programs using Windows source');
rc = lineout(instfile, 'code. This is also refered to as Odin32 whereas Odin is the name');
rc = lineout(instfile, 'of the Win32 binary emulator.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'Odin is available thanks to the work of the following people:');
rc = lineout(instfile, '');
rc = lineout(instfile, '     Christoph Bratschi');
rc = lineout(instfile, '     Edgar Buerkle');
rc = lineout(instfile, '     Yuri Dario');
rc = lineout(instfile, '     Przemyslaw Dobrowolski');
rc = lineout(instfile, '     Daniela Engert');
rc = lineout(instfile, '     Peter Fitzsimmons');
rc = lineout(instfile, '     Adrian Gschwend');
rc = lineout(instfile, '     Jeroen van den Horn');
rc = lineout(instfile, '     Patrick Haller');
rc = lineout(instfile, '     Achim Hasenmueller');
rc = lineout(instfile, '     Kevin Langman');
rc = lineout(instfile, '     Bart van Leeuwen');
rc = lineout(instfile, '     Sander van Leeuwen');
rc = lineout(instfile, '     Felix Maschek');
rc = lineout(instfile, '     Chris McKillop');
rc = lineout(instfile, '     Nenad Milenkovic');
rc = lineout(instfile, '     Markus Montkowski');
rc = lineout(instfile, '     Michal Necasek');
rc = lineout(instfile, '     Ondrej Necasek');
rc = lineout(instfile, '     Knut Stange Osmundsen');
rc = lineout(instfile, '     Vitali E. Pelenyov');
rc = lineout(instfile, '     Rene Pronk');
rc = lineout(instfile, '     David Raison');
rc = lineout(instfile, '     Carsten Tenbrink');
rc = lineout(instfile, '     Vit Timchishin');
rc = lineout(instfile, '     Joel Troster');
rc = lineout(instfile, '     Jens Wiessner');
rc = lineout(instfile, '     Vince Vielhaber');
rc = lineout(instfile, '     All the WINE people');
rc = lineout(instfile, '');
rc = lineout(instfile, '</TEXT>');
rc = lineout(instfile, '</PAGE>');
rc = lineout(instfile, '<PAGE INDEX=2 TYPE=README>');
rc = lineout(instfile, '<NEXTBUTTON TARGET=3>~I agree</NEXTBUTTON>');
rc = lineout(instfile, '<TEXT>');
rc = lineout(instfile, 'By pressing the "I agree" button, you agree to all terms and conditions to the below licence agreement.');
rc = lineout(instfile, '</TEXT>');
rc = lineout(instfile, '<README>');
rc = lineout(instfile, 'Project Odin Software License');
rc = lineout(instfile, '');
rc = lineout(instfile, 'You may, without charge, royalty, or other payment, copy and');
rc = lineout(instfile, 'distribute copies of this work and derivative works of this work');
rc = lineout(instfile, 'in source or binary form provided that:');
rc = lineout(instfile, '(1) you appropriately publish on each copy an appropriate');
rc = lineout(instfile, 'copyright notice; (2) faithfully reproduce all prior copyright');
rc = lineout(instfile, 'included in the original work; and (3) agree to indemnify and');
rc = lineout(instfile, 'hold all prior authors, copyright holders and licensors of the');
rc = lineout(instfile, 'work harmless from and against all damages arising from use of');
rc = lineout(instfile, 'or the inability to use the work.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'You may distribute binaries and sources of derivative works of');
rc = lineout(instfile, 'the work provided that: (1) all modifications to the original');
rc = lineout(instfile, 'source code are publically available; (2) all modifications are');
rc = lineout(instfile, 'released under this license; (3) all copyrights of the original');
rc = lineout(instfile, 'work are included; (4) the modifications are a direct advantage');
rc = lineout(instfile, 'for the functionality and completeness of the product or improve');
rc = lineout(instfile, 'interoperability of the product with another product and do not');
rc = lineout(instfile, 'interfere with the goal of an open source and free emulation');
rc = lineout(instfile, 'product.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'You may NOT use the work as base of another work that is');
rc = lineout(instfile, 'not released under this license agreement meaning that you');
rc = lineout(instfile, 'especially are not allowed to develop any other product which');
rc = lineout(instfile, 'is based on the sources and/or binaries of the work.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'However, you are encouraged to use the work for porting your');
rc = lineout(instfile, 'free or commercial applications. You can ship software');
rc = lineout(instfile, 'converted using the work as long as the original copyright');
rc = lineout(instfile, 'for the work is mentioned and you do not charge for');
rc = lineout(instfile, 'using any part of the work. You may also ship the work in');
rc = lineout(instfile, 'binaries and sources for use with your applications as long');
rc = lineout(instfile, 'as you do not violate the distribution regulations of this license.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'These license terms can be amended by a custom license');
rc = lineout(instfile, 'agreement between a third party and the administration');
rc = lineout(instfile, 'team. Such a custom license might only limit the above terms');
rc = lineout(instfile, 'if the changes cause a direct advantage for the functionality');
rc = lineout(instfile, 'or completeness of the product and do not interfere with the');
rc = lineout(instfile, 'goal of an open-source and free emulation product.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'Parts of the work may be governed by a license other than');
rc = lineout(instfile, 'this one, if the source directory of the part contains a');
rc = lineout(instfile, 'license file called "LICENSE.TXT" of its own. If a different');
rc = lineout(instfile, 'license exists, it supercedes this license. Removing the');
rc = lineout(instfile, 'the part specific license will automatically put back the');
rc = lineout(instfile, 'part under the terms of this license. Only the Odin management');
rc = lineout(instfile, 'team is allowed to put parts of the work under a different');
rc = lineout(instfile, 'license and remove the part specific licenses. The purpose');
rc = lineout(instfile, 'of using different licenses is solely for allowing code');
rc = lineout(instfile, 'sharing of certain parts of the work with other projects that');
rc = lineout(instfile, 'have incompatible licenses. These parts should either have');
rc = lineout(instfile, 'this license in a version that makes it compatible with the');
rc = lineout(instfile, 'license of the second project, or the license of the second');
rc = lineout(instfile, 'project as long as it conforms to the goals of an open source');
rc = lineout(instfile, 'and free emulation software.');
rc = lineout(instfile, '');
rc = lineout(instfile, 'Warranty');
rc = lineout(instfile, '');
rc = lineout(instfile, 'EXCEPT AS OTHERWISE RESTRICTED BY LAW, THIS WORK IS PROVIDED');
rc = lineout(instfile, 'WITHOUT ANY EXPRESSED OR IMPLIED WARRANTIES OF ANY KIND, INCLUDING');
rc = lineout(instfile, 'BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A');
rc = lineout(instfile, 'PARTICULAR PURPOSE, MERCHANTABILITY OR TITLE.  EXCEPT AS');
rc = lineout(instfile, 'OTHERWISE PROVIDED BY LAW, NO AUTHOR, COPYRIGHT HOLDER OR');
rc = lineout(instfile, 'LICENSOR SHALL BE LIABLE TO YOU FOR DAMAGES OF ANY KIND, EVEN IF');
rc = lineout(instfile, 'THEY HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.');
rc = lineout(instfile, '</README>');
rc = lineout(instfile, '</PAGE>');
rc = lineout(instfile, '<PAGE INDEX=3 TYPE=README>');
rc = lineout(instfile, '<NEXTBUTTON TARGET=4>~Next</NEXTBUTTON>');
rc = lineout(instfile, '<TEXT>');
rc = lineout(instfile, 'List of changes and bugfixes for this Odin daily build ('date()')');
rc = lineout(instfile, '</TEXT>');
rc = lineout(instfile, '<README>');
/* Insert ChangeLog */
changelog = '..\..\ChangeLog'
logline = linein(changelog);
do while lines(changelog) > 0
rc = lineout(instfile, logline);
logline = linein(changelog);
end
call stream changelog, 'c', 'close';

rc = lineout(instfile, '</README>');
rc = lineout(instfile, '</PAGE>');
rc = lineout(instfile, '<PAGE INDEX=4 TYPE=CONTAINER>');
rc = lineout(instfile, '<NEXTBUTTON TARGET=5>~Next</NEXTBUTTON>');
rc = lineout(instfile, '<TEXT>');
rc = lineout(instfile, 'Please select the packages which are to be installed. You may change the target paths for the packages.');
rc = lineout(instfile, '</TEXT>');
rc = lineout(instfile, '</PAGE>');
rc = lineout(instfile, '<PAGE INDEX=5 TYPE=CONFIGURE>');
rc = lineout(instfile, '<NEXTBUTTON TARGET=6>~Next</NEXTBUTTON>');
rc = lineout(instfile, '<TEXT>');
rc = lineout(instfile, 'Please select additional configuration that WarpIN should perform after installing this archive.');
rc = lineout(instfile, '</TEXT>');
rc = lineout(instfile, '</PAGE>');
rc = lineout(instfile, '<PAGE INDEX=6 TYPE=TEXT>');
rc = lineout(instfile, '<NEXTBUTTON TARGET=0>I~nstall</NEXTBUTTON>');
rc = lineout(instfile, '<TEXT>');
rc = lineout(instfile, 'Press "Install" to begin installing Odin.</TEXT>');
rc = lineout(instfile, '</PAGE>');
rc = lineout(instfile, '</BODY>');
rc = lineout(instfile, '</WARPIN>');
rc = lineout(instfile, '');
rc = lineout(instfile);
/* rc = SysFileDelete(instfile); */

parse arg cmdline

curdir = directory();
if cmdline = "debug"
then
do
	dlldir = "bin\debug";
	installarchive = "odin32bin-"date('S')"-debug"
end
else
do
	dlldir = "bin\release";
	installarchive = "odin32bin-"date('S')"-release"
end

maindir = "..\..";

"del "installarchive".wpi";

"copy ..\..\ChangeLog"
"wic.exe "curdir"\"installarchive" -a 1 ChangeLog";

call directory maindir;
"wic.exe "curdir"\"installarchive" -a 1 LICENSE.TXT";

call directory doc
"wic.exe "curdir"\"installarchive" -a 1 ChangeLog-1999 ChangeLog-2000 Readme.txt ReportingBugs.txt Logging.txt";
call directory ".."

call directory dlldir;
if cmdline = "debug"
then
do
"wic.exe "curdir"\"installarchive" -a 2 *.dll *.sym pe.exe pec.exe odininst.exe regsvr32.exe *.ini win32k.sys win32k.ddp Win32kCC.exe kRx.exe";
end
else
do
"wic.exe "curdir"\"installarchive" -a 2 *.dll pe.exe pec.exe odininst.exe regsvr32.exe *.ini win32k.sys win32k.ddp Win32kCC.exe kRx.exe";
end

call directory "Glide"
"wic.exe "curdir"\"installarchive" -a 3 *.dll";

call directory "Voodoo1"
"wic.exe "curdir"\"installarchive" -a 4 *.dll";

call directory "..\Voodoo2"
"wic.exe "curdir"\"installarchive" -a 5 *.dll";

call directory curdir;

/* add script */
"wic.exe "curdir"\"installarchive" -s "curdir"\odininst.wis"

exit;

GetBootDrive: procedure
    parse upper value VALUE( "PATH",, "OS2ENVIRONMENT" ) with "\OS2\SYSTEM" -2 boot_drive +2
return boot_drive

