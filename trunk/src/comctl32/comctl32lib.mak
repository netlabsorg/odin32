# $Id: comctl32lib.mak,v 1.2 2001-07-29 19:00:00 sandervl Exp $

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
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\comctl32.obj \
$(OBJDIR)\initcomctl32.obj \
$(OBJDIR)\progress.obj \
$(OBJDIR)\comctl32undoc.obj \
$(OBJDIR)\imagelist.obj \
$(OBJDIR)\comboex.obj \
$(OBJDIR)\animate.obj \
$(OBJDIR)\datetime.obj \
$(OBJDIR)\flatsb.obj \
$(OBJDIR)\hotkey.obj \
$(OBJDIR)\monthcal.obj \
$(OBJDIR)\nativefont.obj \
$(OBJDIR)\pager.obj \
$(OBJDIR)\tab.obj \
$(OBJDIR)\status.obj \
$(OBJDIR)\header.obj \
$(OBJDIR)\updown.obj \
$(OBJDIR)\rebar.obj \
$(OBJDIR)\trackbar.obj \
$(OBJDIR)\tooltips.obj \
$(OBJDIR)\toolbar.obj \
$(OBJDIR)\treeview.obj \
$(OBJDIR)\propsheet.obj \
$(OBJDIR)\listview.obj \
$(OBJDIR)\draglist.obj \
$(OBJDIR)\ipaddress.obj \
$(OBJDIR)\ccbase.obj \
$(OBJDIR)\rsrc.obj \
$(OBJDIR)\smoothscroll.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = comctl32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

