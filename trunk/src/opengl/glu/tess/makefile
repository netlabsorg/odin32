# $Id: makefile,v 1.4 2000-03-06 23:33:50 bird Exp $

#
# Odin32 API
#
#       glu/tess makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


# Local cleanup only.
LOCALCLEAN = 1


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS =

CINCLUDE    = -I.. -I..\..\mesa -I..\include -I..\..\mesa\GL
CDEFS       = -DNDEBUG -Tdp
CFLAGS      = $(CFLAGS) $(CINCLUDE) $(CDEFS) -DNDEBUG
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE) -$(CDEFS) -DNDEBUG



# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS= \
$(OBJDIR)\dict.obj \
$(OBJDIR)\memalloc.obj \
$(OBJDIR)\mesh.obj \
$(OBJDIR)\normal.obj \
$(OBJDIR)\priorityq.obj \
$(OBJDIR)\render.obj \
$(OBJDIR)\sweep.obj \
$(OBJDIR)\tess.obj \
$(OBJDIR)\tessmono.obj \
$(OBJDIR)\geom.obj


# Target name - name of the library without extention and path.
TARGET = tess


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
        ..\..\mesa\*.h ..\*.h ..\include\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

