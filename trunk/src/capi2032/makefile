# $Id: makefile,v 1.14 2000-11-19 09:23:29 bird Exp $

#
# Odin32 API
#
#       capi2032.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\capi2032.obj \
$(OBJDIR)\capi2032rsrc.obj \
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
TARGET = capi2032


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

