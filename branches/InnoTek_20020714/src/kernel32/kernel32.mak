# $Id: kernel32.mak,v 1.33 2002-07-13 15:58:20 sandervl Exp $

#
# Odin32 API
#
#       kernel32.dll makefile
#

#
# Alternate makefile name.
#
MAKEFILE=kernel32.mak

#
# Compiler, tools, and interference rules.
#
!if "$(DEBUG)" == "1"
DEFFILE    = kernel32dbg.def
ORGDEFFILE = kernel32.def
!endif

WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc

##CDEFINES = $(CDEFINES) /Fa+

!ifdef WITH_KLIB
CDEFINES = $(CDEFINES) -DWITH_KLIB
!endif

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kernel32.obj \
$(OBJDIR)\ccollection.obj \
$(OBJDIR)\perfview.obj \
$(OBJDIR)\kobjects.obj \
$(OBJDIR)\console.obj \
$(OBJDIR)\conin.obj \
$(OBJDIR)\conbuffer.obj \
$(OBJDIR)\conbuffervio.obj \
$(OBJDIR)\conout.obj \
$(OBJDIR)\network.obj \
$(OBJDIR)\hmdevio.obj \
$(OBJDIR)\profile.obj \
$(OBJDIR)\thread.obj \
$(OBJDIR)\virtual.obj \
$(OBJDIR)\oslibmem.obj \
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
$(OBJDIR)\char.obj \
$(OBJDIR)\oslibmisc.obj \
$(OBJDIR)\ole2nls.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\exceptutil.obj \
$(OBJDIR)\asmutil.obj \
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
$(OBJDIR)\initkernel32.obj \
$(OBJDIR)\handlemanager.obj \
$(OBJDIR)\handlenames.obj \
$(OBJDIR)\environ.obj \
$(OBJDIR)\initsystem.obj \
$(OBJDIR)\hmdevice.obj \
$(OBJDIR)\hmopen32.obj \
$(OBJDIR)\hmobjects.obj \
$(OBJDIR)\overlappedio.obj \
$(OBJDIR)\hmevent.obj \
$(OBJDIR)\hmfile.obj \
$(OBJDIR)\hmmutex.obj \
$(OBJDIR)\hmcomm.obj \
$(OBJDIR)\hmparport.obj \
$(OBJDIR)\hmnul.obj \
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
$(OBJDIR)\winexedummy.obj \
$(OBJDIR)\critsection.obj \
$(OBJDIR)\pefile.obj \
$(OBJDIR)\winimgres.obj \
$(OBJDIR)\wintls.obj \
$(OBJDIR)\async.obj \
$(OBJDIR)\fileio.obj \
$(OBJDIR)\hmtoken.obj \
$(OBJDIR)\atom.obj \
$(OBJDIR)\disk.obj \
$(OBJDIR)\osliblvm.obj \
$(OBJDIR)\directory.obj \
$(OBJDIR)\hmmmap.obj \
$(OBJDIR)\oslibexcept.obj \
$(OBJDIR)\cpu.obj \
$(OBJDIR)\process.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\dosdevice.obj \
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
$(OBJDIR)\oslibcdio.obj \
$(OBJDIR)\version.obj \
!ifdef DEBUG
$(OBJDIR)\exceptstackdump.obj \
$(OBJDIR)\dbgwrap.obj \
!endif
$(OBJDIR)\module.obj \
$(OBJDIR)\hmmailslot.obj \
$(OBJDIR)\mailslot.obj \
$(OBJDIR)\oslibtime.obj \
$(OBJDIR)\conodin32.obj \
$(OBJDIR)\kernelrsrc.obj


#
# Libraries. One space before the '\'.
# (Note! the order here is important!)
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)\wgss50.LIB \
$(ODIN32_LIB)\WIN32K.LIB \
$(ODIN32_LIB)\UNICODE.LIB \
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