# $Id: makefile,v 1.14 2000-11-22 00:49:56 phaller Exp $

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
$(OBJDIR)\crtdll_main.obj \
$(OBJDIR)\dir.obj \
$(OBJDIR)\exit.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\mbstring.obj \
$(OBJDIR)\memory.obj \
$(OBJDIR)\spawn.obj \
                     \
$(OBJDIR)\crtdll.obj \
$(OBJDIR)\asmhlp.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\internal.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\crt_memory.obj \
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
