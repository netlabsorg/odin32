# $Id: makefile,v 1.6 2000-11-19 08:58:14 bird Exp $

#
# Odin32 API
#
#       glu/nurbs/internals makefile
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\..\tools\bin


#
# Tell buildenvironmet to build an object library and to invoke additional dep rule.
#
LIBTARGET = 1
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk


#
# Flag overloads.
#
CINCLUDES   = $(CINCLUDES) -I..\internals -I..\nurbtess -I..\..\..\mesa\GL -I..\..\include
CDEFINES    = $(CDEFINES)  -DSTANDALONE -DLIBRARYBUILD -DNDEBUG
CFLAGS      = $(CFLAGS) -O-


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\arc.obj \
$(OBJDIR)\arcsorter.obj \
$(OBJDIR)\arctess.obj \
$(OBJDIR)\backend.obj \
$(OBJDIR)\basiccrveval.obj \
$(OBJDIR)\basicsurfeval.obj \
$(OBJDIR)\bin.obj \
$(OBJDIR)\bufpool.obj \
$(OBJDIR)\cachingeval.obj \
$(OBJDIR)\ccw.obj \
$(OBJDIR)\coveandtiler.obj \
$(OBJDIR)\curve.obj \
$(OBJDIR)\curvelist.obj \
$(OBJDIR)\curvesub.obj \
$(OBJDIR)\displaylist.obj \
$(OBJDIR)\flist.obj \
$(OBJDIR)\flistsorter.obj \
$(OBJDIR)\hull.obj \
$(OBJDIR)\intersect.obj \
$(OBJDIR)\knotvector.obj \
$(OBJDIR)\mapdesc.obj \
$(OBJDIR)\mapdescv.obj \
$(OBJDIR)\maplist.obj \
$(OBJDIR)\mesher.obj \
$(OBJDIR)\monotonizer.obj \
$(OBJDIR)\mycode.obj \
$(OBJDIR)\nurbsinterfac.obj \
$(OBJDIR)\nurbstess.obj \
$(OBJDIR)\patch.obj \
$(OBJDIR)\patchlist.obj \
$(OBJDIR)\quilt.obj \
$(OBJDIR)\reader.obj \
$(OBJDIR)\renderhints.obj \
$(OBJDIR)\slicer.obj \
$(OBJDIR)\sorter.obj \
$(OBJDIR)\splitarcs.obj \
$(OBJDIR)\subdivider.obj \
$(OBJDIR)\tobezier.obj \
$(OBJDIR)\trimline.obj \
$(OBJDIR)\trimregion.obj \
$(OBJDIR)\trimvertpool.obj \
$(OBJDIR)\uarray.obj \
$(OBJDIR)\varray.obj \
$(OBJDIR)\dataTransform.obj \
$(OBJDIR)\monoTriangulationBackend.obj


#
# Target name - name of the library without extention and path.
#
TARGET = internals


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        ..\nurbtess\*.h ..\..\..\mesa\GL\*.h ..\..\include\*.h

