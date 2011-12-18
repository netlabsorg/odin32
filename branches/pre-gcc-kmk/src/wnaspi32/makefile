# $Id: makefile,v 1.16 2003-08-08 15:55:20 sandervl Exp $

#
# Odin32 API
#
#       wnaspi32.dll makefile
#

WRC_PREFIX_RESOURCE=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\winaspi32.obj \
$(OBJDIR)\cdio.obj \
$(OBJDIR)\wnaspi32rsrc.obj \
$(OBJDIR)\initwnaspi32.obj \
$(OBJDIR)\initterm.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wnaspi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

