# $Id: makefile,v 1.3 2000-03-06 23:33:47 bird Exp $
#
# Odin32 API
#
#       swlibs\pcilib makefile
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
!include ..\..\sst1\3dfx.mak


# Flag overloads and local macros.
CLEANEXTRAS =

CINCLUDE=-I..\..\sst1\incsrc;..\fxmisc;..\..\sst1\init\initvg;..\..\sst1\glide;..\..\sst1\texus;..\..\sst1\init;$(PDWIN32_INCLUDE)

CFLAGS = $(CFLAGS) $(CINCLUDE)
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE)


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\fxos2.obj \
$(OBJDIR)\fxpci.obj \
$(OBJDIR)\sst1_pci.obj \
$(OBJDIR)\fxinfo.obj


# Target name - name of the library without extention and path.
TARGET  = pcilib


# All rule - build objs and target library.
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
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\..\sst1\incsrc\*.h ..\fxmisc\*.h ..\..\sst1\init\initvg\*.h \
        ..\..\sst1\glide*.h ..\..\sst1\texus\*.h ..\..\sst1\init\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

