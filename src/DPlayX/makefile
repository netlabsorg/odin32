# $Id: makefile,v 1.12 2000-11-19 09:18:20 bird Exp $

#
# Odin32 API
#
#       dplayx.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


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
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/advapi32.lib \
OS2386.LIB \
$(RTLLIB_O) \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/ole32.lib \
$(PDWIN32_LIB)/winmm.lib \
$(PDWIN32_LIB)/ntdll.lib \
$(PDWIN32_LIB)/msvfw32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = dplayx


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

