# $Id: makefile,v 1.24 2000-07-19 19:05:05 sandervl Exp $
#
# ddraw.dll makefile
#
# Copyright 1998 Sander van Leeuwen
#           1999 Markus Montkowski
#
# Project Odin Software License can be found in LICENSE.TXT
#
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = ddrawrsrc.asm
ASFLAGS     = -Sc -Sv:ALP


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\ddraw.obj \
$(OBJDIR)\os2ddraw.obj \
$(OBJDIR)\os2clipper.obj \
$(OBJDIR)\os2d3d.obj \
$(OBJDIR)\iccio1.obj \
$(OBJDIR)\asmutil.obj \
$(OBJDIR)\os2surface.obj \
$(OBJDIR)\os2palette.obj \
$(OBJDIR)\os2palset.obj \
$(OBJDIR)\rectangle.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\os2util.obj \
$(OBJDIR)\bltFunc.obj \
$(OBJDIR)\fillfunc.obj \
$(OBJDIR)\colorconv.obj \
$(OBJDIR)\ddrawrsrc.obj


# Target name - name of the dll without extention and path.
TARGET = ddraw


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/ole32.lib
$(PDWIN32_LIB)/advapi32.lib
$(PDWIN32_LIB)/gdi32.lib
$(PDWIN32_LIB)/comctl32.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/$(ODINCRT).lib
$(PDWIN32_LIB)/user32.lib
mmpm2.lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

