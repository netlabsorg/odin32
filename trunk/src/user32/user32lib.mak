# $Id: user32lib.mak,v 1.4 2001-09-24 18:43:28 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1
WRC_PREFIX_RESOURCE=1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
!ifndef WAT
CDEFINES = $(CDEFINES) -DINVERT -DCLIENTFRAME
!else
CDEFINES += -DINVERT -DCLIENTFRAME
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dc.obj \
$(OBJDIR)\dcrgn.obj \
$(OBJDIR)\dcscroll.obj \
$(OBJDIR)\user32.obj \
$(OBJDIR)\dde.obj \
$(OBJDIR)\wsprintf.obj \
$(OBJDIR)\winkeyboard.obj \
$(OBJDIR)\winmouse.obj \
$(OBJDIR)\winevent.obj \
$(OBJDIR)\defwndproc.obj \
$(OBJDIR)\syscolor.obj \
$(OBJDIR)\char.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\inituser32.obj \
$(OBJDIR)\uitools.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\spy.obj \
$(OBJDIR)\wndmsg.obj \
$(OBJDIR)\display.obj \
$(OBJDIR)\pmwindow.obj \
$(OBJDIR)\oslibmsgtranslate.obj \
$(OBJDIR)\msgbox.obj \
$(OBJDIR)\window.obj \
$(OBJDIR)\gen_object.obj \
$(OBJDIR)\win32wndchild.obj \
$(OBJDIR)\win32wbase.obj \
$(OBJDIR)\win32wbasenonclient.obj \
$(OBJDIR)\win32wbasepos.obj \
$(OBJDIR)\win32wbaseprop.obj \
$(OBJDIR)\win32wndhandle.obj \
$(OBJDIR)\win32wmisc.obj \
$(OBJDIR)\windowword.obj \
$(OBJDIR)\windowmsg.obj \
$(OBJDIR)\winproc.obj \
$(OBJDIR)\oslibmsg.obj \
$(OBJDIR)\hook.obj \
$(OBJDIR)\win32wmdiclient.obj \
$(OBJDIR)\mdi.obj \
$(OBJDIR)\win32wdesktop.obj \
$(OBJDIR)\win32dlg.obj \
$(OBJDIR)\windlgmsg.obj \
$(OBJDIR)\windlg.obj \
$(OBJDIR)\win32class.obj \
$(OBJDIR)\windowclass.obj \
$(OBJDIR)\oslibwin.obj \
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
$(OBJDIR)\oslibutil.obj \
$(OBJDIR)\dib.obj \
$(OBJDIR)\oslibdos.obj \
$(OBJDIR)\oslibgdi.obj \
$(OBJDIR)\oslibres.obj \
$(OBJDIR)\loadres.obj \
$(OBJDIR)\winaccel.obj \
$(OBJDIR)\icon.obj \
$(OBJDIR)\winicon.obj \
$(OBJDIR)\timer.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\caret.obj \
$(OBJDIR)\text.obj \
$(OBJDIR)\oldnls32.obj \
$(OBJDIR)\user32rsrc.obj \
$(OBJDIR)\oslibprf.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = user32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

