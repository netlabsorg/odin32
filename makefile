# $Id: makefile,v 1.5 1999-09-14 15:58:42 sandervl Exp $

#
# PD-Win32 API
#
#	Top of the tree makefile
#
#
#	Usage: nmake ( debug | release | all | clean )
#
#            debug: Change to a debug build.
#            release: Change to a release build.
#            all: Build the entire tree.
#            clean: Bring tree back to a "virgin" state.
#
#
#

all:		odin_libraries  needed_tools
		cd src
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

release:	odin_libraries  needed_tools
		cd src
		nmake -nologo all



# --- common section ---
odin_libraries:
	cd lib
	nmake -nologo
	cd ..


needed_tools:
    cd tools
    nmake needed
    cd ..
