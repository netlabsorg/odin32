# $Id: Makefile,v 1.67 2000-03-04 23:47:19 sandervl Exp $

#
# PD-Win32 API
#
#       user32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\bin
PDWIN32_TOOLS   = ..\..\tools\bin

#SvL: Remove this when everything has been converted to the new build rules
NEWBUILD=1

!include $(PDWIN32_INCLUDE)/pdwin32.mk

RESOURCES= resource
CFLAGS   = $(CFLAGS)   -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

TARGET = user32

OBJS = \
$(OBJDIR)\user32.obj \
$(OBJDIR)\loadres.obj \
$(OBJDIR)\dde.obj \
$(OBJDIR)\win32wndhandle.obj \
$(OBJDIR)\wsprintf.obj \
$(OBJDIR)\winmouse.obj \
$(OBJDIR)\icon.obj \
$(OBJDIR)\hook.obj \
$(OBJDIR)\winkeyboard.obj \
$(OBJDIR)\defwndproc.obj \
$(OBJDIR)\syscolor.obj \
$(OBJDIR)\char.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\uitools.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\spy.obj \
$(OBJDIR)\wndmsg.obj \
$(OBJDIR)\display.obj \
$(OBJDIR)\pmwindow.obj \
$(OBJDIR)\pmframe.obj \
$(OBJDIR)\win32class.obj \
$(OBJDIR)\win32wnd.obj \
$(OBJDIR)\win32dlg.obj \
$(OBJDIR)\msgbox.obj \
$(OBJDIR)\window.obj \
$(OBJDIR)\windowmsg.obj \
$(OBJDIR)\windowclass.obj \
$(OBJDIR)\win32wbase.obj \
$(OBJDIR)\windowword.obj \
$(OBJDIR)\gen_object.obj \
$(OBJDIR)\oslibwin.obj \
$(OBJDIR)\win32wndchild.obj \
$(OBJDIR)\controls.obj \
$(OBJDIR)\button.obj \
$(OBJDIR)\static.obj \
$(OBJDIR)\scroll.obj \
$(OBJDIR)\listbox.obj \
$(OBJDIR)\menu.obj \
$(OBJDIR)\combo.obj \
$(OBJDIR)\edit.obj \
$(OBJDIR)\winswitch.obj \
$(OBJDIR)\icontitle.obj \
$(OBJDIR)\clipboard.obj \
$(OBJDIR)\winicon.obj \
$(OBJDIR)\win32wmdiclient.obj \
$(OBJDIR)\oslibutil.obj \
$(OBJDIR)\oslibmsg.obj \
$(OBJDIR)\dib.obj \
$(OBJDIR)\oslibdos.obj \
$(OBJDIR)\win32wmdichild.obj \
$(OBJDIR)\winprop.obj \
$(OBJDIR)\wingdi.obj \
$(OBJDIR)\oslibgdi.obj \
$(OBJDIR)\winaccel.obj \
$(OBJDIR)\win32wbasepos.obj \
$(OBJDIR)\oslibres.obj \
$(OBJDIR)\dc.obj \
$(OBJDIR)\timer.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\caret.obj \
$(OBJDIR)\$(RESOURCES).obj \
$(OBJDIR)\winproc.obj \
$(OBJDIR)\text.obj \
$(OBJDIR)\oslibmsgtranslate.obj \
$(OBJDIR)\windlgmsg.obj \
$(OBJDIR)\windlg.obj \
$(OBJDIR)\win32wdesktop.obj \
$(OBJDIR)\win32wbasenonclient.obj

all: $(OBJDIR) $(TARGET).dll $(TARGET).lib

$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
        $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf
        $(CP) $@ $(PDWIN32_BIN)

$(OBJDIR)\$(TARGET).lrf: makefile
        @echo Creating file <<$(OBJDIR)\$(@B).lrf
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


$(RESOURCES).asm: user32.rc
    $(RC) $(RCFLAGS) -o $(RESOURCES).asm user32.rc

dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!include $(PDWIN32_INCLUDE)/pdwin32.post

