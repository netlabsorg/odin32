# $Id: ddrawlib.mak,v 1.2 2001-10-15 17:20:33 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1
WRC_PREFIX_RESOURCE=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES    = $(CDEFINES) -DINVERT


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
$(OBJDIR)\oslibgpi.obj \
$(OBJDIR)\rectangle.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\os2util.obj \
$(OBJDIR)\bltFunc.obj \
$(OBJDIR)\fillfunc.obj \
$(OBJDIR)\colorconv.obj \
$(OBJDIR)\ddrawrsrc.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = ddrawo


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

