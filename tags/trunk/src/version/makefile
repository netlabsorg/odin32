# $Id: makefile,v 1.16 2000-11-19 08:39:49 bird Exp $

#
# Odin32 API
#
#       version.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\version.obj \
$(OBJDIR)\install.obj \
$(OBJDIR)\info.obj \
$(OBJDIR)\versionrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/crtdll.lib \
$(PDWIN32_LIB)/lz32.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = version


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

