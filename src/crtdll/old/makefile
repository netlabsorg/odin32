# $Id: makefile,v 1.2 2000-12-02 23:37:31 bird Exp $

#
# Odin32 API
#
#       crtdll.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\crtdll.obj \
$(OBJDIR)\asmhlp.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\internal.obj \
$(OBJDIR)\crt_string.obj \
$(OBJDIR)\crt_memory.obj \
$(OBJDIR)\crt_mb.obj \
$(OBJDIR)\crt_wc.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\crtdllrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/pmwinx.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = crtdll


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

