# $Id: Makefile,v 1.21 1999-09-23 10:33:59 sandervl Exp $

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


TARGET = user32

OBJS =  user32.obj loadres.obj \
        dde.obj heapshared.obj win32wndhandle.obj \
        icon.obj hook.obj hooks.obj winmenu.obj \
        defwndproc.obj syscolor.obj char.obj initterm.obj \
        uitools.obj unknown.obj spy.obj wndmsg.obj \
        pmwindow.obj win32class.obj win32wnd.obj win32dlg.obj \
        msgbox.obj window.obj windowmsg.obj windowclass.obj win32wbase.obj \
        windowword.obj gen_object.obj oslibwin.obj win32wndchild.obj \
        controls.obj button.obj static.obj scroll.obj listbox.obj \
        combo.obj edit.obj clipboard.obj winicon.obj win32wmdiclient.obj \
        oslibutil.obj oslibmsg.obj dib.obj oslibdos.obj win32wmdichild.obj \
        winprop.obj wingdi.obj oslibgdi.obj winaccel.obj winscrollbar.obj \
        nativerc.obj oslibres.obj dummy.obj oslibmenu.obj dc.obj\
        windlgmsg.obj windlg.obj win32wdesktop.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def $(TARGET).res
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/gdi32.lib $(PDWIN32_LIB)/kernel32.lib $(PDWIN32_LIB)/libuls.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        rc $(TARGET).res $(TARGET).dll
        $(CP) $@ $(PDWIN32_BIN)

$(TARGET).res:
        rc -r user32.rc user32.res

$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

#SvL: Does not work correctly for GetOS2Callback__15Win32WindowProcFv
#$(TARGET)exp.def: $(TARGET).def
#        $(IMPDEF) $** $@


user32.obj: user32.cpp $(PDWIN32_INCLUDE)\wndproc.h \
            $(PDWIN32_INCLUDE)\winicon.h syscolor.h
dummy.obj: dummy.cpp
char.obj: char.cpp user32.h
loadres.obj: loadres.cpp user32.h dib.h
icon.obj: icon.cpp $(PDWIN32_INCLUDE)\winicon.h
winmenu.obj: winmenu.cpp user32.h $(PDWIN32_INCLUDE)\winres.h $(PDWIN32_INCLUDE)\winresmenu.h win32wbase.h
syscolor.obj: syscolor.cpp syscolor.h
dde.obj: dde.cpp
hook.obj: hook.cpp hooks.h
defwndproc.obj: defwndproc.cpp syscolor.h win32wmdiclient.h win32wbase.h win32wnd.h win32dlg.h
hooks.obj: hooks.cpp hooks.h
initterm.obj: initterm.cpp $(PDWIN32_INCLUDE)\spy.h pmwindow.h heapshared.h
uitools.obj: uitools.cpp
unknown.obj: unknown.cpp
wndmsg.obj: wndmsg.cpp wndmsg.h

spy.obj: spy.cpp $(PDWIN32_INCLUDE)\spy.h

controls.obj: controls.cpp $(PDWIN32_INCLUDE)\win\controls.h win32wmdiclient.h win32dlg.h win32wdesktop.h
button.obj: button.cpp $(PDWIN32_INCLUDE)\win\button.h
static.obj: static.cpp $(PDWIN32_INCLUDE)\win\static.h
scroll.obj: scroll.cpp $(PDWIN32_INCLUDE)\win\scroll.h
listbox.obj: listbox.cpp $(PDWIN32_INCLUDE)\win\combo.h
combo.obj: combo.cpp $(PDWIN32_INCLUDE)\win\combo.h
edit.obj: edit.cpp $(PDWIN32_INCLUDE)\win\combo.h

nativerc.obj: nativerc.cpp

msgbox.obj:    msgbox.cpp
window.obj:    window.cpp win32class.h win32wbase.h win32wndchild.h oslibwin.h win32wmdiclient.h win32wdesktop.h
windowmsg.obj: windowmsg.cpp win32class.h win32wbase.h win32wndchild.h oslibmsg.h
winprop.obj:   winprop.cpp
wingdi.obj:    wingdi.cpp win32wbase.h oslibgdi.h
windowclass.obj: windowclass.cpp win32class.h win32wbase.h win32wndchild.h
windowword.obj:  windowword.cpp win32class.h win32wbase.h win32dlg.h win32wndchild.h
winaccel.obj:     winaccel.cpp  $(PDWIN32_INCLUDE)\winres.h win32wbase.h
winscrollbar.obj: winscrollbar.cpp win32wbase.h
winicon.obj:    winicon.cpp $(PDWIN32_INCLUDE)\winicon.h win32wbase.h
windlg.obj:    windlg.cpp win32wbase.h win32dlg.h
windlgmsg.obj: windlgmsg.cpp win32class.h win32wbase.h win32dlg.h win32wndchild.h
win32wndhandle.obj: win32wndhandle.cpp win32wndhandle.h

pmwindow.obj:   pmwindow.cpp win32class.h win32wbase.h win32dlg.h pmwindow.h win32wndchild.h $(PDWIN32_INCLUDE)\wprocess.h oslibgdi.h oslibwin.h dc.h
win32class.obj: win32class.cpp win32class.h win32wbase.h win32dlg.h gen_object.h heapshared.h oslibwin.h win32wndchild.h
win32wbase.obj:   win32wbase.cpp win32class.h win32wbase.h win32dlg.h gen_object.h heapshared.h oslibwin.h win32wndchild.h  $(PDWIN32_INCLUDE)\winres.h oslibres.h win32wndhandle.h oslibdos.h dc.h
win32wnd.obj:   win32wnd.cpp win32class.h win32wbase.h win32wnd.h win32dlg.h gen_object.h heapshared.h oslibwin.h win32wndchild.h  $(PDWIN32_INCLUDE)\winres.h oslibres.h win32wndhandle.h oslibdos.h oslibmenu.h
win32dlg.obj:   win32dlg.cpp win32class.h win32wbase.h win32dlg.h gen_object.h heapshared.h oslibwin.h win32wndchild.h oslibmsg.h 
win32wndchild.obj: win32wndchild.cpp win32wndchild.h gen_object.h 
win32wmdiclient.obj: win32wmdiclient.cpp win32wbase.h win32wmdiclient.h win32wmdichild.h gen_object.h 
win32wmdichild.obj: win32wmdichild.cpp win32wbase.h win32wmdiclient.h win32wmdichild.h gen_object.h 
win32wdesktop.obj: win32wdesktop.cpp win32wbase.h win32wdesktop.h gen_object.h 
gen_object.obj: gen_object.cpp gen_object.h heapshared.h
oslibwin.obj:   oslibwin.cpp oslibwin.h oslibutil.h oslibgdi.h $(PDWIN32_INCLUDE)\winconst.h
oslibutil.obj:  oslibutil.cpp oslibutil.h $(PDWIN32_INCLUDE)\wprocess.h oslibmsg.h
oslibmsg.obj:   oslibmsg.cpp oslibmsg.h
oslibmenu.obj:  oslibmenu.cpp oslibmenu.h
oslibgdi.obj:   oslibgdi.cpp oslibgdi.h win32wbase.h
oslibres.obj:   oslibres.cpp oslibwin.h oslibres.h $(PDWIN32_INCLUDE)\winconst.h
oslibdos.obj:   oslibdos.cpp oslibdos.h
dc.obj:         dc.cpp win32wbase.h dc.h

clipboard.obj:  clipboard.cpp
dib.obj:        dib.cpp dib.h

heapshared.obj: heapshared.cpp heapshared.h

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch *.res
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
#        $(RM) $(TARGET)exp.def

