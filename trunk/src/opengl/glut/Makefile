# $Id: Makefile,v 1.17 2000-12-02 23:58:44 bird Exp $

#
# Odin32 API
#
#       glut32.dll makefile
#


#
# Tell buildenvironmet to invoke additional dep rule.
#
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Overrides
#
CINCLUDES   = -I..\mesa\GL -I..\glu $(CINCLUDES) -I..\mesa
CDEFINES    = $(CDEFINES_WIN32APP)
CFLAGS      = $(CFLAGS_WIN32APP)
CXXFLAGS    = $(CXXFLAGS_WIN32APP)


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
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


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(RTLLIB_O) \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/glu32.lib \
$(ODIN32_LIB)/opengl32.lib \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = glut32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dep rule - adds dependencies for some additional headers.
#
mydep:
    $(DEPEND) -a $(CINCLUDES) \
        *.c *.cpp *.asm *.inc $(ODIN32_INCLUDE)\*.h ..\mesa\gl\*.h

