# $Id: makefile,v 1.4 2000-03-01 10:44:25 bird Exp $
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

CINCLUDE=-I..\..\..\mesa\GL -I..\.. -I..\internals -I..\nurbtess -I$(PDWIN32_INCLUDE) -I..\..\include  -I..\..\..\mesa

CFLAGS = $(CFLAGS) $(CINCLUDE)  -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -DNDEBUG

OBJS = glcurveval.obj glinterface.obj glrenderer.obj glsurfeval.obj \
       insurfeval.obj bezierPatch.obj bezierEval.obj bezierPatchMesh.obj \
       incurveeval.obj

all: $(OBJS)


dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc \
        ..\internals\*.h ..\nurbtess\*.h
# removed: ..\..\..\mesa\GL\*.h - cyclic dep: gl.h <-> mesa_wgl.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.pch

