# $Id: Zombie.mak,v 1.1 2001-02-21 22:29:29 bird Exp $

#
# Odin32 API
#
#       Zombie.exe makefile
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
$(OBJDIR)\Zombie.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = Zombie


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

