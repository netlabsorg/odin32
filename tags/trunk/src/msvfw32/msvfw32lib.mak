# $Id: msvfw32lib.mak,v 1.1 2001-07-20 15:36:29 sandervl Exp $

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


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES    = $(CDEFINES) -DINVERT


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\msvfw32.obj \
$(OBJDIR)\msvfw32rsrc.obj \

#
# Target name - name of the library without extention and path.
#
TARGET  = msvfw32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

