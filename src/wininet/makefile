# $Id: makefile,v 1.10 2000-11-19 08:35:50 bird Exp $

#
# Odin32 API
#
#       wininet.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wininet.obj \
$(OBJDIR)\ftp.obj \
$(OBJDIR)\utility.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\internet.obj \
$(OBJDIR)\wininetrsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/wsock32.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wininet


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

