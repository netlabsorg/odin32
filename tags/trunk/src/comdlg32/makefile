# $Id: makefile,v 1.23 2001-01-11 18:05:02 sandervl Exp $

#
# Odin32 API
#
#       comdlg32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\colordlg.obj \
$(OBJDIR)\fontdlg.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\cdlg32.obj \
$(OBJDIR)\filetitle.obj \
$(OBJDIR)\finddlg32.obj \
$(OBJDIR)\filedlg.obj \
$(OBJDIR)\filedlg95.obj \
$(OBJDIR)\comdlg32.obj \
$(OBJDIR)\filedlgbrowser.obj \
$(OBJDIR)\comdlgguid.obj \
$(OBJDIR)\rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/shlwapi.lib \
$(ODIN32_LIB)/shell32.lib \
$(ODIN32_LIB)/comctl32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/pmwinx.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = comdlg32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

