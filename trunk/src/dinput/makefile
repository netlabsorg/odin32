# $Id: makefile,v 1.8 2000-11-19 09:20:11 bird Exp $

#
# Odin32 API
#
#       dinput.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dinput.obj \
$(OBJDIR)\oslibinput.obj \
$(OBJDIR)\dinputguid.obj \
$(OBJDIR)\dinputrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O) \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/ole32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = dinput


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

