# $Id: Makefile,v 1.1 2003-10-06 10:09:05 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc

CDEFINES = $(CDEFINES) -I..\include

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dev2gif.obj \
$(OBJDIR)\dgif_lib.obj \
$(OBJDIR)\egif_lib.obj \
$(OBJDIR)\gif_err.obj \
$(OBJDIR)\gif_font.obj \
$(OBJDIR)\gif_hash.obj \
$(OBJDIR)\gifalloc.obj \
$(OBJDIR)\qprintf.obj \
$(OBJDIR)\quantize.obj 


#
# Target name - name of the library without extention and path.
#
TARGET  = giflib


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

