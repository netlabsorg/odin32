# $Id: makefile,v 1.1 2000-02-25 00:33:57 sandervl Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin

TARGET=pcilib

!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\..\sst1\3dfx.mak

CINCLUDE=-I..\..\sst1\incsrc;..\fxmisc;..\..\sst1\init\initvg;..\..\sst1\glide;..\..\sst1\texus;..\..\sst1\init

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)

OBJS = fxos2.obj fxpci.obj sst1_pci.obj fxinfo.obj

all: $(TARGET).lib

$(TARGET).lib: $(OBJS)
	$(RM) $(TARGET).lib
	$(ILIB) $(ILIBFLAGS) $(TARGET).lib $(OBJS) ,,

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\..\sst1\incsrc\*.h ..\fxmisc\*.h ..\..\sst1\init\initvg\*.h ..\..\sst1\glide*.h ..\..\sst1\texus\*.h ..\..\sst1\init\*.h 

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.pch
    $(RM) *.lib

