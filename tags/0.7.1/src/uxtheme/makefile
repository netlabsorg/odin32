# $Id: makefile,v 1.1 2002-04-10 18:35:18 bird Exp $

#
# Odin32 API
#
#       UxTheme.dll makefile
#


#
# Compiler, tools, and interference rules.
#
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initUxTheme.obj \
$(OBJDIR)\uxtheme.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\uxthemersrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/wgss50.lib \
$(ODIN32_LIB)/guidlib.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = uxtheme


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

