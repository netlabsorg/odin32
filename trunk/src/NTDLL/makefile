# $Id: makefile,v 1.25 2000-11-19 12:56:37 sandervl Exp $

#
# Odin32 API
#
#       ntdll.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ntdll.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\exception.obj \
$(OBJDIR)\nt.obj \
$(OBJDIR)\om.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\rtl.obj \
$(OBJDIR)\rtlstr.obj \
$(OBJDIR)\sec.obj \
$(OBJDIR)\sync.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\arith64.obj \
$(OBJDIR)\regfunc.obj \
$(OBJDIR)\ntdllrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/crtdll.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = ntdll


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

