# $Id: makefile,v 1.2 2000-02-09 23:49:55 bird Exp $

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

dep: # dummy rule

clean:
   -@$(RM) *.obj
   -@$(RM) *.exe

