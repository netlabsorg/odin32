# $Id: makefile,v 1.14 2001-10-15 17:06:19 sandervl Exp $

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
$(OBJDIR)\aspilib.obj \
$(OBJDIR)\winaspi32.obj \
$(OBJDIR)\wnaspi32rsrc.obj \
$(OBJDIR)\initwnaspi32.obj \
$(OBJDIR)\initterm.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/advapi32.lib \
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

