# $Id: makefile,v 1.14 2000-12-02 23:39:57 bird Exp $

#
# Odin32 API
#
#       lz32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\lz32.obj \
$(OBJDIR)\lz32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/kernel32.lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = lz32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

