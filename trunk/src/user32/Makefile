# $Id: Makefile,v 1.68 2000-03-06 23:39:12 bird Exp $

#
# Odin32 API
#
#       user32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = user32rsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
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
$(OBJDIR)\winproc.obj \
$(OBJDIR)\text.obj \
$(OBJDIR)\oslibmsgtranslate.obj \
$(OBJDIR)\windlgmsg.obj \
$(OBJDIR)\windlg.obj \
$(OBJDIR)\win32wdesktop.obj \
$(OBJDIR)\win32wbasenonclient.obj \
$(OBJDIR)\user32rsrc.obj


# Target name - name of the dll without extention and path.
TARGET = user32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
        @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
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


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

