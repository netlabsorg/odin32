# $Id: makefile,v 1.16 2000-11-19 08:48:32 bird Exp $

#
# Odin32 API
#
#       tapi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\tapi32.obj \
$(OBJDIR)\tapi32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = tapi32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

