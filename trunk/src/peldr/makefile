# $Id: makefile,v 1.13 2000-02-09 23:40:52 bird Exp $

#
# PD-Win32 API
#
#       pe.exe makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

CFLAGS   = $(CFLAGS) /Ge+ -I$(PDWIN32_INCLUDE) -Gn-
CXXFLAGS = $(CXXFLAGS) /Ge+ -I$(PDWIN32_INCLUDE) -Gn-
LDFLAGS  = $(LDFLAGS_ODINCRT) /Ge+ /B"/pmtype:pm /stack:0x100000 /NOBASE /Map" \
           os2386.lib


TARGET = pe

OBJS =  pe.obj

all: $(TARGET).exe


$(TARGET).exe: $(OBJS) makefile
    $(LD) $(LDFLAGS) -Fe$@ $(OBJS)
    $(CP) $(TARGET).exe $(PDWIN32_BIN)
    $(CP) $@ $(PDWIN32_BIN)


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch *.exe \
        $(PDWIN32_BIN)\$(TARGET).exe

