# $Id: makefile,v 1.23 2004-12-30 18:45:43 sao2l02 Exp $

#
# Odin32 API
#
#       version.dll makefile
#

#
# Target name - name of the dll without extention and path.
#
TARGET = version

#
# Compiler, tools, and interference rules.
#
ODIN32_DBGWRAP = 1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\install.obj \
$(OBJDIR)\info.obj \
$(OBJDIR)\resource.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\versionrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/lz32.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)
