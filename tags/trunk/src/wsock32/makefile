# $Id: makefile,v 1.28 2000-11-19 08:35:49 bird Exp $

#
# Odin32 API
#
#       wsock32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Overrides.
#
CXXFLAGS = $(CXXFLAGS) -DNEW_ASYNC


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wsock32.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\wsa.obj \
$(OBJDIR)\wsastruct.obj \
$(OBJDIR)\wsock32rsrc.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\asyncapi.obj \
$(OBJDIR)\asyncthread.obj \
$(OBJDIR)\dbglocal.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/user32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
so32dll.lib \
tcp32dll.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = wsock32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

