# $Id: makefile,v 1.2 2002-02-23 23:50:47 bird Exp $

#
# Odin32 API
#
#       odinprof.dll makefile
#


#
# Tell the buildenvironment not to
#
NOTEXPDEF   = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CXXFLAGS    = $(CXXFLAGS_ODINCRT)
LD2FLAGS    = $(LD2FLAGS_ODINCRT)
CDEFINES    = $(CDEFINES_ODINCRT)
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase
DEFFILE     = odinprof.def


#
# Object files. All objects should be prefixed with $(OBJDIR)!
#
OBJS = \
$(OBJDIR)\odinprof.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\profiler.obj \
$(OBJDIR)\profasm.obj \
$(OBJDIR)\profcollection.obj \
$(OBJDIR)\symfile.obj


#
# Libraries. One space before the '\'.
#
LIBS = $(LIB_DEMANGLE)
!if "$(LIB_DEMANGLE)" == ""
!error demangle not configured for this compiler
!endif


#
# Target name - name of the dll without extention and path.
#
TARGET = odinprof


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

