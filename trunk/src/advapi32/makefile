# $Id: makefile,v 1.26 2000-11-19 09:27:40 bird Exp $

#
# Odin32 API
#
#       advapi32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\advapi32.obj \
$(OBJDIR)\advapi32rsrc.obj \
$(OBJDIR)\lsa.obj \
$(OBJDIR)\security.obj \
$(OBJDIR)\service.obj \
$(OBJDIR)\eventlog.obj \
$(OBJDIR)\crypt.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/ntdll.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = advapi32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

