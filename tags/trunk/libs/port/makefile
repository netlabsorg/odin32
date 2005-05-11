# $Id: makefile,v 1.3 2005-05-11 21:32:26 sao2l02 Exp $

#
# Odin32 API
#
#       lib_port.lib makefile
#

#
# Target name - name of the library without extention and path.
#
TARGET  = lib_port

#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Addjust common definitions
#
!if "$(CCENV)" == "EMX"
CDEFINES   = $(CDEFINES) -D__EMX__ -DMASM -D__SEMICOLON__
!else
CDEFINES   = $(CDEFINES) -DMASM -D__SEMICOLON__
#CDEFINES   = $(CDEFINES) -DMASM -D__SEMICOLON__ -DYY_USE_PROTOS
!endif
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
!ifdef DEBUG
CFLAGS     = $(CFLAGS) -Tm-
#CFLAGS     = $(CFLAGS) -O+ -Tm-
!endif
!endif


YACC    = bison
YACCOPT = #-v
LEX     = flex


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\mkstemps.obj \
$(OBJDIR)\strncasecmp.obj \
$(OBJDIR)\strcasecmp.obj

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

#
# Needed tools
#
needed: all
