; $Id: Readme.txt,v 1.1 1999-05-24 20:19:05 ktk Exp $ 

15/05/99: Project Odin Source Code Notes
========================================

Required compilers/tools:
	- IBM VisualAge 3.0 (CTx8 fixpack applied)
	  (version 3.6 might also work, but version 4 is not
 	   recommended)
	- ALP 4.0 (IBM Assembly Language Processor)
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
	- OLE32: David Raison is currently changing it to make it compile with
          the Wine headers. His changes will also be included in a future 
          release.

Wine header notes:
	- Several header files have been changed to make them usable for VAC
	- Several header files contain bugfixes (mostly parameter type changes)
	- Several header files include additional declarations
	- At this time we don't intend to have our changes included in the
	  Wine codebase as header files are not likely to change often.

Coding conventions:
	- Do NOT directly call the Open32 apis unless you're writing
	  a wrapper for it.
	- Use the correct Win32 api when there are two version (ascii & unicode)
	  Calling the standard one (SendMessage) causes a error during compilation.
	  Use SendMessageA instead.
	- Source code style will be decided on in the near future.


15/05/99: Sander van Leeuwen (sandervl@xs4all.nl)
