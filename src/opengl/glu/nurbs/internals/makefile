# $Id: makefile,v 1.4 2000-03-06 23:33:49 bird Exp $

#
# Odin32 API
#
#       glu/nurbs/internals makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\..\tools\bin


# Local cleanup only.
LOCALCLEAN = 1


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS =

CINCLUDE    = -I..\internals -I..\nurbtess -I..\..\..\mesa\GL -I..\..\include
CDEFS       = -DSTANDALONE -DLIBRARYBUILD -DNDEBUG

CFLAGS      = $(CFLAGS) $(CINCLUDE) $(CDEFS)
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE) $(CDEFS)


# Object files. All objects should be prefixed with $(OBJDIR)!
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
$(OBJDIR)\mycode.obj

OBJS2 = \
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


# Target name - name of the library without extention and path.
TARGET = internals


# Create all libraries (not importlibraries)
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).lib


# Create all libraries (not importlibraries)
libs: all


# Lib rule - build importlibrary (and evt. other libs)
$(OBJDIR)\$(TARGET).lib: $(OBJS) $(OBJS2)
    $(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS);
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS2);


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc  \
        ..\nurbtess\*.h ..\..\..\mesa\GL\*.h ..\..\include\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

