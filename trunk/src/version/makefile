# $Id: makefile,v 1.17 2000-12-02 23:56:35 bird Exp $

#
# Odin32 API
#
#       version.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\version.obj \
$(OBJDIR)\install.obj \
$(OBJDIR)\info.obj \
$(OBJDIR)\versionrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/crtdll.lib \
$(ODIN32_LIB)/lz32.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = version


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

