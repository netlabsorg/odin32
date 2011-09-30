# $Id: makefile,v 1.36 2004/12/04 10:22:58 sao2l02 Exp $

#
# Odin32 API
#
#       oleacc.dll makefile
#


#
# Compiler, tools, and interference rules.
#
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES = $(CDEFINES) -DWIN32 -DSTDARG -DTHINK_C

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\main.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initoleacc.obj \
$(OBJDIR)\oleaccrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = oleacc


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

