# $Id: makefile,v 1.9 2000-11-19 09:17:41 bird Exp $

#
# Odin32 API
#
#       dplay.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dplay.obj \
$(OBJDIR)\dplayrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O) \
$(PDWIN32_LIB)/ole32.lib \
$(PDWIN32_LIB)/advapi32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = dplay


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

