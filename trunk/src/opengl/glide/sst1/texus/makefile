# $Id: makefile,v 1.2 2000-03-06 23:33:47 bird Exp $

#
# Odin32 API
#
#       Texus makefile
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
!include ..\3dfx.mak

# Flag overloads and local macros.
CLEANEXTRAS =

CINCLUDE    = -I..\incsrc;..\..\swlibs\fxmisc;..\init\initvg;..\init;..\glide;..\..\swlibs\pcilib
CFLAGS      = $(CFLAGS) $(CINCLUDE)
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE)


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\3DF.OBJ \
$(OBJDIR)\CLAMP.OBJ \
$(OBJDIR)\DEQUANT.OBJ \
$(OBJDIR)\DIFFUSE.OBJ \
$(OBJDIR)\GLDUTIL.OBJ \
$(OBJDIR)\MIPMAP.OBJ \
$(OBJDIR)\NCC.OBJ \
$(OBJDIR)\NCCNNET.OBJ \
$(OBJDIR)\PAL256.OBJ \
$(OBJDIR)\PPM.OBJ \
$(OBJDIR)\QUANTIZE.OBJ \
$(OBJDIR)\READ.OBJ \
$(OBJDIR)\RESAMPLE.OBJ \
$(OBJDIR)\RGT.OBJ \
$(OBJDIR)\TEXUSLIB.OBJ \
$(OBJDIR)\TGA.OBJ \
$(OBJDIR)\UTIL.OBJ \
$(OBJDIR)\VIEW.OBJ \
$(OBJDIR)\WRITE.OBJ


# Target name - name of the library without extention and path.
TARGET  = texus


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
        ..\incsrc\*.h ..\..\swlibs\fxmisc*.h ..\init\initvg\*.h \
        ..\glide\*.h ..\init\*.h ..\..\swlibs\pcilib\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

