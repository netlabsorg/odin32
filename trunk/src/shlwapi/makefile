# $Id: makefile,v 1.27 2002-06-07 08:02:17 sandervl Exp $

#
# Odin32 API
#
#       shlwapi.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!if "$(DEBUG)" == "1"
DEFFILE    = shlwapidbg.def
ORGDEFFILE = shlwapi.def
!endif

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
!if "$(DEBUG)" == "1"
$(OBJDIR)\dbgwrap.obj \
!endif
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
# Target name - name of the dll without extention and path.
#
TARGET = shlwapi


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

