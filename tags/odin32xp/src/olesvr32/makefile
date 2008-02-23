# $Id: makefile,v 1.13 2000-12-02 23:41:17 bird Exp $

#
# Odin32 API
#
#       olesvr32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\olesvr32.obj \
$(OBJDIR)\olesvr32rsrc.obj \
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
TARGET = olesvr32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

