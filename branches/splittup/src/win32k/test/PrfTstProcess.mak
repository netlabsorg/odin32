# $Id: PrfTstProcess.mak,v 1.1.4.1 2002-03-31 20:09:22 bird Exp $

#
# Odin32 API
#
#       PrfTstProcess.exe makefile
#


#
# Tell buildenvironment that we're making an exe and to invoke additional dep rule
#
EXETARGET = 1
VIO = 1
MAKEFILE = $(TARGET).mak


#
# Compiler, tools, and interference rules thru the win32k makefile.inc file.
#
!include ..\..\..\makefile.inc

LD2TARGETFLAGS = $(LD2TARGETFLAGS) /pmtype:vio

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\PrfTstProcess.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
$(ODIN32_LIB)\win32k.lib \
tcp32dll.lib \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = PrfTstProcess


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

