# $Id: makefile,v 1.1 2001-01-20 18:06:12 sandervl Exp $

#
# Odin32 API
#
#       setupapi.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\setupapirsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = setupapi


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

