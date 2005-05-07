# $Id: makefile,v 1.1 2005-05-07 08:25:34 sao2l02 Exp $

#
# Odin32 API
#
#       libwpp.lib makefile
#

#
# Target name - name of the library without extention and path.
#
TARGET  = libwpp

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
CDEFINES   = $(CDEFINES) -DMASM -D__SEMICOLON__ -DYY_USE_PROTOS
!endif
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
!ifdef DEBUG
CFLAGS     = $(CFLAGS) -O+ -Tm-
!endif
!endif


YACC    = bison
YACCOPT = #-v
LEX     = flex


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\preproc.obj \
$(OBJDIR)\wpp.obj \
$(OBJDIR)\y.tab.obj \
$(OBJDIR)\lex.yy.obj

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

#
# Needed tools
#
#needed: all


#
# Extra rules.
#
!ifdef HAVE_BISON
y.tab.c y.tab.h: ppy.y
    $(YACC) -d -t ppy.y -o y.tab.c
!endif

!ifdef HAVE_FLEX
lex.yy.c: ppl.l
    $(LEX) -8 -d ppl.l
!endif

