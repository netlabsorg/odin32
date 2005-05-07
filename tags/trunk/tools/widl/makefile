# $Id: makefile,v 1.2 2005-05-07 06:00:54 sao2l02 Exp $

#
# WRC makefile.
#
#   Note. If you wanna recompile everything you'll need flex and bison.
#         Both are found at hobbes.
#         AND you'll have to set env.var. HAVE_BISON and HAVE_FLEX.
#


#
# Directory macro.
#
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Tell buildenvironment that we're making an VIO .exe.
# Tell buildenvironment that we like to use static linked CRT.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1


#
# include common definitions
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
# Object extension.
#
OBJ = obj

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\client.$(OBJ) \
$(OBJDIR)\hash.$(OBJ) \
$(OBJDIR)\header.$(OBJ) \
$(OBJDIR)\proxy.$(OBJ) \
$(OBJDIR)\server.$(OBJ) \
$(OBJDIR)\typelib.$(OBJ) \
$(OBJDIR)\utils.$(OBJ) \
$(OBJDIR)\widl.$(OBJ) \
$(OBJDIR)\y.tab.$(OBJ) \
$(OBJDIR)\lex.yy.$(OBJ) \
$(OBJDIR)\write_msft.$(OBJ) \
!if "$(CCENV)" == "VAC36" || "$(CCENV)" == "VAC3"
$(OBJDIR)\getopt.$(OBJ)
!endif


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/u.lib \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = widl


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Needed tools
#
needed: all


#
# Extra rules.
#
!ifdef HAVE_BISON
y.tab.c y.tab.h: parser.y
    $(YACC) -d -t parser.y -o y.tab.c
!endif

!ifdef HAVE_FLEX
lex.yy.c: parser.l
    $(LEX) -8 -d parser.l
!endif
