# $Id: oleautos2.mak,v 1.2 2004-12-30 18:45:03 sao2l02 Exp $

#
# Odin32 API
#
#       ole32aut.dll makefile
#


#
# Target and original target names - names of the dll without extention and path
#
TARGET      = olautos2
ORGTARGET   = oleaut32

#
# Compiler, tools, and interference rules.
#
ODIN32_DBGWRAP = 1
!include ../../makefile.inc

CDEFINES = $(CDEFINES) -DHAVE_LIBJPEG -DHAVE_JPEGLIB_H -DHAVE_GIF_LIB_H -I.\include

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\oleaut.obj \
$(OBJDIR)\olefont.obj \
$(OBJDIR)\variant.obj \
$(OBJDIR)\ole2disp.obj \
$(OBJDIR)\parsedt.obj \
$(OBJDIR)\olepicture.obj \
$(OBJDIR)\asmutil.obj \
$(OBJDIR)\typelib.obj \
$(OBJDIR)\tmarshal.obj \
$(OBJDIR)\hash.obj \
$(OBJDIR)\safearray.obj \
$(OBJDIR)\connpt.obj \
$(OBJDIR)\dispatch.obj \
$(OBJDIR)\stubs.obj \
$(DLLENTRY) \
$(OBJDIR)\oleaut32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/guidlib.lib \
$(ODIN32_LIB)/giflib.lib \
$(ODIN32_LIB)/jpeglib.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

