# $Id: makefile,v 1.3 2000-02-26 19:20:02 bird Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\tools\bin

TARGET = glide2x

!include $(PDWIN32_INCLUDE)/pdwin32.mk

CFLAGS = $(CFLAGS)  -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS)  -I$(PDWIN32_INCLUDE)

OBJS =	initterm.obj \
	glideres.obj \
	..\swlibs\pcilib\pcilib.lib \
	texus\texus.lib \
	init\init.lib \
        glide\glide.lib \

all: ..\swlibs\pcilib\pcilib.lib init\init.lib texus\texus.lib glide\glide.lib $(TARGET).dll $(TARGET).lib

..\swlibs\pcilib\pcilib.lib:
    cd ..\swlibs\pcilib
    $(MAKE_CMD)
    cd ..\..\sst1

init\init.lib:
    cd init
    $(MAKE_CMD)
    cd ..

glide\glide.lib:
    cd glide
    $(MAKE_CMD)
    cd ..

texus\texus.lib:
    cd texus
    $(MAKE_CMD)
    cd ..

$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
          $(PDWIN32_LIB)/kernel32.lib \
          $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
    $(CP) $@ $(PDWIN32_BIN)\Glide\Voodoo2

lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET).def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).def
    $(CP) $@ $(PDWIN32_LIB)

RCFLAGS = $(RCFLAGS) -I..\swlibs\fxmisc -Iglide

glideres.asm: glide\glide.rc
    $(RC) $(RCFLAGS) -o glideres.asm glide\glide.rc

initterm.obj: initterm.cpp


#
# Please check this rule. I had to add the statments from 'cd cvs\init'.
#
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h
    cd ..\swlibs\pcilib
    $(MAKE_CMD) NODEP=1 dep
    cd ..\..\sst1\glide
    $(MAKE_CMD) NODEP=1 dep
    cd ..\init
    $(MAKE_CMD) NODEP=1 dep
    cd ..\texus
    $(MAKE_CMD) NODEP=1 dep
    cd ..
    cd ..\cvg\init
    $(MAKE_CMD) NODEP=1 dep
    cd ..\texus
    $(MAKE_CMD) NODEP=1 dep
    cd ..\glide
    $(MAKE_CMD) NODEP=1 dep
    cd ..

!ifndef NODEP
!include .depend
!endif

clean:
    $(RM) *.obj *.pch
    $(RM) *.map *.dll
    $(RM) *.lib
    $(RM) glideres.asm
    cd ..\swlibs\pcilib
    $(MAKE_CMD) clean
    cd ..\..\cvg\glide
    $(MAKE_CMD) clean
    cd ..\init
    $(MAKE_CMD) clean
    cd ..\texus
    $(MAKE_CMD) clean
    cd ..

