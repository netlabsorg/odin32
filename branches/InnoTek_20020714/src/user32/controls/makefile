# $Id: makefile,v 1.1 2002-06-02 10:09:30 sandervl Exp $

#
# Odin32 API
#
#       odinctrl.dll makefile
#


!include ../../../makefile.inc



#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\controls.obj \
$(OBJDIR)\edit.obj \
$(OBJDIR)\static.obj \
$(OBJDIR)\button.obj \
$(OBJDIR)\mdi.obj \
$(OBJDIR)\listbox.obj \
$(OBJDIR)\combo.obj \
$(OBJDIR)\icontitle.obj \
$(OBJDIR)\scroll.obj \
$(OBJDIR)\winswitch.obj \
$(OBJDIR)\menu.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\dialog.obj \
$(OBJDIR)\defdlg.obj \
$(OBJDIR)\desktop.obj \
!ifdef DEBUG
$(OBJDIR)\dbgwrap.obj \
$(OBJDIR)\dbglocal.obj 
!endif


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/wgss50.lib \
$(ODIN32_LIB)/libuls.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = odinctrl


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

