# $Id: Makefile,v 1.64 2000-02-09 23:47:03 bird Exp $

#
# PD-Win32 API
#
#       user32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

!ifdef DEBUGALL
CFLAGS = $(CFLAGS) -DDEBUG_ENABLELOG_LEVEL2
CXXFLAGS = $(CXXFLAGS) -DDEBUG_ENABLELOG_LEVEL2
!endif

TARGET = user32

OBJS =  user32.obj loadres.obj \
        dde.obj win32wndhandle.obj wsprintf.obj winmouse.obj \
        icon.obj hook.obj winkeyboard.obj \
        defwndproc.obj syscolor.obj char.obj initterm.obj \
        uitools.obj unknown.obj spy.obj wndmsg.obj display.obj \
        pmwindow.obj pmframe.obj win32class.obj win32wnd.obj win32dlg.obj \
        msgbox.obj window.obj windowmsg.obj windowclass.obj win32wbase.obj \
        windowword.obj gen_object.obj oslibwin.obj win32wndchild.obj \
        controls.obj button.obj static.obj scroll.obj listbox.obj menu.obj\
        combo.obj edit.obj winswitch.obj icontitle.obj clipboard.obj winicon.obj \
        win32wmdiclient.obj oslibutil.obj oslibmsg.obj dib.obj oslibdos.obj \
        win32wmdichild.obj winprop.obj wingdi.obj oslibgdi.obj winaccel.obj \
        win32wbasepos.obj oslibres.obj dc.obj timer.obj \
        caret.obj resource.obj winproc.obj text.obj oslibmsgtranslate.obj \
        windlgmsg.obj windlg.obj win32wdesktop.obj win32wbasenonclient.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def $(TARGET).lrf
!ifdef DEBUG
!ifdef NODEBUGINFO
        ilink /nodeb /map /noe /NOD /NoLogo /NOFREE @$(TARGET).lrf
!else
        ilink /de /map  /noe /NOD /dbgpack /NoLogo /NOFREE @$(TARGET).lrf
!endif
!else
        ilink /map  /noe /NOD /NoLogo /FREE @$(TARGET).lrf
!endif
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).lrf: makefile
        @echo Creating file <<$(@B).lrf
$(OBJS)
$(PDWIN32_LIB)/pmwinx.lib
$(PDWIN32_LIB)/gdi32.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/libuls.lib
$(PDWIN32_LIB)/odincrt.lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: user32.rc
    $(RC) $(RCFLAGS) -o resource.asm user32.rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch *.res *.lrf resource.asm *.lrf \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
        $(TARGET)exp.def

