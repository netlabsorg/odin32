# $Id: makefile,v 1.3 2000-03-06 23:33:46 bird Exp $
#
# Odin32 API
#
#       sst1\Init makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\..\..\tools\bin

# Local cleanup only and extra cleanrule.
LOCALCLEAN = 1
CLEAN2     = 1

# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk
!include ..\3dfx.mak


# Flag overloads and local macros.
CLEANEXTRAS =

CINCLUDE    = -I..\incsrc;..\..\swlibs\fxmisc;initvg;..\glide;..\texus;..\..\swlibs\pcilib
CFLAGS      = $(CFLAGS) $(CINCLUDE)
CXXFLAGS    = $(CXXFLAGS) $(CINCLUDE)


# Object files. All objects should be prefixed with $(OBJDIR)!
INITSUB = initvg
OBJS = \
$(OBJDIR)\H3DRVR.OBJ \
$(OBJDIR)\INIT.OBJ \
$(OBJDIR)\VG96DRVR.OBJ \
$(OBJDIR)\VGDRVR.OBJ

# More object files
OBJSSUB = \
$(INITSUB)\$(OBJDIR)\DAC.OBJ \
$(INITSUB)\$(OBJDIR)\GAMMA.OBJ \
$(INITSUB)\$(OBJDIR)\GDEBUG.OBJ \
$(INITSUB)\$(OBJDIR)\INFO.OBJ \
$(INITSUB)\$(OBJDIR)\PARSE.OBJ \
$(INITSUB)\$(OBJDIR)\PRINT.OBJ \
$(INITSUB)\$(OBJDIR)\SLI.OBJ \
$(INITSUB)\$(OBJDIR)\SST1INIT.OBJ \
$(INITSUB)\$(OBJDIR)\UTIL.OBJ \
$(INITSUB)\$(OBJDIR)\VIDEO.OBJ


# Target name - name of the library without extention and path.
TARGET  = init


# All rule - build objs and target library.
all:    $(OBJDIR) \
        initsub \
        $(OBJDIR)\$(TARGET).lib \


# Make objectfiles in subdirectories.
initsub:
    cd $(INITSUB)
    $(MAKE_CMD) all
    cd ..


# Create all libraries (not importlibraries)
libs: all


# Lib rule - build importlibrary (and evt. other libs)
$(OBJDIR)\$(TARGET).lib: $(OBJS)
    $(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS) $(OBJSSUB);


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win $(CINCLUDE) \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        ..\incsrc\*.h ..\..\swlibs\fxmisc\*.h initvg\*.h ..\glide\*.h ..\texus\*.h ..\..\swlibs\pcilib\*.h
    cd initvg
    $(MAKE_CMD) $@
    cd ..


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post


# extra clean rule
clean2:
    cd initvg
    $(MAKE_CMD) clean
    cd ..
