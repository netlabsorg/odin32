# $Id: makefile,v 1.1 2001-11-22 10:43:59 phaller Exp $

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

LIBS = demangl.lib

#
# Target name - name of the dll without extention and path.
#
TARGET = odinprof


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

