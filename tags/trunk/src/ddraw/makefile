# $Id: makefile,v 1.35 2003-08-08 15:43:12 sandervl Exp $

#
# Odin32 API
#
#       ddraw.dll makefile
#


#
# Compiler, tools, and inference rules.
#
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ddmain.obj \
$(OBJDIR)\ddclipper.obj \
$(OBJDIR)\ddraw2d.obj \
$(OBJDIR)\clipper.obj \
$(OBJDIR)\oslibgpi.obj \
$(OBJDIR)\ddraw3d.obj \
##$(OBJDIR)\iccio1.obj \
$(OBJDIR)\asmutil.obj \
$(OBJDIR)\surface.obj \
$(OBJDIR)\surfblit.obj \
$(OBJDIR)\surfoverlay.obj \
$(OBJDIR)\surfacehlp.obj \
$(OBJDIR)\wndproc.obj \
$(OBJDIR)\palette.obj \
$(OBJDIR)\os2palset.obj \
$(OBJDIR)\os2fsdd.obj \
$(OBJDIR)\rectangle.obj \
$(OBJDIR)\initddraw.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\divewrap.obj \
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
$(ODIN32_LIB)/libwrap.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/user32.lib \
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

