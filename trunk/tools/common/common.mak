# $Id: common.mak,v 1.2 2000-12-02 23:26:57 bird Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kFilePe.obj \
$(OBJDIR)\kFileDef.obj \
$(OBJDIR)\kFileLx.obj \
$(OBJDIR)\kFileFormatBase.obj \
$(OBJDIR)\kFile.obj


#
# Target name - name of the library without extention and path.
#
TARGET  = common


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

