# $Id: makefile,v 1.12 2000-11-19 09:00:28 bird Exp $

#
# Odin32 API
#
#       olesvr32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\olesvr32.obj \
$(OBJDIR)\olesvr32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = olesvr32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

