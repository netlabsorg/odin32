# $Id: Makefile,v 1.11 2000-03-06 23:33:11 bird Exp $

#
# Odin32 API
#
#       glut32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = 32rsrc.asm

CFLAGS      = -Tdp -I..\mesa\GL -I..\glu $(CFLAGS) -I..\mesa
CXXFLAGS    = -Tdp -I..\mesa\GL -I..\glu $(CXXFLAGS) -I..\mesa


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\glut_8x13.obj \
$(OBJDIR)\glut_9x15.obj \
$(OBJDIR)\glut_bitmap.obj \
$(OBJDIR)\glut_bwidth.obj \
$(OBJDIR)\glut_cindex.obj \
$(OBJDIR)\glut_cmap.obj \
$(OBJDIR)\glut_cursor.obj \
$(OBJDIR)\glut_dials.obj \
$(OBJDIR)\glut_dstr.obj \
$(OBJDIR)\glut_event.obj \
$(OBJDIR)\glut_ext.obj \
$(OBJDIR)\glut_fullscrn.obj \
$(OBJDIR)\glut_gamemode.obj \
$(OBJDIR)\glut_get.obj \
$(OBJDIR)\glut_hel10.obj \
$(OBJDIR)\glut_hel12.obj \
$(OBJDIR)\glut_hel18.obj \
$(OBJDIR)\glut_init.obj \
$(OBJDIR)\glut_input.obj \
$(OBJDIR)\glut_joy.obj \
$(OBJDIR)\glut_key.obj \
$(OBJDIR)\glut_keyctrl.obj \
$(OBJDIR)\glut_keyup.obj \
$(OBJDIR)\glut_mesa.obj \
$(OBJDIR)\glut_modifier.obj \
$(OBJDIR)\glut_mroman.obj \
$(OBJDIR)\glut_overlay.obj \
$(OBJDIR)\glut_roman.obj \
$(OBJDIR)\glut_shapes.obj \
$(OBJDIR)\glut_space.obj \
$(OBJDIR)\glut_stroke.obj \
$(OBJDIR)\glut_swap.obj \
$(OBJDIR)\glut_swidth.obj \
$(OBJDIR)\glut_tablet.obj \
$(OBJDIR)\glut_teapot.obj \
$(OBJDIR)\glut_tr10.obj \
$(OBJDIR)\glut_tr24.obj \
$(OBJDIR)\glut_util.obj \
$(OBJDIR)\glut_vidresize.obj \
$(OBJDIR)\glut_warp.obj \
$(OBJDIR)\glut_win.obj \
$(OBJDIR)\glut_winmisc.obj \
$(OBJDIR)\win32_glx.obj \
$(OBJDIR)\win32_menu.obj \
$(OBJDIR)\win32_util.obj \
$(OBJDIR)\win32_winproc.obj \
$(OBJDIR)\win32_x11.obj \
$(OBJDIR)\glut_term.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\glut32rsrc.obj


# Target name - name of the dll without extention and path.
TARGET  = glut32


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
$(OBJS)
$(PDWIN32_LIB)\ODINCRT.LIB
$(RTLLIB_O)
$(PDWIN32_LIB)\WINMM.LIB
$(PDWIN32_LIB)\KERNEL32.LIB
$(PDWIN32_LIB)\GDI32.LIB
$(PDWIN32_LIB)\USER32.LIB
$(PDWIN32_LIB)\GLU32.LIB
$(PDWIN32_LIB)\OPENGL32.LIB
OS2386.LIB
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\mesa\GL \
        *.c *.cpp *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

