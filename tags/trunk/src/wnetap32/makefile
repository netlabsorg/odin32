# $Id: makefile,v 1.11 2000-03-06 23:39:19 bird Exp $

#
# Odin32 API
#
#       wnetap32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = netapi32rsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\wnetap32.obj \
$(OBJDIR)\netapi32rsrc.obj \
$(PDWIN32_LIB)/dllentry.obj


# Target and original target names - names of the dll without extention and path
TARGET      = wnetap32
ORGTARGET   = netapi32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(ORGTARGET).lib $(PDWIN32_LIB)\$(ORGTARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(ORGTARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/user32.lib
$(PDWIN32_LIB)/odincrt.lib
OS2386.LIB $(RTLLIB_O)
$(ORGTARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

