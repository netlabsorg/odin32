# $Id: makefile,v 1.28 2001-05-28 17:17:15 phaller Exp $

#
# Odin32 API
#
#       ddraw.dll makefile
#


#
# Compiler, tools, and inference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ddraw.obj \
$(OBJDIR)\ddrawclipper.obj \
$(OBJDIR)\os2ddraw.obj \
$(OBJDIR)\os2clipper.obj \
$(OBJDIR)\os2d3d.obj \
$(OBJDIR)\iccio1.obj \
$(OBJDIR)\asmutil.obj \
$(OBJDIR)\os2surface.obj \
$(OBJDIR)\os2palette.obj \
$(OBJDIR)\os2palset.obj \
$(OBJDIR)\os2fsdd.obj \
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
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/user32.lib \
mmpm2.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extension and path.
#
TARGET = ddraw


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

