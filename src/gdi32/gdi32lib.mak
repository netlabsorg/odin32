# $Id: gdi32lib.mak,v 1.1 2001-07-20 15:35:57 sandervl Exp $

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
$(OBJDIR)\gdi32.obj \
$(OBJDIR)\icm.obj \
$(OBJDIR)\driver.obj \
$(OBJDIR)\opengl.obj \
$(OBJDIR)\callback.obj \
$(OBJDIR)\dibsect.obj \
$(OBJDIR)\initgdi32.obj \
$(OBJDIR)\oslibgdi.obj \
$(OBJDIR)\font.obj \
$(OBJDIR)\text.obj \
$(OBJDIR)\palette.obj \
$(OBJDIR)\line.obj \
$(OBJDIR)\oslibgpi.obj \
$(OBJDIR)\region.obj \
$(OBJDIR)\transform.obj \
$(OBJDIR)\metafile.obj \
$(OBJDIR)\dibitmap.obj \
$(OBJDIR)\blit.obj \
$(OBJDIR)\rgbcvt.obj \
$(OBJDIR)\objhandle.obj \
$(OBJDIR)\printer.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\gdi32rsrc.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = gdi32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

