# $Id: makefile,v 1.16 2000-03-18 19:48:53 sandervl Exp $

#
# Odin32 API
#
#       msvcrt.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = msvcrtrsrc.asm msvcrt20rsrc.asm msvcrt40rsrc.asm \
              $(PDWIN32_BIN)\msvcrt.dll   $(PDWIN32_LIB)\msvcrt.lib   \
              $(PDWIN32_BIN)\msvcrt20.dll $(PDWIN32_LIB)\msvcrt20.lib \
              $(PDWIN32_BIN)\msvcrt40.dll $(PDWIN32_LIB)\msvcrt40.lib


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\msvcrt.obj \
$(OBJDIR)\util.obj \
$(OBJDIR)\$(TARGET)rsrc.obj \
$(PDWIN32_LIB)\dllentry.obj


!ifndef TARGET

# Dummy all rule - if not both all and lib will be invoked when
# invoking nmake without a target.
_all: all


# All and lib rules - builds all targets by reinvoking make with target macro set.
all lib:
    $(MAKE_CMD) TARGET=msvcrt   $@
    $(MAKE_CMD) TARGET=msvcrt20 $@
    $(MAKE_CMD) TARGET=msvcrt40 $@


!else


# Real all rule - build objs, target dll, copies dll to bin and makes libs.
all:   $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Real lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

!endif


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    -4 $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/crtdll.lib
$(PDWIN32_LIB)/odincrt.lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Dummy target used for clean
!ifndef TARGET
TARGET = msvcrt
!endif


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

