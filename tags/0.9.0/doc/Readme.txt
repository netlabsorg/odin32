
                    O D I N  Version 0.8
                    ~~~~~~~~~~~~~~~~~~~~

WANTED: We are still looking for talented developers that want to
        contribute! Please take a look at the last section!

NOTE:   LICENSE.TXT describes the Project Odin Software License.


Project Odin Information
========================


1.0 Introduction
----------------

This readme is intended for end users that have downloaded the
latest public release and want to give it a try. For more
information, be sure to regularly check http://svn.netlabs.org/odin32/.


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

Odin is still in the early state so that it is very far from
being finished. With version 0.8 we reached the goal, that Flash 11
and OpenJDK6 runs.


1.3 Changes and additions
-------------------------

Please refer to the ChangeLog file where you will find a brief description
of changes from release to release.


2.0 Installing Odin (UPDATED)
-----------------------------

Builds of Odin are available at ftp://ftp.netlabs.org/pub/odin/.
For people that do not have a suitable development setup for ODIN,
we provide the latest binary code that way.

Odin builds are now also distributed as RPM packages beside the ZIP archives.

Requirements for installing Odin:
- You must install Odin on a partition that supports long filenames (e.g.
  HPFS/JFS). Odin will not work on FAT!
- OS/2 Warp 4, eComStation or Warp Server for e-Business (WSeB)
  When using Warp 4, we recommend you apply fixpack 13 or higher.
  * Warp 3 is not officially supported, but is reported to work by using
    Warp 4 open32 dlls and patching them.
- kLIBC runtime v0.6.5 (ftp://ftp.netlabs.org/pub/gcc/libc-0_6_5-csd5.wpi).
- GCC runtime v4.4.6  (ftp://ftp.netlabs.org/pub/gcc/gcc4core-1_2_1.wpi).
For the latest requirements always see http://svn.netlabs.org/odin32

In order to use system tray (notification/indicator area) support, you will
need to install XWP/eWPS and the Extended System Tray widget for XCenter/eCenter
available at ftp://ftp.netlabs.org/pub/qt4/xsystray/ (version 0.1.1 and above).

Manual installation without RPM is not recommended. If you don't use RPM,
then you *must* execute odininst.exe in the Odin system32 directory. (the first
time Odin is installed or when the ChangeLog mentions fixes/changes in the
installer)

NOTE: You must install Odin on a partition that supports long filenames (e.g.
      HPFS/JFS). Odin will not work on FAT!

To install Odin, run YUM install libodin

If you don't like RPM then use the ZIP and do the following:
  - unzip Odin to a directory of your choice but preserve the structure in the ZIP
  - run odininst.exe from the x:\odin\system32 directory
  - add x:\odin\system32 to the path and libpath settings in your config.sys

To run DirectDraw games or Quake 2 in Voodoo mode, you must install Holger
Veit's XFree86/2 support driver (XF86SUP.SYS).
Visit http://ais.gmd.de/~veit/os2/xf86sup.html to download it.

NOTE: The Glide and 3Dfx Opengl dll are no longer part of the basic Odin
      installation package. It's available as opengl3dfx.zip on ftp.os2.org.

To run Win32 apps not built with the SDK you will probably also need the
OdinApp package.
The package is available at ftp://ftp.netlabs.org/pub/odin/Daily/odinapp.zip

2.0.1 Log files
---------------
The binaries can generate logfiles to show what a win32 application is doing.
This can be very useful to determine why certain applications don't run correctly.

The major disadvantage of loggging is the overhead. Therefor it has been
disabled by default in the release builds.
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

NOTE: This driver is not working anymore!! So the complete chapter 2.3 stays
      for historical reasons. If the driver will ever work again is not decided
      yet.

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



2.4 The Ring3 conversion utility (Xx2Lx.exe)
--------------------------------------------

Xx2Lx.exe was the first method of running Win32 programs under
OS/2. It is still provided and also functional but it has been
deprecated. Using it is only sensible for debugging certain
problems as it requires all DLLs and EXEs to be converted into
another file format thus changing the original Win32 files.
(It's only future use will be to convert Win32 executable with
debuginfo to LX with HLL (IPMD/ICSDEBUG) debuginfo for
sourcelevel debugging of Win32 executables under Odin32. This
is not yet implemented but is being worked on.)

Note. The name changed from Pe2Lx to Xx2Lx. It is in prepare for Elf2Lx.


3.0 Applications known to be working
------------------------------------

- Flash 11
- OpenJDK6

Note that many other applications load and/or partly work.


4.0 Reporting Problems and Successes
------------------------------------

Bug reports can be filed in the ticket system at http://svn.netlabs.org/odin32/.


5.0 Project Odin Source Code Notes
----------------------------------

5.1.0 Introduction
------------------

This chapter is intended for people more familiar with ODIN. It describes
the various aspects of the project such as accessing the latest sources via
svn, building the binaries, running win32 applications, how to identify and
report problems and bugs, as well as how to participate in this very
promising project.


5.1.1 Prerequisites:
--------------------

Required compilers/tools:
        - GCC 4.4.6 (available as RPM)
        - OS/2 Toolkit 4.5
        - kBuild utility (available as RPM)

Changes from last source code release:
        - Uses Wine headers and a stripped down version of os2win.h (Open32
          header)
        - All Open32 apis have the 'O32_' prefix.
        - All Open32 declarations are appended with '_O32' (mostly due to
          calling convention differences)
        - kmk BUILD_TYPE=debug builds the debug version
          kmk         builds the release version
        - Several dlls (i.e. USER32, KERNEL32, ADVAPI32) contain extra .def
          files that are used to create libraries that contain the api names
          as required by the linker when you use them. (_apiname@nrparameterbytes)

Wine header notes:
        - Several header files have been changed to make them usable for GCC
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


5.2.0 Accessing ODIN source code via SVN
----------------------------------------

Please check out http://svn.netlabs.org/odin32/.


5.3.0 Building the binaries
---------------------------

The first time you want to build Odin, you must copy the LocalConfig.kmk.tpl
to LocalConfig.kmk and adjust it to your environment.

If your system setup meets above stated requirements, everything you
need to do next for a binary build is:

  kmk BUILD_TYPE=debug

or alternatively

  kmk

As the current state of the project is still in early stage, one should
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


5.3.1 Build problems
--------------------

ODIN is moving at a fast pace. Sometimes side effects of source dependencies may
require you to do a full rebuild of the binaries instead of an incremental build.

However, rarely it also happens the sources contained in the SVN repository do
not build completely due to erroneous source code. Usually, this is fixed within
two days at maximum.
In such event, one can normally stick with the previous version of the particular
module to further test win32 applications on ODIN.

Please report such build problems through the appropriate channel.


5.3.2 Downloading daily binaries
--------------------------------

We do not provide any daily binaries anymore.


5.4.0 Project participation
---------------------------

As ODIN became an open source project, everybody is kindly invited to
contribute his/her share to the progress of the project. May it be
active coding, fixing bugs or just providing detailed information about
examined problems.

We suggest you subscribe to odin-user@netlabs.org.
In case you are interested in participating, every member of the project will
be happy to give you direction to the right places and to give a personal
introduction to further development of the particular modules.


6.0 Warranty
------------

EXCEPT AS OTHERWISE RESTRICTED BY LAW, THIS WORK IS PROVIDED
WITHOUT ANY EXPRESSED OR IMPLIED WARRANTIES OF ANY KIND, INCLUDING
BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A
PARTICULAR PURPOSE, MERCHANTABILITY OR TITLE.  EXCEPT AS
OTHERWISE PROVIDED BY LAW, NO AUTHOR, COPYRIGHT HOLDER OR
LICENSOR SHALL BE LIABLE TO YOU FOR DAMAGES OF ANY KIND, EVEN IF
THEY HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
