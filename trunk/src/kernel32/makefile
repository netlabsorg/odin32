# $Id: makefile,v 1.77 2000-01-02 22:09:01 sandervl Exp $

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


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) /Tm+
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) /Tm+

!ifdef NODEBUGINFO
CFLAGS = $(CFLAGS) -DDEFAULT_LOGGING_OFF
CXXFLAGS = $(CXXFLAGS) -DDEFAULT_LOGGING_OFF
!endif

!ifdef DEBUGALL
CFLAGS = $(CFLAGS) -DDEBUG_ENABLELOG_LEVEL2
CXXFLAGS = $(CXXFLAGS) -DDEBUG_ENABLELOG_LEVEL2
!endif

RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win

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
debug.obj oslibdebug.obj 


TARGET = kernel32

all: $(PDWIN32_LIB)\pmwinx.lib $(TARGET).dll $(TARGET).LIB


$(TARGET).dll: $(OBJS) $(TARGET).def $(TARGET).lrf
#        $(LD) $(LDFLAGS) /Fm /Fe$@ /B"/FREE" @$(TARGET).lrf
!ifdef DEBUG
!ifdef NODEBUGINFO
	ilink /nodeb /map /noe /NOD /NoLogo /NOFREE @$(TARGET).lrf
!else
	ilink /de /map  /noe /NOD /dbgpack /NoLogo /NOFREE @$(TARGET).lrf
!endif
!else
	ilink /map  /noe /NOD /NoLogo /FREE @$(TARGET).lrf
!endif
        rc -r console.rc console.res
        rc console.res $@
        $(CP) $@ $(PDWIN32_BIN)

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

$(TARGET).LIB: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

$(PDWIN32_LIB)\pmwinx.lib:
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(PDWIN32_LIB)\pmwinx.def

kernelrsrc.asm: kernel32.rc
    $(RC) $(RCFLAGS) -o kernelrsrc.asm kernel32.rc

kernelrsrc.obj: kernelrsrc.asm

kernel32.OBJ: kernel32.cpp \
    exceptutil.h \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\heap.h \
    $(PDWIN32_INCLUDE)\versionos2.h \
    $(PDWIN32_INCLUDE)\wprocess.h

process.obj: process.cpp \
    $(PDWIN32_INCLUDE)\misc.h \
    oslibdos.h \
    $(PDWIN32_INCLUDE)\winimagebase.h \
    $(PDWIN32_INCLUDE)\wprocess.h

kobjects.obj: kobjects.cpp

comm.OBJ: \
    .\comm.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

profile.obj: profile.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

directory.obj: directory.cpp \
    $(PDWIN32_INCLUDE)\unicode.h \
    initterm.h directory.h oslibdos.h profile.h

disk.obj: disk.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

atom.obj: atom.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

time.obj: time.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

wintls.obj: wintls.cpp $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winexebase.h $(PDWIN32_INCLUDE)\windllbase.h

fileio.obj: fileio.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

thread.OBJ: \
    .\thread.cpp \
    $(PDWIN32_INCLUDE)\wprocess.h \
    $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winexebase.h $(PDWIN32_INCLUDE)\windllbase.h \
    thread.h

heap.OBJ: \
    .\heap.cpp \
    $(PDWIN32_INCLUDE)\heap.h \
    os2heap.h

heapstring.obj: \
    .\heapstring.cpp \
    $(PDWIN32_INCLUDE)\heap.h \
    $(PDWIN32_INCLUDE)\heapstring.h

os2heap.OBJ: \
    .\os2heap.cpp \
    $(PDWIN32_INCLUDE)\vmutex.h \
    $(PDWIN32_INCLUDE)\win32type.h \
    os2heap.h \
    initterm.h

initterm.OBJ: .\initterm.cpp initterm.h  $(PDWIN32_INCLUDE)\heapshared.h  mmap.h directory.h hmdevio.h initsystem.h $(PDWIN32_INCLUDE)\heapcode.h
initsystem.obj: initsystem.cpp cpuhlp.h initsystem.h $(PDWIN32_INCLUDE)\misc.h directory.h $(PDWIN32_INCLUDE)\versionos2.h


thunk.OBJ: \
    .\thunk.cpp \
    thunk.h

vmutex.OBJ: vmutex.cpp \
    $(PDWIN32_INCLUDE)\vmutex.h

exceptions.OBJ: \
    .\exceptions.cpp \
    oslibexcept.h \
    mmap.h \
    $(PDWIN32_INCLUDE)\exceptions.h \
    exceptutil.h

message.OBJ: \
    .\message.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

oslibexcept.obj: \
    oslibexcept.cpp \
    $(PDWIN32_INCLUDE)\exceptions.h \
    oslibexcept.h

hmdevio.OBJ: \
    .\hmdevice.h \
    .\hmdevio.cpp \
    exceptutil.h \
    hmdevio.h \
    map.h \
    cio.h

LANG.OBJ: \
    .\lang.cpp

wprocess.OBJ: \
    .\wprocess.cpp \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\winexebase.h \
    $(PDWIN32_INCLUDE)\windllbase.h \
    $(PDWIN32_INCLUDE)\winexepeldr.h \
    $(PDWIN32_INCLUDE)\windllpeldr.h \
    $(PDWIN32_INCLUDE)\winimagebase.h \
    $(PDWIN32_INCLUDE)\winfakepeldr.h \
    $(PDWIN32_INCLUDE)\winimagepeldr.h \
    $(PDWIN32_INCLUDE)\versionos2.h \
    $(PDWIN32_INCLUDE)\wprocess.h \
    $(PDWIN32_INCLUDE)\win\thread.h \
    oslibdebug.h \
    console2.h \
    mmap.h \
    cio.h \
    oslibmisc.h

map.OBJ: \
    .\map.cpp \
    map.h \
    mapos2.h

oslibdos.OBJ: \
    .\oslibdos.cpp oslibdos.h initterm.h

network.OBJ: \
    .\network.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

misc.OBJ: \
    .\misc.cpp \
    initterm.h \
    $(PDWIN32_INCLUDE)\misc.h \
    $(PDWIN32_INCLUDE)\win\thread.h

win32util.OBJ: \
    .\win32util.cpp \
    $(PDWIN32_INCLUDE)\win32util.h

npipe.OBJ: \
    .\npipe.cpp \
    $(PDWIN32_INCLUDE)\misc.h

unicode.OBJ: \
    .\unicode.cpp \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\misc.h

console.OBJ: \
    .\console.cpp \
    .\console2.h \
    $(PDWIN32_INCLUDE)\misc.h

conin.OBJ: \
    .\conin.h \
    .\hmdevice.h \
    .\conin.cpp

conout.OBJ: \
    .\conout.h \
    .\hmdevice.h \
    .\conout.cpp

conbuffer.OBJ: \
    .\conbuffer.h \
    .\hmdevice.h \
    .\conbuffer.cpp

lfile.OBJ: \
    .\lfile.cpp \
    $(PDWIN32_INCLUDE)\wprocess.h

obsolete.OBJ: \
    .\obsolete.cpp

resource.OBJ: \
    .\resource.cpp \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\winimagebase.h \
    $(PDWIN32_INCLUDE)\winres.h \
    $(PDWIN32_INCLUDE)\misc.h

exceptutil.OBJ: \
    .\exceptutil.asm

iccio.OBJ: \
    .\iccio.asm

winres.OBJ: \
    .\winres.cpp \
    cvtresource.h \
    $(PDWIN32_INCLUDE)\winres.h \
    $(PDWIN32_INCLUDE)\winimagebase.h \
    $(PDWIN32_INCLUDE)\winexebase.h \
    $(PDWIN32_INCLUDE)\windllbase.h \
    $(PDWIN32_INCLUDE)\misc.h

oslibmisc.OBJ: .\oslibmisc.cpp oslibmisc.h

handlemanager.OBJ: \
        .\handlemanager.cpp \
        .\hmevent.h \
        .\hmmutex.h \
        .\hmopen32.h \
        .\hmsemaphore.h \
        .\hmmmap.h \
	.\hmtoken.h \
        $(PDWIN32_INCLUDE)\winconst.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmtoken.obj: hmtoken.cpp \
	.\hmtoken.h \
        .\hmdevice.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmcomm.obj: \
        .\hmcomm.cpp \
        .\hmdevice.h \
        .\hmcomm.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmdevice.OBJ: \
        .\hmdevice.cpp \
        .\hmdevice.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmopen32.OBJ: \
        .\hmopen32.cpp \
        .\hmdevice.h \
        .\hmopen32.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmobjects.obj: \
        .\hmobjects.cpp \
        .\hmdevice.h \
        .\hmobjects.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmevent.obj: \
        .\hmevent.cpp \
        .\hmevent.h \
        .\hmdevice.h \
        .\hmopen32.h

hmmutex.obj: \
        .\hmmutex.cpp \
        .\hmmutex.h \
        .\hmdevice.h \
        .\hmopen32.h

hmsemaphore.obj: \
        .\hmsemaphore.cpp \
        .\hmsemaphore.h \
        .\hmdevice.h \
        .\hmopen32.h

hmmmap.obj: hmmmap.cpp hmdevice.h hmopen32.h mmap.h $(PDWIN32_INCLUDE)\heapshared.h

conprop.OBJ: \
        .\conprop.h \
        .\conprop.cpp

conprop2.OBJ: \
        .\conprop.h \
        .\conprop2.cpp

async.OBJ: \
        .\async.cpp \
        $(PDWIN32_INCLUDE)\win32type.h \
        $(PDWIN32_INCLUDE)\misc.h \
        $(PDWIN32_INCLUDE)\unicode.h

virtual.obj:   virtual.cpp $(PDWIN32_INCLUDE)\win\virtual.h $(PDWIN32_INCLUDE)\handlemanager.h mmap.h
mmap.obj:      mmap.cpp mmap.h $(PDWIN32_INCLUDE)\vmutex.h oslibdos.h $(PDWIN32_INCLUDE)\heapshared.h $(PDWIN32_INCLUDE)\winimagepeldr.h

pefile.OBJ:    pefile.cpp $(PDWIN32_INCLUDE)\pefile.h $(PDWIN32_INCLUDE)\winres.h

winimagebase.obj:  winimagebase.cpp $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\windllbase.h
winimagepe2lx.obj: winimagepe2lx.cpp $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winimagepe2lx.h $(PDWIN32_INCLUDE)\windllbase.h
winimagepeldr.obj: winimagepeldr.cpp $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winimagepeldr.h $(PDWIN32_INCLUDE)\windllbase.h
winimagelx.obj:    winimagelx.cpp $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winimagelx.h
windllbase.obj:    windllbase.cpp $(PDWIN32_INCLUDE)\windllbase.h $(PDWIN32_INCLUDE)\winimagebase.h profile.h
windllpe2lx.obj:   windllpe2lx.cpp $(PDWIN32_INCLUDE)\windllbase.h $(PDWIN32_INCLUDE)\windllpe2lx.h \
                   $(PDWIN32_INCLUDE)\winimagepe2lx.h $(PDWIN32_INCLUDE)\winimagebase.h
windlllx.obj:      windlllx.cpp $(PDWIN32_INCLUDE)\windllbase.h $(PDWIN32_INCLUDE)\windlllx.h \
                   $(PDWIN32_INCLUDE)\winimagelx.h $(PDWIN32_INCLUDE)\odinlx.h \
                   $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winimagelx.h
windllpeldr.obj:   windllpeldr.cpp $(PDWIN32_INCLUDE)\windllbase.h $(PDWIN32_INCLUDE)\windllpeldr.h $(PDWIN32_INCLUDE)\winimagepeldr.h $(PDWIN32_INCLUDE)\winimagebase.h
winexebase.obj:    winexebase.cpp $(PDWIN32_INCLUDE)\winexebase.h $(PDWIN32_INCLUDE)\winimagebase.h
winexepe2lx.obj:   winexepe2lx.cpp $(PDWIN32_INCLUDE)\winexebase.h $(PDWIN32_INCLUDE)\winexepe2lx.h $(PDWIN32_INCLUDE)\winimagepe2lx.h $(PDWIN32_INCLUDE)\winimagebase.h
winexelx.obj:      winexelx.cpp $(PDWIN32_INCLUDE)\winexebase.h $(PDWIN32_INCLUDE)\winexelx.h \
                   $(PDWIN32_INCLUDE)\winimagelx.h $(PDWIN32_INCLUDE)\odinlx.h $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winimagelx.h
winexepeldr.obj:   winexepeldr.cpp $(PDWIN32_INCLUDE)\winexebase.h $(PDWIN32_INCLUDE)\winexepeldr.h $(PDWIN32_INCLUDE)\winimagepeldr.h $(PDWIN32_INCLUDE)\winimagebase.h
winfakepeldr.obj:  winfakepeldr.cpp $(PDWIN32_INCLUDE)\winimagepeldr.h $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\winfakepeldr.h

winimgres.OBJ: winimgres.cpp $(PDWIN32_INCLUDE)\winimagebase.h $(PDWIN32_INCLUDE)\pefile.h $(PDWIN32_INCLUDE)\winimagepe2lx.h $(PDWIN32_INCLUDE)\winimagepeldr.h $(PDWIN32_INCLUDE)\winimagelx.h $(PDWIN32_INCLUDE)\winres.h
stubs.obj:     stubs.cpp stubs.h

cvtbitmap.obj: cvtbitmap.cpp $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtaccel.obj: cvtaccel.cpp $(PDWIN32_INCLUDE)\winaccel.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvticon.obj: cvticon.cpp $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvticongrp.obj: cvticongrp.cpp $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtcursor.obj: cvtcursor.cpp $(PDWIN32_INCLUDE)\wincursor.h $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtcursorgrp.obj: cvtcursorgrp.cpp $(PDWIN32_INCLUDE)\wincursor.h $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h

interlock.obj: interlock.asm
toolhelp.obj: toolhelp.cpp

heapshared.obj: heapshared.cpp $(PDWIN32_INCLUDE)\heapshared.h
heapcode.obj: heapcode.cpp $(PDWIN32_INCLUDE)\heapshared.h $(PDWIN32_INCLUDE)\heapcode.h
cpu.obj: cpu.cpp cpuhlp.h
cpuhlp.obj: cpuhlp.asm
environ.obj: environ.cpp $(PDWIN32_INCLUDE)\misc.h
codepage.obj: codepage.cpp oslibdos.h profile.h $(PDWIN32_INCLUDE)\codepage.h

debug.obj:      debug.cpp oslibdebug.h
oslibdebug.obj: oslibdebug.cpp oslibdebug.h

clean:
        $(RM) *.OBJ *.LIB *.dll *.lrf *.res *.map *.pch kernelrsrc.asm
        $(RM) $(PDWIN32_LIB)\$(TARGET).LIB
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(TARGET)exp.def
