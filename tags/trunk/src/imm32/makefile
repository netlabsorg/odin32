# $Id: makefile,v 1.13 2004-04-27 08:28:45 sandervl Exp $

#
# Odin32 API
#
#       imm32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\imm32.obj \
$(OBJDIR)\imm32rsrc.obj \
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
TARGET = imm32os2


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

