# $Id: makefile,v 1.11 2000-12-02 23:58:01 bird Exp $

#
# Odin32 API
#
#       pe2lx.exe makefile
#
# WARNING: This is the old PE2LX. The new Pe2Lx is found in src\win32k\pe2lx.
#

OLD_STYLE=1
!include ../../makefile.inc


CFLAGS = $(CFLAGS) /Ge+ /Tm+ /I$(ODIN32_INCLUDE) /D__PE2LX__
CXXFLAGS = $(CXXFLAGS) -Ge+ /Tm+ /I$(ODIN32_INCLUDE) /D__PE2LX__
LDFLAGS = $(LDFLAGS) -Ge+

TARGET = pe2lx

OBJS =  pefile.obj pe.obj lx.obj icon.obj \
        menu.obj dialog.obj misc.obj \
        rcdata.obj icongrp.obj \
        strings.obj bitmap.obj accelerator.obj \
        cursor.obj cursorgrp.obj

all:  $(TARGET).exe


$(TARGET).exe: $(OBJS)
        $(LD) $(LDFLAGS) -Fe$@ $(OBJS) \
        $(ODIN32_LIB)\LIBCONV.LIB $(ODIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $(TARGET).exe $(ODIN32_BIN)


accelerator.obj: accelerator.cpp $(ODIN32_INCLUDE)\pefile.h lx.h \
                 accelerator.h  icon.h misc.h
bitmap.obj: bitmap.cpp $(ODIN32_INCLUDE)\pefile.h lx.h \
            bitmap.h icon.h misc.h
cursor.obj: cursor.cpp $(ODIN32_INCLUDE)\pefile.h lx.h \
            cursor.h icon.h misc.h
cursorgrp.obj: cursorgrp.cpp $(ODIN32_INCLUDE)\pefile.h lx.h  \
               cursor.h cursorgrp.h misc.h
dialog.obj: dialog.cpp $(ODIN32_INCLUDE)\pefile.h lx.h \
            dialog.h misc.h
icon.obj: icon.cpp $(ODIN32_INCLUDE)\pefile.h lx.h icon.h misc.h
icongrp.obj: icongrp.cpp $(ODIN32_INCLUDE)\pefile.h lx.h icon.h \
             icongrp.h misc.h
lx.obj: lx.cpp lx.h
menu.obj: menu.cpp $(ODIN32_INCLUDE)\pefile.h lx.h menu.h
misc.obj: misc.cpp $(ODIN32_INCLUDE)\pefile.h lx.h ..\..\include\versionos2.h
pe.obj: pe.cpp $(ODIN32_INCLUDE)\pefile.h lx.h \
        accelerator.h bitmap.h cursor.h \
        cursorgrp.h dialog.h icon.h \
        icongrp.h menu.h misc.h rcdata.h \
        strings.h
pefile.obj: pefile.cpp $(ODIN32_INCLUDE)\pefile.h misc.h
rcdata.obj: rcdata.cpp lx.h rcdata.h
strings.obj: strings.cpp lx.h misc.h strings.h


clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch *.exe
        $(RM) $(ODIN32_BIN)\$(TARGET).exe
