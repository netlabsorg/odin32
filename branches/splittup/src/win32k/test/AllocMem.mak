# $Id: AllocMem.mak,v 1.1 2001-02-11 15:25:51 bird Exp $

#
# Odin32 API
#
#       AllocMem.exe makefile
#


#
# Tell buildenvironment that we're making an exe and to invoke additional dep rule
#
EXETARGET = 1
ADDITIONAL_DEP = mydep
WIN32KINCLUDE = ..\include
VIO = 1


#
# Compiler, tools, and interference rules thru the win32k makefile.inc file.
#
!include ..\..\..\makefile.inc


#
# Tools and Flags Addjustments
#  (The author of this is kinda SM when it comes to warnings... ;-)
#
CINCLUDES  = -I$(WIN32KINCLUDE) $(CINCLUDES)
CDEFINES   = $(CDEFINES) -DRING3


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\AllocMem.obj


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
TARGET = AllocMem


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Dependencies.
#
mydep:
    $(DEPEND) -a -o$$(OBJDIR) $(CINCLUDES) $(WIN32KINCLUDE)\*.h

