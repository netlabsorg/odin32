# $Id: makefile,v 1.32 2002-06-15 11:16:52 sandervl Exp $

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
$(OBJDIR)\wcstring.obj \
$(OBJDIR)\sec.obj \
$(OBJDIR)\sync.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\large_int.obj \
$(OBJDIR)\regfunc.obj \
$(OBJDIR)\error.obj \
$(OBJDIR)\loader.obj \
##$(OBJDIR)\heap.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\string.obj \
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

