# $Id: makefile,v 1.16 2000-12-02 23:44:57 bird Exp $

#
# Odin32 API
#
#       wnetap32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wnetap32.obj \
$(OBJDIR)\oslibnet.obj \
$(OBJDIR)\netapi32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
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
!include $(ODIN32_POST_INC)

