# $Id: makefile.wat,v 1.1 1999-11-28 23:10:08 bird Exp $

#
# PD-Win32 API
#
#       kernel32testw.exe makefile
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin
PDWIN32_TOOLS = ..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

INCLUDES = $(WATCOM)\h;$(WATCOM)\h\nt;$(PDWIN32_INCLUDE)

RC      = $(WATCOM)\binw\wrc.exe
RCFLAGS = -bt=nt -I. -I$(INCLUDES)

CXX      = wcc386
CXXFLAGS = -w4 -I$(INCLUDES) -bt=nt -d2

CXX      = wpp386
CXXFLAGS = -w4 -I$(INCLUDES) -bt=nt -d2

LD       = wlink

OBJS =  winmain.objw \
        kernel32test.objw \
        resources.objw \
        testlib.objw

FILEOBJS = \
        file winmain.objw \
        file kernel32test.objw \
        file resources.objw \
        file testlib.objw


#
# Interference rules
#
.c.objw:
        $(CC) $(CFLAGS) -Fo=$@ $<

.cpp.objw:
        $(CXX) $(CXXFLAGS) -Fo=$@ $<



TARGET = kernel32testw


all: $(TARGET).exe


$(TARGET).exe: $(OBJS) rsrc.res
    $(LD) @<<
name $@
system nt
debug All
libpath $(WATCOM)\lib386
libpath $(WATCOM)\lib386\nt
$(FILEOBJS)
<<
    $(RC) rsrc.res $@

rsrc.res: rsrc.rc rsrc.h
    $(RC) -r $(RCFLAGS) $(*B).rc $@

clean:
    $(RM) *.OBJ $(TARGET).exe rsrc.res $(TARGET).map *.pch
