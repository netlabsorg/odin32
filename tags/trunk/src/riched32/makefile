# $Id: makefile,v 1.3 2000-11-19 08:51:27 bird Exp $

#
# Odin32 API
#
#       riched32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Overrides.
#
CDEFINES = $(CDEFINES) -DWIN32 -DSTDARG -DTHINK_C


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\reader.obj \
$(OBJDIR)\charlist.obj \
$(OBJDIR)\text-writer.obj \
$(OBJDIR)\richedit.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\riched32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = riched32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

