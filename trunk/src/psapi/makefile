# $Id: makefile,v 1.10 2000-11-19 08:53:32 bird Exp $

#
# Odin32 API
#
#       psapi.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\psapi.obj \
$(OBJDIR)\psapirsrc.obj \
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
TARGET = psapi


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

