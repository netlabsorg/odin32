# $Id: makefile,v 1.3 2000-03-06 23:33:43 bird Exp $

#
# Odin32 API
#
#       Glide makefile
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
CLEANEXTRAS = glidersrc.asm

CINCLUDE = -I..\incsrc;..\..\swlibs\fxmisc;..\init\initvg;..\init;..\texus;..\..\swlibs\pcilib;
CFLAGS   = $(CFLAGS) $(CINCLUDE) /Os-
CXXFLAGS = $(CXXFLAGS) $(CINCLUDE) /Os-
RCFLAGS  = $(RCFLAGS) $(CINCLUDE)


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\CPUDTECT.OBJ \
$(OBJDIR)\DDGUMP.OBJ \
$(OBJDIR)\DIGLIDE.OBJ \
$(OBJDIR)\DIGUTEX.OBJ \
$(OBJDIR)\DISST.OBJ \
$(OBJDIR)\DITEX.OBJ \
$(OBJDIR)\G3DF.OBJ \
$(OBJDIR)\GAA.OBJ \
$(OBJDIR)\GBANNER.OBJ \
$(OBJDIR)\GDRAW.OBJ \
$(OBJDIR)\GERROR.OBJ \
$(OBJDIR)\GGLIDE.OBJ \
$(OBJDIR)\GLFB.OBJ \
$(OBJDIR)\GMOVIE.OBJ \
$(OBJDIR)\GPCI.OBJ \
$(OBJDIR)\GSPLASH.OBJ \
$(OBJDIR)\GSST.OBJ \
$(OBJDIR)\GTEX.OBJ \
$(OBJDIR)\GTEXDL.OBJ \
$(OBJDIR)\GU.OBJ \
$(OBJDIR)\GUCLIP.OBJ \
$(OBJDIR)\GUMP.OBJ \
$(OBJDIR)\GUTEX.OBJ \
$(OBJDIR)\GXDRAW.OBJ \
$(OBJDIR)\P6FENCE.OBJ \
$(OBJDIR)\FIFO.OBJ \
$(OBJDIR)\glidersrc.obj


# Target name - name of the library without extention and path.
TARGET  = glide


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
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h ..\init\initvg\*.h ..\init\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

