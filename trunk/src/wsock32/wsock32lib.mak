# $Id: wsock32lib.mak,v 1.2 2001-07-29 19:02:14 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#
WRC_PREFIX_RESOURCE=1


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


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
# Target name - name of the library without extention and path.
#
TARGET  = wsock32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

