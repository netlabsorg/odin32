# $Id: makefile,v 1.12 2001-07-21 17:39:44 sandervl Exp $

#
# Odin32 API
#
#       msvfw32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\msvfw32.obj \
$(OBJDIR)\drawdib.obj \
$(OBJDIR)\msvideo_main.obj \
$(OBJDIR)\msvfw32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/version.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = msvfw32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

