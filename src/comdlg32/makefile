# $Id: makefile,v 1.21 2000-11-19 09:23:00 bird Exp $

#
# Odin32 API
#
#       comdlg32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


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
$(OBJDIR)\filedlg95.obj \
$(OBJDIR)\comdlg32.obj \
$(OBJDIR)\filedlgbrowser.obj \
$(OBJDIR)\comdlgguid.obj \
$(OBJDIR)\rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/shlwapi.lib \
$(PDWIN32_LIB)/shell32.lib \
$(PDWIN32_LIB)/comctl32.lib \
$(PDWIN32_LIB)/gdi32.lib \
$(PDWIN32_LIB)/pmwinx.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = comdlg32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

