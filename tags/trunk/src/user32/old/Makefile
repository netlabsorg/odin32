# $Id: Makefile,v 1.4 1999-09-25 19:07:32 sandervl Exp $

#
# PD-Win32 API
#
#       user32.dll makefile
#

PDWIN32_INCLUDE = ..\..\..\include\old
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = .
PDWIN32_TOOLS = ..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\..\win
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\..\win


TARGET = user32

OBJS =  user32.obj resstring.obj dialog.obj loadres.obj \
        wndproc.obj wndclass.obj dde.obj \
        dlgconvert.obj icon.obj hook.obj hooks.obj menu.obj \
        usrcall.obj defwndproc.obj syscolor.obj char.obj initterm.obj \
        uitools.obj unknown.obj spy.obj setwindow.obj wndmsg.obj


all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/gdi32.lib \
              ..\..\kernel32\old\kernel32.lib $(PDWIN32_LIB)/libuls.lib \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

user32.obj: user32.cpp $(PDWIN32_INCLUDE)\wndproc.h \
            $(PDWIN32_INCLUDE)\wndclass.h $(PDWIN32_INCLUDE)\winicon.h syscolor.h
resstring.obj: resstring.cpp resstring.h
char.obj: char.cpp user32.h
loadres.obj: loadres.cpp user32.h
icon.obj: icon.cpp $(PDWIN32_INCLUDE)\winicon.h
menu.obj: menu.cpp user32.h
syscolor.obj: syscolor.cpp syscolor.h
usrcall.obj: usrcall.cpp usrcall.h
dialog.obj: dialog.cpp \
            $(PDWIN32_INCLUDE)\wndproc.h $(PDWIN32_INCLUDE)\wndclass.h \
            dlgconvert.h
dde.obj: dde.cpp
hook.obj: hook.cpp hooks.h
defwndproc.obj: defwndproc.cpp syscolor.h $(PDWIN32_INCLUDE)\wndproc.h
hooks.obj: hooks.cpp hooks.h
dlgconvert.obj: dlgconvert.cpp \
                $(PDWIN32_INCLUDE)\wndproc.h $(PDWIN32_INCLUDE)\wndclass.h \
                dlgconvert.h
wndproc.obj: wndproc.cpp \
             $(PDWIN32_INCLUDE)\wndproc.h $(PDWIN32_INCLUDE)\wndclass.h \
             dlgconvert.h hooks.h
wndclass.obj: wndclass.cpp \
              $(PDWIN32_INCLUDE)\wndproc.h $(PDWIN32_INCLUDE)\wndclass.h  wndmsg.h
initterm.obj: initterm.cpp $(PDWIN32_INCLUDE)\spy.h
uitools.obj: uitools.cpp
unknown.obj: unknown.cpp
setwindow.obj: setwindow.cpp $(PDWIN32_INCLUDE)\wndproc.h $(PDWIN32_INCLUDE)\wndclass.h  wndmsg.h
wndmsg.obj: wndmsg.cpp wndmsg.h

spy.obj: spy.cpp $(PDWIN32_INCLUDE)\spy.h


clean:
        $(RM) *.obj *.lib *.dll *~ *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib
        $(RM) $(TARGET)exp.def
