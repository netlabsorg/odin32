# $Id: makefile,v 1.15 2000-02-09 23:40:47 bird Exp $

#
# PD-Win32 API
#
#       odincrt.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin

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


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET).def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib

