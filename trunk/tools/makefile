# $Id: makefile,v 1.13 2000-11-19 08:20:55 bird Exp $

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


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\include
PDWIN32_TOOLS   = .\bin
!include $(PDWIN32_INCLUDE)\pdwin32.tools


#
# The directories included in the usual makes; all, clean, dep rules.
#
ALL_DIRS = \
.\common \
.\database \
.\impdef \
.\fastdep \
.\wrc \
.\omfdumper \
.\sdf \
.\install


#
# Dummy all rule.
#
_all: all


#
# common rules.
#
dep clean all:
    $(DODIRS) "$(ALL_DIRS)" $(MAKE_CMD) $@


#
# compile tools needed to compile rest of the tree.
#
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


#
# Do not includes the common rules.
#

