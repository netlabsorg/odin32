# $Id: makefile,v 1.23 2001-07-20 15:37:52 sandervl Exp $

#
# Odin32 API
#
#       shlwapi.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\shlwapi.obj \
$(OBJDIR)\shlstub.obj \
$(OBJDIR)\url.obj \
$(OBJDIR)\url_odin.obj \
$(OBJDIR)\reg_odin.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\regstream.obj \
$(OBJDIR)\shlwapiguid.obj \
$(OBJDIR)\pathcpp.obj \
$(OBJDIR)\path.obj \
$(OBJDIR)\string_odin.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\shlwapirsrc.obj \
$(OBJDIR)\ordinal.obj \
$(OBJDIR)\ordinal_odin.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
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

