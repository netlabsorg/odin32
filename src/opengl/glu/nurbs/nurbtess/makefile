# $Id: makefile,v 1.4 2000-03-06 23:33:49 bird Exp $

#
# Odin32 API
#
#       glu/nurbs/nurbtess makefile
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
CFLAGS      = $(CFLAGS) $(CINCLUDE) -DNDEBUG
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE) -DNDEBUG


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\directedLine.obj \
$(OBJDIR)\gridWrap.obj \
$(OBJDIR)\monoTriangulation.obj \
$(OBJDIR)\partitionY.obj \
$(OBJDIR)\polyDBG.obj \
$(OBJDIR)\polyUtil.obj \
$(OBJDIR)\primitiveStream.obj \
$(OBJDIR)\quicksort.obj \
$(OBJDIR)\rectBlock.obj \
$(OBJDIR)\sampleComp.obj \
$(OBJDIR)\sampleCompBot.obj \
$(OBJDIR)\sampleCompRight.obj \
$(OBJDIR)\sampleCompTop.obj \
$(OBJDIR)\sampleMonoPoly.obj \
$(OBJDIR)\sampledLine.obj \
$(OBJDIR)\searchTree.obj \
$(OBJDIR)\partitionX.obj \
$(OBJDIR)\monoChain.obj \
$(OBJDIR)\monoPolyPart.obj


# Target name - name of the library without extention and path.
TARGET = nurbtess


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
    $(DEPEND) $(CINCLUDE) *.c *.cpp *.h *.asm *.inc  \
        ..\..\mesa\*.h ..\nurbtess\*.h ..\internals\*.h ..\..\include\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

