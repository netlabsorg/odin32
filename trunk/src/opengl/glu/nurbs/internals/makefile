# $Id: makefile,v 1.2 2000-02-09 23:40:51 bird Exp $

#
# PD-Win32 API
#
#       glu/nurbs/internals makefile
#

PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB = ..\..\..\..\..\lib
PDWIN32_BIN = ..\..\..\..\..\bin
PDWIN32_TOOLS = ..\..\..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

CINCLUDE=-I..\internals -I..\nurbtess -I..\..\..\mesa -I..\..\include -I$(PDWIN32_INCLUDE)
CDEFS=-DSTANDALONE -DLIBRARYBUILD -DNDEBUG

CFLAGS = $(CFLAGS) $(CINCLUDE) $(CDEFS)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) $(CDEFS)

OBJS = arc.obj arcsorter.obj arctess.obj backend.obj basiccrveval.obj \
       basicsurfeval.obj bin.obj bufpool.obj cachingeval.obj ccw.obj \
       coveandtiler.obj curve.obj curvelist.obj curvesub.obj displaylist.obj \
       flist.obj flistsorter.obj hull.obj intersect.obj knotvector.obj \
       mapdesc.obj mapdescv.obj maplist.obj mesher.obj monotonizer.obj \
       mycode.obj nurbsinterfac.obj nurbstess.obj patch.obj patchlist.obj \
       quilt.obj reader.obj renderhints.obj slicer.obj sorter.obj \
       splitarcs.obj subdivider.obj tobezier.obj trimline.obj trimregion.obj \
       trimvertpool.obj uarray.obj varray.obj dataTransform.obj \
       monoTriangulationBackend.obj

all: $(OBJS)


dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc  \
        ..\nurbtess\*.h ..\..\..\mesa\*.h ..\..\include\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.pch

