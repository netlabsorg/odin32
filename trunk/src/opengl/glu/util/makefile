# $Id: makefile,v 1.4 2000-03-06 23:33:50 bird Exp $

#
# Odin32 API
#
#       glu/util makefile
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
CFLAGS      = $(CFLAGS) $(CINCLUDE) -Tdp -DNDEBUG
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE)  -Tdp -DNDEBUG


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\error.obj \
$(OBJDIR)\glue.obj \
$(OBJDIR)\mipmap.obj \
$(OBJDIR)\project.obj \
$(OBJDIR)\quad.obj \
$(OBJDIR)\registry.obj


# Target name - name of the library without extention and path.
TARGET = util


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
        ..\include\*.h ..\..\mesa\*.h ..\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

