# $Id: makefile,v 1.11 2002-04-30 14:52:03 sandervl Exp $

#
# Odin32 API
#
#       dinput.dll makefile
#

WRC_PREFIX_RESOURCE=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\device.obj \
$(OBJDIR)\keyboard.obj \
$(OBJDIR)\mouse.obj \
$(OBJDIR)\dinput_main.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initdinput.obj \
$(OBJDIR)\dinputrsrc.obj 


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O) \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/guidlib.lib

#
# Target name - name of the dll without extention and path.
#
TARGET = dinput


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

