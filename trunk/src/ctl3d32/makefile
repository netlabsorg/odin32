# $Id: makefile,v 1.1 2001-09-21 18:38:12 sandervl Exp $

#
# Odin32 API
#
#       dplay.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ctl3d32.obj \
$(OBJDIR)\ctl3d32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O) \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/advapi32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = ctl3d32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

