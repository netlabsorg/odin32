# $Id: makefile,v 1.2 2000-02-09 23:40:44 bird Exp $

#
# PD-Win32 API
#
#       dllentry library makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = dllentry

all: $(PDWIN32_LIB)\$(TARGET).obj

$(PDWIN32_LIB)\$(TARGET).obj: dllentry.obj
    $(CP) $(TARGET).obj $(PDWIN32_LIB)

lib: $(PDWIN32_LIB)\$(TARGET).obj

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj $(PDWIN32_LIB)\$(TARGET).obj *.pch

