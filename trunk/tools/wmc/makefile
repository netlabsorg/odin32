# $Id: makefile,v 1.1 2003-07-28 11:23:46 sandervl Exp $

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
OBJS=\
$(OBJDIR)\lang.$(OBJ) \
$(OBJDIR)\mcl.$(OBJ) \
$(OBJDIR)\utils.$(OBJ) \
$(OBJDIR)\wmc.$(OBJ) \
$(OBJDIR)\write.$(OBJ) \
$(OBJDIR)\y.tab.$(OBJ) \
$(OBJDIR)\getopt.$(OBJ)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/unicode.lib \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = wmc


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
y.tab.c y.tab.h: mcy.y
    $(YACC) -d -t mcy.y -o y.tab.c

ppy.tab.c ppy.tab.h: ppy.y
    $(YACC) $(YACCOPT) -bppy -ppp -d -t ppy.y -o ppy.tab.c
!endif

!ifdef HAVE_FLEX
lexyy.c: parser.l
    $(LEX) -8 -d parser.l

lex.ppl.c: ppl.l
    $(LEX) $(LEXOPT) -d -Ppp -8 -olex.ppl.c ppl.l
!endif
