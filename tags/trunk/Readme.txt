/* $Id: Readme.txt,v 1.6 1999-06-30 17:27:58 sandervl Exp $ */

39/06/99: Project Odin Source Code Notes
========================================

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
	- Uses Wine headers and a stripped down version of os2win.h (Open32 header)
	- All Open32 apis have the 'O32_' prefix.
	- All Open32 declarations are appended with '_O32' (mostly due to 
          calling convention differences)
	- nmake DEBUG=1 builds the debug version
          nmake         builds the release version
	- The USER32, KERNEL32, ADVAPI32, COMDLG32, GDI32 contain extra .def
	  files that are used to create libraries that contain the api names
	  as required by the linker when you use them. (_apiname@nrparameterbytes)
NOTE:	- It's not unlikely this rewrite contains extra bugs due to the huge
          number of small changes I had to make.
	  There might be a few functions that call themselves instead of the
          Open32 apis. 

Non-functional code:
	- DDRAW & DSOUND: Markus Montkowski will take care of this and his
          changes will be included in a future release

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
	  Calling the standard one (SendMessage) causes a error during compilation.
	  Use SendMessageA instead.
	- Source code style will be decided on in the near future.

19/06/99: Sander van Leeuwen (sandervl@xs4all.nl)
