# $Id: makefile,v 1.3 2000-02-29 13:56:49 sandervl Exp $

#
# PD-Win32 API
#
#       glu/nurbs/nurbtess makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CINCLUDE=-I..\internals -I..\nurbtess -I..\..\..\mesa\GL -I$(PDWIN32_INCLUDE) -I..\..\include

CFLAGS = $(CFLAGS) $(CINCLUDE) -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -DNDEBUG

OBJS = directedLine.obj gridWrap.obj monoTriangulation.obj partitionY.obj \
       polyDBG.obj polyUtil.obj primitiveStream.obj quicksort.obj \
       rectBlock.obj sampleComp.obj sampleCompBot.obj sampleCompRight.obj \
       sampleCompTop.obj sampleMonoPoly.obj sampledLine.obj searchTree.obj \
       partitionX.obj monoChain.obj monoPolyPart.obj

all: $(OBJS)


dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc  \
        ..\..\mesa\*.h ..\nurbtess\*.h ..\internals\*.h ..\..\include\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.pch

