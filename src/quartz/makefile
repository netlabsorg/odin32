# $Id: makefile,v 1.6 2002-06-15 11:54:31 sandervl Exp $

#
# Odin32 API
#
#       quartz.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
!ifndef WMAKE
CDEFINES    = $(CDEFINES) -DICOM_CINTERFACE -DCINTERFACE
!else
CDEFINES   += -DICOM_CINTERFACE -DCINTERFACE
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\main.obj \
$(OBJDIR)\quartzrsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/winmm.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/oleaut32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = quartz

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

