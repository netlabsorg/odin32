# $Id: makefile,v 1.12 2000-11-19 09:22:35 bird Exp $

#
# Odin32 API
#
#       crtdll.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


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
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/pmwinx.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = crtdll


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

