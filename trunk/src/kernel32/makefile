# $Id: makefile,v 1.110 2001-02-17 21:48:23 bird Exp $

#
# Odin32 API
#
#       kernel32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kernel32.obj \
$(OBJDIR)\kobjects.obj \
$(OBJDIR)\console.obj \
$(OBJDIR)\conin.obj \
$(OBJDIR)\conbuffer.obj \
$(OBJDIR)\conbuffervio.obj \
$(OBJDIR)\conout.obj \
$(OBJDIR)\unicode.obj \
$(OBJDIR)\network.obj \
$(OBJDIR)\hmdevio.obj \
$(OBJDIR)\profile.obj \
$(OBJDIR)\thread.obj \
$(OBJDIR)\virtual.obj \
$(OBJDIR)\thunk.obj \
$(OBJDIR)\obsolete.obj \
$(OBJDIR)\comm.obj \
$(OBJDIR)\message.obj \
$(OBJDIR)\resource.obj \
$(OBJDIR)\exceptions.obj \
$(OBJDIR)\heapshared.obj \
$(OBJDIR)\cpuhlp.obj \
$(OBJDIR)\heapcode.obj \
$(OBJDIR)\lfile.obj \
$(OBJDIR)\npipe.obj \
$(OBJDIR)\oslibdos.obj \
$(OBJDIR)\oslibmisc.obj \
$(OBJDIR)\ole2nls.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\exceptutil.obj \
$(OBJDIR)\lang.obj \
$(OBJDIR)\iccio.obj \
$(OBJDIR)\map.obj \
$(OBJDIR)\win32util.obj \
$(OBJDIR)\heap.obj \
$(OBJDIR)\heapstring.obj \
$(OBJDIR)\os2heap.obj \
$(OBJDIR)\vmutex.obj \
$(OBJDIR)\vsemaphore.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\handlemanager.obj \
$(OBJDIR)\environ.obj \
$(OBJDIR)\initsystem.obj \
$(OBJDIR)\hmdevice.obj \
$(OBJDIR)\hmopen32.obj \
$(OBJDIR)\hmobjects.obj \
$(OBJDIR)\hmevent.obj \
$(OBJDIR)\hmfile.obj \
$(OBJDIR)\hmmutex.obj \
$(OBJDIR)\hmcomm.obj \
$(OBJDIR)\hmsemaphore.obj \
$(OBJDIR)\hmstd.obj \
$(OBJDIR)\wprocess.obj \
$(OBJDIR)\conprop.obj \
$(OBJDIR)\conprop2.obj \
$(OBJDIR)\winimagelx.obj \
$(OBJDIR)\winimagebase.obj \
$(OBJDIR)\windllbase.obj \
$(OBJDIR)\winexebase.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\mmap.obj \
$(OBJDIR)\winimagepe2lx.obj \
$(OBJDIR)\winimagepeldr.obj \
$(OBJDIR)\windllpe2lx.obj \
$(OBJDIR)\windlllx.obj \
$(OBJDIR)\windllpeldr.obj \
$(OBJDIR)\winexepe2lx.obj \
$(OBJDIR)\winexelx.obj \
$(OBJDIR)\winexepeldr.obj \
$(OBJDIR)\winres.obj \
$(OBJDIR)\critsection.obj \
$(OBJDIR)\pefile.obj \
$(OBJDIR)\winimgres.obj \
$(OBJDIR)\wintls.obj \
$(OBJDIR)\async.obj \
$(OBJDIR)\fileio.obj \
$(OBJDIR)\hmtoken.obj \
$(OBJDIR)\atom.obj \
$(OBJDIR)\disk.obj \
$(OBJDIR)\directory.obj \
$(OBJDIR)\hmmmap.obj \
$(OBJDIR)\oslibexcept.obj \
$(OBJDIR)\cpu.obj \
$(OBJDIR)\process.obj \
$(OBJDIR)\cvtcursor.obj \
$(OBJDIR)\cvtcursorgrp.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\ordinals.obj \
$(OBJDIR)\interlock.obj \
$(OBJDIR)\toolhelp.obj \
$(OBJDIR)\codepage.obj \
$(OBJDIR)\debug.obj \
$(OBJDIR)\oslibdebug.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\registry.obj \
$(OBJDIR)\queue.obj \
$(OBJDIR)\hmthread.obj \
$(OBJDIR)\hmnpipe.obj \
$(OBJDIR)\hmdisk.obj \
$(OBJDIR)\version.obj \
!ifdef DEBUG
$(OBJDIR)\exceptstackdump.obj \
!endif
$(OBJDIR)\kernelrsrc.obj


#
# Libraries. One space before the '\'.
# (Note! the order here is important!)
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)\PMWINX.LIB \
$(ODIN32_LIB)\LIBULS.LIB \
$(ODIN32_LIB)\LIBCONV.LIB \
$(ODIN32_LIB)\WIN32K.LIB \
OS2386.LIB \
$(RTLLIB_O)


#
# OS/2 resourcefiles
#
OS2RES = \
$(OBJDIR)\console.res


#
# Target name - name of the dll without extention and path.
#
TARGET = kernel32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Override flags for exceptions.cpp
#  Hmm. This is compiler depended!
#
$(OBJDIR)\exceptions.obj: exceptions.cpp
!if "$(VAC3)" == "1"  ||  "$(VAC36)" == "1"
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Op- \
        -Fo$(OBJDIR)\$(@B).obj -c exceptions.cpp
!else
# Since this only turns off stack optimizations I take it's a VAC only fix.
# -kso (2001-02-17)
# !   error "Compiler is not supported yet."
!endif

