# $Id: makefile,v 1.3 2000-11-19 08:26:36 bird Exp $

#
# PD-Win32 API
#
#       odininst.exe makefile
#


#
# Tell that we're producing an executable.
#
EXETARGET = 1


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Overrides
#
LD2FLAGS = $(LD2FLAGS_ODINCRT) /PMTYPE:pm /STACK:0x100000 /NOBASE


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\odininst.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
os2386.lib \
$(PDWIN32_LIB)\kernel32.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = odininst


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)\pdwin32.post

