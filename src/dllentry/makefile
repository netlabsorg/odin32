# $Id: makefile,v 1.1 2000-02-05 01:49:48 sandervl Exp $

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

$(TARGET).obj: dllentry.cpp

clean:
        $(RM) *.obj
        $(RM) $(PDWIN32_LIB)\$(TARGET).obj





