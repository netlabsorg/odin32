# $Id: makefile,v 1.2 2000-11-19 09:21:41 bird Exp $

#
# Odin32 API
#
#       dciman32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dciman32.obj \
$(OBJDIR)\dcimanrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/gdi32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = dciman32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

