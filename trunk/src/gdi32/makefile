# $Id: makefile,v 1.32 2000-12-02 23:39:32 bird Exp $

#
# Odin32 API
#
#       gdi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES    = $(CDEFINES) -DINVERT -DUSING_OPEN32


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\gdi32.obj \
$(OBJDIR)\driver.obj \
$(OBJDIR)\opengl.obj \
$(OBJDIR)\callback.obj \
$(OBJDIR)\dibsect.obj \
$(OBJDIR)\initterm.obj \
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
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\gdi32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = gdi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

