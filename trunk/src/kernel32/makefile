# $Id: makefile,v 1.24 1999-08-22 11:11:10 sandervl Exp $

#
# PD-Win32 API
#
#       kernel32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) /Tm+
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) /Tm+


OBJS = KERNEL32.OBJ KOBJECTS.OBJ CONSOLE.OBJ CONIN.OBJ CONBUFFER.OBJ \
       CONOUT.OBJ UNICODE.OBJ network.OBJ DEVIO.OBJ profile.obj \
       THREAD.OBJ virtual.obj THUNK.OBJ OBSOLETE.OBJ COMM.OBJ \
       MESSAGE.OBJ RESOURCE.OBJ NAMEID.OBJ WINRES.OBJ WINMOD.OBJ \
       OS2NATIVE.OBJ EXCEPTIONS.OBJ LFILE.OBJ NPIPE.OBJ \
       MISC.OBJ EXCEPTUTIL.OBJ LANG.OBJ ICCIO.OBJ MAP.OBJ \
       WIN32UTIL.OBJ \
       heap.OBJ \
       heapstring.obj \
       os2heap.OBJ \
       vmutex.OBJ \
       initterm.OBJ \
       os2util.OBJ \
       handlemanager.OBJ \
       hmdevice.obj \
       hmopen32.obj \
       hmobjects.obj \
       hmevent.obj \
       hmmutex.obj \
       hmsemaphore.obj \
       wprocess.OBJ \
       conprop.OBJ \
       winimage.OBJ \
       windll.OBJ \
       winexe.OBJ \
       time.obj \
       pefile.OBJ \
       winimgres.OBJ \
       wintls.obj \
       async.OBJ \
       fileio.obj \
       atom.obj \
       disk.obj \
       directory.obj \
       cvtbitmap.obj \
       cvtmenu.obj \
       cvtaccel.obj \
       cvticon.obj \
       cvticongrp.obj \
       cvtcursor.obj \
       cvtcursorgrp.obj \
       stubs.obj


TARGET = kernel32

all: $(PDWIN32_LIB)\pmwinx.lib $(TARGET).dll $(TARGET).LIB


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(PDWIN32_LIB)\PMWINX.LIB $(PDWIN32_LIB)\LIBULS.LIB $(PDWIN32_LIB)\LIBCONV.LIB $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        rc -r console.rc console.res
        rc console.res $@
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).LIB: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(PDWIN32_LIB)\pmwinx.lib:
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(PDWIN32_LIB)\pmwinx.def

kernel32.OBJ: kernel32.cpp \
    exceptutil.h \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\heap.h \
    $(PDWIN32_INCLUDE)\wprocess.h \
    os2util.h \
    $(PDWIN32_INCLUDE)\nameid.h

kobjects.obj: kobjects.cpp

comm.OBJ: \
    .\comm.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

profile.obj: profile.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

directory.obj: directory.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

disk.obj: disk.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

atom.obj: atom.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

time.obj: time.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

wintls.obj: wintls.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\winexe.h $(PDWIN32_INCLUDE)\windll.h

fileio.obj: fileio.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

thread.OBJ: \
    .\thread.cpp \
    $(PDWIN32_INCLUDE)\wprocess.h \
    $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\winexe.h $(PDWIN32_INCLUDE)\windll.h \
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
    os2heap.h

initterm.OBJ: .\initterm.cpp

thunk.OBJ: \
    .\thunk.cpp \
    thunk.h

vmutex.OBJ: vmutex.cpp \
    $(PDWIN32_INCLUDE)\vmutex.h

exceptions.OBJ: \
    .\exceptions.cpp \
    $(PDWIN32_INCLUDE)\exceptions.h \
    exceptutil.h

message.OBJ: \
    .\message.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

devio.OBJ: \
    .\devio.cpp \
    exceptutil.h \
    devio.h \
    map.h \
    cio.h

LANG.OBJ: \
    .\lang.cpp

wprocess.OBJ: \
    .\wprocess.cpp \
    $(PDWIN32_INCLUDE)\nameid.h \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\winexe.h \
    $(PDWIN32_INCLUDE)\windll.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\versionos2.h \
    $(PDWIN32_INCLUDE)\wprocess.h \
    console2.h \
    cio.h \
    os2util.h

map.OBJ: \
    .\map.cpp \
    map.h \
    mapos2.h

os2native.OBJ: \
    .\os2native.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

network.OBJ: \
    .\network.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

misc.OBJ: \
    .\misc.cpp \
    $(PDWIN32_INCLUDE)\misc.h

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
    .\conin.cpp

conout.OBJ: \
    .\conout.h \
    .\conout.cpp

conbuffer.OBJ: \
    .\conbuffer.h \
    .\conbuffer.cpp

lfile.OBJ: \
    .\lfile.cpp \
    $(PDWIN32_INCLUDE)\wprocess.h \
    os2util.h

obsolete.OBJ: \
    .\obsolete.cpp

nameid.OBJ: \
    .\nameid.cpp \
    $(PDWIN32_INCLUDE)\nameid.h \
    $(PDWIN32_INCLUDE)\win32util.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\winexe.h \
    $(PDWIN32_INCLUDE)\windll.h \
    $(PDWIN32_INCLUDE)\misc.h

resource.OBJ: \
    .\resource.cpp \
    $(PDWIN32_INCLUDE)\nameid.h \
    $(PDWIN32_INCLUDE)\unicode.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\winres.h \
    $(PDWIN32_INCLUDE)\misc.h

exceptutil.OBJ: \
    .\exceptutil.asm

iccio.OBJ: \
    .\iccio.asm

winres.OBJ: \
    .\winres.cpp \
    $(PDWIN32_INCLUDE)\winres.h \
    $(PDWIN32_INCLUDE)\nameid.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\winexe.h \
    $(PDWIN32_INCLUDE)\windll.h \
    $(PDWIN32_INCLUDE)\misc.h

winmod.OBJ: \
    .\winmod.cpp \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\winexe.h \
    $(PDWIN32_INCLUDE)\windll.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\misc.h

os2util.OBJ: .\os2util.cpp

handlemanager.OBJ: \
        .\handlemanager.cpp \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmdevice.OBJ: \
        .\hmdevice.cpp \
        .\hmdevice.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmopen32.OBJ: \
        .\hmopen32.cpp \
        .\hmopen32.h \
        $(PDWIN32_INCLUDE)\handlemanager.h

hmobjects.obj: \
        .\hmobjects.cpp \
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

conprop.OBJ: \
        .\conprop.h \
        .\conprop.cpp

async.OBJ: \
        .\async.cpp \
        $(PDWIN32_INCLUDE)\win32type.h \
        $(PDWIN32_INCLUDE)\misc.h \
        $(PDWIN32_INCLUDE)\unicode.h

virtual.obj:   virtual.cpp $(PDWIN32_INCLUDE)\win\virtual.h

pefile.OBJ:    pefile.cpp $(PDWIN32_INCLUDE)\pefile.h
winimage.OBJ:  winimage.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h $(PDWIN32_INCLUDE)\pefile.h $(PDWIN32_INCLUDE)\winres.h
winimgres.OBJ: winimgres.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h $(PDWIN32_INCLUDE)\pefile.h $(PDWIN32_INCLUDE)\winexe.h $(PDWIN32_INCLUDE)\winres.h
windll.OBJ:    windll.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h
winexe.OBJ:    winexe.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h $(PDWIN32_INCLUDE)\winexe.h
stubs.obj:     stubs.cpp stubs.h

cvtbitmap.obj: cvtbitmap.cpp $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtmenu.obj: cvtmenu.cpp $(PDWIN32_INCLUDE)\winmenu.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtaccel.obj: cvtaccel.cpp $(PDWIN32_INCLUDE)\winaccel.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvticon.obj: cvticon.cpp $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvticongrp.obj: cvticongrp.cpp $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtcursor.obj: cvtcursor.cpp $(PDWIN32_INCLUDE)\wincursor.h $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h
cvtcursorgrp.obj: cvtcursorgrp.cpp $(PDWIN32_INCLUDE)\wincursor.h $(PDWIN32_INCLUDE)\winicon.h $(PDWIN32_INCLUDE)\misc.h cvtresource.h

clean:
        $(RM) *.OBJ *.LIB *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_LIB)\$(TARGET).LIB
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll


