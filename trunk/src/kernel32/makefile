# $Id: makefile,v 1.6 1999-06-17 21:52:01 phaller Exp $

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


OBJS = KERNEL32.OBJ \
       KOBJECTS.OBJ \
       CONSOLE.OBJ \
       CONIN.OBJ \
       CONBUFFER.OBJ \
       CONOUT.OBJ \
       UNICODE.OBJ \
       network.OBJ \
       DEVIO.OBJ \
       profile.obj \
       THREAD.OBJ \
       THUNK.OBJ \
       OBSOLETE.OBJ \
       COMM.OBJ \
       MESSAGE.OBJ \
       RESOURCE.OBJ \
       NAMEID.OBJ \
       WINRES.OBJ \
       WINMOD.OBJ \
       OS2NATIVE.OBJ \
       EXCEPTIONS.OBJ \
       LFILE.OBJ \
       MMAP.OBJ \
       NPIPE.OBJ \
       MISC.OBJ \
       EXCEPT.OBJ \
       LANG.OBJ \
       ICCIO.OBJ \
       MAP.OBJ \
       WIN32UTIL.OBJ \
       heap.OBJ \
       os2heap.OBJ \
       vmutex.OBJ \
       initterm.OBJ \
       os2util.OBJ \
       handlemanager.OBJ \
       hmdevice.obj \
       hmopen32.obj \
       wprocess.OBJ \
       conprop.OBJ \
       winimage.OBJ \
       windll.OBJ \
       winexe.OBJ \
       time.obj \
       pefile.OBJ \
       winimgres.OBJ \
       async.OBJ \
       fileio.obj \
       atom.obj \
       disk.obj \
       directory.obj \
       stubs.obj


TARGET = kernel32

all: $(PDWIN32_LIB)\pmwinx.lib $(TARGET).dll $(TARGET).LIB


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(PDWIN32_LIB)\PMWINX.LIB $(PDWIN32_LIB)\LIBULS.LIB $(PDWIN32_LIB)\LIBCONV.LIB
        rc -r console.rc console.res
        rc console.res $@
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).LIB: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(PDWIN32_LIB)\pmwinx.lib:
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(PDWIN32_LIB)\pmwinx.def

kernel32.OBJ: kernel32.cpp \
    except.h \
    $(PDWIN32_INCLUDE)\unicode.h \
    heap.h \
    wprocess.h \
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

fileio.obj: fileio.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

thread.OBJ: \
    .\thread.cpp \
    thread.h

heap.OBJ: \
    .\heap.cpp \
    os2heap.h

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
    except.h

message.OBJ: \
    .\message.cpp \
    $(PDWIN32_INCLUDE)\unicode.h

devio.OBJ: \
    .\devio.cpp \
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

mmap.OBJ: \
    .\mmap.cpp \
    mmap.h \
    $(PDWIN32_INCLUDE)\unicode.h

lfile.OBJ: \
    .\lfile.cpp \
    wprocess.h \
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
    winres.h \
    $(PDWIN32_INCLUDE)\misc.h

except.OBJ: \
    .\except.asm

iccio.OBJ: \
    .\iccio.asm

winres.OBJ: \
    .\winres.cpp \
    winres.h \
    $(PDWIN32_INCLUDE)\nameid.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\winexe.h \
    $(PDWIN32_INCLUDE)\windll.h \
    $(PDWIN32_INCLUDE)\misc.h

winmod.OBJ: \
    .\winres.cpp \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\winexe.h \
    $(PDWIN32_INCLUDE)\windll.h \
    $(PDWIN32_INCLUDE)\winimage.h \
    $(PDWIN32_INCLUDE)\misc.h

os2util.OBJ: .\os2util.cpp

handlemanager.OBJ: \
        .\handlemanager.cpp \
        .\handlemanager.h

hmdevice.OBJ: \
        .\hmdevice.cpp \
        .\hmdevice.h \
        .\handlemanager.h

hmopen32.OBJ: \
        .\hmopen32.cpp \
        .\hmopen32.h \
        .\handlemanager.h


conprop.OBJ: \
        .\conprop.h \
        .\conprop.cpp

async.OBJ: \
        .\async.cpp \
        $(PDWIN32_INCLUDE)\win32type.h \
        $(PDWIN32_INCLUDE)\misc.h \
        $(PDWIN32_INCLUDE)\unicode.h

pefile.OBJ:    pefile.cpp $(PDWIN32_INCLUDE)\pefile.h
winimage.OBJ:  winimage.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h $(PDWIN32_INCLUDE)\pefile.h
winimgres.OBJ: winimgres.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h $(PDWIN32_INCLUDE)\pefile.h $(PDWIN32_INCLUDE)\winexe.h winres.h
windll.OBJ:    windll.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h
winexe.OBJ:    winexe.cpp $(PDWIN32_INCLUDE)\winimage.h $(PDWIN32_INCLUDE)\windll.h $(PDWIN32_INCLUDE)\winexe.h
stubs.obj:     stubs.cpp stubs.h


clean:
        $(RM) *.OBJ *.LIB *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_LIB)\$(TARGET).LIB
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll


