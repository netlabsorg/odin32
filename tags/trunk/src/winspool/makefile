# $Id: makefile,v 1.24 2003-08-14 10:43:26 sandervl Exp $

#
# Odin32 API
#
#       winspool.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\winspool.obj \
$(OBJDIR)\oslibspl.obj \
$(OBJDIR)\winspoolrsrc.obj  \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/pmwinx.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/libwrap.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = winspool


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

