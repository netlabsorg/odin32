# $Id: makefile,v 1.6 1999-08-17 12:46:04 sandervl Exp $

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

OBJS =  odincrt.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib

$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS_ODINCRT) -Fm -Fe$@ $(OBJS) $(TARGET).def
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).dll
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
        $(CP) $@ $(PDWIN32_LIB)

initterm.obj: initterm.cpp

odincrt.obj: odincrt.asm

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

