# $Id: makefile,v 1.15 1999-07-20 20:24:54 cbratschi Exp $

#
# PD-Win32 API
#
#       user32.dll makefile
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


TARGET = user32

OBJS =  user32.obj resstring.obj dialog.obj loadres.obj \
        dde.obj \
        icon.obj hook.obj hooks.obj winmenu.obj \
        usrcall.obj defwndproc.obj syscolor.obj char.obj initterm.obj \
        uitools.obj unknown.obj spy.obj wndmsg.obj \
        pmwindow.obj win32class.obj win32wnd.obj win32dlg.obj \
        msgbox.obj window.obj windowmsg.obj windowclass.obj windlgmsg.obj \
        windowword.obj gen_object.obj oslibwin.obj win32wndchild.obj \
        controls.obj button.obj static.obj \
        oslibutil.obj oslibmsg.obj windlg.obj \
        winprop.obj wingdi.obj oslibgdi.obj winaccel.obj winscrollbar.obj \
        nativerc.obj oslibres.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def $(TARGET).res
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/gdi32.lib $(PDWIN32_LIB)/kernel32.lib $(PDWIN32_LIB)/libuls.lib
        rc $(TARGET).res $(TARGET).dll
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).res:
        rc -r user32.rc user32.res

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)


user32.obj: user32.cpp $(PDWIN32_INCLUDE)\wndproc.h \
            icon.h syscolor.h
resstring.obj: resstring.cpp resstring.h
char.obj: char.cpp user32.h
loadres.obj: loadres.cpp user32.h
icon.obj: icon.cpp icon.h
winmenu.obj: winmenu.cpp user32.h $(PDWIN32_INCLUDE)\winres.h
syscolor.obj: syscolor.cpp syscolor.h
usrcall.obj: usrcall.cpp usrcall.h
dialog.obj: dialog.cpp
dde.obj: dde.cpp
hook.obj: hook.cpp hooks.h
defwndproc.obj: defwndproc.cpp syscolor.h
hooks.obj: hooks.cpp hooks.h
initterm.obj: initterm.cpp $(PDWIN32_INCLUDE)\spy.h pmwindow.h
uitools.obj: uitools.cpp
unknown.obj: unknown.cpp
wndmsg.obj: wndmsg.cpp wndmsg.h

spy.obj: spy.cpp $(PDWIN32_INCLUDE)\spy.h

controls.obj: controls.cpp $(PDWIN32_INCLUDE)\win\controls.h
button.obj: button.cpp $(PDWIN32_INCLUDE)\win\button.h
static.obj: static.cpp $(PDWIN32_INCLUDE)\win\static.h

nativerc.obj: nativerc.cpp

msgbox.obj:    msgbox.cpp
window.obj:    window.cpp win32class.h win32wnd.h win32wndchild.h
windowmsg.obj: windowmsg.cpp win32class.h win32wnd.h win32wndchild.h
windlg.obj:    windlg.cpp win32wnd.h win32dlg.h
winprop.obj:   winprop.cpp
wingdi.obj:    wingdi.cpp win32wnd.h oslibgdi.h
windlgmsg.obj: windlgmsg.cpp win32class.h win32wnd.h win32dlg.h win32wndchild.h
windowclass.obj: windowclass.cpp win32class.h win32wnd.h win32wndchild.h
windowword.obj:  windowword.cpp win32class.h win32wnd.h win32dlg.h win32wndchild.h
winaccel.obj:     winaccel.cpp  $(PDWIN32_INCLUDE)\winres.h
winscrollbar.obj: winscrollbar.cpp

pmwindow.obj:   pmwindow.cpp win32class.h win32wnd.h win32dlg.h pmwindow.h win32wndchild.h $(PDWIN32_INCLUDE)\wprocess.h  oslibgdi.h
win32class.obj: win32class.cpp win32class.h win32wnd.h win32dlg.h gen_object.h oslibwin.h win32wndchild.h
win32wnd.obj:   win32wnd.cpp win32class.h win32wnd.h win32dlg.h gen_object.h oslibwin.h win32wndchild.h  $(PDWIN32_INCLUDE)\winres.h oslibres.h
win32dlg.obj:   win32dlg.cpp win32class.h win32wnd.h win32dlg.h gen_object.h oslibwin.h win32wndchild.h
win32wndchild.obj: win32wndchild.cpp win32wndchild.h
gen_object.obj: gen_object.cpp gen_object.h
oslibwin.obj:   oslibwin.cpp oslibwin.h oslibutil.h oslibgdi.h
oslibutil.obj:  oslibutil.cpp oslibutil.h $(PDWIN32_INCLUDE)\wprocess.h oslibmsg.h
oslibmsg.obj:   oslibmsg.cpp oslibmsg.h
oslibgdi.obj:   oslibgdi.cpp oslibgdi.h
oslibres.obj:   oslibres.cpp oslibwin.h oslibres.h

clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
