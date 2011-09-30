# $Id: makefile.wat,v 1.2 2000-12-03 00:34:40 bird Exp $

#
# Odin32 API
#
#       kernel32testw.exe makefile
#

!error "this don't work"
!include ../../../makefile.inc

INCLUDES = $(WATCOM)\h;$(WATCOM)\h\nt;$(ODIN32_INCLUDE)

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

