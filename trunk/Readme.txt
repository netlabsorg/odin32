/* $Id: Readme.txt,v 1.9 1999-11-14 13:14:16 achimha Exp $ */

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
codes. This is also refered to as Odin32 whereas Odin is the name
of the Win32 binary emulator.

1.2 Current Status
------------------

Odin is still in the early alpha state so that it is very far from
being widely usable. Most programs will not work or show many
problems. Even though it is not usable for most programs, we have
decided to do a public release in order to show how much progress
the project has been made. A second goal is to attract more developers
in order to speedup development. Our job includes rewriting most
parts of Windows 95/NT without having the original source codes so
the more developers we have the more progess will be seen. The
last public release (Alpha 0.004) has been in 1997 and the project
has changed much since then. Everything is free and Open Source now
and the base architecture has been improved quite a lot. We are now
in a position where we can expect the first bigger applications to
run well and to make much more visible progress.


2.0 Using Odin to run your Windows applications
-----------------------------------------------

Odin still has several ways to execute Win32 programs.


2.1 The Ring3 Win32 Loader (PE.EXE)
----------------------------------

This is the new and also recommended method for loading programs.
Unlike with the previous alphas, it is not required to convert or
anyhow change the original files. Installing a device driver is
also not necessary. Be sure to put the Odin binary directory into
both the PATH and LIBPATH (alternatively you can use a batch file
with SET PATH=C:\Odin\bin;%PATH% and SET BEGINLIBPATH=C:\Odin\bin;)
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


2.1.1 PE loader issues
----------------------

As the PE loader will create the process and load the program,
a process utility such as pstat will always show PE.EXE as the
executable file.
It is also possible that a certain Win32 applications becomes
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


2.2 The Ring0 conversion utility driver (WIN32K.SYS)
----------------------------------------------------


2.3 The Ring3 conversion utility (PE2LX.EXE)
--------------------------------------------

PE2LX.EXE was the first method of running Win32 programs under
OS/2. It is still provided and also functional but it has been
deprecated. Using it is only sensible for debugging certain
problems as it requires all DLLs and EXEs to be converted into
another file format thus changing the original Win32 files.


3.0 Applications known to be working
------------------------------------

- Windows 95/NT Help Viewer (winhlp32.exe)
- Windows 95/NT Registry Editor
- Windows NT Solitaire (not the one from Windows 95)
- Windows NT Freecell (not the one from Windows 95)
- Windows 95/NT Calculator
- Windows NT Pinball
- Quake II
- Red Alert
- Professional Minesweeper
- Windiff (part of Win32 SDK)


Project Odin Source Code Notes
==============================

1.0 Introduction
----------------

This readme is intended for making people more familiar with ODIN and to 
describe various aspects of the project such as accessing latest sources via 
cvs, building the binaries, running win32 applications, how to identify and 
report problems and bugs, as well as how to acticely participate in this very 
promising project.

99/11/05: Sander van Leeuwen <sandervl@xs4all.nl>
99/11/05: Patrick Haller <phaller@gmx.net>
99/11/14: Achim Hasenmueller <achimha@innotek.de>


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

  make debug

or alternatively

  make release

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


3.1 Build problems
------------------

ODIN is moving at fast pace. Sometimes side effects of source dependencies may 
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


4.0 Running Win32 applications on ODIN
--------------------------------------


5.0 How to identify and report problems
---------------------------------------


5.1 Understanding the logfiles
------------------------------


5.2 Reporting a problem
-----------------------


6.0 Project participation
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


