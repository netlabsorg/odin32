# $Id: makefile,v 1.1 2000-02-09 08:49:03 jeroen Exp $
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

CINCLUDE=-I..\.. -I..\internals -I..\nurbtess -I..\..\..\mesa

CFLAGS = $(CFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -I..\..\include -DNDEBUG
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -I$(PDWIN32_INCLUDE) -I..\..\include -DNDEBUG

OBJS = glcurveval.obj glinterface.obj glrenderer.obj glsurfeval.obj \
       insurfeval.obj bezierPatch.obj bezierEval.obj bezierPatchMesh.obj \
       incurveeval.obj

all: $(OBJS)

#
# Dependencies
#
glcurveval.obj: glcurveval.cpp glimports.h glrenderer.h glcurveval.h
glinterface.obj: glinterface.cpp glimports.h glrenderer.h
glrenderer.obj: glrenderer.cpp glimports.h glrenderer.h
glsurfeval.obj: glsurfeval.cpp glimports.h glrenderer.h glsurfeval.h bezierPatchMesh.h
insurfeval.obj: insurfeval.cpp glsurfeval.h
bezierPatch.obj: bezierPatch.cpp bezierPatch.h bezierEval.h
bezierEval.obj: bezierEval.cpp bezierEval.h
bezierPatchMeshobj: bezierPatchMesh.cpp bezierPatchMesh.h bezierEval.h
incurveeval.obj: incurveeval.cpp glcurveval.h

clean:
    $(RM) *.obj

