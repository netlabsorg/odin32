# $Id: makefile,v 1.25 2000-03-23 17:14:45 cbratschi Exp $

#
# Odin32 API
#
#       comctl32.dll makefile
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


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\comctl32.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\progress.obj \
$(OBJDIR)\comctl32undoc.obj \
$(OBJDIR)\imagelist.obj \
$(OBJDIR)\comboex.obj \
$(OBJDIR)\animate.obj \
$(OBJDIR)\datetime.obj \
$(OBJDIR)\flatsb.obj \
$(OBJDIR)\hotkey.obj \
$(OBJDIR)\monthcal.obj \
$(OBJDIR)\nativefont.obj \
$(OBJDIR)\pager.obj \
$(OBJDIR)\tab.obj \
$(OBJDIR)\status.obj \
$(OBJDIR)\header.obj \
$(OBJDIR)\updown.obj \
$(OBJDIR)\rebar.obj \
$(OBJDIR)\trackbar.obj \
$(OBJDIR)\tooltips.obj \
$(OBJDIR)\toolbar.obj \
$(OBJDIR)\treeview.obj \
$(OBJDIR)\propsheet.obj \
$(OBJDIR)\listview.obj \
$(OBJDIR)\draglist.obj \
$(OBJDIR)\ipaddress.obj \
$(OBJDIR)\ccbase.obj \
$(OBJDIR)\rsrc.obj


# Target name - name of the dll without extention and path.
TARGET = comctl32


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
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/gdi32.lib
$(PDWIN32_LIB)/user32.lib
$(PDWIN32_LIB)/odincrt.lib
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

