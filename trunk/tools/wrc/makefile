# $Id: makefile,v 1.4 2001-06-25 13:54:43 bird Exp $


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

# Subdirectories.
SUBDIRS = \
.\u


#
# include common definitions
#
CCENV=EMX
!include ../../makefile.inc

#
# Addjust common definitions
#
#DEFS      = -D__WINE__ -D__WIN32OS2__ -idirafter ../../include/win -idirafter ../../include
CDEFINES   = -D__EMX__ -DMASM -D__SEMICOLON__

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
$(OBJDIR)\dumpres.$(OBJ) \
$(OBJDIR)\genres.$(OBJ) \
$(OBJDIR)\newstruc.$(OBJ) \
$(OBJDIR)\preproc.$(OBJ) \
$(OBJDIR)\readres.$(OBJ) \
$(OBJDIR)\utils.$(OBJ) \
$(OBJDIR)\wrc.$(OBJ) \
$(OBJDIR)\y.tab.$(OBJ) \
$(OBJDIR)\lexyy.$(OBJ) \
$(OBJDIR)\ppy.tab.$(OBJ) \
$(OBJDIR)\lex.ppl.$(OBJ) \
$(OBJDIR)\writeres.$(OBJ) \


#
# Libraries. One space before the '\'.
#
LIBS = \
u\$(OBJDIR)\u.lib\
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = wrc


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Extra rules.
#
y.tab.c y.tab.h: parser.y
    $(YACC) -d -t parser.y -o y.tab.c

lexyy.c: parser.l
    $(LEX) -8 -d parser.l

ppy.tab.c ppy.tab.h: ppy.y
    $(YACC) $(YACCOPT) -bppy -ppp -d -t ppy.y -o ppy.tab.c

lex.ppl.c: ppl.l
    $(LEX) $(LEXOPT) -d -Ppp -8 -olex.ppl.c ppl.l

