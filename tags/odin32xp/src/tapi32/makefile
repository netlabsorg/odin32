# $Id: makefile,v 1.17 2000-12-02 23:57:02 bird Exp $

#
# Odin32 API
#
#       tapi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\tapi32.obj \
$(OBJDIR)\tapi32rsrc.obj \
$(DLLENTRY)


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
TARGET = tapi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

