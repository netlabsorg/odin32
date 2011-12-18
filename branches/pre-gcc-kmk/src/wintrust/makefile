# $Id: makefile,v 1.1 2001-12-13 13:01:56 sandervl Exp $

#
# Odin32 API
#
#       vdmdbg.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(DLLENTRY) \
$(OBJDIR)\wintrust.obj\
$(OBJDIR)\$(TARGET)rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wintrust


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

