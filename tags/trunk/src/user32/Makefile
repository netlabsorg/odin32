# $Id: Makefile,v 1.82 2001-04-12 14:04:32 sandervl Exp $

#
# Odin32 API
#
#       user32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
!ifndef WAT
CDEFINES = $(CDEFINES) -DINVERT -DODIN_HITTEST
!else
CDEFINES += -DINVERT -DODIN_HITTEST
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dc.obj \
$(OBJDIR)\dcrgn.obj \
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
$(OBJDIR)\oslibgdi.obj \
$(OBJDIR)\winaccel.obj \
$(OBJDIR)\win32wbasepos.obj \
$(OBJDIR)\win32wbaseprop.obj \
$(OBJDIR)\oslibres.obj \
$(OBJDIR)\timer.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\caret.obj \
$(OBJDIR)\winproc.obj \
$(OBJDIR)\text.obj \
$(OBJDIR)\oslibmsgtranslate.obj \
$(OBJDIR)\windlgmsg.obj \
$(OBJDIR)\windlg.obj \
$(OBJDIR)\win32wdesktop.obj \
$(OBJDIR)\win32wbasenonclient.obj \
$(OBJDIR)\oldnls32.obj \
$(OBJDIR)\user32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/wgss50.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/libuls.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = user32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

