# $Id: makefile,v 1.17 2000-03-06 23:38:49 bird Exp $

#
# Odin32 API
#
#       odincrt.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = lz32rsrc.asm
NOTEXPDEF   = 1

CXXFLAGS    = $(CXXFLAGS_ODINCRT)
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase

!ifdef VAC36
RUNTIMELIBDEF   = odin36.def
!else
RUNTIMELIBDEF   = odincrt.def
!endif


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\odincrt.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\malloc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\file.obj


# Target name - name of the dll without extention and path.
TARGET = odincrt


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib

# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# ImpLib rule - build importlibrary (and evt. other libs)
$(TARGET).lib: $(TARGET).def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(RUNTIMELIBDEF) $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS_ODINCRT) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(RUNTIMELIBDEF)
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

