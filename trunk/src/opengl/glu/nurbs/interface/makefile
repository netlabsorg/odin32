# $Id: makefile,v 1.2 2000-02-09 23:40:51 bird Exp $
#
# PD-Win32 API
#
#       glu/nurbs/interface makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CINCLUDE=-I..\.. -I..\internals -I..\nurbtess -I..\..\..\mesa -I$(PDWIN32_INCLUDE) -I..\..\include

CFLAGS = $(CFLAGS) $(CINCLUDE)  -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -DNDEBUG

OBJS = glcurveval.obj glinterface.obj glrenderer.obj glsurfeval.obj \
       insurfeval.obj bezierPatch.obj bezierEval.obj bezierPatchMesh.obj \
       incurveeval.obj

all: $(OBJS)


dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc \
        ..\internals\*.h ..\nurbtess\*.h ..\..\..\mesa\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.pch

