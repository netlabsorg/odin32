# $Id: pe.mak,v 1.3 2000-12-02 23:57:55 bird Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
EXETARGET = 1
MAKEFILE = pe.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
LD2FLAGS = $(LD2FLAGS) /PMTYPE:pm /STACK:0x100000 /NOBASE
!else
!error Compiler not supported yet
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\pe.obj


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
TARGET  = pe


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

