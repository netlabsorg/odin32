# $Id: makefile,v 1.28 2001-07-09 08:07:59 bird Exp $

#
# Odin32 API
#
#       ntdll.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


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
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/crtdll.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = ntdll


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

