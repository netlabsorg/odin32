# $Id: MemInfoTest.mak,v 1.2 2001-02-12 21:09:18 bird Exp $

#
# Odin32 API
#
#       MemInfoTest.exe makefile
#


#
# Tell buildenvironment that we're making an exe and to invoke additional dep rule
#
EXETARGET = 1
VIO = 1


#
# Compiler, tools, and interference rules thru the win32k makefile.inc file.
#
!include ..\..\..\makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\MemInfoTest.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
$(ODIN32_LIB)\win32k.lib \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = MemInfoTest


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

