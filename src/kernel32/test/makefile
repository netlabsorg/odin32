# $Id: makefile,v 1.1 1999-11-28 23:10:08 bird Exp $

#
# PD-Win32 API
#
#       kernel32test.exe makefile
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin
PDWIN32_TOOLS = ..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) /Tm+ -Ge+
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) /Tm+ -Ge+

OBJS =  os2main.obj \
        kernel32test.obj \
        resources.obj \
        testlib.obj \
        rsrc.obj

TARGET = kernel32test


all: $(TARGET).exe


$(TARGET).exe: $(OBJS)
    $(LD) $(LDFLAGS) -Ge+ -Fe$@ $(OBJS) \
        $(PDWIN32_LIB)\odincrt.lib \
        $(PDWIN32_LIB)\kernel32.lib \
        OS2386.LIB $(RTLLIB_O)

rsrc.obj: rsrc.asm

rsrc.asm: rsrc.rc rsrc.h
    $(RC) $(RCFLAGS) -o $@ $(*B).rc

clean:
    $(RM) *.OBJ $(TARGET).exe rsrc.asm $(TARGET).map *.pch
