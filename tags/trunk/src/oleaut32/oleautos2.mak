# $Id: oleautos2.mak,v 1.1 2003-10-06 10:09:02 sandervl Exp $

#
# Odin32 API
#
#       ole32aut.dll makefile
#


!if "$(DEBUG)" == "1"
DEFFILE    = oleaut32dbg.def
ORGDEFFILE = oleaut32.def
!endif

#
# Compiler, tools, and interference rules.
#
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
!if "$(DEBUG)" == "1"
$(OBJDIR)\dbgwrap.obj \
!endif
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
# Target and original target names - names of the dll without extention and path
#
TARGET      = olautos2
ORGTARGET   = oleaut32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

