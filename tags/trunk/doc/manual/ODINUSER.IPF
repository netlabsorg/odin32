:userdoc.:title.Project Odin User's Manual
:docprof toc=123.
.***************************************************
.*** IPF-Generator Version 0.82 for Framework IV ***
.***************************************************

:h1 res=100 x=right y=bottom width=70% height=100%.Introduction

:font facename='Default' size=0x0.:artwork align=center name='E:\DEV\INF\ODINDOC\ODIN.BMP' .
:lines align=center.:font facename='Helv' size=8x4.:hp2.User's Manual Release 1 (2002&slr.04&slr.14):elines.:font facename='Default' size=0x0.
:p.:ehp2.Odin is a free Open Source project (read about license and warranty :link reftype=hd res=600.here:elink.) with
the goal to eventually provide a fully working
Win95&slr.98&slr.NT&slr.2000&slr.XP runtime environment. This will make it
possible to run your favorite Windows programs under OS&slr.2 (eCS) as if they
were native.

:p.This document's intention is to provide an easy installation guide for Odin
itself and some basic concepts for trying to install and run
Win95&slr.98&slr.ME&slr.NT&slr.2000&slr.XP programs on OS&slr.2 (eCS).

:p.Win95&slr.98&slr.ME&slr.NT&slr.2000&slr.XP programs will be referred to as
Win32 programs in this document as that is what Microsoft calls this
programming model.

:p.This document will not cover all technical details and it is not supposed to
do so.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Odin is still work in progress. It is not complete in any way!

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Many (not to say most) programs still do not run with Odin&comma.
but the number of programs that run or partly run has increased dramatically
during the last months.
:p.[Home] [Previous] :link reftype=hd res=200.:hp2.[Next]:elink.:ehp2. 
:i1.purpose of this document
:i1.Win95&slr.98&slr.ME&slr.NT&slr.2000&slr.XP
:i1.What is Odin
:i1.Status of Odin

:h1 res=200 x=right y=bottom width=70% height=100%.How does Odin work?

:font facename='Default' size=0x0.:p.Odin has 2 main tasks to accomplish&colon.
:ol.
:li.:link reftype=hd res=220.Make OS&slr.2 (eCS) load Win32 programs:elink..
:li.:link reftype=hd res=230.Provide an environment:elink. for Win32 programs where they can be
executed.
:eol.
:p.Both tasks are described on the following pages.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=100.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=220.:hp2.[Next]:elink.:ehp2. 

:h2 res=220 x=right y=bottom width=70% height=100%.Make OS/2 load Win32 programs

:font facename='Default' size=0x0.:p.OS&slr.2 (eCS) uses a different binary format for program files
than Windows. Because OS&slr.2 (eCS) does not understand this
Windows format it does not know how to load it.
:p.There are 3 possibilities implemented in Odin in order to load
Win32 programs in OS&slr.2 (eCS)&colon.
:ol.
:li.:link reftype=hd res=222.Win32K.SYS:elink.
:li.:link reftype=hd res=223.PE.EXE&slr.PEC.EXE:elink.
:li.:link reftype=hd res=224.XX2LX.EXE (formerly known as PE2LX.EXE):elink.
:eol.
:p.:hp5.:hp2.Exkurs&colon.:ehp5.
:p.:ehp2.It might be of interest to know the internal name of these binary
format types. However it is not needed for using Odin.
:ul compact.
:li.:link reftype=fn refid=31.LX:elink.&colon. 32 bit OS&slr.2 (eCS) executable
:li.:link reftype=fn refid=31.PE:elink.&colon. 32 bit Windows executable
:li.:link reftype=fn refid=31.NE:elink.&colon. 16 bit Windows and 16 bit OS&slr.2
:eul.
:p.It is quite easy to determine which type of executable a binary
is&colon. Take a look at the file contents (using a file viewer)
and usually (not always) you will find one of the above types in
capitalized letters within the first 100 bytes or so.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=220.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=222.:hp2.[Next]:elink.:ehp2. 
:fn id=31.:p.LX = :hp5.L:ehp5.inear e:hp5.X:ehp5.ecutable
:p.PE = :hp5.P:ehp5.ortable :hp5.E:ehp5.xecutable
:p.NE = :hp5.N:ehp5.ew :hp5.E:ehp5.xecutable (funny&comma. isn't it?)
:efn.
:i1.loading Win32 programs
:i1.executable types

:h3 res=222 x=right y=bottom width=70% height=100%.Win32K.SYS

:font facename='Default' size=0x0.:p.Win32K.SYS acts as an extension to the OS&slr.2 (eCS) kernel to
make OS&slr.2 (eCS) think Win32 programs are native. It provides
a way to seamlessly load a Win32 program. If you have Win32K.SYS
installed you should not notice any difference between OS&slr.2
(eCS) and Windows programs in respect of loading a program
whether from a command line or WPS object.
:p.:hp8.WARNING! Bugs in Win32K.SYS may crash and&slr.or corrupt your
entire system! Make sure you have backed up important stuff
first!
:p.Team Odin is not responsible for ANYTHING AT ALL!
:p.Before you install Win32K.SYS make sure that you have a backdoor
to your system which works. For example try to boot your system
using the Alt&plus.F1 during bootup and choose Commandline boot
(F2). Make sure you have an textmode editor which works; for
example TEDIT.EXE.:ehp8.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Win32K.SYS :link reftype=hd res=460.parameters:elink. are set automatically when
installing a weekly build.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Win32K.SYS :link reftype=hd res=460.parameters:elink. can be configured using Win32K
Control Center in the Odin folder.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.If you are using a recent testcase kernel and you
experience a trap (black screen of death) in module GRACE then it
is Win32K.SYS. The following programs are currently causing such
traps with Win32K.SYS loaded&colon.
:ul compact.
:li.The old (but nevertheless quite often found) IBM Installer
(PMView 2000&comma. Netscape Communicator 4.61 and Lotus
SmartSuite come with it). The files of this installer start with
the letters "EPFI".
:li.Stardock's Object Packages from Object Desktop.
:li.There may be others as well...
:eul.
:p.Remove Win32K.SYS from your :font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0. and reboot before trying
to run one of these programs.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=220.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=223.:hp2.[Next]:elink.:ehp2. 
:i1.Win32K.SYS

:h3 res=223 x=right y=bottom width=70% height=100%.PE.EXE/PEC.EXE

:font facename='Default' size=0x0.:p.PE.EXE can be used if :link reftype=hd res=222.Win32K.SYS:elink. is not installed. It has to
precede the executable name of every Win32 program that is going
to be run. For example&colon.
If a Win32 program executable is called REALPLAY.EXE then your
commandline will look like the following&colon.
:p.:font facename='Courier' size=14x8.PE.EXE REALPLAY.EXE :font facename='Default' size=0x0.(or:font facename='Courier' size=14x8. PE REALPLAY).:font facename='Default' size=0x0.
:p.:font facename='Courier' size=14x8.PEC.EXE:font facename='Default' size=0x0. is a near relative of :font facename='Courier' size=14x8.PE.EXE:font facename='Default' size=0x0.. It is used the same
way&comma. despite it does not display textmode programs in a
separate window but in an OS&slr.2 window or fullscreen session.
This is handy for Win32 commandline programs.
:p.A WPS program object using PE.EXE would look like this&colon.
:p.
:artwork align=center name='E:\DEV\INF\ODINDOC\PROGOBJ.BMP' .
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Your program object will have the OS&slr.2 (eCS)
standard icon. You have to assign your favorite icon manually.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.PE.EXE searches for executables (.EXE&comma. .DLL) in
the following locations&colon.
:ol compact.
:li.Current directory
:li.:font facename='Courier' size=14x8.WINDOWSPATH:font facename='Default' size=0x0. (PATH-like Odin specific environment variable.)
:li.:font facename='Courier' size=14x8.PATH:font facename='Default' size=0x0.
:eol.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.It is not recommended to use :font facename='Courier' size=14x8.PE.EXE:font facename='Default' size=0x0. if :link reftype=hd res=222.Win32K.SYS:elink. is
installed. However from practical experience I found that using
:font facename='Courier' size=14x8.PE.EXE:font facename='Default' size=0x0. if :link reftype=hd res=222.Win32K.SYS:elink. is installed in PE-mode does not seem to
hurt. Nevertheless it is not recommended.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=222.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=224.:hp2.[Next]:elink.:ehp2. 
:i1.PE.EXE
:i1.PEC.EXE
:i1.PATH
:i1.WINDOWSPATH

:h3 res=224 x=right y=bottom width=70% height=100%.XX2LX.EXE

:font facename='Default' size=0x0.:p.PE2LX is called XX2LX.EXE in recent builds. The use of PE2LX (aka
Xx2LX) is not recommended anymore&comma. especially not for
endusers.
:p.PE2LX has to be used on every single binary file that comes with
a Win32 program before the program can be used on OS&slr.2 e.g.
you have to convert every binary on a file per file basis from
Windows PE format to OS&slr.2 LX format using PE2LX.EXE.
:p.You will not find any further information upon PE2LX (aka Xx2LX)
in this document. The best advice is - do not use it!
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=223.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=230.:hp2.[Next]:elink.:ehp2. 
:i1.PE2LX.EXE
:i1.XX2LX.EXE 

:h2 res=230 x=right y=bottom width=70% height=100%.Provide an environment for Win32 programs

:font facename='Default' size=0x0.:p.After the Win32 executable is loaded it needs an appropriate
environment.

:p.Odin provides the environment by adding many .DLLs to OS&slr.2
(eCS) that a Win32 program expects to find under Windows so it
can make all the :link reftype=fn refid=16.API:elink. calls it has to. Those .DLLs that Odin
provides do their best to answer every possible call a Win32
program can make.

:p.And that's where the problems begin&colon. Odin's .DLLs are still
incomplete. Some :link reftype=fn refid=16.API:elink. calls work perfectly&comma. some only
partially and some fail. The number of :link reftype=fn refid=16.API:elink. calls that work
correctly grows with every build that is released.

:p.In addition&comma. Odin provides things for Win32 programs that
otherwise simply do not exist in OS&slr.2 (eCS). Odin adds&colon.
:ul compact.
:li.Specific directories&comma.
:li.the biggest part of the registry&comma.
:li.OLE (not fully implemented yet)&comma.
:li.and some more things.
:eul.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\BL_PIN.BMP' runin.:hp2.Conclusion&colon. :ehp2.If a Win32 program only uses calls and
facilities that are provided by Odin it will (probably) work. If
it wants to use things that are currently not present in Odin it
will fail.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.There are still Windows programs around that claim to
be 32 bit but contain portions of code that rely on 16 bit
Windows APIs. Such programs will not work with Odin. For 16 bit
installers look :link reftype=hd res=570.here:elink..
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=224.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=300.:hp2.[Next]:elink.:ehp2. 
:fn id=16.:p.API = Application Programming Interface:efn.
:i1.environment for Win32 programs
:i1.API calls quality

:h1 res=300 x=right y=bottom width=70% height=100%.Distribution of Odin

:font facename='Default' size=0x0.:p.First you need to decide how you are going to use Odin and how adventurous
you are.
:p.Odin binary builds come in several flavors. The main flavors are&colon.
:ul compact.
:li.:link reftype=hd res=320.:hp2.Weekly buildfiles:elink.:ehp2.&colon. These are intended to be stable builds for
end-users.
:li.:link reftype=hd res=330.:hp2.Daily buildfiles:elink.:ehp2.&colon. These are intended for developers and the adventurous
who prefer to be on the bleeding edge.
:eul.

:p.Both of these build types come in two sub-flavors&colon.

:ul compact.
:li.Release builds contain no debugging code and are generally smaller and
faster.
:li.Debug builds contain extensive debugging and logging code. Therefore you
should have :link reftype=fn refid=7.lots of free space:elink. on a drive on which you plan to run an Odin
debug build with :link reftype=hd res=550.logging enabled:elink..
:eul.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.You will often be asked to install a debug build to assist in
diagnosing the source of the defect if you report a problem with a release
build.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.If you need to run a debug build&comma. be aware that the logs
can be large. You need to ensure you have sufficient free disk space.
:p.For details about reporting bugs take a look at :link reftype=hd res=550.OdinBug:elink.&comma. the bug report
generator for Odin. It has an extensive on-line help which covers every
detail about reporting bugs.
:p.Of course&comma. if all of the above options are all too tame&comma. you can
always build Odin from source.

:p.That's one of the beauties of Open Source.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=230.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=320.:hp2.[Next]:elink.:ehp2. 
:i1.flavors of Odin
:i1.subflavors of Odin
:i1.release builds
:i1.debug builds

:h2 res=320 x=right y=bottom width=70% height=100%.Weekly builds

:font facename='Default' size=0x0.:p.The weekly builds are considered to be (relatively) stable builds for
endusers.

:p.They come in WarpIN .WPI file format.

:p.These are the recommended buildfiles for the novice Odin user. It is
essential that your first installation of Odin is done with a weekly build.
It is not important whether this first installation is done with a release or
a debug build.

:p.In order to install these weekly builds you will have to get :link reftype=hd res=430.WarpIN:elink.&comma.
the popular installer kit done by Mr. xWorkplace&comma. Ulrich M&oe.ller.
:link reftype=hd res=430.WarpIN:elink. can be found :link reftype=fn refid=5.here:elink..

:p.The usage of WarpIN for the first installation of Odin is recommended&comma.
because the usage of WarpIN ensures that a few basics are done right.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Be careful when downloading .WPI files with Netscape. By default
Netscape considers .WPI files as text files and damages them. There is a
little program that comes with :link reftype=hd res=430.WarpIN:elink. named Wizilla.EXE. It has to be run
once in order to make Netscape recognize .WPI files as binaries (and download
them correctly!).
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=300.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=330.:hp2.[Next]:elink.:ehp2. 
:i1.Weekly builds
:i1.WPI builds
:i1.stable builds

:h2 res=330 x=right y=bottom width=70% height=100%.Daily builds

:font facename='Default' size=0x0.:p.Daily builds come in the traditional .ZIP file format.
:p.They have to be installed manually or with a little tool named
OdinTool available in the file area of the odinusers list at
YahooGroups.
:p.The .ZIP file should be unpacked in the &bsl.Odin directory. When
unZIPping be sure not to trash directories&comma. because the
build files have an appropriate directory structure stored
inside.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.You have to run :link reftype=hd res=520.:font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0.:elink. after unZIPping a daily
build in order to update the directory tree and registry.

:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=320.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=340.:hp2.[Next]:elink.:ehp2. 
:i1.Daily builds
:i1.ZIP builds

:h2 res=340 x=right y=bottom width=70% height=100%.Getting an Install Kit

:font facename='Default' size=0x0.:p.The recommended location to get Odin builds is from
:ul compact.
:li.:link reftype=fn refid=1.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.daily:elink.
:li.:link reftype=fn refid=2.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.weekly:elink.
:eul.
:p.However&comma. since no service is available 24 hours 7 days a
week&comma. an alternative source is
:ul compact.
:li.:link reftype=fn refid=3.ftp&colon.&slr.&slr.ftp.netlabs.org&slr.pub&slr.odin&slr.daily:elink.
:li.:link reftype=fn refid=4.ftp&colon.&slr.&slr.ftp.netlabs.org&slr.pub&slr.odin&slr.weekly:elink.
:eul.

:p.Both Daily and Weekly builds are named based on the build date and style&colon.
:lines align=center.:font facename='Courier' size=14x8.
odin32bin-YYYYMMDD-STYLE.FORMAT
:elines.:font facename='Default' size=0x0.
:p.Now&comma. what do these stand for&colon.
:sl.
:li.YYYYMMDD is the build date.  
:li.STYLE is the build style and will be one of&colon.
:sl compact.
:li."debug" for the debug builds
:li."release" for the release builds
:esl.
:li.FORMAT is the build type
:sl compact.
:li."WPI" for the weekly builds in :link reftype=hd res=430.WarpIN:elink. format
:li."ZIP" for the daily builds in ZIP format
:esl.
:esl.
:p.For example&colon.
:lines align=center.:font facename='Courier' size=14x8.
odin32bin-20020214-debug.wpi
:elines.:font facename='Default' size=0x0.
:p.indicates the weekly debug build from 02-14-2002.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=330.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=400.:hp2.[Next]:elink.:ehp2. 
:fn id=1.:p.:link reftype=launch object='NETSCAPE.EXE' data='ftp://ftp.os2.org/odin/daily'.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.daily:elink.:efn.
:fn id=2.:p.:link reftype=launch object='NETSCAPE.EXE' data='ftp://ftp.os2.org/odin/weekly'.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.weekly:elink.:efn.
:fn id=3.:p.:link reftype=launch object='NETSCAPE.EXE' data='ftp://ftp.netlabs.org/pub/odin/daily'.ftp&colon.&slr.&slr.ftp.netlabs.org&slr.pub&slr.odin&slr.daily:elink.:efn.
:fn id=4.:p.:link reftype=launch object='NETSCAPE.EXE' data='ftp://ftp.netlabs.org/pub/odin/weekly'.ftp&colon.&slr.&slr.ftp.netlabs.org&slr.pub&slr.odin&slr.weekly:elink.:efn.
:i1.naming scheme
:i1.build names
:i1.where to get builds
:i1.downloading Odin builds
:i1.ftp.os2.org
:i1.ftp.netlabs.org
:i1.netlabs
:i1.os2.org

:h1 res=400 x=right y=bottom width=70% height=100%.Installing Odin

:font facename='Default' size=0x0.:p.Odin is easy to install. However&comma. there are enough options that you
should do a little planning before you start. So here is a short cookbook with
links to get the appropriate details for every step&colon.
:ol.
:li.Check whether your system meets the :link reftype=hd res=420.requirements:elink. for installing Odin.
:li.Decide whether to use a
:ol compact.
:li.:link reftype=hd res=320.weekly build:elink. with :link reftype=fn refid=21.release:elink. or :link reftype=fn refid=22.debug:elink. flavor.
:li.:link reftype=hd res=330.daily build:elink. with :link reftype=fn refid=21.release:elink. or :link reftype=fn refid=22.debug:elink. flavor.
:eol.
:li.Get :link reftype=hd res=340.the desired build:elink..
:li.Get and install :link reftype=hd res=430.WarpIN:elink..
:li.Install the :link reftype=hd res=440.weekly:elink. or :link reftype=hd res=450.daily:elink. build.
:eol.
:p.You are done!
:p.Subsequent builds can be installed over the existing one&comma. there is no
need to uninstall (neither for weekly nor for daily builds). It is no problem
at all to install a daily build over a weekly build and vice versa.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=340.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=420.:hp2.[Next]:elink.:ehp2. 

:fn id=21.:p.Release builds are considered stable builds for endusers. They are smaller and
faster than debug builds.:p.They :hp5.:hp2.cannot:ehp5.:ehp2. be used to report bugs.:efn.
:fn id=22.:p.Debug builds contain extensive debugging and logging code. They are bigger and
slower than release builds.:p.They :hp5.:hp2.must:ehp5.:ehp2. be used to report bugs.:efn.

:i1.cookbook
:i1.installation cookbook
:i1.subsequent installations
:i1.installing over previous installations

:h2 res=420 x=right y=bottom width=70% height=100%.Requirements

:font facename='Default' size=0x0.:p.Your system must meet the following specifications in order to be able to
install Odin successfully&colon.
:ol.
:li.The :hp2.recommended operating system version:ehp2. is any flavor of OS&slr.2 version 4.5
:ul compact.
:li.OS&slr.2 Warp 4 with fixpack 13 or better (15 recommended)
:li.OS&slr.2 Warp 4.5x (MCP1 or MCP2)
:li.OS&slr.2 Warp Server for eBusiness
:li.eComStation
:eul.
:p.When using OS&slr.2 Warp 4.0 (e.g. with a fixpack level prior to 13) it should
at least be at fixpack level 5.
:p.OS&slr.2 Warp 3 is not officially supported&comma. but is reported to work by
using Warp 4 Open32 dlls and patching them.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.The reason why OS&slr.2 version 4.5 is recommended and not version
4.0 with fixpack level of 5 or better is the infamous 512 MB virtual memory
limit per process which has been :link reftype=fn refid=13.lifted:elink. to 3072 MB in fixpack 13 or better
(e.g. OS&slr.2 version 4.5).
:li.You must have OS&slr.2 with :hp2.Open32&slr.DAX&slr.DAPIE:ehp2. installed. This is (was?)
an option during the initial installation process of OS&slr.2.
:p.Check for the presence of the following files on your system in order to
determine whether Open32&slr.DAX&slr.DAPIE is installed&colon.
:ul compact.
:li.:font facename='Courier' size=14x8.&bsl.OS2&bsl.DLL&bsl.PMWINX.DLL :font facename='Default' size=0x0.(main DLL&comma. although Odin does not need
it anymore)
:li.:font facename='Courier' size=14x8.&bsl.OS2&bsl.DLL&bsl.REGISTRY.DLL :font facename='Default' size=0x0.(this one and all the following are required
by Odin)
:li.:font facename='Courier' size=14x8.&bsl.OS2&bsl.SYSTEM&bsl.USER.DAT
:li.&bsl.OS2&bsl.SYSTEM&bsl.SYSTEM.DAT
:eul.:font facename='Default' size=0x0.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.There are a few more files that belong to
Open32&slr.DAX&slr.DAPIE. However there have been some changes during its
history and some of these files are :link reftype=fn refid=19.obsolete:elink. today.
:p.Generally speaking&colon. If you can run Lotus SmartSuite for OS&slr.2 you can
run Odin&comma. too.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.There is a :link reftype=fn refid=15.defect:elink. in the Warp4 default.RSP file that affects CID
installs. This is why CID installs often will not support Open32. 
:li.You must install Odin on a :hp2.partition that supports long filenames:ehp2. (e.g.
HPFS&slr.JFS&slr.FAT32). Odin will not work on FAT16 partitions because
OS&slr.2 cannot write long filenames on FAT16 partitions.
:li.Video is always an issue with Odin. You should run an up to date display
driver when installing (and using) Odin. The :hp2.SciTech Display Doctor Suite is
the recommended:ehp2. driver for Odin (both&comma. IBM version or SciTech's
SDD&slr.Pro). The native Matrox drivers are also good drivers for this
purpose. There is no recommended resolution or color depth&comma. however 16
bit color depth (e.g. 65.536 colors) is generally known to work best (personal
opinion of the :link reftype=fn refid=999.author:elink. based on reading lots of newsgroup and mailing list
articles).
:eol.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=400.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=430.:hp2.[Next]:elink.:ehp2. 
:fn id=15.
:p.As delivered&comma. the .RSP file is missing the entries&colon.
:ul compact.
:li.DAXCOMP1.Selection=1
:li.DAXCOMP1.TarDrv=x&colon.
:eul.
:p.where x&colon. is the install drive.  
:efn.
:fn id=19.:p.Among these are
:ul compact.
:li.all :font facename='Courier' size=14x8.NAME*.EXE:font facename='Default' size=0x0. files&comma.
:li.:font facename='Courier' size=14x8.boot.dat&comma. registry.dat:font facename='Default' size=0x0.
:eul.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Although these files are obsolete it is not recommended to delete
them!
:efn.
:fn id=13.:p.In order to enable the new memory limit per process put
:font facename='Courier' size=14x8.VIRTUALADDRESSLIMIT=3072:font facename='Default' size=0x0. into your :font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0.. Some Win32 applications
(especially Microsoft Office) are designed in a way that they refuse to
run&comma. if they do not have at least 2048 MB of virtual address space.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.This does not mean that this setting will enable you to run
Microsoft Office with Odin today. It just opens the possibility to do it in
the future.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.This setting has nothing to do with the amount of RAM you have
currently plugged into your system.
:efn.
:i1.OS&slr.2 version
:i1.Warp 3
:i1.Warp 4
:i1.Warp 4.5
:i1.Warp 4.51
:i1.Warp 4.52
:i1.WSeB
:i1.eComStation
:i1.FAT16
:i1.FAT32
:i1.JFS
:i1.HPFS
:i1.DAX
:i1.Open32
:i1.DAPIE
:i1.long filenames
:i1.prerequisites
:i1.video driver
:i1.display driver
:i1.SciTech Display Doctor
:i1.512 MB memory limit per process
:i1.VIRTUALADDRESSLIMIT

:h2 res=430 x=right y=bottom width=70% height=100%.Installing WarpIN

:font facename='Default' size=0x0.:p.WarpIN is required for installing Odin's weekly builds. Even if you plan on
using only daily builds&comma. which do not require WarpIN&comma. it is
strongly recommended you install WarpIN. It is quickly becoming a preferred
OS&slr.2 installer for complex applications.



:p.In addition the first installation of Odin on your machine should be a weekly
build.
:ol.
:li.:hp2.Getting a WarpIN Install Kit
:p.:ehp2.You can always get the latest WarpIN installer kit from
:link reftype=fn refid=5.ftp&colon.&slr.&slr.ftp.os2.org&slr.xworkplace:elink..The most recent release of
WarpIN at the time of this writing is warpin-0-9-16.zip.
:li.:hp2.Choosing an Install Location

:p.:ehp2.The WarpIN developers have defined default locations for applications
installed by WarpIN&colon.
:ul compact.
:li.:font facename='Courier' size=14x8.&bsl.Tools:font facename='Default' size=0x0. - Tools and utilities
:li.:font facename='Courier' size=14x8.&bsl.Apps:font facename='Default' size=0x0. - Applications
:p.If you use these defaults&comma. you would want to install WarpIN to&colon.
:li.:font facename='Courier' size=14x8.&bsl.Tools&bsl.WarpIN:font facename='Default' size=0x0.
:eul.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.These defaults can be overridden. If you don't like WarpIN's
defaults&comma. you can change them after WarpIN is installed.
:li.:hp2.Installing:ehp2. 
:ul compact.
:li.Create your target directory for WarpIN.
:li.Unzip the WarpIN package to this directory (no temporary directory needed)
:li.Run WarpIN.EXE
:eul.
:p.This will create a WarpIN folder and register the WarpIN class so that you
can use the usual Workplace Shell (WPS) features to activate the installer.
:p.WarpIN makes no changes to :font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0.. A reboot is not necessary.
:eol.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=420.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=440.:hp2.[Next]:elink.:ehp2. 
:fn id=5.:p.:link reftype=launch object='NETSCAPE.EXE' data='ftp://ftp.os2.org/xworkplace'.ftp&colon.&slr.&slr.ftp.os2.org&slr.xworkplace:elink.:efn.
:i1.installing WarpIN

:h2 res=440 x=right y=bottom width=70% height=100%.Installing a weekly build

:font facename='Default' size=0x0.:p.This is a step-by-step install cookbook for the end user&colon.
:ol.
:li.Make sure you have :link reftype=hd res=430.:hp2.WarpIN:elink. installed:ehp2. as described :link reftype=hd res=430.here:elink..
:li.:hp2.Get an Odin weekly build:ehp2. as described :link reftype=hd res=340.here:elink..
:li.:hp2.Decide where:ehp2. to install it.
:p.The default path that is proposed by the developers is
:link reftype=fn refid=27.C&colon.&bsl.Odin:elink.&comma. but this can be overridden.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.You should install Odin on a partition with plenty of free space.
You might need to create a debug logfile and these can become quite :link reftype=fn refid=7.large:elink..
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Do not (never&comma. never&comma. never) install Odin into your
real Windows installation. The result will be that your Windows installation
completely damaged and Odin will not work!
:li.:hp2.Decide how:ehp2. you want to run&colon.
:ul compact.
:li.Device Driver (:link reftype=hd res=222.Win32K.SYS:elink.&comma. optionally :link reftype=hd res=460.parameters:elink. can be changed manually
afterwards)
:li.With or without :font facename='Courier' size=14x8.PATH:font facename='Default' size=0x0. and :font facename='Courier' size=14x8.LIBPATH:font facename='Default' size=0x0. updates. :font facename='Courier' size=14x8.:link reftype=fn
refid=23.:hp1.x:elink.:ehp1.&colon.&bsl.ODIN&bsl.SYSTEM32:font facename='Default' size=0x0.
should be both in your :font facename='Courier' size=14x8.PATH:font facename='Default' size=0x0. and :font facename='Courier' size=14x8.LIBPATH:font facename='Default' size=0x0..
:li.With or without :link reftype=hd res=223.PE.EXE:elink..
:eul.
:li.:hp2.Install Odin:ehp2. buildfile using :link reftype=hd res=430.WarpIN:elink..
:li.:hp2.Reboot:ehp2. if needed.
:li.:hp2.Run :link reftype=hd res=540.:font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0.:elink.:ehp2. to verify&comma. that basics went well&comma. as described:hp2. :link reftype=hd res=540.:ehp2.here:elink..
:eol.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.In case you get an error message from :link reftype=hd res=222.Win32K.SYS:elink.
which tells you that Win32K.SYS failed to load&comma. the most
probable reason is a missing or inadequate symbol file. Read on
:link reftype=hd res=470.here:elink. in order to resolve this problem.

:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=430.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=450.:hp2.[Next]:elink.:ehp2. 
:fn id=7.:p.The debug files grow rapidly during the execution of a program. 40-50 MB for 5
minutes running RealPlayer 8 is not unusual.:efn.
:fn id=27.C&colon. does not represent your bootdrive here.:efn.
:fn id=23.:p.x = the drive you installed Odin on.:efn.
:i1.installing Odin
:i1.installing weekly builds
:i1.installing WPI builds

:h2 res=450 x=right y=bottom width=70% height=100%.Installing a daily build

:font facename='Default' size=0x0.:p.This is a Bleeding Edge Install Cookbook for the more adventurous users&colon.
:ol.
:li.:hp2.Get an Odin daily build:ehp2. (.ZIP file) as described :link reftype=hd res=340.here:elink..
:li.:hp2.Decide where:ehp2. to install it
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.You should install Odin on a partition with plenty of free space.
You might want to create a debug logfile and these can become quite :link reftype=fn refid=7.large:elink..
:li.:hp2.Unzip the build:ehp2. to the install directory
:li.:hp2.Update :font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0. if needed. You might have to update&colon.
:ul compact.
:li.:font facename='Courier' size=14x8.:ehp2.DEVICE=:link reftype=fn refid=23.:hp1.x:elink.:ehp1.&colon.&bsl.ODIN&bsl.SYSTEM32&bsl.:link reftype=hd res=222.Win32K.SYS:elink.:font facename='Default' size=0x0. [:link reftype=hd res=460.parameters:elink.]
:li.Your :font facename='Courier' size=14x8.PATH (:link reftype=fn refid=23.:hp1.x:elink.:ehp1.&colon.&bsl.ODIN&bsl.SYSTEM32:font facename='Default' size=0x0. should be added to the end of your :font facename='Courier' size=14x8.PATH:font
facename='Default' size=0x0.)
:li.Your :font facename='Courier' size=14x8.LIBPATH (:link reftype=fn refid=23.:hp1.x:elink.:ehp1.&colon.&bsl.ODIN&bsl.SYSTEM32:font facename='Default' size=0x0. should be added to the end of your :font facename='Courier' size=14x8.LIBPATH:font
facename='Default' size=0x0.)
:li.Your :font facename='Courier' size=14x8.WINDOWSPATH (:link reftype=fn refid=23.:hp1.x:elink.:ehp1.&colon.&bsl.ODIN:font facename='Default' size=0x0. and :link reftype=fn refid=23.:hp1.x:elink.:ehp1.&colon.&bsl.ODIN&bsl.SYSTEM32:font facename='Default'
size=0x0. should be added)
:eul.
:li.:hp2.Reboot:ehp2. if needed.
:li.:hp2.Run :link reftype=hd res=520.:font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0.:elink. :ehp2.to initialize the Open32 Registry and :link reftype=hd res=530.:font facename='Courier' size=14x8.Odin.INI:font
facename='Default' size=0x0.:elink..
:li.:hp2.Run :link reftype=hd res=540.:font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0.:elink. :ehp2.to verify.
:eol.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.In case you get an error message from :link reftype=hd res=222.Win32K.SYS:elink.
which tells you that Win32K.SYS failed to load&comma. the most
probable reason is a missing or inadequate symbol file. Read on
:link reftype=hd res=470.here:elink. in order to resolve this problem.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=440.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=460.:hp2.[Next]:elink.:ehp2. 
:i1.installing ZIP builds
:i1.installing daily builds

:h2 res=460 x=right y=bottom width=70% height=100%.Important Win32K.SYS parameters

:font facename='Default' size=0x0.:p.Win32K.SYS understands a lot of parameters. This is a short
summary of the most important parameters needed for daily use of
Odin.

:p.The Capitalized letters are the one which is tested for when
identifying the argument. Blanks have to be inserted between
individual parameters&comma. but no blanks within a parameter. |
means OR. The "<" and ">" indicate a list of options and must be
omitted in the device driver statement.

:p.:hp2.Generally useful parameters:ehp2. for Win32K.SYS&colon.
:lines align=left.:font facename='Courier' size=12x6.
-Quiet             :font facename='Default' size=0x0.Quiet driver initiation.:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.Default&colon. Disabled (e.g. -V):font facename='Courier' size=14x8.

-Verbose         :font facename='Default' size=0x0.Verbose driver initiation. :font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.Default&colon. Enabled:font facename='Courier' size=14x8.

-S&colon.<filename>    :font facename='Default' size=0x0.Full path to the current OS&slr.2 kernel symbol file.:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.When this is specified the given symbol "database":font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.and default locations are not searched. Continue:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.:link reftype=hd res=470.here:elink. to learn more about symbol files.:font facename='Courier' size=14x8.
:elines.:font facename='Default' size=0x0.
:p.:hp2.Win32 programs loader parameters&colon.
:lines align=left.:font facename='Courier' size=12x6.
:ehp2.-PE&colon.<PE|PE2LX|MIXED|NOT>
                   :font facename='Default' size=0x0.Enable&slr.disable and set mode of the PE loading.:font facename='Courier' size=14x8.

                 PE&colon.     :font facename='Default' size=0x0.PE.EXE only. The program is started using :font facename='Courier' size=14x8.
                         :font facename='Default' size=0x0.PE.EXE and the process name will be PE.EXE.:font facename='Courier' size=14x8.
                         :font facename='Default' size=0x0.This is the recommended setting.:font facename='Courier' size=14x8.

                 PE2LX&colon.  :font facename='Default' size=0x0.PE2LX loader only.:font facename='Courier' size=14x8.

                 MIXED&colon.  :font facename='Default' size=0x0.Mix of PE2LX and PE.EXE. It uses PE2LX:font facename='Courier' size=14x8.
                         :font facename='Default' size=0x0.when possible and backs out to PE.EXE if :font facename='Courier' size=14x8.
                         :font facename='Default' size=0x0.the executable is not supported by the :font facename='Courier' size=14x8.
                         :font facename='Default' size=0x0.converter.:font facename='Courier' size=14x8.
                 NOT&colon.    :font facename='Default' size=0x0.Disable PE loading.:font facename='Courier' size=14x8.
:elines.:font facename='Default' size=0x0.
:p.:hp2.OS&slr.2 loader parameter&colon.
:lines align=left.:font facename='Courier' size=12x6.
:ehp2.-Dllfixes<&colon.E|&colon.D>   :font facename='Default' size=0x0.Enables or disables the long DLL name and non .DLL:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.extension fixes.:font facename='Courier' size=14x8.

                 :font facename='Default' size=0x0.Explanantion&colon. OS&slr.2 DLLs have been limited to 8 :font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.characters names since version 2.0. This is a very :font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.bad limitation. Also OS&slr.2 forces all DLL loaded at:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.loadtime to have the extension '.DLL'.:font facename='Courier' size=14x8.

                 :font facename='Default' size=0x0.Both these restrictions are removed if this setting is:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.enabled.:font facename='Courier' size=14x8.

                 :font facename='Default' size=0x0.Current state for this feature is EXPERIMENTAL.:font facename='Courier' size=14x8.
                 :font facename='Default' size=0x0.Default&colon. Enabled:font facename='Courier' size=14x8.

                 :font facename='Default' size=0x0.-Dllfixes can be used independently of the -PE parameter.:font facename='Courier' size=14x8.

:elines.:font facename='Default' size=0x0.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=450.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=470.:hp2.[Next]:elink.:ehp2. 
:i1.Win32K.SYS parameters (excerpt)

:h2 res=470 x=right y=bottom width=70% height=100%.Providing a symbol file for Win32K.SYS

:font facename='Default' size=0x0.:p.:link reftype=hd res=222.Win32K.SYS:elink. is not a "real" device driver&comma. but an extension
to the OS&slr.2 kernel. It integrates into the kernel by patching
it. In order to be able to patch the kernel&comma. it needs to
know the locations of routines within the kernel.
:p.As these locations change from build to build&comma. :link reftype=hd res=222.Win32K.SYS:elink.
needs to search the kernel to find these locations.

:p.When programmers write code&comma. they use symbols to represent
functions and data because it allows them to abstract the
process.  Once the compiled code is inside the computer&comma.
all we have is binary addresses.  The .sym files contain the data
needed to convert an address back to a symbolic name.
:p.Without this :link reftype=hd res=222.Win32K.SYS:elink. would have to debug in binary which just
slows the process down considerably. That is why :link reftype=hd res=222.Win32K.SYS:elink. needs
the .sym. files.

:p.Procedure in order to the provide the appropriate symbol
file&colon.
:ol.
:li.You should look into the following places&comma. depending on
what version of OS&slr.2 you are using (x&colon. is equivalent to
your boot drive)&colon.

:ul.
:li.OS&slr.2 Warp 4
:sl compact.
:li.x&colon.&bsl.os2&bsl.pdpsi&bsl.pmdf&bsl.warp4&bsl.os2krnlr.sym
:esl.
:li.OS&slr.2 Warp 4.5
:sl compact.
:li.x&colon.&bsl.os2&bsl.pdpsi&bsl.pmdf&bsl.warp45&bsl.os2krnlr.sym
:li.x&colon.&bsl.os2&bsl.pdpsi&bsl.pmdf&bsl.warp45_u&bsl.os2krnlr.sym
:li.x&colon.&bsl.os2&bsl.pdpsi&bsl.pmdf&bsl.warp45_s&bsl.os2krnlr.sym
:esl.
:li.Recent testcase kernels (OS&slr.2 Warp 4.5)
:sl compact.
:li.x&colon.&bsl.os2krnl.sym
:esl.
:li.OS&slr.2 Warp 3 (not officially supported)
:sl compact.
:li.x&colon.&bsl.os2&bsl.system&bsl.pmdf&bsl.os2krnlr.sym
:esl.
:eul.
:li.Now that you have found your symbol file&comma. copy it to
os2krnl.sym the in the root directory of your bootdrive. The
symbol file of the testcase kernels are already there. You might
also use the -S switch instead of copying the file.
:li.Reboot and test. If Win32K.SYS does not complain anymore&comma.
you are done!
:eol.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.The kernel and the symbol file must match&comma.
which means they must be of the same revision (a similar date and
time indicates a matching revision)&comma.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.When looking for the symbol files as described
above&comma. you might find files named os2krnld.sym and
os2krnlb.sym. They are for debug kernels. Do not use the
them&comma. unless you know what you are doing! 
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.The "r" in os2krnlr.sym stands for "retail".
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=460.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=500.:hp2.[Next]:elink.:ehp2. 
:i1.Win32K.SYS loading error
:i1.error loading Win32K.SYS
:i1.os2krnl.sym
:i1.os2krnlr.sym
:i1.symbol files

:h1 res=500 x=right y=bottom width=70% height=100%.Using Odin

:font facename='Default' size=0x0.:p.The following sections describe some important parts of
Odin&colon.
:ul.
:li.:link reftype=hd res=520.OdinInst.EXE:elink.
:li.:link reftype=hd res=530.Odin.INI:elink.
:li.:link reftype=hd res=540.OdinApp.EXE:elink.
:li.:link reftype=hd res=550.OdinBug.EXE:elink.
:li.:link reftype=hd res=560.OpenGL3dfx.ZIP:elink.
:li.:link reftype=hd res=570.Installing Win32 applications:elink.
:eul.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=440.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=520.:hp2.[Next]:elink.:ehp2. 

:h2 res=520 x=right y=bottom width=70% height=100%.Initializing Odin - OdinInst.EXE

:font facename='Default' size=0x0.:p.The :font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0. utility initializes or updates the Open32 registry keys and
:link reftype=hd res=530.:font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0.:elink.. Many Win32 programs depend on the existence of certain Registry
keys. :font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0. adds these keys to the Open32 Registry. In addition&comma.
:font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0. adds :link reftype=hd res=530.:font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0.:elink. key values&comma. as needed.

:p.If you suspect that you are missing some required values&comma. you can run
:font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0.. 

:p.What is more&comma. :font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0. creates the following tree&comma. which
mimics the Windows directory structure&comma. within the Odin folder&colon.
:cgraphic.
:hp2.      Odin
      ÃÄÄ All Users
      ³   ÀÄÄ Start Menu
      ³       ÀÄÄ Programs
      ³           ÀÄÄ StartUp
      ÃÄÄ Application Data
      ÃÄÄ Cookies
      ÃÄÄ Desktop
      ÃÄÄ Favorites
      ÃÄÄ Fonts
      ÃÄÄ History
      ÃÄÄ My Documents
      ÃÄÄ NetHood
      ÃÄÄ PrintHood
      ÃÄÄ Recent
      ÃÄÄ SendTo
      ÃÄÄ ShellNew
      ÃÄÄ Start Menu
      ³   ÀÄÄ Programs
      ³       ÀÄÄ Startup
      ÃÄÄ System32
      ³   ÀÄÄ Drivers
      ³       ÀÄÄ etc
      ÃÄÄ Temp
      ÀÄÄ Temporary Internet Files:ecgraphic.

:p.:ehp2.After installing manually from a .ZIP build it is essential to run
:font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0. once.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :font facename='Courier' size=14x8.:ehp2.OdinInst.EXE:font facename='Default' size=0x0. updates all registry entries with its own
defaults&comma. no matter if the user provided his own customized entries.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Odin and LotusSmartSuite share the same registry files. Lotus
SmartSuite comes with a program to reset the registry. This will also destroy
Odin's settings. After resetting the registry&comma. be sure to rerun
OdinInst.EXE.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.All current incarnations of :font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0. have a bug&colon.
:p.If :font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0. is typed ":font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0." on the commandline&comma. it might hang. Type
":font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0." instead of ":font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0." (not case sensitive) to avoid the hang. A
hanging :font facename='Courier' size=14x8.OdinInst:font facename='Default' size=0x0. can be terminated by pressing CTRL-BREAK twice. This bug is
under investigation.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=500.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=530.:hp2.[Next]:elink.:ehp2. 
:i1.OdinInst.EXE

:h2 res=530 x=right y=bottom width=70% height=100%.Configuring Odin - Odin.INI

:font facename='Default' size=0x0.:p.:font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0. stores a variety of configuration data that Odin uses to accommodate
differences between the OS&slr.2 and Win32 applications that may vary from
system to system. The file is created when you install Odin and&comma. if you
use the WarpIN installer&comma. it will be updated each time you install a new
build. If you install daily builds&comma. you will need to do the update
manually by running :link reftype=hd res=520.:font facename='Courier' size=14x8.:hp2.OdinInst:font facename='Default' size=0x0.:elink.:ehp2..EXE.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :font facename='Courier' size=14x8.:ehp2.Odin.INI:font facename='Default' size=0x0. is stored in the Odin&bsl.System32 directory. Older
builds placed :font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0. in the Odin directory - and if you find one
there&comma. delete it to avoid confusion. 
:p.:font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0. is a Windows 3.1-style INI file&comma. where the profile name has to
be in sqare brackets and the setting must come as settings name=settings
value. Currently the following settings can be configured in :font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0.&colon.
:lines align=left.:font facename='Courier' size=12x6.
:hp2.[COLORS]
:ehp2.UseWinColors      :font facename='Default' size=0x0.Use Windows colors:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.values&colon. 1&slr.0 (1 = Yes&comma. 0 = No):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default&colon. 1:font facename='Courier' size=14x8.
SaveColors      :font facename='Default' size=0x0.Save&slr.restore colors to&slr.from :font facename='Courier' size=14x8.Odin.INI:font facename='Default' size=0x0.:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.values&colon. 1&slr.0 (1 = Yes&comma. 0 = No):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default&colon. 0:font facename='Courier' size=14x8.
:hp2.[ODINSYSTEM]
:ehp2.HIGHMEM         :font facename='Default' size=0x0.Turns off high memory usage in Odin. Useful for Warp 4.5 systems:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.(FP#13 or better) that still use the old 16 bits TCP&slr.IP stack.:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.values&colon. 1&slr.0 (1 = Yes&comma. 0 = No):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default&colon. 1:font facename='Courier' size=14x8.
INSTALLDATE     :font facename='Default' size=0x0.This setting is updated everytime :link reftype=hd res=520.:font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0.:elink. is executed:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.:link reftype=hd res=550.OdinBug:elink. evaluates this when creating a bugreport.:font facename='Courier' size=14x8.
OS2Look         :font facename='Default' size=0x0.Creates an OS&slr.2 titlebar&comma. system menu&comma. min&slr.max&slr.close buttons.:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.Also overrides UseWinColors setting (uses OS&slr.2 colors).:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.values&colon. 1&slr.0 (1 = Yes&comma. 0 = No):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default  0
:elines.:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.OS2Look can interfere with windows applications that draw their
own custom titlebar. (e.g. RealPlayer). In addition OS2Look ignores some of
Styler&slr.2's titlebar changes.
:lines align=left.:font facename='Courier' size=14x8.
:hp2.[Font Mapping]:ehp2.  :font facename='Default' size=0x0.Maps MS-Windows font names to OS&slr.2 fonts.:font facename='Courier' size=14x8.
MS Sans Serif=WarpSans  (default)
MS Shell Dlg 2=WarpSans (default):font facename='Default' size=0x0.
:elines.:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Font names are case sensitive in OS&slr.2.
:lines align=left.:font facename='Courier' size=14x8.
:hp2.[WinVersion]
:ehp2.Version=Win98   :font facename='Default' size=0x0.-> Windows 98:font facename='Courier' size=14x8.
Version=WinME   :font facename='Default' size=0x0.-> Windows ME:font facename='Courier' size=14x8.
Version=NT4     :font facename='Default' size=0x0.-> Windows NT4  SP6 :font facename='Courier' size=14x8.
Version=Win2000 :font facename='Default' size=0x0.-> Windows 2000 SP2 (default):font facename='Courier' size=14x8.
Version=WinXP   :font facename='Default' size=0x0.-> Windows XP:font facename='Courier' size=14x8.
:elines.:font facename='Default' size=0x0.
:p.Used to override the version Odin reports to the application.
:lines align=left.:font facename='Courier' size=14x8.
:hp2.[TWAIN]
:ehp2.TwainIF         :font facename='Default' size=0x0.values 1&comma.2&comma.3 (default=1):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.1 -> Use CFM Twain Driver:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.2 -> Use STI Twain Driver (not supported at the moment):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.3 -> Use SANE Driver (not supported at the moment):font facename='Courier' size=14x8.

:hp2.[DirectDraw]
:ehp2.Fullscreen      :font facename='Default' size=0x0.Use&slr.don't use fullscreen DirectDraw:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.values&colon. 1&slr.0 (1 = Yes&comma. 0 = No):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default  0:font facename='Courier' size=14x8.

:hp2.[WINMM]
:ehp2.DirectAudio     :font facename='Default' size=0x0.Use DirectAudio interface (if available) for wave playback:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.in WINMM and DSOUND.:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.values&colon. 1&slr.0 (1 = Yes&comma. 0 = No):font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default = 1:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.:hp2.Note&colon. :ehp2.Only available in SB Live! audio driver!:font facename='Courier' size=14x8.
DefVolume       :font facename='Default' size=0x0.Default waveout volume:font facename='Courier' size=14x8.
                :font facename='Default' size=0x0.default = 100:font facename='Courier' size=14x8.
:elines.:font facename='Default' size=0x0.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.The [CodePage] section found in earlier Odin.INI files is obsolete
and not used anymore!
:lines align=left.:font facename='Courier' size=14x8.
:hp2.[DLLRENAMEWIN]:ehp2. :font facename='Default' size=0x0.do not change these&comma. otherwise Odin will not work anymore!:font facename='Courier' size=14x8.
OLE32=OLE32OS2
NETAPI32=WNETAP32
OLEAUT32=OLAUTOS2
OPENGL=OPENGL32
WINSPOOL=WINSPOOL.DLL
MCICDA=MCICDA.DLL
CRTDLL=CRTDLL32

:hp2.[DLLRENAMEOS2]:ehp2. :font facename='Default' size=0x0.do not change these&comma. otherwise Odin will not work anymore!:font facename='Courier' size=14x8.
OLE32OS2=OLE32
WNETAP32=NETAPI32
OLAUTOS2=OLEAUT32
OPENGL32=OPENGL
WINSPOOL=WINSPOOL.DRV
MCICDA=MCICDA.DRV
CRTDLL32=CRTDLL
:elines.:font facename='Default' size=0x0.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=520.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=540.:hp2.[Next]:elink.:ehp2. 
:i1.Odin.INI

:h2 res=540 x=right y=bottom width=70% height=100%.The first application - OdinApp.EXE

:font facename='Default' size=0x0.:p.:font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0. is a Win32 application. It serves two purposes&colon.
:ol.
:li.When :font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0. is installed&comma. the installer also installs some core
Microsoft runtime DLLs&colon.
:ul compact.
:li.MSVCRT40.DLL
:li.MSVCIRT.DLL
:li.MSVCRT.DLL
:li.MFC42.DLL
:eul.
:p.The Microsoft license does not allow Netlabs to distribute these DLLs with
Odin. They must be distributed as part of a Win32 application. 
:li.If :font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0. installs and runs successfully&comma. it indicates your Odin setup
has no major problems. It opens a window&comma. presents an empty file browser
window all within a single window. :font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0. has no useful functionality e.g.
you cannot perform a particular task with it. You can play with it by creating
multiple child windows&comma. print these empty windows (and get a blank page
out of the printer). But that is all&comma. folks!
:li.If installed and run successfully&comma. :font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0. looks like this&colon.
:eol.
:artwork align=center name='E:\DEV\INF\ODINDOC\ODINAPP.BMP' .
:p.You can get :font facename='Courier' size=14x8.OdinApp:font facename='Default' size=0x0. from the same location as the build files.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=530.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=550.:hp2.[Next]:elink.:ehp2. 
:i1.OdinApp.EXE
:i1.HelloWorld.EXE
:i1.Microsoft runtime DLLs

:h2 res=550 x=right y=bottom width=70% height=100%.Reporting bugs - OdinBug.EXE

:font facename='Default' size=0x0.:p.OdinBug is a helper application for Odin. Its purpose is to ease the creation
of proper bug reports&comma. so in other words it is a bug report generator.
:p.If everything goes well&comma. you just have to specify a proper logfile and
some more (non technical) information. OdinBug does (or should do) the rest
for you.
:p.OdinBug has its own extensive on-line help file with explanations
on how to create logfiles and bug reports.
:p.Only debug builds can generate logfiles.
:p.In order to enable a debug build to generate logfiles the
environment variable
:p.:font facename='Courier' size=14x8.SET WIN32LOG_ENABLED=1
:p.:font facename='Default' size=0x0.must be set.
:p.You have 2 possibilities&colon.
:ol.
:li.Put :font facename='Courier' size=14x8.SET WIN32LOG_ENABLED=1:font facename='Default' size=0x0. into your :font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0.. This will enable
logging for all Win32 programs run. If you want a particular
program to run without logging&comma. you have to type :font facename='Courier' size=14x8.SET
WIN32LOG_ENABLED=:font facename='Default' size=0x0. (no value) on the commandline to clear the
variable. This effects only the session in which you cleared the
variable. I do not recommend to put :font facename='Courier' size=14x8.SET WIN32LOG_ENABLED=1:font facename='Default' size=0x0. into
:font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0..
:li.Type :font facename='Courier' size=14x8.SET WIN32LOG_ENABLED=1:font facename='Default' size=0x0. in the session in which you want to
run a particular Win32 program with logging. Win32 programs in
other sessions will still run without logging. This is the
recommended procedure.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.DragText by R. L. Walsh provides an extended WPS
class for program objects that allows you to enter environment
variables into the settings pages of a programs object (this is
one of the freeware features).
:artwork align=center name='E:\DEV\INF\ODINDOC\DTPROG.BMP' .
:p.This will enable logging for the program started from this
object. Removing the entry will disable logging again.
:eol.
:p.For details about logging and bug reporting&comma. please&comma.
read the OdinBug on-line help file&comma. which does not only
serve as an online help but also as a full fledged manual for
everything concerning bugs and reporting them.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=540.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=560.:hp2.[Next]:elink.:ehp2. 
:i1.bug reports
:i1.OdinBug
:i1.reporting bugs
:i1.DragText

:h2 res=560 x=right y=bottom width=70% height=100%.Using Glide with 3dfx cards - OpenGL3dfx.ZIP

:font facename='Default' size=0x0.:p.To run DirectDraw games or Quake 2 in Voodoo mode (Voodoo 2 or 3)&comma. you
must install Holger
Veit's XFree86&slr.2 support driver (:font facename='Courier' size=14x8.XF86SUP.SYS:font facename='Default' size=0x0.). You should have a line
like
:p.:font facename='Courier' size=14x8.DEVICE=x&colon.&bsl.OS2&bsl.BOOT&bsl.XF86SUP.SYS:font facename='Default' size=0x0.
:p.in your :font facename='Courier' size=14x8.CONFIG.SYS:font facename='Default' size=0x0. where x&colon. is your boot drive.
:p.You can get :font facename='Courier' size=14x8.XF86SUP.SYS:font facename='Default' size=0x0. from
:link reftype=fn refid=6.http&colon.&slr.&slr.ais.gmd.de&slr.veit&slr.os2&slr.xf86sup.html:elink..
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.The Glide and 3Dfx OpenGL DLL's are no longer part of the basic
Odin installation package. Development is halted on the 3dfx package.
:p.You can get OpenGL3dfx.ZIP from the same :link reftype=hd res=340.locations:elink. as the build files.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=550.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=570.:hp2.[Next]:elink.:ehp2. 
:fn id=6.:p.:link reftype=launch object='NETSCAPE.EXE' data='http://ais.gmd.de/veit/os2/xf86sup.html'.http&colon.&slr.&slr.ais.gmd.de&slr.veit&slr.os2&slr.xf86sup.html:elink.:efn.


:i1.Glide
:i1.OpenGL
:i1.3dfx
:i1.Voodoo
:i1.XF86SUP.SYS

:h2 res=570 x=right y=bottom width=70% height=100%.Installing Win32 programs

:font facename='Default' size=0x0.:p.Generally&comma. you install applications as you would under Windows. You may
have to make some accommodations&comma. because they are Windows applications
after all.

:p.Most Win32 applications come packaged as a self-extracting executable with
built-in installers. 

:p.There are a couple of installers&colon.
:ul compact.
:li.Full 32-bit
:li.Full 16-bit
:eul.
:p.Odin should have no problems running the 32-bit installers unless they happen
to require some feature not yet implemented by Odin.

:p.The 16-bit installers can be a problem. Odin can not yet run these directly.
If they are run under WinOS2&comma. these installers will eventually attempt
to run 32-bit code and fail.

:p.Sometimes the 16-bit part of the installer&comma. usually setup.exe&comma. can
be replaced with a 32-bit version. The Yahoo OdinUsers group maintains an
OdinUsers File Repository which contains setup.exe's for some of the widely
used Win32 installers. 

:p.Sometimes&comma. the 16-bit part of the installer can be run under WinOS2. It
will fail&comma. but may leave behind the decompressed 32-bit programs. These
can be run under Odin to complete the install. It might help to copy the
decompressed files somewhere else before trying to run them.

:p.If all else fails you can try to copy the installation from your Windows
machine. But&comma. it will not work if the application needs the installer to
set up registry entries. However&comma. some users are persistent and have
succeeded in copying the registry entries using RegEdit32 running under Odin.

:p.From 09&slr.2001 to 01&slr.2002 there was a 5 part article series on the
OS&slr.2 Voice Newsletter covering all aspects of the installation of Win32
applications on OS&slr.2 using Odin.

:p.You can find them at :link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org'.http&colon.&slr.&slr.www.os2voice.org:elink.!

:p.Part 1&colon. Why are so many installation packages not working with Odin? 
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/vnl/past_issues/vnl0901h/vnewsf7.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.vnl&slr.past_issues&slr.vnl0901h&slr.vnewsf7.htm:elink.
:p.Part 2&colon. Excursion&colon. Self-expanding installation packages based upon
InstallShield
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/vnl/past_issues/vnl1001h/vnewsf6.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.vnl&slr.past_issues&slr.vnl1001h&slr.vnewsf6.htm:elink.
:p.Part 3&colon. Unpacked - how to get everything in the right place?
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL1101H/vnewsf6.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.VNL1101H&slr.vnewsf6.htm:elink.
:p.Part 4&colon. Registry issues - how do I find out what belongs into the registry?
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL1201H/vnewsf6.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.VNL1201H&slr.vnewsf6.htm:elink.
:p.Part 5&colon. It's still not working - what can you do now?
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL0102H/vnewsf5.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.VNL0102H&slr.vnewsf5.htm:elink.

:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=560.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=580.:hp2.[Next]:elink.:ehp2. 
 
:i1.installing Win32 programs
:i1.16 bit installers
:i1.32 bit installers
:i1.WinOS2
:i1.OS&slr.2 Voice Newsletter
:i1.Voice
:i1.articles about installation

:h2 res=580 x=right y=bottom width=70% height=100%.Some popular Windows programs that (usually) work

:font facename='Default' size=0x0.:p.The following applications work quite well with Odin and might be interesting
things to start with&colon.
:ul.
:li.:hp2.FreeCell:ehp2. (Oh yeah!)
:p.Be sure to use a FreeCell either from the Win32s package or from Windows
NT&comma. as the one that comes with Windows 95&slr.98&slr.ME is still 16 bit
and does not work with Odin. You need :font facename='Courier' size=14x8.FREECELL.EXE&comma. FREECELL.HLP:font facename='Default' size=0x0. and
:font facename='Courier' size=14x8.CARDS.DLL:font facename='Default' size=0x0. to run it!
:li.:hp2.Adobe Acrobat 4.0:ehp2. (not 4.05 and not 5.0)
:p.You must replace the 16 bit setup files with their 32 bit counterparts&comma.
described in the previous chapter.
:p.In order to display text correctly you must turn off "smooth text" in the
"preferences dialog" otherwise it works nicely.
:li.:hp2.RealPlayer 7 or 8 basic
:p.:ehp2.Fullscreen and double size is usually distorted&comma. otherwise works very
well. Make sure you have unchecked the startcenter option in the settings
notebook.
:artwork align=center name='E:\DEV\INF\ODINDOC\RPSTCTR.BMP' .
:li.:hp2.Lotus Notes 5.0x:ehp2. client (version 5.09a being current)
:p.It has to be installed on a Windows system first. Then one has to
copy the entire Notes directory tree to the Odin partition. When
bringing over from the Windows machine make sure to edit the
NOTES.INI to have the Notes Data Path and WinNTIconPath point the
appropriate paths on the Odin system.
:p.Be aware that it may work for a long time and then completely
lock up your machine&comma. with nothing left than
Ctrl-Alt-Del&comma. though it could be killed successfully with
kill.exe occasionally (available on dink.org). 
:li.:hp2.Palm Desktop&colon. :ehp2.Take a look at Tereul De Campo's article on running Palm desktop under Odin at :link reftype=launch object='NETSCAPE.EXE'
data='http://www.os2voice.org/VNL/past_issues/VNL0301H/vnewsf4.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.VNL0301H&slr.vnewsf4.htm.:elink.
:li.:hp2.CALC.EXE:ehp2. and :hp2.TELNET.EXE:ehp2. from Win95&slr.98 work well.
:li.:hp2.Home Planet 3.0:ehp2. (an astronomy application) also works well.
:eul.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.Although these applications generally work&comma. you will
probably experience infrequent crashes.
:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.There is a large database of tested applications at the Odin site
at Netlabs.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=570.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=600.:hp2.[Next]:elink.:ehp2. 
 
:i1.applications that work
:i1.RealPlayer
:i1.Adobe Acrobat
:i1.FreeCell
:i1.Palm Desktop
:i1.Home Planet
:i1.Telnet
:i1.Calc

:h1 res=600 x=right y=bottom width=70% height=100%.License and warranty

:font facename='Default' size=0x0.:p.:hp8.EXCEPT AS OTHERWISE RESTRICTED BY LAW&comma. THIS WORK IS PROVIDED WITHOUT ANY
EXPRESSED OR IMPLIED WARRANTIES OF ANY KIND&comma. INCLUDING BUT NOT LIMITED
TO&comma. ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE&comma.
MERCHANTABILITY OR TITLE. EXCEPT AS OTHERWISE PROVIDED BY LAW&comma. NO
AUTHOR&comma. COPYRIGHT HOLDER OR LICENSOR SHALL BE LIABLE TO YOU FOR DAMAGES
OF ANY KIND&comma. EVEN IF THEY HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES.:ehp8.
:p.The following license applies to Project Odin&colon. :link reftype=launch object='E.EXE' data='LICENSE.TXT'.Click here to read!:elink.
:p.The following license applies to WGSS50 library&colon. :link reftype=launch object='E.EXE' data='WGSS50.LIC'.Click here to read!:elink.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=580.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=700.:hp2.[Next]:elink.:ehp2. 
:i1.license
:i1.warranty

:h1 res=700 x=right y=bottom width=70% height=100%.Project participation, getting help

:font facename='Default' size=0x0.:p.:hp2."Do not ask&comma. what Odin can do for you&comma. ask what you can do for
Odin!"
:artwork align=left name='E:\DEV\INF\ODINDOC\UNCLESAM.BMP'.
:p.:hp9.:ehp2.WANTED&colon. We are still looking for talented developers that want to
contribute!:ehp9.

:p.As Odin became an open source project&comma. everybody is kindly invited to
contribute his&slr.her share to the progress of the project. May it be active
coding&comma. fixing bugs or just providing detailed information about
examined problems.
:p.There is no formal support infrastructure for Odin. Please&comma. remember
most work on Odin is done by a few volunteers in their spare time.
:p.The following mailing lists are considered to be the universal platform for
people needing help and people wishing to contribute.

:p.We suggest you subscribe to&colon.
:ul.
:li.:hp2.OdinUsers:ehp2. for endusers wishing to contribute&comma. discussing about
problems&comma. getting help.
:li.:hp2.Win32OS2-WAI:ehp2. for developers wishing to contribute.
:eul.
:p.on :link reftype=fn refid=10.http&colon.&slr.&slr.www.yahoogroups.com:elink..

:p.In case you are interested in participating&comma. every member of the project
will
be happy to give you direction to the right places and to give a personal
introduction to further development of the particular modules.

:p.:artwork align=left name='E:\DEV\INF\ODINDOC\RD_PIN.BMP' runin.:hp2.Note&colon. :ehp2.There is another mailing list named :hp2.Win32OS2:ehp2. at YahooGroups. It is
also Odin-related&comma. but is considered outdated and obsolete.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=600.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=800.:hp2.[Next]:elink.:ehp2. 

:fn id=10.:p.:link reftype=launch object='NETSCAPE.EXE' data='http://www.yahoogroups.com'.http&colon.&slr.&slr.www.yahoogroups.com:elink.:efn.

:i1.participation
:i1.support
:i1.getting help

:h1 res=800 x=right y=bottom width=70% height=100%.Odin related links

:font facename='Default' size=0x0.:p.The following links might be interesting for anybody needing information about
Odin&colon.
:ul.
:li.Netlabs at :link reftype=launch object='NETSCAPE.EXE' data='http://odin.netlabs.org'.http&colon.&slr.&slr.odin.netlabs.org:elink.
:ul compact.
:li.:link reftype=fn refid=3.ftp&colon.&slr.&slr.ftp.netlabs.org&slr.pub&slr.odin&slr.daily:elink.
:li.:link reftype=fn refid=4.ftp&colon.&slr.&slr.ftp.netlabs.org&slr.pub&slr.odin&slr.weekly:elink.
:eul.
:li.OS2.org at :link reftype=launch object='NETSCAPE.EXE' data='http://www.os2.org'.http&colon.&slr.&slr.www.os2.org:elink.
:ul compact.
:li.:link reftype=fn refid=1.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.daily:elink.
:li.:link reftype=fn refid=2.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.weekly:elink.
:eul.
:li.Innotek Systemberatung at :link reftype=launch object='NETSCAPE.EXE' data='http://www.innotek.de'.http&colon.&slr.&slr.www.innotek.de:elink.
:ul compact.
:li.The company backing Odin! Also take a look at their other products!
:eul.
:li.OS&slr.2 VOICE newsletter at :link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org'.http&colon.&slr.&slr.www.os2voice.org:elink.
:ul compact.
:li.Running Palm Desktop under Odin (by Tereul De Campo) at
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL0301H/vnewsf4.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.vnl0301h&slr.vnewsf4.htm.:elink.
:li.Part 1&colon. Why are so many installation packages not working with Odin? 
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/vnl/past_issues/vnl0901h/vnewsf7.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.vnl&slr.past_issues&slr.vnl0901h&slr.vnewsf7.htm:elink.
:li.Part 2&colon. Excursion&colon. Self-expanding installation packages based upon
InstallShield
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/vnl/past_issues/vnl1001h/vnewsf6.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.vnl&slr.past_issues&slr.vnl1001h&slr.vnewsf6.htm:elink.
:li.Part 3&colon. Unpacked - how to get everything in the right place?
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL1101H/vnewsf6.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.vnl1101h&slr.vnewsf6.htm:elink.
:li.Part 4&colon. Registry issues - how do I find out what belongs into the registry?
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL1201H/vnewsf6.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.vnl1201h&slr.vnewsf6.htm:elink.
:li.Part 5&colon. It's still not working - what can you do now?
:link reftype=launch object='NETSCAPE.EXE' data='http://www.os2voice.org/VNL/past_issues/VNL0102H/vnewsf5.htm'.http&colon.&slr.&slr.www.os2voice.org&slr.VNL&slr.past_issues&slr.vnl0102h&slr.vnewsf5.htm:elink.
:eul.
:li.OS&slr.2 e-zine newsletter at :link reftype=launch object='NETSCAPE.EXE' data='http://www.os2ezine.com'.http&colon.&slr.&slr.www.os2ezine.com:elink.
:li.OS&slr.2 Connect newsletter at :link reftype=launch object='NETSCAPE.EXE' data='http://www.os2ss.com/connect'.http&colon.&slr.&slr.www.os2ss.com&slr.connect:elink.
:li.SCOUG (Southern California OS&slr.2 User Group) at
:link reftype=launch object='NETSCAPE.EXE' data='http://www.scoug.com'.http&colon.&slr.&slr.www.scoug.com:elink.
:ul compact.
:li.Search for "Odin" in order to find further information!
:eul.
:li.Wine Headquarters at :link reftype=launch object='NETSCAPE.EXE' data='http://www.winehq.com'.http&colon.&slr.&slr.www.winehq.com:elink.
:ul compact.
:li.Wine is for Linux what Odin is for OS&slr.2. In addition&comma. Odin is partly
based on work of the Wine people.
:eul.
:eul.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=700.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=900.:hp2.[Next]:elink.:ehp2. 
:i1.links

:h1 res=900 x=right y=bottom width=70% height=100%.Contributors to this manual

:font facename='Default' size=0x0.:artwork align=center name='E:\DEV\INF\ODINDOC\ODINBANN.BMP' .
:p.This manual is copyrighted material.
:p.This compilation was done from January to April 2002 by&colon.

:p.:link reftype=fn refid=998.Herwig:elink. Bauernfeind (:link reftype=fn refid=999.herwig.bauernfeind@aon.at:elink.)
:p.Send corrections&comma. additions or whatever either directly to me or to the
odinusers list at Yahoo.

:p.The following sources have been used and were partly merged into this
manual&colon.
:ul.
:li.The old ReadMe.TXT dated 1999&slr.11&slr.16&comma. last modified on
2001&slr.04&slr.18&comma. that came with every Odin build done by&colon.
:ul compact.
:li.Sander van Leeuwen&comma. 
:li.Achim Hasenm&ue.ller and 
:li.Knut Stange Osmundsen
:eul.
:li.The article at
:link reftype=launch object='NETSCAPE.EXE' data='http://www.scoug.com/OS24U/2001/SCOUG009.ODINTODAY.HTML'.http&colon.&slr.&slr.www.scoug.com&slr.OS24U&slr.2001&slr.SCOUG009.ODINTODAY.HTML:elink. done by&colon.
:ul compact.
:li.Steven Levine for SCOUG 08&slr.2001. Thanks for the kind permission to use it.
:eul.
:li.The following persons contributed information&comma. ideas&comma. concepts and
proof reading to this manual&colon.
:ul compact.
:li.Seth Berkowitz (a lot of proof reading&comma. ideas and concepts)
:li.Mark Abramowitz (screenshots)
:li.Derek Wright (ideas&comma. concepts)
:li.Steven Levine (persuasion&comma. information)
:li.Sander van Leeuwen (information)
:eul.
:eul.
:fn id=998.:p.:artwork align=left name='E:\DEV\INF\HERWIG.BMP' runin.:p.You got me!:efn.
:fn id=999.:p.mailto&colon. herwig.bauernfeind@aon.at:efn.
:p.:link reftype=hd res=100.:hp2.[Home]:elink.:ehp2. :link reftype=hd res=800.:hp2.[Previous]:elink.:ehp2. :link reftype=hd res=100.:hp2.[Next]:elink.:ehp2. 






:euserdoc.
