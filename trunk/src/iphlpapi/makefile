# $Id: makefile,v 1.2 2001-10-10 16:22:20 phaller Exp $

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
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = iphlpapi


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

