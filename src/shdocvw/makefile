# $Id: makefile,v 1.1 2001-01-14 17:14:16 sandervl Exp $

#
# Odin32 API
#
#       shdocvw.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\classinfo.obj \
$(OBJDIR)\events.obj \
$(OBJDIR)\factory.obj \
$(OBJDIR)\oleobject.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\persist.obj \
$(OBJDIR)\shdocvw_main.obj \
$(OBJDIR)\shdocvwrsrc.obj \
$(OBJDIR)\webbrowser.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = shdocvw


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

