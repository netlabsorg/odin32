# $Id: makefile,v 1.7 2000-11-19 08:47:57 bird Exp $

#
# Odin32 API
#
#       twain_32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Overrides.
#
CDEFINES = $(CDEFINES) -DWIN32


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\twain_32.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\twain_32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = twain_32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

