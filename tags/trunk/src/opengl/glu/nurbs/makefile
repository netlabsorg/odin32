# $Id: makefile,v 1.6 2000-03-06 23:33:48 bird Exp $

#
# Odin32 API
#
#       glu/nurbs makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# All, libs, dep and clean rule.
clean dep all libs:
    cd interface
    $(MAKE_CMD) $@
    cd ..\internals
    $(MAKE_CMD) $@
    cd ..\nurbtess
    $(MAKE_CMD) $@
    cd ..

