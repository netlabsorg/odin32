# $Id: Makefile,v 1.112 2004-05-26 08:45:07 sandervl Exp $

#
# Odin32 API
#
#       user32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!if "$(DEBUG)" == "1"
DEFFILE    = user32dbg.def
ORGDEFFILE = user32.def
!endif

WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides.
#
!ifndef WMAKE
CDEFINES  = $(CDEFINES) -DINVERT -DCLIENTFRAME
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
$(OBJDIR)\oslibkbd.obj \
$(OBJDIR)\winmouse.obj \
$(OBJDIR)\winevent.obj \
$(OBJDIR)\defwndproc.obj \
$(OBJDIR)\syscolor.obj \
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
$(OBJDIR)\win32wdesktop.obj \
$(OBJDIR)\win32wfake.obj \
$(OBJDIR)\windowword.obj \
$(OBJDIR)\windowmsg.obj \
$(OBJDIR)\callwrap.obj \
$(OBJDIR)\message.obj \
$(OBJDIR)\winproc.obj \
$(OBJDIR)\oslibmsg.obj \
$(OBJDIR)\hook.obj \
$(OBJDIR)\win32wmdiclient.obj \
$(OBJDIR)\mdi.obj \
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
$(OBJDIR)\exticon.obj \
$(OBJDIR)\wintrack.obj \
$(OBJDIR)\oldnls32.obj \
$(OBJDIR)\user32rsrc.obj \
$(OBJDIR)\oslibprf.obj \
$(OBJDIR)\dragdrop.obj \
$(OBJDIR)\oslibdnd.obj \
$(OBJDIR)\oslibclipbrd.obj \
$(OBJDIR)\cp_cvt.obj \
!ifdef DEBUG
$(OBJDIR)\dbgwrap.obj \
!endif
$(OBJDIR)\stats.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/wgss50.lib \
$(ODIN32_LIB)/libuls.lib \
$(ODIN32_LIB)/libconv.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/imm32os2.lib \
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

