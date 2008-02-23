# $Id: makefile,v 1.5 2000-12-02 23:55:35 bird Exp $

#
# Odin32 API
#
#       mcianim.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
mcianim.obj \
mcianimrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/pmwinx.lib \
$(RTLLIB_O) \
OS2386.LIB


#
# Target name - name of the dll without extention and path.
#
TARGET = mcianim


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

