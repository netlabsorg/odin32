# $Id: ole32.mak,v 1.4 2001-01-25 20:17:23 sandervl Exp $

#
# Odin32 API
#
#       ole32.dll makefile
#

#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = ole32.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ole32.obj \
$(OBJDIR)\antimoniker.obj \
$(OBJDIR)\bindctx.obj \
$(OBJDIR)\clsid.obj \
$(OBJDIR)\compositemoniker.obj \
$(OBJDIR)\datacache.obj \
$(OBJDIR)\defaulthandler.obj \
$(OBJDIR)\errorinfo.obj \
$(OBJDIR)\filemoniker.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\hglobalstream.obj \
$(OBJDIR)\initialise.obj \
$(OBJDIR)\itemmoniker.obj \
$(OBJDIR)\iunknown.obj \
$(OBJDIR)\imessagefilter.obj \
$(OBJDIR)\library.obj \
$(OBJDIR)\memlockbytes.obj \
$(OBJDIR)\moniker.obj \
$(OBJDIR)\oString.obj \
$(OBJDIR)\ole2impl.obj \
$(OBJDIR)\ole2.obj \
$(OBJDIR)\oleobj.obj \
$(OBJDIR)\oleClip.obj \
$(OBJDIR)\oleDrag.obj \
$(OBJDIR)\oleMenu.obj \
$(OBJDIR)\stg_bigblockfile.obj \
$(OBJDIR)\stg_stream.obj \
$(OBJDIR)\storage32.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\taskmem.obj \
$(OBJDIR)\ole32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/rpcrt4.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/oleaut32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target and original target names - names of the dll without extention and path
#
TARGET      = ole32os2
ORGTARGET   = ole32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

