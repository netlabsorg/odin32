# $Id: makefile,v 1.28 2004-12-30 18:45:14 sao2l02 Exp $

#
# Odin32 API
#
#       shlwapi.dll makefile
#

#
# Target name - name of the dll without extention and path.
#
TARGET = shlwapi

#
# Compiler, tools, and interference rules.
#
ODIN32_DBGWRAP = 1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\shlwapi.obj \
$(OBJDIR)\shlwapi_main.obj \
$(OBJDIR)\shlstub.obj \
$(OBJDIR)\url.obj \
$(OBJDIR)\url_odin.obj \
$(OBJDIR)\reg_odin.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\regstream.obj \
$(OBJDIR)\path.obj \
$(OBJDIR)\string_odin.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\ordinal.obj \
$(OBJDIR)\ordinal_odin.obj \
$(OBJDIR)\wsprintf.obj \
$(OBJDIR)\shlwapirsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/guidlib.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
