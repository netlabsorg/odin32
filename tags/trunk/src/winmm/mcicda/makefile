# $Id: makefile,v 1.4 2000-12-02 11:37:05 sandervl Exp $

#
# PD-Win32 API
#
#	mcicda.dll makefile
#

#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\..\tools\bin

#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk

#
# Target name - name of the dll without extention and path.
#
TARGET = mcicdos2

OBJS =  $(OBJDIR)\mcicda.obj \
	$(OBJDIR)\cdrom.obj \
	$(OBJDIR)\mcicdos2rsrc.obj \
	$(PDWIN32_LIB)/dllentry.obj \
	$(OBJDIR)\os2cdrom.obj 

#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)\WINMM.LIB \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)



#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post
