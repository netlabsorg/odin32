# $Id: makefile,v 1.22 2003-10-29 19:18:14 sandervl Exp $

#
# Odin32 API
#
#       version.dll makefile
#

!if "$(DEBUG)" == "1"
DEFFILE    = versiondbg.def
ORGDEFFILE = version.def
!endif


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\install.obj \
$(OBJDIR)\info.obj \
$(OBJDIR)\resource.obj \
$(OBJDIR)\dbglocal.obj \
!ifdef DEBUG
$(OBJDIR)\dbgwrap.obj \
!endif
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
# Target name - name of the dll without extention and path.
#
TARGET = version


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

