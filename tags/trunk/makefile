# $Id: makefile,v 1.2 1999-06-01 08:43:08 phaller Exp $ 

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

all:		odin_libraries
		cd src
		nmake -nologo all

clean:		odin_libraries
		cd src
		nmake -nologo clean

debug:		odin_libraries
		cd src
		nmake -nologo all DEBUG=1

release:	odin_libraries
		cd src
		nmake -nologo all 



# --- common section ---
odin_libraries:
	cd lib
	nmake -nologo
	cd ..

