#
# PD-Win32 API
#
#

# Tell that we're producing an executable
EXETARGET = 1

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

TARGET = odininst

all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).exe \
        $(PDWIN32_BIN)\$(TARGET).exe

# Flag overloads and local macros.
CLEANEXTRAS = $(PDWIN32_BIN)\$(TARGET).exe

# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = $(OBJDIR)\odininst.obj


# Lib rule - dummy rule.
lib:


# Exe rule - builds the target exe.
$(OBJDIR)\$(TARGET).exe: $(OBJS) $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS_ODINCRT) @$(OBJDIR)\$(TARGET).lrf

# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).exe
/MAP:$(OBJDIR)\$(TARGET).map
/PMTYPE:pm
/STACK:0x100000
/NOBASE
$(OBJS)
os2386.lib
$(PDWIN32_LIB)\kernel32.lib
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post
