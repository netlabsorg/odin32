# $Id: makefile,v 1.12 2000-03-06 23:33:48 bird Exp $

#
# Odin32 API
#
# Top-level makefile for glu32
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\tools\bin

# Have extra clean rule.
CLEAN2 = 1


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = glu32rsrc.asm

CFLAGS      = $(CFLAGS) -I..\mesa\GL
CXXFLAGS    = $(CXXFLAGS) -I..\mesa\GL



# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\glu32rsrc.obj


# Target name - name of the library without extention and path.
TARGET = glu32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Libs rule - Make (sub) libraries. (Not import libraries!)
libs:
    cd util
    $(MAKE_CMD) $@
    cd ..\tess
    $(MAKE_CMD) $@
    cd ..\nurbs
    $(MAKE_CMD) $@
    cd ..


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) libs $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
nurbs\interface\$(OBJDIR)\interface.lib
nurbs\internals\$(OBJDIR)\internals.lib
nurbs\nurbtess\$(OBJDIR)\nurbtess.lib
tess\$(OBJDIR)\tess.lib
util\$(OBJDIR)\util.lib
$(PDWIN32_LIB)\ODINCRT.LIB
$(PDWIN32_LIB)\KERNEL32.LIB
$(PDWIN32_LIB)\OPENGL32.LIB
$(RTLLIB_O)
OS2386.LIB
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\mesa\GL \
    *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h
    cd util
    $(MAKE_CMD) $@
    cd ..\tess
    $(MAKE_CMD) $@
    cd ..\nurbs
    $(MAKE_CMD) $@
    cd ..


# Extra Clean.
clean2:
    cd util
    $(MAKE_CMD) clean
    cd ..\tess
    $(MAKE_CMD) clean
    cd ..\nurbs
    $(MAKE_CMD) clean
    cd ..


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

