# $Id: makefile,v 1.14 2000-02-05 02:05:38 sandervl Exp $

#
# PD-Win32 API
#
#       odincrt.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CXXFLAGS = $(CXXFLAGS_ODINCRT) -I$(PDWIN32_INCLUDE)
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase

TARGET = odincrt

!ifdef VAC36
RUNTIMELIBDEF=odin36.def
!else
RUNTIMELIBDEF=odincrt.def
!endif

OBJS =  odincrt.obj initterm.obj malloc.obj string.obj file.obj

all: $(TARGET).dll $(TARGET).lib

$(TARGET).dll: $(OBJS) $(RUNTIMELIBDEF)
        $(LD) $(LDFLAGS_ODINCRT) -Fm -Fe$@ $(OBJS) $(RUNTIMELIBDEF) $(SOMLIB)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $**
        $(CP) $@ $(PDWIN32_LIB)

initterm.obj: initterm.cpp $(PDWIN32_INCLUDE)\exitlist.h

string.obj: string.cpp

odincrt.obj: odincrt.asm

file.obj: file.cpp

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

