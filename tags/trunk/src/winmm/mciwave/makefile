# $Id: makefile,v 1.5 2000-12-02 23:55:38 bird Exp $

#
# Odin32 API
#
#       mciwave.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\mciwave.obj \
$(OBJDIR)\mciwaversrc.obj \
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
TARGET = mciwave


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

