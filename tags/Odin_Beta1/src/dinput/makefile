# $Id: makefile,v 1.10 2001-12-06 10:13:59 sandervl Exp $

#
# Odin32 API
#
#       dinput.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dinput.obj \
$(OBJDIR)\oslibinput.obj \
$(OBJDIR)\dinputrsrc.obj \
$(DLLENTRY)


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

