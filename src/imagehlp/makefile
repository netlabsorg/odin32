# $Id: makefile,v 1.13 2000-12-02 23:39:44 bird Exp $

#
# Odin32 API
#
#       imagehlp.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\imagehlp.obj \
$(OBJDIR)\access.obj \
$(OBJDIR)\debug.obj \
$(OBJDIR)\integrity.obj \
$(OBJDIR)\internal.obj \
$(OBJDIR)\modify.obj \
$(OBJDIR)\symbol.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\imagehlprsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = imagehlp


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

