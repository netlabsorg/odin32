# $Id: makefile,v 1.14 2001-09-28 07:50:10 sandervl Exp $

#
# Odin32 API
#
#       wininet.dll makefile
#
WRC_PREFIX_RESOURCE=1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wininet.obj \
$(OBJDIR)\wininet_main.obj \
$(OBJDIR)\ftp.obj \
$(OBJDIR)\http.obj \
$(OBJDIR)\utility.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initwininet.obj \
$(OBJDIR)\internet.obj \
$(OBJDIR)\wininetrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/wsock32.lib \
$(ODIN32_LIB)/icmp.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wininet


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

