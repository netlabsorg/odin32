# $Id: makefile,v 1.11 2000-05-02 11:24:52 bird Exp $

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

all:		odin_libraries  needed_tools
    cd src
    nmake -nologo all
    cd ..\tools\install
    nmake -nologo all

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
    start nmake -i -f makefile.smp -nologo all DEBUG=1
    nmake -nologo all DEBUG=1
    cd ..\tools\install
    nmake -nologo all DEBUG=1

nodebuginfo:	odin_libraries  needed_tools
    cd src
    nmake -nologo all DEBUG=1 NODEBUGINFO=1
    cd ..\tools\install
    nmake -nologo all DEBUG=1 NODEBUGINFO=1

nodebuginfosmp:	odin_libraries  needed_tools
    cd src
    start nmake -i -f makefile.smp -nologo all DEBUG=1 NODEBUGINFO=1
    nmake -nologo all DEBUG=1 NODEBUGINFO=1
    cd ..\tools\install
    nmake -nologo all DEBUG=1 NODEBUGINFO=1

release:	odin_libraries  needed_tools
    cd src
    nmake -nologo all
    cd ..\tools\install
    nmake -nologo all

releasesmp:	odin_libraries  needed_tools
    cd src
    start nmake -i -f makefile.smp -nologo all
    nmake -nologo all
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
