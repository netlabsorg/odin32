# $Id: makefile,v 1.27 2001-06-07 12:48:18 sandervl Exp $

#
# Odin32 API
#
#       ole32aut.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\oleaut32.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\oList.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\olefont.obj \
$(OBJDIR)\variant.obj \
$(OBJDIR)\ole2disp.obj \
$(OBJDIR)\parsedt.obj \
$(OBJDIR)\olepicture.obj \
$(OBJDIR)\asmutil.obj \
#$(OBJDIR)\iPicture.obj \
#$(OBJDIR)\iPictureNone.obj \
#$(OBJDIR)\iPictureBmp.obj \
#$(OBJDIR)\iPictureMeta.obj \
#$(OBJDIR)\iPictureIcon.obj \
#$(OBJDIR)\iPictureEmf.obj \
$(OBJDIR)\typelib.obj \
$(OBJDIR)\itypelib.obj \
$(OBJDIR)\itypecomp.obj \
$(OBJDIR)\itypeinfo.obj \
$(OBJDIR)\hash.obj \
$(OBJDIR)\safearray.obj \
$(OBJDIR)\connpt.obj \
$(OBJDIR)\dispatch.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\oleaut32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/pmwinx.lib \
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

