# $Id: makefile,v 1.3 2000-02-29 13:56:50 sandervl Exp $
#
# PD-Win32 API
#
#       glu/tess makefile
#

PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CINCLUDE=-I.. -I..\..\mesa -I$(PDWIN32_INCLUDE) -I..\include -I..\..\mesa\GL
CDEFS=-DNDEBUG -Tdp

CFLAGS = $(CFLAGS) $(CINCLUDE) $(CDEFS) -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -$(CDEFS) -DNDEBUG

OBJS= dict.obj memalloc.obj mesh.obj normal.obj priorityq.obj \
      render.obj sweep.obj tess.obj tessmono.obj geom.obj

all: $(OBJS)


dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc  \
        ..\..\mesa\*.h ..\*.h ..\include\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj
