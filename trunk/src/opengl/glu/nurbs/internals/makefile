# $Id: makefile,v 1.1 2000-02-09 08:50:24 jeroen Exp $
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

CINCLUDE=-I..\internals -I..\nurbtess -I..\..\..\mesa
CDEFS=-DSTANDALONE -DLIBRARYBUILD -DNDEBUG

CFLAGS = $(CFLAGS) $(CINCLUDE) -I..\..\include -I$(PDWIN32_INCLUDE) $(CDEFS)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) -I..\..\include -I$(PDWIN32_INCLUDE) $(CDEFS)

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

#
# Dependencies
#
arc.obj: arc.cpp myassert.h arc.h bin.h bezierarc.h pwlarc.h simplemath.h
arcsorter.obj: arcsorter.cpp arc.h arcsorter.h subdivider.h
arctess.obj: arctess.cpp myassert.h arctess.h bufpool.h simplemath.h bezierarc.h trimvertex.h trimvertpool.h
backend.obj: backend.cpp backend.h basiccrveval.h basicsurfeval.h nurbsconsts.h
basiccrveval.obj: basiccrveval.cpp types.h basiccrveval.h
basicsurfeval.obj: basicsurfeval.cpp types.h basicsurfeval.h
bin.obj: bin.cpp myassert.h bin.h
bufpool.obj: bufpool.cpp myassert.h bufpool.h
cachingeval.obj: cachingeval.cpp cachingeval.h
ccw.obj: ccw.cpp myassert.h subdivider.h types.h arc.h trimvertex.h simplemath.h mysetjmp.h
coveandtiler.obj: coveandtiler.cpp myassert.h coveandtiler.h gridvertex.h gridtrimvertex.h uarray.h backend.h
curve.obj: curve.cpp myassert.h mymath.h curve.h mapdesc.h types.h quilt.h nurbsconsts.h
curvelist.obj: curvelist.cpp myassert.h quilt.h curvelist.h curve.h nurbsconsts.h types.h
curvesub.obj: curvesub.cpp myassert.h subdivider.h renderhints.h backend.h quilt.h curvelist.h curve.h nurbsconsts.h
displaylist.obj: displaylist.cpp nurbstess.h displaylist.h
flist.obj: flist.cpp myassert.h flist.h
flistsorter.obj: flistsorter.cpp flistsorter.h
hull.obj: hull.cpp myassert.h hull.h gridvertex.h gridtrimvertex.h gridline.h trimline.h uarray.h trimregion.h
intersect.obj: intersect.cpp myassert.h subdivider.h arc.h bin.h backend.h trimvertpool.h
knotvector.obj: knotvector.cpp myassert.h knotvector.h defines.h
mapdesc.obj: mapdesc.cpp myassert.h mystring.h mymath.h backend.h nurbsconsts.h mapdesc.h
mapdescv.obj: mapdescv.cpp myassert.h mystring.h mymath.h nurbsconsts.h mapdesc.h
maplist.obj: maplist.cpp myassert.h mymath.h nurbsconsts.h maplist.h mapdesc.h backend.h
mesher.obj: mesher.cpp myassert.h mesher.h gridvertex.h gridtrimvertex.h jarcloc.h gridline.h trimline.h uarray.h backend.h
monotonizer.obj: monotonizer.cpp myassert.h arc.h arctess.h bezierarc.h bin.h mapdesc.h nurbsconsts.h subdivider.h
mycode.obj: mycode.cpp mymath.h
nurbsinterfac.obj: nurbsinterfac.cpp nurbsconsts.h nurbstess.h bufpool.h quilt.h displaylist.h knotvector.h mapdesc.h
nurbstess.obj: nurbstess.cpp myassert.h mysetjmp.h nurbsconsts.h nurbstess.h bufpool.h quilt.h knotvector.h mapdesc.h maplist.h
patch.obj: patch.cpp myassert.h mymath.h mystring.h patch.h mapdesc.h quilt.h nurbsconsts.h simplemath.h
patchlist.obj: patchlist.cpp myassert.h quilt.h patchlist.h patch.h nurbsconsts.h
quilt.obj: quilt.cpp myassert.h quilt.h backend.h mapdesc.h flist.h knotvector.h patchlist.h simplemath.h
reader.obj: reader.cpp nurbsconsts.h reader.h trimvertex.h simplemath.h
renderhints.obj: renderhints.cpp renderhints.h defines.h nurbsconsts.h
slicer.obj: slicer.cpp myassert.h bufpool.h slicer.h backend.h arc.h gridtrimvertex.h trimvertex.h varray.h
sorter.obj: sorter.cpp sorter.h
splitarcs.obj: splitarcs.cpp myassert.h subdivider.h arcsorter.h arc.h bin.h
subdivider.obj: subdivider.cpp myassert.h subdivider.h arc.h bezierarc.h bin.h renderhints.h backend.h mapdesc.h quilt.h patchlist.h patch.h nurbsconsts.h trimvertpool.h simplemath.h
tobezier.obj: tobezier.cpp myassert.h mystring.h quilt.h knotvector.h
trimline.obj: trimline.cpp myassert.h backend.h trimline.h
trimregion.obj: trimregion.cpp myassert.h backend.h trimregion.h
trimvertpool.obj: trimvertpool.cpp myassert.h mystring.h trimvertex.h trimvertpool.h bufpool.h
uarray.obj: uarray.cpp myassert.h uarray.h arc.h
varray.obj: varray.cpp myassert.h varray.h arc.h
dataTransform.obj: dataTransform.cpp myassert.h nurbsconsts.h trimvertex.h datatransform.h
monoTriangulationBackend.obj: monoTriangulationBackend.cpp backend.h arc.h

clean:
    $(RM) *.obj

