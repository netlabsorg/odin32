# $Id: makefile,v 1.6 2000-12-02 23:39:37 bird Exp $

#
# icmp.dll makefile
#
#       Project Odin Software License can be found in LICENSE.TXT
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


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
$(ODIN32_LIB)/wsock32.lib \
$(RTLLIB_O) \
$(ODIN32_LIB)/$(ODINCRT).lib \
$(ODIN32_LIB)/kernel32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = icmp


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

