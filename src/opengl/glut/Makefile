# $Id: Makefile,v 1.9 2000-03-04 19:55:35 bird Exp $

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


CFLAGS   = -Tdp -I..\mesa\GL -I..\glu $(CFLAGS) -I$(PDWIN32_INCLUDE) -I..\mesa
CXXFLAGS = -Tdp -I..\mesa\GL -I..\glu $(CXXFLAGS) -I$(PDWIN32_INCLUDE) -I..\mesa

CFLAGS=$(CFLAGS) -O-

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

all: $(TARGET).DLL

$(TARGET).DLL: $(OBJS) $(TARGET).def $(TARGET).lrf
    $(LD2) $(LD2FLAGS) /FREE @$(TARGET).lrf
    $(CP) $@ $(PDWIN32_BIN)


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


$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: $(TARGET).rc
    $(RC) $(RCFLAGS) -o resource.asm $(TARGET).rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win -I..\mesa\GL \
        *.c *.cpp *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.lrf *exp.def resource.asm \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib
