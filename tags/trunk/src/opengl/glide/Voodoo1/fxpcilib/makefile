# $Id: makefile,v 1.1 2000-02-18 10:45:12 sandervl Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin

TARGET=fxpcilib

!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\3dfx.mak

CINCLUDE=-I..\incsrc;..\fxmisc;..\init\initvg;..\glide;..\texus;..\init

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)

OBJS = fxos2.obj fxpci.obj

all: $(TARGET).lib

$(TARGET).lib: $(OBJS)
	$(RM) $(TARGET).lib
	$(ILIB) $(ILIBFLAGS) $(TARGET).lib $(OBJS) ,,

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\incsrc\*.h ..\fxmisc\*.h ..\init\initvg\*.h ..\glide*.h ..\texus\*.h ..\init\*.h 

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.pch
    $(RM) *.lib

