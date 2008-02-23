# $Id: ole32.mak,v 1.23 2004-12-30 18:44:55 sao2l02 Exp $

#
# Odin32 API
#
#       ole32.dll makefile
#

#
# Target and original target names - names of the dll without extention and path
#
TARGET      = ole32os2
ORGTARGET   = ole32

#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = ole32.mak

#
# Compiler, tools, and interference rules.
#
ODIN32_DBGWRAP = 1
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES = $(CDEFINES) -DWINE_LARGE_INTEGER


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ole32.obj \
$(OBJDIR)\antimoniker.obj \
$(OBJDIR)\bindctx.obj \
$(OBJDIR)\compositemoniker.obj \
$(OBJDIR)\compobj.obj \
$(OBJDIR)\datacache.obj \
$(OBJDIR)\defaulthandler.obj \
$(OBJDIR)\errorinfo.obj \
$(OBJDIR)\filemoniker.obj \
$(OBJDIR)\ftmarshal.obj \
$(OBJDIR)\hglobalstream.obj \
$(OBJDIR)\initialise.obj \
$(OBJDIR)\itemmoniker.obj \
$(OBJDIR)\imessagefilter.obj \
$(OBJDIR)\memlockbytes.obj \
!if "$(DEBUG)" == "1"
$(OBJDIR)\oString.obj \
!endif
$(OBJDIR)\ole2impl.obj \
$(OBJDIR)\ole2.obj \
$(OBJDIR)\oleobj.obj \
$(OBJDIR)\clipboard.obj \
$(OBJDIR)\ole2stubs.obj \
$(OBJDIR)\ole32_main.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initole32.obj \
$(OBJDIR)\stg_bigblockfile.obj \
$(OBJDIR)\stg_stream.obj \
$(OBJDIR)\storage32.obj \
$(OBJDIR)\marshal.obj \
$(OBJDIR)\oleproxy.obj \
$(OBJDIR)\rpc.obj \
$(OBJDIR)\moniker.obj \
$(OBJDIR)\ifs.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\ole32rsrc.obj

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
$(ODIN32_LIB)/guidlib.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
