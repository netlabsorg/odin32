# $Id: msvcrt.mak,v 1.2 2000-12-02 23:40:15 bird Exp $

#
# Odin32 API
#
#       msvcrt.dll makefile
#


#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = msvcrt.mak


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\msvcrt.obj \
$(OBJDIR)\critsec.obj \
$(OBJDIR)\util.obj \
$(OBJDIR)\$(TARGET)rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/crtdll.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = msvcrt


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

