# $Id: makefile,v 1.13 2002-06-15 12:14:26 sandervl Exp $

#
# Odin32 API
#
#       avifil32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\avifile.obj \
$(OBJDIR)\avifil32.obj \
$(DLLENTRY) 


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/msvfw32.lib \
$(ODIN32_LIB)/guidlib.lib \
OS2386.LIB \
$(RTLLIB_O) 


#
# Target name - name of the dll without extention and path.
#
TARGET = avifil32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

