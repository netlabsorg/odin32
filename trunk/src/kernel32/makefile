# $Id: makefile,v 1.86 2000-03-04 19:17:27 bird Exp $

#
# PD-Win32 API
#
#       kernel32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

!ifdef NODEBUGINFO
CFLAGS = $(CFLAGS) -DDEFAULT_LOGGING_OFF
CXXFLAGS = $(CXXFLAGS) -DDEFAULT_LOGGING_OFF
!endif

OBJS = KERNEL32.OBJ KOBJECTS.OBJ CONSOLE.OBJ CONIN.OBJ \
CONBUFFER.OBJ CONOUT.OBJ UNICODE.OBJ network.OBJ HMDEVIO.OBJ \
profile.obj THREAD.OBJ virtual.obj THUNK.OBJ OBSOLETE.OBJ COMM.OBJ\
MESSAGE.OBJ RESOURCE.OBJ EXCEPTIONS.OBJ heapshared.obj cpuhlp.obj heapcode.obj \
LFILE.OBJ NPIPE.OBJ oslibdos.obj oslibmisc.obj MISC.OBJ EXCEPTUTIL.OBJ \
LANG.OBJ ICCIO.OBJ MAP.OBJ WIN32UTIL.OBJ heap.OBJ heapstring.obj \
os2heap.OBJ vmutex.OBJ initterm.OBJ handlemanager.OBJ environ.obj initsystem.obj \
hmdevice.obj hmopen32.obj hmobjects.obj hmevent.obj hmmutex.obj hmcomm.obj \
hmsemaphore.obj wprocess.OBJ conprop.OBJ conprop2.obj winimagelx.obj \
winimagebase.OBJ windllbase.OBJ winexebase.OBJ time.obj mmap.obj \
winimagepe2lx.obj winimagepeldr.obj windllpe2lx.obj windlllx.obj windllpeldr.obj \
winexepe2lx.obj winexelx.obj winexepeldr.obj WINRES.OBJ critsection.obj \
pefile.OBJ winimgres.OBJ wintls.obj async.OBJ fileio.obj hmtoken.obj kernelrsrc.obj \
atom.obj disk.obj directory.obj cvtbitmap.obj hmmmap.obj winfakepeldr.obj \
cvtaccel.obj cvticon.obj cvticongrp.obj oslibexcept.obj cpu.obj process.obj \
cvtcursor.obj cvtcursorgrp.obj stubs.obj interlock.obj toolhelp.obj codepage.obj \
debug.obj oslibdebug.obj dbglocal.obj registry.obj


TARGET = kernel32

all: $(PDWIN32_LIB)\pmwinx.lib $(TARGET).dll $(TARGET).LIB


$(TARGET).dll: $(OBJS) $(TARGET).def $(TARGET).lrf
    $(LD2) $(LD2FLAGS) /NOFREE @$(TARGET).lrf
    rc -r console.rc console.res
    rc console.res $@
    $(CP) $@ $(PDWIN32_BIN)

# old linker stuff
#   $(LD) $(LDFLAGS) /Fm /Fe$@ /B"/FREE" @$(TARGET).lrf
#!ifdef DEBUG
#!ifdef NODEBUGINFO
#    ilink /nodeb /map /noe /NOD /NoLogo /NOFREE @$(TARGET).lrf
#!else
#    ilink /de /map  /noe /NOD /dbgpack /NoLogo /NOFREE @$(TARGET).lrf
#!endif
#!else
#    ilink /map  /noe /NOD /NoLogo /NOFREE @$(TARGET).lrf
#!endif



$(TARGET).lrf: makefile
        @echo Creating file <<$(@B).lrf
$(OBJS)
$(PDWIN32_LIB)\odincrt.lib
$(PDWIN32_LIB)\PMWINX.LIB
$(PDWIN32_LIB)\LIBULS.LIB
$(PDWIN32_LIB)\LIBCONV.LIB
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).LIB: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


$(PDWIN32_LIB)\pmwinx.lib:
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(PDWIN32_LIB)\pmwinx.def

kernelrsrc.asm: kernel32.rc
    $(RC) $(RCFLAGS) -o kernelrsrc.asm kernel32.rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.OBJ *.LIB *.dll *.lrf *.res *.map *.pch kernelrsrc.asm \
        $(PDWIN32_LIB)\$(TARGET).LIB $(PDWIN32_BIN)\$(TARGET).dll \
        $(TARGET)exp.def \
