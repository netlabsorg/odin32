# $Id: makefile,v 1.6 2000-03-06 23:33:49 bird Exp $
#
# PD-Win32 API
#
#       glu/nurbs/interface makefile
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

CINCLUDE    = -I..\..\..\mesa\GL -I..\.. -I..\internals -I..\nurbtess \
              -I..\..\include  -I..\..\..\mesa

CFLAGS      = $(CFLAGS) $(CINCLUDE)  -DNDEBUG
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE) -DNDEBUG


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\glcurveval.obj \
$(OBJDIR)\glinterface.obj \
$(OBJDIR)\glrenderer.obj \
$(OBJDIR)\glsurfeval.obj \
$(OBJDIR)\insurfeval.obj \
$(OBJDIR)\bezierPatch.obj \
$(OBJDIR)\bezierEval.obj \
$(OBJDIR)\bezierPatchMesh.obj \
$(OBJDIR)\incurveeval.obj


# Target name - name of the library without extention and path.
TARGET = interface


# Create all libraries (not importlibraries)
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).lib


# Create all libraries (not importlibraries)
libs: all


# Lib rule - build importlibrary (and evt. other libs)
$(OBJDIR)\$(TARGET).lib: $(OBJS)
    $(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS);


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc \
        ..\internals\*.h ..\nurbtess\*.h ..\..\..\mesa\GL\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

