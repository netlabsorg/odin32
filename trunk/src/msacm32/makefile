# $Id: makefile,v 1.12 2000-11-19 09:12:04 bird Exp $

#
# Odin32 API
#
#       msacm32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\msacm32.obj \
$(OBJDIR)\filter.obj \
$(OBJDIR)\format.obj \
$(OBJDIR)\internal.obj \
$(OBJDIR)\stream.obj \
$(OBJDIR)\driver.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\msacm32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/winmm.lib \
$(PDWIN32_LIB)/advapi32.lib \
$(PDWIN32_LIB)/pmwinx.lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = msacm32


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

