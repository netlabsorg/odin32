# $Id: makefile,v 1.1 2000-02-09 08:48:27 jeroen Exp $
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


CINCLUDE=-I..\internals -I..\nurbtess -I..\..\..\mesa

CFLAGS = $(CFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -I..\..\include -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -I..\..\include -DNDEBUG

OBJS = directedLine.obj gridWrap.obj monoTriangulation.obj partitionY.obj \
       polyDBG.obj polyUtil.obj primitiveStream.obj quicksort.obj \
       rectBlock.obj sampleComp.obj sampleCompBot.obj sampleCompRight.obj \
       sampleCompTop.obj sampleMonoPoly.obj sampledLine.obj searchTree.obj \
       partitionX.obj monoChain.obj monoPolyPart.obj

all: $(OBJS)

#
# Dependencies
#
directedLine.obj: directedLine.cpp glimports.h zlassert.h quicksort.h directedLine.h polyDBG.h
gridWrap.obj: gridWrap.cpp zlassert.h gridWrap.h
monoTriangulation.obj: monoTriangulation.cpp glimports.h zlassert.h monoTriangulation.h polyUtil.h partitionX.h monoPolyPart.h
partitionY.obj: partitionY.cpp zlassert.h partitionY.h searchTree.h quicksort.h polyUtil.h
polyDBG.obj: polyDBG.cpp zlassert.h polyDBG.h
polyUtil.obj: polyUtil.cpp polyUtil.h
primitiveStream.obj: primitiveStream.cpp primitiveStream.h
quicksort.obj: quicksort.cpp
rectBlock.obj: rectBlock.cpp glimports.h zlassert.h rectBlock.h
sampleComp.obj: sampleComp.cpp glimports.h sampleComp.h sampleCompTop.h sampleCompBot.h sampleCompRight.h
sampleCompBot.obj: sampleCompBot.cpp zlassert.h sampleCompTop.h sampleCompBot.h
sampleCompRight.obj: sampleCompRight.cpp glimports.h zlassert.h sampleCompRight.h
sampleCompTop.obj: sampleCompTop.cpp zlassert.h sampleCompTop.h sampleCompRight.h
sampleMonoPoly.obj: sampleMonoPoly.cpp glimports.h zlassert.h sampleMonoPoly.h sampleComp.h polyDBG.h partitionX.h
sampledLine.obj: sampledLine.cpp glimports.h zlassert.h sampledLine.h
searchTree.obj: searchTree.cpp zlassert.h searchTree.h
partitionX.obj: partitionX.cpp partitionX.h
monoChain.obj: monoChain.cpp glimports.h zlassert.h monoChain.h quicksort.h searchTree.h polyUtil.h
monoPolyPart.obj: monoPolyPart.cpp directedLine.h monoPolyPart.h

clean:
    $(RM) *.obj

