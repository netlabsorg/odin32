# $Id: makefile,v 1.12 2000-09-28 03:06:26 bird Exp $

#
# PD-Win32 API
#
#       Top of the tools tree makefile
#
#
#       Usage: nmake ( all | needed | dep | clean )
#
#            all: Build the entire tools tree.
#            needed: Builds the tools used during compilation of the src tree.
#            dep: Make dependencies for the tools tree.
#            clean: Bring tools tree back to a "virgin" state.
#
#
#
PDWIN32_INCLUDE = ..\include
!include $(PDWIN32_INCLUDE)\pdwin32.tools

!ifdef DEBUG
MAKE_CMD = $(MAKE) -nologo DEBUG=1
!else
MAKE_CMD = $(MAKE) -nologo
!endif


all:
    cd common
    $(MAKE_CMD)
    cd ..\database
    $(MAKE_CMD)
    cd ..\impdef
    $(MAKE_CMD)
    cd ..\fastdep
    $(MAKE_CMD)
    cd ..\wrc
    $(MAKE_CMD) -f makefile.os2
    cd ..\omfdumper
    $(MAKE_CMD)
    cd ..\sdf
    $(MAKE_CMD)
    cd ..\install
    $(MAKE_CMD)
    cd ..

# compile tools needed to compile rest of the tree.
needed:
    cd fastdep
    $(MAKE_CMD)
    cd ..\common
    $(MAKE_CMD) NODEP=1 dep
    $(MAKE_CMD) needed
    cd ..\impdef
    $(MAKE_CMD) NODEP=1 dep
    $(MAKE_CMD)
    cd ..


dep:
    cd common
    $(MAKE_CMD) NODEP=1 dep
    cd ..\database
    $(MAKE_CMD) NODEP=1 dep
    cd ..\impdef
    $(MAKE_CMD) NODEP=1 dep
    cd ..\wrc
    $(MAKE_CMD) -f makefile.os2 NODEP=1 dep
    cd ..\omfdumper
    $(MAKE_CMD) NODEP=1 dep
    cd ..\sdf
    $(MAKE_CMD) NODEP=1 dep
    cd ..


clean:
    cd common
    $(MAKE_CMD) NODEP=1 clean
    cd ..\database
    $(MAKE_CMD) NODEP=1 clean
    cd ..\impdef
    $(MAKE_CMD) NODEP=1 clean
    cd ..\fastdep
    $(MAKE_CMD) NODEP=1 clean
    cd ..\wrc
    $(MAKE_CMD) -f makefile.os2 NODEP=1 clean
    cd ..\omfdumper
    $(MAKE_CMD) NODEP=1 clean
    cd ..\sdf
    $(MAKE_CMD) NODEP=1 clean
    cd ..

