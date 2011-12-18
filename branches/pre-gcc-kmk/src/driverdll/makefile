# $Id: makefile,v 1.2 2003-08-08 15:51:47 sandervl Exp $

#
#       testdrv.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\testdrv.obj \
$(OBJDIR)\testdrvrsrc.obj \
$(DLLENTRY)


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
TARGET = testdrv


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

