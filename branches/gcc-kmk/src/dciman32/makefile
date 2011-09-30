# $Id: makefile,v 1.3 2000-12-02 23:38:47 bird Exp $

#
# Odin32 API
#
#       dciman32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dciman32.obj \
$(OBJDIR)\dcimanrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = dciman32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

