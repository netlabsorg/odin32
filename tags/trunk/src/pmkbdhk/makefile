# $Id: makefile,v 1.1 2001-10-22 23:13:39 phaller Exp $

#
# InnoTek OS/2 Presentation Manager Keyboard Hook
#
#       pmkbdhk.dll makefile
#


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
$(OBJDIR)\pmkbdhk.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = pmkbdhk


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

