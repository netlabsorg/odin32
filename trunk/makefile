# $Id: makefile,v 1.13 2000-11-20 05:00:50 bird Exp $

#
# PD-Win32 API
#
#	Top of the tree makefile
#
#
#	Usage: nmake ( debug | nodebuginfo | release | all | dep | clean )
#
#            debug: Change to a debug build.
#            debugsmp: Start nmake process in background that processes all
#                      dlls in reverse
#            nodebuginfo: Change to a debug build without debug info in binaries
#            nodebuginfosmp: Change to an SMP debug build without debug info in binaries
#            release: Change to a release build.
#            releasesmp: Change to an SMP release build.
#            all: Build the entire tree.
#            dep: Make dependencies for the entire tree.
#            clean: Bring tree back to a "virgin" state.
#
#
#

#
# Directory macros.
#
PDWIN32_INCLUDE = .\include
PDWIN32_TOOLS   = .\tools
PDWIN32_LIB     = .\lib
PDWIN32_BIN     = .\bin


#
# Include makefile. (Not completeled)
#
#!include $(PDWIN32_INCLUDE)/pdwin32.mk

all:		odin_libraries  needed_tools
    cd src
    nmake -nologo  all
    cd ..\tools\install
    nmake -nologo  all

clean:
    cd lib
    nmake -nologo clean
    cd ..\tools
    nmake -nologo clean
    cd ..\src
    nmake -nologo clean

debug:		odin_libraries  needed_tools
    cd src
    nmake -nologo all DEBUG=1
    cd ..\tools\install
    nmake -nologo all DEBUG=1

debugsmp:	odin_libraries  needed_tools
    cd src
    nmake -nologo DEBUG=1 smp
    cd ..\tools\install
    nmake -nologo DEBUG=1 all

nodebuginfo:	odin_libraries  needed_tools
    cd src
    nmake -nologo all DEBUG=1 NODEBUGINFO=1
    cd ..\tools\install
    nmake -nologo all DEBUG=1 NODEBUGINFO=1

nodebuginfosmp:	odin_libraries  needed_tools
    cd src
    nmake -nologo DEBUG=1 NODEBUGINFO=1 smp
    cd ..\tools\install
    nmake -nologo DEBUG=1 NODEBUGINFO=1 all

release:	odin_libraries  needed_tools
    SET DEBUG=
    cd src
    nmake -nologo all
    cd ..\tools\install
    nmake -nologo all

releasesmp:	odin_libraries  needed_tools
    SET DEBUG=
    cd src
    nmake -nologo smp
    cd ..\tools\install
    nmake -nologo all

dep: needed_tools
    cd tools
    nmake -nologo dep
    cd ..\src
    nmake -nologo dep


# --- common section ---
odin_libraries:
	cd lib
	nmake -nologo
	cd ..


needed_tools:
    cd tools
    nmake needed
    cd ..
