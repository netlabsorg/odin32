# $Id: makefile,v 1.18 2000-02-05 01:55:52 sandervl Exp $
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


LIBS =  $(PDWIN32_LIB)\pmwinx.lib $(PDWIN32_LIB)\advapi32.lib $(PDWIN32_LIB)\gdi32.lib \
        $(PDWIN32_LIB)\comctl32.lib $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)\odincrt.lib \
        $(PDWIN32_LIB)\user32.lib mmpm2.lib OS2386.LIB $(RTLLIB_O)

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(LIBS)
  $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET).def
  $(IMPLIB) $(IMPLIBFLAGS) $@ $**
  $(CP) $@ $(PDWIN32_LIB)


ddraw.obj: ddraw.cpp  \
    os2ddraw.h        \
    $(PDWIN32_INCLUDE)\win\ddraw.h

os2ddraw.obj: os2ddraw.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

os2d3d.obj: os2d3d.cpp \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

os2palette.obj: os2palette.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2palset.h \
    os2ddraw.h

os2palset.obj: os2palset.cpp  \
    os2palset.h

os2clipper.obj: os2clipper.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2ddraw.h\
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

os2surface.obj: os2surface.cpp  \
    os2palette.h \
    os2clipper.h \
    os2surface.h \
    os2ddraw.h\
    rectangle.h \
    $(PDWIN32_INCLUDE)\win\ddraw.h \
    $(PDWIN32_INCLUDE)\win\d3d.h

resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

rectangle.obj: rectangle.cpp rectangle.h

initterm.obj: initterm.cpp initterm.h $(PDWIN32_INCLUDE)\exitlist.h

os2util.obj:  os2util.cpp os2util.h

bltFunc.obj: bltFunc.cpp bltFunc.h \
    $(PDWIN32_INCLUDE)\misc.h

fillfunc.obj: fillfunc.cpp fillfunc.h \
    $(PDWIN32_INCLUDE)\misc.h

colorconv.obj: colorconv.cpp colorconv.h \
    $(PDWIN32_INCLUDE)\misc.h

iccio1.obj: iccio1.asm

asmutil.obj: asmutil.asm

clean:
  $(RM) *.obj *.lib *.dll *.map *.pch
  $(RM) $(PDWIN32_BIN)\$(TARGET).dll
  $(RM) $(PDWIN32_LIB)\$(TARGET).lib
