# $Id: makefile,v 1.3 2002-02-20 11:57:31 sandervl Exp $

#
# Odin32 API
#
#       iphlpapi.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\iphlpapi.obj \
$(OBJDIR)\oslibioctl.obj \
$(OBJDIR)\iphlpapirsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
so32dll.lib \
tcp32dll.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = iphlpapi


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

