# $Id: makefile,v 1.47 2004-12-30 18:44:37 sao2l02 Exp $

#
# Odin32 API
#
#       gdi32.dll makefile
#

#
# Target name - name of the dll without extention and path.
#
TARGET = gdi32

#
# Compiler, tools, and interference rules.
#
ODIN32_DBGWRAP = 1
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides.
#
!ifndef WMAKE
CDEFINES    = $(CDEFINES) -DINVERT
!else
CDEFINES   += -DINVERT
!endif


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
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initgdi32.obj \
$(OBJDIR)\oslibgdi.obj \
$(OBJDIR)\font.obj \
$(OBJDIR)\fontres.obj \
$(OBJDIR)\text.obj \
$(OBJDIR)\devcontext.obj \
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
$(OBJDIR)\gdistats.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\ft2supp.obj \
$(OBJDIR)\gdi32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/wgss50.lib \
OS2386.LIB \
$(RTLLIB_O)

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

