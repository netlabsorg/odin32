# $Id: makefile,v 1.25 2000-11-19 09:20:50 bird Exp $
#
# ddraw.dll makefile
#
# Copyright 1998 Sander van Leeuwen
#           1999 Markus Montkowski
#
# Project Odin Software License can be found in LICENSE.TXT
#
#



#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ddraw.obj \
$(OBJDIR)\os2ddraw.obj \
$(OBJDIR)\os2clipper.obj \
$(OBJDIR)\os2d3d.obj \
$(OBJDIR)\iccio1.obj \
$(OBJDIR)\asmutil.obj \
$(OBJDIR)\os2surface.obj \
$(OBJDIR)\os2palette.obj \
$(OBJDIR)\os2palset.obj \
$(OBJDIR)\rectangle.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\os2util.obj \
$(OBJDIR)\bltFunc.obj \
$(OBJDIR)\fillfunc.obj \
$(OBJDIR)\colorconv.obj \
$(OBJDIR)\ddrawrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/ole32.lib \
$(PDWIN32_LIB)/advapi32.lib \
$(PDWIN32_LIB)/gdi32.lib \
$(PDWIN32_LIB)/comctl32.lib \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/user32.lib \
mmpm2.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = ddraw


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

