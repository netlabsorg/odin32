# $Id: msvcrt40.mak,v 1.1 2000-11-19 09:09:12 bird Exp $

#
# Odin32 API
#
#       msvcrt.dll makefile
#


#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = msvcrt40.mak


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\msvcrt.obj \
$(OBJDIR)\critsec.obj \
$(OBJDIR)\util.obj \
$(OBJDIR)\$(TARGET)rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/crtdll.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = msvcrt40


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

