# $Id: makefile,v 1.31 2000-11-19 09:34:39 bird Exp $

#
# Odin32 API
#
#       gdi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


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
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = gdi32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

