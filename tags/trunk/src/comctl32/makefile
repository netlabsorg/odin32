# $Id: makefile,v 1.31 2000-12-02 23:37:13 bird Exp $

#
# Odin32 API
#
#       comctl32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\comctl32.obj \
$(OBJDIR)\initterm.obj \
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
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/msvfw32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = comctl32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

