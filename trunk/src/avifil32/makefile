# $Id: makefile,v 1.12 2001-07-21 17:38:50 sandervl Exp $

#
# Odin32 API
#
#       avifil32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initavifil32.obj \
$(OBJDIR)\api.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\comentry.obj \
$(OBJDIR)\iafile.obj \
$(OBJDIR)\iastream.obj \
$(OBJDIR)\igframe.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\avifil32.obj 


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/msvfw32.lib \
OS2386.LIB \
$(RTLLIB_O) 


#
# Target name - name of the dll without extention and path.
#
TARGET = avifil32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

