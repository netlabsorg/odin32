# $Id: makefile,v 1.19 2000-11-19 08:49:02 bird Exp $

#
# Odin32 API
#
#       shlwapi.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\shlwapi.obj \
$(OBJDIR)\shlstub.obj \
$(OBJDIR)\url.obj \
$(OBJDIR)\reg_odin.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\regstream.obj \
$(OBJDIR)\pathcpp.obj \
$(OBJDIR)\path.obj \
$(OBJDIR)\string_odin.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\shlwapirsrc.obj \
$(OBJDIR)\ordinal.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/gdi32.lib \
$(PDWIN32_LIB)/crtdll.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = shlwapi


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

