# $Id: pe.mak,v 1.1 2000-11-19 08:57:05 bird Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
EXETARGET = 1


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Overrides.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
LD2FLAGS = /PMTYPE:pm /STACK:0x100000 /NOBASE
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
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = pe


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

