# $Id: makefile,v 1.3 2000-02-29 13:56:50 sandervl Exp $
#
# PD-Win32 API
#
#       glu/util makefile
#

PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CINCLUDE=-I.. -I..\..\mesa -I$(PDWIN32_INCLUDE) -I..\include -I..\..\mesa\GL

CFLAGS = $(CFLAGS) $(CINCLUDE) -Tdp -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)  -Tdp -DNDEBUG

OBJS= error.obj glue.obj mipmap.obj project.obj quad.obj registry.obj

all: $(OBJS)


dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc \
        ..\include\*.h ..\..\mesa\*.h ..\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.map
