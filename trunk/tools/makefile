# $Id: makefile,v 1.2 1999-09-11 23:45:36 bird Exp $

#
# PD-Win32 API
#
#       Top of the tools tree makefile
#
#
#       Usage: nmake ( all | needed | clean )
#
#            all: Build the entire tools tree.
#            needed: Builds the tools used during compilation of the src tree.
#            clean: Bring tools tree back to a "virgin" state.
#
#
#
!include ..\include\pdwin32.tools

!ifdef DEBUG
MAKE_CMD = nmake -nologo DEBUG=1
!else
MAKE_CMD = nmake -nologo
!endif


all:
        cd common
        $(MAKE_CMD)
        cd ..\database
        $(MAKE_CMD)
        cd ..\impdef
        $(MAKE_CMD)
        cd ..\wrc
        $(MAKE_CMD)
        cd ..

# compile tools needed to compile rest of the tree.
needed:
        cd common
        $(MAKE_CMD) needed
        cd ..\impdef
        $(MAKE_CMD)
        cd ..

clean:
        cd common
        $(MAKE_CMD) clean
        cd ..\database
        $(MAKE_CMD) clean
        cd ..\impdef
        $(MAKE_CMD) clean
        cd ..\wrc
        $(MAKE_CMD) clean
        cd ..\bin
        -$(RM) *.exe *.dll
        cd ..

