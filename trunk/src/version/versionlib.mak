# $Id: versionlib.mak,v 1.1 2001-07-20 15:42:12 sandervl Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell build environmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


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

#
# Target name - name of the library without extention and path.
#
TARGET  = versiono


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

