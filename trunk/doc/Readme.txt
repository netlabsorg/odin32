
                O D I N  Public Alpha 5 - 1999/11/16
                ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WANTED: We are still looking for talented developers that want to
        contribute! Please take a look at the last section!

NOTE:   LICENSE.TXT describes the Project Odin Software License.


Project Odin Information
========================

1.0 Introduction
----------------

This readme is intended for end users that have downloaded the
latest public alpha release and want to give it a try. For more
information, be sure to regularly check http://www.netlabs.org/odin.

1.1 Purpose
-----------

Odin is a free Open Source project with the goal to eventually
provide a fully working Win32 (Win 95/98/NT/2000) runtime environment,
making it possible to run your favorite Windows programs under OS/2,
as if they were native OS/2 programs. The second goal of Odin is to
provide the full Win32 application programming interface (API) so
that it is possible to build OS/2 programs using Windows source
code. This is also refered to as Odin32 whereas Odin is the name
of the Win32 binary emulator.

1.2 Current Status
------------------

Odin is still in the early alpha state so that it is very far from
being widely usable. Most programs will not work or show many
problems. Even though it is not usable for most programs, we have
decided to do a public release in order to show how much progress
the project has been made. A second goal is to attract more developers
in order to speedup development. Our job includes rewriting most
parts of Windows 95/NT without having the original source code so
the more developers we have the more progess will be seen. The
last public release (Alpha 0.004) has been in 1997 and the project
has changed much since then. Everything is free and Open Source now
and the base architecture has been improved quite a lot. We are now
in a position where we can expect the first bigger applications to
run well and to make much more visible progress.

1.3 Changes and additions in alpha 5
------------------------------------
- Removed Open32 dependacy in User32 code: windows, dialogs, buttons and
  other controls creation and manipulation is now implemented fully in Odin32.
  Applications have Win32 look: Win32 specific controls are also supported
  (Open32 is still a requirement as other Odin32 functions use it!)
- New and improved PE2LX converter
- New Portable Executable Loader (PE) that overcomes some differences
  between OS/2 and Win32 memory models (this does not include 512MB
  per-process memory limitation in OS/2, you still need the Aurora-level
  kernel for that)
- New WIN32K32k.SYS driver that automates loading of Win32 binaries (no
  conversion or invocation through PE.EXE loader is needed), now
  compatible with WSeB (Aurora) and no longer dependant of code under
  GNU license
- Wine ports of common controls (comctl32), shell extensions (shell32) and
  OLE
- Full implementation of memory mapped files
- Joystick support (winmm)
- Direct Input (DirectX keyboard & mouse) added (Wine Port)
- Rewrite of DirectDraw. More complete implementation.
- Couple of (not core Win32 API, but often used) new DLLs (winasp32, lz32,
  avifil32...), so users don't have to have Windows license at all to run
  Windows programs
- Many new API functions implemented or ported from Wine, allows
  developers to port Win32 apps directly to OS/2 with little to no rewriting
- Many bugfixes and enhancements
- Many new programs that load and/or work with Odin
- Full source code available



2.0 Installing Odin (UPDATED)
-----------------------------

Odin daily builds are now also distributed as WarpIn archives.
WarpIn is a generic OS/2 installation application that can be
downloaded from warpin.netlabs.org.

To install Odin run warpin.exe with the name of the daily build archive
as parameter.
Follow the instructions to complete the installation.
The Odin installation consists of:
  - creating an Odin directory and copying the included files to
    x:\odin and x:\odin\system32
  - creating empty directories in x:\odin (to create a similar directory
    structure as found in Windows)
  - creating a basic registry for executing win32 applications in OS/2
  - adds x:\odin\system32 to the path and libpath settings in your config.sys

NOTE: You MUST install Odin at least once using WarpIn (unless you know
      what you're doing). Afterwards you can use the daily build zipfiles
      to upgrade to a newer version.

To run DirectDraw games or Quake 2 in Voodoo mode, you must install Holger
Veit's XFree86/2 support driver (XF86SUP.SYS).
Visit http://ais.gmd.de/~veit/os2/xf86sup.html to download it.

2.0.1 Log files
---------------
The alpha 5 binaries and daily build zipfiles can generate logfiles to show
what a win32 application is doing. This can be very useful to determine
why certain applications don't run correctly.

The major disadvantage of loggging is the overhead. Therefor it has been
disabled by default in the alpha 5 release and daily builds.
To enable logging set the environment variable WIN32LOG_ENABLED:
   SET WIN32LOG_ENABLED=1

To disable logging again, you must clear this variable:
   SET WIN32LOG_ENABLED=

2.1 Using Odin to run your Windows applications
-----------------------------------------------

Odin still has several ways to execute Win32 programs.


2.2 The Ring3 Win32 Loader (PE.EXE)
----------------------------------

This is the new and also recommended method for loading programs.
Unlike with the previous alphas, it is not required to convert or
anyhow change the original files. Installing a device driver is
also not necessary. Be sure to put the Odin binary directory into
both the PATH and LIBPATH (alternatively you can use a batch file
with SET PATH=C:\Odin;%PATH% and SET BEGINLIBPATH=C:\Odin;)
and then call pe.exe winprog.exe param1 param2 ... If you wanted
to start the Windows calculator, you would type

pe calc

The PE (Portable Executable, the Win32 executable format) loader
will now try to load the application into memory and execute it.
This will either work or fail with a loader message. If you get
such a loader message, it is very likely that a required DLL is
missing. To correct that problem, take a look at pe_0.log (or
pe_1.log for the second program running at this time and so on).
At the bottom of the file you will see the DLLs and the functions
that the loader imports. It will tell you if it couldn't find a
DLL and you can correct the problem and try it again.


2.2.1 PE loader issues
----------------------

As the PE loader will create the process and load the program,
a process utility such as pstat will always show PE.EXE as the
executable file.
It is also possible that a certain Win32 application becomes
a zombie (that is when it does certain things the OS/2 kernel
cannot recover such as die when waiting on a semaphore). Then
it will not be possible to kill the application - only a system
restart will help.
Certain programs do use fixed memory addresses where they need
to be loaded (i.e. they won't use fixups). You can notice this
at the top of the loader log file (pe_0.log). If the fixed address
is above the 512MB barrier present in Warp 4 and Warp 3, the
program can not be run under these system. On Aurora (Warp Server
for e-business) this limitation has been removed. Programs known
to fall under this category are Microsoft Office (also the
Office viewers) and other mainly Microsoft programs.


2.3 Win32k.sys - native OS/2 PE-loader
--------------------------------------

The win32k.sys driver makes PE (Win32) executables equal to native OS/2
executables by implementing a Ring 0 on-the-fly loader / converter.

You may wonder how you may run a Win32 program using a device driver. You
don't have to worry about that, simply treat the Win32 program just as an
another OS/2 program. OS/2 can't tell the difference!
No conversion of dlls or running programs thru Pe.EXE.

For example, if you fancy playing windows solitaire.
    1) Make sure the necessary DLLs (MSVCRT.DLL and CARDS.DLL) are in you
       PATH, current dir, SOL.EXE dir, Odin32 System32 dir, Odin32 System dir,
       Odin32 dir, or LIBPATHs; and that you have SOL.EXE somewhere.

    2) Then start SOL.EXE by simply typing SOL.EXE (with path if necessary).
       It's also possible to start SOL.EXE from the desktop if you like.
       Simply find the SOL.EXE icon (or create it) and double click on it!


2.3.1 WARNING
-------------

The Win32k.sys is quite alpha software and any bugs which haven't been found
yet may crash and/or corrupt you entire system! Make sure you have backed up
important stuff first!

We are not responsible for ANYTHING AT ALL! See Odin32 disclaimer...

Before you install Win32k.sys make sure that you have a backdoor to your system
which works. For example try to boot your system using the Alt+F1 during bootup
and choose Commandline boot (F2). Make sure you have an textmode editor which
works; for example TEDIT.EXE.


2.3.2 Requirements
------------------

Win32k should at least work on OS/2 Warp 3 and above, but I don't get too
surprised if it worked on 2.11 too. DBCS are not supported yet AFAIK.

It no longer requires a kernel file for retail kernels (if you don't know what
kernel you have, you have a retail kernel). Win32k does now contain a
"database" for the most recent retail kernels. Currently this is:
    Warp 4 fp#1 -> fp#14
    Aurora GA, FP1, preFP2
    Warp 3 fp#32-> fp#42

If the running kernel is not found in the "database" or is a debug kernel
wink32.sys will look for symbolfiles as it allways did.
If you use the -s:<symfile> argument the given symbol file will be
investigated before the database is searched.

NOTE! It hasn't been fully verified that all the different national fixpaks
have the same symbols as the us-english ones. Feedback on this issue is
appreciated.

NOTE! A piece of advice for those playing with debug-kernels: Make sure that
c:\OS2KRNL.SYM is matching the running kernel.

2.3.2.1 Warp 3
--------------

Win32k is not tested with Warp 3. It is _supposed_ to work with Warp 3
provided you are running fixpack 32 or later.
(Fixpack 32 + y2kclock.exe is required for Y2K readyness anyway so...)


2.3.3 Installation
------------------

To use Win32k.sys you have to add the following line into your config.sys:
    device=<d:\path\system32>win32k.sys

Where <d:\path\system32> is the path to the win32k.sys, i.e. usually the odin32
system32 directory.

After doing this, you'll have to reboot your machine to load the driver. During
the boot process you will on a successful installation see some lines of
technical info and finally a line saying:
Win32k - Odin32 support driver.
    Build:            14046 - v20.45
    Found symbolfile: Win32k Symbol Database
Win32k.sys succesfully initiated!

If you don't see the last line something is wrong, and some error info will be
shown. In the case of error the 'rc=<value>' is important to remember.


2.3.4 Win32k parameters
-----------------------

The Capitalized letters are the one which is tested for when identifying the
argument.

There are some useful parameters for Win32k:
-Quited         Quiet driver initiation.
                Default: disabled (ie. -V)

-Verbose        Verbose driver initiation.
                Default: enabled

-S:<filename>   Full path to the current OS/2 kernel symbol file.
                When this is specified the given
                Default: symbol "database" and default locations are searched.


Loader parameters:
-SCript:<Y|N>   Enable / disable UNIX script support.
                Default: Enabled.

-REXX:<Y|N>     Enable / disable pure REXX script support.
                Default: Enabled.

-Java:<Y|N>     Enable / disable JAVA loading.

-Pe:<MIXED|PE2LX|PE|NOT>
                Enabled / disable and set mode of the PE loading.
                    MIXED:  Mix of Pe2Lx and Pe.exe. It uses Pe2Lx when
                            possible and backs out to pe.exe if the executables
                            aren't supported by the converter.
                    PE2LX:  Pe2Lx loader only.
                    PE:     Pe.exe only. The program is started using Pe.exe,
                            and the process name will be pe.exe.
                    NOT:    Disable PE loading.


Logging parameters:
-L<:E|:D>       Enables/Disables logging.
                Default: disabled.

-Com:<1|2|3|4>
or
-C<1|2|3|4>     Which com-port to log to.
                Default: -C2

-Warning:<1|2|3|4>
or
-W<1|2|3|4>     Warning/info level for PE loader. (This is warnings and info
                which is published thru the com-port as logging information.)
                Default: -W0


Heap parameters:
-HEAPMax:       Maximum swappable heap size in bytes.
                Default: 512KB

-Heapinit:      Initial heapsize in bytes.
                Default: 16MB

-RESHEAPMax:    Maximum resident heap size in bytes.
                Default: 256KB

-Resheapinit:   Initial resident in bytes.
                Default: 16MB


OS/2 Loader parameter:
-Dllfixes<:E|:D>
                Enables or Disables the long DLL name and non .DLL
                extention fixes. OS/2 DLLs have been limited to 8 chars
                names since version 2.<something>. This is a very bad
                limitation. Also OS/2 forces all DLL loaded to loadtime
                to have the extention '.DLL'.
                Both these retritions are removed if this option is
                enabled.
                For DLL with non .DLL extention the internal name must
                have to the extention included. (If you the experiences
                problems with IMPLIB.EXE, enclose the internal name
                in double-quotes.)
                You should not make a DLL with a name starting with
                DOSCALLS, DOSCALLS1, IBMVGA32, IBMDEV32 or IBMXGA32.
                Current state for this feature is EXPERIMENTAL.
                Default: Enabled


Obsolete:
-K:<filename> Ignored. Win32k scans the kernel image in memory!


2.3.5 PE Loading
----------------

Win32k.sys are able to do PE loading in two different ways.

2.3.5.1 PE2LX
-------------
The first one is called PE2LX and hence uses the original Win32k.sys method
which is similar to Pe2Lx. This only requires that KERNEL32.DLL (ie. Odin32
System32 directory) is present somewhere in the PATH or LIBPATHs. The DLL
search order of PE2LX is now conforming with the Win32 order:
    1. The directory of the executable (.EXE).
    2. The current directory.
    3. The Odin32 System32 directory. (Kernel32.dll directory)
    4. The Odin32 System directory. (sibling of system32)
    5. The Odin32 directory. (parent of system32)
    6. The PATH environment variable. (TODO: quotes are not currently supported)
    7. The BEGINLIBPATH.
    8. The LIBPATH.
    9. The ENDLIBPATH.
(Note that the OS/2 DLLs in the _Odin32_ process are also found using this
search order. This shouldn't matter much as nearly all of the OS/2 DLL are
allready loaded. But if this proves a problem it could be changed.)

2.3.5.2 PE
----------
The second one is called PE and invokes the Pe.EXE to execute the PE (Win32)
programs. This of course requies that PE.EXE is found in the PATH.

2.3.5.3 Modes
--------------
Win32k.sys have three (or four) different ways of combining these two methods,
called modes:
    PE2LX only
    PE only
    PE2LX with fallback on PE.
    (fourth: NOT load any PE executables at all)

The last one is the default one and is probably the best one currently.


2.3.6 UNIX Shell Script Loader
------------------------------

The UNIX Shell Script Loader makes OS/2 able to run UNIX shell scripts as
native executables, just as UNIX does. A unix shell script is a text file
which has a starting line which specifies the interpreter for the script.

For example a perl script:
#!/bin/perl5 -Sx

Or the most common one:
#! /bin/sh

Since Most OS/2 systems don't have a /bin directory or a /usr/bin directory
the executable specified are looked for in these ways:
    1. The exact filename specified.
    2. The filename specified with .EXE extention.
    3. Search the PATH for the filename (with out path of course).
    4. Search the PATH for the filename with .EXE extention (and with out path of course).

So, you could now execute UNIX shell script in OS/2 without even modify the
path of the interpreter!

The UNIX Shell Script Loader is enabled by default.


2.3.6 Pure REXX Loader
----------------------

The Pure REXX Loader is a win32k.sys feature which enables REXX script with
the extentions .RX and .REX to run as native OS/2 executables. These REXX
scripts are executed using kRX.EXE which is found in the Odin32 System32
directory.

When we say "Pure REXX" scripts, that means no use of command line should
occure in the script.

The Pure REXX Loader is enabled by default.


2.3.7 Java Loader/Invoker
-------------------------

Just for fun, I've added some code which detects JAVA class binaries and
executes the class using JAVA.EXE. I first thougth this should be easy, but
it turned out to have several problems.
So please note the following:
    - The classname is case sensitive, you have to write the exact class
      name (with .class extention or what ever extention the file have)
      when trying to execute a class. This could cause some throuble making
      WPS program objects for the classes....
    - To support executing of .class-files which are not in the current
      directory win32k.sys send the directory of the .class-file in to
      java.exe as a '-classpath' option. This option I soon found out had the
      sideeffect of making JAVA.exe ignore the other CLASSPATH and not use
      it's default class path. So now the contents of the CLASSPATH env. var.
      or a generated default classpath is also sent in with the '-classpath'
      option.
    - The CLASSPATH environment variable should not be longer than 639 chars.

When you have successfully installed Win32k.sys try type this at the
commandline:
c:\java11\demo\hello\hello.class

This is then printed at my screen:
Hello, Warped world!

The JAVA Loader/Invoker is enabled by default.


2.3.8 Rings - Ring 0 / Ring 3
-----------------------------

FYI. Ring 0 is the most priveleged level of execution in OS/2. The OS/2 kernel
and device drivers execute at this level, while user applications execute in
Ring 3, the least priveleged executing level.


2.3.9 Win32k Control Center
---------------------------

In your Odin32 System32 directory there is a program called Win32kCC.exe,
this is the Win32k Control Center (Win32kCC). Win32kCC is as the name says
a program controlling Win32k.sys.

Win32kCC allows you to enable, disable and change behaviour of different
parts in Win32k.sys.

For example: You could disable PE loading, or change it from MIXED mode (see
2.3.4, -PE) to PE2LX mode. Or change it from PE2LX mode to PE mode to see
if PE.EXE does a better job loading an executable.

Note that the changes would not effect allready loaded executables.


Win32kCC also displays status info on the running Win32k.sys and OS/2 kernel:
    -Win32k.sys version number
    -Win32k build time/date
    -Path of the symbol file which was used (or Win32k Symbol Database).
    -OS/2 kernel build number fixpack (guess)
    -OS/2 kernel type as win32k has detected it
    -Number of Pe2Lx currently loaded modules
    -Win32k resident and swappable heap stats.

PS. Beside the "PE Loader" area there is an "Elf Loader" area this is for a
future Elf2Lx loader.



2.4 The Ring3 conversion utility (PE2LX.EXE)
--------------------------------------------

PE2LX.EXE was the first method of running Win32 programs under
OS/2. It is still provided and also functional but it has been
deprecated. Using it is only sensible for debugging certain
problems as it requires all DLLs and EXEs to be converted into
another file format thus changing the original Win32 files.
(It's only future use will be to convert Win32 executable with
debuginfo to LX with HLL (IPMD/ICSDEBUG) debuginfo for
sourcelevel debugging of Win32 executables under Odin32. This
is not yet implemented but are being worked on.)



3.0 Applications known to be working
------------------------------------

- Windows 95/NT Help Viewer (winhlp32.exe)
- Windows 95/NT Registry Editor
- Windows NT Solitaire (not the one from Windows 95)
- Windows NT Freecell (not the one from Windows 95)
- Windows 95/NT Calculator
- Windows NT Pinball
- Windows NT Minesweeper
- Quake II
- Red Alert
- Professional Minesweeper
- Windiff (part of Win32 SDK)

Note that many other applications load and/or partly work.


4.0 Reporting Problems and Successes
------------------------------------

In this early stage it is not required that you report every
bug to the developers as we still have huge lists of to do items.
However, an application compatibility database has been created
at Netlabs where you can share your experiences with Odin.
The application compatibility database can be found at

http://www.netlabs.org/odin/appdatabase


Project Odin Source Code Notes
==============================

1.0 Introduction
----------------

This readme is intended for making people more familiar with ODIN and to
describe various aspects of the project such as accessing latest sources via
cvs, building the binaries, running win32 applications, how to identify and
report problems and bugs, as well as how to participate in this very
promising project.


1.1 Prerequisites:
------------------

Required compilers/tools:
        - IBM VisualAge 3.0 (CTx8 fixpack applied)
          (version 3.6 might also work, but version 4 is not
           recommended)
        - OS/2 Warp 4 Toolkit (project apparently doesn't compile with
          VAC's OS/2 headers)
          Might also work with EMX headers. (haven't tried this!)
        - ALP 4.0 (IBM Assembly Language Processor)
          Download the tools zipfile from http://service.boulder.ibm.com/ddk/
          (you need to register, but it's free)
        - Unix-like rm.exe (nmake clean)

Changes from last source code release:
        - Uses Wine headers and a stripped down version of os2win.h (Open32
          header)
        - All Open32 apis have the 'O32_' prefix.
        - All Open32 declarations are appended with '_O32' (mostly due to
          calling convention differences)
        - nmake DEBUG=1 builds the debug version
          nmake         builds the release version
        - Several dlls (i.e. USER32, KERNEL32, ADVAPI32) contain extra .def
          files that are used to create libraries that contain the api names
          as required by the linker when you use them. (_apiname@nrparameterbytes)

Wine header notes:
        - Several header files have been changed to make them usable for VAC
        - Several header files contain bugfixes (mostly parameter type changes)
        - Several header files include additional declarations
        - At this time we don't intend to have our changes included in the
          Wine codebase as header files are not likely to change often.

Coding conventions:
        - Do NOT directly call the Open32 apis unless you're writing
          a wrapper for it.
        - Do NOT include os2.h directly. Use os2wrap.h instead.
          os2wrap.h contains all the OS/2 apis with a wrapper around it to save
          and restore the selector in FS. (some OS/2 apis set it back to the
          default value (150b))
        - Use the correct Win32 api when there are two version (ascii & unicode)
          Calling the standard one (SendMessage) causes a error during
          compilation.
          Use SendMessageA instead.
        - Source code style will be decided on in the near future.


2.0 Accessing ODIN source code via CVS
--------------------------------------

Please check out http://www.netlabs.org/ and http://www.netlabs.org/odin.


3.0 Building the binaries
-------------------------

Basically, if your system setup meets above stated requirements, everything you
need to do for a binary build is:

  nmake DEBUG=1

or alternatively

  nmake release (not working right now!)

As the current state of the project is still known to be alpha level, one should
expect some functions not to work properly. Therefore, we recommend the use of
the debug-version which creates long logfiles for problem determination purpose
and to allow active debugging with debuggers such as IPMD, ICAT, KDB, etc.

Please note if you do not want to suffer from the performance loss of writing
logfiles, you can disable logging even with the debug binaries of ODIN by
setting:

  SET NOWIN32LOG=YES

To enable logging again, clear this environment variable by typing:

  SET NOWIN32LOG=

Logging is enabled by default in the debug build, but disabled by default
in the nodebuginfo build and not present in the release build.

3.1 Build problems
------------------

ODIN is moving at a fast pace. Sometimes side effects of source dependencies may
require you to do a full rebuild of the binaries instead of an incremental build.

However, rarely it also happens the sources contained in the CVS repository do
not build completely due to erroneous source code. Usually, this is fixed within
two days at maximum.
In such event, one can normally stick with the previous version of the particular
module to further test win32 applications on ODIN.

Please report such build problems through the appropriate channel.


3.2 Downloading daily built binaries
------------------------------------

http://www.os2.org/ has setup an ftp server that hosts the automatically
built binaries on daily basis. For people that do not have a suitable
development setup for ODIN, we provide the latest binary code that way.


4.0 Project participation
-------------------------

As ODIN became an open source project, everybody is kindly invited to
contribute his/her share to the progress of the project. May it be
active coding, fixing bugs or just providing detailed information about
examined problems.

We suggest you subscribe to win32os2-wai and the corresponsing mailing lists
on http://www.egroups.com.
In case you are interested in participating, every member of the project will
be happy to give you direction to the right places and to give a personal
introduction to further development of the particular modules.

