# $Id: makefile,v 1.21 2000-02-09 23:40:43 bird Exp $
#
# ddraw.dll makefile
#
# Copyright 1998 Sander van Leeuwen
#           1999 Markus Montkowski
#
# Project Odin Software License can be found in LICENSE.TXT
#
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

ASFLAGS = -Sc -Sv:ALP

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE);
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE);

TARGET = ddraw

OBJS =  ddraw.obj os2ddraw.obj os2clipper.obj os2d3d.obj iccio1.obj asmutil.obj\
        os2surface.obj os2palette.obj os2palset.obj rectangle.obj initterm.obj \
        os2util.obj bltFunc.obj fillfunc.obj colorconv.obj resource.obj


LIBS =  $(PDWIN32_LIB)\ole32.lib $(PDWIN32_LIB)\advapi32.lib $(PDWIN32_LIB)\gdi32.lib \
        $(PDWIN32_LIB)\comctl32.lib $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)\odincrt.lib \
        $(PDWIN32_LIB)\user32.lib mmpm2.lib OS2386.LIB $(RTLLIB_O)

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(LIBS)
    $(CP) $@ $(PDWIN32_BIN)


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_BIN)\$(TARGET).dll \
        $(PDWIN32_LIB)\$(TARGET).lib \
        resource.asm $(TARGET)exp.def

