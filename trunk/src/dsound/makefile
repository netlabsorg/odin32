# $Id: makefile,v 1.20 2000-11-19 09:16:55 bird Exp $

#
# Odin32 API
#
#       dsound.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dsound.obj \
$(OBJDIR)\os2dsound.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\os2sndbuffer.obj \
$(OBJDIR)\os2primbuff.obj \
$(OBJDIR)\os2notify.obj \
$(OBJDIR)\os23dlistener.obj \
$(OBJDIR)\os23dbuffer.obj \
$(OBJDIR)\dart.obj \
$(OBJDIR)\dsmixer.obj \
$(OBJDIR)\dsoundrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/ole32.lib \
pmwinx.lib \
mmpm2.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = dsound


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

