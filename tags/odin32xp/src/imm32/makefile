# $Id: makefile,v 1.16 2004-12-04 10:22:22 sao2l02 Exp $

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
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = imm32os2


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

