# $Id: makefile,v 1.1 1999-09-05 02:34:31 bird Exp $

#
# PD-Win32 API
#
# Very simple OMF/LIB dumper.
#

!include ..\..\include\pdwin32.tools

all: omfdumper.exe ..\bin\omfdumper.exe

omfdumper.exe:  omfdumper.c omf.h
   icc /ti+ /Q+ omfdumper.c

..\bin\omfdumper.exe: omfdumper.exe
   $(CP) $** $@

clean:
   -@$(RM) *.obj
   -@$(RM) *.exe

