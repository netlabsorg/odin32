# $Id: makefile,v 1.2 2000-12-03 00:34:40 bird Exp $

#
# Odin32 API
#
#       kernel32test.exe makefile
#


OLD_STYLE=1
!include ../../../makefile.inc

RC = $(ODIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(ODIN32_INCLUDE) -I$(ODIN32_INCLUDE)\win

CFLAGS = $(CFLAGS) -I$(ODIN32_INCLUDE) /Tm+ -Ge+
CXXFLAGS = $(CXXFLAGS) -I$(ODIN32_INCLUDE) /Tm+ -Ge+

OBJS =  os2main.obj \
        kernel32test.obj \
        resources.obj \
        testlib.obj \
        rsrc.obj

TARGET = kernel32test


all: $(TARGET).exe


$(TARGET).exe: $(OBJS)
    $(LD) $(LDFLAGS) -Ge+ -Fe$@ $(OBJS) \
        $(ODIN32_LIB)\odincrt.lib \
        $(ODIN32_LIB)\kernel32.lib \
        OS2386.LIB $(RTLLIB_O)

rsrc.obj: rsrc.asm

rsrc.asm: rsrc.rc rsrc.h
    $(RC) $(RCFLAGS) -o $@ $(*B).rc

clean:
    $(RM) *.OBJ $(TARGET).exe rsrc.asm $(TARGET).map *.pch

