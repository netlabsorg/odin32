# $Id: makefile,v 1.2 2000-03-06 23:33:47 bird Exp $
#
# PD-Win32 API
#
#       Glide makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\..\..\tools\bin


# Local cleanup only.
LOCALCLEAN = 1


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\..\3dfx.mak


# Flag overloads and local macros.
CLEANEXTRAS = glide.asm

CINCLUDE    = -I..\..\incsrc;..\..\..\swlibs\fxmisc;..\..\init;..\..\glide;..\..\texus;..\..\..\swlibs\pcilib
CFLAGS      = $(CFLAGS) $(CINCLUDE)
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE)


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\DAC.OBJ \
$(OBJDIR)\GAMMA.OBJ \
$(OBJDIR)\GDEBUG.OBJ \
$(OBJDIR)\INFO.OBJ \
$(OBJDIR)\PARSE.OBJ \
$(OBJDIR)\PRINT.OBJ \
$(OBJDIR)\SLI.OBJ \
$(OBJDIR)\SST1INIT.OBJ \
$(OBJDIR)\UTIL.OBJ \
$(OBJDIR)\VIDEO.OBJ


# All - rule make all objects.
all:    $(OBJDIR) \
        $(OBJS)


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\..\incsrc\*.h ..\..\..\swlibs\fxmisc\*.h ..\*.h ..\..\glide\*.h \
        ..\..\texus\*.h ..\..\..\swlibs\pcilib\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

