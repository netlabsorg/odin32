# $Id: makefile,v 1.15 2001-04-24 19:44:13 sandervl Exp $

#
# Odin32 API
#
#       dplayx.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dpclassfactory.obj \
$(OBJDIR)\dplay.obj \
$(OBJDIR)\dplaysp.obj \
$(OBJDIR)\dplayx_global.obj \
$(OBJDIR)\dplayx_main.obj \
$(OBJDIR)\dplayx_messages.obj \
$(OBJDIR)\dplobby.obj \
$(OBJDIR)\name_server.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\dplayxrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/advapi32.lib \
OS2386.LIB \
$(RTLLIB_O) \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/msvfw32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = dplayx


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

