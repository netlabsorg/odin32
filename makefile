# $Id: makefile,v 1.16 2000-12-09 16:04:55 phaller Exp $

#
# Odin32
#
#   Top of the tree makefile
#
#
#   Usage: nmake ( debug | nodebuginfo | profile | release | all | dep | clean )
#
#            debug: Change to a debug build.
#            debugsmp: Start nmake process in background that processes all
#                      dlls in reverse
#            nodebuginfo: Change to a debug build without debug info in binaries
#            nodebuginfosmp: Change to an SMP debug build without debug info in binaries
#            profile: Change to a debug build with profiling hooks
#            release: Change to a release build.
#            releasesmp: Change to an SMP release build.
#            all: Build the entire tree.
#            dep: Make dependencies for the entire tree.
#            clean: Bring tree back to a "virgin" state.
#
#
#


#
# Check that makefile.inc exists, If it doesn't we'll try make it.
#
!if [.\tools\bin\Exists.cmd makefile.inc] == 1
!   if [configure.cmd noninteractive]
!   endif
!endif


#
# Include makefile.
#
!include makefile.inc

all:        odin_libraries  needed_tools
    cd src
    $(MAKE) -nologo  all
    cd ..\tools\install
    $(MAKE) -nologo  all

clean:
    cd lib
    $(MAKE) -nologo clean
    cd ..\tools
    $(MAKE) -nologo clean
    cd ..\src
    $(MAKE) -nologo clean

profile:      odin_libraries  needed_tools
    cd src
    $(MAKE) -nologo all DEBUG=1 PROFILE=1
    cd ..\tools\install
    $(MAKE) -nologo all DEBUG=1 PROFILE=1

debug:      odin_libraries  needed_tools
    cd src
    $(MAKE) -nologo all DEBUG=1
    cd ..\tools\install
    $(MAKE) -nologo all DEBUG=1

debugsmp:   odin_libraries  needed_tools
    cd src
    $(MAKE) -nologo DEBUG=1 smp
    cd ..\tools\install
    $(MAKE) -nologo DEBUG=1 all

nodebuginfo:    odin_libraries  needed_tools
    cd src
    $(MAKE) -nologo all DEBUG=1 NODEBUGINFO=1
    cd ..\tools\install
    $(MAKE) -nologo all DEBUG=1 NODEBUGINFO=1

nodebuginfosmp: odin_libraries  needed_tools
    cd src
    $(MAKE) -nologo DEBUG=1 NODEBUGINFO=1 smp
    cd ..\tools\install
    $(MAKE) -nologo DEBUG=1 NODEBUGINFO=1 all

release:    odin_libraries  needed_tools
    SET DEBUG=
    cd src
    $(MAKE) -nologo all
    cd ..\tools\install
    $(MAKE) -nologo all

releasesmp: odin_libraries  needed_tools
    SET DEBUG=
    cd src
    $(MAKE) -nologo smp
    cd ..\tools\install
    $(MAKE) -nologo all

dep: needed_tools
    cd tools
    $(MAKE) -nologo dep
    cd ..\src
    $(MAKE) -nologo dep


# --- common section ---
odin_libraries:
    cd lib
    $(MAKE) -nologo
    cd ..


needed_tools:
    cd tools
    $(MAKE) -nologo needed
    cd ..

