# $Id: makefile,v 1.30 2000-11-19 08:50:16 bird Exp $

#
# Odin32 API
#
#       shell32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
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
$(OBJDIR)\shellguid.obj \
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
$(OBJDIR)\misc.obj \
$(OBJDIR)\she.obj \
$(OBJDIR)\sh.obj \
$(OBJDIR)\shpolicy.obj \
$(OBJDIR)\systray.obj \
$(OBJDIR)\shres.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/advapi32.lib \
$(PDWIN32_LIB)/gdi32.lib \
$(PDWIN32_LIB)/ole32.lib \
$(PDWIN32_LIB)/version.lib \
$(PDWIN32_LIB)/comctl32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/pmwinx.lib \
$(PDWIN32_LIB)/shlwapi.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = shell32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

