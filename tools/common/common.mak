# $Id: common.mak,v 1.5 2002-02-23 23:27:18 bird Exp $

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
# We use exception, so we'll have to enable them.
#
!ifndef WMAKE
CXXFLAGS = $(CXXFLAGS) $(CXXEXCEPTIONS)
!else
CXXFLAGS += $(CXXEXCEPTIONS)
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\kFileDEF.obj \
$(OBJDIR)\kFileLX.obj \
$(OBJDIR)\kFilePE.obj \
$(OBJDIR)\kFileSDF.obj \
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

