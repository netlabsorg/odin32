# $Id: makefile,v 1.14 2000-11-19 08:35:50 bird Exp $

#
# Odin32 API
#
#       wnetap32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wnetap32.obj \
$(OBJDIR)\oslibnet.obj \
$(OBJDIR)\netapi32rsrc.obj \
$(PDWIN32_LIB)/dllentry.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target and original target names - names of the dll without extention and path
#
TARGET      = wnetap32
ORGTARGET   = netapi32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

