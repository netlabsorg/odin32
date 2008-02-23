# $Id: Makefile,v 1.1 2001-06-13 04:45:32 bird Exp $

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
$(OBJDIR)\dbglocal.obj\
$(OBJDIR)\vdmdbg.obj\
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
TARGET = vdmdbg


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

