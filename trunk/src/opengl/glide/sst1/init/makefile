# $Id: makefile,v 1.1 2000-02-25 00:31:27 sandervl Exp $
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

TARGET1=init
TARGET2=initvg

CINCLUDE=-I..\incsrc;..\..\swlibs\fxmisc;initvg;..\glide;..\texus;..\..\swlibs\pcilib

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)

OBJS =	H3DRVR.OBJ      INIT.OBJ        VG96DRVR.OBJ    VGDRVR.OBJ \
 	$(TARGET2)\DAC.OBJ         $(TARGET2)\GAMMA.OBJ       $(TARGET2)\GDEBUG.OBJ      $(TARGET2)\INFO.OBJ        $(TARGET2)\PARSE.OBJ \
	$(TARGET2)\PRINT.OBJ       $(TARGET2)\SLI.OBJ         $(TARGET2)\SST1INIT.OBJ    $(TARGET2)\UTIL.OBJ        $(TARGET2)\VIDEO.OBJ

all: $(TARGET2).lib $(TARGET1).lib

$(TARGET1).lib: $(OBJS)
	$(RM) $(TARGET1).lib
	$(ILIB) $(ILIBFLAGS) $(TARGET1).lib $(OBJS),,


$(TARGET2).lib:
    cd initvg
    $(MAKE_CMD)
    cd ..

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h initvg\*.h ..\glide\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h 
    cd initvg
    $(MAKE_CMD) NODEP=1 dep

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.pch
    $(RM) *.lib
    cd initvg
    $(MAKE_CMD) clean
    cd ..

