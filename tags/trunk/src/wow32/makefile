# $Id: makefile,v 1.1 2001-05-28 18:13:47 phaller Exp $

#
# Odin32 API
#
#       wow32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wow32.obj \
$(OBJDIR)\wow32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/crtdll.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wow32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

