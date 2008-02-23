# $Id: makefile,v 1.10 2004-12-04 10:22:41 sao2l02 Exp $

#
# Odin32 API
#
#       twain_32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES = $(CDEFINES) -DWIN32


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\twain_32.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\twain_32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = twain_32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

