# $Id: makefile,v 1.2 2002-05-15 12:39:55 sandervl Exp $

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
$(OBJDIR)\devinst.obj \
$(OBJDIR)\dirid.obj \
$(OBJDIR)\infparse.obj \
$(OBJDIR)\install.obj \
$(OBJDIR)\parser.obj \
$(OBJDIR)\queue.obj \
$(OBJDIR)\stubs.obj \
##$(OBJDIR)\virtcopy.obj \
$(OBJDIR)\setupapirsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/ntdll.lib \
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

