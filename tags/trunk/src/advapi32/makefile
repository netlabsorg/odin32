# $Id: makefile,v 1.25 2000-09-28 03:17:10 bird Exp $

#
# Odin32 API
#
#       advapi32.dll makefile
#


# For the time being
NEW_STYLE = 1


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
