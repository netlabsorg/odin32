# $Id: makefile,v 1.1 2000-02-25 00:31:20 sandervl Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin

TARGET=glide

!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\3dfx.mak

CINCLUDE=-I..\incsrc;..\..\swlibs\fxmisc;..\init\initvg;..\init;..\texus;..\..\swlibs\pcilib;$(PDWIN32_INCLUDE)

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)

OBJS =	CPUDTECT.OBJ    DDGUMP.OBJ      DIGLIDE.OBJ     DIGUTEX.OBJ     DISST.OBJ \
	DITEX.OBJ       G3DF.OBJ        GAA.OBJ         GBANNER.OBJ \
	GDRAW.OBJ       GERROR.OBJ      GGLIDE.OBJ      GLFB.OBJ        GMOVIE.OBJ \
	GPCI.OBJ        GSPLASH.OBJ     GSST.OBJ        GTEX.OBJ        GTEXDL.OBJ \
	GU.OBJ          GUCLIP.OBJ      GUMP.OBJ        GUTEX.OBJ       GXDRAW.OBJ \
	P6FENCE.OBJ     SST96.OBJ


all: $(TARGET).lib

$(TARGET).lib: $(OBJS)
	$(RM) $(TARGET).lib
	$(ILIB) $(ILIBFLAGS) $(TARGET).lib $(OBJS) ,,

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\init\initvg\*.h ..\init\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h 

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.pch
    $(RM) *.lib

