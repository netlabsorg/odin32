# $Id: makefile,v 1.20 2003-08-08 15:55:08 sandervl Exp $

#
# Odin32 API
#
#       winspool.dll makefile
#


#
# Compiler, tools, and interference rules.
#
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\winspool.obj \
$(OBJDIR)\oslibspl.obj \
$(OBJDIR)\winspoolrsrc.obj \
$(OBJDIR)\initwinspool.obj \
$(OBJDIR)\initterm.obj 


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

