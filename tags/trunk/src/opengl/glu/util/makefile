# $Id: makefile,v 1.1 2000-02-09 08:47:12 jeroen Exp $
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

CINCLUDE=-I.. -I..\..\mesa

CFLAGS = $(CFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -I..\include -Tdp -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -I..\include -Tdp -DNDEBUG

OBJS= error.obj glue.obj mipmap.obj project.obj quad.obj registry.obj

all: $(OBJS)

#
# Dependencies
#
error.obj: error.c
glue.obj: glue.c gluint.h
mipmap.obj: mipmap.c gluint.h
project.obj: project.c gluint.h
quad.obj: quad.c gluint.h
registry.obj: registry.c

clean:
    $(RM) *.obj *.lib *.map
