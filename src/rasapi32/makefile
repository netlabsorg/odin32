# $Id: makefile,v 1.11 2000-11-19 08:51:45 bird Exp $

#
# Odin32 API
#
#       rasapi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\rasapi32.obj \
$(OBJDIR)\rasapi32rsrc.obj \
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
TARGET = rasapi32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

