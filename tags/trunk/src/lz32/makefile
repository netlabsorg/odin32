# $Id: makefile,v 1.6 2000-01-03 10:25:57 sandervl Exp $

#
# PD-Win32 API
#
#       lz32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)
IMPLIBFLAGS = $(IMPLIBFLAGS)

TARGET = lz32

OBJS =  lz32.obj initterm.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) \
        $(PDWIN32_LIB)\odincrt.lib \
        $(PDWIN32_LIB)\kernel32.lib \
        os2386.lib \
        $(RTLLIB_O) \
        $(TARGET).def
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $**
        $(CP) $@ $(PDWIN32_LIB)


lz32.obj: lz32.cpp lz32.h $(PDWIN32_INCLUDE)\win\debugtools.h
initterm.obj: initterm.cpp

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

