# $Id: makefile,v 1.1 1999-05-24 20:19:05 ktk Exp $ 

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

all:
	cd src
	nmake -nologo all

clean:
	cd src
	nmake -nologo clean

debug:
	cd src
	nmake -nologo all DEBUG=1

release:
	cd src
	nmake -nologo all 
