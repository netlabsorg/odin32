# $Id: makefile,v 1.5 2000-12-02 23:55:36 bird Exp $

#
# Odin32 API
#
#       mciseq.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\mciseq.obj \
$(OBJDIR)\mciseqrsrc.obj \
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
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = mciseq


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

