# $Id: makefile,v 1.6 2001-09-30 08:45:52 bird Exp $

#
# Odin32 API
#
#       riched32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides.
#
!if "$(CUSTOMBUILD)" != "1"
CDEFINES = $(CDEFINES) -DWIN32 -DSTDARG -DTHINK_C
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\reader.obj \
$(OBJDIR)\charlist.obj \
$(OBJDIR)\textwriter.obj \
$(OBJDIR)\richedit.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initriched32.obj \
$(OBJDIR)\riched32rsrc.obj


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
TARGET = riched32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

