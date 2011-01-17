# $Id: pec.mak,v 1.6 2002-12-30 19:05:38 sandervl Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
STACKSIZE = 0x100000
VIO = 1
EXETARGET = 1
MAKEFILE = pec.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc

#
# Overrides.
#
CXXFLAGS    = $(CXXFLAGS_ODINCRT)
LD2FLAGS    = $(LD2FLAGS_ODINCRT)


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\pec.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB_O) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = pec


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Special rule for pec.obj
#
$(OBJDIR)\pec.obj: pe.cpp
!if "$(VAC3)" == "1" || "$(VAC36)" == "1" || "$(WAT)" == "1"
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -DCOMMAND_LINE_VERSION -Fo$@ $**
!else
!error Compiler not supported yet
!endif

