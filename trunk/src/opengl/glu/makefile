# $Id: makefile,v 1.10 2000-02-29 13:56:48 sandervl Exp $

#
# PD-Win32
#
# Top-level makefile for glu32
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin
PDWIN32_TOOLS = ..\..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

!ifdef DEBUG
!ifdef NODEBUGINFO
MAKE_CMD = nmake -nologo DEBUG=1 NODEBUGINFO=1
L=/NODEBUG
!else
MAKE_CMD = nmake -nologo DEBUG=1
L=/DE
!endif
!else
MAKE_CMD = nmake -nologo
!endif

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) -I..\mesa\GL
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) -I..\mesa\GL

TARGET = GLU32

all: $(TARGET).DLL $(TARGET).LIB

SOURCES = \
  nurbs\interface\bezierEval.cpp \
  nurbs\interface\bezierPatch.cpp \
  nurbs\interface\bezierPatchMesh.cpp \
  nurbs\interface\glcurveval.cpp \
  nurbs\interface\glinterface.cpp \
  nurbs\interface\glrenderer.cpp \
  nurbs\interface\glsurfeval.cpp \
  nurbs\interface\incurveeval.cpp \
  nurbs\interface\insurfeval.cpp \
  nurbs\interface\bezierEval.h \
  nurbs\interface\bezierPatch.h \
  nurbs\interface\bezierPatchMesh.h \
  nurbs\interface\glcurveval.h \
  nurbs\interface\glimports.h \
  nurbs\interface\glrenderer.h \
  nurbs\interface\glsurfeval.h \
  nurbs\interface\mystdio.h \
  nurbs\interface\mystdlib.h \
  nurbs\internals\arc.cpp \
  nurbs\internals\arcsorter.cpp \
  nurbs\internals\arctess.cpp \
  nurbs\internals\backend.cpp \
  nurbs\internals\basiccrveval.cpp \
  nurbs\internals\basicsurfeval.cpp \
  nurbs\internals\bin.cpp \
  nurbs\internals\bufpool.cpp \
  nurbs\internals\cachingeval.cpp \
  nurbs\internals\ccw.cpp \
  nurbs\internals\coveandtiler.cpp \
  nurbs\internals\curve.cpp \
  nurbs\internals\curvelist.cpp \
  nurbs\internals\curvesub.cpp \
  nurbs\internals\dataTransform.cpp \
  nurbs\internals\displaylist.cpp \
  nurbs\internals\flist.cpp \
  nurbs\internals\flistsorter.cpp \
  nurbs\internals\hull.cpp \
  nurbs\internals\intersect.cpp \
  nurbs\internals\knotvector.cpp \
  nurbs\internals\mapdesc.cpp \
  nurbs\internals\mapdescv.cpp \
  nurbs\internals\maplist.cpp \
  nurbs\internals\mesher.cpp \
  nurbs\internals\monotonizer.cpp \
  nurbs\internals\monoTriangulationBackend.cpp \
  nurbs\internals\mycode.cpp \
  nurbs\internals\nurbsinterfac.cpp \
  nurbs\internals\nurbstess.cpp \
  nurbs\internals\patch.cpp \
  nurbs\internals\patchlist.cpp \
  nurbs\internals\quilt.cpp \
  nurbs\internals\reader.cpp \
  nurbs\internals\renderhints.cpp \
  nurbs\internals\slicer.cpp \
  nurbs\internals\sorter.cpp \
  nurbs\internals\splitarcs.cpp \
  nurbs\internals\subdivider.cpp \
  nurbs\internals\tobezier.cpp \
  nurbs\internals\trimline.cpp \
  nurbs\internals\trimregion.cpp \
  nurbs\internals\trimvertpool.cpp \
  nurbs\internals\uarray.cpp \
  nurbs\internals\varray.cpp \
  nurbs\internals\arc.h \
  nurbs\internals\arcsorter.h \
  nurbs\internals\arctess.h \
  nurbs\internals\backend.h \
  nurbs\internals\basiccrveval.h \
  nurbs\internals\basicsurfeval.h \
  nurbs\internals\bezierarc.h \
  nurbs\internals\bin.h \
  nurbs\internals\bufpool.h \
  nurbs\internals\cachingeval.h \
  nurbs\internals\coveandtiler.h \
  nurbs\internals\curve.h \
  nurbs\internals\curvelist.h \
  nurbs\internals\dataTransform.h \
  nurbs\internals\defines.h \
  nurbs\internals\displaylist.h \
  nurbs\internals\displaymode.h \
  nurbs\internals\flist.h \
  nurbs\internals\flistsorter.h \
  nurbs\internals\gridline.h \
  nurbs\internals\gridtrimvertex.h \
  nurbs\internals\gridvertex.h \
  nurbs\internals\hull.h \
  nurbs\internals\jarcloc.h \
  nurbs\internals\knotvector.h \
  nurbs\internals\mapdesc.h \
  nurbs\internals\maplist.h \
  nurbs\internals\mesher.h \
  nurbs\internals\myassert.h \
  nurbs\internals\mymath.h \
  nurbs\internals\mysetjmp.h \
  nurbs\internals\mystring.h \
  nurbs\internals\nurbsconsts.h \
  nurbs\internals\nurbstess.h \
  nurbs\internals\patch.h \
  nurbs\internals\patchlist.h \
  nurbs\internals\pwlarc.h \
  nurbs\internals\quilt.h \
  nurbs\internals\reader.h \
  nurbs\internals\renderhints.h \
  nurbs\internals\simplemath.h \
  nurbs\internals\slicer.h \
  nurbs\internals\sorter.h \
  nurbs\internals\subdivider.h \
  nurbs\internals\trimline.h \
  nurbs\internals\trimregion.h \
  nurbs\internals\trimvertex.h \
  nurbs\internals\trimvertpool.h \
  nurbs\internals\types.h \
  nurbs\internals\uarray.h \
  nurbs\internals\varray.h \
  nurbs\nurbtess\directedLine.cpp \
  nurbs\nurbtess\gridWrap.cpp \
  nurbs\nurbtess\monoChain.cpp \
  nurbs\nurbtess\monoPolyPart.cpp \
  nurbs\nurbtess\monoTriangulation.cpp \
  nurbs\nurbtess\partitionX.cpp \
  nurbs\nurbtess\partitionY.cpp \
  nurbs\nurbtess\polyDBG.cpp \
  nurbs\nurbtess\polyUtil.cpp \
  nurbs\nurbtess\primitiveStream.cpp \
  nurbs\nurbtess\quicksort.cpp \
  nurbs\nurbtess\rectBlock.cpp \
  nurbs\nurbtess\sampleComp.cpp \
  nurbs\nurbtess\sampleCompBot.cpp \
  nurbs\nurbtess\sampleCompRight.cpp \
  nurbs\nurbtess\sampleCompTop.cpp \
  nurbs\nurbtess\sampledLine.cpp \
  nurbs\nurbtess\sampleMonoPoly.cpp \
  nurbs\nurbtess\searchTree.cpp \
  nurbs\nurbtess\definitions.h \
  nurbs\nurbtess\directedLine.h \
  nurbs\nurbtess\glimports.h \
  nurbs\nurbtess\gridWrap.h \
  nurbs\nurbtess\monoChain.h \
  nurbs\nurbtess\monoPolyPart.h \
  nurbs\nurbtess\monoTriangulation.h \
  nurbs\nurbtess\mystdio.h \
  nurbs\nurbtess\mystdlib.h \
  nurbs\nurbtess\partitionX.h \
  nurbs\nurbtess\partitionY.h \
  nurbs\nurbtess\polyDBG.h \
  nurbs\nurbtess\polyUtil.h \
  nurbs\nurbtess\primitiveStream.h \
  nurbs\nurbtess\quicksort.h \
  nurbs\nurbtess\rectBlock.h \
  nurbs\nurbtess\sampleComp.h \
  nurbs\nurbtess\sampleCompBot.h \
  nurbs\nurbtess\sampleCompRight.h \
  nurbs\nurbtess\sampleCompTop.h \
  nurbs\nurbtess\sampledLine.h \
  nurbs\nurbtess\sampleMonoPoly.h \
  nurbs\nurbtess\searchTree.h \
  nurbs\nurbtess\zlassert.h \
  tess\dict.c \
  tess\geom.c \
  tess\memalloc.c \
  tess\mesh.c \
  tess\normal.c \
  tess\priorityq.c \
  tess\render.c \
  tess\sweep.c \
  tess\tess.c \
  tess\tessmono.c \
  tess\dict-list.h \
  tess\dict.h \
  tess\geom.h \
  tess\memalloc.h \
  tess\mesh.h \
  tess\normal.h \
  tess\priorityq-heap.h \
  tess\priorityq-sort.h \
  tess\priorityq.h \
  tess\render.h \
  tess\sweep.h \
  tess\tess.h \
  tess\tessmono.h \
  util\glue.c \
  util\error.c \
  util\mipmap.c \
  util\project.c \
  util\quad.c \
  util\registry.c \
  util\gluint.h

OBJS = \
  initterm.obj \
  resource.obj \
  nurbs\interface\bezierEval.obj \
  nurbs\interface\bezierPatch.obj \
  nurbs\interface\bezierPatchMesh.obj \
  nurbs\interface\glcurveval.obj \
  nurbs\interface\glinterface.obj \
  nurbs\interface\glrenderer.obj \
  nurbs\interface\glsurfeval.obj \
  nurbs\interface\incurveeval.obj \
  nurbs\interface\insurfeval.obj \
  nurbs\internals\arc.obj \
  nurbs\internals\arcsorter.obj \
  nurbs\internals\arctess.obj \
  nurbs\internals\backend.obj \
  nurbs\internals\basiccrveval.obj \
  nurbs\internals\basicsurfeval.obj \
  nurbs\internals\bin.obj \
  nurbs\internals\bufpool.obj \
  nurbs\internals\cachingeval.obj \
  nurbs\internals\ccw.obj \
  nurbs\internals\coveandtiler.obj \
  nurbs\internals\curve.obj \
  nurbs\internals\curvelist.obj \
  nurbs\internals\curvesub.obj \
  nurbs\internals\dataTransform.obj \
  nurbs\internals\displaylist.obj \
  nurbs\internals\flist.obj \
  nurbs\internals\flistsorter.obj \
  nurbs\internals\hull.obj \
  nurbs\internals\intersect.obj \
  nurbs\internals\knotvector.obj \
  nurbs\internals\mapdesc.obj \
  nurbs\internals\mapdescv.obj \
  nurbs\internals\maplist.obj \
  nurbs\internals\mesher.obj \
  nurbs\internals\monotonizer.obj \
  nurbs\internals\monoTriangulationBackend.obj \
  nurbs\internals\mycode.obj \
  nurbs\internals\nurbsinterfac.obj \
  nurbs\internals\nurbstess.obj \
  nurbs\internals\patch.obj \
  nurbs\internals\patchlist.obj \
  nurbs\internals\quilt.obj \
  nurbs\internals\reader.obj \
  nurbs\internals\renderhints.obj \
  nurbs\internals\slicer.obj \
  nurbs\internals\sorter.obj \
  nurbs\internals\splitarcs.obj \
  nurbs\internals\subdivider.obj \
  nurbs\internals\tobezier.obj \
  nurbs\internals\trimline.obj \
  nurbs\internals\trimregion.obj \
  nurbs\internals\trimvertpool.obj \
  nurbs\internals\uarray.obj \
  nurbs\internals\varray.obj \
  nurbs\nurbtess\directedLine.obj \
  nurbs\nurbtess\gridWrap.obj \
  nurbs\nurbtess\monoChain.obj \
  nurbs\nurbtess\monoPolyPart.obj \
  nurbs\nurbtess\monoTriangulation.obj \
  nurbs\nurbtess\partitionX.obj \
  nurbs\nurbtess\partitionY.obj \
  nurbs\nurbtess\polyDBG.obj \
  nurbs\nurbtess\polyUtil.obj \
  nurbs\nurbtess\primitiveStream.obj \
  nurbs\nurbtess\quicksort.obj \
  nurbs\nurbtess\rectBlock.obj \
  nurbs\nurbtess\sampleComp.obj \
  nurbs\nurbtess\sampleCompBot.obj \
  nurbs\nurbtess\sampleCompRight.obj \
  nurbs\nurbtess\sampleCompTop.obj \
  nurbs\nurbtess\sampledLine.obj \
  nurbs\nurbtess\sampleMonoPoly.obj \
  nurbs\nurbtess\searchTree.obj \
  tess\dict.obj \
  tess\geom.obj \
  tess\memalloc.obj \
  tess\mesh.obj \
  tess\normal.obj \
  tess\priorityq.obj \
  tess\render.obj \
  tess\sweep.obj \
  tess\tess.obj \
  tess\tessmono.obj \
  util\glue.obj \
  util\error.obj \
  util\mipmap.obj \
  util\project.obj \
  util\quad.obj \
  util\registry.obj


$(TARGET).DLL: $(SOURCES) initterm.obj resource.obj $(TARGET).lrf $(TARGET).def
    cd util
    $(MAKE_CMD)
    cd ..\tess
    $(MAKE_CMD)
    cd ..\nurbs
    $(MAKE_CMD)
    cd ..
    ILINK /FREE /NOE $(L) @$(TARGET).lrf
    $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lrf: makefile
    @echo Creating file <<$(@B).lrf
$(OBJS)
/OUT:$(TARGET).DLL
$(PDWIN32_LIB)\ODINCRT.LIB
$(PDWIN32_LIB)\KERNEL32.LIB
$(PDWIN32_LIB)\OPENGL32.LIB
$(RTLLIB_O)
OS2386.LIB
$(TARGET).def
<<keep


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\mesa\GL \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h
    cd util
    $(MAKE_CMD) NODEP=1 dep
    cd ..\tess
    $(MAKE_CMD) NODEP=1 dep
    cd ..\nurbs
    $(MAKE_CMD) NODEP=1 dep
    cd ..

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.dll *.lib *.lrf *exp.def resource.asm \
      $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib
    cd util
    $(MAKE_CMD) clean
    cd ..\tess
    $(MAKE_CMD) clean
    cd ..\nurbs
    $(MAKE_CMD) clean
    cd ..

