# $Id: makefile,v 1.3 2000-11-19 08:40:22 bird Exp $

#
# Odin32 API
#
#       urlmon.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\urlmon.obj \
$(OBJDIR)\umon.obj \
$(OBJDIR)\urlmonrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/ole32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = urlmon


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

