# $Id: shell32lib.mak,v 1.2 2001-07-29 19:01:06 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#
WRC_PREFIX_RESOURCE=1


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides
#
RCFLAGS = $(RCFLAGS) -D__WIN32OS2__


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initshell32.obj \
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
##$(OBJDIR)\shellguid.obj \
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
$(OBJDIR)\shres.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = shell32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

