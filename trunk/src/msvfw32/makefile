# $Id: makefile,v 1.10 2000-11-19 09:08:05 bird Exp $

#
# Odin32 API
#
#       msvfw32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\msvfw32.obj \
$(OBJDIR)\msvfw32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/gdi32.lib \
$(PDWIN32_LIB)/winmm.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = msvfw32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

