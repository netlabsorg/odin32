# $Id: makefile,v 1.5 1999-08-16 16:28:03 sandervl Exp $

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

OBJS =  odincrt.obj odinfs.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib

$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS_ODINCRT) -Fm -Fe$@ $(OBJS) $(TARGET).def
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
        $(CP) $@ $(PDWIN32_LIB)


odincrt.obj: odincrt.cpp $(PDWIN32_INCLUDE)\odincrt.h

initterm.obj: initterm.cpp

odinfs.obj: odinfs.asm

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

