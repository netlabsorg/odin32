# $Id: makefile,v 1.12 2000-11-19 08:35:50 bird Exp $

#
# Odin32 API
#
#       wnaspi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\aspilib.obj \
$(OBJDIR)\winaspi32.obj \
$(OBJDIR)\wnaspi32rsrc.obj \
$(OBJDIR)\initterm.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/advapi32.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wnaspi32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

