# $Id: makefile,v 1.4 2000-11-19 08:50:47 bird Exp $

#
# Odin32 API
#
#       rpcrt4.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\binding.obj \
$(OBJDIR)\initterm.obj \
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
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
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
!include $(PDWIN32_INCLUDE)/pdwin32.post

