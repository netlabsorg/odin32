# $Id: makefile,v 1.3 2000-05-27 20:01:34 jeroen Exp $
#
# icmp.dll makefile
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
CLEANEXTRAS = icmprsrc.asm

# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\icmp.obj \
$(OBJDIR)\icmprsrc.obj \
$(PDWIN32_LIB)\dllentry.obj

# Target name - name of the dll without extention and path.
TARGET = icmp

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
OS2386.LIB
$(PDWIN32_LIB)/WSOCK32.LIB
$(RTLLIB_O)
$(PDWIN32_LIB)/odincrt.lib
$(PDWIN32_LIB)/kernel32.lib
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

