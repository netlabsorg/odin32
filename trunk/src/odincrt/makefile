# $Id: makefile,v 1.2 1999-08-09 18:10:50 achimha Exp $

#
# PD-Win32 API
#
#       odincrt.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) /B"/NOIgnoreCase"
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) /B"/NOIgnoreCase"
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase

TARGET = odincrt

OBJS =  odincrt.obj odinfs.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) CPPOM30.LIB CPPOM30O.LIB \
        OS2386.LIB SOMTK.LIB $(TARGET).def
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
        $(CP) $@ $(PDWIN32_LIB)


odincrt.obj: odincrt.cpp odincrt.h
initterm.obj: initterm.cpp
odinfs.obj: odinfs.asm

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

