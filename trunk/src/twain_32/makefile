# $Id: makefile,v 1.8 2000-12-02 23:56:55 bird Exp $

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

