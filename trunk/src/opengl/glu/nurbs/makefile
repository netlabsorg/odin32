# $Id: makefile,v 1.7 2000-11-19 08:58:13 bird Exp $

#
# Odin32 API
#
#       glu/nurbs makefile
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk


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
