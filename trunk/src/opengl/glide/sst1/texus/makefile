# $Id: makefile,v 1.1 2000-02-25 00:31:37 sandervl Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin

TARGET=TEXUS

!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\3dfx.mak

CINCLUDE=-I..\incsrc;..\..\swlibs\fxmisc;..\init\initvg;..\init;..\glide;..\..\swlibs\pcilib

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)

OBJS =	3DF.OBJ         CLAMP.OBJ       DEQUANT.OBJ     DIFFUSE.OBJ     GLDUTIL.OBJ \
	MIPMAP.OBJ      NCC.OBJ         NCCNNET.OBJ     PAL256.OBJ      PPM.OBJ \
	QUANTIZE.OBJ    READ.OBJ        RESAMPLE.OBJ    RGT.OBJ         TEXUSLIB.OBJ \
	TGA.OBJ         UTIL.OBJ        VIEW.OBJ        WRITE.OBJ

all: $(TARGET).lib

$(TARGET).lib: $(OBJS)
	$(RM) $(TARGET).lib
	$(ILIB) $(ILIBFLAGS) $(TARGET).lib $(OBJS) ,,

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\incsrc\*.h ..\..\swlibs\fxmisc*.h ..\init\initvg\*.h ..\glide\*.h ..\init\*.h ..\..\swlibs\pcilib\*.h 

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.lib

