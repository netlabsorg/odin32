# $Id: rpcrt4lib.mak,v 1.2 2001-07-29 19:01:28 sandervl Exp $

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


OBJS = \
$(OBJDIR)\binding.obj \
$(OBJDIR)\initrpcrt4.obj \
$(OBJDIR)\mgmt.obj \
$(OBJDIR)\rpcrt4rsrc.obj \
$(OBJDIR)\server.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\stubs.obj\
##$(OBJDIR)\util.obj \
$(OBJDIR)\uuid.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = rpcrt4o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

