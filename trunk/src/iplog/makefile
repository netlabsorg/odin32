# $Id: makefile,v 1.2 2003-01-23 13:08:11 sandervl Exp $

#
# Odin32 API
#
#       pe.exe makefile
#

#
# Tell that we're producing an executable
#
STACKSIZE = 0x100000
EXETARGET = 1
VIO = 1
STATIC_CRT=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc



#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\iplog.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
so32dll.lib \
tcp32dll.lib \
$(RTLLIB) \
os2386.lib


#
# Target name - name of the dll without extention and path.
#
TARGET  = iplog


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

