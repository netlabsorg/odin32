# $Id: makefile,v 1.4 1999-08-10 11:29:43 phaller Exp $

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
        $(PDWIN32_LIB)\LIBULS.LIB $(PDWIN32_LIB)\LIBCONV.LIB \
        OS2386.LIB SOMTK.LIB $(TARGET).def
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

