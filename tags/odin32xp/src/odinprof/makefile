# $Id: makefile,v 1.7 2004-12-04 10:22:34 sao2l02 Exp $

#
# Odin32 API
#
#       odinprof.dll makefile
#


#
# Tell the buildenvironment not to
#
NOTEXPDEF   = 1
STATIC_CRT  = 1

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
OBJ_PROFILE =
LIB_PROFILE =


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
LIBS = $(LIB_DEMANGLE) \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
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

