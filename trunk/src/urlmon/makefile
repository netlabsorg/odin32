# $Id: makefile,v 1.4 2000-12-02 23:56:48 bird Exp $

#
# Odin32 API
#
#       urlmon.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


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
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = urlmon


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

