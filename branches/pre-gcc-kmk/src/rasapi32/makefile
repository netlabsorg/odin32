# $Id: makefile,v 1.13 2001-02-11 10:35:15 sandervl Exp $

#
# Odin32 API
#
#       rasapi32.dll makefile
#

# lxlite crashes on this binary
NO_LXLITE=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\rasapi32.obj \
$(OBJDIR)\rasapi32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = rasapi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

