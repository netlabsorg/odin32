# $Id: makefile,v 1.2 2001-05-19 09:06:42 sandervl Exp $

#
# Odin32 API
#
#       wsock32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CXXFLAGS = $(CXXFLAGS)


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\socket.obj \
$(OBJDIR)\ws2_32rsrc.obj \
$(OBJDIR)\dbglocal.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/wsock32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
so32dll.lib \
tcp32dll.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = ws2_32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

