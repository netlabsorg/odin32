# $Id: makefile,v 1.12 2000-11-19 09:15:10 bird Exp $

#
# Odin32 API
#
#       imagehlp.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


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
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = imagehlp


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

