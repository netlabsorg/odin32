# $Id: makefile,v 1.7 2001-07-29 19:01:28 sandervl Exp $

#
# Odin32 API
#
#       rpcrt4.dll makefile
#
WRC_PREFIX_RESOURCE=1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\binding.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initrpcrt4.obj \
$(OBJDIR)\mgmt.obj \
$(OBJDIR)\rpcrt4rsrc.obj \
$(OBJDIR)\server.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\stubs.obj\
$(OBJDIR)\util.obj \
$(OBJDIR)\uuid.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
so32dll.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = rpcrt4


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

