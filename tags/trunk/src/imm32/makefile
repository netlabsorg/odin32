# $Id: makefile,v 1.14 2004-05-24 08:51:00 sandervl Exp $

#
# Odin32 API
#
#       imm32.dll makefile
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
$(OBJDIR)\im32.obj \
$(OBJDIR)\imm32.obj \
$(OBJDIR)\imm32rsrc.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initimm32.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/libwrap.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = imm32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

