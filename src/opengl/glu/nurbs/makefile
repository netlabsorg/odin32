# $Id: makefile,v 1.8 2000-12-03 00:26:15 bird Exp $

#
# Odin32 API
#
#       glu/nurbs makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../../../makefile.inc


#
# Subdirectories to travers.
#
SUBDIRS = \
.\interface \
.\internals \
.\nurbtess


#
# Dummy all rule - nmake without target will make all targets of the first rule.
#
all_: all


#
# All, libs, dep and clean rules.
#
clean dep all libs:
    $(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@
