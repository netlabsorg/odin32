# $Id: ole32lib.mak,v 1.3 2001-07-30 22:53:03 bird Exp $

#
# Odin32 API
#
#       common.lib makefile
#
WRC_PREFIX_RESOURCE=1


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


CDEFINES = $(CDEFINES) -DWINE_LARGE_INTEGER

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
##$(OBJDIR)\guid.obj \
$(OBJDIR)\hglobalstream.obj \
$(OBJDIR)\initialise.obj \
$(OBJDIR)\itemmoniker.obj \
$(OBJDIR)\imessagefilter.obj \
$(OBJDIR)\memlockbytes.obj \
$(OBJDIR)\moniker.obj \
$(OBJDIR)\oString.obj \
$(OBJDIR)\ole2impl.obj \
$(OBJDIR)\ole2.obj \
$(OBJDIR)\oleobj.obj \
$(OBJDIR)\compobj.obj \
$(OBJDIR)\clipboard.obj \
$(OBJDIR)\ole2stubs.obj \
$(OBJDIR)\ole32_main.obj \
$(OBJDIR)\initole32.obj \
$(OBJDIR)\stg_bigblockfile.obj \
$(OBJDIR)\stg_stream.obj \
$(OBJDIR)\storage32.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\ifs.obj \
$(OBJDIR)\ole32rsrc.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = ole32os2o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

