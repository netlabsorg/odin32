# $Id: makefile,v 1.4 2001-07-30 22:54:19 bird Exp $

#
# Odin32 API
#
#       gdi32.dll makefile
#
NOINTERNALFUNCTIONS=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc

#
# Overrides.
#
#CXXFLAGS    = $(CXXFLAGS_ODINCRT)
#LD2FLAGS    = $(LD2FLAGS_ODINCRT)
#CDEFINES    = $(CDEFINES_ODINCRT)
#IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\math64.obj \
$(OBJDIR)\large_int.obj

#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32o.lib \
$(ODIN32_LIB)/user32o.lib \
$(ODIN32_LIB)/gdi32o.lib \
$(ODIN32_LIB)/comctl32o.lib \
$(ODIN32_LIB)/comdlg32o.lib \
$(ODIN32_LIB)/shell32o.lib \
$(ODIN32_LIB)/shlwapio.lib \
$(ODIN32_LIB)/msvfw32o.lib \
$(ODIN32_LIB)/wsock32o.lib \
$(ODIN32_LIB)/winmmo.lib \
$(ODIN32_LIB)/ole32os2o.lib \
$(ODIN32_LIB)/rpcrt4o.lib \
$(ODIN32_LIB)/versiono.lib \
$(ODIN32_LIB)/lz32o.lib \
$(ODIN32_LIB)/unicode.lib \
$(ODIN32_LIB)/win32k.lib \
$(ODIN32_LIB)/libuls.lib \
$(ODIN32_LIB)/libconv.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/wgss50.lib \
OS2386.LIB \
mmpm2.lib \
so32dll.lib \
tcp32dll.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = xxodin32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


