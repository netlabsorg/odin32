# $Id: riched32lib.mak,v 1.1 2001-08-25 13:37:44 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1
WRC_PREFIX_RESOURCE=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\reader.obj \
$(OBJDIR)\charlist.obj \
$(OBJDIR)\textwriter.obj \
$(OBJDIR)\richedit.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initriched32.obj \
$(OBJDIR)\riched32rsrc.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = riched32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

