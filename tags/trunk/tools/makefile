# $Id: makefile,v 1.22 2003-10-24 14:52:51 sandervl Exp $

#
# Odin32 API
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
!include ../makefile.inc


#
# The directories included in the usual makes; all, clean, dep rules.
#
ALL_DIRS = \
.\cmdqd \
#.\profilerfix \
.\common \
.\fastdep \
.\impdef \
.\install \
.\regedit \
.\database \
.\omfdumper \
.\pebuild \
.\sdf \
.\wrc \


#
# Dummy all rule.
#
_all: all


#
# common rules.
#
$(COMMONRULES):
    @$(DODIRS) "$(ALL_DIRS)" $(MAKE_CMD) $@


#
# compile tools needed to compile rest of the tree.
#
needed:
    @$(DODIRS) "cmdqd fastdep common impdef pebuild wrc" $(MAKE_CMD)


#
# Do not includes the common rules.
#

