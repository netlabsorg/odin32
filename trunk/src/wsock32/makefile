# $Id: makefile,v 1.32 2001-07-29 19:01:56 sandervl Exp $

#
# Odin32 API
#
#       wsock32.dll makefile
#
WRC_PREFIX_RESOURCE=1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CXXFLAGS = $(CXXFLAGS) -DNEW_ASYNC


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wsock32.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\wsa.obj \
$(OBJDIR)\wsastruct.obj \
$(OBJDIR)\wsock32rsrc.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initwsock32.obj \
$(OBJDIR)\asyncapi.obj \
$(OBJDIR)\asyncthread.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\wsock2.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
so32dll.lib \
tcp32dll.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wsock32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

