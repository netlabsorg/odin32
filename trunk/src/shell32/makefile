# $Id: makefile,v 1.25 2000-05-18 18:06:59 sandervl Exp $

#
# Odin32 API
#
#       shell32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin

# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk

# Flag overloads and local macros.
CLEANEXTRAS = shres.asm

CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS =  \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\brsfolder.obj \
$(OBJDIR)\classes.obj \
$(OBJDIR)\contmenu.obj \
$(OBJDIR)\dataobject.obj \
$(OBJDIR)\enumidlist.obj \
$(OBJDIR)\folders.obj \
$(OBJDIR)\iconcache.obj \
$(OBJDIR)\memorystream.obj \
$(OBJDIR)\pidl.obj \
$(OBJDIR)\regstream.obj \
$(OBJDIR)\shell32_main.obj \
$(OBJDIR)\shellguid.obj \
$(OBJDIR)\shelllink.obj \
$(OBJDIR)\shlmenu.obj \
$(OBJDIR)\shellpath.obj \
$(OBJDIR)\shellole.obj \
$(OBJDIR)\shellstub.obj \
$(OBJDIR)\shellord.obj \
$(OBJDIR)\shv_bg_cmenu.obj \
$(OBJDIR)\shlfolder.obj \
$(OBJDIR)\shlview.obj \
$(OBJDIR)\shell.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\she.obj \
$(OBJDIR)\sh.obj \
$(OBJDIR)\shpolicy.obj \
$(OBJDIR)\systray.obj \
$(OBJDIR)\shres.obj


# Target name - name of the dll without extention and path.
TARGET = shell32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    -4$(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/pmwinx.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/user32.lib
$(PDWIN32_LIB)/advapi32.lib
$(PDWIN32_LIB)/gdi32.lib
$(PDWIN32_LIB)/ole32.lib
$(PDWIN32_LIB)/version.lib
$(PDWIN32_LIB)/comctl32.lib
$(PDWIN32_LIB)/odincrt.lib
$(PDWIN32_LIB)/shlwapi.lib
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

