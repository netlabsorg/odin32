# $Id: Makefile,v 1.5 2000-02-09 14:46:26 sandervl Exp $
#
# PD-Win32 API
#
#       glut32.dll makefile
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin
PDWIN32_TOOLS = ..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


!ifdef DEBUG
!ifdef NODEBUGINFO
L=/NODEBUG
!else
L=/DE
!endif
!endif

CFLAGS = -Tdp -I..\mesa -I..\glu $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = -Tdp -I..\mesa -I..\glu $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

TARGET=GLUT32

all: $(TARGET).DLL $(TARGET).LIB

OBJS = glut_8x13.obj glut_9x15.obj glut_bitmap.obj glut_bwidth.obj glut_cindex.obj \
       glut_cmap.obj glut_cursor.obj glut_dials.obj glut_dstr.obj glut_event.obj \
       glut_ext.obj glut_fullscrn.obj glut_gamemode.obj glut_get.obj glut_hel10.obj \
       glut_hel12.obj glut_hel18.obj glut_init.obj glut_input.obj glut_joy.obj glut_key.obj \
       glut_keyctrl.obj glut_keyup.obj glut_mesa.obj glut_modifier.obj glut_mroman.obj \
       glut_overlay.obj glut_roman.obj glut_shapes.obj glut_space.obj glut_stroke.obj \
       glut_swap.obj glut_swidth.obj glut_tablet.obj glut_teapot.obj glut_tr10.obj \
       glut_tr24.obj glut_util.obj glut_vidresize.obj glut_warp.obj glut_win.obj \
       glut_winmisc.obj win32_glx.obj win32_menu.obj win32_util.obj win32_winproc.obj \
       win32_x11.obj glut_term.obj initterm.obj resource.obj

all     : $(TARGET).DLL

$(TARGET).DLL: $(OBJS) $(TARGET).def $(TARGET).lrf
        ILINK /FREE /NOE $(L) @$(TARGET).lrf
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc

resource.obj: resource.asm

initterm.obj: initterm.cpp
win32_glx.obj: win32_glx.c os2_glx.h
win32_x11.obj: win32_x11.c os2_x11.h
win32_menu.obj: win32_menu.c glutint.h glut.h
win32_util.obj: win32_util.c glutint.h
win32_winproc.obj: win32_winproc.c glutint.h

glut_mroman.obj: glut_mroman.c glutint.h
glut_roman.obj: glut_roman.c glutint.h
glut_hel12.obj: glut_hel12.c glutint.h
glut_8x13.obj: glut_8x13.c glutint.h
glut_hel18.obj: glut_hel18.c glutint.h
glut_9x15.obj: glut_9x15.c glutint.h
glut_tr10.obj: glut_tr10.c glutint.h
glut_hel10.obj: glut_hel10.c glutint.h
glut_tr24.obj: glut_tr24.c glutint.h

glut_bitmap.obj: glut_bitmap.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_bwidth.obj: glut_bwidth.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_cindex.obj: glut_cindex.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_cmap.obj: glut_cmap.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_cursor.obj: glut_cursor.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_dials.obj: glut_dials.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_dstr.obj: glut_dstr.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_event.obj: glut_event.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_ext.obj: glut_ext.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_fullscrn.obj: glut_fullscrn.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_gamemode.obj: glut_gamemode.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_get.obj: glut_get.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_init.obj: glut_init.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_input.obj: glut_input.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_joy.obj: glut_joy.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_key.obj: glut_key.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_keyctrl.obj: glut_keyctrl.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_keyup.obj: glut_keyup.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_mesa.obj: glut_mesa.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_modifier.obj: glut_modifier.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_overlay.obj: glut_overlay.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_shapes.obj: glut_shapes.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_space.obj: glut_space.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_swap.obj: glut_swap.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_swidth.obj: glut_swidth.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_tablet.obj: glut_tablet.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_teapot.obj: glut_teapot.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_util.obj: glut_util.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_vidresize.obj: glut_vidresize.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_warp.obj: glut_warp.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h
glut_win.obj: glut_win.c glutint.h glut.h glutos2.h
glut_winmisc.obj: glut_winmisc.c glutint.h glut.h glutos2.h os2_glx.h os2_x11.h

$(TARGET).lrf: makefile
   @echo Creating file <<$(@B).lrf
$(OBJS)
/OUT:$(TARGET).DLL /DLL
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

clean:
        $(RM) *.obj *.lib *.dll *.map *.lrf *exp.def resource.asm
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
