# $Id: Makefile,v 1.53 1999-12-29 12:39:43 sandervl Exp $

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

RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win

TARGET = user32

OBJS =  user32.obj loadres.obj \
        dde.obj win32wndhandle.obj wsprintf.obj winmouse.obj \
        icon.obj hook.obj winmenu.obj winkeyboard.obj \
        defwndproc.obj syscolor.obj char.obj initterm.obj \
        uitools.obj unknown.obj spy.obj wndmsg.obj display.obj \
        pmwindow.obj pmframe.obj win32class.obj win32wnd.obj win32dlg.obj \
        msgbox.obj window.obj windowmsg.obj windowclass.obj win32wbase.obj \
        windowword.obj gen_object.obj oslibwin.obj win32wndchild.obj \
        controls.obj button.obj static.obj scroll.obj listbox.obj \
        combo.obj edit.obj clipboard.obj winicon.obj win32wmdiclient.obj \
        oslibutil.obj oslibmsg.obj dib.obj oslibdos.obj win32wmdichild.obj \
        winprop.obj wingdi.obj oslibgdi.obj winaccel.obj win32wbasepos.obj \
        oslibres.obj dummy.obj oslibmenu.obj dc.obj timer.obj \
        caret.obj resource.obj winproc.obj text.obj oslibmsgtranslate.obj \
        windlgmsg.obj windlg.obj win32wdesktop.obj


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

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@


resource.asm: user32.rc
    $(RC) $(RCFLAGS) -o resource.asm user32.rc

resource.obj: resource.asm

user32.obj: user32.cpp $(PDWIN32_INCLUDE)\wndproc.h \
            $(PDWIN32_INCLUDE)\winicon.h syscolor.h pmwindow.h
dummy.obj: dummy.cpp
char.obj: char.cpp user32.h
loadres.obj: loadres.cpp user32.h dib.h
icon.obj: icon.cpp $(PDWIN32_INCLUDE)\winicon.h
winmenu.obj: winmenu.cpp user32.h $(PDWIN32_INCLUDE)\winres.h win32wbase.h winmenudef.h oslibwin.h oslibmenu.h
syscolor.obj: syscolor.cpp syscolor.h
dde.obj: dde.cpp
hook.obj: hook.cpp $(PDWIN32_INCLUDE)\win\hook.h
defwndproc.obj: defwndproc.cpp syscolor.h win32wmdiclient.h win32wbase.h win32wnd.h win32dlg.h
initterm.obj: initterm.cpp $(PDWIN32_INCLUDE)\spy.h pmwindow.h initterm.h
uitools.obj: uitools.cpp win32wbase.h
unknown.obj: unknown.cpp
wndmsg.obj: wndmsg.cpp wndmsg.h $(PDWIN32_INCLUDE)\spy.h

spy.obj: spy.cpp $(PDWIN32_INCLUDE)\spy.h

controls.obj: controls.cpp controls.h win32wmdiclient.h win32dlg.h win32wdesktop.h
button.obj: button.cpp $(PDWIN32_INCLUDE)\win\button.h initterm.h
static.obj: static.cpp $(PDWIN32_INCLUDE)\win\static.h initterm.h
scroll.obj: scroll.cpp $(PDWIN32_INCLUDE)\win\scroll.h win32wbase.h oslibwin.h
listbox.obj: listbox.cpp $(PDWIN32_INCLUDE)\win\combo.h
combo.obj: combo.cpp $(PDWIN32_INCLUDE)\win\combo.h initterm.h
edit.obj: edit.cpp $(PDWIN32_INCLUDE)\win\combo.h

msgbox.obj:    msgbox.cpp
window.obj:    window.cpp win32class.h win32wbase.h win32wndchild.h oslibwin.h win32wmdiclient.h win32wdesktop.h
windowmsg.obj: windowmsg.cpp win32class.h win32wbase.h win32wndchild.h oslibmsg.h
winprop.obj:   winprop.cpp win32wbase.h
wingdi.obj:    wingdi.cpp win32wbase.h oslibgdi.h
windowclass.obj: windowclass.cpp win32class.h win32wbase.h win32wndchild.h
windowword.obj:  windowword.cpp win32class.h win32wbase.h win32dlg.h win32wndchild.h
winaccel.obj:     winaccel.cpp  $(PDWIN32_INCLUDE)\winres.h win32wbase.h
winicon.obj:    winicon.cpp $(PDWIN32_INCLUDE)\winicon.h win32wbase.h
windlg.obj:    windlg.cpp win32wbase.h win32dlg.h
windlgmsg.obj: windlgmsg.cpp win32class.h win32wbase.h win32dlg.h win32wndchild.h
win32wndhandle.obj: win32wndhandle.cpp win32wndhandle.h

pmwindow.obj:   pmwindow.cpp win32class.h win32wbase.h win32dlg.h pmwindow.h win32wndchild.h $(PDWIN32_INCLUDE)\wprocess.h oslibgdi.h oslibwin.h dc.h $(PDWIN32_INCLUDE)\win\thread.h
pmframe.obj: pmframe.cpp win32class.h win32wbase.h pmframe.h win32wndchild.h
win32class.obj: win32class.cpp win32class.h win32wbase.h win32dlg.h gen_object.h $(PDWIN32_INCLUDE)\heapshared.h oslibwin.h win32wndchild.h $(PDWIN32_INCLUDE)\win\winproc.h
win32wbase.obj:   win32wbase.cpp win32class.h win32wbase.h win32dlg.h gen_object.h $(PDWIN32_INCLUDE)\heapshared.h oslibwin.h win32wndchild.h  $(PDWIN32_INCLUDE)\winres.h oslibres.h win32wndhandle.h oslibdos.h dc.h pmframe.h win32wdesktop.h controls.h winmouse.h $(PDWIN32_INCLUDE)\win\winproc.h $(PDWIN32_INCLUDE)\win\hook.h oslibmsg.h
win32wbasepos.obj: win32wbasepos.cpp win32class.h win32wbase.h win32dlg.h gen_object.h $(PDWIN32_INCLUDE)\heapshared.h oslibwin.h win32wndchild.h  $(PDWIN32_INCLUDE)\winres.h oslibres.h win32wndhandle.h oslibdos.h dc.h pmframe.h win32wdesktop.h
win32wnd.obj:   win32wnd.cpp win32class.h win32wbase.h win32wnd.h win32dlg.h gen_object.h $(PDWIN32_INCLUDE)\heapshared.h oslibwin.h win32wndchild.h  $(PDWIN32_INCLUDE)\winres.h oslibres.h win32wndhandle.h oslibdos.h oslibmenu.h
win32dlg.obj:   win32dlg.cpp win32class.h win32wbase.h win32dlg.h gen_object.h $(PDWIN32_INCLUDE)\heapshared.h oslibwin.h win32wndchild.h oslibmsg.h controls.h $(PDWIN32_INCLUDE)\win\winproc.h
win32wndchild.obj: win32wndchild.cpp win32wndchild.h gen_object.h
win32wmdiclient.obj: win32wmdiclient.cpp win32wbase.h win32wmdiclient.h win32wmdichild.h gen_object.h
win32wmdichild.obj: win32wmdichild.cpp win32wbase.h win32wmdiclient.h win32wmdichild.h gen_object.h
win32wdesktop.obj: win32wdesktop.cpp win32wbase.h win32wdesktop.h gen_object.h
gen_object.obj: gen_object.cpp gen_object.h $(PDWIN32_INCLUDE)\heapshared.h
oslibwin.obj:   oslibwin.cpp oslibwin.h oslibutil.h oslibgdi.h $(PDWIN32_INCLUDE)\winconst.h
oslibutil.obj:  oslibutil.cpp oslibutil.h $(PDWIN32_INCLUDE)\wprocess.h oslibmsg.h
oslibmsg.obj:   oslibmsg.cpp oslibmsg.h timer.h win32wbase.h win32wnd.h $(PDWIN32_INCLUDE)\win\thread.h
oslibmsgtranslate.obj: oslibmsgtranslate.cpp oslibmsg.h timer.h win32wbase.h win32wnd.h $(PDWIN32_INCLUDE)\win\thread.h
oslibmenu.obj:  oslibmenu.cpp oslibmenu.h
oslibgdi.obj:   oslibgdi.cpp oslibgdi.h win32wbase.h
oslibres.obj:   oslibres.cpp oslibwin.h oslibres.h $(PDWIN32_INCLUDE)\winconst.h
oslibdos.obj:   oslibdos.cpp oslibdos.h
dc.obj:         dc.cpp win32wbase.h dc.h dcdata.h
timer.obj:      timer.cpp win32wbase.h timer.h
caret.obj:      caret.cpp win32wbase.h caret.h dcdata.h
text.obj:       text.cpp

clipboard.obj:  clipboard.cpp win32wbase.h
dib.obj:        dib.cpp dib.h
wsprintf.obj:   wsprintf.cpp

winmouse.obj:   winmouse.cpp winmouse.h oslibmsg.h pmwindow.h win32wbase.h
winkeyboard.obj: winkeyboard.cpp $(PDWIN32_INCLUDE)\winkeyboard.h
display.obj: display.cpp pmwindow.h

winproc.obj: winproc.cpp $(PDWIN32_INCLUDE)\win\winproc.h $(PDWIN32_INCLUDE)\heapcode.h win32wbase.h

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch *.res *.lrf resource.asm *.lrf
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
        $(RM) $(TARGET)exp.def

