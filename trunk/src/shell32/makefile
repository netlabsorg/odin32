# $Id: makefile,v 1.40 2003-02-15 10:20:48 sandervl Exp $

#
# Odin32 API
#
#       shell32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!if "$(DEBUG)" == "1"
DEFFILE    = shell32dbg.def
ORGDEFFILE = shell32.def
!endif

WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides
#
RCFLAGS = $(RCFLAGS) -D__WIN32OS2__


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initshell32.obj \
$(OBJDIR)\os2_integration.obj \
$(OBJDIR)\brsfolder.obj \
$(OBJDIR)\changenotify.obj \
$(OBJDIR)\classes.obj \
$(OBJDIR)\clipboard.obj \
$(OBJDIR)\dataobject.obj \
$(OBJDIR)\dialogs.obj \
$(OBJDIR)\enumidlist.obj \
$(OBJDIR)\folders.obj \
$(OBJDIR)\iconcache.obj \
$(OBJDIR)\memorystream.obj \
$(OBJDIR)\pidl.obj \
$(OBJDIR)\shell32_main.obj \
$(OBJDIR)\shell32_odin.obj \
$(OBJDIR)\shelllink.obj \
$(OBJDIR)\shlmenu.obj \
$(OBJDIR)\shellpath.obj \
$(OBJDIR)\shellole.obj \
$(OBJDIR)\shellreg.obj \
$(OBJDIR)\shellstring.obj \
$(OBJDIR)\shellstub.obj \
$(OBJDIR)\shellord.obj \
$(OBJDIR)\shlfileop.obj \
$(OBJDIR)\shlfolder.obj \
$(OBJDIR)\shlview.obj \
$(OBJDIR)\shell.obj \
$(OBJDIR)\shv_bg_cmenu.obj \
$(OBJDIR)\shv_item_cmenu.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\control.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\she.obj \
$(OBJDIR)\sh.obj \
$(OBJDIR)\shpolicy.obj \
$(OBJDIR)\systray.obj \
!if "$(DEBUG)" == "1"
$(OBJDIR)\dbgwrap.obj \
!endif
$(OBJDIR)\shres.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/version.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/guidlib.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/pmwinx.lib \
$(ODIN32_LIB)/shlwapi.lib \
$(ODIN32_LIB)/ntdll.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = shell32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

