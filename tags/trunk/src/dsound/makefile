# $Id: makefile,v 1.23 2001-04-27 17:39:49 sandervl Exp $

#
# Odin32 API
#
#       dsound.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


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
$(OBJDIR)\daudiobuffer.obj \
$(OBJDIR)\daudionotify.obj \
$(OBJDIR)\os23dlistener.obj \
$(OBJDIR)\os23dbuffer.obj \
$(OBJDIR)\dart.obj \
$(OBJDIR)\dsmixer.obj \
$(OBJDIR)\waveoutdaud.obj \
$(OBJDIR)\dsoundrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/ole32.lib \
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
!include $(ODIN32_POST_INC)

