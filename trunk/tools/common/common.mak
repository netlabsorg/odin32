# $Id: common.mak,v 1.7 2003-01-23 13:47:50 bird Exp $

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
STATIC_CRT = 1

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
$(OBJDIR)\kFile.obj \
$(OBJDIR)\kFileInterfaces.obj \
$(OBJDIR)\kAssert.obj \
$(OBJDIR)\kError.obj \


#
# Target name - name of the library without extention and path.
#
TARGET  = common


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

