# $Id: makefile,v 1.1 2000-02-25 00:37:52 sandervl Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\3dfx.mak

TARGET=init

CINCLUDE=-I..\incsrc;..\..\swlibs\fxmisc;..\glide;..\texus;..\..\swlibs\pcilib

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)

OBJS =	DAC.OBJ GAMMA.OBJ GDEBUG.OBJ INFO.OBJ PARSE.OBJ \
	PRINT.OBJ SLI.OBJ SST1INIT.OBJ UTIL.OBJ VIDEO.OBJ

all: $(TARGET).lib

$(TARGET).lib: $(OBJS)
	$(RM) $(TARGET).lib
	$(ILIB) $(ILIBFLAGS) $(TARGET).lib $(OBJS),,



dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\glide\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h 

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.pch
    $(RM) *.lib

