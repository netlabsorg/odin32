# $Id: makefile,v 1.1 2000-02-09 08:47:34 jeroen Exp $
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

CINCLUDE=-I.. -I..\..\mesa
CDEFS=-DNDEBUG -Tdp

CFLAGS = $(CFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) $(CDEFS) -I..\include -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -$(CDEFS) -I..\include -DNDEBUG

OBJS= dict.obj memalloc.obj mesh.obj normal.obj priorityq.obj \
      render.obj sweep.obj tess.obj tessmono.obj geom.obj

all: $(OBJS)

#
# Dependencies
#
dict.obj: dict.c dict-list.h memalloc.h
memalloc.obj: memalloc.c memalloc.h
mesh.obj: mesh.c mesh.h memalloc.h
normal.obj: normal.c mesh.h tess.h normal.h
priorityq.obj: priorityq.c memalloc.c priorityq-heap.h priorityq-sort.h
render.obj: render.c mesh.h tess.h render.h
sweep.obj: sweep.c mesh.h geom.h tess.h dict.h priorityq.h memalloc.h sweep.h
tess.obj: tess.c memalloc.c tess.h mesh.h normal.h sweep.h tessmono.h render.h
tessmono.obj: tessmono.c geom.h mesh.h tessmono.h
geom.obj: geom.c mesh.h geom.h

clean:
    $(RM) *.obj
