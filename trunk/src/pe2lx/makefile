# $Id: makefile,v 1.10 2000-03-06 23:38:55 bird Exp $

#
# Odin32 API
#
#       pe2lx.exe makefile
#
# WARNING: This is the old PE2LX. The new Pe2Lx is found in src\win32k\pe2lx.
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) /Ge+ /Tm+ /I$(PDWIN32_INCLUDE) /D__PE2LX__
CXXFLAGS = $(CXXFLAGS) -Ge+ /Tm+ /I$(PDWIN32_INCLUDE) /D__PE2LX__
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
        $(PDWIN32_LIB)\LIBCONV.LIB $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $(TARGET).exe $(PDWIN32_BIN)


accelerator.obj: accelerator.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h \
                 accelerator.h  icon.h misc.h
bitmap.obj: bitmap.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h \
            bitmap.h icon.h misc.h
cursor.obj: cursor.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h \
            cursor.h icon.h misc.h
cursorgrp.obj: cursorgrp.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h  \
               cursor.h cursorgrp.h misc.h
dialog.obj: dialog.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h \
            dialog.h misc.h
icon.obj: icon.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h icon.h misc.h
icongrp.obj: icongrp.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h icon.h \
             icongrp.h misc.h
lx.obj: lx.cpp lx.h
menu.obj: menu.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h menu.h
misc.obj: misc.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h ..\..\include\versionos2.h
pe.obj: pe.cpp $(PDWIN32_INCLUDE)\pefile.h lx.h \
        accelerator.h bitmap.h cursor.h \
        cursorgrp.h dialog.h icon.h \
        icongrp.h menu.h misc.h rcdata.h \
        strings.h
pefile.obj: pefile.cpp $(PDWIN32_INCLUDE)\pefile.h misc.h
rcdata.obj: rcdata.cpp lx.h rcdata.h
strings.obj: strings.cpp lx.h misc.h strings.h


clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch *.exe
        $(RM) $(PDWIN32_BIN)\$(TARGET).exe
