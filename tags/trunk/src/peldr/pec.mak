# $Id: pec.mak,v 1.3 2000-12-02 23:57:55 bird Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
EXETARGET = 1
MAKEFILE = pec.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
LD2FLAGS = $(LD2FLAGS) /PMTYPE:vio /STACK:0x100000 /NOBASE
!else
!error Compiler not supported yet
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\pec.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
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
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -DCOMMAND_LINE_VERSION -Fo$@ -c $**
!else
!error Compiler not supported yet
!endif

