# $Id: makefile,v 1.5 2000-11-19 09:15:25 bird Exp $
#
# icmp.dll makefile
#
# Project Odin Software License can be found in LICENSE.TXT
#
#


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\icmp.obj \
$(OBJDIR)\icmprsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
os2386.lib \
$(PDWIN32_LIB)/wsock32.lib \
$(RTLLIB_O) \
$(PDWIN32_LIB)/$(ODINCRT).lib \
$(PDWIN32_LIB)/kernel32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = icmp


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

