# $Id: wnaspi32lib.mak,v 1.1 2001-10-15 17:06:40 sandervl Exp $

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
CDEFINES    = $(CDEFINES) -DINVERT


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\aspilib.obj \
$(OBJDIR)\winaspi32.obj \
$(OBJDIR)\initwnaspi32.obj \
$(OBJDIR)\wnaspi32rsrc.obj 

#
# Target name - name of the library without extention and path.
#
TARGET  = wnaspi32o


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

