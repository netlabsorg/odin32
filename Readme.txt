/* $Id: Readme.txt,v 1.8 1999-11-05 14:27:09 sandervl Exp $ */

1999/11/05: Project Odin Source Code Notes
==========================================

1.0 Introduction
----------------

This readme is intended for making people more familiar with ODIN and to 
describe various aspects of the project such as accessing latest sources via 
cvs, building the binaries, running win32 applications, how to identify and 
report problems and bugs, as well as how to acticely participate in this very 
promising project.

99/11/05: Sander van Leeuwen (sandervl@xs4all.nl)
99/11/05: Patrick Haller     (phaller@gmx.net)


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
	- Watcom C/C++ can also be used for pe2lx

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

We suggest you subscribe to win32os2-WAI and the corresponsing mailing lists
on http://www.egroups.com.
In case you are interested in participating, every member of the project will 
be happy to give you direction to the right places and to give a personal 
introduction to further development of the particular modules.


